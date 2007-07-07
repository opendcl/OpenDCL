// ArgumentsRetrieval.cpp : implementation file
//

#include "stdafx.h"
#include "ArgumentsRetrieval.h"
#include "Resource.h"
#include "ErrorLexicon.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ArxProject.h"
#include "DclFormObject.h"
#include "ControlPane.h"
#include "VarUtils.h"



CString LtoString (long nLong)
{
	char Value[80];
	_ltoa(nLong, Value, 10);
	return Value;
}

CControlPane* GetRequestedControlPane(CString sFileName, CString sDialogName)
{
	CDialogObject* pDialog = theArxWorkspace.FindDialog(sFileName, sDialogName);
	if (pDialog)
		return &pDialog->GetControlPane();
	return NULL;
}

//CDclControlObject * GetRequestedArxObject(CString sFileName, CString sDialogName, CString sControlName)
//{
//	CDialogObject* pDialog = theArxWorkspace.FindDialog(sFileName, sDialogName);
//	if (!pDialog)
//		return NULL;
//	return pDialog->GetControlPane().FindArxObject(sControlName);
//	//return theArxWorkspace.FindControl(sFileName, sDialogName, sControlName);
//}

TDialogControlPtr GetRequestedControl( LPCTSTR pszProjectKey, LPCTSTR pszDialogName, LPCTSTR pszControlName,
																			 ControlType nControlType = CtlInvalid )
{
	CDialogObject *pDialog = theArxWorkspace.FindDialog( pszProjectKey, pszDialogName );
	if( !pDialog )
		return NULL;
	return pDialog->GetControlPane().FindControl( pszControlName, nControlType );
}

CWnd* GetRequestedControlWnd( LPCTSTR pszProjectKey,
															LPCTSTR pszDialogName,
															LPCTSTR pszControlName,
															ControlType nControlType = CtlInvalid )
{
	TDialogControlPtr pControl = GetRequestedControl( pszProjectKey, pszDialogName, pszControlName, nControlType );
	if( !pControl )
		return NULL;
	return pControl->GetControl();
}

CDclControlObject* GetRequestedDclControl( LPCTSTR pszProjectKey,
																					 LPCTSTR pszDialogName,
																					 LPCTSTR pszControlName,
																					 ControlType nControlType = CtlInvalid )
{
	TDialogControlPtr pControl = GetRequestedControl( pszProjectKey, pszDialogName, pszControlName, nControlType );
	if( !pControl )
		return NULL;
	return pControl->GetTemplate();
}

///////////////////1//////////////////////////////////////////////////////////
// ArgumentsRetrieval

//*****************************************************************************
// 
// Method: IsArgumentString()
// 
// Purpose: [check if this arg is a string]
// 
// Parameters:  
//		[restype]:  [TODO: Write the description of this parameter]
// 
// Returns:	int
// 
//*****************************************************************************
bool IsArgumentString(short restype, int index, CString sMethod)
{
	if (restype != RTSTR)
	{
		CString sIndex = LtoString(index);
		if (sMethod.GetLength() > 0)
		{
			// inform the programmer that he did not make the correct call
			theWorkspace.DisplayAlert(
				CString(ErrorTheErrorStart) + sIndex + ErrorTheErrorMiddle + sMethod +
				ErrorEndErrorNotaString	);	
		}
		return false;
	}

	return true;
}

bool IsArgumentInt(short restype, int index, CString sMethod)
{
	if (restype != RTSHORT)
	{
		CString sIndex = LtoString(index);
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(
			CString(ErrorTheErrorStart) + sIndex + ErrorTheErrorMiddle + sMethod +
			ErrorEndErrorNotanInt);	
		return false;
	}

	return true;
}

bool IsArgumentDouble(short restype, int index, CString sMethod)
{
	if (restype != RTREAL && restype != RTSHORT)
	{
		CString sIndex = LtoString(index);
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(
			CString(ErrorTheErrorStart) + sIndex + ErrorTheErrorMiddle + sMethod +
			ErrorEndErrorNotaDbl);
		return false;
	}

	return true;
}


//*****************************************************************************
// 
// Method: GetProjectAndFormName()
// 
// Purpose: [Get the arguments sent by AutoCAD. 
//			 With this overloaded version get 2 strings]
// 
// Parameters:  
//		[*pArg1]:  [TODO: Write the description of this parameter]
//		[*pArg2]:  [TODO: Write the description of this parameter]
// 
// 
//*****************************************************************************
struct resbuf *GetProjectAndFormName(CString *pArg1, CString *pArg2, CString sMethod)
{	
	struct resbuf *ListData;

 	//ensure AutoLISP has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod); 			
        return NULL; 
	}
	
	if (IsArgumentString(ListData->restype, 0, sMethod)) 
	{		
		// get the first argument required
		*pArg1 = ListData->resval.rstring;				
	}
	else
	{	
	    return NULL; 
	}

	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// check if the argument is missing
	if (ListData == NULL) 
	{
		CString sIndex = LtoString(1);
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(
			CString(ErrorTheErrorStart) + sIndex + ErrorTheErrorMiddle + sMethod +
			ErrorArgumentMissing);	
		        return NULL; 
	} 

	if (IsArgumentString(ListData->restype, 1, sMethod))
	{
		// get the argument required
		*pArg2 = ListData->resval.rstring;			
	}
	else
	{
		return NULL; 
	}
	
	return ListData;
}

//*****************************************************************************
// 
// Method: GetStringArgument()
// 
// Purpose: [gets the expected string argument from AutoLISP]
// 
// Parameters:  
//		[nIndex]:  [0 based index]
//		[*pArg]:  [TODO: Write the description of this parameter]
// 
// Returns:	bool
// 
//*****************************************************************************
bool GetStringArgument(int nIndex, CString *pArg, CString sMethod)
{
	struct resbuf *ListData;

	//ensure AutoLISP has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod); 			
        return false; 
	}

	for (int i = 0; i < nIndex; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		return false;
	}

	// if this argument is the begining of a list
	if (ListData->restype == RTLB)
		return false; 

	if (IsArgumentString(ListData->restype, nIndex, sMethod)) 
	{		
		// get the first argument required
		*pArg = ListData->resval.rstring;				
	}
	else
	{	
		return false; 
	}
	return true; 
}
//*****************************************************************************
// 
// Method: GetStringOrLongArgument()
// 
// Purpose: [gets the expected string or long argument from AutoLISP]
// 
// Parameters:  
//		[nIndex]:  [0 based index]
//		[*pArg]:  [TODO: Write the description of this parameter]
// 
// Returns:	bool
// 
//*****************************************************************************
bool GetStringOrLongArgument(int nIndex, CString *pArg, ULONG *uLong, CString sMethod)
{
	struct resbuf *ListData;

	//ensure AutoLISP has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod); 			
        return false; 
	}

	for (int i = 0; i < nIndex; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		return false;
	}

	// if this argument is the begining of a list
	if (ListData->restype == RTLB)
		return false; 

	if (ListData->restype == RTLONG)
	{
		// get the first argument required
		*uLong = ListData->resval.rlong;				
	}
	else if (ListData->restype == RTSHORT)
	{
		// get the first argument required
		*uLong = ListData->resval.rint;				
	}	
	else if (IsArgumentString(ListData->restype, nIndex, sMethod)) 
	{		
		// get the first argument required
		*pArg = ListData->resval.rstring;				
		*uLong = 0;
	}
	else
	{	
		return false; 
	}
	return true; 
}
//*****************************************************************************
// 
// Method: FindOptionalStringArgument()
// 
// Purpose: [looks for optional to get the expected string argument from AutoLISP, but does not
//			 prompt the user if not found]
// 
// Parameters:  
//		[nIndex]:  [TODO: Write the description of this parameter]
//		[*pArg]:  [TODO: Write the description of this parameter]
// 
// Returns:	bool
// 
//*****************************************************************************
bool FindOptionalStringArgument(int nIndex, CString *pArg, CString sMethod)
{
	struct resbuf *ListData;

	//ensure AutoLISP has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		return false; 
	}

	for (int i = 0; i < nIndex; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
		
		if (ListData == NULL)
		{
			return false;
		}
	}


	if (ListData->restype == RTSTR)
	{		
		// get the first argument required
		*pArg = ListData->resval.rstring;				
	}
	else
	{	
		return false; 
	}
	return true; 
}
//*****************************************************************************
// 
// Method: GetDoubleArgument()
// 
// Purpose: [gets the expected int argument from AutoLISP]
// 
// Parameters:  
//		[nIndex]:  [0 base index]
//		[*pArg]:  [TODO: Write the description of this parameter]
// 
// Returns:	bool
// 
//*****************************************************************************
bool GetDoubleArgument(int nIndex, double *pArg, CString sMethod)
{
	struct resbuf *ListData;

	//ensure AutoLISP has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod); 			
        return false; 
	}

	for (int i = 0; i < nIndex; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		return false;
	}

	if (ListData->restype == RTSHORT)
	{
		// get the argument required as an int and convert it to a double
		*pArg = (double)ListData->resval.rint;				
		return true;
	}
	else if (IsArgumentDouble(ListData->restype, nIndex, sMethod)) 
	{		
		// get the argument required
		*pArg = ListData->resval.rreal;				
	}
	else
	{	
		// inform the programmer that he did not make the correct call
	   return false; 
	}
	return true; 
}

//*****************************************************************************
// 
// Method: GetIntArgument()
// 
// Purpose: [gets the expected int argument from AutoLISP]
// 
// Parameters:  
//		[nIndex]:  [0 base index]
//		[*pArg]:  [TODO: Write the description of this parameter]
// 
// Returns:	bool
// 
//*****************************************************************************
bool GetIntArgument(int nIndex, int *pArg, CString sMethod)
{
	struct resbuf *ListData;

	//ensure AutoLISP has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod); 			
        return false; 
	}

	for (int i = 0; i < nIndex; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
		return false;

	if (!IsArgumentInt(ListData->restype, nIndex, sMethod))
		return false;
	*pArg = ListData->resval.rint;		
	return true; 
}

//*****************************************************************************
// 
// Method: FindOptionalIntArgument()
// 
// Purpose: [gets the optional int argument from AutoLISP]
// 
// Parameters:  
//		[nIndex]:  [0 base index]
//		[*pArg]:  [TODO: Write the description of this parameter]
// 
// Returns:	bool
// 
//*****************************************************************************
bool FindOptionalIntArgument(int nIndex, int *pArg, CString sMethod)
{
	struct resbuf *ListData;

	//ensure AutoLISP has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		*pArg = -1;
		// inform the programmer that he did not make the correct call
		return false; 
	}

	for (int i = 0; i < nIndex; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
		if (ListData == NULL)
		{
			*pArg = -1;
			return false;
		}
	}

	
	if (ListData->restype == RTSHORT)
	{		
		// get the first argument required
		*pArg = ListData->resval.rint;				
	}
	else if (ListData->restype == RTREAL)
	{		
		// get the first argument required
		*pArg = ListData->resval.rreal;				
	}
	else
	{
		*pArg = -1;
		// inform the programmer that he did not make the correct call
		return false; 
	}
	return true; 
}

//*****************************************************************************
// 
// Method: FindOptionalDoubleArgument()
// 
// Purpose: [gets the optional int argument from AutoLISP]
// 
// Parameters:  
//		[nIndex]:  [0 base index]
//		[*pArg]:  [TODO: Write the description of this parameter]
// 
// Returns:	bool
// 
//*****************************************************************************
bool FindOptionalDoubleArgument(int nIndex, double *pArg, CString sMethod)
{
	struct resbuf *ListData;

	//ensure AutoLISP has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		return false; 
	}

	for (int i = 0; i < nIndex; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
		if (ListData == NULL)
		{
			return false;
		}
	}

	
	if (ListData->restype == RTSHORT)
	{		
		// get the first argument required
		*pArg = (double)ListData->resval.rint;				
	}
	else if (ListData->restype == RTREAL)
	{		
		// get the first argument required
		*pArg = ListData->resval.rreal;				
	}
	else
	{
		// inform the programmer that he did not make the correct call
		return false; 
	}
	return true; 
}


//*****************************************************************************
// 
// Method: GetNextString()
// 
// Purpose: [get the next argument assuming it's a string]
// 
// Parameters:  
//		[*ListData]:  [TODO: Write the description of this parameter]
//		[*pArg]:  [TODO: Write the description of this parameter]
// 
// Returns:	bool
// 
//*****************************************************************************
bool GetNextString(struct resbuf *ListData, CString *pArg, int nIndex, CString sMethod)
{
	// first iterate forward to the next required argument
	ListData = ListData->rbnext;
	if (ListData == NULL)
	{
		return false;
	}

	if (IsArgumentString(ListData->restype, nIndex, sMethod)) 
	{		
		// get the first argument required
		*pArg = ListData->resval.rstring;				
	}
	else
	{	
		return false; 
	}

	return true;
}

//*****************************************************************************
// 
// Method: GetControlPointer()
// 
// Purpose: [gets a pointer to the requested CWnd control]
// 
// Parameters: none
// 
// Returns:	CWnd *
// 
//*****************************************************************************
CWnd * GetControlPointer(int nControlType, CString sMethod, int *pnArgs)
{
	CString sProject;
	CString sDialogBox;
	CString sControlName;
	CString sItem;
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData;
	CWnd *pControl = NULL;

	ListData = acedGetArgs();

	if (ListData == NULL)
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod); 			
        return NULL; 
	}

	if (ListData->restype == RTNIL)
	{		
		CString sError;
		sError.Format( theWorkspace.LoadResourceString(IDS_METHODVARNAMEINCORRECT), (LPCTSTR)sMethod );
		theWorkspace.DisplayAlert(sError); 
		return NULL; 
	}

	else if (ListData->restype == RTSHORT)
	{	
		pArxObject = (CDclControlObject *)ListData->resval.rint;
	}
	else if (ListData->restype == RTLONG)
	{	
		pArxObject = (CDclControlObject *)ListData->resval.rlong;
	}
	else if (ListData->restype == RTREAL)
	{	
		double dValue = ListData->resval.rreal;
		pArxObject = (CDclControlObject *)((long)dValue);
	}
	else if (ListData->restype == RTSTR)
	{		
		// get the first argument required
		sProject = ListData->resval.rstring;				
	
		// iterate forward to the next required argument
		ListData = ListData->rbnext;

		if (ListData == NULL)
		{
			theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod);
			return NULL;
		}

		if (ListData->restype == RTSTR)
		{		
			// get the argument required
			sDialogBox = ListData->resval.rstring;				
		}
		else
		{
			theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sMethod);
			return NULL;
		}
	
		// iterate forward to the next required argument
		ListData = ListData->rbnext;

		if (ListData == NULL)
		{
			theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod);
			return NULL;
		}

		if (ListData->restype == RTSTR)
		{		
			// get the argument required
			sControlName = ListData->resval.rstring;				
		}
		else
		{
			theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sMethod);
			return NULL;
		}
	
		if (pnArgs != NULL)
			*pnArgs = 3;

		return GetRequestedControlWnd(sProject, sDialogBox, sControlName);
	}
	else
	{		
		return NULL; 
	}

	if (pnArgs != NULL)
		*pnArgs = 1;
	
	
	if (pArxObject == NULL)
	{
		return NULL;
	}
	
	return pArxObject->GetWindow();
}

CWnd * GetControlPointer(ControlType nControlType, CString sMethod, CDclFormObject *pDclObject)
{
	CString sProject;
	CString sDialogBox;
	CString sControlName;
	CString sItem;
	struct resbuf *ListData;

	// call the method to get 2 expected string arguments from AutoLISP
	ListData = GetProjectAndFormName(&sProject, &sDialogBox, sMethod);
	
	if (ListData == NULL)
		return NULL;

	if (!GetNextString(ListData, &sControlName, 2, sMethod))
	{
		theWorkspace.DisplayAlert(CString(ErrorNoControlNameArgument) + sMethod);
		return NULL;
	}

	return GetRequestedControlWnd(sProject, sDialogBox, sControlName, nControlType);
}

//*****************************************************************************
// 
// Method: GetControlArxObject()
// 
// Purpose: [gets a pointer to the arx control object requested]
// 
// Parameters: none
// 
// Returns:	CDclControlObject *
// 
//*****************************************************************************
CDclControlObject * GetControlArxObject(CString sMethod, int *pnArgs)
{
	CString sProject;
	CString sDialogBox;
	CString sControlName;
	CString sItem;
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData;
	CWnd *pControl = NULL;

	ListData = acedGetArgs();

	if (ListData == NULL)
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod); 			
        return NULL; 
	}

	if (ListData->restype == RTNIL)
	{		
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString("The control variable argument passed to ") + sMethod + " was NIL."); 			
        return NULL; 
	}

	else if (ListData->restype == RTSHORT)
	{	
		pArxObject = (CDclControlObject *)ListData->resval.rint;
	}
	else if (ListData->restype == RTLONG)
	{	
		pArxObject = (CDclControlObject *)ListData->resval.rlong;
	}
	else if (ListData->restype == RTREAL)
	{	
		double dValue = ListData->resval.rreal;
		pArxObject = (CDclControlObject *)((long)dValue);
	}
	else if (ListData->restype == RTSTR)
	{		
		// get the first argument required
		sProject = ListData->resval.rstring;				
	
		// iterate forward to the next required argument
		ListData = ListData->rbnext;

		if (ListData == NULL)
		{
			theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod);
			return NULL;
		}

		if (ListData->restype == RTSTR)
		{		
			// get the argument required
			sDialogBox = ListData->resval.rstring;				
		}
		else
		{
			theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sMethod);
			return NULL;
		}
	
		// iterate forward to the next required argument
		ListData = ListData->rbnext;

		if (ListData == NULL)
		{
			theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod);
			return NULL;
		}

		if (ListData->restype == RTSTR)
		{		
			// get the argument required
			sControlName = ListData->resval.rstring;				
		}
		else
		{
			theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sMethod);
			return NULL;
		}
	
		if (pnArgs != NULL)
			*pnArgs = 3;
		CDclControlObject *pArxObject = GetRequestedDclControl(sProject, sDialogBox, sControlName);

		if (pArxObject == NULL)
		{
			return NULL;
		}
		
		return pArxObject;
	}
	else
	{		
		return NULL; 
	}

	if (pnArgs != NULL)
		*pnArgs = 1;
	
	
	if (pArxObject == NULL)
	{
		return NULL;
	}
	
	return pArxObject;
}

//*****************************************************************************
// 
// Method: GetPropertyArgument()
// 
// Purpose: [gets the argument at the requested index and returns a CPropertyObject]
// 
// Parameters:  
//		[nIndex]:  [0 based argument index]
// 
// Returns:	RefCountedPtr< CPropertyObject > 
// 
//*****************************************************************************
RefCountedPtr< CPropertyObject > GetPropertyArgument(int nIndex, CString sMethod)
{
	struct resbuf *ListData;

	//ensure AutoLISP has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod); 			
        return false; 
	}

	for (int i = 0; i < nIndex; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		return NULL;
	}
	if (ListData->restype == RTLB)
	{
		return NULL;
	}
	
	RefCountedPtr< CPropertyObject > pProperty;

	switch (ListData->restype)
	{	
		case RTREAL:
		case RTANG:
			{
				pProperty = new CPropertyObject(PropDouble);
				// get the first argument required
				pProperty->SetDoubleValue(ListData->resval.rreal);
				break;
			}
		case RTSHORT:
			{
				pProperty = new CPropertyObject(PropLong);
				// get the first argument required
				pProperty->SetLongValue(ListData->resval.rint);
				break;
			}
		case RTLONG:
			{
				pProperty = new CPropertyObject(PropLong);
				// get the first argument required
				pProperty->SetLongValue(ListData->resval.rlong);
				break;
			}
		case RTSTR:
			{
				pProperty = new CPropertyObject(PropString);
				// get the first argument required
				pProperty->SetStringValue(ListData->resval.rstring);
				break;
			}
		case RTT:
			{
				pProperty = new CPropertyObject(PropBool);
				// get the first argument required
				pProperty->SetBooleanValue(true);				
				break;
			}
		case RTNIL:
			{
				pProperty = new CPropertyObject(PropBool);
				// get the first argument required
				pProperty->SetBooleanValue(false);				
				break;
			}
		default:
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(
					CString(ErrorInappropriateFound)
					+ sMethod); 			
				return NULL; 
				break;
			}		
	}
	//acutRelRb(ListData);
	return pProperty; 
}

bool GetAxPropertyArgument(struct resbuf*& ListData, COleVariant *oleVar, const AxArg& type)
{	
	oleVar->Clear();
	switch (ListData->restype)
	{	
		case RTREAL:
		case RTANG:
		case RTORINT:
			{
				oleVar->vt = VT_R8;
				oleVar->dblVal = ListData->resval.rreal;
				ListData = ListData->rbnext;
				break;
			}
		case RTSHORT:
			{
				oleVar->vt = VT_I2;
				oleVar->iVal = ListData->resval.rint;
				if (type.vt == VT_DISPATCH)
				{
					oleVar->vt = VT_DISPATCH;
					COleDispatchDriver *pDisp = (COleDispatchDriver *)ListData->resval.rint;
					oleVar->pdispVal = pDisp->m_lpDispatch;
					pDisp->m_lpDispatch = NULL;
				}
				ListData = ListData->rbnext;
				break;
			}
		case RTLONG:
			{
				if (type.vt == VT_DISPATCH)
				{
					oleVar->vt = VT_DISPATCH;
					COleDispatchDriver *pDisp = (COleDispatchDriver *)ListData->resval.rlong;
					oleVar->pdispVal = pDisp->m_lpDispatch;
					pDisp->m_lpDispatch = NULL;
				}
				else
				{
					oleVar->vt = VT_I4;
					oleVar->lVal = ListData->resval.rlong;
				}
				ListData = ListData->rbnext;
				break;
			}
		case RTSTR:
			{
				//oleVar->vt = VT_LPSTR;
				*oleVar = ListData->resval.rstring;				
				ListData = ListData->rbnext;
				break;
			}
		case RTT:
			{
				oleVar->vt = VT_BOOL;
				oleVar->boolVal = VARIANT_TRUE;				
				ListData = ListData->rbnext;
				break;
			}
		case RTNIL:
			{
				if (type.vt == VT_VARIANT)
				{
					oleVar->vt = VT_VARIANT|VT_BYREF;
					oleVar->pvarVal = NULL;
				}
				else
				{
					oleVar->vt = VT_BOOL;
					oleVar->boolVal = VARIANT_FALSE;				
				}
				ListData = ListData->rbnext;
				break;
			}
			// must be a list
		case RT3DPOINT:
		case RTPOINT:
			{
				switch (type.vt)
				{
				case VT_DISPATCH:
				case VT_UNKNOWN:
					{
						oleVar->vt = VT_DISPATCH;
						COleDispatchDriver *pDisp = (COleDispatchDriver *)((ULONG)ListData->resval.rpoint[1]);
						oleVar->pdispVal = pDisp->m_lpDispatch;
						pDisp->m_lpDispatch = NULL;
						ListData = ListData->rbnext;
						break;
					}
				case VT_DISPATCH|VT_BYREF:
				case VT_UNKNOWN|VT_BYREF:
					{
						oleVar->vt = VT_DISPATCH|VT_BYREF;
						COleDispatchDriver *pDisp = NULL;
						pDisp = (COleDispatchDriver *)((ULONG)ListData->resval.rpoint[1]);
						oleVar->ppdispVal = &pDisp->m_lpDispatch;
						pDisp->m_lpDispatch = NULL;
						ListData = ListData->rbnext;
						break;
					}
				case VT_DATE:
					{
						COleDateTime Date;
						Date.SetDate(
							ListData->resval.rpoint[0],
							ListData->resval.rpoint[1],
							ListData->resval.rpoint[2]);
						oleVar->vt = VT_DATE;
						oleVar->date = Date;
						ListData = ListData->rbnext;
						break;
					}
				case VT_UI4:
					{
						oleVar->vt = VT_UI4;
						oleVar->ulVal = RGB(ListData->resval.rpoint[0], ListData->resval.rpoint[1], ListData->resval.rpoint[2]);
						ListData = ListData->rbnext;
						break;
					}
				}
				break;
			}
		default:
			{
				return false; 
				break;
			}		
	}
	bool bReturn = (type.vt == VT_VARIANT || S_OK == VariantChangeType( oleVar, oleVar, 0, type.vt ));

/*
	// set to converted as false for default

	// here we need to check the COleVariant to ensure that it is set to the correct type and that the 
	// user did not pass in a value that is not allowed.
	switch(varType)
	{
		case VT_UI1:
	
			if (oleVar->vt == VT_BOOL)
			{
				oleVar->vt = VT_UI1;
				oleVar->bVal = oleVar->boolVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_UI1;
				if (oleVar->iVal == 0)
					oleVar->bVal = TRUE;
				else
					oleVar->bVal = FALSE;
				bReturn = true;
			}
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_UI1;
				if (oleVar->lVal == 0)
					oleVar->bVal = TRUE;
				else
					oleVar->bVal = FALSE;
				bReturn = true;
			}			
			break;

		case VT_I2:
			if (oleVar->vt == VT_I2)
				bReturn = true;
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_I2;
				oleVar->iVal = oleVar->lVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_R8)
			{
				// convert to an float
				oleVar->vt = VT_I2;
				oleVar->iVal = oleVar->dblVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_R4)
			{
				// convert to an float
				oleVar->vt = VT_I2;
				oleVar->iVal = oleVar->dblVal;
				bReturn = true;
			}
			break;
		case VT_I4:
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_I4;
				oleVar->lVal = oleVar->iVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I4)
				bReturn = true;
			if (oleVar->vt == VT_R8)
			{
				// convert to an float
				oleVar->vt = VT_I4;
				oleVar->lVal = oleVar->dblVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_R4)
			{
				// convert to an float
				oleVar->vt = VT_I4;
				oleVar->lVal = oleVar->dblVal;
				bReturn = true;
			}
			break;
		case VT_UI4:
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_I4;
				oleVar->lVal = oleVar->iVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I4)
				bReturn = true;
			if (oleVar->vt == VT_R8)
			{
				// convert to an float
				oleVar->vt = VT_I4;
				oleVar->lVal = oleVar->dblVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_R4)
			{
				// convert to an float
				oleVar->vt = VT_I4;
				oleVar->lVal = oleVar->dblVal;
				bReturn = true;
			}
			break;
		case VT_R4:
			if (oleVar->vt == VT_R8)
			{
				// convert to an float
				oleVar->vt = VT_R4;
				oleVar->fltVal = oleVar->dblVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_R4)
				bReturn = true;
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_R4;
				oleVar->fltVal = oleVar->iVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_R4;
				oleVar->fltVal = oleVar->lVal;
				bReturn = true;
			}
			break;
		case VT_R8:
			if (oleVar->vt == VT_R4)
			{
				// convert to an float
				oleVar->vt = VT_R8;
				oleVar->dblVal = oleVar->fltVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_R8)
				bReturn = true;
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_R8;
				oleVar->dblVal = oleVar->iVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_R8;
				oleVar->dblVal = oleVar->lVal;
				bReturn = true;
			}
			break;
		case VT_DATE:
			{
			return true;
			break;
			}
		case VT_CY:
			{
			// convert the values to the correct type
			if (oleVar->vt == RTREAL || oleVar->vt == RTANG)
			{
				oleVar->vt = VT_CY;
				oleVar->cyVal.Lo = oleVar->dblVal;
				oleVar->cyVal.Hi = 0;
				oleVar->cyVal.int64 = oleVar->cyVal.Lo;
				
				bReturn = true;
			}
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_CY;
				oleVar->cyVal.Lo = oleVar->iVal;
				oleVar->cyVal.Hi = 0;
				oleVar->cyVal.int64 = oleVar->cyVal.Lo ;
				
				bReturn = true;
			}
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_CY;
				oleVar->cyVal.Lo = oleVar->lVal;
				oleVar->cyVal.Hi = 0;
				oleVar->cyVal.int64 = oleVar->cyVal.Lo ;
				bReturn = true;
			}
			if (oleVar->vt == VT_R4)
			{
				oleVar->vt = VT_CY;
				oleVar->cyVal.Lo = oleVar->fltVal;
				oleVar->cyVal.Hi = 0;
				oleVar->cyVal.int64 = oleVar->cyVal.Lo ;
				bReturn = true;
			}
			if (oleVar->vt == VT_R8)
			{
				oleVar->vt = VT_CY;
				oleVar->cyVal.Lo = oleVar->dblVal;
				oleVar->cyVal.Hi = 0;
				oleVar->cyVal.int64 = oleVar->cyVal.Lo ;
				bReturn = true;
			}
			break;
			}
		case VT_BSTR:
#if !defined(_UNICODE) && !defined(OLE2ANSI)
		case VT_BSTRA:
#endif
			{
			bReturn = true;
			break;
			}
		case VT_DISPATCH:
			return true;
			break;
		case VT_ERROR:
			{
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_ERROR;
				oleVar->scode = oleVar->iVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_ERROR;
				oleVar->scode = oleVar->lVal;
				bReturn = true;
			}			
			break;
			}
		case VT_BOOL:			
			{
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_BOOL;
				if (oleVar->iVal == 0)
					oleVar->boolVal = FALSE;
				else
					oleVar->boolVal = TRUE;
				bReturn = true;
			}
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_BOOL;
				if (oleVar->lVal == 0)
					oleVar->boolVal = FALSE;
				else
					oleVar->boolVal = TRUE;
				bReturn = true;
			}
			if (oleVar->vt = VT_BOOL)			
				bReturn = true;
			break;
			}
		case VT_VARIANT:
			{			
			bReturn = true;
			break;
			}
		case VT_UNKNOWN:
			acedAlert (_T("VT_UNKNOWN To Do"));
			return false;
			break;

		case VT_UI1|VT_BYREF:
			if (oleVar->vt == VT_BOOL)
			{
				oleVar->vt = VT_UI1|VT_BYREF;
				oleVar->bVal = oleVar->boolVal;
				oleVar->pbVal = &oleVar->bVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_UI1|VT_BYREF;
				oleVar->bVal = oleVar->iVal;
				oleVar->pbVal = &oleVar->bVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_UI1|VT_BYREF;
				oleVar->bVal = oleVar->lVal;
				oleVar->pbVal = &oleVar->bVal;
				bReturn = true;
			}			
			break;
		case VT_I2|VT_BYREF:			
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_I2|VT_BYREF;
				oleVar->piVal = &oleVar->iVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_I2|VT_BYREF;
				oleVar->iVal = oleVar->lVal;
				oleVar->piVal = &oleVar->iVal;
				bReturn = true;
			}
			break;		
		case VT_I4|VT_BYREF:
			{
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_I4|VT_BYREF;
				oleVar->plVal = &oleVar->lVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_I4|VT_BYREF;
				oleVar->lVal = oleVar->iVal;
				oleVar->plVal = &oleVar->lVal;
				bReturn = true;
			}
			
			break;
			}
		case VT_R4|VT_BYREF:
			if (oleVar->vt == VT_R4)
			{
				oleVar->vt = VT_R4|VT_BYREF;
				oleVar->pfltVal = &oleVar->fltVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_R8)
			{
				oleVar->vt = VT_R4|VT_BYREF;
				oleVar->fltVal = oleVar->dblVal;
				oleVar->pfltVal = &oleVar->fltVal;
				bReturn = true;
			}
			break;
		case VT_R8|VT_BYREF:
			if (oleVar->vt == VT_R4)
			{
				oleVar->vt = VT_R4|VT_BYREF;
				oleVar->dblVal = oleVar->fltVal;
				oleVar->pdblVal = &oleVar->dblVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_R8)
			{
				oleVar->vt = VT_R8|VT_BYREF;
				oleVar->pdblVal = &oleVar->dblVal;
				bReturn = true;
			}
			
			break;
		case VT_DATE|VT_BYREF:
			{			
			return true;
			break;
			}
		case VT_CY|VT_BYREF:
			{
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_CY|VT_BYREF;
				oleVar->cyVal.Lo = oleVar->lVal;
				oleVar->pcyVal = &oleVar->cyVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_CY|VT_BYREF;
				oleVar->cyVal.Lo = oleVar->iVal;
				oleVar->pcyVal = &oleVar->cyVal;
				bReturn = true;
			}
			
			break;
			}
		case VT_BSTR|VT_BYREF:
			{
			if (oleVar->vt == VT_BSTR)
			{
				oleVar->vt = VT_BSTR|VT_BYREF;
				oleVar->pbstrVal = &oleVar->bstrVal;
				bReturn = true;
			}
			break;
			}
		case VT_DISPATCH|VT_BYREF:
			acedAlert (_T("VT_DISPATCH|VT_BYREF To Do"));
			return false;
			//varGet.ppdispVal = argList->m_Variant[i].ppdispVal;
			break;
		case VT_ERROR|VT_BYREF:
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_ERROR|VT_BYREF;
				oleVar->scode = oleVar->iVal;
				oleVar->pscode = &oleVar->scode;
				bReturn = true;
			}
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_ERROR|VT_BYREF;
				oleVar->scode = oleVar->lVal;
				oleVar->pscode = &oleVar->scode;
				bReturn = true;
			}						
			break;
		case VT_BOOL|VT_BYREF:
			if (oleVar->vt == VT_BOOL)
			{
				oleVar->vt = VT_BOOL|VT_BYREF;
				oleVar->pboolVal = &oleVar->boolVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I2)
			{
				oleVar->vt = VT_BOOL|VT_BYREF;
				oleVar->boolVal  = oleVar->iVal;
				oleVar->pboolVal = &oleVar->boolVal;
				bReturn = true;
			}
			if (oleVar->vt == VT_I4)
			{
				oleVar->vt = VT_BOOL|VT_BYREF;
				oleVar->boolVal  = oleVar->lVal;
				oleVar->pboolVal = &oleVar->boolVal;
				bReturn = true;
			}	
			break;
		case VT_VARIANT|VT_BYREF:
			acedAlert (_T("VT_VARIANT|VT_BYREF To Do"));
			return false;
			//varGet.pvarVal = argList->m_Variant[i].pvarVal;
			break;
		case VT_UNKNOWN|VT_BYREF:
			acedAlert (_T("VT_UNKNOWN|VT_BYREF To Do"));
			return false;
			break;
	}
*/

	if (!bReturn)
	{
		CString sArgErr;
		sArgErr.Format( theWorkspace.LoadResourceString( IDS_ERR_INVALIDARGUMENTTYPE ), VTToString( type.vt ), VTToString( oleVar->vt ) );
		theWorkspace.DisplayAlert( sArgErr );
	}

	return bReturn; 
}

bool GetDateArgument(int nIndex, COleDateTime *pArg, CString sMethod)
{
	struct resbuf *ListData;
	int nYear;
	int nMonth;
	int nDay;

	//ensure AutoLISP has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod); 					 
        return false; 
	}

	for (int i = 0; i < nIndex; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		return false;
	}


	if (ListData->restype == RT3DPOINT) 
	{
		nYear = ListData->resval.rpoint[X];
		nMonth = ListData->resval.rpoint[Y];
		nDay = ListData->resval.rpoint[Z];
	}
	else if (ListData->restype == RTLB) 
	{		
		// advance to the first list item
		ListData = ListData->rbnext;
		if (ListData == NULL)
		{
			// inform the programmer that he did not make the correct call
			theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sMethod);			
			return false; 
		}

		// get the first argument required
		nYear = ListData->resval.rint;

		// advance to the first list item
		ListData = ListData->rbnext;
		if (ListData == NULL)
		{
			// inform the programmer that he did not make the correct call
			theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sMethod);			
			return false; 
		}
		
		// get the argument required
		nMonth = ListData->resval.rint;

		// advance to the first list item
		ListData = ListData->rbnext;
		if (ListData == NULL)
		{
			// inform the programmer that he did not make the correct call
			theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sMethod);			
			return false; 
		}
		
		// get the third argument required
		nDay = ListData->resval.rint;
	}
	else
	{	
		// inform the programmer that he did not make the correct call
		return false; 
	}

	pArg->SetDate(
		nYear,
		nMonth,
		nDay);

	//acutRelRb(ListData);
	return true; 
}

CWnd * GetArgsControlInt(int nControlType, CString sMethod, int &nInt)
{
	int nArgs = 0;
	CWnd *pControl = GetControlPointer(
		nControlType,
		sMethod,
		&nArgs);


	if (pControl == NULL || !GetIntArgument(nArgs, &nInt, sMethod))
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	return pControl;
}

CWnd * GetArgsControlDouble(int nControlType, CString sMethod, double &dDouble)
{
	int nArgs = 0;
	CWnd *pControl = GetControlPointer(
		nControlType,
		sMethod,
		&nArgs);


	if (pControl == NULL || !GetDoubleArgument(nArgs, &dDouble, sMethod))
	{
		// return nil
		acedRetInt(-1);
		return NULL;
	}
	return pControl;
}

CWnd * GetArgsControlString(int nControlType, CString sMethod, CString &sString)
{
	int nArgs = 0;
	CWnd *pControl = GetControlPointer(
		nControlType,
		sMethod,
		&nArgs);


	if (pControl == NULL || !GetStringArgument(nArgs, &sString, sMethod))
	{
		// return nil
		acedRetInt(-1);
		return NULL;
	}
	return pControl;
}


CWnd * GetArgsControlIntString(int nControlType, CString sMethod, int &nInt, CString &sString)
{
	int nArg = 0;
	CWnd *pControl = GetControlPointer(
		nControlType,
		sMethod,
		&nArg);


	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return NULL;
	}
	struct resbuf *ListData;
	CString sNewString;
	
	//ensure AutoLISP has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}


	if (ListData->restype != RTSHORT && ListData->restype != RTLONG) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sMethod);	
		acedRetInt(-1);  return NULL; 
	}

	if (ListData->restype == RTSHORT)
		// get the first argument required
		nInt = ListData->resval.rint;

	if (ListData->restype == RTLONG)
		// get the first argument required
		nInt = ListData->resval.rlong;

	ListData = ListData->rbnext;

	if (ListData->restype != RTSTR) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sMethod);	
		acedRetInt(-1);  return NULL; 
	}

	// get the first argument required
	sString = ListData->resval.rstring;


	return pControl;
}

CWnd * GetArgsControlStringString(int nControlType, CString sMethod, CString &sString, CString &sString2)
{
	int nArg = 0;
	CWnd *pControl = GetControlPointer(
		nControlType,
		sMethod,
		&nArg);


	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return NULL;
	}
	struct resbuf *ListData;
	CString sNewString;
	
	//ensure AutoLISP has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (ListData->restype != RTSTR) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sMethod);	
		acedRetInt(-1);  return NULL; 
	}

	// get the first argument required
	sString = ListData->resval.rstring;

	ListData = ListData->rbnext;

	if (ListData->restype != RTSTR) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sMethod);	
		acedRetInt(-1);  return NULL; 
	}

	// get the first argument required
	sString2 = ListData->resval.rstring;


	return pControl;
}


CWnd * GetArgsControlIntInt(int nControlType, CString sMethod, int &nInt, int &nInt2)
{
	int nArg = 0;
	CWnd *pControl = GetControlPointer(
		nControlType,
		sMethod,
		&nArg);


	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return NULL;
	}
	struct resbuf *ListData;
	CString sNewString;
	
	//ensure AutoLISP has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (ListData->restype != RTSHORT && ListData->restype != RTLONG) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sMethod);	
		acedRetInt(-1);  return NULL; 
	}

	if (ListData->restype == RTSHORT)
		// get the first argument required
		nInt = ListData->resval.rint;

	if (ListData->restype == RTLONG)
		// get the first argument required
		nInt = ListData->resval.rlong;

	ListData = ListData->rbnext;

	if (ListData->restype != RTSHORT && ListData->restype != RTLONG && 
		ListData->restype != RTT && ListData->restype != RTNIL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sMethod);	
		acedRetInt(-1);  return NULL; 
	}

	if (ListData->restype == RTSHORT)
		// get the first argument required
		nInt2 = ListData->resval.rint;

	if (ListData->restype == RTLONG)
		// get the first argument required
		nInt2 = ListData->resval.rlong;

	if (ListData->restype == RTT)
		// get the first argument required
		nInt2 = 1;

	if (ListData->restype == RTNIL)
		// get the first argument required
		nInt2 = 0;

	return pControl;
}


CWnd * GetArgsControlIntIntInt(int nControlType, CString sMethod, int &nInt, int &nInt2, int &nInt3)
{
	int nArg = 0;
	CWnd *pControl = GetControlPointer(
		nControlType,
		sMethod,
		&nArg);


	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return NULL;
	}
	struct resbuf *ListData;
	CString sNewString;
	
	//ensure AutoLISP has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (ListData->restype != RTSHORT && ListData->restype != RTLONG) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sMethod);	
		acedRetInt(-1);  return NULL; 
	}

	if (ListData->restype == RTSHORT)
		// get the first argument required
		nInt = ListData->resval.rint;

	if (ListData->restype == RTLONG)
		// get the first argument required
		nInt = ListData->resval.rlong;

	ListData = ListData->rbnext;

	if (ListData->restype != RTSHORT && ListData->restype != RTLONG && 
		ListData->restype != RTT && ListData->restype != RTNIL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sMethod);	
		acedRetInt(-1);  return NULL; 
	}

	if (ListData->restype == RTSHORT)
		// get the first argument required
		nInt2 = ListData->resval.rint;

	if (ListData->restype == RTLONG)
		// get the first argument required
		nInt2 = ListData->resval.rlong;

	if (ListData->restype == RTT)
		// get the first argument required
		nInt2 = 1;

	if (ListData->restype == RTNIL)
		// get the first argument required
		nInt2 = 0;

		ListData = ListData->rbnext;

	if (ListData->restype != RTSHORT && ListData->restype != RTLONG && 
		ListData->restype != RTT && ListData->restype != RTNIL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sMethod);	
		acedRetInt(-1);  return NULL; 
	}

	if (ListData->restype == RTSHORT)
		// get the first argument required
		nInt3 = ListData->resval.rint;

	if (ListData->restype == RTLONG)
		// get the first argument required
		nInt3 = ListData->resval.rlong;

	if (ListData->restype == RTT)
		// get the first argument required
		nInt3 = 1;

	if (ListData->restype == RTNIL)
		// get the first argument required
		nInt3 = 0;



	return pControl;
}

CWnd * GetArgsControlStringIntInt(int nControlType, CString sMethod, CString &sString, int &nInt, int &nInt2)
{
	int nArg = 0;
	CWnd *pControl = GetControlPointer(
		nControlType,
		sMethod,
		&nArg);


	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return NULL;
	}
	struct resbuf *ListData;
	CString sNewString;
	
	//ensure AutoLISP has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (ListData->restype != RTSTR) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sMethod);	
		acedRetInt(-1);  return NULL; 
	}

	// get the first argument required
	sString = ListData->resval.rstring;

	ListData = ListData->rbnext;

	if (ListData->restype != RTSHORT && ListData->restype != RTLONG && 
		ListData->restype != RTT && ListData->restype != RTNIL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sMethod);	
		acedRetInt(-1);  return NULL; 
	}

	if (ListData->restype == RTSHORT)
		// get the first argument required
		nInt = ListData->resval.rint;

	if (ListData->restype == RTLONG)
		// get the first argument required
		nInt = ListData->resval.rlong;

	
	if (ListData->restype == RTT)
		// get the first argument required
		nInt = 1;

	if (ListData->restype == RTNIL)
		// get the first argument required
		nInt = 0;

	ListData = ListData->rbnext;

	if (ListData->restype != RTSHORT && ListData->restype != RTLONG && 
		ListData->restype != RTT && ListData->restype != RTNIL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sMethod);	
		acedRetInt(-1);  return NULL; 
	}

	if (ListData->restype == RTSHORT)
		// get the first argument required
		nInt2 = ListData->resval.rint;

	if (ListData->restype == RTLONG)
		// get the first argument required
		nInt2 = ListData->resval.rlong;

	
	if (ListData->restype == RTT)
		// get the first argument required
		nInt2 = 1;

	if (ListData->restype == RTNIL)
		// get the first argument required
		nInt2 = 0;

	return pControl;
}

struct resbuf *getLispTargetInput(LPCTSTR sMethod, CDclControlObject *&pArg)
{
	CString sProject;
	CString sDialogBox;
	CString sControlName;
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData;
	
	ListData = acedGetArgs();

	if (ListData == NULL)
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod); 			
        return ListData; 
	}

	if (ListData->restype == RTNIL)
	{		
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString("The control variable argument passed to ") + sMethod + " was NIL."); 			
        return ListData; 
	}

	else if (ListData->restype == RTSHORT)
	{	
		pArg = (CDclControlObject *)ListData->resval.rint;
		return ListData;
	}
	else if (ListData->restype == RTLONG)
	{	
		pArg = (CDclControlObject *)ListData->resval.rlong;
		return ListData;
	}
	else if (ListData->restype == RTREAL)
	{	
		double dValue = ListData->resval.rreal;
		pArg = (CDclControlObject *)((long)dValue);
		return ListData;
	}
	else if (ListData->restype == RTSTR)
	{		
		// get the first argument required
		sProject = ListData->resval.rstring;				
	
		// iterate forward to the next required argument
		ListData = ListData->rbnext;

		if (ListData == NULL)
		{
			theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod);
			return ListData;
		}

		if (ListData->restype == RTSTR)
		{		
			// get the argument required
			sDialogBox = ListData->resval.rstring;				
		}
		else
		{
			theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sMethod);
			return ListData;
		}
	
		// iterate forward to the next required argument
		ListData = ListData->rbnext;

		if (ListData == NULL)
		{
			theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sMethod);
			return ListData;
		}

		if (ListData->restype == RTSTR)
		{		
			// get the argument required
			sControlName = ListData->resval.rstring;				
		}
		else
		{
			theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sMethod);
			return ListData;
		}
	
		CDclControlObject *pArxObject = GetRequestedDclControl(sProject, sDialogBox, sControlName);

		if (pArxObject == NULL)
		{
			return ListData;
		}
		pArg = pArxObject;
		return ListData;
	}

	return ListData; 	
}

void GetLispStrInput(struct resbuf *ListData, CString &sArg1)
{
	// get the argument
	if (ListData->restype == RTSTR)
	{		
		// get the first argument required
		sArg1 = ListData->resval.rstring;					
	}

}

void GetLispIntInput(struct resbuf *ListData, int &nArg1)
{
	// get the first argument
	if (ListData->restype == RTSHORT)
	{		
		// get the first argument required
		nArg1 = ListData->resval.rint;		
	}
	else if (ListData->restype == RTREAL ||
			 ListData->restype == RTORINT ||
			 ListData->restype == RTANG)
	{		
		// get the first argument required
		nArg1 = (int)ListData->resval.rreal;				
	}	
	else if (ListData->restype == RTLONG)
	{		
		// get the first argument required
		nArg1 = (int)ListData->resval.rlong;				
	}
	else if (ListData->restype == RTT)
	{
		nArg1 = 1;
	}
	else if (ListData->restype == RTNIL)
	{
		nArg1 = 0;
	}
}

void GetLispBoolInput(struct resbuf *ListData, bool &bArg1)
{
	// get the first argument
	if (ListData->restype == RTSHORT)
	{		
		// get the first argument required
		bArg1 = ListData->resval.rint == 1;		
	}
	else if (ListData->restype == RTREAL ||
			 ListData->restype == RTORINT ||
			 ListData->restype == RTANG)
	{		
		// get the first argument required
		bArg1 = ListData->resval.rreal == 1.0;				
	}	
	else if (ListData->restype == RTLONG)
	{		
		// get the first argument required
		bArg1 = ListData->resval.rlong == 1;				
	}
	else if (ListData->restype == RTT)
	{
		bArg1 = true;
	}
	else if (ListData->restype == RTNIL)
	{
		bArg1 = false;
	}
}

void GetLispRealInput(struct resbuf *ListData, double &dArg1)
{
	// get the first argument
	if (ListData->restype == RTSHORT)
	{		
		// get the first argument required
		dArg1 = ListData->resval.rint;		
	}
	else if (ListData->restype == RTREAL ||
			 ListData->restype == RTORINT ||
			 ListData->restype == RTANG)
	{		
		// get the first argument required
		dArg1 = ListData->resval.rreal;				
	}	
	else if (ListData->restype == RTLONG)
	{		
		// get the first argument required
		dArg1 = ListData->resval.rlong;				
	}
	else if (ListData->restype == RTT)
	{
		dArg1 = 1;
	}
	else if (ListData->restype == RTNIL)
	{
		dArg1 = 0;
	}
}
void GetLispLongInput(struct resbuf *ListData, long &lArg1)
{
	// get the first argument
	if (ListData->restype == RTSHORT)
	{		
		// get the first argument required
		lArg1 = (long)ListData->resval.rint;		
	}
	else if (ListData->restype == RTREAL ||
			 ListData->restype == RTORINT ||
			 ListData->restype == RTANG)
	{		
		// get the first argument required
		lArg1 = (long)ListData->resval.rreal;				
	}	
	else if (ListData->restype == RTLONG)
	{		
		// get the first argument required
		lArg1 = ListData->resval.rlong;				
	}
	else if (ListData->restype == RTT)
	{
		lArg1 = 1;
	}
	else if (ListData->restype == RTNIL)
	{
		lArg1 = 0;
	}
}

//*****************************************************************************
// 
// Method: GetLispInput()
// 
// Purpose: [gets the lisp input data]
// 
// Parameters: none
// 
// Returns:	CDclControlObject *
// 
//*****************************************************************************
CDclControlObject * GetLispInput(LPCTSTR sMethod, CString &sArg1, CString &sArg2)
{
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sMethod, pArxObject);

	if (pArxObject == NULL)
	{
		return NULL;
	}

	ListData = ListData->rbnext;

	// get the first argument
	GetLispStrInput(ListData, sArg1);

	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}

	// get the argument
	GetLispStrInput(ListData, sArg2);

	return pArxObject;
}
CDclControlObject * GetLispInput(LPCTSTR sMethod, CPoint &Arg1)
{
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sMethod, pArxObject);

	if (pArxObject == NULL)
	{
		return NULL;
	}

	ListData = ListData->rbnext;

	if (ListData == NULL)
		return pArxObject;

	if (ListData->restype == RT3DPOINT)
	{
		Arg1.x = ListData->resval.rpoint[X];
		Arg1.y = ListData->resval.rpoint[Y];
	}
	else if (ListData->restype == RTPOINT)
	{
		Arg1.x = ListData->resval.rpoint[X];
		Arg1.y = ListData->resval.rpoint[Y];
	}

	return pArxObject;
}
CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, CString &sArg2)
{
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sMethod, pArxObject);

	if (pArxObject == NULL)
	{
		return NULL;
	}

	ListData = ListData->rbnext;

	// get the first argument
	GetLispIntInput(ListData, nArg1);

	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}

	// get the argument
	GetLispStrInput(ListData, sArg2);

	return pArxObject;
}

//*****************************************************************************
// 
// Method: GetLispInput()
// 
// Purpose: [gets the lisp input data]
// 
// Parameters: none
// 
// Returns:	CDclControlObject *
// 
//*****************************************************************************
CDclControlObject * GetLispInput(LPCTSTR sMethod, int nArg1, CString &sArg2, CString &sArg3)
{
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sMethod, pArxObject);

	if (pArxObject == NULL)
	{
		return NULL;
	}

	ListData = ListData->rbnext;

	// get the first argument
	GetLispIntInput(ListData, nArg1);

	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}

	// get the first argument
	GetLispStrInput(ListData, sArg2);

	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}

	// get the argument
	GetLispStrInput(ListData, sArg3);

	return pArxObject;
}
CDclControlObject * GetLispInput(LPCTSTR sMethod, CString &sArg1)
{
	CString sEmpty;
	return GetLispInput(sMethod, sArg1, sEmpty);
}

CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2)
{
	int nArg3;
	return GetLispInput(sMethod, nArg1, nArg2, nArg3);
}

CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1)
{
	int nArg2;
	return GetLispInput(sMethod, nArg1, nArg2);

}

CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2, int &nArg3)
{
	int nArg4;
	return GetLispInput(sMethod, nArg1, nArg2, nArg3, nArg4);
}

CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2, int &nArg3, int &nArg4)
{
	int nArg5;
	return GetLispInput(sMethod, nArg1, nArg2, nArg3, nArg4, nArg5);
}

CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, long &lArg2)
{
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sMethod, pArxObject);

	if (pArxObject == NULL)
	{
		return NULL;
	}

	ListData = ListData->rbnext;

	// get the first argument
	GetLispIntInput(ListData, nArg1);
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}

	// get the argument
	GetLispLongInput(ListData, lArg2);
	
	return pArxObject;
	
}

CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2, int &nArg3, int &nArg4, int &nArg5)
{
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sMethod, pArxObject);

	if (pArxObject == NULL)
	{
		return NULL;
	}

	ListData = ListData->rbnext;

	// get the first argument
	GetLispIntInput(ListData, nArg1);
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}

	// get the argument
	GetLispIntInput(ListData, nArg2);
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}
	
	// get the argument
	GetLispIntInput(ListData, nArg3);

	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}
	
	// get the argument
	GetLispIntInput(ListData, nArg4);

	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}
	
	// get the argument
	GetLispIntInput(ListData, nArg5);

	return pArxObject;
}

CDclControlObject * GetLispInput(LPCTSTR sMethod, CString &sArg1, int &nArg2, int &nArg3, int &nArg4, int &nArg5)
{
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sMethod, pArxObject);

	if (pArxObject == NULL)
	{
		return NULL;
	}

	ListData = ListData->rbnext;

	// get the first argument
	GetLispStrInput(ListData, sArg1);
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}

	// get the argument
	GetLispIntInput(ListData, nArg2);
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}
	
	// get the argument
	GetLispIntInput(ListData, nArg3);

	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}
	
	// get the argument
	GetLispIntInput(ListData, nArg4);

	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}

	// get the argument
	GetLispIntInput(ListData, nArg5);

	return pArxObject;
}
CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2, int &nArg3, int &nArg4, int &nArg5, CString &sOptionalString)
{
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sMethod, pArxObject);

	if (pArxObject == NULL)
	{
		return NULL;
	}

	ListData = ListData->rbnext;

	// get the first argument
	GetLispIntInput(ListData, nArg1);
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}

	// get the argument
	GetLispIntInput(ListData, nArg2);
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}
	
	// get the argument
	GetLispIntInput(ListData, nArg3);

	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}
	
	if (ListData->restype == RTSTR)
	{
		// get the argument
		GetLispStrInput(ListData, sOptionalString);
		return pArxObject;
	}
	// get the argument
	GetLispIntInput(ListData, nArg4);

	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}
	if (ListData->restype == RTSTR)
	{
		// get the argument
		GetLispStrInput(ListData, sOptionalString);
		return pArxObject;
	}
	
	// get the argument
	GetLispIntInput(ListData, nArg5);

	return pArxObject;
}

CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2, CString &sArg3)
{
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sMethod, pArxObject);

	if (pArxObject == NULL)
	{
		return NULL;
	}

	ListData = ListData->rbnext;

	// get the first argument
	GetLispIntInput(ListData, nArg1);

	// iterate forward to the next required argument
	ListData = ListData->rbnext;
	
	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}

	// get the argument
	GetLispIntInput(ListData, nArg2);
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}

	// get the third argument
	GetLispStrInput(ListData, sArg3);

	return pArxObject;
}


CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2, CArray<int, int> *pIntArray, CStringArray *pStrings)
{
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sMethod, pArxObject);

	if (pArxObject == NULL)
	{
		return NULL;
	}

	ListData = ListData->rbnext;

	// get the first argument
	GetLispIntInput(ListData, nArg1);
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}

	// get the argument
	GetLispIntInput(ListData, nArg2);
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	// if the argument is missing
	if (ListData == NULL)
	{
		return pArxObject;
	}
	
	if (ListData->restype == RTLB)
	{	
		while (ListData != NULL)
		{
			if (ListData->restype == RTSTR)
			{
				CString sText;
				// get the argument
				GetLispStrInput(ListData, sText);
				
				pStrings->Add(sText);
			}
			else if (ListData->restype == RTSHORT ||
				ListData->restype == RTLONG ||
				ListData->restype == RTREAL ||
				ListData->restype == RTANG ||
				ListData->restype == RTORINT)
			{
				int nArg;
				// get the argument
				GetLispIntInput(ListData, nArg);

				if (pIntArray != NULL)
					pIntArray->Add(nArg);
			}

			// iterate forward to the next required argument
			ListData = ListData->rbnext;
		}
		return pArxObject;
	}
	
	return pArxObject;
}


bool isPairArgument(struct resbuf* args) {
	return (args->rbnext != NULL 
		&& args->rbnext->rbnext != NULL 
		&& args->rbnext->rbnext->rbnext != NULL 
		&& args->restype == RTLB 
		&& args->rbnext->rbnext->rbnext->restype == RTDOTE);
}
bool getStringArgument(struct resbuf* &args, CString &str) {
	if (args == NULL || args->restype != RTSTR) return false;
	str = args->resval.rstring;
	args = args->rbnext;
	return true;
};
bool getCharStrArgument(struct resbuf* &args, char* str) {
	if (args == NULL || args->restype != RTSTR) return false;
	int len = _tcslen(args->resval.rstring);
	str = new char[len + 1];
	int i;
	for (i = 0; i < len; i++) {
		str[i] = (char)(args->resval.rstring[i]);
	}
	str[i] = '\0';
	args = args->rbnext;
	return true;
};
bool getDoubleArgument(struct resbuf* &args, double &d) {
	if (args == NULL || args->restype != RTREAL) {
		//If it is not a double, try it as an int. 
		int temp;
		if (!getIntArgument(args, temp)) return false;
		d = (double)temp;
		return true;
	}
	d = args->resval.rreal;
	args = args->rbnext;
	return true;
}
bool getIntArgument(struct resbuf* &args, int &i) {
	if (args == NULL) return false;
	if (args->restype == RTSHORT) {
		i = args->resval.rint;
		args = args->rbnext;
		return true;
	} else if (args->restype == RTLONG) {
		i = args->resval.rlong;
		args = args->rbnext;
		return true;
	} else if (args->restype == RTREAL) {
		i = (int)args->resval.rreal;
		args = args->rbnext;
		return true;
	} else if (args->restype < 1000) {
		//Don't advance in this case
		i = args->restype;
		return true;
	}
	return false;
}
bool getPointArgument(struct resbuf* &args, AcGePoint3d &p) {
	if (args == NULL) return false;
	if (args->restype == RTPOINT 
		|| args->restype == RT3DPOINT
		|| (10 <= args->restype && args->restype <= 17)) {
			p[X] = args->resval.rpoint[X];
			p[Y] = args->resval.rpoint[Y];
			if (args->restype == RTPOINT) {
				p[Z] = 0;
			} else {
				p[Z] = args->resval.rpoint[Z];
			}
			args = args->rbnext;
			return true;
	} 
	if ((args->restype == RTREAL || args->restype == RTLONG || args->restype == RTSHORT)
		&& args->rbnext != NULL 
		&& (args->rbnext->restype == RTREAL || args->rbnext->restype == RTLONG || args->restype == RTSHORT))
	{
		if (args->restype == RTREAL)
			p[X] = args->resval.rreal;
		else
			p[X] = args->resval.rlong;

		if (args->rbnext->restype == RTREAL)
			p[Y] = args->rbnext->resval.rreal;
		else
			p[Y] = args->rbnext->resval.rlong;

		args = args->rbnext->rbnext;
		if (args != NULL && (args->restype == RTREAL || args->restype == RTLONG || args->restype == RTSHORT)) {
			if (args->restype == RTREAL)
				p[Z] = args->resval.rreal;
			else
				p[Z] = args->resval.rlong;
			args = args->rbnext;
		} else {
			p[Z] = 0;
		}
		return true;
	}
	return false;
}
bool get2DPointArgument(struct resbuf* &args, AcGePoint2d &p) {
	if (args == NULL) return false;
	if (args->restype == RTPOINT 
		|| args->restype == RT3DPOINT
		|| (10 <= args->restype && args->restype <= 17)) {
			p[X] = args->resval.rpoint[X];
			p[Y] = args->resval.rpoint[Y];
			args = args->rbnext;
			return true;
	} 
	if (args->restype == RTREAL
		&& args->rbnext != NULL && args->rbnext->restype == RTREAL)
	{
		p[X] = args->resval.rreal;
		p[Y] = args->rbnext->resval.rreal;
		args = args->rbnext->rbnext;
		return true;
	}
	return false;
}
bool getBoolArgument(struct resbuf* &args, bool& b) {
	if (args == NULL) return false;
	if (args->restype == RTNIL) {
		args = args->rbnext;
		b = false;
		return true;
	} else if (args->restype == RTLE) {
		//The end of a list will be considered nil, but we won't move to the next argument.
		//This handles dotted pairs where the second half is nil.
		b = false;
		return true;
	} else {
		//Anything that is not NIL is true.
		//Don't check for RTT because people keep using T as a variable. Sigh.
		//Since it could be anything, it might be a list. If that happens, then we will
		//need to consume the list here. For the moment, I will simply crash on that, because I
		//don't think it will happen.
		if (args->restype != RTLB) {
			args = args->rbnext;
			b = true;
			return true;
		}
	}
	return false;
};
bool getOLEArgument(struct resbuf* &args, COleVariant &ole) {
	switch (args->restype) {
		case RTREAL:
			ole.vt = VT_R8;  
			ole.dblVal = args->resval.rreal;
			return true;
		case RTSHORT:
			ole.vt = VT_I4;  
			ole.lVal = args->resval.rlong;
			return true;
		case RTLONG:
			ole.vt = VT_I4;  
			ole.lVal = args->resval.rlong;
			return true;
		case RTSTR:
			{
				ole.vt = VT_BSTR;  
				CString str(args->resval.rstring);
				ole.bstrVal = str.AllocSysString();
				return true;
			}
		case RTT:
			ole.vt = VT_BOOL;  
			ole.boolVal = true;
			return true;
		case RTNIL:
			ole.vt = VT_BOOL;  
			ole.boolVal = false;
			return true;
		default:
			return false;
	}
}
