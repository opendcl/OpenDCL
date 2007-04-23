// Methods_Properties.cpp : implementation file
//

#include "stdafx.h"
#include "ArgumentsRetrieval.h"
#include "Methods_Properties.h"
#include "UserMessageId.h"
#include "ErrorLexicon.h"
#include "MethodLexicon.h"
#include "VdclRadioButton.h"
#include "OdclEdit.h"
#include "ArxGraphicButtonCtrl.h"
#include "DwgDirList.h"
#include "OdclEdit.h"
#include "OdclListCtrl.h"
#include "OdclMonth.h"
#include "OptionListBox.h"
#include "OdclLayerCombo.h"
#include "RoundSliderCtrl.h"
#include "PictureBox.h"
#include "DwgPreviewCtrl.h"
#include "ButtonST.h"
#include "FontCombo.h"
#include "Splitter.h"
#include "StaticLink.h"
#include "GsPreviewCtrl.h"
#include "SlideHolder.h"
#include "VdclAngleEdit.h"
#include "VdclArrowHeadComboBox.h"
#include "VdclCheckBox.h"
#include "VdclColorComboBox.h"
#include "VdclComboBox.h"
#include "VdclComboBoxEx.h"
#include "VdclGroupBox.h"
#include "VdclLineWeightComboBox.h"
#include "VdclListBox.h"
#include "VdclNumericEdit.h"
#include "VdclPlotStyleNamesComboBox.h"
#include "VdclPlotStyleTablesComboBox.h"
#include "VdclProgressCtrl.h"
#include "VdclRadioButton.h"
#include "VdclScrollBar.h"
#include "VdclSliderCtrl.h"
#include "VdclSpinButton.h"
#include "VdclStatic.h"
#include "VdclSymbolEdit.h"
#include "VdclTab.h"
#include "VdclTree.h"
#include "VdclTextButton.h"
#include "HtmlCtrl.h"
#include "PrinterComboBox.h"
#include "AxContainerCtrl.h"
#include "ComboBoxFolder.h"
#include "ArxGridCtrl.h"
#include "ToolTips.h"
#include "PropertyNames.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlTypes.h"
#include "DclFormObject.h"
#include "ArxWorkspace.h"
#include "ArxControlPane.h"


const int nBufSize = 100;
const int nFalse = 0;
const TCHAR sT[] = _T("T");
const TCHAR sGetPropertry [] = _T("Odcl_Control_Get");
const TCHAR sSetPropertry [] = _T("Odcl_Control_Set");

bool SetPropertyObject(RefCountedPtr< CPropertyObject > pProperty, struct resbuf *ListData, CDclControlObject *pArxObject);



//*****************************************************************************
// 
// Method: SetCtrlProperty(PropertyId id)
// 
// Purpose: [sets the value of a property]
// 
// Parameters:  
// 
// Returns:	int
// 
//*****************************************************************************
bool SetCtrlProperty(PropertyId id)
{
	CString sProp = CString(sSetPropertry) + GetPropertyName(id);

	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sProp, pArxObject);
	if (pArxObject == NULL)
		return false;

	RefCountedPtr< CPropertyObject > pProperty = pArxObject->GetPropertyObject(id);
	if (pProperty == NULL)
		return false;

	// get the control
	CWnd *pControl = pArxObject->GetWindow();
	if (pControl == NULL)		
		return false;

	SetPropertyObject(pProperty, ListData->rbnext, pArxObject);
	UpdateControl(pControl, pProperty, pArxObject, id);

	return true;
}

//*****************************************************************************
// 
// Method: GetCtrlProperty(PropertyId id)
// 
// Purpose: [gets the value of a property]
// 
// Parameters:  
// 
// Returns:	int
// 
//*****************************************************************************
bool GetCtrlProperty(PropertyId id)
{
	CString sProp = CString(sGetPropertry) + GetPropertyName(id);

	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sProp, pArxObject);
	if (pArxObject == NULL)
		return false;

	RefCountedPtr< CPropertyObject > pProperty = pArxObject->GetPropertyObject(id);
	if (pProperty == NULL)
		return false;

	// here we need to do special position lookups for width, height, left and top properties.
	if (id == nWidth)
	{
		CRect rc;
		pArxObject->GetWindow()->GetWindowRect(rc);
		acedRetInt(rc.Width());
		return true;
	}
	else if (id == nHeight)
	{
		CRect rc;
		pArxObject->GetWindow()->GetWindowRect(rc);
		acedRetInt(rc.Height());
		return true;
	}
	else if (id == nTop)
	{
		CRect rc;
		pArxObject->GetWindow()->GetWindowRect(rc);
		pArxObject->GetWindow()->GetParent()->ScreenToClient(rc);
		acedRetInt(rc.top);
		return true;
	}
	else if (id == nLeft)
	{
		CRect rc;
		pArxObject->GetWindow()->GetWindowRect(rc);
		pArxObject->GetWindow()->GetParent()->ScreenToClient(rc);
		acedRetInt(rc.left);
		return true;
	}

	// if this control is a radio or option button
	// and the request property is the value
	// we have to query the control and not the stored
	// property for it's value
	if (id == nValue && pArxObject->GetType() == CtlOptionButton)
	{
		int nCheckValue = ((VdclRadioButton*)pArxObject->GetWindow())->GetCheck();
		// this is important, nValue must equal 1 because it may return another value
		if (nCheckValue == 1)
			pProperty->SetBooleanValue(true);
		else
			pProperty->SetBooleanValue(false);
	}
	if (pProperty == NULL)
	{
		theWorkspace.DisplayAlert(CString (ErrorTheProperty) + GetPropertyName(id) + ErrorPropertyWasNotFound + ErrorForControl + pArxObject->GetStrProperty(nName));
		// return a negitive value to indicate to the calling autolisp function that the call failed
		acedRetInt(-1);
		return false;
	}

	SetReturnValue(pProperty->GetType(), pProperty);
	return true;
}

bool SetPropertyObject(RefCountedPtr< CPropertyObject > pProperty, struct resbuf *ListData, CDclControlObject *pCtrl)
{
	// here we are going to accept any numeric value for a text property and convert it to text.
	// just to make it easier for the programmers.
	if (pProperty->GetType() == PropString || pProperty->GetID() == nText)
	{
		TCHAR value[80];
			
		switch (ListData->restype)
		{
		case RTSTR:
			pProperty->SetStringValue(ListData->resval.rstring);
			break;
		case RTT:
			pProperty->SetStringValue(sT);
			break;
		case RTNIL:
			pProperty->SetStringValue(NULL);
			break;
		case RTLONG:
			acdbRToS(ListData->resval.rlong, -1, -1, value);				
			pProperty->SetStringValue(value);
			break;
		case RTSHORT:
			acdbRToS(ListData->resval.rint, -1, -1, value);				
			pProperty->SetStringValue(value);
			break;
		case RTANG:			
			acdbAngToS(ListData->resval.rreal, -1, -1, value);				
			pProperty->SetStringValue(value);
			break;
		case RTREAL:			
		case RTORINT:
			{
			// if the control is a text box we need to find out if it's an AngleEdit type
			// so we can convert the double as an angle unit and not a real unit.
			if (pCtrl->GetType() == CtlTextBox && pCtrl->GetLngProperty(nFilterStyle) == EditFilter_Angle)
				acdbAngToS(ListData->resval.rreal, -1, -1, value);
			else
				acdbRToS(ListData->resval.rreal, -1, -1, value);
			
			pProperty->SetStringValue(value);
			break;
			}
		}
		pProperty->SetType(PropString);	
		return true;
	}

	

	// test to ensure the type received and the type pass were the same
	if (pProperty->GetType() == PropLong ||
		pProperty->GetType() == PropEnum ||
		pProperty->GetType() == PropPicture)
	{
		// if pass then set the property
		if (ListData->restype == RTLONG)
			pProperty->SetLongValue(ListData->resval.rlong);
		else if (ListData->restype == RTSHORT)
			pProperty->SetLongValue(ListData->resval.rint);
		else if (ListData->restype == RTREAL ||
				 ListData->restype == RTORINT ||
				 ListData->restype == RTANG)
			pProperty->SetLongValue((long)ListData->resval.rreal);
		return true;
	}
	else if (pProperty->GetType() == PropBool)
	{
		// if pass then set the property
		if (ListData->restype == RTT)
			pProperty->SetBooleanValue(true);
		else if (ListData->restype == RTNIL)
			pProperty->SetBooleanValue(false);
		else if (ListData->restype == RTSHORT)
			pProperty->SetBooleanValue(ListData->resval.rint > 0);
		else if (ListData->restype == RTLONG)
			pProperty->SetBooleanValue(ListData->resval.rlong > 0);
		else if (ListData->restype == RTREAL ||
				 ListData->restype == RTORINT ||
				 ListData->restype == RTANG)
			pProperty->SetBooleanValue(ListData->resval.rreal > 0.0);
		return true;
	}
	else if (pProperty->GetType() == PropDouble)
	{
		if (ListData->restype == RTSHORT)
			pProperty->SetDoubleValue(ListData->resval.rint);
		else if (ListData->restype == RTLONG)
			pProperty->SetDoubleValue(ListData->resval.rlong);
		else if (ListData->restype == RTREAL ||
				 ListData->restype == RTORINT ||
				 ListData->restype == RTANG)
			pProperty->SetDoubleValue(ListData->resval.rreal);
		return true;
	}
	else
	{
		return false;		
	}

	return true;

}

//*****************************************************************************
// 
// Method: SetProperty()
// 
// Purpose: [Sets a property of the requested control]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int SetProperty()
{
	int nArgs = 0;	
	CString sPropNameArg;

	// call the method to get the property object
	CDclControlObject *pArxObject = GetControlArxObject(sSetProperty, &nArgs);
	

	if (!GetStringArgument(nArgs, &sPropNameArg, sSetProperty) || pArxObject == NULL)
	{
		if (!Property_SetByList(pArxObject))
		{
			// return a negitive value to indicate to the calling autolisp function that the call failed
			acedRetInt(-1);
			return 0;
		}
		else
		{
			acedRetVoid();
			return 0;
		}
	}
	nArgs++;
	PropertyId nPropertyId = GetPropertyId(sPropNameArg);
	
	RefCountedPtr< CPropertyObject > pProperty = pArxObject->GetPropertyObject(nPropertyId);

	if (pProperty == NULL)
	{
		// return a negitive value to indicate to the calling autolisp function that the call failed
		acedRetInt(-1);
		return 0;
	}

	// get the argument sent by AutoLISP and return it as a CPropertyObject pointer
	RefCountedPtr< CPropertyObject > pPropArg = GetPropertyArgument(nArgs, sSetProperty);

	if (pPropArg == NULL)
	{
		// return a negitive value to indicate to the calling autolisp function that the call failed
		acedRetInt(-1);
		return 0;
	}

	if (!SetPropertyObject(pProperty, pPropArg, pArxObject))
	{
		// set this up latter to tell the user what was received and what it expected
		theWorkspace.DisplayAlert(CString(ErrorWrongArgTypeForProp) + ErrorForControl + pArxObject->GetStrProperty(nName));
		// return a negitive value to indicate to the calling autolisp function that the call failed
		acedRetInt(-1);
		return 0;
	}

	// get the control
	CWnd *pControl = pArxObject->GetWindow();
	
	if (pControl == NULL)		
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	UpdateControl(pControl, pProperty, pArxObject, nPropertyId);

	acedRetVoid();
	return 0;

}


bool Property_SetByList(CDclControlObject *pArxObject)
{
	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < 3; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		return false;
	}

	if (ListData->restype != RTLB) 
		return false;

	if (ListData->restype == RTLB) 
	{		
		// get the control
		CWnd *pControl = GetControlPointer(pArxObject->GetType(), sSetProperty);
		
		if (pControl == NULL)		
		{
			// return nil
			acedRetInt(-1);
			return 0;
		}
		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSetProperty + ErrorForControl + pArxObject->GetStrProperty(nName));	
				
				return false;
			}
		
			if (ListData->restype == RTLE) 
				return true;

			if (ListData->restype != RTSTR) 
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sSetProperty + ErrorForControl + pArxObject->GetStrProperty(nName));	
				return false;
			}				
				
			// get the next argument required
			CString sPropertyName = ListData->resval.rstring;
		
			PropertyId nPropertyId = GetPropertyId(sPropertyName);

			RefCountedPtr< CPropertyObject > pProperty = pArxObject->GetPropertyObject(nPropertyId);
								
			ListData = ListData->rbnext;

			RefCountedPtr< CPropertyObject > pNewProperty;

			switch (ListData->restype)
			{	
				case RTREAL:
				case RTANG:
					{
						pNewProperty = new CPropertyObject(PropDouble);
						// get the first argument required
						pNewProperty->SetDoubleValue(ListData->resval.rreal);				
						break;
					}
				case RTSHORT:
				case RTLONG:
					{
						pNewProperty = new CPropertyObject(PropLong);
						// get the first argument required
						pNewProperty->SetLongValue(ListData->resval.rint);
						break;
					}
				case RTSTR:
					{
						pNewProperty = new CPropertyObject(PropString);
						// get the first argument required
						pNewProperty->SetStringValue(ListData->resval.rstring);				
						break;
					}
				case RTT:
					{
						pNewProperty = new CPropertyObject(PropBool);
						// get the first argument required
						pNewProperty->SetBooleanValue(true);				
						break;
					}
				case RTNIL:
					{
						pNewProperty = new CPropertyObject(PropBool);
						// get the first argument required
						pNewProperty->SetBooleanValue(false);				
						break;
					}
				default:
					{
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(
							CString(ErrorInappropriateFound)
							+ sSetProperty
							+ ErrorForControl + pArxObject->GetStrProperty(nName)); 			
						return false; 
						break;
					}
			}
			if (!SetPropertyObject(pProperty, pNewProperty))
			{
				// set this up latter to tell the user what was received and what it expected
				theWorkspace.DisplayAlert(CString(ErrorWrongArgTypeForProp) + ErrorForControl + pArxObject->GetStrProperty(nName));
				return false;
			}

			// call method to update the control	
			UpdateControl(pControl, pProperty, pArxObject, nPropertyId);	
		}
	}
	else
	{	
        return false;
	}	
	return true;
}

bool SetPropertyObject(RefCountedPtr< CPropertyObject > pProperty, RefCountedPtr< CPropertyObject > pPropArg, CDclControlObject *pCtrl)
{
	// here we are going to accept any numeric value for a text property and convert it to text.
	// just to make it easier for the programmers.
	if (pProperty->GetID() == nText)
	{
		switch (pPropArg->GetType())
		{
		case PropBool:
			pPropArg->SetStringValue(sT);
			break;
		case PropLong:
		case PropEnum:
		case PropPicture:
			TCHAR value[80];
			acdbRToS(pPropArg->GetLongValue(), 2, 0, value);				
			pPropArg->SetStringValue(value);
			break;
		case PropDouble:
			{
			// if the control is a text box we need to find out if it's an AngleEdit type
			// so we can convert the double as an angle unit and not a real unit.
			if (pCtrl->GetType() == CtlTextBox && pCtrl->GetLngProperty(nFilterStyle) == EditFilter_Angle)
			{
				TCHAR value[80];
				acdbAngToS(pPropArg->GetDoubleValue(), -1, -1, value);
				pPropArg->SetStringValue(value);
			}		
			else
			{
				TCHAR value[80];
				acdbRToS(pPropArg->GetDoubleValue(), -1, -1, value);
				pPropArg->SetStringValue(value);
			}
			break;
			}
		}
		pPropArg->SetType(PropString);		
	}

	// test to ensure the type received and the type pass were the same
	if (pProperty->GetType() == PropLong && pPropArg->GetType() == PropLong)
	{
		// if pass then set the property
		pProperty->SetLongValue(pPropArg->GetLongValue());
	
	}
	// test to ensure the type received and the type pass were the same
	else if (pProperty->GetType() == PropEnum && pPropArg->GetType() == PropEnum)
	{
		// if pass then set the property
		pProperty->SetLongValue(pPropArg->GetLongValue());
	
	}
	// test to ensure the type received and the type pass were the same
	else if (pProperty->GetType() == PropPicture && pPropArg->GetType() == PropPicture)
	{
		// if pass then set the property
		pProperty->SetLongValue(pPropArg->GetLongValue());
	
	}
	// test to ensure the type received and the type pass were the same
	else if (pProperty->GetType() == PropString && pPropArg->GetType() == PropString)
	{
		// if pass then set the property
		pProperty->SetStringValue(pPropArg->GetStringValue());
	
	}
	else if (pProperty->GetType() == PropBool && pPropArg->GetType() == PropBool)
	{
		// if pass then set the property
		pProperty->SetBooleanValue(pPropArg->GetBooleanValue());
	
	}
	else if (pProperty->GetType() == pPropArg->GetType())
	{
		// if pass then set the property
		pProperty->SetStringValue(pPropArg->GetStdProperty());
	}
	else if (pProperty->GetType() == PropBool && pPropArg->GetType() == PropLong)
	{
		// if pass then set the property
		pProperty->SetBooleanValue(pPropArg->GetLongValue() != 0);
	}	
	else if (pProperty->GetType() == PropLong && pPropArg->GetType() == PropBool)
	{
		// if pass then set the property
		pProperty->SetLongValue(pPropArg->GetBooleanValue());
	}	
	else if (pProperty->GetType() == PropEnum && pPropArg->GetType() == PropLong)
	{
		// if pass then set the property
		pProperty->SetLongValue(pPropArg->GetLongValue());
	}	
	else if (pProperty->GetType() == PropPicture)
	{
		// if pass then set the property
		pProperty->SetLongValue(pPropArg->GetLongValue());
		
	}	
	else
	{
		return false;		
	}

	return true;

}

void UpdateControl(CWnd *pControl,RefCountedPtr< CPropertyObject > pProperty, CDclControlObject *pTemplate, PropertyId nPropertyId)
{
	CRect rcThisControl;
	pControl->GetWindowRect(&rcThisControl);
	
	switch (pProperty->GetID())
	{
	case nLeftFromRight:
	case nRightFromRight:
	case nTopFromBottom:
	case nBottomFromBottom:
	case nLeft:
	case nTop:
	case nWidth:
	case nHeight:
		{
		// call the method to update the control
			pTemplate->GetControlInstance()->GetControlPane()->ResetControlsPos(pTemplate);
		break;
		}
	// if the font size property has been found pass it the font name only
	// and set it's size properly
	case nLabelSize:
		{
			CArxDialogControl::UpdateProperty(pTemplate,
																				pTemplate->GetControlInstance()->GetControlPane(),
																				pTemplate->GetControlInstance()->GetControlId(),
																				nLabelName);
		break;
		}
	// if a font property has been found pass it the font name only
	// otherwise the font will not be updated.
	case nLabelUnderline:
	case nLabelItalic:
	case nLabelBold:
	case nLabelStrikeOut:
		{
		// call the method to update the control
		CArxDialogControl::UpdateProperty(pTemplate,
																			pTemplate->GetControlInstance()->GetControlPane(),
																			pTemplate->GetControlInstance()->GetControlId(),
																			nLabelName);
		break;
		}
	default:
		{
			
		// call the method to update the control
		CArxDialogControl::UpdateProperty(pTemplate,
																			pTemplate->GetControlInstance()->GetControlPane(),
																			pTemplate->GetControlInstance()->GetControlId(),
																			nPropertyId);
		break;
		}
	}

}

int GetProperty()
{
	int nArgs = 0;
	CString sPropNameArg;

	// call the method to get the property object
	CDclControlObject *pArxObject = GetControlArxObject(sGetProperty, &nArgs);
	
	if (!GetStringArgument(nArgs, &sPropNameArg, sGetProperty) || pArxObject == NULL)
	{
		// return a negitive value to indicate to the calling autolisp function that the call failed
		acedRetInt(-1);
		return 0;
	}

	PropertyId nPropertyId = GetPropertyId(sPropNameArg);

	// here we need to do special position lookups for width, height, left and top properties.
	if (nPropertyId == nWidth)
	{
		CRect rc;
		pArxObject->GetWindow()->GetWindowRect(rc);
		acedRetInt(rc.Width());
		return 0;
	}
	else if (nPropertyId == nHeight)
	{
		CRect rc;
		pArxObject->GetWindow()->GetWindowRect(rc);
		acedRetInt(rc.Height());
		return 0;
	}
	else if (nPropertyId == nTop)
	{
		CRect rc;
		pArxObject->GetWindow()->GetWindowRect(rc);
		pArxObject->GetWindow()->GetParent()->ScreenToClient(rc);
		acedRetInt(rc.top);
		return 0;
	}
	else if (nPropertyId == nLeft)
	{
		CRect rc;
		pArxObject->GetWindow()->GetWindowRect(rc);
		pArxObject->GetWindow()->GetParent()->ScreenToClient(rc);
		acedRetInt(rc.top);
		return 0;
	}

	RefCountedPtr< CPropertyObject > pProperty = pArxObject->GetPropertyObject(nPropertyId);

	// if this control is a radio or option button
	// and the request property is the value
	// we have to query the control and not the stored
	// property for it's value
	if (nPropertyId == nValue &&
		pArxObject->GetType() == CtlOptionButton)
	{
		int nValue = ((VdclRadioButton*)pArxObject->GetWindow())->GetCheck();
		// this is important, nValue must equal 1 because it may return another value
		if (nValue == 1)
			pProperty->SetBooleanValue(true);
		else
			pProperty->SetBooleanValue(false);
	}
	if (pProperty == NULL)
	{
		theWorkspace.DisplayAlert(CString (ErrorTheProperty) + sPropNameArg + ErrorPropertyWasNotFound + ErrorForControl + pArxObject->GetStrProperty(nName));
		// return a negitive value to indicate to the calling autolisp function that the call failed
		acedRetInt(-1);
		return 0;
	}

	SetReturnValue(pProperty->GetType(), pProperty);
	return 0;

}

void SetReturnValue(int nType, RefCountedPtr< CPropertyObject > pProperty)
{
	if (nType == PropBool)
	{
		if (pProperty->GetBooleanValue() == TRUE)
			acedRetT();
		else
			acedRetNil();
		return;
	}
	
	if (pProperty->GetType() == PropLong ||
		pProperty->GetType() == PropEnum ||
		pProperty->GetType() == PropPicture)
	{
		acedRetInt(pProperty->GetLongValue());
		return;
	}
	
	if (pProperty->GetType() == PropString ||
			pProperty->GetType() == PropEvent)
	{
		acedRetStr(pProperty->GetStringValue());
		return;
	}
	
	if (pProperty->GetType() == PropDouble)
	{
		acedRetReal(pProperty->GetDoubleValue());
		return;
	}	
}

int SetControlFocus()
{
	// call the method to get the property object
	CDclControlObject *pArxObject = GetControlArxObject(sSetFocus);
	if (pArxObject == NULL)
	{
		acedRetInt(-1);
		return 0;
	}

	// get the control
	CWnd *pControl = pArxObject->GetWindow();
	if (pControl == NULL)		
	{
		acedRetInt(-1);
		return 0;
	}

	if (!pControl->GetParent()->IsWindowVisible())		
	{
		// return nil
		acedRetVoid();	
		return 0;
	}

	pControl->SetFocus();
	acedRetVoid();
	return 0;
}
int ZOrder()
{
	int nArg;
	// call the method to get the property object
	CDclControlObject *pArxObject = GetControlArxObject(sZOrder, &nArg);

	if (pArxObject == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	// get the control
	CWnd *pControl = pArxObject->GetWindow();

	int nZOrder;
	if (!GetIntArgument(nArg, &nZOrder, sZOrder) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (nZOrder == 0)
		pControl->SetWindowPos(&CWnd::wndBottom, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	else
		pControl->SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);

	acedRetVoid();
	return 0;
}


int Control_GetCurPos()
{
	// call the method to get the property object
	CDclControlObject *pArxObject = GetControlArxObject(sControl_GetCurPos);
	
	if (pArxObject == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	// get the control
	CWnd *pControl = pArxObject->GetWindow();//GetControlPointer(pArxObject->GetType(), sZOrder);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CRect rcCtrl;
	pControl->GetWindowRect(&rcCtrl);
	pControl->GetParent()->ScreenToClient(rcCtrl);
	
	// this code is for all other dialogs
	int stat;
	struct resbuf *list;    

	list = acutBuildList(
		RTSHORT, rcCtrl.left,
		RTSHORT, rcCtrl.top,
		RTSHORT, rcCtrl.Width(),
		RTSHORT, rcCtrl.Height(),
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 
	else 
		acedRetNil();
	return 0;
}


int Control_SetPos()
{	
	if (!DoSetPosByList())
	{
		// return nil
		acedRetInt(-1);
	}
	else 
		acedRetVoid();

	return 0;
}

bool DoSetPosByPtrList()
{
	long lPointer = 0;
	int nLeftValue;
	int nTopValue;
	int nWidthValue;
	int nHeightValue;
	
	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();

	if (ListData == NULL)
		return false;
	
	// get the long point argument required
	if (ListData->restype == RTLONG) 
		lPointer = ListData->resval.rlong;
	else if (ListData->restype == RTREAL) 
		lPointer =(long) ListData->resval.rreal;
	else if (ListData->restype == RTSHORT) 
		lPointer = ListData->resval.rint;
	
	ListData = ListData->rbnext;
	if (ListData == NULL)
		return false;

	CDclControlObject *pControl = NULL;

	if (lPointer != 0)
	{
		pControl = (CDclControlObject*)lPointer;

		if (pControl == NULL)
		{
			// inform the programmer that he did not make the correct call
			theWorkspace.DisplayAlert(CString(ErrorControlNotFound) + sControl_SetPos);	
			return false;
		}						
	}
	
	if (ListData->restype == RTPOINT)
	{
		nLeftValue = (int)ListData->resval.rpoint[0];
		nTopValue = (int)ListData->resval.rpoint[1];		

		CRect rcCtrl;
		pControl->GetWindow()->GetWindowRect(&rcCtrl);
		nHeightValue = rcCtrl.Height();
		nWidthValue	= rcCtrl.Width();

		CRect rcNew(
			nLeftValue,
			nTopValue,
			nLeftValue + nWidthValue,
			nTopValue + nHeightValue);

		pControl->GetWindow()->MoveWindow(rcNew, pControl->GetWindow()->IsWindowVisible());
		
	}
	else if (ListData->restype != RTLB) 
	{		
			
		if (ListData->restype == RTSHORT)
			// get the next argument required
			nLeftValue = ListData->resval.rint;
		else if (ListData->restype == RTLONG) 
			// get the next argument required
			nLeftValue = ListData->resval.rlong;
		else if (ListData->restype == RTREAL) 
			// get the next argument required
			nLeftValue = (int)ListData->resval.rreal;
		else
		{
			// inform the programmer that he did not make the correct call
			theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sControl_SetPos + ErrorFile);	
			return false;
		}				
	
		
		ListData = ListData->rbnext;
		if (ListData == NULL)
		{
			// inform the programmer that he did not make the correct call
			theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sControl_SetPos + ErrorFile);	
			return false;
		}
		
		if (ListData->restype == RTSHORT)
			// get the next argument required
			nTopValue = ListData->resval.rint;
		else if (ListData->restype == RTLONG) 
			// get the next argument required
			nTopValue = ListData->resval.rlong;
		else if (ListData->restype == RTREAL) 
			// get the next argument required
			nTopValue = (int)ListData->resval.rreal;
		else
		{
			// inform the programmer that he did not make the correct call
			theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sControl_SetPos + ErrorFile);	
			return false;
		}		

		bool bListEndFound = false;

		// advance to the first list item
		ListData = ListData->rbnext;
		
		if (ListData == NULL) 
		{
			bListEndFound = true;
			CRect rcCtrl;
			pControl->GetWindow()->GetWindowRect(&rcCtrl);
			nHeightValue = rcCtrl.Height();
			nWidthValue	= rcCtrl.Width();
		}
			
		if (!bListEndFound) 
		{
			if (ListData->restype == RTSHORT)
				// get the next argument required
				nWidthValue = ListData->resval.rint;
			else if (ListData->restype == RTLONG) 
				// get the next argument required
				nWidthValue = ListData->resval.rlong;
			else if (ListData->restype == RTREAL) 
				// get the next argument required
				nWidthValue = (int)ListData->resval.rreal;
			else
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sControl_SetPos + ErrorFile);	
				return false;
			}		

			// advance to the next list item
			ListData = ListData->rbnext;
			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sControl_SetPos + ErrorFile);	
				return false;
			}
			if (ListData->restype == RTSHORT)
				// get the next argument required
				nHeightValue = ListData->resval.rint;
			else if (ListData->restype == RTLONG) 
				// get the next argument required
				nHeightValue = ListData->resval.rlong;
			else if (ListData->restype == RTREAL) 
				// get the next argument required
				nHeightValue = (int)ListData->resval.rreal;
			else
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sControl_SetPos + ErrorFile);	
				return false;
			}				
		}
		
		CRect rcNew(
			nLeftValue,
			nTopValue,
			nLeftValue + nWidthValue,
			nTopValue + nHeightValue);

		pControl->GetWindow()->MoveWindow(rcNew, pControl->GetWindow()->IsWindowVisible());
		return true;
	}
	else if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData == NULL)
			{
				return true;
			}
			
			if (ListData->restype == NULL) 
			{
				return false;
			}
			
			if (ListData->restype == RTLE) 
			{
				return true;
			}
			
			if (ListData->restype == RTLB) 
				// advance to the first list item
				ListData = ListData->rbnext;

			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sControl_SetPos + ErrorFile);	
				return false;
			}
		
			if (ListData->restype == NULL) 
			{
				return false;
			}
			
			if (ListData->restype == RTLE) 
			{
				return true;
			}
				
			// get the long point argument required
			if (ListData->restype == RTREAL) 
				lPointer = (long)ListData->resval.rreal;
			if (ListData->restype == RTSHORT) 
				lPointer = (int)ListData->resval.rint;
			if (ListData->restype == RTLONG) 
				lPointer = (long)ListData->resval.rlong;
			
			
			CDclControlObject *pControl = (CDclControlObject*)lPointer;

			if (pControl == NULL)
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorControlNotFound) + sControl_SetPos + ErrorFile);	
				return false;
			}				
				
			ListData = ListData->rbnext;
			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sControl_SetPos + ErrorFile);	
				return false;
			}
			
			if (ListData->restype != RTSHORT) 
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sControl_SetPos + ErrorFile);	
				return false;
			}				
		
			// get the next argument required
			nLeftValue = ListData->resval.rint;
			
			ListData = ListData->rbnext;
			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sControl_SetPos + ErrorFile);	
				return false;
			}
			
			if (ListData->restype != RTSHORT) 
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sControl_SetPos + ErrorFile);	
				return false;
			}				
		
			// get the next argument required
			nTopValue = ListData->resval.rint;
			
			bool bListEndFound = false;

			// advance to the first list item
			ListData = ListData->rbnext;
			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sControl_SetPos + ErrorFile);	
				return false;
			}

			if (ListData->restype == RTLE) 
			{
				bListEndFound = true;
				CRect rcCtrl;
				pControl->GetWindow()->GetWindowRect(&rcCtrl);
				nHeightValue = rcCtrl.Height();
				nWidthValue	= rcCtrl.Width();
			}
				
			if (ListData->restype != RTLE) 
			{
				if (ListData->restype != RTSHORT) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sControl_SetPos + ErrorFile);	
					return false;
				}
				// get the first argument required
				nWidthValue = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sControl_SetPos + ErrorFile);	
					return false;
				}
				if (ListData->restype != RTSHORT) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sControl_SetPos + ErrorFile);	
					return false;
				}
				
				// get the second argument required
				nHeightValue = ListData->resval.rint;					
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData->restype != RTLE) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sControl_SetPos + ErrorFile);	
					return false;
				}
				else
					bListEndFound = true;
				
			}
			
			CRect rcNew(
				nLeftValue,
				nTopValue,
				nLeftValue + nWidthValue,
				nTopValue + nHeightValue);

			
			pControl->GetWindow()->MoveWindow(rcNew, pControl->GetWindow()->IsWindowVisible());
				
			if (bListEndFound == false)
			{
				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sControl_SetPos + ErrorFile);	
					return false;
				}			
			}	
		}
	}
	else
	{	
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sControl_SetPos + ErrorFile);			
        return false;
	}	
	return true;
}


bool DoSetPosByList()
{
	CString sProjectName;
	CString sFormName;
	
	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();

	if (ListData == NULL)
		return false;
	
	// if a point variable was used (a pointer cast to a long and set using)
	if (ListData->restype == RTSHORT || ListData->restype == RTLONG || ListData->restype == RTREAL || ListData->restype == RTLB) 
	{
		// call DoSetPosByPtrList instead and exit here
		return DoSetPosByPtrList();
	}
	if (ListData->restype != RTSTR) 
		return false;

	// get the next argument required
	sProjectName = ListData->resval.rstring;
				
	ListData = ListData->rbnext;
	if (ListData == NULL)
		return false;
	
	// get the next argument required
	sFormName = ListData->resval.rstring;
	
	ListData = ListData->rbnext;
	if (ListData == NULL)
		return false;

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		int nLeftValue;
		int nTopValue;
		int nWidthValue;
		int nHeightValue;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData == NULL)
			{
				return true;
			}
			
			if (ListData->restype == NULL) 
			{
				return false;
			}
			
			if (ListData->restype == RTLE) 
			{
				return true;
			}
			
			if (ListData->restype == RTLB) 
				// advance to the first list item
				ListData = ListData->rbnext;

			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
				return false;
			}
		
			if (ListData->restype == NULL) 
			{
				return false;
			}
			
			if (ListData->restype == RTLE) 
			{
				return true;
			}
			
				
			
			if (ListData->restype != RTSTR) 
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
				return false;
			}				
			
			// get the next argument required
			CString sControlName = ListData->resval.rstring;
			
			CDclControlObject *pControl = theArxWorkspace.FindControl(sProjectName, sFormName, sControlName);

			if (pControl == NULL)
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorControlNotFound) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
				return false;
			}				
				
			ListData = ListData->rbnext;
			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
				return false;
			}
			
			if (ListData->restype != RTSHORT) 
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
				return false;
			}				
		
			// get the next argument required
			nLeftValue = ListData->resval.rint;
			
			ListData = ListData->rbnext;
			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
				return false;
			}
			
			if (ListData->restype != RTSHORT) 
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
				return false;
			}				
		
			// get the next argument required
			nTopValue = ListData->resval.rint;
			
			bool bListEndFound = false;

			// advance to the first list item
			ListData = ListData->rbnext;
			if (ListData == NULL)
			{
				// inform the programmer that he did not make the correct call
				theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
				return false;
			}

			if (ListData->restype == RTLE) 
			{
				bListEndFound = true;
				CRect rcCtrl;
				pControl->GetWindow()->GetWindowRect(&rcCtrl);
				nHeightValue = rcCtrl.Height();
				nWidthValue	= rcCtrl.Width();
			}
				
			if (ListData->restype != RTLE) 
			{
				if (ListData->restype != RTSHORT) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
					return false;
				}
				// get the first argument required
				nWidthValue = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
					return false;
				}
				if (ListData->restype != RTSHORT) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
					return false;
				}
				
				// get the second argument required
				nHeightValue = ListData->resval.rint;					
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData->restype != RTLE) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
					return false;
				}
				else
					bListEndFound = true;
				
			}
			
			CRect rcNew(
				nLeftValue,
				nTopValue,
				nLeftValue + nWidthValue,
				nTopValue + nHeightValue);

			
			pControl->GetWindow()->MoveWindow(rcNew, pControl->GetWindow()->IsWindowVisible());
				
			if (bListEndFound == false)
			{
				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);	
					return false;
				}			
			}
		}
	}
	else
	{	
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sControl_SetPos + ErrorFile + sProjectName + ErrorDialog + sFormName);			
        return false;
	}	
	return true;
}

int ForceUpdateNow()
{
	// call the method to get the property object
	CDclControlObject *pArxObject = GetControlArxObject(sForceUpdateNow);

	if (pArxObject == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	// get the control
	CWnd *pControl = pArxObject->GetWindow();

	pControl->RedrawWindow();

	acedRetVoid();
	return 0;
}

int ShowToolTip()
{
	CPoint pt(-1,-1);
	CDclControlObject *pArxObject = GetLispInput(sShowToolTip, pt);

	CWnd *pControl  = pArxObject->GetWindow();

	CRect rc;
	pControl->GetWindowRect(rc);
	if (pt.x == -1 && pt.y == -1)
		pt = rc.CenterPoint();

	PPTOOLTIP_INFO TI;
	switch (pArxObject->GetType())
	{
	case CtlDwgPreview:
		{
		CDwgPreviewCtrl* pCtrl = (CDwgPreviewCtrl*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlBlockView:
	case CtlHatch:
		{
		CGsPreviewCtrl *pCtrl = (CGsPreviewCtrl*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlSlideView:
		{
		CSlideHolder *pCtrl = (CSlideHolder*)pControl;
		if( pCtrl->GetToolTipCtrl().GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->GetToolTipCtrl().ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlRoundSlider:
		{
		CRoundSliderCtrl *pCtrl = (CRoundSliderCtrl*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlStaticURL:
		{
		CStaticLink *pCtrl = (CStaticLink*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlProgress:
		{
		VdclProgressCtrl *pCtrl = (VdclProgressCtrl*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlMonth:
		{
		OdclMonth *pCtrl = (OdclMonth*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlSlider:
		{
		VdclSliderCtrl *pCtrl = (VdclSliderCtrl*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlPictureBox:
		{
		CPictureBox *pCtrl = (CPictureBox*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlListBox:
		{
		VdclListBox *pCtrl = (VdclListBox*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlOptionButton:
		{
		VdclRadioButton *pCtrl = (VdclRadioButton*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlOptionList:
		{
		COptionListBox *pCtrl = (COptionListBox*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlCheckBox:
		{
		VdclCheckBox *pCtrl = (VdclCheckBox*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlTextBox:
		{
			switch (pArxObject->GetLngProperty(nFilterStyle))
			{
				case EditFilter_Symbol:
				{
					VdclSymbolEdit *pCtrl = (VdclSymbolEdit*)pControl;
					if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
						pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
					break;
				}
				case EditFilter_Angle:
				{
					VdclAngleEdit *pCtrl = (VdclAngleEdit*)pControl;
					if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
						pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
					break;
				}
				case EditFilter_Numeric:
				{
					VdclNumericEdit *pCtrl = (VdclNumericEdit*)pControl;
					if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
						pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
					break;
				}
				default:
				{
					OdclEdit *pCtrl = (OdclEdit*)pControl;
					if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
						pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
					break;
				}
			}
		break;
		}
	case CtlStdButton:
		{
		VdclTextButton *pCtrl = (VdclTextButton*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlTree:
		{
		VdclTree *pCtrl = (VdclTree*)pControl;
		if( pCtrl->m_ChildTree.m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ChildTree.m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}

	case CtlGraphicButton:
		{
		CArxGraphicButtonCtrl *pCtrl = (CArxGraphicButtonCtrl*)pControl;
		if( pCtrl->GetToolTipCtrl().GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->GetToolTipCtrl().ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlActiveX:
		{
		CAxContainerCtrl *pCtrl = (CAxContainerCtrl*)pControl;
		if( pCtrl->GetToolTipCtrl().GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->GetToolTipCtrl().ShowHelpTooltip(&pt, TI);
		break;
		}
	case CtlDwgList:
		{
		CDwgDirList *pCtrl = (CDwgDirList*)pControl;
		if( pCtrl->m_ToolTip.GetToolInfo( TI, pControl, DWORD(0) ) )
			pCtrl->m_ToolTip.ShowHelpTooltip(&pt, TI);
		break;
		}
	}
	acedRetVoid();
	return 0;
}



const char sProgressBarSetPos[] = "ProgressBar_SetPos";

int ProgressBar_SetPos()
{
	int nPos;

	CWnd *pControl = GetArgsControlInt(CtlTextBox, sProgressBarSetPos, nPos);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	((CProgressCtrl*)pControl)->SetPos(nPos);
	((CProgressCtrl*)pControl)->RedrawWindow();
	
	// return nil
	acedRetVoid();
	return 0;
}


