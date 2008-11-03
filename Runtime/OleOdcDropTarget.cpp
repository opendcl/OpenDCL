// OleOdcDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "OleOdcDropTarget.h"
#include "InvokeMethod.h"
#include "UserMessageId.h"
#include "ArxDwgListCtrl.h"
#include "ImageTreectrl.h"
#include "AxContainerCtrl.h"
#include "DclControlObject.h"
#include "DclFormObject.h"
#include "PropertyIds.h"
#include "StdDropSource.h"
#include "AutoDocLock.h"
#include "ControlTypes.h"
#include "Workspace.h"


const TCHAR *formatname = _T("CDclControlObject");

static CString RtoS( double dValue )
{
	static CString sDefault = _T("0.0");
	CString sReturn;
	if( RTNORM != acdbRToS( dValue, 2,8, sReturn.GetBuffer( 32 ) ) )
		return sDefault;
	sReturn.ReleaseBuffer();
	return sReturn;
}

static void ReadExternalDWGfile(CString sPath, CString sFile, acedDwgPoint ptPoint, bool bXref)
{
	CString sFileName;
	if (sPath.Right(1) != _T("\\"))
		sPath = sPath + _T("\\");

	CString sLispSafePath = _T("");
	CString sChar;
	for (int i=0; i<sPath.GetLength(); i++)
	{
		sChar = sPath.Mid(i, 1);
		if (sChar == _T("\\"))
			sLispSafePath += _T("\\\\");
		else if (sChar == _T("/"))
			sLispSafePath += _T("//");
		else
			sLispSafePath += sChar;
	}

	// this code if for the dockable dialog only
	CString sCommand;

	if (bXref)
	{
		sCommand = _T("(dcl_LoadXref \"");
		sCommand +=	sLispSafePath +	_T("\" \"") + sFile + _T("\" (list ");
		sCommand +=	RtoS(ptPoint[0]) + _T(" ");
		sCommand +=	RtoS(ptPoint[1]) + _T(" "); 
		sCommand +=	RtoS(ptPoint[2]) + _T(")) ");
	}
	else
	{
		sCommand = _T("(dcl_Insert \"");
		sCommand +=	sLispSafePath + sFile +	_T("\" (list ");
		sCommand +=	RtoS(ptPoint[0]) + _T(" ");
		sCommand +=	RtoS(ptPoint[1]) + _T(" "); 
		sCommand +=	RtoS(ptPoint[2]) + _T(")) ");
	}
	
	Acad::ErrorStatus es;
	
	// get current document
	AcApDocument* pDoc = acDocManager->curDocument();

	// give the command bar focus
	//CWnd* CmdBarWnd = acedGetAcadDockCmdLine();
	//CmdBarWnd->SetFocus();		

	// send cancel string to current document
	es = acDocManager->sendStringToExecute(pDoc, sCommand, false, true, false);		
}

static Adesk::Boolean append(AcDbDatabase*pDb, AcDbEntity* pEntity, int vport)
{
	AcDbBlockTableRecord *pBlockRec;
	Acad::ErrorStatus es = acdbOpenObject( pBlockRec, pDb->currentSpaceId(), AcDb::kForWrite );
	if (es != Acad::eOk)
	{
		theWorkspace.DisplayAlert(_T("Failed to get block table record!"));
		return Adesk::kFalse;
	}

	es = pBlockRec->appendAcDbEntity(pEntity);
	pBlockRec->close();
	if (es != Acad::eOk)
	{
		theWorkspace.DisplayAlert(_T("Failed to append entity!"));
		delete pEntity;
		return Adesk::kFalse;
	}
	return Adesk::kTrue;
}


static BOOL PasteFromData(COleDataObject* pDataObject, CRect& resRect, int& objType)
{
	STGMEDIUM medium;

	if (!SUCCEEDED(pDataObject->GetData(CF_TEXT, &medium)))
		return FALSE;

	CRect rect;
	int type, left, top, right, bottom;
	  
	char * pstr = (char*) GlobalLock (medium.hGlobal);
	if (sscanf(pstr, "%d %d %d %d %d", &type, &left, &top, &right, &bottom) != 5)
		TRACE("Getting incorrect ObjType & CRect information\n");
	GlobalUnlock(medium.hGlobal);

	rect.SetRect(left, top, right, bottom);
	rect.NormalizeRect();
	resRect = rect;
	objType = type;

	ReleaseStgMedium(&medium);
	return TRUE;
}

CMyOverrideDropTarget::CMyOverrideDropTarget()
{
	m_pBlockNames = NULL;
}

CMyOverrideDropTarget::~CMyOverrideDropTarget()
{
}

DROPEFFECT CMyOverrideDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point)
{
  if (! pDataObject->IsDataAvailable(CF_TEXT))
    return DROPEFFECT_NONE;
  return OnDragOver(pWnd, pDataObject, dwKeyState, point); 
}


DROPEFFECT CMyOverrideDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point)
{
  // find the tracker size
  CSize trackerSize;
  
  if (NULL==pDataObject)
		return DROPEFFECT_NONE;;

  // check if it's the same point
	if (point == m_lastDragPt)
		return DROPEFFECT_COPY;
    
	return DROPEFFECT_COPY;
}


void CMyOverrideDropTarget::OnDragLeave(CWnd* pWnd)
{
	if (m_prevDropEffect != DROPEFFECT_NONE)
	{
		CDC* pDC = pWnd->GetDC();
		ASSERT(pDC);
		// erase previous focus rect
		pDC->DrawFocusRect(CRect(m_lastDragPt, m_trackerSize)); 
		pDC->DeleteDC();
		m_prevDropEffect = DROPEFFECT_NONE;
	}
}
 
DROPEFFECT CMyOverrideDropTarget::OnDropEx(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point)
{
	acedDwgPoint dwgPt;

	int windnum = acedGetWinNum(point.x, point.y);
	if (windnum == 0)
		return FALSE;

	bool bUseSendString = true;
	
	acedCoordFromPixelToWorld(windnum, point, dwgPt);
    
	if (m_pThisArxControl->GetType() == CtlDwgList)
	{
		// get the path. The first item always holds the path.
		CString sPath = m_pBlockNames->GetAt(0);
		// get the event defun name
		CString sEventDefun = m_pThisArxControl->GetStringProperty(Prop::DragnDropToAutoCAD);

		// if the event defun name is valid
		if (sEventDefun.GetLength() > 0)
		{
			//acedGetAcadFrame()->SetActiveWindow();

			// give the command bar focus
			//CWnd* CmdBarWnd = acedGetAcadDockCmdLine();
			//if (CmdBarWnd != NULL)
			//	CmdBarWnd->SetFocus();		

			InvokeMethodPoint3DInt(
				sEventDefun,
				dwgPt,
				windnum,
				bUseSendString);	

			// return here on purpose to skip the insert if we are to fire the event.
			return -1;
		}

		// other wise loop thru all files to insert.
		for (int i=1; i<m_pBlockNames->GetSize(); i++)
		{
			if (m_pThisArxControl->GetLongProperty(Prop::InsertOrXref) == 0)
				ReadExternalDWGfile(sPath, m_pBlockNames->GetAt(i), dwgPt, false);	
			else
				ReadExternalDWGfile(sPath, m_pBlockNames->GetAt(i), dwgPt, true);		
		}			
		
		// Set Focus to AutoCAD because AutoCAD doesn't update its
		// display if it's not in focus.
		acedGetAcadFrame()->SetActiveWindow();

		// give the command bar focus
		//CWnd* CmdBarWnd = acedGetAcadDockCmdLine();
		//if (CmdBarWnd != NULL)
		//	CmdBarWnd->SetFocus();		
    return -1; // calls OnDrop();
	}
	else if (m_pBlockNames != NULL)
	{
		CString sEventName = m_pThisArxControl->GetStringProperty(Prop::DragnDropToAutoCAD);

		if (sEventName.GetLength() > 0)
		{
			InvokeMethodPoint3DInt(
				sEventName,
				dwgPt,
				windnum,
				bUseSendString);

			return -1;
		}

		AcDbObjectId recordId;
		AcDbBlockTable *pTable;

		// Get the object ID of the block definition.
		AcDbDatabase *pDb = AcApGetDatabase((CView*)pWnd);
		CAutoDocWriteLock CurDocLock( pDb );

		pDb->getBlockTable(pTable, AcDb::kForRead);
		
		for (int i=0; i<m_pBlockNames->GetSize(); i++)
		{
			CString sBlockName = m_pBlockNames->GetAt(i);
			if (sBlockName.GetLength() > 0)
			{
				pTable->getAt(sBlockName, recordId);
				
				if (recordId != NULL)
				{
					AcGePoint3d pt(dwgPt[0], dwgPt[1], dwgPt[2]);

					// Create a new insert.
					AcDbBlockReference *pInsert = new AcDbBlockReference(pt, recordId);

					::append(pDb, pInsert, windnum);
					pInsert->close();
				}
			}
		}
		pTable->close();

		// Set Focus to AutoCAD because AutoCAD doesn't update its
		// display if it's not in focus.
		acedGetAcadFrame()->SetActiveWindow();

		// give the command bar focus
		//CWnd* CmdBarWnd = acedGetAcadDockCmdLine();
		//if (CmdBarWnd != NULL)
		//	CmdBarWnd->SetFocus();		
	}
	else
	{
		InvokeMethodPoint3DInt(
			m_pThisArxControl->GetStringProperty(Prop::DragnDropToAutoCAD),
			dwgPt,
			windnum,
			bUseSendString);
	}
  return -1; // calls OnDrop();
}


BOOL CMyOverrideDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	BOOL fRet=TRUE;

	if (NULL == pDataObject)
		return FALSE;

	OnDragLeave(pWnd);

	CRect rSize;
	int objType;
	fRet = ::PasteFromData(pDataObject, rSize, objType);

	if (rSize.IsRectEmpty() == TRUE)
		return FALSE;

	//toggle lock mode (I'm not sure why, but I left it here just in case it has a purpose -- ORW [2007-09-03])
	{ CAutoDocWriteLock CurDocLock( AcApGetDatabase( (CView*)pWnd ) ); }

	// Set Focus to AutoCAD because AutoCAD doesn't update its
	// display if it's not in focus.
	acedGetAcadFrame()->SetActiveWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// COleOdcDropTarget - Enable OLE dragdrop for the ListBox control

COleOdcDropTarget::COleOdcDropTarget() 
{
	m_pParent = NULL;
	m_pThisArxControl = NULL;
}

COleOdcDropTarget::~COleOdcDropTarget() {}

//
// OnDragEnter is called by OLE dll's when drag cursor enters
// a window that is REGISTERed with the OLE dll's
//
DROPEFFECT COleOdcDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* 
           pDataObject, DWORD dwKeyState, CPoint point )
{   
	/*
    // If point is inside of the source list box, don't 
    // allow a drop                             
    pWnd->ClientToScreen(&point);   // convert client point to screen point
    
    */             
    // Check if the Control key was pressed
    if((dwKeyState&MK_CONTROL) == MK_CONTROL)
        return DROPEFFECT_COPY; // copy source text
    else
        return DROPEFFECT_MOVE; // move source text   
		
}

//
// OnDragLeave is called by OLE dll's when drag cursor leaves
// a window that is REGISTERed with the OLE dll's
//
void COleOdcDropTarget::OnDragLeave(CWnd* pWnd)
{
    // Call base class implementation
    COleDropTarget:: OnDragLeave(pWnd);
}

// 
// OnDragOver is called by OLE dll's when cursor is dragged over 
// a window that is REGISTERed with the OLE dll's
//
DROPEFFECT COleOdcDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* 
           pDataObject, DWORD dwKeyState, CPoint point )
{          
	if (m_pThisArxControl->GetType() == CtlTree)
	{
		HTREEITEM	hitem;
		UINT		flags = 0;

		hitem = ((CImageTreeCtrl*)m_pThisArxControl->GetWindow())->HitTest(point, &flags);

		if (hitem != NULL && (TVHT_ONITEM & flags))
		{
			((CImageTreeCtrl*)m_pThisArxControl->GetWindow())->SelectDropTarget(hitem);
		}
	}

	//pWnd->ClientToScreen(&point);   // convert client to screen 
	  
	if((dwKeyState&MK_CONTROL) == MK_CONTROL)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_MOVE;    
}

// 
// OnDrop is called by OLE dll's when item is dropped in a window
// that is REGISTERed with the OLE dll's
//
BOOL COleOdcDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, 
                 DROPEFFECT dropEffect, CPoint point )
{           
	CWnd* pDwgView = acedGetAcadDwgView();

	bool bUseSendString = true;
	if (m_pThisArxControl->GetOwnerForm()->GetType() == FrmFileDlg)
		bUseSendString = false;
	
	TDclControlPtr pControl = PasteArxObjectFromClipboard();
	if (m_pThisArxControl->GetType() == CtlDwgList)
		CArxDwgListCtrl *pDwgList = (CArxDwgListCtrl*)pControl->GetWindow();
	
	if (m_pThisArxControl->GetType() == CtlTree)
	{
		HTREEITEM	hitem;
		UINT		flags = 0;
		CString		sTreeItemKey;
		CImageTreeCtrl *pTree = (CImageTreeCtrl*)m_pThisArxControl->GetWindow();

		if ((hitem = pTree->HitTest(point, &flags)) != NULL)
		{
			sTreeItemKey = pTree->GetItemKey(hitem);			
		}
		
		if (pControl == NULL)
		{
			if (sTreeItemKey.GetLength() > 0)
			{
				// From AcadDwgView
				InvokeMethodLong(
					m_pThisArxControl->GetStringProperty(Prop::DragnDropFromOther),
					(DWORD_PTR)hitem,
					(m_pThisArxControl->GetLongProperty(Prop::EventInvoke) == 1));
			}
			else
			{
				// From AcadDwgView
				InvokeMethodString(
					m_pThisArxControl->GetStringProperty(Prop::DragnDropFromOther),
					sTreeItemKey,
					(m_pThisArxControl->GetLongProperty(Prop::EventInvoke) == 1));
			}
		}
		else
		{
			if (sTreeItemKey.GetLength() == 0)
			{
				// From Control
				InvokeMethod3StringsLong(
					m_pThisArxControl->GetStringProperty(Prop::DragnDropFromControl), 
					pControl->GetOwnerProject()->GetKeyName(),
					pControl->GetOwnerForm()->GetKeyName(),
					pControl->GetStringProperty(Prop::Name),
					(DWORD_PTR)hitem,
					(m_pThisArxControl->GetLongProperty(Prop::EventInvoke) == 1));
			}
			else
			{
				// From Control
				InvokeMethod4Strings(
					m_pThisArxControl->GetStringProperty(Prop::DragnDropFromControl), 
					pControl->GetOwnerProject()->GetKeyName(),
					pControl->GetOwnerForm()->GetKeyName(),
					pControl->GetStringProperty(Prop::Name),
					sTreeItemKey,
					(m_pThisArxControl->GetLongProperty(Prop::EventInvoke) == 1));
			}
		}
	}
	else
	{
		
		if (pControl == NULL)
		{
			// From AcadDwgView
			InvokeMethodPoint(
				m_pThisArxControl->GetStringProperty(Prop::DragnDropFromOther),
				point,
				(m_pThisArxControl->GetLongProperty(Prop::EventInvoke) == 1));
		}
		else
		{
			// From Control
			InvokeMethod3StringsPoint(
				m_pThisArxControl->GetStringProperty(Prop::DragnDropFromControl), 
				pControl->GetOwnerProject()->GetKeyName(),
				pControl->GetOwnerForm()->GetKeyName(),
				pControl->GetStringProperty(Prop::Name),
				point,
				(m_pThisArxControl->GetLongProperty(Prop::EventInvoke) == 1));
		}
	}
	
	if (pControl != NULL)
		pControl = NULL; // we must delete the arx object now!!!

	return TRUE;
}

TDclControlPtr  COleOdcDropTarget::PasteArxObjectFromClipboard()
{
	TDclControlPtr pControl = NULL; 
			
	// CG: This block was added by the Clipboard Assistant component
	if (m_pParent->OpenClipboard())
	{
		UINT m_nClipboardFormat = RegisterClipboardFormat(formatname);
		HANDLE hData = ::GetClipboardData(m_nClipboardFormat);
		if (hData != NULL)
		{
			pControl = new CDclControlObject(m_pThisArxControl->GetOwnerForm()); 
			CSharedFile memFile;
			memFile.SetHandle(hData,FALSE);
			CArchive ar(&memFile, CArchive::load);
			
			// Serialize data to document
			pControl->Serialize(ar);
			ar.Close();
			
			//Resolution for CSharedFile::Detach() problem
			//Check <LINK TYPE="ARTICLE" VALUE="Q148455">Q148455</LINK> for current status.
			// _MFC_VER might need to be updated.
			#if _MFC_VER <= 0x0420
				::GlobalUnlock(memFile.Detach());
			#else
				memFile.Detach();
			#endif
		}

		CloseClipboard();
	}
	else
		AfxMessageBox(_T("Error, cannot open clipboard."));
	
	ClearTheClipboard();

	return pControl;
}

void COleOdcDropTarget::ClearTheClipboard()
{
   // CG: This block was added by the Clipboard Assistant component
	CSharedFile memFile;
	CArchive ar(&memFile, CArchive::store|CArchive::bNoFlushOnDelete);
	UINT m_nClipboardFormat = RegisterClipboardFormat(formatname);

	ar.Flush();
	HGLOBAL hData = memFile.Detach();

	//Resolution for CSharedFile::Detach() problem
	//Check <LINK TYPE="ARTICLE" VALUE="Q148455">Q148455</LINK> for current status.
	// _MFC_VER might need to be updated.
	#if _MFC_VER <= 0x0420
		::GlobalUnlock(hData);
	#endif

	if (m_pParent->OpenClipboard())
	{		
		::SetClipboardData(m_nClipboardFormat, NULL);
		CloseClipboard();
	}
	else
		AfxMessageBox(_T("Error, cannot open clipboard."));

	EmptyClipboard();
}


void CopyArxObjectToClipboard(TDclControlPtr pControl)
{
   // CG: This block was added by the Clipboard Assistant component
	CSharedFile memFile;
	CArchive ar(&memFile, CArchive::store|CArchive::bNoFlushOnDelete);
	UINT m_nClipboardFormat = RegisterClipboardFormat(formatname);

	// serialize data to archive object       
	pControl->Serialize(ar);

	ar.Flush();
	HGLOBAL hData = memFile.Detach();

	//Resolution for CSharedFile::Detach() problem
	//Check <LINK TYPE="ARTICLE" VALUE="Q148455">Q148455</LINK> for current status.
	// _MFC_VER might need to be updated.
	#if _MFC_VER <= 0x0420
		::GlobalUnlock(hData);
	#endif

	if (pControl->GetWindow()->OpenClipboard())
	{
		::SetClipboardData(m_nClipboardFormat, hData);
		CloseClipboard();
	}
	else
		AfxMessageBox(_T("Error, cannot open clipboard."));
}

DROPEFFECT BeginDragnDrop(TDclControlPtr pControl, CPoint point, bool bInvokeWithSendString)
{
	// copy this control's Arx control object to the clip board
	CopyArxObjectToClipboard(pControl);

	// call methods to invoke the event
	InvokeMethod(pControl->GetStringProperty(Prop::DragnDropBegin), bInvokeWithSendString);

	int objType = 0;
	COleDataSource *pSource = generateDataSource(objType, CRect(point, CSize(0,0)));
	
	CMyOverrideDropTarget myDT;
	CStdDropSource myDS;

	myDT.m_pThisArxControl = pControl;
	// Start overriding AutoCAD's Droptarget
	if (!acedStartOverrideDropTarget(&myDT))
		TRACE("Error in overriding Custom drop target!\n");

	DROPEFFECT dwEffect = pSource->DoDragDrop( DROPEFFECT_MOVE | DROPEFFECT_COPY, &CRect(point, CSize(3,3)), &myDS);

	// End overriding AutoCAD default droptarget
	if (!acedEndOverrideDropTarget(&myDT))
		TRACE("Error in ending override drop target\n");
	
	pSource->Empty(); 
	delete pSource;
	return dwEffect;
}

DROPEFFECT BeginDragnDropToInsertBlocks(TDclControlPtr pControl, CPoint point, bool bInvokeWithSendString, CStringArray &BlockNames)
{
	// copy this control's Arx control object to the clip board
	CopyArxObjectToClipboard(pControl);

	// call methods to invoke the event
	InvokeMethod(pControl->GetStringProperty(Prop::DragnDropBegin), bInvokeWithSendString);

	int objType = 0;
	COleDataSource *pSource = generateDataSource(objType, CRect(point, CSize(0,0)));
	
	CMyOverrideDropTarget myDT;
	CStdDropSource myDS;

	myDT.m_pBlockNames = &BlockNames;

	myDT.m_pThisArxControl = pControl;
	// Start overriding AutoCAD's Droptarget
	if (!acedStartOverrideDropTarget(&myDT))
		TRACE("Error in overriding Custom drop target!\n");

	DROPEFFECT dwEffect = pSource->DoDragDrop( DROPEFFECT_MOVE | DROPEFFECT_COPY, &CRect(point, CSize(3,3)), &myDS);

	// End overriding AutoCAD default droptarget
	if (!acedEndOverrideDropTarget(&myDT))
		TRACE("Error in ending override drop target\n");
	
	pSource->Empty(); 
	delete pSource;
	return dwEffect;
}

COleDataSource* generateDataSource(int objType, const CRect& rect)
{
	COleDataSource* pSource = new COleDataSource();
	HGLOBAL temp = GlobalAlloc(GHND, 64);
	char* data = (char*) GlobalLock(temp);
	_snprintf(data, 64,"%d %d %d %d %d", objType, rect.left, rect.top, rect.right, rect.bottom);
	GlobalUnlock(temp);
	pSource->CacheGlobalData(CF_TEXT, temp);
	return pSource;                               
}