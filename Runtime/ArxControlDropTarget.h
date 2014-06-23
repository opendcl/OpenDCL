// ArxControlDropTarget.h : header file
//

#pragma once

class CDialogControl;


/////////////////////////////////////////////////////////////////////////////
// CArxControlDropTarget window

class CArxControlDropTarget : public COleDropTarget
{
	CDialogControl* mpDlgControl;

public:
	CArxControlDropTarget( CDialogControl* pDlgControl );
	virtual ~CArxControlDropTarget();

// Implementation
public:
	TDclControlPtr PasteArxObjectFromClipboard();
	void ClearTheClipboard();

// COleDropTarget Overrides
	virtual DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual void OnDragLeave( CWnd* pWnd );               
	virtual BOOL OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );
	virtual DROPEFFECT OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point);
};
