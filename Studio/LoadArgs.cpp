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
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTDESC_SPLITTERMOVED);
		if (pControl->GetType() == CtlSplitter)
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_RECT);			
		break;

	case Prop::EventBtnClicked:
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTDESC_BTNCLICKED);
		
		if (pControl->GetType() == CtlGrid)
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);			
		break;
    
	case Prop::EventClicked:
		switch( pControl->GetType())
		{
		case CtlCheckBox:
		case CtlOptionButton:
			sArgs = theWorkspace.LoadResourceString(1188);
			break;
		case CtlListView:
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);
			break;
		case CtlBlockList:
			sArgs = theWorkspace.LoadResourceString(1189);
			break;
		default:
			sArgs = CString();
			break;
		}
		sDesc = theWorkspace.LoadResourceString(5001);
		break;      
		
	case Prop::EventFolderChanged:
		sArgs = theWorkspace.LoadResourceString(1220);
        sDesc = theWorkspace.LoadResourceString(5219);
		break;
	case Prop::EventOnHelp:
		sArgs = CString();
        sDesc = theWorkspace.LoadResourceString(5220);
		break;
	case Prop::EventOnTypeChange:
		sArgs = theWorkspace.LoadResourceString(1221);
        sDesc = theWorkspace.LoadResourceString(5221);
		break;
	
	case Prop::EventEditChanged:
		sArgs = theWorkspace.LoadResourceString(1208);
		sDesc = theWorkspace.LoadResourceString(5002);
		break;

	case Prop::EventDblClicked:
		switch( pControl->GetType())
		{
		case CtlListView:
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);
			break;
		case CtlBlockList:
			sArgs = theWorkspace.LoadResourceString(1189);
			break;
		default:
			sArgs = CString();
			break;
		}
		sDesc = theWorkspace.LoadResourceString(5003);
		break;
      
	case Prop::EventSelChanged:
    
		switch( pControl->GetType())
		{
		case CtlGrid:
			{
				sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);
				sDesc = theWorkspace.LoadResourceString(5004);			
				break;
		
			}
		case CtlComboBox:
			if (pControl->GetLongProperty(Prop::ComboBoxStyle) == 12)
			{
				sArgs = theWorkspace.LoadResourceString(1223);
				sDesc = theWorkspace.LoadResourceString(5223);
			}
			else
			{
				sArgs = theWorkspace.LoadResourceString(1190);
				sDesc = theWorkspace.LoadResourceString(5004);
			}
		
			break;
		case CtlListBox:
		case CtlOptionList:
			sArgs = theWorkspace.LoadResourceString(1191);
			sDesc = theWorkspace.LoadResourceString(5004);
			break;
		case CtlTree:
			sArgs = theWorkspace.LoadResourceString(1192);
			sDesc = theWorkspace.LoadResourceString(5004);
			break;
		case CtlFileDlgCtrl:
			sArgs = theWorkspace.LoadResourceString(1191);
			sDesc = theWorkspace.LoadResourceString(5004);
			break;
		default:
			sArgs = theWorkspace.LoadResourceString(1191);
			sDesc = theWorkspace.LoadResourceString(5004);	
			break;
		}
      
		break;
      
	case Prop::EventGetDayState:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5005);
		break;

	case Prop::EventSelect:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5006);
		break;

	case Prop::EventKillFocus:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5007);
		break;

	case Prop::EventOutOfMemory:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5008);
		break;

	case Prop::EventRClick:
		switch( pControl->GetType())
		{
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);			
			break;
		case CtlListView:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ITEM);
			break;
		default:
			sArgs.Empty();
			break;
		}
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTDESC_RCLICK);
		break;

	case Prop::EventRDblClick:
		switch( pControl->GetType())
		{
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);			
			break;
		case CtlListView:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ITEM);
			break;
		default:
			sArgs.Empty();
			break;
		}
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTDESC_RDBLCLICK);
		break;

	case Prop::EventReturn:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5011);
		break;

	case Prop::EventSetFocus:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5012);
		break;
    
	case Prop::EventBeginLabelEdit:
		switch( pControl->GetType())
		{
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);			
			break;
		case CtlListView:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ITEM);
			break;
		case CtlTree:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_KEY);
			break;
		}
		sDesc = theWorkspace.LoadResourceString(5013);
		break;
    
	case Prop::EventDeleteItem:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5014);
		break;
    
	case Prop::EventEndLabelEdit:
		switch( pControl->GetType())
		{		   
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(IDS_ARGS_ROWCOL);			
			sDesc = theWorkspace.LoadResourceString(5225);
			break;
		case CtlListView:
			sArgs = theWorkspace.LoadResourceString(1195);
			sDesc = theWorkspace.LoadResourceString(5015);
			break;
		case CtlTree:
			sArgs = theWorkspace.LoadResourceString(1196);
			sDesc = theWorkspace.LoadResourceString(5015);
			break;
		}  
		break;

	case Prop::EventItemExpanded:
		sArgs = "ItemText Key";
		sDesc = theWorkspace.LoadResourceString(5016);
		break;
    
	case Prop::EventItemExpanding:
		sArgs = "ItemText Key";
		sDesc = theWorkspace.LoadResourceString(5017);
		break;
    
	case Prop::EventKeyDown:
		switch( pControl->GetType())
		{
    case CtlBlockList:
		case CtlListView:
			sArgs = theWorkspace.LoadResourceString(1198);
			break;
		default:
			sArgs = theWorkspace.LoadResourceString(1198);
			break;
		}
		sDesc = theWorkspace.LoadResourceString(5018);
		break;
    
	case Prop::EventKeyUp:
		sArgs = theWorkspace.LoadResourceString(1199);
		sDesc = theWorkspace.LoadResourceString(5033);
		break;
    
	case Prop::EventMouseDown:
		sArgs = theWorkspace.LoadResourceString(1200);
		sDesc = theWorkspace.LoadResourceString(5034);
		break;
    
	case Prop::EventMouseUp:
		sArgs = theWorkspace.LoadResourceString(1200);
		sDesc = theWorkspace.LoadResourceString(5035);
		break;
    
	case Prop::EventMouseMove:
		sArgs = theWorkspace.LoadResourceString(1201);
		sDesc = theWorkspace.LoadResourceString(5036);
		break;
    
	case Prop::EventMouseMovedOff:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5037);
		break;
    
	case Prop::EventMouseEntered:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5042);
		break;
    
	case Prop::EventColumnClick:
		sArgs = theWorkspace.LoadResourceString(1202);
		sDesc = theWorkspace.LoadResourceString(5052);
		break;
    
	case Prop::EventMouseDblClick:
		sArgs = theWorkspace.LoadResourceString(1200);
		sDesc = theWorkspace.LoadResourceString(5038);
		break;
    
	case Prop::EventMouseWheel:
		sArgs = theWorkspace.LoadResourceString(1203);
		sDesc = theWorkspace.LoadResourceString(5039);
		break;
    
	case Prop::EventPaint:
		sArgs = theWorkspace.LoadResourceString(1204);
		sDesc = theWorkspace.LoadResourceString(5040);		  
		break;
    
	case Prop::EventNavigateComplete:
		sArgs = theWorkspace.LoadResourceString(1205);
		sDesc = theWorkspace.LoadResourceString(5041);
		break;
      
	case Prop::EventSelChanging:
		switch( pControl->GetType())
		{
		case CtlTabStrip:
			sArgs = theWorkspace.LoadResourceString(1206);
			break;
		default:
			sArgs = CString();
			break;
		}
		sDesc = theWorkspace.LoadResourceString(5019);
		break;
    
	case Prop::EventChanged:
		switch( pControl->GetType())
		{
		case CtlTabStrip:
			sArgs = theWorkspace.LoadResourceString(1206);
			break;
		case CtlSpinButton:
			sArgs = theWorkspace.LoadResourceString(1207);
			break;
		default:
			sArgs = CString();
			break;
		}  
		sDesc = theWorkspace.LoadResourceString(5020);
		break;

	case Prop::EventMaxText:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5021);
		break;
  
	case Prop::EventUpdate:
		sArgs = theWorkspace.LoadResourceString(1208);
		sDesc = theWorkspace.LoadResourceString(5022);
		break;
  
	case Prop::EventDropDown:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5023);
		break;
  
	case Prop::EventScroll:
		sArgs = theWorkspace.LoadResourceString(1188);
		sDesc = theWorkspace.LoadResourceString(5024);
		break;
  
	case Prop::EventScrolled:
        sArgs = theWorkspace.LoadResourceString(1188);
        sDesc = theWorkspace.LoadResourceString(5053);		
		break;
      
	case Prop::EventReleasedCapture:
		sArgs = theWorkspace.LoadResourceString(1188);
		sDesc = theWorkspace.LoadResourceString(5025);		
		break;
      
	case Prop::FormEventClose:
		switch( pControl->GetOwnerForm()->GetType() )
		{
		case VdclFileDialog:
		case VdclModal:
		case VdclModeless:
			sArgs = theWorkspace.LoadResourceString(1209);
		  break;
		default:
			sArgs = CString();
			break;
		}
		sDesc = theWorkspace.LoadResourceString(5026);
		break;
    
	case Prop::FormEventInitialize:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5027);
		break;
	case Prop::FormEventSize:
		sArgs = theWorkspace.LoadResourceString(1210);
		sDesc = theWorkspace.LoadResourceString(5028);
		break;
	case Prop::FormEventShow:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5029);
		break;
	case Prop::DocEventActivated:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5030);
		break;
	case Prop::CfgEventCancel:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5031);
		break;
	case Prop::CfgEventOK:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5032);
		break;
	case Prop::CfgEventHelp:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5033);
		break;
	case Prop::CfgEventApply:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5034);
		break;
	case Prop::EventReturnPressed:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTDESC_RETURNPRESSED);
		break;
	case Prop::FormEventCancelClose:
		sArgs = theWorkspace.LoadResourceString(IDS_ARG_CANCELLING);
		sDesc = theWorkspace.LoadResourceString(IDS_EVENTDESC_CANCELCLOSE);
		break;
  case Prop::FormEventOnOk:
    sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(14686);
    break;
	case Prop::FormEventOnCancel:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(14687);
		break;
	case Prop::OnLMouseEvent:
		sArgs = theWorkspace.LoadResourceString(1212);
		sDesc = theWorkspace.LoadResourceString(5045);
		break;
	case Prop::OnMMouseEvent:
		sArgs = theWorkspace.LoadResourceString(1212);
		sDesc = theWorkspace.LoadResourceString(5046);
		break;
	case Prop::OnRMouseEvent:
		sArgs = theWorkspace.LoadResourceString(1212);
		sDesc = theWorkspace.LoadResourceString(5047);
		break;
      
	case Prop::DragnDropToAutoCAD:
		sArgs = theWorkspace.LoadResourceString(1213);
		sDesc = theWorkspace.LoadResourceString(5048);
		break;
      
	case Prop::DragnDropFromControl:
		switch(pControl->GetType())
		{
		case CtlTree:
			sArgs = theWorkspace.LoadResourceString(1214);
			break;
		default:
			sArgs = theWorkspace.LoadResourceString(1215);
			break;
		}
		sDesc = theWorkspace.LoadResourceString(5049);
		break;
	case Prop::DragnDropFromAutoCAD:
		if (pControl->GetOwnerForm()->GetType() == VdclFileDialog)
		{
			switch( pControl->GetType())
			{
			case CtlTree:
				sArgs = theWorkspace.LoadResourceString(1216);			
				break;
			default:
				sArgs = theWorkspace.LoadResourceString(1217);
				break;
			}
			sDesc = theWorkspace.LoadResourceString(5222);
		}
		
		if (pControl->GetOwnerForm()->GetType() != VdclFileDialog)
		{
			switch( pControl->GetType())
			{
			case CtlTree:
				sArgs = theWorkspace.LoadResourceString(1216);			
				break;
			default:
				sArgs = theWorkspace.LoadResourceString(1217);
				break;
			}
			sDesc = theWorkspace.LoadResourceString(5050);
		}
		break;

	case Prop::DragnDropBegin:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5051);
		break;
	}
}
