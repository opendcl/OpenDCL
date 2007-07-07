#include "stdafx.h"
#include "ProgressTimeToComplete.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "MemDC.h"
//#include "tcharHandling.h"


BEGIN_MESSAGE_MAP(TProgressTimeToComplete, CProgressCtrl)
    //{{AFX_MSG_MAP(TProgressTimeToComplete)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

TProgressTimeToComplete::TProgressTimeToComplete()
{
    LOGFONT logFount;

	m_bPercentage = false;
	m_bTime = false;
    m_lfPercent = 0.0;
    m_start     = clock();

    // create a horizontal fount
    m_fountHorz.CreatePointFont(90, _T("MS Sans"), NULL);

    // create a vertical fount the same as the horizontal one
    m_fountHorz.GetLogFont(&logFount);
    logFount.lfEscapement  = 900;
    logFount.lfOrientation = 900;
    m_fountVert.CreateFontIndirect(&logFount);
}

TProgressTimeToComplete::~TProgressTimeToComplete()
{
}

void TProgressTimeToComplete::ResetStartTime(void)
{
    m_start = clock();
}


BOOL TProgressTimeToComplete::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	DWORD dwStyle = WS_CHILD|WS_VISIBLE | WS_CLIPSIBLINGS;

	if (pControl->GetBoolProperty(nSmoothProgress) == TRUE)
		dwStyle = dwStyle | PBS_SMOOTH;

	if (pControl->GetLongProperty(nOrientation) == 1)
		dwStyle = dwStyle | PBS_VERTICAL ;

	RetVal = CProgressCtrl::Create(dwStyle, ArxRect, pParentWnd, nID);

	m_bPercentage = m_ArxControl->GetBoolProperty(nDisplayPercentage) == TRUE;
	m_bTime = m_ArxControl->GetBoolProperty(nDisplaySeconds) == TRUE;

	// Set the range to be 0 to 100.
	//CProgressCtrl::SetRange( 0, 100 );

	//CProgressCtrl::SetPos (50);
	

	return RetVal;
}

void TProgressTimeToComplete::OnPaint() 
{
    int      nRange, nLower, nUpper;
    CBrush   brush(::GetSysColor(COLOR_HIGHLIGHT));
    CPaintDC dcPaint(this);
    CRect    rcClient, rcBlock, rcComplete;
    LONG     lStyle;
    int      nGapWidth = 2;
    int      nLoop, nBlocks;
    clock_t  now = clock();
    CString  strRemaining;

    this->GetRange(nLower, nUpper);
    this->GetClientRect(&rcClient);
    rcBlock = rcClient;

	CMemDC   dc(&dcPaint, rcClient);
    
    nRange      = nUpper - nLower;
    m_lfPercent = (double)this->GetPos() / (double)nRange;
    lStyle      = ::GetWindowLong(*this, GWL_STYLE);

    if ((lStyle & PBS_VERTICAL) == PBS_VERTICAL)
        rcBlock.top = rcBlock.bottom - (int)(rcBlock.Height() * m_lfPercent);
    else
        rcBlock.right = rcBlock.left + (int)(rcBlock.Width() * m_lfPercent);
    rcComplete = rcBlock;

    if ((lStyle & PBS_SMOOTH) == PBS_SMOOTH)
        nBlocks = 1;
    else
    {
        #define BLOCKS 18.0
        rcBlock = rcClient;
        if ((lStyle & PBS_VERTICAL) == PBS_VERTICAL)
            rcBlock.top = rcBlock.bottom - (int)((rcClient.Height()-(9*nGapWidth)) / BLOCKS);
        else
            rcBlock.right = rcBlock.left + (int)((rcClient.Width()-(9*nGapWidth)) / BLOCKS);
        nBlocks = (int)((m_lfPercent*BLOCKS)+0.9);
    }

    if (this->GetParent() != NULL)
        ::FillRect(dc, &rcClient, (HBRUSH)this->GetParent()->SendMessage(WM_CTLCOLORSTATIC, (WPARAM)(HDC)dc, (LPARAM)this->GetSafeHwnd()));

    for (nLoop=1; nLoop<=nBlocks; nLoop++)
    {
        ::FillRect(dc, &rcBlock, brush);
        if ((lStyle & PBS_VERTICAL) == PBS_VERTICAL)
        {
            rcComplete.top = rcBlock.top;
            rcBlock.OffsetRect(0, -(rcBlock.Height()+nGapWidth));
        }
        else
        {
            rcComplete.right = rcBlock.right;
            rcBlock.OffsetRect(rcBlock.Width()+nGapWidth, 0);
        }
    }

    if (m_lfPercent > 0.02)
    {
        CFont  *pFount;
        double  lfSecsSoFar = (double)(now - m_start) / CLOCKS_PER_SEC;
        strRemaining = this->GetRemainingText(m_lfPercent, (lfSecsSoFar / m_lfPercent) * (1.0 - m_lfPercent));
        dc.SetBkMode(TRANSPARENT);

        if ((lStyle & PBS_VERTICAL) == PBS_VERTICAL)
        {
            CSize  szExt;
            CPoint ptText;

            // draw the vertical text centred in the control
            pFount = dc.SelectObject(&m_fountVert);
            szExt  = dc.GetTextExtent(strRemaining);
            ptText.x = (rcClient.Width() - szExt.cy) >> 1;
            ptText.y = rcClient.bottom - ((rcClient.Height() - szExt.cx) >> 1);

            dc.SaveDC();
            dc.IntersectClipRect(rcComplete);
            dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
            dc.ExtTextOut(ptText.x, ptText.y, ETO_CLIPPED, rcClient, strRemaining, NULL);
            dc.RestoreDC(-1);

            dc.SaveDC();
            dc.ExcludeClipRect(rcComplete);
            dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
            dc.ExtTextOut(ptText.x, ptText.y, ETO_CLIPPED, rcClient, strRemaining, NULL);
            dc.RestoreDC(-1);

            dc.SelectObject(pFount);
        }
        else
        {
            // horizontal text is easier
            pFount = dc.SelectObject(&m_fountHorz);

            dc.SaveDC();
            dc.IntersectClipRect(rcComplete);
            dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
            dc.DrawText(strRemaining, rcClient, DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
            dc.RestoreDC(-1);

            dc.SaveDC();
            dc.ExcludeClipRect(rcComplete);
            dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
            dc.DrawText(strRemaining, rcClient, DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
            dc.RestoreDC(-1);

            dc.SelectObject(pFount);
        }
    }
}

BOOL TProgressTimeToComplete::OnEraseBkgnd(CDC* pDC) 
{
    return TRUE;
}

const TCHAR sPercentage[] = _T("%");


CString TProgressTimeToComplete::GetRemainingText(double lfPercent, double lfSecsRemaining)
{
    CString str;

	if (m_bPercentage)
	{
		int nLower, nUpper;
		int nPos = GetPos();

		GetRange(nLower, nUpper);

		if (nLower > 0)
		{
			nUpper -= nLower;
			nPos -= nLower;
		}

		double dPercentage = double(nPos) / double(nUpper);
		dPercentage *= 100;

		str.Format(_T("%d"),(long)dPercentage);

		str += sPercentage;

		return str;
	}
	else if (m_bTime)
	{
		int nSeconds = (int)fmod(lfSecsRemaining, 60.0);

		if (lfSecsRemaining < 60)
		{
			if (nSeconds < 1)
			{	
				str.Format(CString("%d ") + 
						   m_ArxControl->GetStrProperty(nSecondText), 
						   nSeconds);
			}
			else
			{
				str.Format(CString("%d ") + 
						   m_ArxControl->GetStrProperty(nSecondsText), 
						   nSeconds);
			}
		}
		else
		{
			int nMinutes = (int)(lfSecsRemaining/60.0);
			str.Format(_T("%d minute%s, %d second%s remaining"), nMinutes, nMinutes==1? _T(""):_T("s"),
															 nSeconds, nSeconds==1? _T(""):_T("s"));
		}
	}
/*
	m_Minute.SetWindowText(m_pArxCtrl->GetStrProperty(nMinuteText));
	m_Minutes.SetWindowText(m_pArxCtrl->GetStrProperty(nMinutesText));

	m_Second.SetWindowText(m_pArxCtrl->GetStrProperty(nSecondText));
	m_Seconds.SetWindowText(m_pArxCtrl->GetStrProperty(nSecondsText));
*/	

    return str;
}
