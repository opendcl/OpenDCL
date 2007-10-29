// ProgressBarCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressBarCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "PropertyIds.h"
#include "Workspace.h"
#include "SharedRes.h"
#include "MemDC.h"


/////////////////////////////////////////////////////////////////////////////
// CProgressBarCtrl

CProgressBarCtrl::CProgressBarCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mbPercentage( false )
, mbTime( false )
, mStartTime( clock() )
{
  mfontHoriz.CreatePointFont( 90, _T("MS Shell Dlg"), NULL );
	LOGFONT logFont;
  mfontHoriz.GetLogFont( &logFont );
  logFont.lfEscapement  = 900;
  logFont.lfOrientation = 900;
  mfontVert.CreateFontIndirect( &logFont );
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CProgressBarCtrl::~CProgressBarCtrl()
{
}

bool CProgressBarCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CProgressBarCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	if( mpTemplate->GetBooleanProperty( Prop::SmoothProgress ) )
		dwStyle |= PBS_SMOOTH;
	if( mpTemplate->GetLongProperty( Prop::Orientation ) == 1 )
		dwStyle |= PBS_VERTICAL;

	return dwStyle;
}

bool CProgressBarCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::DisplaySeconds:
		mbTime = pProp->GetBooleanValue();
		break;
	case Prop::DisplayPercentage:
		mbPercentage = pProp->GetBooleanValue();
		break;
	case Prop::SmoothProgress:
		{
			if( pProp->GetBooleanValue() )
				ModifyStyle( 0, PBS_SMOOTH, SWP_FRAMECHANGED );
			else
				ModifyStyle( PBS_SMOOTH, 0, SWP_FRAMECHANGED );
		}
		break;
	case Prop::Orientation:
		{
			int nPos = GetPos();
			if( pProp->GetLongValue() == 1 )
				ModifyStyle( 0, PBS_VERTICAL, SWP_FRAMECHANGED );
			else
				ModifyStyle( PBS_VERTICAL, 0, SWP_FRAMECHANGED );
			SetPos( nPos );
		}
		break;
	case Prop::Value:
		SetRange( (short)mpTemplate->GetLongProperty( Prop::MinValue ),
							(short)mpTemplate->GetLongProperty( Prop::MaxValue ) );
		SetPos( pProp->GetLongValue() );
		break;
	case Prop::MinValue:
		{
			if( !IsEnumeratingProperties() )
				bFailed = !OnApplyProperty( mpTemplate->GetPropertyObject( Prop::Value ) );
		}
		break;
	case Prop::MaxValue:
		{
			if( !IsEnumeratingProperties() )
				bFailed = !OnApplyProperty( mpTemplate->GetPropertyObject( Prop::Value ) );
		}
		break;
	}
	return !bFailed;
}

void CProgressBarCtrl::ResetStartTime(void)
{
  mStartTime = clock();
}

CString CProgressBarCtrl::GetRemainingText( double lfPercent, double lfSecsRemaining )
{
  CString sResult;
	if( mbPercentage )
	{
		int nPos = GetPos();
		int nLower, nUpper;
		GetRange( nLower, nUpper );
		if( nLower != 0 )
		{
			nUpper -= nLower;
			nPos -= nLower;
		}
		sResult.Format( _T("%d"), (int)(double(nPos * 100) / double(nUpper)) );
		sResult += _T('%');
	}
	else if( mbTime )
	{
		if( lfSecsRemaining >= 60 )
		{
			int nMinutes = (int)(lfSecsRemaining / 60.0);
			lfSecsRemaining -= (nMinutes * 60);
			CString sMinutes;
			if( nMinutes == 1 )
				sMinutes.Format( _T("%d %s"), nMinutes, (LPCTSTR)mpTemplate->GetStringProperty( Prop::MinuteText ) );
			else
				sMinutes.Format( _T("%d %s"), nMinutes, (LPCTSTR)mpTemplate->GetStringProperty( Prop::MinutesText ) );
			sResult += sMinutes;
		}
		if( lfSecsRemaining > 0 )
		{
			int nMinutes = (int)lfSecsRemaining;
			CString sSeconds;
			if( lfSecsRemaining == 1 )
				sSeconds.Format( _T("%d %s"), nMinutes, (LPCTSTR)mpTemplate->GetStringProperty( Prop::SecondText ) );
			else
				sSeconds.Format( _T("%d %s"), nMinutes, (LPCTSTR)mpTemplate->GetStringProperty( Prop::SecondsText ) );
			if( !sResult.IsEmpty() )
				sResult += _T(", ");
			sResult += sSeconds;
		}
		if( !sResult.IsEmpty() )
			sResult += theWorkspace.LoadResourceString( IDS_REMAINING );
	}

  return sResult;
}


BEGIN_MESSAGE_MAP(CProgressBarCtrl, CProgressCtrl)
	ON_WM_CTLCOLOR_REFLECT()
	ON_NOTIFY_REFLECT (NM_CUSTOMDRAW, &CProgressBarCtrl::OnNotifyCustomDraw)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProgressBarCtrl message handlers

BOOL CProgressBarCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CProgressBarCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if( !IsWindowEnabled() )
		return NULL;
	pDC->SetBkMode( TRANSPARENT );	
	pDC->SetTextColor( mAcadColorService.GetForegroundColor() );
	return mAcadColorService.GetBackgroundBrush();	
}

void CProgressBarCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CProgressBarCtrl::OnNotifyCustomDraw ( NMHDR * pNotifyStruct, LRESULT* result )
{
	LPNMCUSTOMDRAW pCustomDraw = (LPNMCUSTOMDRAW)pNotifyStruct;
	ASSERT (pCustomDraw->hdr.hwndFrom == m_hWnd);
	ASSERT (pCustomDraw->hdr.code = NM_CUSTOMDRAW);

	if( pCustomDraw->dwDrawStage == CDDS_PREPAINT )
	{
		::SetTextColor( pCustomDraw->hdc, mAcadColorService.GetForegroundColor() );
		::SetBkColor( pCustomDraw->hdc, mAcadColorService.GetBackgroundColor() );
	}
	*result = CDRF_DODEFAULT;
}

void CProgressBarCtrl::OnPaint() 
{
	if( mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ) )
	{
		Default();
		return;
	}
	CPaintDC dcPaint(this);
	CBrush   brush(::GetSysColor(COLOR_HIGHLIGHT));
	LONG     lStyle;
	int      nGapWidth = 2;
	int      nLoop, nBlocks;
	clock_t  now = clock();
	CString  strRemaining;

	int      nRange, nLower, nUpper;
	GetRange(nLower, nUpper);
	CRect    rcClient, rcBlock, rcComplete;
	GetClientRect(&rcClient);
	rcBlock = rcClient;

	CMemDC   dc(&dcPaint, rcClient);
    
  nRange      = nUpper - nLower;
  double lfPercent = (double)GetPos() / (double)nRange;
  lStyle = GetStyle();

  if ((lStyle & PBS_VERTICAL) == PBS_VERTICAL)
    rcBlock.top = rcBlock.bottom - (int)(rcBlock.Height() * lfPercent);
  else
    rcBlock.right = rcBlock.left + (int)(rcBlock.Width() * lfPercent);
  rcComplete = rcBlock;

  if ((lStyle & PBS_SMOOTH) == PBS_SMOOTH)
    nBlocks = 1;
  else
  {
    rcBlock = rcClient;
    if ((lStyle & PBS_VERTICAL) == PBS_VERTICAL)
      rcBlock.top = rcBlock.bottom - (int)((rcClient.Height()-(9*nGapWidth)) / 18.0);
    else
      rcBlock.right = rcBlock.left + (int)((rcClient.Width()-(9*nGapWidth)) / 18.0);
    nBlocks = (int)((lfPercent*18.0)+0.9);
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

  if (lfPercent > 0.02)
  {
    CFont  *pFount;
    double  lfSecsSoFar = (double)(now - mStartTime) / CLOCKS_PER_SEC;
    strRemaining = GetRemainingText(lfPercent, (lfSecsSoFar / lfPercent) * (1.0 - lfPercent));
    dc.SetBkMode(TRANSPARENT);

    if ((lStyle & PBS_VERTICAL) == PBS_VERTICAL)
    {
      CSize  szExt;
      CPoint ptText;

      // draw the vertical text centred in the control
      pFount = dc.SelectObject(&mfontVert);
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
      pFount = dc.SelectObject(&mfontHoriz);

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

BOOL CProgressBarCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
