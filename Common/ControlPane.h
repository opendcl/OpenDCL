// ControlPane.h : header file
//

#pragma once

#include "DialogControl.h"
#include <vector>

class CProject;
class CDclControlObject;
class CDclFormObject;
class CFontCollection;
enum PropertyId;


const int EditFilter_String		= 0;
const int EditFilter_Angle		= 1;
const int EditFilter_Integer	= 2;
const int EditFilter_Numeric	= 3;
const int EditFilter_Symbol		= 4;
const int EditFilter_UpperCase	= 5;
const int EditFilter_LowerCase	= 6;
const int EditFilter_Password	= 7;
const int EditFilter_Multiline	= 8;

const int ButtonStyle_Raised	= 0;
const int ButtonStyle_Flat		= 1;
const int ButtonStyle_Pick		= 2;
const int ButtonStyle_Select	= 3;
const int ButtonStyle_Filter	= 4;
const int ButtonStyle_NoBorder	= 5;

const int CmboStyle_Combo		= 0;
const int CmboStyle_Simple		= 1;
const int CmboStyle_DropDown	= 2;
const int CmboStyle_ArrowHead	= 3;
const int CmboStyle_Color		= 4;
const int CmboStyle_LineWeight	= 5;
const int CmboStyle_PlotNames	= 6;
const int CmboStyle_PlotTables	= 7;
const int CmboStyle_FontDropList	= 8;
const int CmboStyle_FontSimpleList	= 9;
const int CmboStyle_Plotters		= 10;
const int CmboStyle_PlotterPaperSizes = 11;
const int CmboStyle_DirPicker = 12;
const int CmboStyle_Layers = 13;


/////////////////////////////////////////////////////////////////////////////
// CControlPane window

class CControlPane  : public CObject
{
public:
	typedef std::vector< TDialogControlPtr > TDialogControls;

// Attributes
protected:
	CProject* mpProject;
	CDclFormObject *mpSourceForm;
	TDialogControls mControls;
	CWnd* mpHostDlg;
	CRect mPaneWindowPos;

// Construction
protected:
	CControlPane();
public:
	CControlPane(CDclFormObject* pSourceForm, CWnd* pHostDlg);
	virtual ~CControlPane();

// Properties
public:
	CProject* GetProject() const { return mpProject; }
	CDclFormObject* GetSourceForm() const { return mpSourceForm; }
	CWnd* GetHostDialog() const { return mpHostDlg; }
	const CRect& GetPaneWindowRect() const { return mPaneWindowPos; }
	CRect& GetPaneWindowRect() { return mPaneWindowPos; }
	const TDialogControls& GetControlsList() const { return mControls; }
	TDialogControls& GetControlsList() { return mControls; }

// Operations
public:
	void SetFirstControlFocus() const;
	void Refresh() { SizeChanged( mPaneWindowPos.Width(), mPaneWindowPos.Height() ); }
	bool CreateControls( CDclFormObject* pDclForm, UINT& nId );
	void AddControl( TDialogControlPtr pControl );

	void UpdateGlobalVariables();
	void SizeChanged(int cx, int cy, bool bRefreshOthers = false);
	void ShowWindow(BOOL bShow);
	void ShowPictureBoxes(BOOL bShow);
	void SetGrphcBtnsParents(bool bForceRefresh);
	void ResetControlsPos2(CDclControlObject *pControl);

protected:
	void SetGrphcBtnsParents(CDclControlObject *pGrphcBtn, CDclControlObject *pOtherBtn, bool bForceRefresh);
	CRect GetSplitterRect(int nId);
	void InvalidateControls();
	void ResetControlsPos(CDclControlObject *pControl, int cx, int cy, bool bRefreshOthers = false);
	void ResetControlsPos(CDclControlObject *pControl, bool bRefreshOthers = false);
	void ResetControlsPos2(CDclControlObject *pControl, int cx, int cy);
	void ZOrderFront(CWnd *pControl);
	void ZOrderBack(CWnd *pControl);

// Implementation
public:
	virtual void CleanUpControls();
	virtual bool FindControl(HWND hwndControl, /*out*/ CString& sControlName) const = 0; //if found, returns true & sets sControlName
	virtual TDialogControlPtr FindControl(HWND hwndControl) const = 0;
	virtual CDclControlObject* FindArxObject(CString sControlName) const = 0;
	virtual CWnd* FindControl(CString sControlName, int nControlType) const = 0;
	virtual class CThemeHelperST* GetThemeHelper() = 0;
	virtual bool CreateTabPanes(CDclFormObject* pTemplate, CWnd *pTabControl, UINT& nId, CRect rcTab) = 0;

protected:
	virtual TDialogControlPtr CreateNewDialogControl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID ) = 0;
};
