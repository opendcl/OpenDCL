// OleOdcDropTarget.h : header file
//

#pragma once

#include "PtrTypes.h"


void CopyArxObjectToClipboard(TDclControlPtr pControl);
COleDataSource* generateDataSource(int objType, const CRect& rect);
DROPEFFECT BeginDragnDrop(TDclControlPtr pControl, CPoint point, bool bInvokeWithSendString);
DROPEFFECT BeginDragnDropToInsertBlocks(TDclControlPtr pControl, CPoint point, bool bInvokeWithSendString, CStringArray &BlockNames);


class CMyOverrideDropTarget  : public COleDropTarget
{
public:
    CMyOverrideDropTarget();
    ~CMyOverrideDropTarget();
	TDclControlPtr m_pThisArxControl;
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT OnDropEx(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point);

	CStringArray *m_pBlockNames;

private:

    CPoint m_lastDragPt;
    DROPEFFECT m_prevDropEffect;
    CSize m_trackerSize;
};


/////////////////////////////////////////////////////////////////////////////
// COleOdcDropTarget window

class COleOdcDropTarget : public COleDropTarget
{
// Construction
public:
	COleOdcDropTarget();
	CWnd *m_pParent;
	TDclControlPtr m_pThisArxControl;

// Implementation
public:
	virtual ~COleOdcDropTarget(); 
	TDclControlPtr  PasteArxObjectFromClipboard();
	void ClearTheClipboard();

 //
 // These members MUST be overridden for an OLE drop target
 // See DRAG and DROP section of OLE classes reference
 //
 DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD 
                                              dwKeyState, CPoint point );
 DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD 
                                             dwKeyState, CPoint point );
 void OnDragLeave(CWnd* pWnd);               
 
 BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT 
                                        dropEffect, CPoint point );
};
