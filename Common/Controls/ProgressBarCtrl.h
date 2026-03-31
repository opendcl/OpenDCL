// ProgressBarCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CProgressBarCtrl window

class CProgressBarCtrl : public CProgressCtrl, public CDialogControl
{
	CAcadColorService mColorService;
  clock_t mStartTime;
	enum Legend { lgNone = 0, lgTime = 1, lgPercent = 2, };
	Legend meLegend;
  CFont mfontHoriz;
  CFont mfontVert;

// Construction
public:
	CProgressBarCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CProgressBarCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	void ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst, std::vector< Prop::Id >& ridLast ) override;
	CAcadColorService* GetColorService() override { return &mColorService; }

public:
	void Reset();

protected:
  virtual CString GetRemainingText(double lfPercent, double lfSecsRemaining);
  virtual void ResetStartTime(void);

protected:
	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg) override;
	void PostNcDestroy() override;
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnPaint();
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
