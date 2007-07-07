// DwgDirList.h : header file
//

#pragma once

#include "ClrListBox.h"
#include "OleOdcDropTarget.h"
#include "PPToolTip.h"

class CComboBoxFolder;

class ThumbnailFile : public CObject
{
public:
	ThumbnailFile(CString *pPath = NULL, CString sFileName = "")
	{
		m_sFileName = sFileName;
		m_pPath = pPath;
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


	CString m_sFileName;
	HGLOBAL m_hDIB;
	HBITMAP m_hBitmap;
	bool m_bLoaded;
	CString *m_pPath;
	int nIndex;
	int nBmpHeight;
	int nBmpWidth;
};



typedef CList<ThumbnailFile*> CThumbnailFileList;

/////////////////////////////////////////////////////////////////////////////
// CDwgDirList window

class CDwgDirList : public CClrListBox
{
// Construction
public:
	CDwgDirList();

// Attributes
public:
	CThumbnailFileList	m_ThumbnailFileList;
	int					m_RowHeight;
	short				m_CurSel;
	bool				m_bInvokeWithSendString;
	CPPToolTip			m_ToolTip;
	CImageList			m_imageList;
	CString				m_sPath;
	CComboBoxFolder		*m_pDirComboBox;
	HBITMAP				m_hR14LogoLarge;
	HBITMAP				m_hR14LogoSmall;


	void CalcRowHeight(ThumbnailFile *pTFile, int cx = 0);
	//*****
    // COleOdcDropTarget, derived from COleDropTarget gives us
    // the functionality to be an OLE drop target.
    //*****
    COleOdcDropTarget	m_DropTarget;

// Operations
public:
	int HitTest(CPoint point);
	void SetRowHeight(int nTheRowHeight);
	void SetDragnDrop(BOOL bRegister);
	void CreateImageList();
	void Dir(CString sDir);
	void ClearThumbnailList();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDwgDirList)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDwgDirList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDwgDirList)
	afx_msg void OnDblclk();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSelchange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
