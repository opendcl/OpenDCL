// ArxBlockListCtrl.h : header file
//

#pragma once

#include "BlockListCtrl.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"

class CAcadDocReactor;
class CAcadBlockReactor;


/////////////////////////////////////////////////////////////////////////////
// CArxBlockListCtrl window

class CArxBlockListCtrl : public CBlockListCtrl
{
protected:
	CArxControlServices	mArxServices;
	CArxDragDropService mDragDropService;
  COleDataSource m_COleDataSource; // Needed to make this control an OLE data SOURCE (see OnLButtonDown)

	class CDocReactor : public AcApDocManagerReactor
	{
		CArxBlockListCtrl* mpListCtrl;
	public:
		CDocReactor( CArxBlockListCtrl* pListCtrl )
			: mpListCtrl( pListCtrl )
			{ acDocManager->addReactor( this ); }
		~CDocReactor()
			{ acDocManager->removeReactor( this ); }
		void documentActivated(AcApDocument* pActivatedDoc)
			{ if( mpListCtrl ) mpListCtrl->RefreshBlockList(); }
		void documentDestroyed(const char* filename)
			{ if( mpListCtrl ) mpListCtrl->RefreshBlockList(); }
	};

protected:
	CDocReactor* mpDocReactor;
	CAcadBlockReactor* mpBlockReactor;	

public:
	AcDbDatabase		*m_pLoadedDwg;
	CString				m_FileName;

// Construction
public:
	CArxBlockListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxBlockListCtrl();

// DialogControl Interface
public:
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual CDragDropService* GetDragDropService() { return &mDragDropService; }

// Implementation
public:
	bool LoadDwg(CString sFileName);
	void RefreshBlockList();
	int extractPreview(const AcDbBlockTableRecord* pBTR);

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReturn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu( CWnd* pTarget, CPoint point );
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
