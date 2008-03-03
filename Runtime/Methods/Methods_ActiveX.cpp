// Methods_ActiveX.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ActiveX.h"
#include "Resource.h"
#include "AxContainerCtrl.h"
#include "AxMethodDescriptor.h"
#include "AxPropertyDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ArgumentsRetrieval.h"
#include "ArxWorkspace.h"
#include "AcadColorTable.h"

static void acedRetOleVar(COleVariant &varGet, TPropertyPtr pProp = NULL, AxMethodDescriptor *pMethod = NULL, CAxContainerCtrl *pAxContainer = NULL, AxPropertyDescriptor *pAxProp = NULL);
static bool GetVariantArgumentList( CArray< COleVariant >& rArgs,
																		size_t ctParams,
																		resbuf*& pArgs,
																		AxPropertyDescriptor* pAxProp,
																		AxMethodDescriptor* pAxMethod );


static void ReturnDate( const COleDateTime& Date )
{
	resbuf rbDay = { NULL, RTSHORT };
	rbDay.resval.rint = Date.GetDay();
	resbuf rbMonth = { &rbDay, RTSHORT };
	rbMonth.resval.rint = Date.GetMonth();
	resbuf rbYear = { &rbMonth, RTSHORT };
	rbYear.resval.rint = Date.GetYear();
	acedRetList( &rbYear );
}


ADSRESULT AxControl::GetProperty()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl = NULL;
	if (!GetControlArgument (pArgs, pControl))
		return RSERR; //invalid input
	CDialogControl* pDlgControl = pControl->GetControlInstance();
	if( !pDlgControl )
		return RSERR; //invalid input
	CAxContainerCtrl* pAxCont = pDlgControl->GetActiveXCtrl();
	if( !pAxCont )
		return RSERR; //invalid input

	CString sPropName;
	if( !GetStringArgument( pArgs, sPropName ) )
		return RSERR; //invalid input
	TPropertyPtr pProp = pControl->FindPropertyObject(sPropName);
	if( !pProp )
		return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, (LPCTSTR)sPropName );

	RefCountedPtr< AxPropertyDescriptor > pAxProp = NULL;
	if (pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef() &&
		(pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_DISPATCH || 
		 pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_UNKNOWN ||
		 pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_VOID))
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef();
	else if (pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPut())
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPut();
	else if (pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef())
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef();
	else if (pProp->GetConstAxInterfaceDescriptorPtr()->GetProp())
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetProp();
	if( !pAxProp )
		return RSRSLT;
	
	size_t ctParams = pAxProp->GetArgs().size();
	CArray< COleVariant > rArgs;
	if( !GetVariantArgumentList( rArgs, ctParams, pArgs, pAxProp, NULL ) )
		return RSRSLT;

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	COleVariant varGet;
	if( !SUCCEEDED(pAxCont->GetProperty( pAxProp, rArgs.GetData(), rArgs.GetSize(), varGet )) )
		return RSRSLT;

	acedRetOleVar( varGet, pProp, NULL, pAxCont, pAxProp );
	return RSRSLT;
}

ADSRESULT AxControl::SetProperty()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl = NULL;
	if (!GetControlArgument (pArgs, pControl))
		return RSERR; //invalid input
	CDialogControl* pDlgControl = pControl->GetControlInstance();
	if( !pDlgControl )
		return RSERR; //invalid input
	CAxContainerCtrl* pAxCont = pDlgControl->GetActiveXCtrl();
	if( !pAxCont )
		return RSERR; //invalid input

	CString sPropName;
	if( !GetStringArgument( pArgs, sPropName ) )
		return RSERR; //invalid input
	TPropertyPtr pProp = pControl->FindPropertyObject(sPropName);
	if( !pProp )
		return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, (LPCTSTR)sPropName );

	RefCountedPtr< AxPropertyDescriptor > pAxProp = NULL;
	if (pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef() &&
			(pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_DISPATCH || 
			 pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_UNKNOWN ||
			 pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_VOID))
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef();
	else if (pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPut())
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPut();
	else if (pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef())
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef();
	else if (pProp->GetConstAxInterfaceDescriptorPtr()->GetProp())
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetProp();
	if( !pAxProp )
		return RSRSLT;
	
	size_t ctParams = pAxProp->GetArgs().size();
	CArray< COleVariant > rArgs;
	if( !GetVariantArgumentList( rArgs, ctParams, pArgs, pAxProp, NULL ) )
		return RSRSLT;

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !SUCCEEDED(pAxCont->SetProperty( pAxProp, rArgs.GetData(), rArgs.GetSize() )) )
		return RSRSLT;

	acedRetT();
	return RSRSLT;
}

ADSRESULT AxControl::DoMethod()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl = NULL;
	if (!GetControlArgument (pArgs, pControl))
		return RSERR; //invalid input
	CDialogControl* pDlgControl = pControl->GetControlInstance();
	if( !pDlgControl )
		return RSERR; //invalid input
	CAxContainerCtrl* pAxCont = pDlgControl->GetActiveXCtrl();
	if( !pAxCont )
		return RSERR; //invalid input

	CString sMethodName;
	if( !GetStringArgument( pArgs, sMethodName ) )
		return RSERR; //invalid input

	TPropertyPtr pProp = pControl->GetMethods();
	if( !pProp || pProp->GetType() != PropActiveXMethods )
		return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, _T("ActiveX Methods") );

	// here we need to search the methods to find the one requested.
	std::vector< RefCountedPtr< AxMethodDescriptor > > *vMethods = pProp->GetConstAxInterfaceDescriptorPtr()->GetMethods();
	RefCountedPtr< AxMethodDescriptor > pMethod;
	for (int i = 0; i < vMethods->size(); i++)
	{
		RefCountedPtr< AxMethodDescriptor > pMethodCheck = vMethods->at(i);
		if (pMethodCheck->GetName() == sMethodName)
		{
			pMethod = pMethodCheck;
			break;
		}
	}
	if( !pMethod )
		return HandleArgValueError( pArgs, IDS_ERR_NOTAMETHOD, (LPCTSTR)sMethodName );

	CArray< COleVariant > rArgs;
	if( !GetVariantArgumentList( rArgs, pMethod->GetArgs().size(), pArgs, NULL, pMethod ) )
		return RSRSLT;

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	COleVariant varResult;
	if( !SUCCEEDED(pAxCont->Invoke( pMethod, rArgs.GetData(), rArgs.GetSize(), varResult )) )
		return RSRSLT;

	if( varResult.vt == VT_EMPTY )
		acedRetT();
	else
		acedRetOleVar( varResult, NULL, pMethod, pAxCont );
	return RSRSLT;
}

ADSRESULT AxControl::SetColorProperty()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl = NULL;
	if (!GetControlArgument (pArgs, pControl))
		return RSERR; //invalid input
	CDialogControl* pDlgControl = pControl->GetControlInstance();
	if( !pDlgControl )
		return RSERR; //invalid input
	CAxContainerCtrl* pAxCont = pDlgControl->GetActiveXCtrl();
	if( !pAxCont )
		return RSERR; //invalid input

	CString sPropName = _T("Color");
	GetStringArgument( pArgs, sPropName, true );
	TPropertyPtr pProp = pControl->FindPropertyObject(sPropName);
	if( !pProp )
		return RSRSLT;

	RefCountedPtr< AxPropertyDescriptor > pAxProp = NULL;
	if (pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef() &&
		(pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_DISPATCH || 
		 pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_UNKNOWN ||
		 pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_VOID))
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef();
	else if (pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPut())
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPut();
	else if (pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef())
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef();
	else if (pProp->GetConstAxInterfaceDescriptorPtr()->GetProp())
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetProp();
	if( !pAxProp )
		return RSRSLT;
	
	if( pAxProp->GetArgs().size() == 0 )
		return RSRSLT;

	COLORREF color;
	if( !GetColorArgument( pArgs, color ) )
		return RSERR;

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	pAxCont->SetColor( pAxProp->GetDispId(), color );
	acedRetT();
	return RSRSLT;
}

ADSRESULT AxControl::SetPictureProperty()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl = NULL;
	if (!GetControlArgument (pArgs, pControl))
		return RSERR; //invalid input
	CDialogControl* pDlgControl = pControl->GetControlInstance();
	if( !pDlgControl )
		return RSERR; //invalid input
	CAxContainerCtrl* pAxCont = pDlgControl->GetActiveXCtrl();
	if( !pAxCont )
		return RSERR; //invalid input

	CString sPropName = _T("Picture");
	GetStringArgument( pArgs, sPropName, true );
	TPropertyPtr pProp = pControl->FindPropertyObject(sPropName);
	if( !pProp )
		return RSRSLT;

	RefCountedPtr< AxPropertyDescriptor > pAxProp = NULL;
	if (pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef() &&
		(pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_DISPATCH || 
		 pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_UNKNOWN ||
		 pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_VOID))
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef();
	else if (pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPut())
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPut();
	else if (pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef())
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetPropPutRef();
	else if (pProp->GetConstAxInterfaceDescriptorPtr()->GetProp())
		pAxProp = pProp->GetConstAxInterfaceDescriptorPtr()->GetProp();
	if( !pAxProp )
		return RSRSLT;
	
	if( pAxProp->GetArgs().size() == 0 )
		return RSRSLT;

	UINT nPicId = -1;
	CString sPicFilename;
	if( !GetStringArgument( pArgs, sPicFilename, true ) &&
			!GetUIntArgument( pArgs, nPicId ) )
		return RSERR;

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !sPicFilename.IsEmpty() )
		pAxCont->LoadPictureFile( pAxProp->GetDispId(), sPicFilename, 0 );
	else
		pAxCont->LoadPicture( pAxProp->GetDispId(), nPicId );
	acedRetT();
	return RSRSLT;
}

ADSRESULT AxControl::GetOleObject()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl = NULL;
	if (!GetControlArgument (pArgs, pControl))
		return RSERR; //invalid input
	CDialogControl* pDlgControl = pControl->GetControlInstance();
	if( !pDlgControl )
		return RSERR; //invalid input
	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CAxContainerCtrl* pAxCont = pDlgControl->GetActiveXCtrl();
	if( !pAxCont )
		return RSERR; //invalid input

	theArxWorkspace.RetIUnknown( pAxCont->GetControlUnknown() );

	return RSRSLT;
}


ADSRESULT AxObject::GetProperty()
{
	struct resbuf *pArgs =acedGetArgs () ;

	IDispatch* pDisp = NULL;
	if (!GetIDispatchArgument (pArgs, pDisp))
		return RSERR; //invalid input
	if( !pDisp )
		return RSRSLT;

	CString sPropName;
	if( !GetStringArgument( pArgs, sPropName ) )
		return RSERR; //invalid input

	bstr_t bsPropName( sPropName );
	DISPID idDisp = 0;
	if( !SUCCEEDED(pDisp->GetIDsOfNames( IID_NULL, &bsPropName.GetBSTR(), 1, LOCALE_INVARIANT, &idDisp )) )
		return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, (LPCTSTR)sPropName );

	resbuf* pArgC = pArgs;
	size_t ctArgs = 0;
	while( pArgC )
	{
		pArgC = pArgC->rbnext;
		++ctArgs;
	}
	pArgC = pArgs;
	COleVariant* rvarArgs = (ctArgs > 0? new COleVariant[ctArgs] : NULL);
	for( size_t idx = 0; idx < ctArgs; ++idx )
	{
		if( !GetVariantArgument( pArgs, rvarArgs[ctArgs - idx - 1], AxArg() ) )
		{
			delete [] rvarArgs;
			return RSERR;
		}
	}

	if( !AssertOutOfArgs( pArgs ) )
	{
		delete [] rvarArgs;
		return RSERR;
	}

	DISPPARAMS params = { rvarArgs, NULL, ctArgs, 0 };
	COleVariant varResult;
	EXCEPINFO exception = { NULL };
	UINT nErrArg = 0;
	HRESULT hr =
		pDisp->Invoke( idDisp, IID_NULL, LOCALE_INVARIANT, DISPATCH_PROPERTYGET, &params, &varResult, &exception, &nErrArg );

	delete [] rvarArgs;

	if( !SUCCEEDED(hr) )
	{
		if( nErrArg > 0 )
		{
			CString sExtraInfo = exception.bstrDescription;
			pArgs = pArgC;
			while( nErrArg > 0 && pArgs )
				pArgs = pArgs->rbnext;
			HandleArgValueError( pArgs, IDS_ERR_AXPARAM, (LPCTSTR)sExtraInfo );
			return RSERR;
		}
		return RSRSLT;
	}

	acedRetOleVar( varResult );
	return RSRSLT;
}

ADSRESULT AxObject::SetProperty()
{
	struct resbuf *pArgs =acedGetArgs () ;

	IDispatch* pDisp = NULL;
	if (!GetIDispatchArgument (pArgs, pDisp))
		return RSERR; //invalid input
	if( !pDisp )
		return RSRSLT;

	CString sPropName;
	if( !GetStringArgument( pArgs, sPropName ) )
		return RSERR; //invalid input

	bstr_t bsPropName( sPropName );
	DISPID idDisp = 0;
	if( !SUCCEEDED(pDisp->GetIDsOfNames( IID_NULL, &bsPropName.GetBSTR(), 1, LOCALE_INVARIANT, &idDisp )) )
		return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, (LPCTSTR)sPropName );

	resbuf* pArgC = pArgs;
	size_t ctArgs = 0;
	while( pArgC )
	{
		pArgC = pArgC->rbnext;
		++ctArgs;
	}
	pArgC = pArgs;
	COleVariant* rvarArgs = (ctArgs > 0? new COleVariant[ctArgs] : NULL);
	for( size_t idx = 0; idx < ctArgs; ++idx )
	{
		if( !GetVariantArgument( pArgs, rvarArgs[ctArgs - idx - 1], AxArg() ) )
		{
			delete [] rvarArgs;
			return RSERR;
		}
	}

	if( !AssertOutOfArgs( pArgs ) )
	{
		delete [] rvarArgs;
		return RSERR;
	}

	DISPPARAMS params = { rvarArgs, NULL, ctArgs, 0 };
	COleVariant varResult;
	EXCEPINFO exception = { NULL };
	UINT nErrArg = 0;
	DISPID d1 = 0;
	params.cNamedArgs = 1;
	params.rgdispidNamedArgs = &d1;
	HRESULT hr =
		pDisp->Invoke( idDisp, IID_NULL, LOCALE_INVARIANT, DISPATCH_PROPERTYPUT, &params, &varResult, &exception, &nErrArg );

	delete [] rvarArgs;

	if( !SUCCEEDED(hr) )
	{
		if( nErrArg > 0 )
		{
			CString sExtraInfo = exception.bstrDescription;
			pArgs = pArgC;
			while( nErrArg > 0 && pArgs )
				pArgs = pArgs->rbnext;
			HandleArgValueError( pArgs, IDS_ERR_AXPARAM, (LPCTSTR)sExtraInfo );
			return RSERR;
		}
		return RSRSLT;
	}

	acedRetT();
	return RSRSLT;
}

ADSRESULT AxObject::DoMethod()
{
	struct resbuf *pArgs =acedGetArgs () ;

	IDispatch* pDisp = NULL;
	if (!GetIDispatchArgument (pArgs, pDisp))
		return RSERR; //invalid input
	if( !pDisp )
		return RSRSLT;

	CString sMethodName;
	if( !GetStringArgument( pArgs, sMethodName ) )
		return RSERR; //invalid input

	bstr_t bsMethodName( sMethodName );
	DISPID idDisp = 0;
	if( !SUCCEEDED(pDisp->GetIDsOfNames( IID_NULL, &bsMethodName.GetBSTR(), 1, LOCALE_INVARIANT, &idDisp )) )
		return HandleArgValueError( pArgs, IDS_ERR_NOTAMETHOD, (LPCTSTR)sMethodName );

	resbuf* pArgC = pArgs;
	size_t ctArgs = 0;
	while( pArgC )
	{
		pArgC = pArgC->rbnext;
		++ctArgs;
	}
	pArgC = pArgs;
	COleVariant* rvarArgs = (ctArgs > 0? new COleVariant[ctArgs] : NULL);
	for( size_t idx = 0; idx < ctArgs; ++idx )
	{
		if( !GetVariantArgument( pArgs, rvarArgs[ctArgs - idx - 1], AxArg() ) )
		{
			delete [] rvarArgs;
			return RSERR;
		}
	}

	if( !AssertOutOfArgs( pArgs ) )
	{
		delete [] rvarArgs;
		return RSERR;
	}

	DISPPARAMS params = { rvarArgs, NULL, ctArgs, 0 };
	COleVariant varResult;
	EXCEPINFO exception = { NULL };
	UINT nErrArg = 0;
	HRESULT hr =
		pDisp->Invoke( idDisp, IID_NULL, LOCALE_INVARIANT, DISPATCH_METHOD, &params, &varResult, &exception, &nErrArg );

	delete [] rvarArgs;

	if( !SUCCEEDED(hr) )
	{
		if( nErrArg > 0 )
		{
			CString sExtraInfo = exception.bstrDescription;
			pArgs = pArgC;
			while( nErrArg > 0 && pArgs )
				pArgs = pArgs->rbnext;
			HandleArgValueError( pArgs, IDS_ERR_AXPARAM, (LPCTSTR)sExtraInfo );
			return RSERR;
		}
		return RSRSLT;
	}

	acedRetOleVar( varResult );
	return RSRSLT;
}

ADSRESULT AxObject::Close()
{
	struct resbuf *pArgs =acedGetArgs () ;

	IUnknown* pUnknown = NULL;
	if( !GetIUnknownArgument( pArgs, pUnknown ) )
		return RSERR; //invalid input
	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pUnknown )
		return RSRSLT;

	theArxWorkspace.RemoveUnknown( pUnknown );
	acedRetT();
	return RSRSLT;
}

void acedRetOleVar(COleVariant &varGet, TPropertyPtr pProp, AxMethodDescriptor *pMethod, CAxContainerCtrl *pAxContainer, AxPropertyDescriptor *pAxProp)
{
	struct resbuf RetVal;
			
	switch (varGet.vt)
	{
		case VT_UI1:
			if (varGet.bVal == 1)
				acedRetT();
			else
				acedRetNil();
			break;
		case VT_I2:
			acedRetInt(varGet.iVal);
			break;
		case VT_UI4:
		case VT_I4:
			{
				if (pAxProp != NULL)
				{
					if (pAxProp->GetGuid() == GUID_COLOR)
					{
						int stat;
						COLORREF clr = varGet.lVal;
						struct resbuf *list;    
						int nRed = GetRValue(clr);
						int nGreen = GetGValue(clr);
						int nBlue = GetBValue(clr);
						
						list = acutBuildList(
							RTSHORT, nRed,
							RTSHORT, nGreen,
							RTSHORT, nBlue,
							RTNONE);

						if (list != NULL) 
						{ 	    
							stat = acedRetList(list);		
							acutRelRb(list); 
						}
					}
					else
						theArxWorkspace.RetLong(varGet.vt == VT_UI4? (LONG)varGet.ulVal : varGet.lVal);
				}
				else if (pMethod != NULL)
				{
					if (pMethod->GetReturnGuid() == GUID_COLOR)
					{
						int stat;
						COLORREF clr = varGet.lVal;
						struct resbuf *list;    
						int nRed = GetRValue(clr);
						int nGreen = GetGValue(clr);
						int nBlue = GetBValue(clr);
						
						list = acutBuildList(
							RTSHORT, nRed,
							RTSHORT, nGreen,
							RTSHORT, nBlue,
							RTNONE);

						if (list != NULL) 
						{ 	    
							stat = acedRetList(list);		
							acutRelRb(list); 
						}
					}
					else
						theArxWorkspace.RetLong(varGet.vt == VT_UI4? (LONG)varGet.ulVal : varGet.lVal);
				}
				else
					theArxWorkspace.RetLong(varGet.vt == VT_UI4? (LONG)varGet.ulVal : varGet.lVal);
			}
			break;
		case VT_R4:
			acedRetReal(varGet.fltVal);
			break;
		case VT_R8:
			acedRetReal(varGet.dblVal);
			break;
		case VT_DATE:
			{
				COleDateTime date(varGet.date);
				ReturnDate(date);
			}
			break;
		case VT_CY:
			{
				CY cy(varGet.cyVal);
				acedRetReal( cy.Lo);
			}
			break;
		case VT_BSTR:
#if !defined(_UNICODE) && !defined(OLE2ANSI)
		case VT_BSTRA:
#endif
			{
				CString sValue = varGet.bstrVal;
				acedRetStr(sValue);
			}
			break;
		case VT_DISPATCH:
			{
				IUnknown* pUnknown = NULL;
				if( varGet.pdispVal )
					varGet.pdispVal->QueryInterface( IID_IUnknown, (void**)&pUnknown );
				theArxWorkspace.RetIUnknown( pUnknown );
			break;
			}
		case VT_ERROR:
			{
				RetVal.restype = RTLONG;
				RetVal.resval.rlong = varGet.scode;
				acedRetVal(&RetVal);
			}
			break;
		case VT_BOOL:			
			{
				if (varGet.boolVal == 1)
					acedRetT();
				else
					acedRetNil();
			}
			break;
		case VT_VARIANT:
			acedRetVoid();
			//varGet.pvarVal
			break;
		case VT_UNKNOWN:
			theArxWorkspace.RetIUnknown( varGet.punkVal );
			break;
		case VT_I2|VT_BYREF:			
			acedRetInt(*varGet.piVal);
			break;
		case VT_UI1|VT_BYREF:
			{
				if (*varGet.pbVal == 1)
					acedRetT();
				else
					acedRetNil();
			}
			break;
		case VT_UI4|VT_BYREF:
			theArxWorkspace.RetLong(*varGet.pulVal);
			break;
		case VT_I4|VT_BYREF:
			theArxWorkspace.RetLong(*varGet.plVal);
			break;
		case VT_R4|VT_BYREF:
			acedRetReal(*varGet.pfltVal);
			break;
		case VT_R8|VT_BYREF:
			acedRetReal(*varGet.pdblVal);
			break;
		case VT_DATE|VT_BYREF:
			{
				COleDateTime date(*varGet.pdate);
				ReturnDate(date);
			}
			break;
		case VT_CY|VT_BYREF:
			{
				CY cy(*varGet.pcyVal);
				acedRetReal(cy.Lo);
			}
			break;
		case VT_BSTR|VT_BYREF:
			{
				CString sValue = *varGet.pbstrVal;
				acedRetStr(sValue);
			}
			break;
		case VT_DISPATCH|VT_BYREF:
			{
				IUnknown* pUnknown = NULL;
				if( varGet.ppdispVal && varGet.ppdispVal )
					(*varGet.ppdispVal)->QueryInterface( IID_IUnknown, (void**)&pUnknown );
				theArxWorkspace.RetIUnknown( pUnknown );
			}
			break;
		case VT_ERROR|VT_BYREF:
			acedRetVoid();
			RetVal.restype = RTLONG;
			RetVal.resval.rlong = *varGet.pscode;
			acedRetVal(&RetVal);
			break;
		case VT_BOOL|VT_BYREF:
			{
				if (*varGet.pboolVal == VARIANT_TRUE)
					acedRetT();
				else
					acedRetNil();
			}
			break;
		case VT_VARIANT|VT_BYREF:
			acedRetVoid();
			break;
		case VT_UNKNOWN|VT_BYREF:
			theArxWorkspace.RetIUnknown( varGet.ppunkVal? *varGet.ppunkVal : NULL );
			break;

		default:
			acedRetNil();
			break;
	}	
}

bool GetVariantArgumentList( CArray< COleVariant >& rArgs,
														 size_t ctParams,
														 resbuf*& pArgs,
														 AxPropertyDescriptor* pAxProp,
														 AxMethodDescriptor* pAxMethod )
{
	//[DPR] argList must be built backward for SetProperty to work properly.
	//I tried reversing the list in SetProperty, but couldn't get variants to copy.
	//Decided to move out the reversing out here instead.

	if (pAxMethod == NULL && pAxProp == NULL)
		return false;

	//I go through the list twice so I can get the number of arguments, then
	//properly read it back into the list backwards.

	//Determine number of arguments, because it might be less than number
	//of parameters
	resbuf* pArgC = pArgs;
	size_t ctArgs = 0;
	while( pArgC )
	{
		pArgC = pArgC->rbnext;
		++ctArgs;
	}
	if( ctArgs > ctParams )
		ctArgs = ctParams;
	rArgs.SetSize( ctArgs );

	for (int idxArg = 0; idxArg < ctArgs; idxArg++)
	{
		if (!GetVariantArgument( pArgs,
														 rArgs[ctArgs - idxArg - 1],
														 pAxProp? pAxProp->GetArgs()[idxArg] : pAxMethod->GetArgs()[idxArg] ))
		{
			return false;
		}
	}
	return true;
}