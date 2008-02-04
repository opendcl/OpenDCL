// LoadArgs.cpp : implementation file
//

#include "stdafx.h"
#include "LoadArgs.h"
#include "Resource.h"
#include "ControlTypes.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "Workspace.h"
#include "DclFormObject.h"


void LoadArgsNDesc(Prop::Id nEventId, const TDclControlPtr pControl, CString &sArgs, CString &sDesc)
{
	// here we need to test the event defun id type to see what arguments need to be added.
	switch (nEventId)
	{
	case Prop::EventSplitterMoved:
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_SPLITTERMOVED);
		if (pControl->GetType() == CtlSplitter)
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_RECT);			
		break;

	case Prop::EventBtnClicked:
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_BTNCLICKED);
		if (pControl->GetType() == CtlGrid)
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);			
		break;
    
	case Prop::EventClicked:
		switch( pControl->GetType())
		{
		case CtlCheckBox:
		case CtlOptionButton:
			sArgs = theWorkspace.LoadResourceString(IDS_ARG_VALUE);
			break;
		case CtlListView:
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);
			break;
		case CtlBlockList:
			sArgs = theWorkspace.LoadResourceString(IDS_ARG_INDEX);
			break;
		default:
			sArgs = CString();
			break;
		}
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_CLICKED);
		break;      
		
	case Prop::EventFolderChanged:
		sArgs = theWorkspace.LoadResourceString(IDS_ARG_FOLDER);
    sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_FOLDERCHANGED);
		break;
	case Prop::EventOnHelp:
		sArgs = CString();
    sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_HELP);
		break;
	case Prop::EventOnTypeChange:
		sArgs = theWorkspace.LoadResourceString(IDS_ARG_TYPE);
    sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_TYPECHANGE);
		break;
	
	case Prop::EventEditChanged:
		sArgs = theWorkspace.LoadResourceString(IDS_ARG_TEXT);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_EDITCHANGED);
		break;

	case Prop::EventDblClicked:
		switch( pControl->GetType())
		{
		case CtlListView:
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);
			break;
		case CtlBlockList:
			sArgs = theWorkspace.LoadResourceString(IDS_ARG_INDEX);
			break;
		default:
			sArgs = CString();
			break;
		}
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_DBLCLICKED);
		break;
      
	case Prop::EventSelChanged:
    
		switch( pControl->GetType())
		{
		case CtlGrid:
			{
				sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);
				sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_SELCHANGED);			
				break;
			}
		case CtlComboBox:
			if (pControl->GetLongProperty(Prop::ComboBoxStyle) == 12)
			{
				sArgs = theWorkspace.LoadResourceString(IDS_ARG_FOLDER);
				sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_DIRCHANGED);
			}
			else
			{
				sArgs = theWorkspace.LoadResourceString(IDS_ARGS_SELTEXT);
				sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_SELCHANGED);
			}
		
			break;
		case CtlListBox:
		case CtlOptionList:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_SELTEXT);
			sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_SELCHANGED);
			break;
		case CtlTree:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_TEXTKEY);
			sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_SELCHANGED);
			break;
		case CtlFileDlgCtrl:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_SELTEXT);
			sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_SELCHANGED);
			break;
		case CtlMonth:
			sArgs.Empty();
			sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_SELCHANGED);
			break;
		default:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_SELTEXT);
			sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_SELCHANGED);	
			break;
		}
      
		break;
      
	case Prop::EventGetDayState:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_GETDAYSTATE);
		break;

	case Prop::EventSelect:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_DATESELECT);
		break;

	case Prop::EventKillFocus:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_KILLFOCUS);
		break;

	case Prop::EventOutOfMemory:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_OUTOFMEM);
		break;

	case Prop::EventRClick:
		switch( pControl->GetType())
		{
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);			
			break;
		case CtlListView:
			sArgs = theWorkspace.LoadResourceString(IDS_ARG_ITEM);
			break;
		default:
			sArgs.Empty();
			break;
		}
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_RCLICK);
		break;

	case Prop::EventRDblClick:
		switch( pControl->GetType())
		{
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);			
			break;
		case CtlListView:
			sArgs = theWorkspace.LoadResourceString(IDS_ARG_ITEM);
			break;
		default:
			sArgs.Empty();
			break;
		}
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_RDBLCLICK);
		break;

	case Prop::EventReturn:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_RETURN);
		break;

	case Prop::EventSetFocus:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_SETFOCUS);
		break;
    
	case Prop::EventBeginLabelEdit:
		switch( pControl->GetType())
		{
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);			
			break;
		case CtlListView:
			sArgs = theWorkspace.LoadResourceString(IDS_ARG_ITEM);
			break;
		case CtlTree:
			sArgs = theWorkspace.LoadResourceString(IDS_ARG_KEY);
			break;
		}
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_BEGINLABELEDIT);
		break;
    
	case Prop::EventDeleteItem:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_DELETEITEM);
		break;
    
	case Prop::EventEndLabelEdit:
		switch( pControl->GetType())
		{		   
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);
			sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_GRIDENDLABELEDIT);
			break;
		case CtlListView:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_TEXTROWCOL);
			sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_ENDLABELEDIT);
			break;
		case CtlTree:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_TEXTKEY);
			sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_ENDLABELEDIT);
			break;
		}  
		break;

	case Prop::EventItemExpanded:
		sArgs = theWorkspace.LoadResourceString(IDS_ARGS_TEXTKEY);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_ITEMEXPANDED);
		break;
    
	case Prop::EventItemExpanding:
		sArgs = theWorkspace.LoadResourceString(IDS_ARGS_TEXTKEY);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_ITEMEXPANDING);
		break;
    
	case Prop::EventKeyDown:
		switch( pControl->GetType())
		{
    case CtlBlockList:
		case CtlListView:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_KEYDOWN);
			break;
		default:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_KEYDOWN);
			break;
		}
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_KEYDOWN);
		sDesc += _T(" ");
		sDesc += theWorkspace.LoadResourceString(IDS_STATEFLAGDESC);
		break;
    
	case Prop::EventKeyUp:
		sArgs = theWorkspace.LoadResourceString(IDS_ARGS_KEYDOWN);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_KEYUP);
		sDesc += _T(" ");
		sDesc += theWorkspace.LoadResourceString(IDS_STATEFLAGDESC);
		break;
    
	case Prop::EventMouseDown:
		sArgs = theWorkspace.LoadResourceString(IDS_ARGS_BUTTONDOWN);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_MOUSEDOWN);
		sDesc += _T(" ");
		sDesc += theWorkspace.LoadResourceString(IDS_STATEFLAGDESC);
		break;
    
	case Prop::EventMouseUp:
		sArgs = theWorkspace.LoadResourceString(IDS_ARGS_BUTTONDOWN);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_MOUSEUP);
		sDesc += _T(" ");
		sDesc += theWorkspace.LoadResourceString(IDS_STATEFLAGDESC);
		break;
    
	case Prop::EventMouseMove:
		sArgs = theWorkspace.LoadResourceString(IDS_ARGS_SELECT);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_MOUSEMOVE);
		sDesc += _T(" ");
		sDesc += theWorkspace.LoadResourceString(IDS_STATEFLAGDESC);
		break;
    
	case Prop::EventMouseMovedOff:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_MOUSELEAVE);
		break;
    
	case Prop::EventMouseEntered:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_MOUSEENTER);
		break;
    
	case Prop::EventColumnClick:
		sArgs = theWorkspace.LoadResourceString(IDS_ARG_COLUMN);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_COLUMNCLICK);
		break;
    
	case Prop::EventMouseDblClick:
		sArgs = theWorkspace.LoadResourceString(IDS_ARGS_BUTTONDOWN);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_MOUSEDBLCLICK);
		sDesc += _T(" ");
		sDesc += theWorkspace.LoadResourceString(IDS_STATEFLAGDESC);
		break;
    
	case Prop::EventMouseWheel:
		sArgs = theWorkspace.LoadResourceString(IDS_ARGS_SCROLL);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_MOUSEWHEEL);
		sDesc += _T(" ");
		sDesc += theWorkspace.LoadResourceString(IDS_STATEFLAGDESC);
		break;
    
	case Prop::EventPaint:
		sArgs = theWorkspace.LoadResourceString(IDS_ARG_FOCUS);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_PAINT);		  
		break;
    
	case Prop::EventNavigateComplete:
		sArgs = theWorkspace.LoadResourceString(IDS_ARG_URL);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_NAVIGATECOMPLETE);
		break;
      
	case Prop::EventSelChanging:
		switch( pControl->GetType())
		{
		case CtlTabStrip:
			sArgs = theWorkspace.LoadResourceString(IDS_ARG_SELIDX);
			break;
		default:
			sArgs = CString();
			break;
		}
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_SELCHANGING);
		break;
    
	case Prop::EventChanged:
		switch( pControl->GetType())
		{
		case CtlTabStrip:
			sArgs = theWorkspace.LoadResourceString(IDS_ARG_SELIDX);
			break;
		case CtlSpinButton:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_SPIN);
			break;
		default:
			sArgs = CString();
			break;
		}  
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_CHANGED);
		break;

	case Prop::EventMaxText:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_MAXTEXT);
		break;
  
	case Prop::EventUpdate:
		sArgs = theWorkspace.LoadResourceString(IDS_ARG_TEXT);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_UPDATE);
		break;
  
	case Prop::EventDropDown:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_DROPDOWN);
		break;
  
	case Prop::EventScroll:
		sArgs = theWorkspace.LoadResourceString(IDS_ARG_VALUE);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_SCROLL);
		break;
  
	case Prop::EventScrolled:
        sArgs = theWorkspace.LoadResourceString(IDS_ARG_VALUE);
        sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_SCROLLED);		
		break;
      
	case Prop::EventReleasedCapture:
		sArgs = theWorkspace.LoadResourceString(IDS_ARG_VALUE);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_RELEASEDCAPTURE);		
		break;
      
	case Prop::FormEventClose:
		switch( pControl->GetOwnerForm()->GetType() )
		{
		case VdclFileDialog:
		case VdclModal:
		case VdclModeless:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_UPLEFT);
		  break;
		default:
			sArgs = CString();
			break;
		}
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_FORMCLOSE);
		break;
    
	case Prop::FormEventInitialize:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_FORMINITIALIZE);
		break;

	case Prop::FormEventSize:
		sArgs = theWorkspace.LoadResourceString(IDS_ARGS_SIZE);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_FORMSIZE);
		break;

	case Prop::FormEventShow:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_FORMSHOW);
		break;

	case Prop::DocEventActivated:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_DOCACTIVATED);
		break;

	case Prop::CfgEventCancel:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_CFGCANCEL);
		break;

	case Prop::CfgEventOK:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_CFGOK);
		break;

	case Prop::CfgEventHelp:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_CFGHELP);
		break;

	case Prop::CfgEventApply:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_CFGAPPLY);
		break;

	case Prop::EventReturnPressed:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_RETURNPRESSED);
		break;

	case Prop::FormEventCancelClose:
		sArgs = theWorkspace.LoadResourceString(IDS_ARG_CANCELLING);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_CANCELCLOSE);
		break;

	case Prop::FormEventOnOk:
    sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_OK);
    break;

	case Prop::FormEventOnCancel:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_CANCEL);
		break;

	case Prop::OnLMouseEvent:
		sArgs = theWorkspace.LoadResourceString(IDS_ARGS_MOUSE);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_LMOUSE);
		sDesc += _T(" ");
		sDesc += theWorkspace.LoadResourceString(IDS_STATEFLAGDESC);
		break;

	case Prop::OnMMouseEvent:
		sArgs = theWorkspace.LoadResourceString(IDS_ARGS_MOUSE);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_MMOUSE);
		sDesc += _T(" ");
		sDesc += theWorkspace.LoadResourceString(IDS_STATEFLAGDESC);
		break;

	case Prop::OnRMouseEvent:
		sArgs = theWorkspace.LoadResourceString(IDS_ARGS_MOUSE);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_RMOUSE);
		sDesc += _T(" ");
		sDesc += theWorkspace.LoadResourceString(IDS_STATEFLAGDESC);
		break;

	case Prop::DragnDropToAutoCAD:
		sArgs = theWorkspace.LoadResourceString(IDS_ARG_3DPOINT);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_DRAGTOACAD);
		break;

	case Prop::DragnDropFromControl:
		switch(pControl->GetType())
		{
		case CtlTree:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_CTRLKEY);
			break;
		default:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_CTRLPOINT);
			break;
		}
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_DRAGFROMCONTROL);
		break;

	case Prop::DragnDropFromAutoCAD:
		if( pControl->GetType() == CtlTree )
			sArgs = theWorkspace.LoadResourceString(IDS_ARG_ITEMKEY);			
		else
			sArgs = theWorkspace.LoadResourceString(IDS_ARG_ITEMPOINT);
		if (pControl->GetOwnerForm()->GetType() == VdclFileDialog)
			sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_DRAGFILEFROMACAD);
		else
			sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_DRAGFROMACAD);
		break;

	case Prop::DragnDropBegin:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_DRAGDROPBEGIN);
		break;

	case Prop::DocEventEnteringNoDocState:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTD_ENTERINGNODOC);
		break;
	}
}
