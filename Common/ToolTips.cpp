// ToolTips.cpp : implementation file
//

#include "stdafx.h"
#include "ToolTips.h"
#include "PPToolTip.h"
#include "AcadColorTable.h"
#include "Project.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "PropertyObject.h"
#include "Resource.h"

const TCHAR s1[] = _T("\\");
const TCHAR s2[] = _T("/");
const TCHAR s3[] = _T(".avi");
const TCHAR s4[] = _T("><al_l>");
const TCHAR s6[] = _T("<b><ct=0x");
const TCHAR s7[] = _T("</ct><br>");
const TCHAR s9[] = _T("\\colortbl");
const TCHAR s10[] = _T("\\red");
const TCHAR s11[] = _T("\\green");
const TCHAR s12[] = _T("\\blue");
const TCHAR s13[] = _T(";");
const TCHAR s14[] = _T("\\viewkind");
const TCHAR s15[] = _T("\\lang");
const TCHAR s16[] = _T("\\f0");
const TCHAR s17[] = _T("\\fs");
const TCHAR s18[] = _T(" ");
const TCHAR s19[] = _T(">");
const TCHAR s20[] = _T(" \\b0");
const TCHAR s21[] = _T("</b>");
const TCHAR s22[] = _T(" \\b");
const TCHAR s23[] = _T("<b>");
const TCHAR s24[] = _T(" \\i0");
const TCHAR s25[] = _T("</i>");
const TCHAR s26[] = _T(" \\i");
const TCHAR s27[] = _T("<i>");
const TCHAR s28[] = _T(" \\ulnone");
const TCHAR s29[] = _T("</u>");
const TCHAR s30[] = _T(" \\ul0");
const TCHAR s31[] = _T(" \\cf");
const TCHAR s32[] = _T(" \\ul");
const TCHAR s33[] = _T("<u>");
const TCHAR s34[] = _T(" \\tab"); 
const TCHAR s35[] = _T("<t=2>");
const TCHAR s36[] = _T("\\b0");
const TCHAR s37[] = _T("<ct=0x");
const TCHAR s38[] = _T("\\b");
const TCHAR s39[] = _T("\\cf");
const TCHAR s40[] = _T("\\i0");
const TCHAR s41[] = _T("\\par }");
const TCHAR s42[] = _T("\\i");
const TCHAR s43[] = _T("\\par ");
const TCHAR s44[] = _T("\\ulnone");
const TCHAR s45[] = _T("\\par \\pard");
const TCHAR s46[] = _T("\\ul0");

const TCHAR s48[] = _T("\\ul");

const TCHAR s50[] = _T("\\tab"); 

const TCHAR s52[] = _T("\\cf0"); 
const TCHAR s53[] = _T("<ct=0x000000>");
const TCHAR s54[] = _T("\\pard");
const TCHAR s55[] = _T("\\ulnone ");
const TCHAR s56[] = _T("\\ul0 ");
const TCHAR s57[] = _T("\\ul ");

const TCHAR s58[] = _T("\\cf0 ");
const TCHAR s59[] = _T("\\par }\r\n");
const TCHAR s60[] = _T("\\par }");
const TCHAR s61[] = _T("\\par  ");


CString ConstructTooltipHtml( LPCTSTR pszTitle, COLORREF crTitleColor /*= RGB(0,0,0)*/,
															bool bSeparator /*= false*/, LPCTSTR pszMainText /*= NULL*/ )
{
	CString sBody;
	if( pszTitle && *pszTitle )
	{
		if( crTitleColor != RGB(0,0,0) )
		{
			sBody.Format( _T("<b><font color=#%02X%02X%02X>%s</font></b>"),
										GetRValue(crTitleColor), GetGValue(crTitleColor), GetBValue(crTitleColor),
										pszTitle );
		}
		else
			sBody.Format( _T("<b>%s</b>"), (LPCTSTR)pszTitle );
	}
	if( bSeparator )
		sBody += _T("<br><hr=100%>");
	if( pszMainText && *pszMainText )
	{
		if( !sBody.IsEmpty() )
			sBody += _T("<br>");
		sBody += pszMainText;
	}
	return sBody;
}


void SetToolTipEx(CWnd *pWnd, CPPToolTip &m_tooltip, CDclControlObject *pControl)
{
	m_tooltip.SetBehaviour( PPTOOLTIP_MULTIPLE_SHOW | PPTOOLTIP_NOCLOSE_OVER );

	RefCountedPtr< CPropertyObject > pToolTipTitle = pControl->GetPropertyObject(nToolTipTitle);	
	RefCountedPtr< CPropertyObject > pToolTipLine = pControl->GetPropertyObject(nToolTipLine);	
	RefCountedPtr< CPropertyObject > pToolTipBody = pControl->GetPropertyObject(nToolTipBody);	
	RefCountedPtr< CPropertyObject > pToolTipPicture = pControl->GetPropertyObject(nToolTipPicture);	
	RefCountedPtr< CPropertyObject > pToolTipAvi = pControl->GetPropertyObject(nToolTipAviFileName);
	RefCountedPtr< CPropertyObject > pToolTipTitleColor = pControl->GetPropertyObject(nToolTipTitleColor);

	CString sTitle;
	CString sMain;
	int nPic = 0;
	CString sAvi;
	if( pToolTipTitle )
		sTitle = pToolTipTitle->GetStringValue();
	if( pToolTipBody )
		sMain = pToolTipBody->GetStringValue();
	if( pToolTipPicture )
		nPic = pToolTipPicture->GetLongValue();
	if( pToolTipAvi )
		sAvi = pToolTipAvi->GetStringValue();
	if( sTitle.IsEmpty() && sMain.IsEmpty() && nPic == 0 && sAvi.IsEmpty() )
		return;
	CString sBody =
		ConstructTooltipHtml( sTitle,
													(pToolTipTitleColor? pToolTipTitleColor->GetOLEColorValue() : RGB(0,0,0)),
													(pToolTipLine && pToolTipLine->GetBooleanValue()),
													sMain );

	CProject* pProject = pControl->GetOwnerProject();

	if( pToolTipPicture )
	{
		if (nPic == -1)
			m_tooltip.AddTool(pWnd, sBody, IDI_HELP);
		else if (nPic == -2)
			m_tooltip.AddTool(pWnd, sBody, IDI_INFO);
		else if (nPic == -3)
			m_tooltip.AddTool(pWnd, sBody, IDI_EXCLEMATION);
		else if (nPic == -4)
			m_tooltip.AddTool(pWnd, sBody, IDI_X);
		else if (nPic == 0)	
			m_tooltip.AddTool(pWnd, sBody);
		else if (nPic > 0 && pProject)
		{
			HICON hIcon = pProject->CloneIcon(nPic);
			if (hIcon != NULL)
				m_tooltip.AddTool(pWnd, sBody, hIcon);
			else				
				m_tooltip.AddTool(pWnd, sBody);
		}
		else
			m_tooltip.AddTool(pWnd, sBody);
	}
	else
		m_tooltip.AddTool(pWnd, sBody);
}

void SetToolTipEx(CWnd *pWnd, CPPToolTip &m_tooltip, 
				  CString sTitleIn, 
				  CString sMainIn, 
				  int nLine,
				  int nColor, 
				  int nPicture,
				  CString sAvi,
				  CDclControlObject *pControl = NULL)
{	
	if (sTitleIn.IsEmpty() && sMainIn.IsEmpty() && nPicture == 0 && sAvi.IsEmpty())
		return;

	m_tooltip.SetBehaviour( PPTOOLTIP_MULTIPLE_SHOW | PPTOOLTIP_NOCLOSE_OVER );
			
	CString sBody = ConstructTooltipHtml( sTitleIn, GetRGBColor( nColor ), (nLine != 0), sMainIn );

	const CProject* pProject = pControl? pControl->GetOwnerProject() : NULL;

	int nPic = nPicture;

	if (nPic == -1)
		m_tooltip.AddTool(pWnd,  sBody, IDI_HELP);
	else if (nPic == -2)
		m_tooltip.AddTool(pWnd,  sBody, IDI_INFO);
	else if (nPic == -3)
		m_tooltip.AddTool(pWnd, sBody, IDI_EXCLEMATION);
	else if (nPic == -4)
		m_tooltip.AddTool(pWnd, sBody, IDI_X);
	else if (nPic == 0)	
		m_tooltip.AddTool(pWnd, sBody);
	else if (nPic > 0 && pProject)
	{
		HICON hIcon = pProject->CloneIcon(nPic);
		if (hIcon != NULL)
			m_tooltip.AddTool(pWnd, sBody, hIcon);
		else				
			m_tooltip.AddTool(pWnd, sBody);
	}
	else
		m_tooltip.AddTool(pWnd, sBody);
}
