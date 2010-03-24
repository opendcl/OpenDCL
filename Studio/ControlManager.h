// ControlManager.h : header file
//

#pragma once

#include "ControlGripper.h"
#include "ControlDropSource.h"
#include "DclControlObject.h"

class CDialogControl;
class CStudioDialogObject;
class CControlPane;
class CAxContainerCtrl;

#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


class CControlManager : public CWnd
{
	CDialogControl* mpDlgControl;
	TDclControlPtr mpTemplate;
	CControlPane* mpControlPane;
	CWnd* mpControlWnd;
	bool mbSelected;
	bool mbIgnoreSizing;
	CPoint mptDragInitialPos;
	CRect mrcDragInitialPos;
	UINT mnSizingType;
	CControlGripper mGripper;

	// Attributes
protected:
	bool mbUndoGroupActive;

public:
	CControlManager( CDialogControl* pDlgControl, bool bCreate = true );
	virtual ~CControlManager();

	BOOL Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd );

	CDialogControl* GetDialogControl() const { return mpDlgControl; }
	TDclControlPtr GetTemplate() const { return mpTemplate; }
	CStudioDialogObject* GetDialogObject() const;
	void SetInitialDragPosition( const CPoint& pt ) { mptDragInitialPos = pt; /*mGripper.SetThickFrame();*/ }
	CPoint GetInitialDragPosition() { return mptDragInitialPos; }

	// Control selection & manipulation
public:
	void SetSelected( bool bSelected = true );
	bool IsSelected() const { return mbSelected; }
	void ClientToControl( CRect& rcTarget ) const;
	void ClientToControl( CPoint& ptTarget ) const;
	void ControlToClient( CRect& rcTarget ) const;
	void ControlToClient( CPoint& ptTarget ) const;
	void CheckNewWindowPosition( CRect& rcManager ) const;
	virtual UINT HitTest( const CPoint& point, bool bNoResize = false ) const; //point is in screen coordinates

public:
	virtual void OnControlPositionChanged();
	virtual void StartSizing( UINT nHitTest, const CPoint& ptStart );

protected:
	virtual void OnSelected( bool bSelected );

protected:
	void AutoSize();

protected:
	DECLARE_MESSAGE_MAP()

	// Generated message map functions
protected:
	virtual void PostNcDestroy();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
