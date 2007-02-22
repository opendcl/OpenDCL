// SnapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SnapDlg.h"
#include "PropertyIds.h"
#include "DclControlObject.h"


const int nDefaultSize = -1;
const int iHeightAdjustment = 27;
const int iWidthAdjustment = 8;

const TCHAR sUnderScore[] = _T("_");
const TCHAR sSizeWidth[] = _T("sizeWidth");
const TCHAR sSizeHeight[] = _T("sizeHeight");
const TCHAR sTopLeftX[] = _T("nTopLeftX");
const TCHAR sTopLeftY[] = _T("nTopLeftY");


/////////////////////////////////////////////////////////////////////////////
// CSnapDlg dialog

CSnapDlg::CSnapDlg(CDclFormObject* pSourceForm, UINT idd, CWnd* pParent /*=NULL*/)
: CDialog(idd, pParent)
, mpSourceForm( pSourceForm )
{
	//{{AFX_DATA_INIT(CSnapDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sizing = FALSE;
	m_hIconAcad = NULL;
}

CSnapDlg::~CSnapDlg()
{
}

void CSnapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSnapDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSnapDlg, CDialog)
	//{{AFX_MSG_MAP(CSnapDlg)
	ON_WM_SIZING()
	ON_WM_CAPTURECHANGED()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSnapDlg message handlers

BOOL CSnapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_sizing = FALSE;
	m_rcGripRect.left = 0;
	m_rcGripRect.top = 0;
	m_rcGripRect.bottom = 10;
	m_rcGripRect.right = 10;
	
	m_szGripSize.cx = GetSystemMetrics(SM_CXVSCROLL);
	m_szGripSize.cy = GetSystemMetrics(SM_CYHSCROLL);
	
	UpdateGripPos();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSnapDlg::OnSizing(UINT fwSide, LPRECT pRect) 
{
	int nNewWidth = pRect->right - pRect->left;
	int nNewHeight = pRect->bottom - pRect->top;
	
	// ensure the user does not size below the min allowable size
	if (nNewWidth < m_nMinWidth)
		pRect->right = pRect->left + m_nMinWidth;

	if (nNewHeight < m_nMinHeight)
		pRect->bottom = pRect->top + m_nMinHeight;

	// ensure the user does not size above the max allowable size
	if (nNewWidth > m_nMaxWidth)
		pRect->right = pRect->left + m_nMaxWidth;

	if (nNewHeight > m_nMaxHeight)
		pRect->bottom = pRect->top + m_nMaxHeight;

	CDialog::OnSizing(fwSide, pRect);
	
	m_sizing = TRUE;
}

void CSnapDlg::OnCaptureChanged(CWnd *pWnd) 
{
	if (m_sizing)
	{
		m_sizing = FALSE;
		//SizeDialog();
	}
	
	CDialog::OnCaptureChanged(pWnd);
}

//The dialog extent values are too small.
//I think they do the inside of the dialog and ignore the title bar and
//edges.
//These values might fail if display settings are different than on my computer,
//but at least it is closer to correct.
void CSnapDlg::SetDialogMinExtents(int nWidth, int nHeight)
{
	m_nMinWidth = nWidth + iWidthAdjustment;
	m_nMinHeight = nHeight + iHeightAdjustment;
}
	
void CSnapDlg::SetDialogMaxExtents(int nWidth, int nHeight)
{
	m_nMaxWidth = nWidth + iWidthAdjustment;
	m_nMaxHeight = nHeight + iHeightAdjustment;
}


void CSnapDlg::SaveSize()
{
	CWinApp* pApp = AfxGetApp();
	CRect rcThis;
	GetWindowRect(&rcThis);
	CString sProfileName = mpSourceForm->GetKeyPath(); 

	if (IsWindow(m_hWnd))
	{
		pApp->WriteProfileInt(sProfileName, sSizeWidth, rcThis.Width());
		pApp->WriteProfileInt(sProfileName, sSizeHeight, rcThis.Height());
		
		pApp->WriteProfileInt(sProfileName, sTopLeftX, rcThis.left);
		pApp->WriteProfileInt(sProfileName, sTopLeftY, rcThis.top);
	}
    
}

CRect CSnapDlg::ReadRect()
{	
	
	CRect rcRet;
	CWinApp* pApp = AfxGetApp();
	CString sProfileName = mpSourceForm->GetKeyPath();
    
    rcRet.left = pApp->GetProfileInt(sProfileName, sTopLeftX, nDefaultSize);
    rcRet.top = pApp->GetProfileInt(sProfileName, sTopLeftY, nDefaultSize);
	
	int nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	// ensure the dialog box is on screen
	if (rcRet.left+10 > nScreenWidth)
		rcRet.left = nDefaultSize;
	if (rcRet.left < nDefaultSize)
		rcRet.left = nDefaultSize;

	if (rcRet.top+10 > nScreenHeight)
		rcRet.top = nDefaultSize;
	if (rcRet.top < nDefaultSize)
		rcRet.top = nDefaultSize;


	rcRet.right = rcRet.left + pApp->GetProfileInt(sProfileName, sSizeWidth, nDefaultSize);
	rcRet.bottom = rcRet.top + pApp->GetProfileInt(sProfileName, sSizeHeight, nDefaultSize);

	// ensure the dialog box is at least as big as the min size
	int nMinWidth = mpSourceForm->GetControlProperties()->GetLngProperty(nMinDialogWidth);
	int nMinHeight = mpSourceForm->GetControlProperties()->GetLngProperty(nMinDialogHeight);		
	int nMaxWidth = mpSourceForm->GetControlProperties()->GetLngProperty(nMaxDialogWidth);
	int nMaxHeight = mpSourceForm->GetControlProperties()->GetLngProperty(nMaxDialogHeight);		
	
	if (rcRet.Width() < nMinWidth || rcRet.Width() > nMaxWidth)
		rcRet.right = rcRet.left + mpSourceForm->GetControlProperties()->GetLngProperty(nWidth);

	if (rcRet.Height() < nMinHeight || rcRet.Height() > nMaxHeight)
		rcRet.bottom = rcRet.top + mpSourceForm->GetControlProperties()->GetLngProperty(nHeight);

	if (rcRet.Width() > nMaxWidth)
		rcRet.right = rcRet.left + nMaxWidth;

	if (rcRet.Height() > nMaxHeight)
		rcRet.bottom = rcRet.top + nMaxHeight;

	
	return rcRet;
}

CSize CSnapDlg::ReadSize()
{	
	CSize szRet;
	CWinApp* pApp = AfxGetApp();
	CString sProfileName = mpSourceForm->GetKeyPath(); 
    
    szRet.cx = pApp->GetProfileInt(sProfileName, sSizeWidth, nDefaultSize);
	szRet.cy = pApp->GetProfileInt(sProfileName, sSizeHeight, nDefaultSize);

	// ensure the dialog box is at least as big as the min size
	int nMinWidth = mpSourceForm->GetControlProperties()->GetLngProperty(nMinDialogWidth);
	int nMinHeight = mpSourceForm->GetControlProperties()->GetLngProperty(nMinDialogHeight);
	int nMaxWidth = mpSourceForm->GetControlProperties()->GetLngProperty(nMaxDialogWidth);
	int nMaxHeight = mpSourceForm->GetControlProperties()->GetLngProperty(nMaxDialogHeight);		
	
	if (szRet.cx < nMinWidth || szRet.cx > nMaxWidth)
		szRet.cx = mpSourceForm->GetControlProperties()->GetLngProperty(nWidth);

	if (szRet.cy < nMinHeight || szRet.cy > nMaxHeight)
		szRet.cy = mpSourceForm->GetControlProperties()->GetLngProperty(nHeight);
	
	if (szRet.cx > nMaxWidth)
		szRet.cx = nMaxWidth;

	if (szRet.cy > nMaxHeight)
		szRet.cy = nMaxHeight;
	
	return szRet;
}

void CSnapDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);	
	UpdateGripPos();
}



void CSnapDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (m_bShowGrip)
	{
		UpdateGripPos();
		// draw size-grip
		dc.DrawFrameControl(&m_rcGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
	}	
}


void CSnapDlg::UpdateGripPos()
{
	// size-grip goes bottom right in the client area

	if (!m_bShowGrip)
		return;
	
	//InvalidateRect(&m_rcGripRect);
	HDC hdc = ::GetDC(m_hWnd);
	
	// draw the solid rectangle
	::SetBkColor(hdc, ::GetSysColor(COLOR_BTNFACE));
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &m_rcGripRect, NULL, 0, NULL);
	
	
	GetClientRect(&m_rcGripRect);

	m_rcGripRect.left = m_rcGripRect.right - m_szGripSize.cx;
	m_rcGripRect.top = m_rcGripRect.bottom - m_szGripSize.cy;

	// draw size-grip
	DrawFrameControl(hdc, &m_rcGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
	
}

#if (_ACADTARGET == 16)
UINT CSnapDlg::OnNcHitTest(CPoint point) 
#else
LRESULT CSnapDlg::OnNcHitTest(CPoint point) 
#endif
{
	CPoint pt = point;
	ScreenToClient(&pt);

	// if in size grip and in client area
	if (m_bShowGrip && m_rcGripRect.PtInRect(pt) &&
		pt.x >= 0 && pt.y >= 0)
		return HTBOTTOMRIGHT;
	
	return CDialog::OnNcHitTest(point);
}

void CSnapDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}
