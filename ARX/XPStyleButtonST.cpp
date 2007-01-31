#include "stdafx.h"
#include "XPStyleButtonST.h"
#include "ThemeHelperST.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "AcadColorTable.h"
#include "PropertyIds.h"
#include "ToolTips.h"


#ifdef	THEMEAPI
#include <tmschema.h>
#else
#define	NO_THEMEAPI_FOUND

#define	BP_PUSHBUTTON	0x00000001
#define	PBS_NORMAL		0x00000001
#define	PBS_HOT			0x00000002
#define	PBS_PRESSED		0x00000003
#define	PBS_DISABLED	0x00000004
#define	PBS_DEFAULTED	0x00000005
#endif


CXPStyleButtonST::CXPStyleButtonST()
{
	// No theme helper
	m_pTheme = NULL;
	// Don't use flat toolbar-style
	m_bDrawAsToolbar = FALSE;
	DrawBorder(FALSE);
}

CXPStyleButtonST::~CXPStyleButtonST()
{
}

// This function is called every time the button border needs to be painted.
// If the button is in standard (not flat) mode this function will NOT be called.
// This is a virtual function that can be rewritten in CButtonST-derived classes
// to produce a whole range of buttons not available by default.
//
// Parameters:
//		[IN]	pDC
//				Pointer to a CDC object that indicates the device context.
//		[IN]	pRect
//				Pointer to a CRect object that indicates the bounds of the
//				area to be painted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CXPStyleButtonST::OnDrawBorder(CDC* pDC, CRect* pRect)
{
	if (m_pTheme == NULL || m_pTheme->IsAppThemed() == FALSE)
	{
		return CButtonST::OnDrawBorder(pDC, pRect);
	} // if

	return BTNST_OK;
} // End of OnDrawBorder

// This function is called every time the button background needs to be painted.
// If the button is in transparent mode this function will NOT be called.
// This is a virtual function that can be rewritten in CButtonST-derived classes
// to produce a whole range of buttons not available by default.
//
// Parameters:
//		[IN]	pDC
//				Pointer to a CDC object that indicates the device context.
//		[IN]	pRect
//				Pointer to a CRect object that indicates the bounds of the
//				area to be painted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CXPStyleButtonST::OnDrawBackground(CDC* pDC, CRect* pRect)
{
	BOOL	bDefaultDraw = FALSE;
	// No theme helper passed
	if (m_pTheme == NULL || m_pTheme->IsAppThemed() == FALSE)
	{
		bDefaultDraw = TRUE;
	} // if
	else
	{
		HTHEME	hTheme = NULL;
		int		iStateId = 0;

		hTheme = m_pTheme->OpenThemeData(GetSafeHwnd(), m_bDrawAsToolbar ? L"TOOLBAR" : L"BUTTON");
		if (hTheme)
		{
			iStateId = PBS_NORMAL;								// Normal
			if ((m_bIsDefault && !m_bDrawAsToolbar) ||
				(m_bIsDefault && m_bDrawAsToolbar && m_bDrawBorder))		iStateId = PBS_DEFAULTED;	// Default button
			if (m_bMouseOnButton)	iStateId = PBS_HOT;			// Hot
			if (m_bIsPressed)		iStateId = PBS_PRESSED;		// Pressed
			if (IsWindowEnabled() == FALSE)		iStateId = PBS_DISABLED;	// Disabled

			if (m_ArxControl == NULL && m_bIsDefault)
				m_bIsDefault = FALSE;
			
			if (m_ArxControl == NULL)
				iStateId = PBS_NORMAL;
				
			m_pTheme->DrawThemeBackground(hTheme, m_hWnd, pDC->GetSafeHdc(), BP_PUSHBUTTON, iStateId, pRect, NULL);
			m_pTheme->CloseThemeData(hTheme);
		} // if
		else
		{
			bDefaultDraw = TRUE;
		} // else
	} // else

	if (bDefaultDraw)
	{
		return CButtonST::OnDrawBackground(pDC, pRect);
	} // if

	return BTNST_OK;
} // End of OnDrawBackground

// This function assigns a CThemeHelperST instance to the button.
//
// Parameters:
//		[IN]	pTheme
//				Pointer to a CThemeHelperST instance.
//				Pass NULL to remove any previous instance.
//
void CXPStyleButtonST::SetThemeHelper(CThemeHelperST* pTheme)
{
	m_pTheme = pTheme;

	// No theme active ?
	if (m_pTheme == NULL || m_pTheme->IsAppThemed() == FALSE)
	{
		m_bDrawBorder = TRUE;
	} // if
} // End of SetThemeHelper

// This function sets if the button must be drawn using the flat toolbar-style.
//
// Parameters:
//		[IN]	bDrawAsToolbar
//				If TRUE the button will be drawn using the flat toolbar-style.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CXPStyleButtonST::DrawAsToolbar(BOOL bDrawAsToolbar, BOOL bRepaint)
{
	m_bDrawAsToolbar = bDrawAsToolbar;
	// Repaint the button
	if (bRepaint) Invalidate();

	return BTNST_OK;
} // End of DrawAsToolbar


BOOL CXPStyleButtonST::Create(CDclControlObject* pControl, CProject *pProject, CWnd* pParentWnd, UINT nID) 
{
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|BS_OWNERDRAW;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	m_pProject	 = pProject;

	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;

	
	
	m_ForeColor = GetRGBColor(pControl->GetLngProperty(nForeColor));
	m_BackColor = GetRGBColor(pControl->GetLngProperty(nAcadColor));

	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP | WS_GROUP;
	else
		dwStyle = dwStyle;

	BOOL RetVal = CButtonST::Create(CString(), dwStyle, ArxRect, pParentWnd, nID);
	VERIFY(CButtonST::SubclassDlgItem(nID, pParentWnd));

	InitToolTip();
	SetToolTipEx(this, m_ToolTip, pControl);

	
	if (m_ArxControl)
	{
		switch (m_ArxControl->GetLngProperty(nEventInvoke))
		{
		case 1:
			m_bInvokeWithSendString = true;
			break;
		default:
			m_bInvokeWithSendString = false;
			break;
		}
	}
	m_bIsCheckBox = FALSE;
	
	return RetVal;
}

BOOL CXPStyleButtonST::Create(CWnd* pParentWnd, CRect rc, UINT nID) 
{
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|BS_OWNERDRAW;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = NULL;
	m_pProject	 = NULL;

	m_ForeColor = GetSysColor(COLOR_BTNTEXT);
	m_BackColor = GetSysColor(COLOR_BTNFACE);

	BOOL RetVal = CButtonST::Create(CString(), dwStyle, rc, pParentWnd, nID);
	VERIFY(CButtonST::SubclassDlgItem(nID, pParentWnd));
	
	m_bInvokeWithSendString = false;
	m_bIsCheckBox = FALSE;
	
	return RetVal;
}

#ifdef	NO_THEMEAPI_FOUND
#undef	NO_THEMEAPI_FOUND
#undef	BP_PUSHBUTTON
#undef	PBS_NORMAL
#undef	PBS_HOT
#undef	PBS_PRESSED
#undef	PBS_DISABLED
#undef	PBS_DEFAULTED
#endif
