// ArxDwgListCtrl.h : header file
//

#pragma once

#include "ListBoxCtrl.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"
#include "AcadBlockInsertDropTarget.h"

class CArxFolderComboCtrl;


class ThumbnailFile : public CObject
{
public:
	CString m_sFileName;
	HGLOBAL m_hDIB;
	HBITMAP m_hBitmap;
	bool m_bLoaded;
	CString m_sPath;
	int nIndex;
	int nBmpHeight;
	int nBmpWidth;

public:
	ThumbnailFile()
	{
		m_hDIB = NULL;
		m_hBitmap = NULL;
		m_bLoaded = false;
		nBmpHeight = 0;
		nBmpWidth = 0;
	}
	ThumbnailFile( const CString& sPath, LPCTSTR pszFilename = NULL )
	{
		m_sFileName = pszFilename;
		m_sPath = sPath;
		m_hDIB = NULL;
		m_hBitmap = NULL;
		m_bLoaded = false;
		nBmpHeight = 0;
		nBmpWidth = 0;
	}
	~ThumbnailFile()
	{
		try
		{
			if (m_hDIB != NULL)
			{
				::GlobalFree(m_hDIB);
				m_hDIB = NULL;
			}
			if (m_hBitmap != NULL)
			{
				DeleteObject(m_hBitmap);
				m_hBitmap = NULL;
			}		
		}
		catch(...)
		{
		}
	}
	BOOL LoadDwgThumbnail();
	void DrawBitmap(CDC* pDC, CPoint pt, int nPaintableWidth, int nPaintableHeight, HBITMAP &hR14LogoLarge, HBITMAP &hR14LogoSmall);
	CSize GetBitmapSize();
};

typedef CList<ThumbnailFile*> CThumbnailFileList;


/////////////////////////////////////////////////////////////////////////////
// CArxDwgListCtrl window

class CArxDwgListCtrl : public CListBoxCtrl
{
	CArxControlServices	mArxServices;
	CAcadBlockInsertDropTarget mBlockInsertDropTarget;
	CArxDragDropService mDragDropService;
	int mnRowHeight;
	HBITMAP mhR14LogoLarge;
	HBITMAP mhR14LogoSmall;
	CString msPath;

	CThumbnailFileList	m_ThumbnailFileList;
	CImageList			m_imageList;

public:
	CArxFolderComboCtrl* m_pDirComboBox;

// Construction
public:
	CArxDwgListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxDwgListCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual CDragDropService* GetDragDropService() { return &mDragDropService; }
	virtual COleDropTarget* GetDropOnAcadTarget() { return &mBlockInsertDropTarget; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool ApplyProperty( TPropertyPtr pProp );

// ListBoxCtrl Interface
	virtual LPCTSTR GetDragTextPrefix() const;

// Operations
public:
	int HitTest(CPoint point);
	void CreateImageList();
	void Dir(CString sDir);
	void ClearThumbnailList();

// Overrides
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg void OnDblclk();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSelchange();
};
