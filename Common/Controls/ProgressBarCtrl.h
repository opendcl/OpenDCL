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
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual void ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst, std::vector< Prop::Id >& ridLast );
	virtual CAcadColorService* GetColorService() { return &mColorService; }

public:
	void Reset();

protected:
  virtual CString GetRemainingText(double lfPercent, double lfSecsRemaining);
  virtual void ResetStartTime(void);

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
