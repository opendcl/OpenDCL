// ArxControlAcadDropTarget.h : header file
//

#pragma once

class CDialogControl;


/////////////////////////////////////////////////////////////////////////////
// CArxControlAcadDropTarget window

class CArxControlAcadDropTarget : public COleDropTarget
{
	CDialogControl* mpDlgControl;

public:
	CArxControlAcadDropTarget( CDialogControl* pDlgControl );
	virtual ~CArxControlAcadDropTarget();

// COleDropTarget Overrides
	virtual DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual void OnDragLeave( CWnd* pWnd );               
	virtual BOOL OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );
	virtual DROPEFFECT OnDropEx( CWnd* pWnd, COleDataObject* pDataObject,
															 DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point);
};
