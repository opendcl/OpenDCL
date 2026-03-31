#pragma once

#include "ArxControlAcadDropTarget.h"


class CAcadBlockInsertDropTarget : public CArxControlAcadDropTarget
{
	CPoint mptLastDrag;
	DROPEFFECT mLastEffect;
	CSize mszTracker;

public:
	CAcadBlockInsertDropTarget( CDialogControl* pDlgControl );
	virtual ~CAcadBlockInsertDropTarget(void);

public:
	static UINT GetAcadBlockClipboardFormat();

public:
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) override;
	DROPEFFECT OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point) override;
};
