// OleOdcDropTarget.h : header file
//

#pragma once

class CDclControlObject;


void CopyArxObjectToClipboard(CDclControlObject *pControl);
COleDataSource* generateDataSource(int objType, const CRect& rect);
DROPEFFECT BeginDragnDrop(CDclControlObject *pControl, CPoint point, bool bInvokeWithSendString);
DROPEFFECT BeginDragnDropToInsertBlocks(CDclControlObject *pControl, CPoint point, bool bInvokeWithSendString, CStringArray &BlockNames);


class CMyOverrideDropTarget  : public COleDropTarget
{
public:
    CMyOverrideDropTarget();
    ~CMyOverrideDropTarget();
	CDclControlObject *m_pThisArxControl;
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
	CDclControlObject *m_pThisArxControl;

// Implementation
public:
	virtual ~COleOdcDropTarget(); 
	CDclControlObject * PasteArxObjectFromClipboard();
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
