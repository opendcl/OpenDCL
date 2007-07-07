// FontSizes.cpp : implementation file
//

#include "stdafx.h"
#include "FontSizes.h"
#include "ToolBox.h"
#include "OpenDCLView.h"
#include "EditorWorkspace.h"


/////////////////////////////////////////////////////////////////////////////
// CFontSizes

CFontSizes::CFontSizes()
{
}

CFontSizes::~CFontSizes()
{
}


BEGIN_MESSAGE_MAP(CFontSizes, CComboBox)
	//{{AFX_MSG_MAP(CFontSizes)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontSizes message handlers

void CFontSizes::OnSelchange() 
{
	CString sString;
	int nSel = GetCurSel();
	int nTextLength = GetLBTextLen(nSel);      
	GetLBText(nSel, sString.GetBuffer(nTextLength));
	sString.ReleaseBuffer();
	
	if (theEditorWorkspace.GetToolBox()->m_pActiveView == NULL)
		return;
	if (((COpenDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->m_pThisDclForm == NULL)
		return;
	
	if (!sString.IsEmpty())
		((COpenDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->UpdateFontSize(_tstol(sString));
	
}

void CFontSizes::OnEditchange() 
{
	CString sString;
	GetWindowText(sString);
	
	if (theEditorWorkspace.GetToolBox()->m_pActiveView == NULL)
		return;
	if (((COpenDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->m_pThisDclForm == NULL)
		return;
	
	if (!sString.IsEmpty())
		((COpenDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->UpdateFontSize(_tstol(sString));
	
}
