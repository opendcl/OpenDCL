// RoundSliderCtrl.h : Header file
//
// Copyright (c) 1999 Daniel Frey
// See RoundSliderCtrl.cpp for details.

#if !defined(_ROUNDSLIDERCTRL_H_)
#define _ROUNDSLIDERCTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRoundSliderCtrl : public CSliderCtrl
{
protected:
	CPoint	m_ptCenter;
	int		m_nRadius;
	CRgn	m_rgn;

	// Color and font stuff from CLabel static control class
	LOGFONT  m_lf;
	CFont	 m_font;
	COLORREF m_crText;
	CBrush	 m_brKnob;
	CBrush	 m_brDial;

	int		m_nZero;
	bool	m_bInverted;

	CString m_strText;
	int		m_nKnobRadius;

	bool	m_bDragging;
	bool	m_bDragChanged;

public:
	DECLARE_DYNAMIC(CRoundSliderCtrl)

	CRoundSliderCtrl();
	virtual ~CRoundSliderCtrl();

	// You may use '%ld' to show the slider value. Default: "%ld?"
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


	// Font stuff ripped off from CLabel class
	CRoundSliderCtrl& SetTextColor(COLORREF crText);
	CRoundSliderCtrl& SetKnobColor(COLORREF crBkgnd);
	CRoundSliderCtrl& SetDialColor(COLORREF crBkgnd);


	//{{AFX_VIRTUAL(CRoundSliderCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

protected:
	virtual void Init();
	virtual bool SetKnob(const CPoint& pt);
	virtual void PostMessageToParent(const int nTBCode) const;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif // _DEBUG
};

//{{AFX_INSERT_LOCATION}}

#endif // _ROUNDSLIDERCTRL_H_
