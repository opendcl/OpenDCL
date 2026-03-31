// ArxImageTreeCtrl.h : header file
//

#pragma once

#include "ImageTreeCtrl.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"


/////////////////////////////////////////////////////////////////////////////
// CArxImageTreeCtrl window

class CArxImageTreeCtrl : public CImageTreeCtrl
{
	class CArxImageTreeControlServices : public CArxControlServices
	{
	public:
		CArxImageTreeControlServices( CDialogControl* pDlgControl ) : CArxControlServices( pDlgControl ) {}
		bool HandleDragOverControl( COleDataObject* pDataObject, DWORD dwKeyState,
																const CPoint& point, DROPEFFECT& dwEffect ) const override
			{ return false; } //force it to use the tree control's custom OnDragOver event handler
		bool HandleDropOnControl( COleDataObject* pDataObject, DROPEFFECT& dwEffect, DROPEFFECT dwAllowed, const CPoint& point ) const override
			{ return false; } //force it to use the tree control's custom OnDrop event handler
	} mArxServices;
	CArxDragDropService mDragDropService;
	bool mbCancelLabelEdit;

public:
	CArxImageTreeCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxImageTreeCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	CDragDropService* GetDragDropService() override { return &mDragDropService; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DROPEFFECT OnDragOver( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState ) override;
	bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT& dropEffect ) override;

	BOOL SetItemText( HTREEITEM hItem, LPCTSTR lpszItem );

protected:
	DECLARE_MESSAGE_MAP();

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRdblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReturn(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocus(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
};
