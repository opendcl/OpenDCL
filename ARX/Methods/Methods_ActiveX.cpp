// Methods_ActiveX.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ActiveX.h"
#include "Methods_Month.h"
#include "AxContainer.h"
#include "AxMethodDescriptor.h"
#include "AxPropertyDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ArgumentsRetrieval.h"
#include "ArxProject.h"
#include "ErrorLexicon.h"
#include "MethodLexicon.h"
#include "AcadColorTable.h"

void acedRetOleVar(COleVariant &varGet, RefCountedPtr< CPropertyObject > pProp = NULL, AxMethodDescriptor *pMethod = NULL, CAxContainer *pAxContainer = NULL, AxPropertyDescriptor *pAxProp = NULL);
bool getActiveXArguments(
	COleVariant argList[]
	, int &nArgCount
	, int nParams
	, struct resbuf* ListData
	, AxPropertyDescriptor* pAxProp
	, AxMethodDescriptor* pAxMethod);

//*****************************************************************************
// 
// Method: sSetAxObjColorProperty()
// 
// Purpose: [Sets an ActiveX color property of the requested object]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int SetAxObjColorProperty()
{
	CString sPropNameArg;
	int nArg = 0;
	int nArgCount=0;
	ULONG lObject;
	ULONG lDispatch;
	struct resbuf *ListData;

	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sAxSetProperty); 			
        return false; 
	}

	// must be a list
	if (ListData->restype == RTPOINT)
	{
		// get the argument
		lObject = ULONG(ListData->resval.rpoint[0]);
		lDispatch = ULONG(ListData->resval.rpoint[1]);			
	}	
	// must be a list
	else if (ListData->restype != RTLB)
	{
		acedRetVoid();
        return 0; 
	}
	else if (ListData->restype == RTLB)
	{
		// iterate forward to the next required argument	
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}	
		
		if (ListData->restype == RTLONG)
		{
			// get the argument
			lObject = ListData->resval.rlong;				
		}
		else if (ListData->restype == RTSHORT)
		{
			// get the argument
			lObject = ListData->resval.rint;				
		}
		
		// iterate forward to the next required argument	
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}
		
		if (ListData->restype == RTSHORT)
		{
			// get the argument
			lDispatch = ListData->resval.rint;				
		}
		// iterate forward to the next required argument
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}

		// must close the list
		if (ListData->restype != RTLE)
		{
			acedRetVoid();
			return 0; 
		}
	}
	
	if (ListData == NULL)
	{
		acedRetVoid();
        return 0; 
	}

	CDclControlObject *pControl = (CDclControlObject*) lObject;

	RefCountedPtr< CPropertyObject > pProp = pControl->FindPropertyObject(sPropNameArg);
	int nParams;
	RefCountedPtr< AxPropertyDescriptor > pAxProp = NULL;
	if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() &&
		(pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_DISPATCH || 
		 pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_UNKNOWN ||
		 pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_VOID))
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPut())
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPut();
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef())
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetProp())
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetProp();
	
	nParams = pAxProp->GetArgs().size();
	// here we are ensuring that there is at least one parameters to be passed in. 
	// Since we are Putting it must have at least one parameter.
	if (nParams == 0)
		return 0;

	// get the AcxtiveX control
	CAxContainer *axContainer = (CAxContainer*)pControl->GetWindow();
	//COleDispatchDriver Disp((LPDISPATCH)lDispatch);// axContainer->GetOleIDispatch();
	
	long lRed = 0;
	long lGreen = 0;
	long lBlue = 0;
	
	if (ListData->restype == RTSHORT)	
	{
		lRed = ListData->resval.rint;
		ListData = ListData->rbnext;
	}
	else if (ListData->restype == RTLONG)	
	{
		lRed = ListData->resval.rint;
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)	
	{
		axContainer->SetColor(pAxProp->GetDispId(),  GetRGBColor(lRed));
		acedRetVoid();
		return 0;	
	}

	if (ListData->restype == RTSHORT)	
	{
		lGreen = ListData->resval.rint;
		ListData = ListData->rbnext;
	}
	else if (ListData->restype == RTLONG)	
	{
		lGreen = ListData->resval.rint;
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)	
	{
		acedRetVoid();
		return 0;	
	}

	if (ListData->restype == RTSHORT)	
	{
		lBlue = ListData->resval.rint;
	}
	else if (ListData->restype == RTLONG)	
	{
		lBlue = ListData->resval.rint;
	}
	
	axContainer->SetColor(pAxProp->GetDispId(), RGB(lRed, lGreen, lBlue));
	acedRetVoid();
	return 0;
}

//*****************************************************************************
// 
// Method: SetAxObjectPictureProperty()
// 
// Purpose: [Sets an ActiveX object's IPictureDisp property]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int SetAxObjectPictureProperty()
{
	CString sPropNameArg;
	struct resbuf *ListData = NULL;
	ULONG lObject = 0;
	ULONG lDispatch = 0;
	CString sFileName;
	int nPictureId = 0;

	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
    {
		acedRetVoid();
        return 0; 
	}
	
	// must be a list
	if (ListData->restype == RTPOINT)
	{
		// get the argument
		lObject = ULONG(ListData->resval.rpoint[0]);
		lDispatch = ULONG(ListData->resval.rpoint[1]);			
	}	
	// must be a list
	else if (ListData->restype != RTLB)
	{
		acedRetVoid();
        return 0; 
	}
	else if (ListData->restype == RTLB)
	{
		// iterate forward to the next required argument	
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}	
		
		if (ListData->restype == RTLONG)
		{
			// get the argument
			lObject = ListData->resval.rlong;				
		}
		else if (ListData->restype == RTSHORT)
		{
			// get the argument
			lObject = ListData->resval.rint;				
		}
		
		// iterate forward to the next required argument	
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}
		
		if (ListData->restype == RTLONG)
		{
			// get the argument
			lDispatch = ListData->resval.rlong;				
		}
		else if (ListData->restype == RTSHORT)
		{
			// get the argument
			lDispatch = ListData->resval.rint;				
		}
		// iterate forward to the next required argument
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}

		// must close the list
		if (ListData->restype != RTLE)
		{
			acedRetVoid();
			return 0; 
		}
		
	}
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;
	
	if (ListData == NULL) 
	{
		acedRetVoid();
        return 0; 
	}

	if (ListData->restype == RTSTR)
	{
		// get the argument
		sPropNameArg = ListData->resval.rstring;
	}
	
	// first iterate forward to the next required argument
	ListData = ListData->rbnext;
	
	if (ListData == NULL)
	{
		acedRetVoid();
        return 0; 
	}

	if (ListData->restype == RTSTR)
	{
		// get the argument
		sFileName = ListData->resval.rstring;
	}
	else if (ListData->restype == RTSHORT)
	{
		// get the argument
		nPictureId = ListData->resval.rint;
	}
	else if (ListData->restype == RTLONG)
	{
		// get the argument
		nPictureId = ListData->resval.rlong;
	}
		
	CDclControlObject *pControl = (CDclControlObject*) lObject;
	RefCountedPtr< CPropertyObject > pProp = pControl->FindPropertyObject(sPropNameArg);

	AxPropertyDescriptor *pAxProp = NULL;
	
	if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() &&
		(pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_DISPATCH || 
		 pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_UNKNOWN ||
		 pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_VOID))
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPut())
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPut();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef())
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetProp())
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetProp();
	}
	
	if (ListData->restype == RTSTR)
	{
		CString sFileName = ListData->resval.rstring;
		
		// call method to load the picture file.
		pControl->m_pAxWnd->LoadPictureFile(pAxProp->GetDispId(), sFileName, 0); //just added the 0 so it compiles [ORW]
	}
	else if (ListData->restype == RTSHORT)	
	{
		// call method to load the picture from the picture folder.
		pControl->m_pAxWnd->LoadPicture(pAxProp->GetDispId(), ListData->resval.rint);	
	}
	else if (ListData->restype == RTLONG)	
	{
		// call method to load the picture from the picture folder.
		pControl->m_pAxWnd->LoadPicture(pAxProp->GetDispId(), ListData->resval.rlong);	
	}
	
	
	acedRetVoid();
	return 0;
}



//*****************************************************************************
// 
// Method: AxGetObject()
// 
// Purpose: [Gets the IDispatch]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int AxGetObject()
{
	int nArg = 0;
	
	// call the method to get the property object
	CDclControlObject *pControl = GetControlArxObject(sAxSetProperty, &nArg);
		
	if (pControl == NULL)
	{
		acedRetVoid();
		return 0;
	}
	CAxContainer *pAxCont = pControl->m_pAxWnd;
		
	if (pAxCont != NULL)
	{
		int stat;
		struct resbuf *list;    

		IDispatch* pDisp;
		pAxCont->GetOleDispatch( &pDisp );
		list = acutBuildList(
			RTLONG, (long)pControl,
			RTLONG, pDisp,
			RTNONE);

		if (list != NULL) 
		{ 	    
			stat = acedRetList(list);		
			acutRelRb(list); 
		} 
	}
	return 0;
}

//*****************************************************************************
// 
// Method: ClostAxObject()
// 
// Purpose: [Closes an ActiveX object]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int CloseAxObject()
{
	CString sPropNameArg;
	struct resbuf *ListData = NULL;
	ULONG lObject = 0;
	ULONG lDispatch = 0;
	
	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
    {
		acedRetVoid();
        return 0; 
	}
	
	// must be a list
	if (ListData->restype == RTPOINT)
	{
		// get the argument
		lObject = ULONG(ListData->resval.rpoint[0]);
		lDispatch = ULONG(ListData->resval.rpoint[1]);			
	}	
	// must be a list
	else if (ListData->restype != RTLB)
	{
		acedRetVoid();
        return 0; 
	}
	else if (ListData->restype == RTLB)
	{
		// iterate forward to the next required argument	
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}	
		
		if (ListData->restype == RTLONG)
		{
			// get the argument
			lObject = ListData->resval.rlong;				
		}
		else if (ListData->restype == RTSHORT)
		{
			// get the argument
			lObject = ListData->resval.rint;				
		}
		
		// iterate forward to the next required argument	
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}
		
		if (ListData->restype == RTLONG)
		{
			// get the argument
			lDispatch = ListData->resval.rlong;				
		}
		else if (ListData->restype == RTSHORT)
		{
			// get the argument
			lDispatch = ListData->resval.rint;				
		}
		// iterate forward to the next required argument
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}

		// must close the list
		if (ListData->restype != RTLE)
		{
			acedRetVoid();
			return 0; 
		}		
	}

	acedRetVoid();
	return 0; 
}
//*****************************************************************************
// 
// Method: GetAxObjectProperty()
// 
// Purpose: [Gets an ActiveX object's property]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int GetAxObjectProperty()
{
	CString sPropNameArg;
	struct resbuf *ListData = NULL;
	ULONG lObject = 0;
	ULONG lDispatch = 0;
	
	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
    {
		acedRetVoid();
        return 0; 
	}
	
	// must be a list
	if (ListData->restype == RTPOINT)
	{
		// get the argument
		lObject = ULONG(ListData->resval.rpoint[0]);
		lDispatch = ULONG(ListData->resval.rpoint[1]);			
	}	
	// must be a list
	else if (ListData->restype != RTLB)
	{
		acedRetVoid();
        return 0; 
	}
	else if (ListData->restype == RTLB)
	{
		// iterate forward to the next required argument	
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}	
		
		if (ListData->restype == RTLONG)
		{
			// get the argument
			lObject = ListData->resval.rlong;				
		}
		else if (ListData->restype == RTSHORT)
		{
			// get the argument
			lObject = ListData->resval.rint;				
		}
		
		// iterate forward to the next required argument	
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}
		
		if (ListData->restype == RTLONG)
		{
			// get the argument
			lDispatch = ListData->resval.rlong;				
		}
		else if (ListData->restype == RTSHORT)
		{
			// get the argument
			lDispatch = ListData->resval.rint;				
		}
		// iterate forward to the next required argument
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}

		// must close the list
		if (ListData->restype != RTLE)
		{
			acedRetVoid();
			return 0; 
		}
		
	}

		// iterate forward to the next required argument
	ListData = ListData->rbnext;
	
	if (ListData == NULL) 
	{
		acedRetVoid();
        return 0; 
	}

	if (ListData->restype == RTSTR)
	{
		// get the argument
		sPropNameArg = ListData->resval.rstring;
	}
	
	// first iterate forward to the next required argument
	ListData = ListData->rbnext;
	

	CDclControlObject *pControl = (CDclControlObject*) lObject;
	RefCountedPtr< CPropertyObject > pProp = pControl->FindPropertyObject(sPropNameArg);
	if (pProp == NULL)
	{
		theWorkspace.DisplayAlert(CString ("The ActiveX get property \"") + sPropNameArg + "\" is not a member of this ActiveX object.");
		acedRetNil();
		return 0;
	}

	int nParams;
	AxPropertyDescriptor *pAxProp = NULL;
	if (pProp->GetAxInterfaceDescriptorPtr()->GetPropGet())
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropGet();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef())
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPut())
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPut();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetProp())
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetProp();
	}
	
	nParams = pAxProp->GetArgs().size();
	//Creating this dynamically based upon nParams would be better than fixing it at 16
	COleVariant argList[16];
	int nArgCount = 0;
	if (!getActiveXArguments(argList, nArgCount, nParams, ListData, pAxProp, NULL)) {
		acedRetNil();
		return 0;
	}

	// get the AcxtiveX control
	CAxContainer *axContainer = (CAxContainer*)pControl->GetWindow();
	
	COleVariant varGet;
	axContainer->GetProperty(pAxProp, argList, nArgCount, varGet);

	acedRetOleVar(varGet, pProp, NULL, axContainer, pAxProp);

	return 0;
}


//*****************************************************************************
// 
// Method: SetAxObjectProperty()
// 
// Purpose: [Sets an ActiveX object's property]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int SetAxObjectProperty()
{
	
	CString sPropNameArg;
	struct resbuf *ListData = NULL;
	ULONG lObject = 0;
	ULONG lDispatch = 0;
	
	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
    {
		acedRetVoid();
        return 0; 
	}
	
	// must be a list
	if (ListData->restype == RTPOINT)
	{
		// get the argument
		lObject = ULONG(ListData->resval.rpoint[0]);
		lDispatch = ULONG(ListData->resval.rpoint[1]);			
	}	
	// must be a list
	else if (ListData->restype != RTLB)
	{
		acedRetVoid();
        return 0; 
	}
	else if (ListData->restype == RTLB)
	{
		// iterate forward to the next required argument	
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}	
		
		if (ListData->restype == RTLONG)
		{
			// get the argument
			lObject = ListData->resval.rlong;				
		}
		else if (ListData->restype == RTSHORT)
		{
			// get the argument
			lObject = ListData->resval.rint;				
		}
		
		// iterate forward to the next required argument	
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}
		
		if (ListData->restype == RTLONG)
		{
			// get the argument
			lDispatch = ListData->resval.rlong;				
		}
		else if (ListData->restype == RTSHORT)
		{
			// get the argument
			lDispatch = ListData->resval.rint;				
		}
		// iterate forward to the next required argument
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}

		// must close the list
		if (ListData->restype != RTLE)
		{
			acedRetVoid();
			return 0; 
		}
		
	}
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;
	
	if (ListData == NULL) 
	{
		acedRetVoid();
        return 0; 
	}

	if (ListData->restype == RTSTR)
	{
		// get the argument
		sPropNameArg = ListData->resval.rstring;
	}
	
	// first iterate forward to the next required argument
	ListData = ListData->rbnext;
	
	if (ListData == NULL)
	{
		acedRetVoid();
        return 0; 
	}

	CDclControlObject *pControl = (CDclControlObject*) lObject;
	if (pControl == NULL)
	{
		acedRetVoid();
        return 0; 
	}

	RefCountedPtr< CPropertyObject > pProp = pControl->FindPropertyObject(sPropNameArg);
	if (pProp == NULL)
	{
		acedRetVoid();
        return 0; 
	}

	int nParams;
	AxPropertyDescriptor *pAxProp = NULL;
	if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() &&
		(pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_DISPATCH || 
		 pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_UNKNOWN ||
		 pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_VOID))
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPut())
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPut();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef())
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetProp())
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetProp();
	}
	
	nParams = pAxProp->GetArgs().size();
	// here we are ensuring that there is at least one parameters to be passed in. 
	// Since we are Putting it must have at least one parameter.
	if (nParams == 0)
		return 0;
	//Creating this dynamically based upon nParams would be better than fixing it at 16
	COleVariant argList[16];
	int nArgCount = 0;
	if (!getActiveXArguments(argList, nArgCount, nParams, ListData, pAxProp, NULL)) {
		acedRetNil();
		return 0;
	}

/* broken, needs to be fixed [ORW]
	if (nParams == 1)
	{		
		// call the set property method to set the property
		pControl->SetAxProperty(pAxProp, &argList[0]);
	}
	else
	{
		COleVariant varGet;
		InvokeAxHelperV(pControl->GetIDispatch(), pAxProp, DISPATCH_PROPERTYPUT,
			pAxProp->GetType(), &varGet, pAxProp, &argList, nParams);
	}
*/
	acedRetVoid();
	return 0;
}

//*****************************************************************************
// 
// Method: DoObjectAxMethod()
// 
// Purpose: [calls an ActiveX method on the requested object]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int DoAxObjectMethod()
{
	struct resbuf *ListData = NULL;
	CString sMethodNameArg;
	int nArgCount=0;
	ULONG lObject = 0;
	ULONG lDispatch = 0;
		
	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
    {
		acedRetVoid();
        return 0; 
	}
	
	// must be a list
	if (ListData->restype == RTPOINT)
	{
		// get the argument
		lObject = ULONG(ListData->resval.rpoint[0]);
		lDispatch = ULONG(ListData->resval.rpoint[1]);			
	}	
	// must be a list
	else if (ListData->restype != RTLB)
	{
		acedRetVoid();
        return 0; 
	}
	else if (ListData->restype == RTLB)
	{
		// iterate forward to the next required argument	
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}	
		
		if (ListData->restype == RTLONG)
		{
			// get the argument
			lObject = ListData->resval.rlong;				
		}
		else if (ListData->restype == RTSHORT)
		{
			// get the argument
			lObject = ListData->resval.rint;				
		}
		
		// iterate forward to the next required argument	
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}
		
		if (ListData->restype == RTLONG)
		{
			// get the argument
			lDispatch = ListData->resval.rlong;				
		}
		else if (ListData->restype == RTSHORT)
		{
			// get the argument
			lDispatch = ListData->resval.rint;				
		}
		// iterate forward to the next required argument
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			acedRetVoid();
			return 0; 
		}

		// must close the list
		if (ListData->restype != RTLE)
		{
			acedRetVoid();
			return 0; 
		}
		
	}
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;
	
	if (ListData == NULL) 
	{
		acedRetVoid();
        return 0; 
	}

	if (ListData->restype == RTSTR)
	{
		// get the argument
		sMethodNameArg = ListData->resval.rstring;
	}
	
	// iterate forward to the next required argument
	ListData = ListData->rbnext;

/* broken [ORW]
	CDclControlObject *pControl = (CDclControlObject*) lObject;
	RefCountedPtr< CPropertyObject > pProp = pControl->GetMethods();
	int nParams;
	
	if (pProp->m_Type != PropActiveXMethods)
	{
		acedRetNil();
		return 0;
	}

	// here we need to search the methods to find the one requestes.
	POSITION pos = pProp->m_pMethods->GetHeadPosition();
	AxMethodDescriptor *pMethod = NULL;
	while (pos != NULL)
	{
		pMethod = pProp->m_pMethods->GetNext(pos);
		// if this is the method, end the while loop now
		if (pMethod->Name == sMethodNameArg)
		{
			pos = NULL;
		}
	}
	
	if (pMethod == NULL)
	{
		acedRetNil();
		return 0;
	}

	nParams = pMethod->nParamQty;
	
	VariantList argList;
	
	while (ListData != NULL)
	{
		COleVariant Var;
		if (GetAxPropertyArgument(ListData, &Var, pMethod->CallingArgs[nArgCount]))
		{
			argList.m_Variant[nArgCount] = Var;
			nArgCount++;			
		}
		else
		{
			acedRetNil();
			return 0;
		}
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
		if (ListData != NULL)
		{
			if (nArgCount == nParams)
				break;
		}
	}


	// get the AcxtiveX control
	
	COleVariant varGet;
	COleDispatch *disp = (COleDispatch*)lDispatch;
	
	// call the set property method to set the property
	DoAxMethod(disp->m_lpDispatch, pMethod, &argList, &varGet);
	acedRetOleVar(varGet, NULL, pMethod, (CAxContainer*)disp->m_pParentAxContainer);
*/

	return 0;
}


//*****************************************************************************
// 
// Method: SetAxColorProperty()
// 
// Purpose: [Sets an ActiveX color property of the requested control]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int SetAxColorProperty()
{
	/*
	CString sPropNameArg;
	int nArg = 0;
	int nArgCount=0;
	// call the method to get the property object
	CDclControlObject *pControl = GetControlArxObject(sAxSetProperty, &nArg);
		
	if (pControl == NULL || !GetStringArgument(nArg, &sPropNameArg, sAxSetProperty))
	{
		acedRetVoid();
		return 0;
	}
	nArg++;
	
	struct resbuf *ListData;

	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sAxSetProperty); 			
        return false; 
	}

	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetVoid();
        return 0; 
	}

	RefCountedPtr< CPropertyObject > pProp = pControl->FindPropertyObject(sPropNameArg);
	if (pProp == NULL)
	{
		acedRetVoid();
        return 0; 
	}

	int nParams;
	AxPropertyDescriptor *pAxProp = NULL;
	if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() != NULL &&
		(pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_DISPATCH || 
		 pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_UNKNOWN ||
		 pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_VOID))
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPut() != NULL)
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPut();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() != NULL)
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetProp() != NULL)
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetProp();
	}
	
	nParams = pAxProp->NumParams;
	// here we are ensuring that there is at least one parameters to be passed in. 
	// Since we are Putting it must have at least one parameter.
	if (nParams == 0)
	{
		nParams = 1;
		//pAxProp->NumParams = nParams;
		pAxProp->CallingArgs[0] = pAxProp->GetType();
	}
	
	// get the AcxtiveX control
	CAxContainer *axContainer = (CAxContainer*)pControl->GetWindow();
	IDispatch *pDisp = axContainer->GetOleIDispatch();
	
	long lRed = 0;
	long lGreen = 0;
	long lBlue = 0;
	
	if (ListData->restype == RTSHORT)	
	{
		lRed = ListData->resval.rint;
		ListData = ListData->rbnext;
	}
	else if (ListData->restype == RTLONG)	
	{
		lRed = ListData->resval.rint;
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)	
	{
		SetColor(pDisp, pAxProp->GetDispId(),  GetRGBColor(lRed));
		pDisp->Release();
		acedRetVoid();
		return 0;	
	}

	if (ListData->restype == RTSHORT)	
	{
		lGreen = ListData->resval.rint;
		ListData = ListData->rbnext;
	}
	else if (ListData->restype == RTLONG)	
	{
		lGreen = ListData->resval.rint;
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)	
	{
		acedRetVoid();
		return 0;	
	}

	if (ListData->restype == RTSHORT)	
	{
		lBlue = ListData->resval.rint;
	}
	else if (ListData->restype == RTLONG)	
	{
		lBlue = ListData->resval.rint;
	}
	
	SetColor(pDisp, pAxProp->GetDispId(), RGB(lRed, lGreen, lBlue));
	pDisp->Release();
	*/
	acedRetVoid();
	return 0;
}


//*****************************************************************************
// 
// Method: SetAxPictureProperty()
// 
// Purpose: [Sets an ActiveX picture property of the requested control]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int SetAxPictureProperty()
{
	/*
	CString sPropNameArg;
	int nArg = 0;
	int nArgCount=0;
	
	// call the method to get the property object
	CDclControlObject *pControl = GetControlArxObject(sAxSetProperty, &nArg);
		
	if (pControl == NULL || !GetStringArgument(nArg, &sPropNameArg, sAxSetProperty))
	{
		acedRetVoid();
		return 0;
	}
	nArg++;
	
	struct resbuf *ListData;

	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sAxSetProperty); 			
        return false; 
	}

	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetVoid();
        return 0; 
	}


	RefCountedPtr< CPropertyObject > pProp = pControl->FindPropertyObject(sPropNameArg);
	int nParams;
	AxPropertyDescriptor *pAxProp = NULL;
	if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() != NULL &&
		(pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_DISPATCH || 
		 pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_UNKNOWN ||
		 pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_VOID))
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPut() != NULL)
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPut();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() != NULL)
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetProp() != NULL)
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetProp();
	}
	
	nParams = pAxProp->NumParams;
	// here we are ensuring that there is at least one parameters to be passed in. 
	// Since we are Putting it must have at least one parameter.
	if (nParams == 0)
	{
		nParams = 1;
		//pAxProp->NumParams = nParams;
		pAxProp->CallingArgs[0] = pAxProp->GetType();
	}

	// get the AcxtiveX control
	CAxContainer *axContainer = (CAxContainer*)pControl->GetWindow();
	IDispatch *pDisp = axContainer->GetOleIDispatch();
		
	if (ListData->restype == RTSTR)
	{
		CString sFileName = ListData->resval.rstring;		
		// call method to load the picture file.
		LoadPictureFile(pDisp, pAxProp->GetDispId(), sFileName);
	}
	else if (ListData->restype == RTSHORT)	
	{
		// call method to load the picture from the picture folder.
		LoadPicture(pDisp, pAxProp->GetDispId(), ListData->resval.rint, pControl);	
	}
	else if (ListData->restype == RTLONG)	
	{
		// call method to load the picture from the picture folder.
		LoadPicture(pDisp, pAxProp->GetDispId(), ListData->resval.rlong, pControl);	
	}
	pDisp->Release();
	acedRetVoid();
	*/
	return 0;
}

	

//*****************************************************************************
// 
// Method: SetAxProperty()
// 
// Purpose: [Sets an ActiveX property of the requested control]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int SetAxProperty()
{
	CString sPropNameArg;
	int nArg = 0;
	int nArgCount=0;
	
	// call the method to get the property object
	CDclControlObject *pControl = GetControlArxObject(sAxSetProperty, &nArg);
		
	if (pControl == NULL || !GetStringArgument(nArg, &sPropNameArg, sAxSetProperty))
	{
		acedRetVoid();
		return 0;
	}
	nArg++;
	
	struct resbuf *ListData;

	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sAxSetProperty); 			
        return false; 
	}

	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetVoid();
        return 0; 
	}


	RefCountedPtr< CPropertyObject > pProp = pControl->FindPropertyObject(sPropNameArg);
	if( !pProp )
		return RSERR; //no property with this name!

	int nParams;
	AxPropertyDescriptor *pAxProp = NULL;
	if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() != NULL &&
		(pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_DISPATCH || 
		 pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_UNKNOWN ||
		 pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_VOID))
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPut() != NULL)
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPut();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() != NULL)
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetProp() != NULL)
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetProp();
	}
	
	nParams = pAxProp->GetArgs().size();
	// here we are ensuring that there is at least one parameters to be passed in. 
	// Since we are Putting it must have at least one parameter.
	if (nParams == 0)
		return 0;

	//Creating this dynamically based upon nParams would be better than fixing it at 16
	COleVariant argList[16];
	if (!getActiveXArguments(argList, nArgCount, nParams, ListData, pAxProp, NULL)) {
		acedRetNil();
		return 0;
	}

	//get the AcxtiveX control
	//[DPR] GetWindow returns a CWnd. For this instance, we know the it will be a CAxContainer, 
	//so force the cast.
	CAxContainer *axContainer = (CAxContainer*)pControl->GetWindow();
	HRESULT hr = axContainer->SetProperty(pAxProp, argList, nArgCount);
	if( FAILED(hr) )
		return RSRSLT;

	acedRetT();

	return 0;
}

//*****************************************************************************
// 
// Method: SetFlexGridColorProperty()
// 
// Purpose: [Sets the color of a flex grid]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int SetFlexGridColorProperty()
{
/*
  CString sPropNameArg;
  int nArg = 0;
  int nArgCount=0;

  // call the method to get the property object
  CDclControlObject *pControl = GetControlArxObject(sAxSetProperty, &nArg);

  if (pControl == NULL || !GetStringArgument(nArg, &sPropNameArg, sAxSetProperty))
  {
    acedRetVoid();
    return 0;
  }
  nArg++;

  struct resbuf *ListData;

  //ensure AutoLisp has passed Arguments	
  if ((ListData = acedGetArgs()) == NULL) 
  {
    // inform the programmer that he did not make the correct call
    theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sAxSetProperty); 			
    return false; 
  }

  for (int i = 0; i < nArg; i++)
  {
    // first iterate forward to the next required argument
    ListData = ListData->rbnext;
  }

  if (ListData == NULL)
  {
    acedRetVoid();
    return 0; 
  }


  RefCountedPtr< CPropertyObject > pProp = pControl->FindPropertyObject(sPropNameArg);
  int nParams;
  AxPropertyDescriptor *pAxProp = NULL;
  if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() != NULL &&
    (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_DISPATCH || 
    pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_UNKNOWN ||
    pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetType() == VT_VOID))
  {
    pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
  }
  else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPut() != NULL)
  {
    pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPut();
  }
  else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() != NULL)
  {
    pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
  }
  else if (pProp->GetAxInterfaceDescriptorPtr()->GetProp() != NULL)
  {
    pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetProp();
  }

  nParams = pAxProp->NumParams;

  //Only one parameter is passed -- the color as a real, which will be converted to
  //an unsigned int.
  double dColor;
  if (!getDoubleArgument(ListData, dColor)) {
    acedRetVoid();
    return 0; 
  }
  unsigned int iColor = (unsigned int)dColor;

  // get the AcxtiveX control
  CAxContainer *axContainer = (CAxContainer*)pControl->GetWindow();

  axContainer->SetFlexGridColorProperty(pAxProp, iColor);
  acedRetVoid();
*/
  return 0;
}

//*****************************************************************************
// 
// Method: GetAxProperty()
// 
// Purpose: [Gets an ActiveX property of the requested control]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int GetAxProperty()
{
	CString sPropNameArg;
	int nArg = 0;
	int nArgCount=0;
	
	// call the method to get the property object
	CDclControlObject *pControl = GetControlArxObject(sAxGetProperty, &nArg);
	
	if (pControl == NULL || !GetStringArgument(nArg, &sPropNameArg, sAxGetProperty))
	{
		acedRetVoid();
		return 0;
	}
	nArg++;
	
	struct resbuf *ListData;

	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sAxGetProperty); 			
        return false; 
	}

	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	RefCountedPtr< CPropertyObject > pProp = pControl->FindPropertyObject(sPropNameArg);
	if( !pProp )
		return RSERR; //no property with this name!

	int nParams;
	AxPropertyDescriptor *pAxProp = NULL;
	if (pProp->GetAxInterfaceDescriptorPtr()->GetPropGet() != NULL)
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropGet();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() != NULL)
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
	}
	else if (pProp->GetAxInterfaceDescriptorPtr()->GetProp() != NULL)
	{
		pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetProp();
	}
	
	nParams = pAxProp->GetArgs().size();
	
	//Creating this dynamically based upon nParams would be better than fixing it at 16
	COleVariant argList[16];
	if (!getActiveXArguments(argList, nArgCount, nParams, ListData, pAxProp, NULL)) {
		acedRetNil();
		return 0;
	}

	// get the AcxtiveX control
	CAxContainer *axContainer = (CAxContainer*)pControl->GetWindow();
	
	COleVariant varGet;
	axContainer->GetProperty(pAxProp, argList, nArgCount, varGet);

	acedRetOleVar(varGet, pProp, NULL, axContainer, pAxProp);
	return 0;
}

//*****************************************************************************
// 
// Method: GetFlexGridColorProperty()
// 
// Purpose: [Gets a color from a flex grid control.]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int GetFlexGridColorProperty()
{
/*
  CString sPropNameArg;
  int nArg = 0;
  int nArgCount=0;

  // call the method to get the property object
  CDclControlObject *pControl = GetControlArxObject(sAxGetProperty, &nArg);

  if (pControl == NULL || !GetStringArgument(nArg, &sPropNameArg, sAxGetProperty))
  {
    acedRetVoid();
    return 0;
  }
  nArg++;

  struct resbuf *ListData;

  //ensure AutoLisp has passed Arguments	
  if ((ListData = acedGetArgs()) == NULL) 
  {
    // inform the programmer that he did not make the correct call
    theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sAxGetProperty); 			
    return false; 
  }

  for (int i = 0; i < nArg; i++)
  {
    // first iterate forward to the next required argument
    ListData = ListData->rbnext;
  }

  RefCountedPtr< CPropertyObject > pProp = pControl->FindPropertyObject(sPropNameArg);
  int nParams;
  AxPropertyDescriptor *pAxProp = NULL;
  if (pProp->GetAxInterfaceDescriptorPtr()->GetPropGet() != NULL)
  {
    pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropGet();
  }
  else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef() != NULL)
  {
    pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef();
  }
  else if (pProp->GetAxInterfaceDescriptorPtr()->GetProp() != NULL)
  {
    pAxProp = pProp->GetAxInterfaceDescriptorPtr()->GetProp();
  }

  nParams = pAxProp->NumParams;

  VariantList argList;

  nArg++;
  while (ListData != NULL)
  {
    COleVariant Var;
    if (GetAxPropertyArgument(ListData, &Var, pAxProp->CallingArgs[nArgCount]))
    {
      argList.m_Variant[nArgCount] = Var;
      nArgCount++;			
    }
    else
    {
      acedRetNil();
      return 0;
    }
    // first iterate forward to the next required argument
    ListData = ListData->rbnext;
    if (ListData != NULL)
    {
      nArg++;				
      if (nArgCount >= nParams)
        break;
    }
  }

  // get the AcxtiveX control
  CAxContainer *axContainer = (CAxContainer*)pControl->GetWindow();

  //Return the color
  unsigned int iColor = axContainer->GetFlexGridColorProperty(pAxProp);
  acedRetReal(iColor);
*/
  return 0;
}
//*****************************************************************************
// 
// Method: DoAxMethod()
// 
// Purpose: [calls an ActiveX method on the requested control]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int DoAxMethod()
{
	CString sMethodNameArg;
	int nArg = 0;
	int nArgCount=0;
	
	// call the method to get the property object
	CDclControlObject *pControl = GetControlArxObject(sDoAxMethod, &nArg);	
	if (pControl == NULL || !GetStringArgument(nArg, &sMethodNameArg, sDoAxMethod))
	{
		acedRetVoid();
		return 0;
	}
	nArg++;
	
	struct resbuf *ListData;

	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorNoArgumentsFound) + sDoAxMethod); 			
        return false; 
	}

	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}


	RefCountedPtr< CPropertyObject > pProp = pControl->GetMethods();
	int nParams;
	
	if (pProp->GetType() != PropActiveXMethods)
	{
		acedRetNil();
		return 0;
	}

	// here we need to search the methods to find the one requested.
	std::vector< RefCountedPtr< AxMethodDescriptor > > *vMethods = pProp->GetAxInterfaceDescriptorPtr()->GetMethods();
	RefCountedPtr< AxMethodDescriptor > pMethod;
	for (int i = 0; i < vMethods->size(); i++)
	{
		RefCountedPtr< AxMethodDescriptor > pMethodCheck = vMethods->at(i);
		if (pMethodCheck->GetName() == sMethodNameArg)
		{
			pMethod = pMethodCheck;
			break;
		}
	}
	
	if (pMethod == NULL)
	{
		acedRetNil();
		return 0;
	}

	nParams = pMethod->GetArgs().size();
	
	//Creating this dynamically based upon nParams would be better than fixing it at 16
	COleVariant argList[16];
	if (!getActiveXArguments(argList, nArgCount, nParams, ListData, NULL, pMethod)) {
		acedRetNil();
		return 0;
	}

	// get the AcxtiveX control
	CAxContainer *axContainer = (CAxContainer*)pControl->GetWindow();
	
	COleVariant varGet;
	// call the set property method to set the property
	HRESULT hr = axContainer->Invoke(pMethod, argList, nArgCount, varGet);
	if( FAILED(hr) )
		return RSRSLT;

	acedRetOleVar(varGet, NULL, pMethod, axContainer);
	return 0;
}

//*****************************************************************************
// 
// Method: acedRetOleVar()
// 
// Purpose: [returns an ActiveX variant to AutoLisp]
// 
// Parameters: varGet
// 
// Returns:	int
// 
//*****************************************************************************
void acedRetOleVar(COleVariant &varGet, RefCountedPtr< CPropertyObject > pProp, AxMethodDescriptor *pMethod, CAxContainer *pAxContainer, AxPropertyDescriptor *pAxProp)
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
					{
						acedRetLong(varGet.lVal);
					}				
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
					{
						acedRetLong(varGet.lVal);
					}
				}
				else
				{
					acedRetLong(varGet.lVal);
				}				
			break;
			}
		case VT_R4:
			acedRetReal(varGet.fltVal);
			break;
		case VT_R8:
			acedRetReal(varGet.dblVal);
			break;
		case VT_DATE:
			{
			COleDateTime date(varGet.date);
			ReturnDate(&date);
			break;
			}
		case VT_CY:
			{
			CY cy(varGet.cyVal);
			acedRetReal( cy.Lo);
			break;
			}
		case VT_BSTR:
#if !defined(_UNICODE) && !defined(OLE2ANSI)
		case VT_BSTRA:
#endif
			{
			CString sValue = varGet.bstrVal;
			acedRetStr(sValue);
			break;
			}
		case VT_DISPATCH:
			{
				RefCountedPtr< COleControlObject > pObject = NULL;
				
				if (pMethod != NULL)
					pObject = theArxWorkspace.GetOleControlFor(pMethod);
				else if (pProp != NULL)
				{
					if (pProp->GetAxInterfaceDescriptorPtr()->GetPropGet())
						pObject = theArxWorkspace.GetOleControlFor(pProp->GetAxInterfaceDescriptorPtr()->GetPropGet());
					else if (pProp->GetAxInterfaceDescriptorPtr()->GetProp())
						pObject = theArxWorkspace.GetOleControlFor(pProp->GetAxInterfaceDescriptorPtr()->GetProp());
					else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPut())
						pObject = theArxWorkspace.GetOleControlFor(pProp->GetAxInterfaceDescriptorPtr()->GetPropPut());
					else if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef())
						pObject = theArxWorkspace.GetOleControlFor(pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef());					
				}

				ULONG lDispatch = (ULONG)varGet.pdispVal;
				ULONG lObject = (ULONG)&*pObject;
				
				int stat;
				struct resbuf *list;    

				list = acutBuildList(
					RTLONG, lObject,
					RTLONG, lDispatch,
					RTNONE);

				if (list != NULL) 
				{ 	    
					stat = acedRetList(list);		
					acutRelRb(list); 
				} 
			break;
			}
		case VT_ERROR:
			{
			RetVal.restype = RTLONG;
			RetVal.resval.rlong = varGet.scode;
			acedRetVal(&RetVal);
			break;
			}
		case VT_BOOL:			
			{
			if (varGet.boolVal == 1)
				acedRetT();
			else
				acedRetNil();
			break;
			}
		case VT_VARIANT:
			acedRetVoid();
			//varGet.pvarVal
			break;
		case VT_UNKNOWN:
			RetVal.restype = RTLONG;
			RetVal.resval.rlong = (long)varGet.punkVal;
			
			//varGet.punkVal = argList->m_Variant[i].punkVal;
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
			break;
			}
		case VT_I4|VT_BYREF:
			{
			acedRetLong(*varGet.plVal);
			break;
			}
		case VT_R4|VT_BYREF:
			acedRetReal(*varGet.pfltVal);
			break;
		case VT_R8|VT_BYREF:
			acedRetReal(*varGet.pdblVal);
			break;
		case VT_DATE|VT_BYREF:
			{
			COleDateTime date(*varGet.pdate);
			ReturnDate(&date);
			break;
			}
		case VT_CY|VT_BYREF:
			{
			CY cy(*varGet.pcyVal);
			acedRetReal(cy.Lo);
			break;
			}
		case VT_BSTR|VT_BYREF:
			{
			CString sValue = *varGet.pbstrVal;
			acedRetStr(sValue);
			break;
			}
		case VT_DISPATCH|VT_BYREF:
			acedRetVoid();
			//varGet.ppdispVal = argList->m_Variant[i].ppdispVal;
			break;
		case VT_ERROR|VT_BYREF:
			acedRetVoid();
			RetVal.restype = RTLONG;
			RetVal.resval.rlong = *varGet.pscode;
			acedRetVal(&RetVal);
			break;
		case VT_BOOL|VT_BYREF:
			{
				if (*varGet.pboolVal == 1)
					acedRetT();
				else
					acedRetNil();
				// coerce BOOL into VARIANT_BOOL
				//BOOL* pboolVal = va_arg(argList, BOOL*);
				//*pboolVal = *pboolVal ? MAKELONG(-1, 0) : 0;
			}
			break;
		case VT_VARIANT|VT_BYREF:
			acedRetVoid();
			//varGet.pvarVal = argList->m_Variant[i].pvarVal;
			break;
		case VT_UNKNOWN|VT_BYREF:
			acedRetVoid();
			//varGet.ppunkVal = argList->m_Variant[i].ppunkVal;
			break;

		default:
			acedRetNil();
			break;
	}	
}

bool getActiveXArguments(COleVariant argList[]
												 , int &nArgCount
												 , int nParams
												 , struct resbuf* ListData
												 , AxPropertyDescriptor* pAxProp
												 , AxMethodDescriptor* pAxMethod) {
	//[DPR] argList must be built backward for SetProperty to work properly.
	//I tried reversing the list in SetProperty, but couldn't get variants to copy.
	//Decided to move out the reversing out here instead.

	//We have a fixed upper limit of 16 variables
	if (nParams > 16) {
		return false;
	}
	if (pAxMethod == NULL && pAxProp == NULL) {
		return false;
	}

	for (nArgCount = 0; nArgCount < nParams; nArgCount++) {
		VARTYPE vt;
		if (pAxProp != NULL) {
			vt = pAxProp->GetArgs()[nArgCount].vt;
		} else if (pAxMethod != NULL) {
			vt = pAxMethod->GetArgs()[nArgCount].vt;
		}
		if (!GetAxPropertyArgument(
			ListData
			, &argList[nParams - nArgCount - 1]
			, vt) )
		{
			return false;
		}
		ListData = ListData->rbnext;
		if (ListData == NULL) {
			nArgCount++;
			break;
		}
	}
	return true;
}