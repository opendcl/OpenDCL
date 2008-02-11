// ArgumentsRetrieval.cpp : implementation file
//

#include "stdafx.h"
#include "ArgumentsRetrieval.h"
#include "Resource.h"
#include "ErrorLexicon.h"
#include "AcadColorTable.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ArxWorkspace.h"
#include "DclFormObject.h"
#include "ControlPane.h"
#include "VarUtils.h"


extern CString GetCurrentFunctionName();
static void HandleArgError( /*in*/ const resbuf* pArgs, /*in*/ odcl::ArgErr stat,
														/*in*/ LPCTSTR pszAdditional = NULL, /*in*/ bool bPreviousArg = false );

static CString GetErrorName( odcl::ArgErr stat )
{
	switch( stat )
	{
	case odcl::argNil: return theWorkspace.LoadResourceString( IDS_ARG_NIL );
	case odcl::argNotEnough: return theWorkspace.LoadResourceString( IDS_ARG_NOTENOUGH );
	case odcl::argWrongType: return theWorkspace.LoadResourceString( IDS_ARG_WRONGTYPE );
	case odcl::argTooMany: return theWorkspace.LoadResourceString( IDS_ARG_TOOMANY );
	case odcl::argInvalid: return theWorkspace.LoadResourceString( IDS_ARG_INVALID );
	case odcl::argNoInstance: return theWorkspace.LoadResourceString( IDS_ARG_NOINSTANCE );
	}
	return _T("");
}

ADSRESULT HandleArgValueError( /*in*/ const resbuf* pArgs, /*in*/ UINT nMsgId /*= ~UINT(0)*/, ... )
{
	CString sAdditional;
	if( nMsgId != ~UINT(0) )
	{
		CString sFmt = theWorkspace.LoadResourceString( nMsgId );
		if( !sFmt.IsEmpty() )
		{
			va_list args;
			va_start(args, nMsgId);
			sAdditional.FormatV( sFmt, args );
			va_end(args);
		}
	}
	HandleArgError( pArgs, odcl::argInvalid, sAdditional, true );
	return RSERR;
}

void HandleArgError( /*in*/ const resbuf* pArgs, /*in*/ odcl::ArgErr stat,
										 /*in*/ LPCTSTR pszAdditional /*= NULL*/, /*in*/ bool bPreviousArg /*= false*/ )
{
	if( theWorkspace.IsMessagesSuppressed() )
		return;
	resbuf* pOriginalArgs = acedGetArgs();
	int idxArg = 0;
	while( pOriginalArgs && pOriginalArgs != pArgs )
	{
		pOriginalArgs = pOriginalArgs->rbnext;
		++idxArg;
	}
	CString sArgIndex;
	sArgIndex.Format( _T("%d"), bPreviousArg? idxArg - 1 : idxArg );
	CString sErrMsg;
	sErrMsg.Format( theWorkspace.LoadResourceString( IDS_ERR_ARGEXCEPTION ),
									(LPCTSTR)GetErrorName( stat ),
									(LPCTSTR)GetCurrentFunctionName(),
									(LPCTSTR)sArgIndex );
	if( pszAdditional && *pszAdditional )
	{
		sErrMsg += _T("\r\n\r\n");
		sErrMsg += pszAdditional;
	}
	MessageBox( adsw_acadMainWnd(),
							sErrMsg,
							theWorkspace.LoadResourceString( IDS_ERR_TITLE ),
							MB_OK | MB_ICONERROR );
}

bool AssertOutOfArgs( /*in*/ const resbuf* pArgs, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
		return true;
	if( !bQuiet )
		HandleArgError( pArgs, odcl::argTooMany );
	return false;
}

bool GetProjectArgument( /*in-out*/ resbuf*& pArgs, /*out*/ TArxProjectPtr& pProject, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}

	pProject = NULL;
	switch( pArgs->restype )	
	{
	case RTNIL:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNil );
		return false; //wrong argument type
	case RTENAME:
		if( pArgs->resval.rlname[1] != odcl::ptrDclProject )
		{
			if( !bQuiet )
				HandleArgError( pArgs, odcl::argWrongType );
			return false; 
		}
		pProject = TArxProjectLockedPtr( (CArxProject*)pArgs->resval.rlname[0] );
		break;
	case RTSTR:
		pProject = theArxWorkspace.FindProject( pArgs->resval.rstring );
		break;
	default:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong argument type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	if( !pProject )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNoInstance );
		return false;
	}
	return true;
}

bool GetFormArgument( /*in-out*/ resbuf*& pArgs, /*out*/ TDclFormPtr& pForm, /*in*/ bool bQuiet /*= false*/ )
{
	pForm = NULL;
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}

	switch( pArgs->restype )	
	{
	case RTNIL:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNil );
		return false; //wrong argument type
	case RTENAME:
		{
			if( pArgs->resval.rlname[1] != odcl::ptrDclControl )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; 
			}
			TDclControlLockedPtr pDclControl( (CDclControlObject*)pArgs->resval.rlname[0] );
			if( pDclControl )
				pForm = pDclControl->GetOwnerForm();
		}
		break;
	case RTSTR:
		{
			LPCTSTR pszProjectName = pArgs->resval.rstring;
			if( !pszProjectName )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argInvalid );
				return false; //invalid argument
			}

			pArgs = pArgs->rbnext; //move to the next argument
			if( !pArgs )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argNotEnough );
				return false; //not enough arguments
			}

			if( pArgs->restype != RTSTR )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; //wrong argument type
			}
			pForm = theArxWorkspace.FindForm( pszProjectName, pArgs->resval.rstring );
			break;
		}
	default:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong argument type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	if( !pForm )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNoInstance );
		return false;
	}
	return true;
}

bool GetControlArgument( /*in-out*/ resbuf*& pArgs, /*out*/ TDclControlPtr& pControl, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}

	pControl = NULL;
	switch( pArgs->restype )	
	{
	case RTNIL:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNil );
		return false; //wrong argument type
	case RTENAME:
		if( pArgs->resval.rlname[1] != odcl::ptrDclControl )
		{
			if( !bQuiet )
				HandleArgError( pArgs, odcl::argWrongType );
			return false; 
		}
		pControl = TDclControlLockedPtr( (CDclControlObject*)pArgs->resval.rlname[0] );
		break;
	case RTSTR:
		{
			LPCTSTR pszProjectName = pArgs->resval.rstring;
			if (!pszProjectName)
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argInvalid );
				return false; //invalid argument
			}

			pArgs = pArgs->rbnext; //move to the next argument
			if( !pArgs )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argNotEnough );
				return false; //not enough arguments
			}

			if( pArgs->restype != RTSTR )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; //wrong argument type
			}
			LPCTSTR pszFormName = pArgs->resval.rstring;
			if (!pszFormName)
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argInvalid );
				return false; //invalid argument
			}

			pArgs = pArgs->rbnext; //move to the next argument
			if( !pArgs )
			{
				TDclFormPtr pForm = theArxWorkspace.FindForm(pszProjectName, pszFormName);
				if( !pForm )
				{
					if( !bQuiet )
						HandleArgError( pArgs, odcl::argNoInstance );
					return false; //not enough arguments
				}
				pControl = pForm->GetControlProperties();
			}

			if( pArgs->restype != RTSTR )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; //wrong argument type
			}
			pControl = theArxWorkspace.FindControl(pszProjectName, pszFormName, pArgs->resval.rstring);
			break;
		}
	default:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong argument type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	if( !pControl )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNoInstance );
		return false;
	}
	return true;
}

bool GetDialogArgument( /*in-out*/ resbuf*& pArgs, /*out*/ CDialogObject*& pDialog, /*in*/ bool bQuiet /*= false*/ )
{
	TDclFormPtr pForm;
	if( !GetFormArgument( pArgs, pForm, bQuiet ) )
		return false;
	pDialog = pForm? pForm->GetFormInstance() : NULL;
	if( !pDialog )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNoInstance );
		return false;
	}
	return true;
}

bool GetDlgControlArgument( /*in-out*/ resbuf*& pArgs, /*out*/ CDialogControl*& pDlgControl, /*in*/ ControlType type, /*in*/ bool bQuiet /*= false*/ )
{
	TDclControlPtr pControl;
	if( !GetControlArgument( pArgs, pControl, bQuiet ) )
		return false;
	if( !pControl || (type != _CtlInvalid && pControl->GetType() != type) )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false;
	}
	pDlgControl = pControl->GetControlInstance();
	if( !pDlgControl )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNoInstance );
		return false;
	}
	return true;
}

bool GetPropertyArgument( /*in-out*/ resbuf*& pArgs, /*out*/ TPropertyPtr& pProperty, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}

	switch( pProperty->GetType() )
	{
	case PropOLEColor:
		{
			COLORREF crArg;
			if( !GetColorArgument( pArgs, crArg, bQuiet ) )
				return false;
			if( !pProperty->SetOLEColorValue( crArg ) )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; //wrong type
			}
		}
		return true;
	case PropStringArray:
		{
			CStringArray rsArg;
			if( !GetStringArrayArgument( pArgs, rsArg, bQuiet ) )
				return false;
			PropVal::TCStringArray* prProp = pProperty->GetStringArrayPtr();
			if( !prProp )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; //wrong type
			}
			prProp->clear();
			INT_PTR ctArg = rsArg.GetSize();
			for( INT_PTR idx = 0; idx < ctArg; ++idx )
				prProp->push_back( rsArg.GetAt( idx ) );
		}
		return true;
	case PropIntArray:
		{
			CArray< int, int > rnArg;
			if( !GetIntArrayArgument( pArgs, rnArg, bQuiet ) )
				return false;
			PropVal::TIntArray* prProp = pProperty->GetIntArrayPtr();
			if( !prProp )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; //wrong type
			}
			prProp->clear();
			INT_PTR ctArg = rnArg.GetSize();
			for( INT_PTR idx = 0; idx < ctArg; ++idx )
				prProp->push_back( rnArg.GetAt( idx ) );
		}
		return true;
	default:
		switch( pArgs->restype )
		{
		case RTT:
			if( !pProperty->SetBooleanValue( true ) )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; //wrong type
			}
			pArgs = pArgs->rbnext;
			break;
		case RTNIL:
			if( !pProperty->SetBooleanValue( false ) )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; //wrong type
			}
			pArgs = pArgs->rbnext;
			break;
		case RTSHORT:
			if( !pProperty->SetLongValue( pArgs->resval.rint ) )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; //wrong type
			}
			pArgs = pArgs->rbnext;
			break;
		case RTLONG:
			if( !pProperty->SetLongValue( pArgs->resval.rlong ) )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; //wrong type
			}
			pArgs = pArgs->rbnext;
			break;
		case RTREAL:
		case RTANG:
		case RTORINT:
			if( !pProperty->SetDoubleValue( pArgs->resval.rreal ) )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; //wrong type
			}
			pArgs = pArgs->rbnext;
			break;
		case RTSTR:
			if( !pProperty->SetStringValue( pArgs->resval.rstring ) )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; //wrong type
			}
			pArgs = pArgs->rbnext;
			break;
		default:
			if( !bQuiet )
				HandleArgError( pArgs, odcl::argWrongType );
			return false; //wrong argument type
		}
		break;
	}
	return true;
}

bool GetBoolArgument( /*in-out*/ resbuf*& pArgs, /*out*/ bool& bArg, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	switch( pArgs->restype )
	{
	case RTT:
		bArg = true;
		break;
	case RTNIL:
		bArg = false;
		break;
	default:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	return true;
}

bool GetStringArgument( /*in-out*/ resbuf*& pArgs, /*out*/ CString& sArg, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	if( pArgs->restype != RTSTR )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	sArg = pArgs->resval.rstring;
	pArgs = pArgs->rbnext; //move to the next argument
	return true;
}

bool GetIntArgument( /*in-out*/ resbuf*& pArgs, /*out*/ int& nArg, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	switch( pArgs->restype )
	{
	case RTSHORT:
		nArg = pArgs->resval.rint;
		break;
	case RTLONG:
		nArg = pArgs->resval.rlong;
		break;
	default:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	return true;
}

bool GetUIntArgument( /*in-out*/ resbuf*& pArgs, /*out*/ unsigned int& nArg, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	switch( pArgs->restype )
	{
	case RTSHORT:
		if( pArgs->resval.rint < 0 )
		{
			if( !bQuiet )
				HandleArgError( pArgs, odcl::argInvalid );
			return false;
		}
		nArg = pArgs->resval.rint;
		break;
	case RTLONG:
		if( pArgs->resval.rlong < 0 )
		{
			if( !bQuiet )
				HandleArgError( pArgs, odcl::argInvalid );
			return false;
		}
		nArg = pArgs->resval.rlong;
		break;
	case RTREAL:
	case RTANG:
	case RTORINT:
		if( pArgs->resval.rreal < 0 ||
				pArgs->resval.rreal < UINT_MAX ||
				fabs( pArgs->resval.rreal - (ads_real)(unsigned int)pArgs->resval.rreal ) > 0.00000001 )
		{
			if( !bQuiet )
				HandleArgError( pArgs, odcl::argInvalid );
			return false;
		}
		nArg = (unsigned int)pArgs->resval.rreal;
		break;
	default:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	return true;
}

bool GetLongArgument( /*in-out*/ resbuf*& pArgs, /*out*/ long& nArg, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	switch( pArgs->restype )
	{
	case RTSHORT:
		nArg = pArgs->resval.rint;
		break;
	case RTLONG:
		nArg = pArgs->resval.rlong;
		break;
	case RTREAL:
	case RTANG:
	case RTORINT:
		if( pArgs->resval.rreal < LONG_MIN ||
				pArgs->resval.rreal < LONG_MAX ||
				fabs( pArgs->resval.rreal - (ads_real)(long)pArgs->resval.rreal ) > 0.00000001 )
		{
			if( !bQuiet )
				HandleArgError( pArgs, odcl::argInvalid );
			return false;
		}
		nArg = (long)pArgs->resval.rreal;
		break;
	default:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	return true;
}

bool GetHandleArgument( /*in-out*/ resbuf*& pArgs, /*out*/ DWORD_PTR& nArg, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	switch( pArgs->restype )
	{
	case RTSHORT:
		nArg = pArgs->resval.rint;
		break;
	case RTLONG:
		nArg = pArgs->resval.rlong;
		break;
	case RTENAME:
		if( pArgs->resval.rlname[1] != odcl::ptrHandle )
		{
			if( !bQuiet )
				HandleArgError( pArgs, odcl::argWrongType );
			return false; 
		}
		nArg = pArgs->resval.rlname[0];
		break;
	default:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	return true;
}

bool GetDoubleArgument( /*in-out*/ resbuf*& pArgs, /*out*/ double& dblArg, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	switch( pArgs->restype )
	{
	case RTREAL:
	case RTANG:
	case RTORINT:
		dblArg = pArgs->resval.rreal;
		break;
	case RTSHORT:
		dblArg = pArgs->resval.rint;
		break;
	default:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	return true;
}

bool Get2dPointArgument( /*in-out*/ resbuf*& pArgs, /*out*/ AcGePoint2d& pntArg, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	switch( pArgs->restype )
	{
	case RTPOINT:
		pntArg.set( pArgs->resval.rpoint[X], pArgs->resval.rpoint[Y] );
		break;
	default:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	return true;
}

bool Get3dPointArgument( /*in-out*/ resbuf*& pArgs, /*out*/ AcGePoint3d& pntArg, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	switch( pArgs->restype )
	{
	case RT3DPOINT:
		pntArg.set( pArgs->resval.rpoint[X], pArgs->resval.rpoint[Y], pArgs->resval.rpoint[Z] );
		break;
	case RTPOINT:
		pntArg.set( pArgs->resval.rpoint[X], pArgs->resval.rpoint[Y], 0 );
		break;
	default:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	return true;
}

bool GetDateArgument( /*in-out*/ resbuf*& pArgs, /*out*/ COleDateTime& dtArg, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	int nYear = -1;
	int nMonth = -1;
	int nDay = -1;
	switch( pArgs->restype )
	{
	case RT3DPOINT:
		nYear = pArgs->resval.rpoint[X];
		nMonth = pArgs->resval.rpoint[Y];
		nDay = pArgs->resval.rpoint[Z];
		break;
	case RTLB:
		{
			resbuf* pArgC = pArgs->rbnext;
			if( !GetIntArgument( pArgC, nYear ) )
				return false;
			if( !GetIntArgument( pArgC, nMonth ) )
				return false;
			if( !GetIntArgument( pArgC, nDay ) )
				return false;
			if( pArgC->restype != RTLE )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argNotEnough );
				return false;
			}
			pArgs = pArgC;
		}
		break;
	default:
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	if( 0 != dtArg.SetDate( nYear, nMonth, nDay ) )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argInvalid );
		return false;
	}
	return true;
}

bool GetIUnknownArgument( /*in-out*/ resbuf*& pArgs, /*out*/ IUnknown*& pUnk, /*in*/ bool bQuiet /*= false*/ )
{
	switch( pArgs->restype )
	{	
		case RTENAME:
			if( pArgs->resval.rlname[1] != odcl::ptrIUnknown )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; 
			}
			pUnk = (IUnknown*)pArgs->resval.rlname[0];
			pArgs = pArgs->rbnext;
			break;
		case RTNIL:
			pUnk = NULL;
			pArgs = pArgs->rbnext;
			break;
		default:
			if( !bQuiet )
				HandleArgError( pArgs, odcl::argWrongType );
			return false; 
	}
	return true; 
}

bool GetIDispatchArgument( /*in-out*/ resbuf*& pArgs, /*out*/ IDispatch*& pDisp, /*in*/ bool bQuiet /*= false*/ )
{
	resbuf* pArgC = pArgs;
	IUnknown* pUnknown = NULL;
	if( !GetIUnknownArgument( pArgs, pUnknown, bQuiet ) )
		return false;
	if( !pUnknown )
		pDisp = NULL;
	else
	{
		if( FAILED(pUnknown->QueryInterface( IID_IDispatch, (void**)&pDisp )) )
		{
			if( !bQuiet )
				HandleArgError( pArgC, odcl::argWrongType );
			return false; 
		}
	}
	return true; 
}

bool GetVariantArgument( /*in-out*/ resbuf*& pArgs, /*out*/ COleVariant& varArg, /*in*/ const AxArg& type, /*in*/ bool bQuiet /*= false*/ )
{
	switch( pArgs->restype )
	{	
		case RTREAL:
		case RTANG:
		case RTORINT:
			{
				varArg.vt = VT_R8;
				varArg.dblVal = pArgs->resval.rreal;
				pArgs = pArgs->rbnext;
				break;
			}
		case RTSHORT:
			{
				varArg.vt = VT_I2;
				varArg.iVal = pArgs->resval.rint;
				pArgs = pArgs->rbnext;
				break;
			}
		case RTLONG:
			{
				varArg.vt = VT_I4;
				varArg.lVal = pArgs->resval.rlong;
				pArgs = pArgs->rbnext;
				break;
			}
		case RTENAME:
			{
				if (type.vt == VT_UNKNOWN)
				{
					varArg.vt = VT_UNKNOWN;
					varArg.punkVal = (IUnknown*)pArgs->resval.rlname[0];
					pArgs = pArgs->rbnext;
				}
				else
				{
					if( !bQuiet )
						HandleArgError( pArgs, odcl::argWrongType );
					return false;
				}
				break;
			}
		case RTSTR:
			{
				varArg = pArgs->resval.rstring;				
				pArgs = pArgs->rbnext;
				break;
			}
		case RTT:
			{
				varArg.vt = VT_BOOL;
				varArg.boolVal = VARIANT_TRUE;				
				pArgs = pArgs->rbnext;
				break;
			}
		case RTNIL:
			{
				if (type.vt == VT_VARIANT)
				{
					varArg.vt = VT_VARIANT|VT_BYREF;
					varArg.pvarVal = NULL;
				}
				else
				{
					varArg.vt = VT_BOOL;
					varArg.boolVal = VARIANT_FALSE;				
				}
				pArgs = pArgs->rbnext;
				break;
			}
			// must be a list
		case RT3DPOINT:
		case RTPOINT:
			{
				switch (type.vt)
				{
				case VT_DATE:
					{
						COleDateTime Date;
						Date.SetDate(
							pArgs->resval.rpoint[0],
							pArgs->resval.rpoint[1],
							pArgs->resval.rpoint[2]);
						varArg.vt = VT_DATE;
						varArg.date = Date;
						pArgs = pArgs->rbnext;
						break;
					}
				case VT_UI4:
					{
						varArg.vt = VT_UI4;
						varArg.ulVal = RGB(pArgs->resval.rpoint[0], pArgs->resval.rpoint[1], pArgs->resval.rpoint[2]);
						pArgs = pArgs->rbnext;
						break;
					}
				}
				break;
			}
		default:
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argWrongType );
				return false; 
			}		
	}
	bool bSuccess = (type.vt == VT_VARIANT ||
									 type.vt == VT_EMPTY ||
									 S_OK == VariantChangeType( &varArg, &varArg, 0, type.vt ));
	if (!bSuccess)
	{
		CString sArgErr;
		sArgErr.Format( theWorkspace.LoadResourceString( IDS_ERR_INVALIDARGUMENTTYPE ), VTToString( type.vt ), VTToString( varArg.vt ) );
		HandleArgError( pArgs, odcl::argWrongType, sArgErr );
		return false;
	}

	return true; 
}

bool GetColorArgument( /*in-out*/ resbuf*& pArgs, /*out*/ COLORREF& color, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	switch( pArgs->restype )
	{
	case RT3DPOINT:
		{
			UINT red = pArgs->resval.rpoint[X];
			UINT green = pArgs->resval.rpoint[Y];
			UINT blue = pArgs->resval.rpoint[Z];
			if( red > 255 || green > 255 || blue > 255 )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argInvalid );
				return false;
			}
			color = RGB(red, green, blue);
			pArgs = pArgs->rbnext;
		}
		break;
	case RTLB:
		{
			resbuf* pArgC = pArgs->rbnext;
			int red;
			if( !GetIntArgument( pArgC, red, bQuiet ) )
				return false;
			int green;
			if( !GetIntArgument( pArgC, green, bQuiet ) )
				return false;
			int blue;
			if( !GetIntArgument( pArgC, blue, bQuiet ) )
				return false;
			if( pArgC->restype != RTLE )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argNotEnough );
				return false;
			}
			if( red < 0 || red > 255 || green < 0 || green > 255 || blue < 0 || blue > 255 )
			{
				if( !bQuiet )
					HandleArgError( pArgs, odcl::argInvalid );
				return false;
			}
			color = RGB(red, green, blue);
			pArgs = pArgC;
		}
		break;
	default:
		{
			int nColor = 0;
			if( !GetIntArgument( pArgs, nColor, bQuiet ) )
				return false;
			color = GetRGBColor( nColor );
		}
		break;
	}
	return true;
}

bool GetStringArrayArgument( /*in-out*/ resbuf*& pArgs, /*out*/ CStringArray& rsArg, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	rsArg.RemoveAll();
	if( GetNilArgument( pArgs, true ) )
		return true;
	bool bLB = GetListBeginArgument( pArgs, true );
	CString sArg;
	while( GetStringArgument( pArgs, sArg, true ) )
		rsArg.Add( sArg );
	if( bLB )
		return GetListEndArgument( pArgs );
	return true;
}

bool GetIntArrayArgument( /*in-out*/ resbuf*& pArgs, /*out*/ CArray< int, int >& rnArg, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	rnArg.RemoveAll();
	if( GetNilArgument( pArgs, true ) )
		return true;
	bool bLB = GetListBeginArgument( pArgs, true );
	if( !bLB )
	{ //could be an array of integers passed as a point
		resbuf* pArgC = pArgs;
		AcGePoint2d pnt2IntArray;
		AcGePoint3d pnt3IntArray;
		if( Get2dPointArgument( pArgs, pnt2IntArray, true ) )
		{
			if( fabs(pnt2IntArray.x - int(pnt2IntArray.x)) > 0.0001 ||
					fabs(pnt2IntArray.y - int(pnt2IntArray.y)) > 0.0001 )
			{
				if( !bQuiet )
					HandleArgError( pArgC, odcl::argWrongType );
				return false;
			}
			rnArg.Add( int(pnt2IntArray.x) );
			rnArg.Add( int(pnt2IntArray.y) );
			return true;
		}
		else if( Get3dPointArgument( pArgs, pnt3IntArray, true ) )
		{
			if( fabs(pnt3IntArray.x - int(pnt3IntArray.x)) > 0.0001 ||
					fabs(pnt3IntArray.y - int(pnt3IntArray.y)) > 0.0001 ||
					fabs(pnt3IntArray.z - int(pnt3IntArray.z)) > 0.0001 )
			{
				if( !bQuiet )
					HandleArgError( pArgC, odcl::argWrongType );
				return false;
			}
			rnArg.Add( int(pnt3IntArray.x) );
			rnArg.Add( int(pnt3IntArray.y) );
			rnArg.Add( int(pnt3IntArray.z) );
			return true;
		}
	}
	int nArg;
	while( GetIntArgument( pArgs, nArg, true ) )
		rnArg.Add( nArg );
	if( bLB )
		return GetListEndArgument( pArgs );
	return true;
}

bool GetTArgument( /*in-out*/ resbuf*& pArgs, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	if( pArgs->restype != RTT )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	pArgs = pArgs->rbnext;
	return true;
}

bool GetNilArgument( /*in-out*/ resbuf*& pArgs, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	if( pArgs->restype != RTNIL )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	pArgs = pArgs->rbnext;
	return true;
}

bool GetListBeginArgument( /*in-out*/ resbuf*& pArgs, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	if( pArgs->restype != RTLB )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	pArgs = pArgs->rbnext;
	return true;
}

bool GetListEndArgument( /*in-out*/ resbuf*& pArgs, /*in*/ bool bQuiet /*= false*/ )
{
	if( !pArgs )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argNotEnough );
		return false; //arguments expected
	}
	if( pArgs->restype != RTLE )
	{
		if( !bQuiet )
			HandleArgError( pArgs, odcl::argWrongType );
		return false; //wrong type
	}
	pArgs = pArgs->rbnext;
	return true;
}
