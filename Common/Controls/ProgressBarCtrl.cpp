// ProgressBarCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressBarCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "PropertyIds.h"
#include "Workspace.h"
#include "Resource.h"
#include "MemDC.h"


static CString GetDisplayText( TDclControlPtr pControl, Prop::Id id, UINT idRes )
{
	CString sText = pControl->GetStringProperty( id );
	if( sText.IsEmpty() )
		sText = theWorkspace.LoadResourceString( idRes );
	return sText;
}

/////////////////////////////////////////////////////////////////////////////
// CProgressBarCtrl

CProgressBarCtrl::CProgressBarCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, meLegend( lgNone )
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
	case Prop::ProgressLegend:
		meLegend = (Legend)pProp->GetLongValue();
		break;
	case Prop::SmoothProgress:
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, PBS_SMOOTH, SWP_FRAMECHANGED );
		else
			ModifyStyle( PBS_SMOOTH, 0, SWP_FRAMECHANGED );
		OnFrameChanged();
		break;
	case Prop::Orientation:
		if( pProp->GetLongValue() == 1 )
			ModifyStyle( 0, PBS_VERTICAL, SWP_FRAMECHANGED );
		else
			ModifyStyle( PBS_VERTICAL, 0, SWP_FRAMECHANGED );
		OnFrameChanged();
		break;
	case Prop::Value:
		if( IsEnumeratingProperties() )
		{
			SetRange32( mpTemplate->GetLongProperty( Prop::MinValue ),
									mpTemplate->GetLongProperty( Prop::MaxValue ) );
		}
		SetPos( pProp->GetLongValue() );
		break;
	case Prop::MinValue:
		if( !IsEnumeratingProperties() )
		{
			SetRange32( pProp->GetLongValue(),
									mpTemplate->GetLongProperty( Prop::MaxValue ) );
			bFailed = !OnApplyProperty( mpTemplate->GetPropertyObject( Prop::Value ) );
		}
		break;
	case Prop::MaxValue:
		if( !IsEnumeratingProperties() )
		{
			SetRange32( mpTemplate->GetLongProperty( Prop::MinValue ),
									pProp->GetLongValue() );
			bFailed = !OnApplyProperty( mpTemplate->GetPropertyObject( Prop::Value ) );
		}
		break;
	}
	return !bFailed;
}

void CProgressBarCtrl::ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst,
																						 std::vector< Prop::Id >& ridLast )
{
	ridLast.push_back( Prop::Value );
}

void CProgressBarCtrl::Reset(void)
{
  ResetStartTime();
	long lMinValue = mpTemplate->GetLongProperty( Prop::MinValue );
	mpTemplate->SetLongProperty( Prop::Value, lMinValue );
	SetPos( lMinValue );
}

void CProgressBarCtrl::ResetStartTime(void)
{
  mStartTime = clock();
}

CString CProgressBarCtrl::GetRemainingText( double lfPercent, double lfSecsRemaining )
{
  CString sResult;
	switch( meLegend )
	{
	case lgPercent:
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
		break;
	case lgTime:
		{
			if( lfSecsRemaining >= 60 )
			{
				int nMinutes = (int)(lfSecsRemaining / 60.0);
				lfSecsRemaining -= (nMinutes * 60);
				CString sMinutes;
				if( nMinutes == 1 )
					sMinutes.Format( _T("%d %s"), nMinutes, (LPCTSTR)GetDisplayText( mpTemplate, Prop::CaptionMinute, IDS_MINUTETEXT ) );
				else
					sMinutes.Format( _T("%d %s"), nMinutes, (LPCTSTR)GetDisplayText( mpTemplate, Prop::CaptionMinutes, IDS_MINUTESTEXT ) );
				sResult += sMinutes;
			}
			if( lfSecsRemaining > 0 )
			{
				int nMinutes = (int)lfSecsRemaining;
				CString sSeconds;
				if( lfSecsRemaining == 1 )
					sSeconds.Format( _T("%d %s"), nMinutes, (LPCTSTR)GetDisplayText( mpTemplate, Prop::CaptionSecond, IDS_SECONDTEXT ) );
				else
					sSeconds.Format( _T("%d %s"), nMinutes, (LPCTSTR)GetDisplayText( mpTemplate, Prop::CaptionSeconds, IDS_SECONDSTEXT ) );
				if( !sResult.IsEmpty() )
					sResult += _T(", ");
				sResult += sSeconds;
			}
			if( !sResult.IsEmpty() )
			{
				sResult += _T(' ');
				sResult += GetDisplayText( mpTemplate, Prop::CaptionRemaining, IDS_REMAININGTEXT );
			}
		}
		break;
	}

  return sResult;
}


BEGIN_MESSAGE_MAP(CProgressBarCtrl, CProgressCtrl)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_PAINT()
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
	return NULL;
	//return mColorService.CtlColor( pDC, nCtlColor );
}

void CProgressBarCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CProgressBarCtrl::OnPaint() 
{
	if( mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ) )
	{
		__super::OnPaint();
		return;
	}
	CPaintDC dcPaint( this );
	CBrush brush( ::GetSysColor( COLOR_HIGHLIGHT ) );

	int nLower, nUpper;
	GetRange( nLower, nUpper );
	CRect rcClient, rcBlock, rcComplete;
	GetClientRect( &rcClient );
	rcBlock = rcClient;

	CMemDC dc( &dcPaint, rcClient );
    
  int nRange = nUpper - nLower;
  double lfPercent = (double)(GetPos() - nLower) / (double)(nRange - nLower);
  LONG lStyle = GetStyle();

  if ((lStyle & PBS_VERTICAL) == PBS_VERTICAL)
    rcBlock.top = rcBlock.bottom - (int)(rcBlock.Height() * lfPercent);
  else
    rcBlock.right = rcBlock.left + (int)(rcBlock.Width() * lfPercent);
  rcComplete = rcBlock;

	static const int nGapWidth = 2;
	int nBlocks = 1;
  if( (lStyle & PBS_SMOOTH) == 0 )
  {
    rcBlock = rcClient;
    if ((lStyle & PBS_VERTICAL) == PBS_VERTICAL)
      rcBlock.top = rcBlock.bottom - (int)((rcClient.Height() - ( 9 * nGapWidth)) / 18.0);
    else
      rcBlock.right = rcBlock.left + (int)((rcClient.Width() - ( 9 * nGapWidth)) / 18.0);
    nBlocks = (int)((lfPercent*18.0)+0.9);
  }

  if (GetParent() != NULL)
    ::FillRect(dc, &rcClient, (HBRUSH)GetParent()->SendMessage(WM_CTLCOLORSTATIC, (WPARAM)(HDC)dc, (LPARAM)this->GetSafeHwnd()));

  for( int nLoop = 1; nLoop <= nBlocks; nLoop++ )
  {
    ::FillRect(dc, &rcBlock, brush);
    if ((lStyle & PBS_VERTICAL) == PBS_VERTICAL)
    {
      rcComplete.top = rcBlock.top;
      rcBlock.OffsetRect( 0, - (rcBlock.Height() + nGapWidth) );
    }
    else
    {
      rcComplete.right = rcBlock.right;
      rcBlock.OffsetRect( rcBlock.Width() + nGapWidth, 0 );
    }
  }

  if (lfPercent > 0.02)
  {
    double  lfSecsSoFar = (double)(clock() - mStartTime) / CLOCKS_PER_SEC;
    CString strRemaining = GetRemainingText(lfPercent, (lfSecsSoFar / lfPercent) * (1.0 - lfPercent));
    dc.SetBkMode(TRANSPARENT);

    if ((lStyle & PBS_VERTICAL) == PBS_VERTICAL)
    {
      dc.SaveDC();
      CSize  szExt;
      CPoint ptText;

      // draw the vertical text centred in the control
      dc.SelectObject(&mfontVert);
      szExt  = dc.GetTextExtent(strRemaining);
      ptText.x = (rcClient.Width() - szExt.cy) >> 1;
      ptText.y = rcClient.bottom - ((rcClient.Height() - szExt.cx) >> 1);

      dc.SaveDC();
      dc.IntersectClipRect(rcComplete);
      dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
      dc.ExtTextOut(ptText.x, ptText.y, ETO_CLIPPED, rcClient, strRemaining, NULL);
      dc.RestoreDC(-1);

      dc.ExcludeClipRect(rcComplete);
      dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
      dc.ExtTextOut(ptText.x, ptText.y, ETO_CLIPPED, rcClient, strRemaining, NULL);
      dc.RestoreDC(-1);
    }
    else
    {
      dc.SaveDC();

      // horizontal text is easier
      dc.SelectObject(&mfontHoriz);
      dc.IntersectClipRect(rcComplete);
      dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
      dc.DrawText(strRemaining, rcClient, DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
      dc.RestoreDC(-1);

      dc.SaveDC();
      dc.ExcludeClipRect(rcComplete);
      dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
      dc.DrawText(strRemaining, rcClient, DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
      dc.RestoreDC(-1);
    }
  }
}

LRESULT CProgressBarCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_STYLECHANGED )
	{ //for some reason the position gets reset when handling WM_STYLECHANGED, so force it back
		int nOldPos = GetPos();
		LRESULT lResult = __super::WindowProc(message, wParam, lParam);
		SetPos( nOldPos );
		return lResult;
	}
	return __super::WindowProc(message, wParam, lParam);
}
