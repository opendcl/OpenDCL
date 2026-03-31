// ArxDwgListCtrl.h : header file
//

#pragma once

#include "ListBoxCtrl.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"
#include "AcadBlockInsertDropTarget.h"
#include "DwgThumbnail.h"

class CArxFolderComboCtrl;


/////////////////////////////////////////////////////////////////////////////
// CArxDwgListCtrl window

class CArxDwgListCtrl : public CListBoxCtrl
{
	CArxControlServices	mArxServices;
	CAcadBlockInsertDropTarget mBlockInsertDropTarget;
	CArxDragDropService mDragDropService;
	int mnRowHeight;
	CString msPath;

	CImageList mImageList;

	typedef CMap<CString, LPCTSTR, CDwgThumbnailPtr, CDwgThumbnailPtr> TDwgThumbnailMap;
	TDwgThumbnailMap mmapThumbnail;

public:
	CArxFolderComboCtrl* m_pDirComboBox;

// Construction
public:
	CArxDwgListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxDwgListCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	CDragDropService* GetDragDropService() override { return &mDragDropService; }
	COleDropTarget* GetDropOnAcadTarget() override { return &mBlockInsertDropTarget; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	void HandleDpiChanged() override; //handle relayed WM_DPICHANGED_AFTERPARENT message
	bool ApplyProperty( TPropertyPtr pProp ) override;

// ListBoxCtrl Interface
	LPCTSTR GetDragTextPrefix() const override;

// Operations
public:
	void Dir(LPCTSTR pszDir);

protected:
	int HitTest(CPoint point);
	void ClearThumbnailList();

// Overrides
protected:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg void OnDblclk();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCbnSelchange();
};
