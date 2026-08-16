// Methods_Ads.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Ads.h"
#include "ArgumentsRetrieval.h"
#include "ArxWorkspace.h"
#include "DpiAwarenessAPI.h"


ADSRESULT Ads::GetVersion()
{
	struct resbuf *pArgs = acedGetArgs();

	if (!AssertOutOfArgs(pArgs))
		return RSERR;

	DWORD dwMajor;
	DWORD dwMinor;
	DWORD dwThird;
	DWORD dwFourth;
	if (!theWorkspace.GetModuleVersionInfo(dwMajor, dwMinor, dwThird, dwFourth, _hdllInstance))
		return RSERR;
	assert(dwMinor < 10); // otherwise the algorithm fails
	if (dwMinor >= 10)
		return RSERR;
	acedRetReal(ads_real(dwMajor) + dwMinor / 10.0);

	return (RSRSLT);
}

ADSRESULT Ads::GetVersionEx()
{
	struct resbuf *pArgs = acedGetArgs();

	if (!AssertOutOfArgs(pArgs))
		return RSERR;

	DWORD dwMajor;
	DWORD dwMinor;
	DWORD dwThird;
	DWORD dwFourth;
	if (!theWorkspace.GetModuleVersionInfo(dwMajor, dwMinor, dwThird, dwFourth, _hdllInstance))
		return RSERR;
	CString sVersion;
	sVersion.Format(_T("%d.%d.%d.%d"), dwMajor, dwMinor, dwThird, dwFourth);
	acedRetStr(sVersion);

	return (RSRSLT);
}

ADSRESULT Ads::GetScreenSize()
{
	struct resbuf *pArgs = acedGetArgs();

	if (!AssertOutOfArgs(pArgs))
		return RSERR;

	CRect rcWorkArea;
	if (SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0) > 0)
	{
		int width = rcWorkArea.Width();
		int height = rcWorkArea.Height();
		static const POINT ptZero = { 0, 0 };
		HMONITOR primaryMonitor = DpiAwarenessHelper::MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
		if (primaryMonitor)
		{
			UINT dpiX = 96, dpiY = 96;
			DpiAwarenessHelper::GetDpiForMonitor(primaryMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
			width = MulDiv(width, 96, dpiX);
			height = MulDiv(height, 96, dpiY);
		}
		resbuf rbHeight = { NULL, RTSHORT };
		rbHeight.resval.rint = height;
		resbuf rbWidth = { &rbHeight, RTSHORT };
		rbWidth.resval.rint = width;
		acedRetList(&rbWidth);
	}

	return (RSRSLT);
}

static ADSRESULT TwipsPixels(bool toPixels, bool axisX)
{
	struct resbuf *pArgs = acedGetArgs();

	long value = 0;
	if (!GetLongArgument(pArgs, value))
		return RSERR;

	if (!AssertOutOfArgs(pArgs))
		return RSERR;

	HDC hdc = ::GetDC(NULL);
	int dpi = GetDeviceCaps(hdc, axisX ? LOGPIXELSX : LOGPIXELSY);
	::ReleaseDC(NULL, hdc);
	if (toPixels)
		theArxWorkspace.RetLong(MulDiv(dpi, value, 1440));
	else
		theArxWorkspace.RetLong(MulDiv(value, 1440, dpi));

	return (RSRSLT);
}

ADSRESULT Ads::XTwipsToPixels() { return TwipsPixels(true, true); }
ADSRESULT Ads::YTwipsToPixels() { return TwipsPixels(true, false); }
ADSRESULT Ads::XPixelsToTwips() { return TwipsPixels(false, true); }
ADSRESULT Ads::YPixelsToTwips() { return TwipsPixels(false, false); }

ADSRESULT Ads::HideErrorMsgBox()
{
	struct resbuf *pArgs = acedGetArgs();

	if (!AssertOutOfArgs(pArgs))
		return RSERR;

	theWorkspace.SetMessagesSuppressed();
	acedRetT();

	return (RSRSLT);
}

ADSRESULT Ads::ShowErrorMsgBox()
{
	struct resbuf *pArgs = acedGetArgs();

	if (!AssertOutOfArgs(pArgs))
		return RSERR;

	theWorkspace.SetMessagesSuppressed(false);
	acedRetT();

	return (RSRSLT);
}

ADSRESULT Ads::SuppressMessages()
{
	struct resbuf *pArgs = acedGetArgs();

	bool bSuppress = true;
	GetBoolArgument(pArgs, bSuppress, true);

	if (!AssertOutOfArgs(pArgs))
		return RSERR;

	theWorkspace.SetMessagesSuppressed(bSuppress);
	acedRetT();

	return (RSRSLT);
}
