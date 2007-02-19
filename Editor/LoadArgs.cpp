// LoadArgs.cpp : implementation file
//

#include "stdafx.h"
#include "LoadArgs.h"
#include "ControlTypes.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "Workspace.h"
#include "DclFormObject.h"


void LoadArgsNDesc(PropertyId nEventId, const CDclControlObject *pControl, CString &sArgs, CString &sDesc)
{
	// here we need to test the event defun id type to see what arguments need to be added.
	switch (nEventId)
	{
	case nEventSplitterMoved:
		sDesc = theWorkspace.LoadResourceString(5224);
		
		if (pControl->GetType() == CtlSplitter)
			sArgs = theWorkspace.LoadResourceString(1331);			
		break;

	case nEventBtnClicked:
		sDesc = theWorkspace.LoadResourceString(5000);
		
		if (pControl->GetType() == CtlGrid)
			sArgs = theWorkspace.LoadResourceString(1187);			
		break;
    
	case nEventClicked:
		switch( pControl->GetType())
		{
		case CtlCheckBox:
		case CtlOptionButton:
			sArgs = theWorkspace.LoadResourceString(1188);
			break;
		case CtlListView:
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(1187);
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
		
	case nEventFolderChanged:
		sArgs = theWorkspace.LoadResourceString(1220);
        sDesc = theWorkspace.LoadResourceString(5219);
		break;
	case nEventOnHelp:
		sArgs = CString();
        sDesc = theWorkspace.LoadResourceString(5220);
		break;
	case nEventOnTypeChange:
		sArgs = theWorkspace.LoadResourceString(1221);
        sDesc = theWorkspace.LoadResourceString(5221);
		break;
	
	case nEventEditChanged:
		sArgs = theWorkspace.LoadResourceString(1208);
        sDesc = theWorkspace.LoadResourceString(5002);
		break;

	case nEventDblClicked:
		switch( pControl->GetType())
		{
		case CtlListView:
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(1187);
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
      
	case nEventSelChanged:
    
		switch( pControl->GetType())
		{
		case CtlGrid:
			{
				sArgs = theWorkspace.LoadResourceString(1187);
				sDesc = theWorkspace.LoadResourceString(5004);			
				break;
		
			}
		case CtlComboBox:
			if (pControl->GetLngProperty(nComboBoxStyle) == 12)
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
      
	case nEventGetDayState:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5005);
		break;

	case nEventSelect:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5006);
		break;

	case nEventKillFocus:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5007);
		break;

	case nEventOutOfMemory:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5008);
		break;

	case nEventRClick:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5009);
		break;

	case nEventRDblClick:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5010);
		break;

	case nEventReturn:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5011);
		break;

	case nEventSetFocus:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5012);
		break;
    
	case nEventBeginLabelEdit:
		switch( pControl->GetType())
		{
		case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(1187);			
			break;
		case CtlListView:
			sArgs = theWorkspace.LoadResourceString(1193);
			break;
		case CtlTree:
            sArgs = theWorkspace.LoadResourceString(1194);
			break;
		}
        sDesc = theWorkspace.LoadResourceString(5013);
		break;
    
	case nEventDeleteItem:
       sArgs = CString();
       sDesc = theWorkspace.LoadResourceString(5014);
	   break;
    
	case nEventEndLabelEdit:
       switch( pControl->GetType())
	   {		   
	   case CtlGrid:
			sArgs = theWorkspace.LoadResourceString(1187);			
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

	case nEventItemExpanded:
       sArgs = "ItemText Key";
       sDesc = theWorkspace.LoadResourceString(5016);
	   break;
    
	case nEventItemExpanding:
		sArgs = "ItemText Key";
        sDesc = theWorkspace.LoadResourceString(5017);
		break;
    
	case nEventKeyDown:
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
    
	case nEventKeyUp:
           sArgs = theWorkspace.LoadResourceString(1199);
           sDesc = theWorkspace.LoadResourceString(5033);
		   break;
    
	case nEventMouseDown:
           sArgs = theWorkspace.LoadResourceString(1200);
           sDesc = theWorkspace.LoadResourceString(5034);
		   break;
    
	case nEventMouseUp:
           sArgs = theWorkspace.LoadResourceString(1200);
           sDesc = theWorkspace.LoadResourceString(5035);
		   break;
    
	case nEventMouseMove:
           sArgs = theWorkspace.LoadResourceString(1201);
           sDesc = theWorkspace.LoadResourceString(5036);
		   break;
    
	case nEventMouseMovedOff:
          sArgs = CString();
          sDesc = theWorkspace.LoadResourceString(5037);
		   break;
    
	case nEventMouseEntered:
          sArgs = CString();
          sDesc = theWorkspace.LoadResourceString(5042);
		   break;
    
	case nEventColumnClick:
          sArgs = theWorkspace.LoadResourceString(1202);
          sDesc = theWorkspace.LoadResourceString(5052);
		   break;
    
	case nEventMouseDblClick:
          sArgs = theWorkspace.LoadResourceString(1200);
          sDesc = theWorkspace.LoadResourceString(5038);
		   break;
    
	case nEventMouseWheel:
          sArgs = theWorkspace.LoadResourceString(1203);
          sDesc = theWorkspace.LoadResourceString(5039);
		   break;
    
	case nEventPaint:
          sArgs = theWorkspace.LoadResourceString(1204);
          sDesc = theWorkspace.LoadResourceString(5040);		  
		   break;
    
	case nEventNavigateComplete:
          sArgs = theWorkspace.LoadResourceString(1205);
          sDesc = theWorkspace.LoadResourceString(5041);
		   break;
      
    case nEventSelChanging:
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
    
	case nEventChanged:
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

	case nEventMaxText:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5021);
		break;
  
    case nEventUpdate:
		sArgs = theWorkspace.LoadResourceString(1208);
		sDesc = theWorkspace.LoadResourceString(5022);
		break;
  
    case nEventDropDown:
		sArgs = CString();
		sDesc = theWorkspace.LoadResourceString(5023);
		break;
  
    case nEventScroll:
		sArgs = theWorkspace.LoadResourceString(1188);
		sDesc = theWorkspace.LoadResourceString(5024);
		break;
  
	case nEventScrolled:
        sArgs = theWorkspace.LoadResourceString(1188);
        sDesc = theWorkspace.LoadResourceString(5053);		
		break;
      
	case nEventReleasedCapture:
		sArgs = theWorkspace.LoadResourceString(1188);
		sDesc = theWorkspace.LoadResourceString(5025);		
		break;
      
	case nFormEventClose:
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
    
	case nFormEventInitialize:
          sArgs = CString();
          sDesc = theWorkspace.LoadResourceString(5027);
		break;
	case nFormEventSize:
          sArgs = theWorkspace.LoadResourceString(1210);
          sDesc = theWorkspace.LoadResourceString(5028);
		break;
	case nFormEventShow:
          sArgs = CString();
          sDesc = theWorkspace.LoadResourceString(5029);
		break;
	case nDocEventActivated:
          sArgs = CString();
          sDesc = theWorkspace.LoadResourceString(5030);
		break;
	case nCfgEventCancel:
          sArgs = CString();
          sDesc = theWorkspace.LoadResourceString(5031);
		break;
	case nCfgEventOK:
          sArgs = CString();
          sDesc = theWorkspace.LoadResourceString(5032);
		break;
	case nCfgEventHelp:
          sArgs = CString();
          sDesc = theWorkspace.LoadResourceString(5033);
		break;
	case nCfgEventApply:
          sArgs = CString();
          sDesc = theWorkspace.LoadResourceString(5034);
		break;
	case nEventReturnPressed:
          sArgs = CString();
          sDesc = theWorkspace.LoadResourceString(5043);
		break;
	case nFormEventCancelClose:
          sArgs = theWorkspace.LoadResourceString(1211);
          sDesc = theWorkspace.LoadResourceString(5044);
		break;
  case nFormEventOnOk:
    sArgs = CString();
    sDesc = _T("Called when OnOk is called.");
    break;
	case nOnLMouseEvent:
          sArgs = theWorkspace.LoadResourceString(1212);
          sDesc = theWorkspace.LoadResourceString(5045);
		break;
	case nOnMMouseEvent:
          sArgs = theWorkspace.LoadResourceString(1212);
         sDesc = theWorkspace.LoadResourceString(5046);
		break;
	case nOnRMouseEvent:
          sArgs = theWorkspace.LoadResourceString(1212);
          sDesc = theWorkspace.LoadResourceString(5047);
		break;
      
	case nDragnDropToAutoCAD:
          sArgs = theWorkspace.LoadResourceString(1213);
          sDesc = theWorkspace.LoadResourceString(5048);
		break;
      
	case nDragnDropFromControl:
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
	case nDragnDropFromAutoCAD:
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

	case nDragnDropBegin:
          sArgs = CString();
          sDesc = theWorkspace.LoadResourceString(5051);
		break;
	}
}
