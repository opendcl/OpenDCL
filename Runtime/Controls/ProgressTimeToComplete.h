#pragma once

#include "PtrTypes.h"


class TProgressTimeToComplete : public CProgressCtrl
{
  private:
    double  m_lfPercent;
    clock_t m_start;
    CFont   m_fountHorz;
    CFont   m_fountVert;
	TDclControlPtr m_ArxControl;
  protected:
    //{{AFX_VIRTUAL(TProgressTimeToComplete)
    //}}AFX_VIRTUAL
    //{{AFX_MSG(TProgressTimeToComplete)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

  protected:
    virtual CString GetRemainingText(double lfPercent, double lfSecsRemaining);
	
  public:

	bool	m_bPercentage;
	bool	m_bTime;

    TProgressTimeToComplete();
    virtual ~TProgressTimeToComplete();
    void ResetStartTime(void);
	BOOL Create(TDclControlPtr pControl, CWnd* pParentWnd, UINT nID );
    DECLARE_MESSAGE_MAP()
};
