// ControlPane.h : header file
//

#pragma once

#include "DialogControl.h"
#include "ThemeHelperST.h"
#include "Project.h"
#include "DclFormObject.h"
#include <vector>

class CDialogObject;
class CFontCollection;
enum Prop::Id;


/////////////////////////////////////////////////////////////////////////////
// CControlPane window

class CControlPane
{
public:
	typedef std::vector< TDialogControlPtr > TDialogControls;

// Attributes
protected:
	TProjectPtr mpProject;
	CDialogObject* mpDlgObject;
	TDclFormPtr mpSourceForm;
	TDialogControls mControls;
	CWnd* mpHostDlg;
	CThemeHelperST mThemeHelper;
	bool mbRecalcInProgress;
	HDWP mhDeferredPos;

// Construction
protected:
	CControlPane();
public:
	CControlPane( TDclFormPtr pSourceForm, CWnd* pHostDlg );
	virtual ~CControlPane();

// Properties
public:
	TProjectPtr GetProject() const { return mpProject; }
	TDclFormPtr GetSourceForm() const { return mpSourceForm; }
	CWnd* GetHostDialog() const { return mpHostDlg; }
	CRect GetControlArea() const;
	const TDialogControls& GetControlsList() const { return mControls; }
	CPoint GetSplitterPos( int nSplitterId );
	bool IsRecalcInProgress() const { return mbRecalcInProgress; }

// Operations
public:
	bool CreateControls( UINT& nId );
	void AddControl( TDialogControlPtr pDlgControl );
	void RemoveControl( TDialogControlPtr pDlgControl );
	void ResetControlsPos( TDclControlPtr pControl );
	void SetFirstControlFocus() const;

protected:
	void InvalidateControls();
	virtual void ZOrderFront( TDialogControlPtr pDlgControl, HDWP hDeferred = NULL );
	virtual void ZOrderBack( TDialogControlPtr pDlgControl, HDWP hDeferred = NULL );
	virtual bool IsInvisibleControlAllowed( TDialogControlPtr pDlgControl ) const { return true; }

// Implementation
public:
	virtual void CleanUpControls();
	virtual CThemeHelperST* GetThemeHelper() { return &mThemeHelper; }
	virtual bool FindControl( HWND hwndControl, /*out*/ CString& sControlName ) const; //if found, returns true & sets sControlName
	virtual TDialogControlPtr FindControl( HWND hwndControl ) const;
	virtual TDialogControlPtr FindControl( LPCTSTR pszControlName, ControlType type = _CtlInvalid ) const;
	virtual void SetGlobalLispSymbols( bool bResetToNil = false ) const {}
	virtual void RecalcLayout(); //recalculate control positions
	virtual void ApplyZOrder(); //reorder all control windows to reflect current Z order
	virtual void ApplyPosition( TDialogControlPtr pDlgControl ); //move control window to new position
	virtual void ApplyVisibility( TDialogControlPtr pDlgControl ); //show or hide control

protected:
	virtual TDialogControlPtr CreateNewDialogControl( TDclControlPtr pTemplate, UINT nID ) = 0;
};
