// ArxControlAcadDropTarget.h : header file
//

#pragma once

class CDialogControl;


/////////////////////////////////////////////////////////////////////////////
// CArxControlAcadDropTarget window

class CArxControlAcadDropTarget : public COleDropTarget
{
	CDialogControl* mpDlgControl;

protected:
	bool mbDropExCalled;

public:
	CArxControlAcadDropTarget( CDialogControl* pDlgControl );
	virtual ~CArxControlAcadDropTarget();

// COleDropTarget Overrides
protected:
	DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point ) override;
	DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point ) override;
	void OnDragLeave( CWnd* pWnd ) override;
	BOOL OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point ) override;
	DROPEFFECT OnDropEx( CWnd* pWnd, COleDataObject* pDataObject,
											 DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point) override;
};
