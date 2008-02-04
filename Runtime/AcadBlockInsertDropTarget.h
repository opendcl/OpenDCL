#pragma once

class CDialogControl;


class CAcadBlockInsertDropTarget : public COleDropTarget
{
	CDialogControl* mpDlgControl;
	CPoint mptLastDrag;
	DROPEFFECT mLastEffect;
	CSize mszTracker;

public:
	CAcadBlockInsertDropTarget( CDialogControl* pDlgControl );
	virtual ~CAcadBlockInsertDropTarget(void);

public:
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point);
};
