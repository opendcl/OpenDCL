#pragma once

class TProgressTimeToComplete : public CProgressCtrl
{
  private:
    double  m_lfPercent;
    clock_t m_start;
    CFont   m_fountHorz;
    CFont   m_fountVert;
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
    TProgressTimeToComplete();
    virtual ~TProgressTimeToComplete();
    void ResetStartTime(void);
	
	bool	m_bPercentage;
	bool	m_bTime;
	CString m_sText;

    DECLARE_MESSAGE_MAP()
};
