#include "StdAfx.h"

#include "acutmem.h"
#include "ArxAxContainerCtrl.h"
#include "AxEventDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "Project.h"
#include "VarUtils.h"
#include "Workspace.h"

CString acedVarToString(VARIANT *pVarGet);
long acedVarToLong(VARIANT *pVarGet);
int acedGetRtType(VARIANT *pVarGet);
double acedVarToDouble(VARIANT *pVarGet);
CString LongToA(long lValue);

BEGIN_MESSAGE_MAP(CArxAxContainerCtrl, CAxContainerCtrl)
END_MESSAGE_MAP()

CArxAxContainerCtrl::CArxAxContainerCtrl(CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate) 
: CAxContainerCtrl(pTemplate, pPane, nID, false)
{
	if( bCreate ) 
	{
		Create( pPane->GetHostDialog(), nID );
	} 
	else 
	{
		m_bInvokeWithSendString = false;
	}
}
bool CArxAxContainerCtrl::Create( CWnd* pParentWnd, UINT nID ) {
	bool bSuccess = CAxContainerCtrl::Create( pParentWnd, nID );

	if( GetTemplate()->GetLngProperty(nEventInvoke) == 1 )
	{
		m_bInvokeWithSendString = true;
	} 
	else
	{
		m_bInvokeWithSendString = false;
	}

	return bSuccess;

}
BOOL CArxAxContainerCtrl::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (nID == GetTemplate()->GetControlInstance()->GetControlId())
		TryToFireAxEvent(nID, (AFX_EVENT*) pExtra);

	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
void CArxAxContainerCtrl::TryToFireAxEvent(UINT idCtrl, AFX_EVENT* pEvent)
{
	const AFX_EVENTSINKMAP* pSinkMap = GetEventSinkMap();
	//const AFX_EVENTSINKMAP_ENTRY* pEntry;
	size_t flag = (pEvent->m_eventKind != AFX_EVENT::event);

	POSITION pos = GetTemplate()->GetPropertyList().GetHeadPosition();
	while (pos != NULL)
	{
		// get a pointer to the property
		CPropertyObject *pProp = GetTemplate()->GetPropertyList().GetNext(pos);
		if (pProp == NULL) {
			continue;
		}
		AxInterfaceDescriptor* pID = pProp->GetAxInterfaceDescriptorPtr();
		if (pID == NULL) {
			continue;
		}
		AxEventDescriptor* pED = pID->GetEvent();
		if (pED == NULL) {
			continue;
		}

		// is this is the event that we are looking for?
		// if the event defun has been set
		if (pED->GetDispId() == pEvent->m_dispid && !pProp->GetStringValue().IsEmpty())
		{
  		FireAxEvent(idCtrl, pProp, pEvent);
		}
	}
}

void CArxAxContainerCtrl::FireAxEvent(UINT idCtrl, CPropertyObject* pProp, AFX_EVENT* pEvent)
{
	try
	{
		if (pEvent->m_eventKind == AFX_EVENT::event)
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
			VARIANT* pvarID = &dispparams.rgvarg[dispparams.cArgs-1];
			V_VT(pvarID) = VT_I4;
			V_I4(pvarID) = idCtrl;

			// if no arguments are to be passed (1 is a default for not arguments)
			if (dispparams.cArgs == 1)
			{				
				// just call InvokeMethod
				delete [] dispparams.rgvarg;
				InvokeMethod(pProp->GetStringValue(), m_bInvokeWithSendString);
				return;
			}
			// this code is used if the programmer want to call the defun using the default option
			struct resbuf *pRBList = NULL, *pRB = NULL, *pRBFinal = NULL;
			// this code is used if the programmer want to call a (Command ...) from the defun
			bool bShowCommand = false;
			CString sCommand = CString("(") + FireCancel(pProp->GetStringValue()) + " ";

			// again this code is for allow (Command ...)
			if (m_bInvokeWithSendString)
			{
				if (pProp->GetStringValue().Left(1) == "'")
				{
					sCommand = pProp->GetStringValue() + " ";
					bShowCommand = true;
				}
			}

			// Again this code is used if the programmer want to call the defun using the default option
			if (!m_bInvokeWithSendString)
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
			// The Index last index is not an argument to be passed to AutoLisp.
			for (int i=dispparams.cArgs-2; i>=0; i--)
			{
				VARIANT* pVarArg = &dispparams.rgvarg[i];

				// again this code is for allow (Command ...)
				if (m_bInvokeWithSendString)
				{
					sCommand += acedVarToString(pVarArg) + " ";
				}

				// Again this code is used if the programmer want to call the defun using the default option
				if (!m_bInvokeWithSendString)
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
								pRB = acutNewRb(RTLONG);	//	create new resbuf

								// get the ITypeInfo
								ITypeInfo *TheInfo = NULL;
								// get this project
								CProject *pProject = theWorkspace.GetActiveProject();

								pDisp->GetTypeInfo(0, ::GetUserDefaultLCID(), &TheInfo);

								// get the TYPEATTR
								TYPEATTR *TheAttr;
								TheInfo->GetTypeAttr(&TheAttr);

								// Get the CArxControlObject
								COleControlObject *pOleObject = pProject->GetOleObject(TheAttr->guid);
								pRB->resval.rlong = (ULONG)pOleObject;

								// Build up the linked list
								// Remember that pRBList still points
								// to the beginning of the liked list.
								pRBList->rbnext = pRB;
								pRBList = pRB;
								// create a new long
								pRB = acutNewRb(RTLONG);	//	create new resbuf
								// set the dispatch as the new long
								pRB->resval.rlong = (ULONG)pVarArg->pdispVal;

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
			if (!m_bInvokeWithSendString)
			{	
				struct resbuf *result = NULL;    
				int stat = acedInvoke(pRBFinal, &result);
				acutRelRb(pRBList);				
			}

			// again this code is for allow (Command ...)
			if (m_bInvokeWithSendString)
			{
				sCommand += ") ";

				Acad::ErrorStatus es;

				// get current document
				AcApDocument* pDoc = acDocManager->curDocument();

				// give the command bar focus
				CWnd* CmdBarWnd = acedGetAcadDockCmdLine();
				CmdBarWnd->SetFocus();		

				// send cancel string to current document
				es = acDocManager->sendStringToExecute(pDoc, sCommand, false, true, bShowCommand);
			}

			//hResult = CallMemberFunc(&pEntry->dispEntry, DISPATCH_METHOD, &var,
			//	(bRange ? &dispparams : pEvent->m_pDispParams), &uArgError);
			ASSERT(FAILED(hResult) || (V_VT(&var) == VT_BOOL));
			BOOL bHandled = V_BOOL(&var);

			delete [] dispparams.rgvarg;
		}
	}
	catch(...)
	{
	}

}

//*****************************************************************************
//Utility functions for dealing with variants and parameters for ActiveX controls.
CString LongToA(long lValue)
{
	TCHAR fmtval[26]; 
	ads_real adsValue = lValue;
	int stat = acdbRToS(adsValue, 2,0, fmtval); 
	CString sReturn = fmtval;
	if (stat == RTNORM)
		return sReturn;	

	return "";
}

//*****************************************************************************
// Method: acedGetRtType()
// Purpose: [returns RT Type from the ActiveX variant for acutNewRb]
// Parameters: varGet
// Returns:	int
//*****************************************************************************
int acedGetRtType(VARIANT *pVarGet)
{
	switch (pVarGet->vt)
	{
	case VT_UI1:
		if (pVarGet->bVal == 1)
			return RTT;
		else
			return RTNIL;
		break;
	case VT_I2:			
		return RTSHORT;
		break;
	case VT_I4:
		return RTLONG;
		break;
	case VT_R4:
		return RTREAL;
		break;
	case VT_R8:
		return RTREAL;
		break;
	case VT_DATE:
		{
			return RTLB;
			break;
		}
	case VT_CY:
		{
			return RTREAL;
			break;
		}
	case VT_BSTR:
#if !defined(_UNICODE) && !defined(OLE2ANSI)
	case VT_BSTRA:
#endif
		{

			return RTSTR;
			break;
		}
	case VT_DISPATCH:
		return RTLB;
		//pVarGet->pdispVal =  argList->m_Variant[i].pdispVal;
		break;
	case VT_ERROR:
		{
			return RTLONG;
			break;
		}
	case VT_BOOL:			
		{
			if (pVarGet->boolVal == 1)
				return RTT;
			else
				return RTNIL;
			break;
		}
	case VT_VARIANT:
		return acedGetRtType(pVarGet->pvarVal);
		break;
	case VT_UNKNOWN:
		return RTLB;
		break;

	case VT_I2|VT_BYREF:			
		return RTSHORT;
		break;
	case VT_UI1|VT_BYREF:
		{
			if (*pVarGet->pbVal == 1)
				return RTT;
			else
				return RTNIL;
			break;
		}
	case VT_I4|VT_BYREF:
		{
			return RTLONG;
			break;
		}
	case VT_R4|VT_BYREF:
		return RTREAL;
		break;
	case VT_R8|VT_BYREF:
		return RTREAL;
		break;
	case VT_DATE|VT_BYREF:
		{
			return RTLB;
			break;
		}
	case VT_CY|VT_BYREF:
		{
			return RTREAL;
			break;
		}
	case VT_BSTR|VT_BYREF:
		{			
			return RTSTR;
			break;
		}
	case VT_DISPATCH|VT_BYREF:
		return RTLB;
		break;
	case VT_ERROR|VT_BYREF:
		return RTLONG;
		break;
	case VT_BOOL|VT_BYREF:
		{
			if (*pVarGet->pboolVal == 1)
				return RTT;
			else
				return RTNIL;
		}
		break;
	case VT_VARIANT|VT_BYREF:
		return acedGetRtType(pVarGet->pvarVal);
		break;
	case VT_UNKNOWN|VT_BYREF:
		return RTNIL;
		break;

	default:
		return RTNIL;
		break;
	}	
	return RTNIL;
}


//*****************************************************************************
// Method: acedVarToLong()
// Purpose: [returns an long from the ActiveX variant for sendStringToExecute]
// Parameters: varGet
// Returns:	long
//*****************************************************************************
long acedVarToLong(VARIANT *pVarGet)
{
	switch (pVarGet->vt)
	{
	case VT_I2:			
		return pVarGet->iVal;
		break;
	case VT_I4:
		return pVarGet->lVal;
		break;
	case VT_CY:
		{				
			CY cy(pVarGet->cyVal);
			return cy.Lo;
			break;
		}
	case VT_ERROR:
		{
			return pVarGet->scode;
			break;
		}
	case VT_I2|VT_BYREF:			
		return *pVarGet->piVal;
		break;
	case VT_I4|VT_BYREF:
		{
			return *pVarGet->plVal;
			break;
		}
	case VT_CY|VT_BYREF:
		{
			CY cy(*pVarGet->pcyVal);
			return cy.Lo;
			break;
		}
	case VT_ERROR|VT_BYREF:
		return *pVarGet->pscode;
		break;
	default:
		return 0;
		break;
	}	
	return 0;
}


//*****************************************************************************
// Method: acedVarToDouble()
// Purpose: [returns an double from the ActiveX variant for sendStringToExecute]
// Parameters: varGet
// Returns:	double
//*****************************************************************************
double acedVarToDouble(VARIANT *pVarGet)
{
	switch (pVarGet->vt)
	{
	case VT_CY:
		return pVarGet->cyVal.Lo;
	case VT_CY|VT_BYREF:
		return pVarGet->pcyVal->Lo;
	case VT_R4:
		return pVarGet->fltVal;
		break;
	case VT_R8:
		return pVarGet->dblVal;
		break;
	case VT_R4|VT_BYREF:
		return *pVarGet->pfltVal;
		break;
	case VT_R8|VT_BYREF:
		return *pVarGet->pdblVal;
		break;
	}	
	return 0.0;
}



//*****************************************************************************
// Method: acedVarToString()
// Purpose: [returns an string from the ActiveX variant for sendStringToExecute]
// Parameters: varGet
// Returns:	CString
//*****************************************************************************
CString acedVarToString(VARIANT *pVarGet)
{
	switch (pVarGet->vt)
	{
	case VT_UI1:
		if (pVarGet->bVal == 1)
			return "1";
		else
			return "";
		break;
	case VT_I2:			
		return LongToA(pVarGet->iVal);
		break;
	case VT_I4:
		return LongToA(pVarGet->lVal);
		break;
	case VT_R4:
		return RtoS(pVarGet->fltVal);
		break;
	case VT_R8:
		return RtoS(pVarGet->dblVal);
		break;
	case VT_CY:
		{
			CY cy(pVarGet->cyVal);
			return RtoS( cy.Lo);
			break;
		}
	case VT_BSTR:
#if !defined(_UNICODE) && !defined(OLE2ANSI)
	case VT_BSTRA:
#endif
		{
			CString sValue = pVarGet->bstrVal;
			return sValue;
			break;
		}
	case VT_ERROR:
		{
			return RtoS(pVarGet->scode);
			break;
		}
	case VT_BOOL:			
		{
			if (pVarGet->boolVal == 1)
				return "t";
			else
				return "nil";
			break;
		}
	case VT_I2|VT_BYREF:			
		return LongToA(*pVarGet->piVal);
		break;
	case VT_UI1|VT_BYREF:
		{
			if (*pVarGet->pbVal == 1)
				return "1";
			else
				return "";
			break;
		}
	case VT_I4|VT_BYREF:
		{
			return LongToA(*pVarGet->plVal);
			break;
		}
	case VT_R4|VT_BYREF:
		return RtoS(*pVarGet->pfltVal);
		break;
	case VT_R8|VT_BYREF:
		return RtoS(*pVarGet->pdblVal);
		break;
	case VT_CY|VT_BYREF:
		{
			CY cy(*pVarGet->pcyVal);
			return RtoS(cy.Lo);
			break;
		}
	case VT_BSTR|VT_BYREF:
		{
			CString sValue = *pVarGet->pbstrVal;
			return sValue;
			break;
		}
	case VT_DISPATCH|VT_BYREF:
		return "VT_DISPATCH|VT_BYREF";
		break;
	case VT_ERROR|VT_BYREF:
		return LongToA(*pVarGet->pscode);
		break;
	case VT_BOOL|VT_BYREF:
		{
			if (*pVarGet->pboolVal == 1)
				return "t";
			else
				return "nil";
		}
		break;
	case VT_VARIANT|VT_BYREF:
		return "VT_VARIANT|VT_BYREF";
		break;
	case VT_UNKNOWN|VT_BYREF:
		return "VT_UNKNOWN|VT_BYREF";
		break;

	default:
		return "nil";
		break;
	}	
	return "nil";
}
