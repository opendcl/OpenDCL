// ArxBlockListCtrl.h : header file
//

#pragma once

#include "BlockListCtrl.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"
#include "AcadBlockInsertDropTarget.h"

class CAcadDocReactor;
class CAcadBlockReactor;


/////////////////////////////////////////////////////////////////////////////
// CArxBlockListCtrl window

class CArxBlockListCtrl : public CBlockListCtrl
{
protected:
	CArxControlServices	mArxServices;
	CAcadBlockInsertDropTarget mBlockInsertDropTarget;
	CArxDragDropService mDragDropService;
	bool mbNoRefresh;
	AcDbDatabase* mpLoadedDwg;

	class CDocReactor : public AcApDocManagerReactor
	{
		CArxBlockListCtrl* mpCtrl;
	public:
		CDocReactor( CArxBlockListCtrl* pCtrl )
			: mpCtrl( pCtrl )
			{ acDocManager->addReactor( this ); }
		~CDocReactor()
			{ acDocManager->removeReactor( this ); }
		void documentActivated(AcApDocument* pActivatedDoc)
			{ mpCtrl->RefreshBlockList(); }
		void documentDestroyed(const char* filename)
			{ mpCtrl->RefreshBlockList(); }
	} mDocReactor;
	friend CDocReactor;

	class CEdReactor : public AcEditorReactor 
	{
		CArxBlockListCtrl* mpCtrl;
	public:
		CEdReactor( CArxBlockListCtrl* pCtrl )
			: mpCtrl( pCtrl )
			{ acedEditor->addReactor( this ); }
		~CEdReactor()
			{ acedEditor->removeReactor( this ); }

		void endInsert( AcDbDatabase* pTo ) override
			{ if( pTo ) mpCtrl->RefreshBlockList(); }
		void endDeepClone( AcDbIdMapping& idMap ) override
			{ mpCtrl->RefreshBlockList(); }
		void commandEnded( LPCTSTR cmdStr ) override
			{ if( lstrcmp( cmdStr, _T("PURGE") ) == 0 ) mpCtrl->RefreshBlockList(); }
	} mEdReactor;
	friend CEdReactor;

// Construction
public:
	CArxBlockListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxBlockListCtrl();

// DialogControl Interface
public:
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	CDragDropService* GetDragDropService() override { return &mDragDropService; }
	COleDropTarget* GetDropOnAcadTarget() override { return &mBlockInsertDropTarget; }
	DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData ) override; //called to get drag data from this control

// ListBoxCtrl Interface
	virtual LPCTSTR GetDragTextPrefix() const { return GetLoadedDwg(); }

// Implementation
public:
	bool LoadDwg( LPCTSTR pszFilename );
	LPCTSTR GetLoadedDwg() const;

protected:
	AcDbDatabase* sourceDwg() const { return mpLoadedDwg; }
	void RefreshBlockList();
	int extractPreview(const AcDbBlockTableRecord* pBTR);

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

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
