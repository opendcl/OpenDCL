// StudioDialogObject.h : interface of the CStudioDialogObject class
//


#pragma once

#include "DialogObject.h"
#include "StudioControlPane.h"
#include "StudioProject.h"
#include "AcadColorService.h"
#include "SelectedControl.h"
#include "FontSettings.h"
#include <set>


class COpenDCLDoc;
class CDclFormView;
class CDialogControl;

#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


class CStudioDialogObject : public CDialog, public CDialogObject
{
	CStudioControlPane mControlPane;
	CDclFormView* mpFormView;
	CAcadColorService mColorService;
	bool mbDrawing;
	bool mbMoving;
	bool mbSizing;
	bool mbMouseDown;
	int mnDragQuadrant;
	CPoint mptDragStart;
	CRect mrcDraw;
	CRect mrcDrawLast;
	UINT mnGridSpacing;

protected:
	CStudioDialogObject();
public:
	CStudioDialogObject( TDclFormPtr pSourceForm, CDclFormView* pFormView, bool bCreate = true );
	virtual ~CStudioDialogObject();

// Attributes
public:
	CDclFormView* GetFormView() const { return mpFormView; }
	TStudioProjectPtr GetStudioProject() const;
	TDclFormPtr GetSourceForm() const { return mpSourceForm; }

// Operations
public:
	virtual CPoint& SnapToGrid( CPoint& pt, bool bLimitToControlArea = false ) const;
	virtual void OnFontChange( const FontSettings& FS, UINT flags = fontAll ); // change font
	virtual void OnGridSpacingChange( UINT nGridSpacing ); // change grid spacing
	virtual void OnUpdateTabOrder() { GetControlPane()->ApplyTabOrder(); }
	virtual void OnActivateDclControl( TDclControlPtr pDclControl ) { SelectControl( pDclControl ); }
	virtual UINT HitTest( const CPoint& point ) const;
	virtual TDclControlPtr InsertControl( ControlType type, const CRect& rcControl, bool bActivateNow = true, bool bToTopOfTabOrder = true );
	virtual void OnControlDeleted( TDclControlPtr pDclControl );

// Implementation
public:
	bool HasSelection() const;
	bool IsOnlyFormActive() const;
	bool IsControlSelected( TDclControlPtr pDclControl ) const;
	void GetSelectedControls( TDclControlList& SelectedControls, bool bIgnoreForm = true );
	size_t CountSelected( bool bIgnoreForm = true ) const;
	bool PromptRemoveChildTabPane( int nIndex );
	CDialogControl* GetControlAtPoint( const CPoint& pt ); //pt is in screen coordinates
	int GetNextControlId();

protected:
	virtual void SelectControl( TDclControlPtr pDclControl );
	virtual void ShiftSelection( long lHoriz, long lVert );

protected:
	void TabOrderSelectedControls( bool bToFront );
	CString GetNextControlName( LPCTSTR pszRootName );

// CDialogObject Overrides
public:
	FormType GetType() const override;
	const CControlPane* GetControlPane() const override { return &mControlPane; }
	CControlPane* GetControlPane() override { return &mControlPane; }
	CAcadColorService* GetColorService() override { return &mColorService; }
	bool IsModeless() const override;
	bool IsDockable() const override { return false; }
	bool IsResizable() const override;
	HWND GetHWnd() const override { return m_hWnd; }
	bool IsFloating() const override { return false; }
	void CloseDialog(int nStatus = -1) override;
protected:
	bool CreateModeless( UINT nID ) override;
	DWORD GetWndStyle() const override; //get window style from properties
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	bool OnApplyName( TPropertyPtr pProp ) override; //Prop::Name

	virtual TDialogControlPtr CreateNewDialogControl( TDclControlPtr pTemplate, UINT nID );

public:
	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	BOOL OnInitDialog() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnSelectPointerTool();
	afx_msg void OnUpdateSelectPointerTool(CCmdUI *pCmdUI);
	afx_msg void OnShiftLeft();
	afx_msg void OnUpdateShiftLeft(CCmdUI *pCmdUI);
	afx_msg void OnShiftRight();
	afx_msg void OnUpdateShiftRight(CCmdUI *pCmdUI);
	afx_msg void OnShiftUp();
	afx_msg void OnUpdateShiftUp(CCmdUI *pCmdUI);
	afx_msg void OnShiftDown();
	afx_msg void OnUpdateShiftDown(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnProperties();
	afx_msg void OnUpdateProperties(CCmdUI *pCmdUI);
	afx_msg void OnAxProperties();
	afx_msg void OnUpdateAxProperties(CCmdUI *pCmdUI);
	afx_msg void OnFontProperties();
	afx_msg void OnTooltipProperties();
	afx_msg void OnForeColorProperties();
	afx_msg void OnBackColorProperties();
	afx_msg void OnAltColorProperties();
	afx_msg void OnImageListProperties();
	afx_msg void OnSize(UINT nType, int cx, int cy);		
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditObjectbrowser();
	afx_msg void OnUpdateEditObjectbrowser(CCmdUI *pCmdUI);
	afx_msg void OnToolsSetlispsymbolnames();
	afx_msg void OnUpdateToolsSetlispsymbolnames(CCmdUI *pCmdUI);
	afx_msg void OnToolsClearlispsymbolnames();
	afx_msg void OnUpdateToolsClearlispsymbolnames(CCmdUI *pCmdUI);
	afx_msg void OnToolsReseteventnames();
	afx_msg void OnUpdateToolsReseteventnames(CCmdUI *pCmdUI);
	afx_msg void OnBringtofront();
	afx_msg void OnUpdateBringtofront(CCmdUI *pCmdUI);
	afx_msg void OnSendtoback();
	afx_msg void OnUpdateSendtoback(CCmdUI *pCmdUI);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
};
