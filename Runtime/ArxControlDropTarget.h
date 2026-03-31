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
	DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point ) override;
	DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point ) override;
	void OnDragLeave( CWnd* pWnd ) override;
	BOOL OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point ) override;
	DROPEFFECT OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point) override;
};
