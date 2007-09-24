// RoundSliderCtrl.h : Header file
//
// Copyright (c) 1999 Daniel Frey
// See RoundSliderCtrl.cpp for details.

#pragma once

#include "PPToolTip.h"
#include "PtrTypes.h"

class CPropertyObject;


class CRoundSliderCtrl : public CSliderCtrl
{
protected:
	CPoint	m_ptCenter;
	int		m_nRadius;
	CRgn	m_rgn;

	int		m_nZero;
	bool	m_bInverted;

	CString	m_strText;
	int		m_nKnobRadius;

	bool	m_bDragging;
	bool	m_bDragChanged;
// attributes
public:
	CPPToolTip m_ToolTip;
	
	void SetPos(int nPos);
	int		m_nPos;
	CFont	*m_pFont;
	TPropertyPtr m_pValueProp;
public:
	//DECLARE_DYNAMIC(CRoundSliderCtrl)

	CRoundSliderCtrl();
	virtual ~CRoundSliderCtrl();

	// You may use '%ld' to show the slider value. Default: "%ld°"
	virtual void SetText(const CString& strNewText);
	virtual CString GetText() const;

	// Override this function for customized output
	virtual CString OnFormatText();

	// Default: 7, Minimum: 5
	virtual void SetKnobRadius(const int nNewKnobRadius);
	virtual int GetKnobRadius() const;

	// 0-359, 0 = Top, 90 = Right, 180 = Bottom, 270 = Left
	virtual void SetZero(const int nZero);
	virtual int GetZero() const;

	virtual void SetInverted(const bool bNewInverted = true);
	virtual bool GetInverted();

	//{{AFX_VIRTUAL(CRoundSliderCtrl)
	public:
	virtual BOOL Create(TDclControlPtr pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

public:
	virtual void Init();
protected:
	
	virtual bool SetKnob(const CPoint& pt);
	virtual void PostMessageToParent(const int nTBCode) const;

	//{{AFX_MSG(CRoundSliderCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	TDclControlPtr m_ArxControl;
	UINT m_hPos;

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif // _DEBUG
};
