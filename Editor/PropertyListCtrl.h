// PropertyListCtrl.h : Declaration of the CPropertyListCtrl Control class.

#pragma once

#include "PropertyButton.h"
#include "PropertyEdit.h"
#include "PropertyList.h"
#include "EditorProject.h"

class CProjectCollection;
class CObjectBrowser;
class CListBoxDlg;
class CImageListObject;
class CDclControlObject;
class CDclFormObject;
class CObjectDCLView;
class CColourPopup;


#define nButtonWidth  15
#define nDropDownXOffset  4
#define nEditCellXOffset  6
#define nEditCellYOffset  1
#define nNameLeftOffset 1
#define nValueLeftOffset  4
#define nTextTopOffset  2
#define nBorderDifferance  4
#define nScrollBarWidth  14
#define nDrawStyle_RemoveHighLight  0
#define nDrawStyle_SetAsHighLight  1
#define nDrawStyle_NonSelectedRow 2
#define nDrawStyle_SelectedRow 3
#define nScrollBarID  198
#define nEditCellID  199
#define IDC_DROPDOWNHOLDER  201
#define IDC_BUTTON  202
#define IDC_POPUP_LISTBOX  203
#define IDC_PROPEDIT  204
#define nReturnError  -100
#define nCharOffset  71
#define nWhite  256
#define nNotSet -1
#define nDeListRowHeight 18
#define nMAXFILE  3562
#define nGreyColor 192
#define nArchiveExtSize 4096
#define nDeTotalHeight 75
#define nDeValueSize 80
#define nDeLargestPicId 99


/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl : See PropertyListCtl.cpp for implementation.

class CPropertyListCtrl : public CWnd
{

// Constructor
public:
	CPropertyListCtrl();
	~CPropertyListCtrl();

// Atributes
public:
	HCURSOR				m_Cursor;
	CDclControlObject	*m_pControl;
	CDclFormObject		*m_pDclForm;
	CPropertyList		m_PropertyList;
	CString				m_ClassName;
	CObjectDCLView		*m_pView;
	CStatic				*m_pPropTitle;
	CEdit				*m_pPropDesc;
	CColourPopup		*m_pColorPopup;
	CFont				*m_pFont;
	CWnd				*m_pParent;
	CObjectBrowser		*m_pIntelHelp;
// Operations
public:
	void SetFont(CFont *pFont);
	void UpdateControls(PropertyId nPropId);
	void PropertyHasChanged(PropertyId nId);
	void SetScrollBar();
	void ClearArea(HDC hdc);
	void RePaint();
	void RefreshGrid(HDC hdc);
	short GetSelectionIndex();
	void SetSelectionIndex(short nNewValue);
	void SetTopIndex(short Index);
	void CloseListBox(int nInstructions);
	CString QueryForLispFileName();
	BOOL SaveDistributionFile(CProjectCollection *pProjectHolder);
	BOOL OpenProjectFile(LPCTSTR FileName, CProject *pProject);
	RefCountedPtr< CPropertyObject > GetPropertyObject(short PropertyIndex);
	void EditObjectbrowser();
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL


// Message maps
	//{{AFX_MSG(CPropertyListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnChangeEdit();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void ClearProject();
	afx_msg void ClearPictures();
	afx_msg short CountPictures();
	afx_msg short AddPictureToImageList(short DclFormIndex, short ArxControlIndex, short PropertyIndex, LPPICTUREDISP newPicture);
	afx_msg void ClearImageListPropery(short DclFormIndex, short ArxControlIndex, short PropertyIndex);
	afx_msg void InitImageList(short DclFormIndex, short ArxControlIndex, short PropertyIndex, short ImageWidth, short ImageHeight);	
	afx_msg LPPICTUREDISP GetImageListPicture(short DclFormIndex, short ArxControlIndex, short PropertyIndex, short ImageIndex);
	afx_msg long GetImageListPictureWidth(short DclFormIndex, short ArxControlIndex, short PropertyIndex);
	afx_msg long GetImageListPictureHeight(short DclFormIndex, short ArxControlIndex, short PropertyIndex);
	afx_msg void ClearGrid();
	afx_msg void DisplayProperties(CDclControlObject *pControl);
	afx_msg LPPICTUREDISP GetPicture(short Index);
	afx_msg void DeleteFlaggedControls(short DclFormIndex);
	afx_msg short GetPictureID(short Index);
	afx_msg void SetPictureID(short Index, short nID);
	afx_msg BOOL SetDclParent(short Index, LPCTSTR ParentName, short TabIndex);
	afx_msg short GetDclParentsTabIndex(short Index);
	afx_msg short GetArxControlClientHeight(short DclFormIndex, short ArxControlIndex);
	afx_msg BOOL IsArxControlDeleted(short DclFormIndex, short ArxControlIndex);
	afx_msg void DisplayVaries();
	afx_msg void SetLispFileName(CString sFileName);
	afx_msg void LoadPicture(LPCTSTR sFileName, short nPictureTag, BOOL bApplyMask);
	afx_msg LPPICTUREDISP LoadMaskedPicture(LPCTSTR sFileName);
	//afx_msg short AddPicture(short nID, LPPICTUREDISP NewPicture);
	afx_msg void CheckPictureRefs();
	afx_msg void DefaultFontDlg();
	afx_msg BOOL IsPictureValid(LPPICTUREDISP NewPicture);
	afx_msg void SetPurchaseMode(short nPurchaseMode);
	//afx_msg CString ImportOdcl(LPCTSTR sFileName);
	afx_msg void ShowPropertyDlg(bool bFontActive = false, bool bImageListActive = false);
	afx_msg void ShowAsFree();
	afx_msg void SetAutoCADVersion(DWORD nVersion);
	afx_msg DWORD GetAutoCADVersion();
	afx_msg void CreateDistributionFile();
	afx_msg void Refresh();
	void DoSetupInputType(int nNewSelectedIndex);

	
public:
	void DrawRow(int nRow, int nDrawStyle, CRect *pRcClientArea, HDC hdc);
	void DrawRowGrid(int nRow, CRect *pRcClientArea, HDC hdc);
	void DrawCell(int nRow, int nCell, int nDrawStyle, CRect *pRcClientArea, HDC hdc);
	int GetSelectedIndex(long y);
	CRect ChangeSelectedItem(int nNewSelectionIndex);	
	BOOL m_ScrollBarNeeded;
	BOOL m_ScrollBarCreated;
	int m_SelectedIndex;
	int m_TopIndex;
	int m_RowsPerPage;
	CScrollBar m_ScrollBar;
	CPropertyButton m_Button;
	CPropertyEdit	m_Edit;
	int nItemRowHeight;
	BOOL m_PopUpCreated;
	CListBoxDlg* m_pModeless;
	int SetListBox();
	short m_CopiedFromDclFormIndex;

public:
	void OnButtonPressed();	
	void OnEnterPressed();	
	CString GetPictureFile(LPCTSTR Filter);

public:	
	CDclControlObject* GetArxControlObject(short DclFormIndex, short ArxControlIndex);
	CDclFormObject* GetDclFormObject(short DclFormIndex);
	CStringList m_FileList;
	BOOL ImageListAddPicture(CPictureHolder *NewPicture, CImageListObject *pImageListObj, CImageList *pImageList, CSize *pImageSize, BOOL ApplyMask);
	CString GetOnePictureFile();
	void SearchPictureRefs(CDclFormObject *pDclObject);
	
	void FirePropertyChanged(PropertyId ChangedPropertyID);		
	void FireInvokeImageList(short PropertyIndex, long PropertyID);
	void UpdatePropHelpCtrls(RefCountedPtr< CPropertyObject > pProp);
	void FireScrolling();
	void FireInvokeFontPane();	
};
