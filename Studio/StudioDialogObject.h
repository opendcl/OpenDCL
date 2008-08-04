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
	virtual void OnUpdateZOrder() { GetControlPane()->ApplyZOrder(); Invalidate(); }
	virtual void OnActivateDclControl( TDclControlPtr pDclControl ) { SelectControl( pDclControl ); }
	virtual UINT HitTest( const CPoint& point ) const;
	virtual TDclControlPtr InsertControl( ControlType type, const CRect& rcControl, bool bActivateNow = true );
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
	void ZOrderSelectedControls( bool bToFront );
	CString GetNextControlName( LPCTSTR pszRootName );

// CDialogObject Overrides
public:
	virtual FormType GetType() const;
	virtual const CControlPane* GetControlPane() const { return &mControlPane; }
	virtual CControlPane* GetControlPane() { return &mControlPane; }
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual bool IsModeless() const;
	virtual bool IsDockable() const { return false; }
	virtual bool IsResizable() const;
	virtual HWND GetHWnd() const { return m_hWnd; }
	virtual bool IsFloating() const { return false; }
	virtual void CloseDialog(int nStatus = -1);
protected:
	virtual bool CreateModeless( UINT nID );
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual TDialogControlPtr CreateNewDialogControl( TDclControlPtr pTemplate, UINT nID );
	virtual bool OnApplyName( TPropertyPtr pProp ); //Prop::Name

public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
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
	afx_msg void OnBringtofront();
	afx_msg void OnUpdateBringtofront(CCmdUI *pCmdUI);
	afx_msg void OnSendtoback();
	afx_msg void OnUpdateSendtoback(CCmdUI *pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
};
