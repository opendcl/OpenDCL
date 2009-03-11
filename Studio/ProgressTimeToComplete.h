#pragma once

class TProgressTimeToComplete : public CProgressCtrl
{
  private:
    double  m_lfPercent;
    clock_t m_start;
    CFont   m_fountHorz;
    CFont   m_fountVert;
  protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

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
