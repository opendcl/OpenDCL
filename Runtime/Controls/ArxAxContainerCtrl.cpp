#include "StdAfx.h"

#include "ArxAxContainerCtrl.h"
#include "ArxWorkspace.h"
#include "AxEventDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "Project.h"
#include "VarUtils.h"
#include "Workspace.h"


static CString LongToS( LONG lValue )
{
	CString sVal;
	sVal.Format( _T("%d"), lValue );
	return sVal;
}


static resbuf* LongToRB( LONG lValue )
{
	if( lValue > SHRT_MAX || lValue < SHRT_MIN )
	{
		resbuf* prb = acutNewRb( RTLONG );
		prb->resval.rlong = (long)lValue;
		return prb;
	}
	resbuf* prb = acutNewRb( RTSHORT );
	prb->resval.rint = (short)lValue;
	return prb;
}


static CString ULongToS( ULONG lValue )
{
	CString sVal;
	sVal.Format( _T("%u"), lValue );
	return sVal;
}


static resbuf* ULongToRB( ULONG lValue )
{
	if( lValue > USHRT_MAX )
	{
		resbuf* prb = acutNewRb( RTLONG );
		prb->resval.rlong = (long)lValue;
		return prb;
	}
	resbuf* prb = acutNewRb( RTSHORT );
	prb->resval.rint = (short)lValue;
	return prb;
}


static CString LongLongToS( LONGLONG lValue )
{
	CString sVal;
	sVal.Format( _T("%I64d"), lValue );
	return sVal;
}


static resbuf* LongLongToRB( LONGLONG lValue )
{
	if( lValue > LONG_MAX || lValue < LONG_MIN )
	{
		resbuf* prb = acutNewRb( RTREAL );
		prb->resval.rreal = (ads_real)lValue;
		return prb;
	}
	if( lValue > SHRT_MAX || lValue < SHRT_MIN )
	{
		resbuf* prb = acutNewRb( RTLONG );
		prb->resval.rlong = (long)lValue;
		return prb;
	}
	resbuf* prb = acutNewRb( RTSHORT );
	prb->resval.rint = (short)lValue;
	return prb;
}


static CString DoubleToS( double dValue )
{
	static CString sDefault = _T("0.0");
	CString sReturn;
	if( RTNORM != acdbRToS( dValue, 2, 8, sReturn.GetBuffer( 256 ) ) )
		return sDefault;
	sReturn.ReleaseBuffer();
	return sReturn;
}


static resbuf* DoubleToRB( double dValue )
{
	resbuf* prb = acutNewRb( RTREAL );
	prb->resval.rreal = (ads_real)dValue;
	return prb;
}


static CString DateToS( DATE dValue )
{
	COleDateTime dt( dValue );
	CString sReturn;
	sReturn.Format( _T("'(%d %d %d)"), dt.GetYear(), dt.GetMonth(), dt.GetDay() );
	return sReturn;
}


static resbuf* DateToRB( DATE dValue )
{
	COleDateTime dt( dValue );
	resbuf* prb = acutNewRb( RT3DPOINT );
	prb->resval.rpoint[0] = (ads_real)dt.GetYear();
	prb->resval.rpoint[1] = (ads_real)dt.GetMonth();
	prb->resval.rpoint[2] = (ads_real)dt.GetDay();
	return prb;
}


static resbuf* IUnknownToRB( IUnknown* pValue )
{
	resbuf* prb = acutNewRb( RTENAME );
	prb->resval.rlname[0] = (LONG_PTR)pValue;
	prb->resval.rlname[1] = odcl::ptrIUnknown;
	return prb;
}


static resbuf* StringToRB( const BSTR& sValue )
{
	resbuf* prb = acutNewRb( RTSTR );
	acutNewString( bstr_t( sValue ), prb->resval.rstring );
	return prb;
}


static resbuf* VariantArgToResbuf( const VARIANTARG& varArg, resbuf*& prbTail )
{
	const VARIANT& var = (varArg.vt == (VT_VARIANT | VT_BYREF)? *varArg.pvarVal : varArg);
	switch( var.vt )
	{
	case VT_UI1: return (prbTail = acutNewRb( (var.bVal == 1)? RTT : RTNIL ));
	case (VT_UI1 | VT_BYREF): return (prbTail = acutNewRb( (*var.pbVal == 1)? RTT : RTNIL ));
	case VT_I2: return (prbTail = LongToRB( var.iVal ));
	case (VT_I2 | VT_BYREF): return (prbTail = LongToRB( *var.piVal ));
	case VT_I4: return (prbTail = LongToRB( var.lVal ));
	case (VT_I4 | VT_BYREF): return (prbTail = LongToRB( *var.plVal ));
	case VT_R4: return (prbTail = DoubleToRB( var.fltVal ));
	case (VT_R4 | VT_BYREF): return (prbTail = DoubleToRB( *var.pfltVal ));
	case VT_R8: return (prbTail = LongToRB( var.dblVal ));
	case (VT_R8 | VT_BYREF): return (prbTail = LongToRB( *var.pdblVal ));
	case VT_CY: return (prbTail = LongLongToRB( var.cyVal.int64 ));
	case (VT_CY | VT_BYREF): return (prbTail = LongLongToRB( var.pcyVal->int64 ));
	case VT_DATE: return (prbTail = DateToRB( var.date ));
	case (VT_DATE | VT_BYREF): return (prbTail = DateToRB( *var.pdate ));
	case VT_BSTR: return (prbTail = StringToRB( var.bstrVal ));
	case (VT_BSTR | VT_BYREF): return (prbTail = StringToRB( *var.pbstrVal ));
#if !defined(_UNICODE) && !defined(OLE2ANSI)
	case VT_BSTRA: return (prbTail = StringToRB( var.bstrVal ));
	case (VT_BSTRA | VT_BYREF): return (prbTail = StringToRB( *var.pbstrVal ));
#endif
	case VT_BOOL: return (prbTail = acutNewRb( (var.boolVal == VARIANT_TRUE)? RTT : RTNIL ));
	case (VT_BOOL | VT_BYREF): return (prbTail = acutNewRb( (*var.pboolVal == VARIANT_TRUE)? RTT : RTNIL ));
	case VT_ERROR: return (prbTail = LongToRB( var.scode ));
	case (VT_ERROR | VT_BYREF): return (prbTail = LongToRB( *var.pscode ));
	case VT_UNKNOWN: return (prbTail = IUnknownToRB( var.punkVal ));
	case (VT_UNKNOWN | VT_BYREF): return (prbTail = IUnknownToRB( *var.ppunkVal ));
	}	
	return (prbTail = acutNewRb( RTNIL ));
}


static CString VariantArgToString( const VARIANTARG& varArg )
{
	const VARIANT& var = (varArg.vt == (VT_VARIANT | VT_BYREF)? *varArg.pvarVal : varArg);
	switch( var.vt )
	{
	case VT_UI1: return ((var.bVal == 1)? _T("T") : _T("NIL"));
	case (VT_UI1 | VT_BYREF): return ((*var.pbVal == 1)? _T("T") : _T("NIL"));
	case VT_I2: return LongToS( var.iVal );
	case (VT_I2 | VT_BYREF): return LongToS( *var.piVal );
	case VT_I4: return LongToS( var.lVal );
	case (VT_I4 | VT_BYREF): return LongToS( *var.plVal );
	case VT_R4: return DoubleToS( var.fltVal );
	case (VT_R4 | VT_BYREF): return DoubleToS( *var.pfltVal );
	case VT_R8: return LongToS( var.dblVal );
	case (VT_R8 | VT_BYREF): return LongToS( *var.pdblVal );
	case VT_CY: return LongLongToS( var.cyVal.int64 );
	case (VT_CY | VT_BYREF): return LongLongToS( var.pcyVal->int64 );
	case VT_DATE: return DateToS( var.date );
	case (VT_DATE | VT_BYREF): return DateToS( *var.pdate );
	case VT_BSTR: return var.bstrVal;
	case (VT_BSTR | VT_BYREF): return *var.pbstrVal;
#if !defined(_UNICODE) && !defined(OLE2ANSI)
	case VT_BSTRA: return var.bstrVal;
	case (VT_BSTRA | VT_BYREF): return *var.pbstrVal;
#endif
	case VT_BOOL: return ((var.boolVal == VARIANT_TRUE)? _T("T") : _T("NIL"));
	case (VT_BOOL | VT_BYREF): return ((*var.pboolVal == VARIANT_TRUE)? _T("T") : _T("NIL"));
	case VT_ERROR: return LongToS( var.scode );
	case (VT_ERROR | VT_BYREF): return LongToS( *var.pscode );
	}	
	return _T("NIL");
}


template<> class arg_t< DISPPARAMS* > : arg_b
{
	const DISPPARAMS* _arg;
public:
	typedef DISPPARAMS* type;
	arg_t( DISPPARAMS* arg ) : _arg( arg ) {}
	virtual resbuf* asResbuf() const
		{
			if( !_arg || _arg->cArgs == 0 )
				return NULL;
			resbuf* prbHead = NULL;
			resbuf* prbTail = NULL;
			UINT idx = _arg->cArgs;
			while( idx > 0 )
			{
				resbuf* prbArgTail = NULL;
				resbuf* prbArg = VariantArgToResbuf( _arg->rgvarg[--idx], prbArgTail );
				if( !prbHead )
					prbHead = prbArg;
				else
					prbTail->rbnext = prbArg;
				prbTail = prbArgTail;
			}
			return prbHead;
		}
	virtual CString asString() const
		{
			if( !_arg || _arg->cArgs == 0 )
				return _T("");
			CString sArg;
			UINT idx = _arg->cArgs;
			while( idx > 0 )
			{
				sArg += _T(' ');
				sArg += VariantArgToString( _arg->rgvarg[--idx] );
			}
			return sArg;
		}
};
typedef args_1< DISPPARAMS* > args_X;


CArxAxContainerCtrl::CArxAxContainerCtrl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate) 
: CAxContainerCtrl(pTemplate, pPane, nID, false)
, mArxServices( this )
{
	TraceFmt( _T("> CArxAxContainerCtrl::CArxAxContainerCtrl(%s [%p], [%p], %s [HWND: %p]) [this: %p]\r\n"),
		(LPCTSTR)pTemplate->GetKeyPath(), &*pTemplate, pPane, (LPCTSTR)CString(mpTemplate->GetRuntimeClass()->m_lpszClassName),
		mpTemplate->GetWindow(), this );

	if( bCreate ) 
		Create( pPane->GetHostDialog(), nID );
}

CArxAxContainerCtrl::~CArxAxContainerCtrl()
{
	TraceFmt( _T("< CArxDialogControl::~CArxDialogControl() [this: %p]\r\n"), this );
}


bool CArxAxContainerCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( pParentWnd, nID );

	return bSuccess;
}


BOOL CArxAxContainerCtrl::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo ) 
{
	if( nCode == CN_EVENT && nID == GetTemplate()->GetControlInstance()->GetControlId() )
		HandleAxEvent( (AFX_EVENT*)pExtra );

	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CArxAxContainerCtrl::HandleAxEvent( AFX_EVENT* pEvent )
{
	if( !pEvent || pEvent->m_eventKind != AFX_EVENT::event )
		return;
	const TPropertyList& Props = GetTemplate()->GetPropertyList();
	for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
	{
		const CPropertyObject* pProp = (*iter);
		if( !pProp )
			continue;
		const AxInterfaceDescriptor* pID = pProp->GetConstAxInterfaceDescriptorPtr();
		if( !pID )
			continue;
		const AxEventDescriptor* pED = pID->GetEvent();
		if( !pED )
			continue;
		if( pED->GetDispId() == pEvent->m_dispid && !pProp->GetStringValue().IsEmpty() )
			GetArxServices()->HandleEvent( pProp->GetStringValue(), args_X( pEvent->m_pDispParams ) );
	}
}


/*
void CArxAxContainerCtrl::FireAxEvent( const CPropertyObject* pProp, AFX_EVENT* pEvent )
{
	try
	{

		// do standard method call
		VARIANT var;
		AfxVariantInit(&var);

		DISPPARAMS dispparams;
		dispparams.rgvarg = NULL;

		memcpy(&dispparams, pEvent->m_pDispParams, sizeof(DISPPARAMS));
		dispparams.rgvarg = new VARIANT[++dispparams.cArgs];
		memcpy(dispparams.rgvarg, pEvent->m_pDispParams->rgvarg,
			sizeof(VARIANT) * (dispparams.cArgs-1));

		// this code is used if the programmer want to call the defun using the default option
		struct resbuf *pRBList = NULL, *pRB = NULL, *pRBFinal = NULL;
		// this code is used if the programmer want to call a (Command ...) from the defun
		bool bShowCommand = false;
		CString sCommand = CString("(") + FireCancel(pProp->GetStringValue()) + " ";

		// again this code is for allow (Command ...)
		if (IsAsyncEvents())
		{
			if (pProp->GetStringValue().Left(1) == _T("'"))
			{
				sCommand = pProp->GetStringValue() + _T(" ");
				bShowCommand = true;
			}
		}

		// Again this code is used if the programmer want to call the defun using the default option
		if (!IsAsyncEvents())
		{
			pRBFinal = acutNewRb(RTSTR);	//	create new resbuf
			acutNewString(pProp->GetStringValue(), pRBFinal->resval.rstring);

			//	If this is first resbuf in list,
			//	we need to store its location in pRBList
			pRBList = pRBFinal;
			pRBList->rbnext = NULL;
			pRBList->restype = RTSTR;
		}

		// do loop starting from the last argument to the index of 1. Because ActiveX arguments are in reverse order.
		// don't ask me why!
		// The Index last index is not an argument to be passed to AutoLISP.
		for (int i=dispparams.cArgs-2; i>=0; i--)
		{
			VARIANT* pVarArg = &dispparams.rgvarg[i];

			// again this code is for allow (Command ...)
			if (IsAsyncEvents())
			{
				sCommand += acedVarToString(pVarArg) + " ";
			}

			// Again this code is used if the programmer want to call the defun using the default option
			if (!IsAsyncEvents())
			{
				// get the type of argument to return;
				int nRtType = acedGetRtType(pVarArg);
				pRB = acutNewRb(nRtType);	//	create new resbuf

				switch (pVarArg->vt)
				{
				case VT_VARIANT:
					break;
				case VT_VARIANT|VT_BYREF:
					pVarArg = pVarArg->pvarVal;
					break;
				}

				switch (nRtType)
				{
				case RTLB:
					try
					{
						IDispatch *pDisp = NULL;

						switch (pVarArg->vt)
						{
						case VT_DISPATCH:
						case VT_UNKNOWN:
							pDisp = pVarArg->pdispVal;
							break;
						case VT_DISPATCH|VT_BYREF:
						case VT_UNKNOWN|VT_BYREF:							
							pDisp = *pVarArg->ppdispVal;								
							break;
						}

						// Build up the linked list
						// Remember that pRBList still points
						// to the beginning of the liked list.
						pRBList->rbnext = pRB;
						pRBList = pRB;

						switch (pVarArg->vt)
						{
						case VT_DATE:
							// create a new short
							pRB = acutNewRb(RTSHORT);	//	create new resbuf

							COleDateTime Date = pVarArg->date;

							pRB->resval.rint = Date.GetYear();

							// Build up the linked list
							// Remember that pRBList still points
							// to the beginning of the liked list.
							pRBList->rbnext = pRB;
							pRBList = pRB;

							// create a new short
							pRB = acutNewRb(RTSHORT);	//	create new resbuf

							pRB->resval.rint = Date.GetMonth();

							// Build up the linked list
							// Remember that pRBList still points
							// to the beginning of the liked list.
							pRBList->rbnext = pRB;
							pRBList = pRB;

							// create a new short
							pRB = acutNewRb(RTSHORT);	//	create new resbuf

							pRB->resval.rint = Date.GetDay();									
							break;
						}
						// for dispatch type arguments
						if (pDisp != NULL)
						{
							// create a new long
							pRB = acutNewRb(RTENAME);	//	create new resbuf

							// get the ITypeInfo
							ITypeInfo *TheInfo = NULL;
							// get this project
							TProjectPtr pProject = GetTemplate()->GetOwnerProject();

							pDisp->GetTypeInfo(0, ::GetUserDefaultLCID(), &TheInfo);

							// get the TYPEATTR
							TYPEATTR *TheAttr;
							TheInfo->GetTypeAttr(&TheAttr);

							IUnknown* pUnknown = NULL;
							if( SUCCEEDED(pDisp->QueryInterface( &pUnknown )) )
								theArxWorkspace.AddUnknown( pUnknown );
							pRB->resval.rlname[0] = (LONG_PTR)pUnknown;
							pRB->resval.rlname[1] = odcl::ptrIUnknown;

							// Build up the linked list
							// Remember that pRBList still points
							// to the beginning of the liked list.
							pRBList->rbnext = pRB;
							pRBList = pRB;
							// create a new long
							pRB = acutNewRb(RTENAME);	//	create new resbuf
							// set the dispatch as the new long
							pRB->resval.rlname[0] = (DWORD_PTR)pVarArg->pdispVal;
							pRB->resval.rlname[1] = 0;
						}
						// Build up the linked list
						// Remember that pRBList still points
						// to the beginning of the liked list.
						pRBList->rbnext = pRB;
						pRBList = pRB;

						// close the list.
						pRB = acutNewRb(RTLE);	//	create new resbuf
					}
					catch(...)
					{
						// close the list.
						pRB = acutNewRb(RTLE);	//	create new resbuf							
					}						
					break;

				case RTNIL:						
					pRB->resval.rint = 0;
					break;
				case RTT:						
					pRB->resval.rint = 1;
					break;
				case RTSHORT:						
					pRB->resval.rint = int(acedVarToLong(pVarArg));
					break;
				case RTLONG:						
					pRB->resval.rlong = acedVarToLong(pVarArg);
					break;
				case RTREAL:						
					pRB->resval.rreal = acedVarToDouble(pVarArg);
					break;
				case RTSTR:		
					acutNewString(acedVarToString(pVarArg), pRB->resval.rstring);
					break;
				}

				// Build up the linked list
				// Remember that pRBList still points
				// to the beginning of the liked list.
				pRBList->rbnext = pRB;
				pRBList = pRB;
			}
		}

		// Again this code is used if the programmer want to call the defun using the default option
		if (!IsAsyncEvents())
		{	
			struct resbuf *result = NULL;    
			int stat = acedInvoke(pRBFinal, &result);
			acutRelRb(pRBList);				
		}

		// again this code is for allow (Command ...)
		if (IsAsyncEvents())
		{
			sCommand += ") ";

			Acad::ErrorStatus es;

			// get current document
			AcApDocument* pDoc = acDocManager->curDocument();

			// give the command bar focus
			//CWnd* CmdBarWnd = acedGetAcadDockCmdLine();
			//CmdBarWnd->SetFocus();		

			// send cancel string to current document
			es = acDocManager->sendStringToExecute(pDoc, sCommand, false, true, bShowCommand);
		}

		//hResult = CallMemberFunc(&pEntry->dispEntry, DISPATCH_METHOD, &var,
		//	(bRange ? &dispparams : pEvent->m_pDispParams), &uArgError);
		ASSERT(FAILED(hResult) || (V_VT(&var) == VT_BOOL));
		BOOL bHandled = V_BOOL(&var);

		delete [] dispparams.rgvarg;
	}
	catch(...)
	{
	}
}
*/
