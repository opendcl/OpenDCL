// ArxDialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "InvokeMethod.h"
#include "ToolTips.h"
#include "ArxWorkspace.h"
#include "PropertyObject.h"

#include "ControlTypes.h"
#include "ArxArrowComboBoxCtrl.h"
#include "ArxAxContainerCtrl.h"
#include "ArxColorComboBoxCtrl.h"
#include "ArxDwgListCtrl.h"
#include "ArxCheckBoxCtrl.h"
#include "ArxFolderComboCtrl.h"
#include "ArxFontComboBoxCtrl.h"
#include "ArxGraphicButtonCtrl.h"
#include "ArxGridCtrl.h"
#include "ArxImageComboBoxCtrl.h"
#include "ArxLayerComboBoxCtrl.h"
#include "ArxLineweightComboBoxCtrl.h"
#include "ArxListBoxCtrl.h"
#include "ArxOptionListCtrl.h"
#include "ArxPaperComboBoxCtrl.h"
#include "ArxPlotStyleNameComboBoxCtrl.h"
#include "ArxPlotStyleTableComboBoxCtrl.h"
#include "ArxPrinterComboBoxCtrl.h"
#include "ArxRadioButtonCtrl.h"
#include "ArxTabStripCtrl.h"
#include "ArxTextBoxCtrl.h"

#include "AngleFilter.h"
#include "CurrencyFilter.h"
#include "DateFilter.h"
#include "IntegerFilter.h"
#include "LowerCaseFilter.h"
#include "MultilineFilter.h"
#include "NumericFilter.h"
#include "PasswordFilter.h"
#include "TimeFilter.h"
#include "UpperCaseFilter.h"

#include "Splitter.h"
#include "VdclStatic.h"
#include "GsPreviewCtrl.h"
#include "OdclListCtrl.h"
#include "VdclGroupBox.h"
#include "DwgPreviewCtrl.h"
#include "OdclMonth.h"
#include "PictureBox.h"
#include "ProgressTimeToComplete.h"
#include "RoundSliderCtrl.h"
#include "VdclSliderCtrl.h"
#include "VdclScrollBar.h"
#include "VdclSpinButton.h"
#include "SlideHolder.h"
#include "StaticLink.h"
#include "HtmlCtrl.h"
#include "VdclTree.h"
#include "VdclTextButton.h"
#include "TabPage.h"


static TDclControlPtr FindPaperCombo( TDclFormPtr pForm )
{
	if( !pForm )
		return NULL;
	TDclControlList listCombos;
	pForm->FindControls( CtlComboBox, listCombos );
	for( TDclControlList::iterator iter = listCombos.begin(); iter != listCombos.end(); ++iter )
	{
		if( (*iter)->GetLongProperty(Prop::ComboBoxStyle) == CmboStyle_PlotterPaperSizes )
			return *iter;
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CArxDialogControl

CArxDialogControl::CArxDialogControl( TDclControlPtr pTemplate, CControlPane* pPane,
																			CWnd* pControl )
: CDialogControl( pTemplate, pPane, pControl )
, mArxServices( pTemplate )
{
	TraceFmt( _T("> CArxDialogControl::CArxDialogControl(%s [%p], [%p], %s [HWND: %p]) [this: %p]\r\n"),
						(LPCTSTR)pTemplate->GetKeyPath(), (const CDclControlObject*)pTemplate, pPane,
						(LPCTSTR)CString(pControl->GetRuntimeClass()->m_lpszClassName),
						pControl->m_hWnd, this );
}

CArxDialogControl::~CArxDialogControl()
{
	TraceFmt( _T("< CArxDialogControl::~CArxDialogControl() [this: %p]\r\n"), this );
}

//static
TDialogControlPtr CArxDialogControl::Create( TDclControlPtr pTemplate, CControlPane* pPane,
																						 UINT nID, ControlParams* pParams /*= NULL*/ )
{
	// check the control type to determine which control to create
	switch(pTemplate->GetType())
	{
	case CtlAnimate:
		{
			CAnimateCtrl *pControl = new CAnimateCtrl;
			CRect rc;
			// get the rectangle of the new control
			rc.top = pTemplate->GetPropertyObject(Prop::Top)->GetLongValue();
			rc.left = pTemplate->GetPropertyObject(Prop::Left)->GetLongValue();
			rc.bottom = pTemplate->GetPropertyObject(Prop::Height)->GetLongValue() + rc.top;
			rc.right = pTemplate->GetPropertyObject(Prop::Width)->GetLongValue() + rc.left;
			pControl->Create(
				WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS |
				ACS_CENTER |ACS_AUTOPLAY|ACS_TRANSPARENT, 
				rc, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}

	case CtlImageComboBox: return CreateComboExControl(pTemplate, pPane, nID);

	case CtlSplitter:
		{
			CSplitter *pControl = new CSplitter;
			pControl->m_pControlPane = pPane;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);			
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}

	case CtlDwgList: return *new CArxDwgListCtrl( pTemplate, pPane, nID );
	case CtlOptionList: return *new CArxOptionListCtrl( pTemplate, pPane, nID );
	case CtlActiveX: return new CArxAxContainerCtrl( pTemplate, pPane, nID ); //ActiveX control must auto-destruct

	case Ctl3DRect:
		{
			VdclStatic *pControl = new VdclStatic;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);			
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}

	case CtlHatch:
		{
			CGsPreviewCtrl *pControl = new CGsPreviewCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}	
	
	case CtlBlockView:	
		{
			CGsPreviewCtrl *pControl = new CGsPreviewCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}	
	
	case CtlGrid: return *new CArxGridCtrl( pTemplate, pPane, nID );
	case CtlListView: return *new OdclListCtrl( *pPane, pTemplate, nID );
	case CtlBlockList: return *new OdclListCtrl( *pPane, pTemplate, nID );
	case CtlCheckBox: return *new CArxCheckBoxCtrl( pTemplate, pPane, nID );
	case CtlComboBox: return CreateComboControl(pTemplate, pPane, nID);

	case CtlDwgPreview:
		{
			CDwgPreviewCtrl *pControl = new CDwgPreviewCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}	
	
	case CtlFrame:
		{
			VdclGroupBox *pControl = new VdclGroupBox;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}

	case CtlGraphicButton: return *new CArxGraphicButtonCtrl( pTemplate, pPane, nID );
			
	case CtlMonth:
		{
			OdclMonth *pControl = new OdclMonth;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}	
	case CtlLabel:
		{
			VdclStatic *pControl = new VdclStatic;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);			
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// // ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}			

	case CtlListBox: return *new CArxListBoxCtrl( pTemplate, pPane, nID );
	case CtlOptionButton: return *new CArxRadioButtonCtrl( pTemplate, pPane, nID );

	case CtlPictureBox:
		{
			// create the picture box control
			CPictureBox *pControl = new CPictureBox;
			pControl->m_pParentPane = pPane;
			pControl->Create(pTemplate, pPane->GetProject(), pPane->GetHostDialog(), nID);	
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CtlProgress:
		{
			TProgressTimeToComplete *pControl = new TProgressTimeToComplete;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}			
		
	case CtlRoundSlider:
		{
			CRoundSliderCtrl *pControl = new CRoundSliderCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}			
	case CtlScrollBar:
		{
			VdclScrollBar *pControl = new VdclScrollBar;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}				

	case CtlSlider:
		{
			VdclSliderCtrl *pControl = new VdclSliderCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}

	case CtlSlideView: return *new CSlideHolder( *pPane, pTemplate, nID );
		
	case CtlSpinButton:		
		{
			VdclSpinButton *pControl = new VdclSpinButton;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CtlStaticURL:		
		{
			CStaticLink *pControl = new CStaticLink;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CtlHtmlCtrl:
		{
			CHtmlCtrl *pControl = new CHtmlCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CtlStdButton:
		{
			VdclTextButton *pControl = new VdclTextButton;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			// set the properties of the control
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}

	case CtlTextBox: return CreateEditControl(pTemplate, pPane, nID);
	case CtlTabStrip: return *new CArxTabStripCtrl( pTemplate, pPane, nID );

	case CtlTree:	
		{
			VdclTree *pControl = new VdclTree;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}			
	}
	return NULL;
}

//static
TDialogControlPtr CArxDialogControl::CreateEditControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID)
{
	CWnd* pParentWnd = pPane->GetHostDialog();

	// check the control type to determine which control to create
	switch( pTemplate->GetLongProperty( Prop::FilterStyle ) )
	{
	case EditFilter_String: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, NULL, AC_ES_STRING );
	case EditFilter_Angle: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CAngleFilter, (AC_ES_ANGLE | AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS) );
	case EditFilter_Integer: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CIntegerFilter, (AC_ES_NUMERIC | AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS) );
	case EditFilter_Numeric: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CNumericFilter, (AC_ES_NUMERIC | AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS) );
	case EditFilter_Symbol: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, NULL, (AC_ES_SYMBOL | AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS) );
	case EditFilter_UpperCase: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CUpperCaseFilter );
	case EditFilter_LowerCase: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CUpperCaseFilter );
	case EditFilter_Password: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CPasswordFilter );
	case EditFilter_Multiline: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CMultilineFilter );
	}
	return NULL;
}

//static
TDialogControlPtr CArxDialogControl::CreateComboExControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID)
{
	return *new CArxImageComboBoxCtrl( pTemplate, pPane, nID );
}

//static
TDialogControlPtr CArxDialogControl::CreateComboControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID)
{
	// check the control type to determine which control to create
	switch(pTemplate->GetLongProperty(Prop::ComboBoxStyle))
	{
	case CmboStyle_Combo: return *new CArxComboBoxCtrl( pTemplate, pPane, nID );
	case CmboStyle_Simple: return *new CArxComboBoxCtrl( pTemplate, pPane, nID );
	case CmboStyle_DropDown: return *new CArxComboBoxCtrl( pTemplate, pPane, nID );
	case CmboStyle_ArrowHead: return *new CArxArrowComboBoxCtrl( pTemplate, pPane, nID );
	case CmboStyle_Color: return *new CArxColorComboBoxCtrl( pTemplate, pPane, nID );
	case CmboStyle_LineWeight: return *new CArxLineweightComboBoxCtrl( pTemplate, pPane, nID );
	case CmboStyle_PlotNames: return *new CArxPlotStyleNameComboBoxCtrl( pTemplate, pPane, nID );
	case CmboStyle_PlotTables: return *new CArxPlotStyleTableComboBoxCtrl( pTemplate, pPane, nID );
	case CmboStyle_FontDropList: return *new CArxFontComboBoxCtrl( pTemplate, pPane, nID );
	case CmboStyle_FontSimpleList: return *new CArxFontComboBoxCtrl( pTemplate, pPane, nID );
	case CmboStyle_Plotters: return *new CArxPrinterComboBoxCtrl( pTemplate, pPane, nID, FindPaperCombo( pTemplate->GetOwnerForm() ) );
	case CmboStyle_PlotterPaperSizes: return *new CArxPaperComboBoxCtrl( pTemplate, pPane, nID );
	case CmboStyle_DirPicker: return *new CArxFolderComboCtrl( pTemplate, pPane, nID );
	case CmboStyle_Layers: return *new CArxLayerComboBoxCtrl( pTemplate, pPane, nID );
	}
	return NULL;
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateChildControl( CWnd* pControlWnd, TDclControlPtr pTemplate, CControlPane* pPane, UINT nControlId )
{
	const TPropertyList& Props = pTemplate->GetPropertyList();
	for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
	{
		Prop::Id nID = (*iter)->GetID();
		switch( nID )
		{
		case Prop::ToolTipBalloon:
		case Prop::ToolTipLine:
		case Prop::ToolTipBody:
		case Prop::ToolTipPicture:
		case Prop::ToolTipAviFileName:
		case Prop::ToolTipTitleColor:
			break; //skip related properties that all get set at once with the main property of the group
		default:
			UpdatePropertyInt( pControlWnd, pTemplate, pPane, nID );
		}
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateProperty(TDclControlPtr pControl, CControlPane* pPane, UINT nControlId, Prop::Id nID)
{
	switch( pControl->GetType() )
	{ //these controls implement the new CDialogControl interface, so use that
	case CtlActiveX:
	case CtlBlockList:
	case CtlCheckBox:
	case CtlComboBox:
	case CtlDwgList:
	case CtlGraphicButton:
	case CtlGrid:
	case CtlImageComboBox:
	case CtlListBox:
	case CtlListView:
	case CtlOptionButton:
	case CtlOptionList:
	case CtlSlideView:
	case CtlTabStrip:
	case CtlTextBox:
		CDialogControl* pDlgControl = pControl->GetControlInstance();
		if( !pDlgControl )
			return;
		pDlgControl->OnApplyProperty( pControl->GetPropertyObject( nID ) );
		pDlgControl->GetControlWnd()->Invalidate();
		return;
	};
	CWnd* pControlWnd = pControl->GetWindow();
	UpdatePropertyInt(pControlWnd, pControl, pPane, nID);
	pControlWnd->Invalidate();
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::SetDwgListComboFolderLink(CArxDwgListCtrl *pDwgList)
{
	const TDclControlList& Controls = pDwgList->GetTemplate()->GetOwnerForm()->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		if ((*iter)->GetType() == CtlComboBox && (*iter)->GetWindow() != NULL)
		{
			// check the control type to determine which control to create
			if ((*iter)->GetLongProperty(Prop::ComboBoxStyle) == CmboStyle_DirPicker)
			{				
				CArxFolderComboCtrl* pComboFolder = (CArxFolderComboCtrl*)(*iter)->GetWindow();
				pComboFolder->SetDwgListCtrl( pDwgList );
				pDwgList->m_pDirComboBox = pComboFolder;			
			}
		}		
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::ResetImageList(TDclControlPtr pArxObject, CWnd *pControl, int nID)
{
	switch (pArxObject->GetType())
	{
		case CtlTree:
		{
			RefCountedPtr< CImageListObject > pImageList = pArxObject->GetImageList();
			if( pImageList )
			{
				pImageList->GetImageList().SetBkColor(((VdclTree*)pControl)->m_ChildTree.GetBkColor());
				((VdclTree*)pControl)->m_ChildTree.SetImageList(&pImageList->GetImageList(), TVSIL_NORMAL);
			}
			break;
		}
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateFont(TDclControlPtr pArxObject, CWnd *pControl, CFont *pFont)
{
	switch (pArxObject->GetType())
	{
	case CtlLabel:
		{
		((VdclStatic*)pControl)->SetFont(pFont);
		break;
		}
	case CtlStdButton:
		{
		((VdclTextButton*)pControl)->SetFont(pFont);
		break;
		}
	case CtlFrame:		
		{		
		((VdclGroupBox*)pControl)->m_Frame.SetFont(NULL);	
		((VdclGroupBox*)pControl)->m_Frame.SetFont(pFont);		
		break;
		}
	case CtlPictureBox:		
		{		
		((CPictureBox*)pControl)->SetFont(pFont);		
		break;
		}
	case CtlMonth:
		{
		((OdclMonth*)pControl)->SetFont(pFont);
		break;
		}		
	case CtlTree:
		{
		((VdclTree*)pControl)->m_ChildTree.SetFont(pFont);
		break;
		}		
	case CtlStaticURL:
		{
		((CStaticLink*)pControl)->SetFont(pFont);
		break;
		}
	case CtlRoundSlider:
		{
		((CRoundSliderCtrl*)pControl)->m_pFont = pFont;
		pControl->RedrawWindow();
		break;
		}
	}
	pControl->Invalidate();
}

//static
void CArxDialogControl::UpdatePropertyInt(CWnd* pControlWnd, TDclControlPtr pControl, CControlPane* pPane, Prop::Id nID)
{
	//I'm moving pPane logic into the child control's Create() function as I work on individual controls. Controls 
	//that don't have their own class I'll leave here for now. 2007-02-05 [ORW]
	try
	{
	TPropertyPtr pProp = pControl->GetPropertyObject(nID);

	if (pProp == NULL)
		return;

	// set the appropriate property
	switch(nID)
	{
		case Prop::BackgroundColor:
		{
		switch (pControl->GetType())
			{
			case CtlBlockView:
			case CtlHatch:
				((CGsPreviewCtrl*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
				break;
			case CtlDwgPreview:
				((CDwgPreviewCtrl*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
				break;
			case CtlPictureBox:
				((CPictureBox*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
				break;
			case CtlLabel:
				((VdclStatic*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
				break;
			case CtlStaticURL:
				((CStaticLink*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
				break;		
			}
		if (pControl->GetType() != CtlPictureBox && 
			pControl->GetType() != CtlSlideView)
				pControlWnd->Invalidate();
			break;
			
		}
		case Prop::ForegroundColor:
		{				
			switch (pControl->GetType())
			{
			case CtlLabel:
				((VdclStatic*)pControlWnd)->SetForeColor(pControl->GetLongProperty(Prop::ForegroundColor));
				break;		
			case CtlStaticURL:
				((CStaticLink*)pControlWnd)->SetForeColor(pControl->GetLongProperty(Prop::ForegroundColor));
				break;
			}
			
			pControlWnd->Invalidate();
			break;
		}	

		case Prop::AllowOrbiting:
		{
			if (pControl->GetBooleanProperty(Prop::AllowOrbiting))
				((CGsPreviewCtrl*)pControlWnd)->AllowOrbiting(true);
			else
				((CGsPreviewCtrl*)pControlWnd)->AllowOrbiting(false);
			break;
		}
		case Prop::AutoSize:
		{
			switch (pControl->GetType())
			{
			case CtlPictureBox:
				((CPictureBox*)pControlWnd)->m_AutoSize = pControl->GetBooleanProperty(Prop::AutoSize);
				break;
			}
			break;
		}
		
		case Prop::BlockName:
		{
			((CGsPreviewCtrl*)pControlWnd)->DisplayBlock(pControl->GetStringProperty(Prop::BlockName));
			break;
		}
		case Prop::BorderStyle:
		{
			switch(pControl->GetLongProperty(Prop::BorderStyle))
			{
			case 0:
				pControlWnd->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
				pControlWnd->ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
				pControlWnd->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
				break;
					
			case 1:
				pControlWnd->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
				pControlWnd->ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
				pControlWnd->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
				break;

			case 2:
				pControlWnd->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
				pControlWnd->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
				pControlWnd->ModifyStyleEx(0, WS_EX_STATICEDGE, SWP_FRAMECHANGED);
				break;
			}
			switch (pControl->GetType())
			{
			case CtlPictureBox:
				((CPictureBox*)pControlWnd)->m_BorderStyle = pControl->GetLongProperty(Prop::BorderStyle);				
				break;
			}
		}	
		case Prop::Caption:
		{				
			UpdateText(pControl, pControlWnd, pControl->GetStringProperty(Prop::Caption));
			break;
		}	

		case Prop::DragnDropAllowDrop:
		{
			switch (pControl->GetType())
			{
			case CtlStdButton:
				((VdclTextButton*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			case CtlBlockView:
			case CtlHatch:
				((CGsPreviewCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			case CtlLabel:
				((VdclStatic*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			case CtlPictureBox:
				((CPictureBox*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			case CtlTree:
				((VdclTree*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			case CtlDwgPreview:
				((CDwgPreviewCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			}
			break;
		} 

		case Prop::Enabled:
		{
			if (pControl->GetType() == CtlSlider)
				((CSliderCtrl *)pControlWnd)->EnableWindow(pControl->GetBooleanProperty(Prop::Enabled));
			else if (pControl->GetType() == CtlTree)
				((VdclTree *)pControlWnd)->m_ChildTree.EnableWindow(pControl->GetBooleanProperty(Prop::Enabled));
			else
			{
				pControlWnd->EnableWindow(pControl->GetBooleanProperty(Prop::Enabled) != FALSE);
				
				if (pControl->GetType() == CtlLabel)
				{
					((VdclStatic*)pControlWnd)->EnableWindow(pControl->GetBooleanProperty(Prop::Enabled) != FALSE);
					((VdclStatic*)pControlWnd)->Invalidate();				
				}				
			}
			break;				
		}	
	
		case Prop::Indent:
		{
			((VdclTree*)pControlWnd)->m_ChildTree.SetIndent(pControl->GetLongProperty(Prop::Indent));
			break;
		}

		case Prop::ImageList:
		{
			ResetImageList(pControl, pControlWnd, nID);
			break;
		}

		case Prop::Justification:
		{
			switch (pControl->GetType())
			{
			case CtlLabel:
				{
					pControlWnd->ModifyStyle(SS_RIGHT, 0, SWP_FRAMECHANGED);
					pControlWnd->ModifyStyle(SS_CENTER, 0, SWP_FRAMECHANGED);
					pControlWnd->ModifyStyle(SS_LEFT, 0, SWP_FRAMECHANGED);
			
					switch (pControl->GetLongProperty(Prop::Justification))
					{
					case 0:/*Left*/
						pControlWnd->ModifyStyle(0, SS_LEFT, SWP_FRAMECHANGED);
						break;
					case 1:/*Center*/
						pControlWnd->ModifyStyle(0, SS_CENTER, SWP_FRAMECHANGED);
						break;
					case 2:/*Right*/
						pControlWnd->ModifyStyle(0, SS_RIGHT, SWP_FRAMECHANGED);
						break;
					}					
					pControlWnd->Invalidate();
					break;
				}
			}
			break;
		}

		case Prop::FontName:
		{
			const CFont *pFont = theWorkspace.GetFontCollection().GetFont(pControl, pControlWnd);

			UpdateFont(pControl, pControlWnd, const_cast<CFont*>(pFont));
			break;
		}

		case Prop::MultiSelection:
		{
			((OdclMonth*)pControlWnd)->SetMaxSelCount(
				pControl->GetLongProperty(Prop::MultiSelection));
			break;
		}
		case Prop::MaxValue:
		case Prop::MinValue:
		{
			switch (pControl->GetType())
			{
			case CtlProgress:
				((CProgressCtrl*)pControlWnd)->SetRange(
					(int)pControl->GetLongProperty(Prop::MinValue),
					(int)pControl->GetLongProperty(Prop::MaxValue));
				break;				
			case CtlScrollBar:
				((CScrollBar*)pControlWnd)->SetScrollRange(
					(int)pControl->GetLongProperty(Prop::MinValue),
					(int)pControl->GetLongProperty(Prop::MaxValue),
					TRUE);
				break;				
			case CtlSlider:
				((CSliderCtrl*)pControlWnd)->SetRange(
					(int)pControl->GetLongProperty(Prop::MinValue),
					(int)pControl->GetLongProperty(Prop::MaxValue));
			
			}
			break;
		}	
		
		case Prop::LargeChange:	
		{
			switch (pControl->GetType())
			{
			case CtlScrollBar:				
				((VdclScrollBar*)pControlWnd)->m_nLargeChange = pControl->GetLongProperty(Prop::LargeChange);
				break;
			case CtlSlider:				
				((VdclSliderCtrl*)pControlWnd)->SetPageSize(pControl->GetLongProperty(Prop::LargeChange));
				break;
			}
			break;
			
		}
		case Prop::Orientation:
		{
			switch(pControl->GetLongProperty(Prop::Orientation))
			{
			case 0:				
				((CSliderCtrl *)pControlWnd)->ModifyStyle(TBS_VERT, 0, SWP_FRAMECHANGED);
				((CSliderCtrl *)pControlWnd)->ModifyStyle(0, TBS_HORZ, SWP_FRAMECHANGED);
				break;
			case 1:
				((CSliderCtrl *)pControlWnd)->ModifyStyle(TBS_HORZ, 0, SWP_FRAMECHANGED);
				((CSliderCtrl *)pControlWnd)->ModifyStyle(0, TBS_VERT, SWP_FRAMECHANGED);
				break;
			}
			break;
		}
		case Prop::Picture:
		{
			switch (pControl->GetType())
			{
			case CtlPictureBox:		
				((CPictureBox *)pControlWnd)->SetPictureID(pControl->GetLongProperty(Prop::Picture));
				break;
			}		
			break;
		}
		case Prop::RenderMode:
		{			
			((CGsPreviewCtrl*)pControlWnd)->SetRenderMode();
			((CGsPreviewCtrl*)pControlWnd)->Invalidate();
			break;
		}
		case Prop::ShowTicks:
		{
			if (pControl->GetBooleanProperty(Prop::ShowTicks) == FALSE)
				((CSliderCtrl *)pControlWnd)->ModifyStyle(TBS_AUTOTICKS, 0, SWP_FRAMECHANGED);
			else
				((CSliderCtrl *)pControlWnd)->ModifyStyle(0, TBS_AUTOTICKS, SWP_FRAMECHANGED);
				
			break;
		}
		case Prop::SmallChange:	
		{
			switch (pControl->GetType())
			{
			case CtlScrollBar:				
				((VdclScrollBar*)pControlWnd)->m_nSmallChange = pControl->GetLongProperty(Prop::SmallChange);
				break;
			case CtlSlider:				
				((VdclSliderCtrl*)pControlWnd)->SetLineSize(pControl->GetLongProperty(Prop::SmallChange));
				break;
			}
			break;
			
		}
		case Prop::Text:
		{
			int nTextLimit = pControl->GetLongProperty(Prop::LimitText);
			CString sNewText = pControl->GetStringProperty(Prop::Text);

			if (nTextLimit > -1) 
				sNewText = sNewText.Left(nTextLimit);

			UpdateText(pControl, pControlWnd, sNewText);
			break;
		}
		case Prop::TickFrequency:
		{
			((CSliderCtrl *)pControlWnd)->SetTicFreq(pControl->GetLongProperty(Prop::TickFrequency));
			
			break;
		}
		
		case Prop::TitleBarText:
		{
			pControlWnd->SetWindowText(pControl->GetStringProperty(Prop::TitleBarText));
			break;
		}
		case Prop::BtnTTText:
		case Prop::ToolTipTitle:
		case Prop::ToolTipBalloon:
		case Prop::ToolTipLine:
		case Prop::ToolTipBody:
		case Prop::ToolTipPicture:
		case Prop::ToolTipAviFileName:
		case Prop::ToolTipTitleColor:
		{
			UpdateToolTip(pControl, pControlWnd);
			break;
		}
		case Prop::UseTabStops:
		{
			if (pControl->GetBooleanProperty(Prop::UseTabStops) == FALSE)
				((CListBox*)pControlWnd)->ModifyStyle(LBS_USETABSTOPS, 0, SWP_FRAMECHANGED);
			else
				((CListBox*)pControlWnd)->ModifyStyle(0, LBS_USETABSTOPS, SWP_FRAMECHANGED);
			break;
		}
		case Prop::URLAddress:
		{
			if (pControl->GetType() == CtlStaticURL)
				((CStaticLink*)pControlWnd)->m_link = pControl->GetStringProperty(Prop::URLAddress);
			break;
		}
		case Prop::Visible:
		{
			if (IsWindow(pControlWnd->m_hWnd))
				pControlWnd->ShowWindow(pControl->GetPropertyObject(Prop::Visible)->GetBooleanValue()? SW_SHOW : SW_HIDE);
			else
				theWorkspace.DisplayAlert(_T("The ActiveX Control is hidden and no longer valid.\nPlease contact the control developer for support."));
			break;
		}
		case Prop::Value:
		{
			switch (pControl->GetType())
			{
			case CtlRoundSlider:
				((CRoundSliderCtrl *)pControlWnd)->SetPos(pControl->GetLongProperty(Prop::Value));
				pControlWnd->RedrawWindow();
				break;
			case CtlSlider:
				((CSliderCtrl*)pControlWnd)->SetPos(pControl->GetLongProperty(Prop::Value));
				pControlWnd->RedrawWindow();
				break;
			case CtlScrollBar:
				{
					((VdclScrollBar *)pControlWnd)->m_hPos = pControl->GetLongProperty(Prop::Value);
					((VdclScrollBar *)pControlWnd)->SetScrollPos(pControl->GetLongProperty(Prop::Value), TRUE);
					pControlWnd->RedrawWindow();
					break;
				}
			case CtlProgress:
				((CProgressCtrl*)pControlWnd)->SetPos(pControl->GetLongProperty(Prop::Value));
				pControlWnd->RedrawWindow();
				break;
			case CtlSpinButton:
				((VdclSpinButton*)pControlWnd)->m_Pos = pControl->GetLongProperty(Prop::Value);
				((VdclSpinButton*)pControlWnd)->SetPos(((VdclSpinButton*)pControlWnd)->m_Pos);				
				break;			
			}
			break;
		}
		case Prop::HScrollBar:
		{
			if (pControl->GetBooleanProperty(Prop::HScrollBar) == FALSE)
				pControlWnd->ModifyStyle(WS_HSCROLL, 0, SWP_FRAMECHANGED);
			else
				pControlWnd->ModifyStyle(0, WS_HSCROLL, SWP_FRAMECHANGED);
			break;
		}
	}	
	}
	catch(...)
	{
		int n = 0;
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateText(TDclControlPtr pTemplate, CWnd *pControl, CString sText)
{
	try
	{
		switch (pTemplate->GetType())
		{
		case CtlLabel:
			{
			((VdclStatic*)pControl)->SetWindowText(sText);
			break;
			}
		case CtlStdButton:
			{
			((VdclTextButton*)pControl)->SetWindowText(sText);
			break;
			}
		case CtlFrame:		
			{		
			((VdclGroupBox*)pControl)->m_Frame.SetWindowText(sText);	
			break;
			}
		case CtlStaticURL:
			{
			((CStaticLink*)pControl)->SetLinkText(sText);
			break;
			}
		}
	}
	catch(...)
	{
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateToolTip(TDclControlPtr pArxObject, CWnd *pControl)
{
	CString sToolTipTitle = pArxObject->GetStringProperty(Prop::ToolTipTitle);
	switch (pArxObject->GetType())
	{
	case CtlDwgPreview:
		{
		SetToolTipEx(pControl, ((CDwgPreviewCtrl*)pControl)->m_ToolTip, pArxObject);
		//((CDwgPreviewCtrl*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlHatch:
		{
		SetToolTipEx(pControl, ((CGsPreviewCtrl*)pControl)->m_ToolTip, pArxObject);
		//((CGsPreviewCtrl*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlRoundSlider:
		{
		SetToolTipEx(pControl, ((CRoundSliderCtrl*)pControl)->m_ToolTip, pArxObject);
		//((CRoundSliderCtrl*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlStaticURL:
		{
		SetToolTipEx(pControl, ((CStaticLink*)pControl)->m_ToolTip, pArxObject);
		//((CStaticLink*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlProgress:
		{
		//SetToolTipEx(pControl, ((TProgressTimeToComplete*)pControl)->m_ToolTip, pArxObject);
		//((VdclProgressCtrl*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlMonth:
		{
		SetToolTipEx(pControl, ((OdclMonth*)pControl)->m_ToolTip, pArxObject);
		//((OdclMonth*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlSlider:
		{
		SetToolTipEx(pControl, ((VdclSliderCtrl*)pControl)->m_ToolTip, pArxObject);
		//((VdclSliderCtrl*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlPictureBox:
		{
		SetToolTipEx(pControl, ((CPictureBox*)pControl)->m_ToolTip, pArxObject);
		//((CPictureBox*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlStdButton:
		{
		SetToolTipEx(pControl, ((VdclTextButton*)pControl)->m_ToolTip, pArxObject);
		//((VdclTextButton*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlTree:
		{
		SetToolTipEx(((VdclTree*)pControl), ((VdclTree*)pControl)->m_ChildTree.m_ToolTip, pArxObject);
		//((VdclTree*)pControl)->m_ChildTree.SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	}
}
