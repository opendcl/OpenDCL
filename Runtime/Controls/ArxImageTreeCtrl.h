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
		virtual bool HandleDragOverControl( COleDataObject* pDataObject, DWORD dwKeyState,
																				const CPoint& point, DROPEFFECT& dwEffect ) const
			{ return false; } //force it to use the tree control's custom OnDragOver event handler
		virtual bool HandleDropOnControl( COleDataObject* pDataObject, DROPEFFECT dropEffect,
																			const CPoint& point ) const
			{ return false; } //force it to use the tree control's custom OnDrop event handler
	} mArxServices;
	CArxDragDropService mDragDropService;
	bool mbCancelLabelEdit;

public:
	CArxImageTreeCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxImageTreeCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual CDragDropService* GetDragDropService() { return &mDragDropService; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DROPEFFECT OnDragOver( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState );
	virtual bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT dropEffect );

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
