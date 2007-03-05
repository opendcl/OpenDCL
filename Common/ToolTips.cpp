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

CString GetHtmlText(CString sMain);

const TCHAR s1[] = _T("\\");
const TCHAR s2[] = _T("/");
const TCHAR s3[] = _T(".avi");
const TCHAR s4[] = _T("><al_l>");
const TCHAR s5[] = _T("</b><br>");
const TCHAR s6[] = _T("<b><ct=0x");
const TCHAR s7[] = _T("</ct><br>");
const TCHAR s8[] = _T("<ct=0x000000><hr=100%>");
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


void SetToolTipEx(CWnd *pWnd, CPPToolTip &m_tooltip, CDclControlObject *pControl)
{
	m_tooltip.SetBehaviour( PPTOOLTIP_MULTIPLE_SHOW | PPTOOLTIP_NOCLOSE_OVER );

	RefCountedPtr< CPropertyObject > pToolTipText = pControl->GetPropertyObject(nToolTipText);	
	RefCountedPtr< CPropertyObject > pToolTipLine = pControl->GetPropertyObject(nToolTipLine);	
	RefCountedPtr< CPropertyObject > pToolTipBody = pControl->GetPropertyObject(nToolTipBody);	
	RefCountedPtr< CPropertyObject > pToolTipPicture = pControl->GetPropertyObject(nToolTipPicture);	
	RefCountedPtr< CPropertyObject > pToolTipAvi = pControl->GetPropertyObject(nToolTipAviFileName);
	RefCountedPtr< CPropertyObject > pToolTipTitleColor = pControl->GetPropertyObject(nToolTipTitleColor);

	CString sBody;
	CString sTitle;
	CString sMain;
	if( pToolTipTitleColor )
	{
		COLORREF clr = pToolTipTitleColor->GetOLEColorValue();
		sBody.Format( _T("<b><font color=#%02X%02X%02X>"), GetRValue(clr), GetGValue(clr), GetBValue(clr) );
	}
	if( pToolTipText )
	{
		sTitle = pToolTipText->GetStringValue();

		sBody += sTitle + s5;
		
		if (pToolTipLine && pToolTipLine->GetBooleanValue() == TRUE)
			sBody += s8;

		if( pToolTipBody )
			sMain = GetHtmlText(pToolTipBody->GetStringValue());

		if (sTitle.GetLength() == 0 && sMain.GetLength() == 0)
		{
			sBody = CString();
			m_tooltip.AddTool(pWnd, sBody);
			return;
		}

		sBody += _T("</font></b><br>") + sMain;
	}
	CProject* pProject = pControl->GetOwnerProject();

	//if( pToolTipAvi )
	//{
	//	CString sAvi = pToolTipAvi->GetStringValue();

	//	m_tooltip.m_AviFileName.Empty();
	//	if (sAvi.GetLength() > 0)
	//	{
	//		if (pProject)
	//		{
	//			int n = pProject->GetBaseFileName().Find(s1);
	//			if (n == -1)
	//				n = pProject->GetBaseFileName().Find(s2);

	//			int nNext = n;
	//			while (nNext > -1)
	//			{
	//				nNext = pProject->GetBaseFileName().Find(s1, n+1);
	//				if (nNext == -1)
	//					nNext = pProject->GetBaseFileName().Find(s2, n+1);
	//				if (nNext > -1)
	//					n = nNext;	
	//			}

	//			sAvi = pProject->GetBaseFileName().Left(n+1) + sAvi;
	//			if (_tcsicmp(sAvi.Right(4), s3) != 0)
	//				sAvi += s3;
	//		}
	//		
	//		m_tooltip.m_AviFileName = sAvi;
	//	}
	//}

	
	if( pToolTipPicture )
	{
		int nPic = pToolTipPicture->GetLongValue();

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
	m_tooltip.SetBehaviour( PPTOOLTIP_MULTIPLE_SHOW | PPTOOLTIP_NOCLOSE_OVER );

	const CProject* pProject = pControl? pControl->GetOwnerProject() : NULL;
			
	if (sTitleIn.GetLength() > 0 && sMainIn.GetLength() == 0)
	{
		m_tooltip.AddTool(pWnd, sTitleIn);
		return;
	}
	if (sTitleIn.GetLength() == 0 && sMainIn.GetLength() > 0)
	{
		m_tooltip.AddTool(pWnd, sMainIn);
		return;
	}
	
	CString sBody;
	CString sTitle;
	CString sMain;

	COLORREF clr = GetRGBColor(nColor);
	sBody.Format( _T("<b><font color=#%02X%02X%02X>"), GetRValue(clr), GetGValue(clr), GetBValue(clr) );
	
	sTitle = sTitleIn;

	sBody += sTitle + s5;
	
	if (nLine == TRUE)
		sBody += s8;
	sMain = GetHtmlText(sMainIn);

	if (sTitle.GetLength() == 0 && sMain.GetLength() == 0)
		return;

	sBody += s7 + sMain;
	
	//m_tooltip.m_AviFileName.Empty();
	//if (sAvi.GetLength() > 0)
	//{
	//	if (pProject != NULL)
	//	{
	//		int n = pProject->GetBaseFileName().Find(s1);
	//		if (n == -1)
	//			n = pProject->GetBaseFileName().Find(s2);

	//		int nNext = n;
	//		while (nNext > -1)
	//		{
	//			nNext = pProject->GetBaseFileName().Find(s1, n+1);
	//			if (nNext == -1)
	//				nNext = pProject->GetBaseFileName().Find(s2, n+1);
	//			if (nNext > -1)
	//				n = nNext;	
	//		}

	//		sAvi = pProject->GetBaseFileName().Left(n+1) + sAvi;
	//		if (_tcsicmp(sAvi.Right(4), s3) != 0)
	//			sAvi += s3;
	//	}
	//	m_tooltip.m_AviFileName = sAvi;
	//}

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

CString GetHtmlText(CString sMain)
{
	CArray<COLORREF, COLORREF> colors;

	if (sMain.GetLength() == 0)
		return sMain; 

	bool bFoundEnd = false;
	int n = sMain.Find(s9);
	while (n > -1)
	{
		CString sRed, sBlue, sGreen;
		
		int nRed = sMain.Find(s10, n);
		int nGreen = sMain.Find(s11, nRed);
		int nBlue = sMain.Find(s12, nGreen);
		int nColorEnd = sMain.Find(s13, nBlue);

		sRed = sMain.Mid(nRed + 4, nGreen - nRed - 4);
		sGreen = sMain.Mid(nGreen + 6, nBlue - nGreen - 6);
		sBlue = sMain.Mid(nBlue + 5, nColorEnd - nBlue - 5);


		colors.Add(RGB(_ttoi(sRed), _ttoi(sGreen), _ttoi(sBlue)));
		if (sMain.Find(s10, nColorEnd) == -1)
			n = -1;
		else
			n = nColorEnd;
	
	}
	
	n = sMain.Find(s14);
	if (n > -1)
		sMain = sMain.Mid(n+9);
	
	n = sMain.Find(s15);	
	if (n > -1)
	{
		sMain = sMain.Mid(n+5);
		n = sMain.Find(s1);	
		if (n > -1)
		{
			sMain = sMain.Mid(n);
		}
	}
	
	n = sMain.Find(s54);
	if (n > -1)
		sMain = sMain.Mid(n+5);

	
	sMain.Replace(s16, CString());
	
	for (int j=30; j>=0; j--)
	{
		char value[80];
		_ltoa(j, value, 10);
		sMain.Replace(CString(s17) + value + s18, CString());
	}
	
	sMain.Replace(s20, s21);
	sMain.Replace(s22, s23);

	sMain.Replace(s24, s25);
	sMain.Replace(s26, s27);

	sMain.Replace(s55, s29);
	sMain.Replace(s56, s29);
	sMain.Replace(s57, s33);
	
	sMain.Replace(s28, s29);
	sMain.Replace(s30, s29);
	//sMain.Replace(s31, s33);

	sMain.Replace(s34, s35);

	sMain.Replace(s36, s21);
	sMain.Replace(s38, s23);

	sMain.Replace(s40, s25);
	sMain.Replace(s42, s27);

	sMain.Replace(s44, s29);
	sMain.Replace(s46, s29);
	sMain.Replace(s48, s33);

	sMain.Replace(s50, s35);

	sMain.Replace(s58, s53);
	sMain.Replace(s52, s53);

	sMain.Replace(s59, CString());
	sMain.Replace(s60, CString());
	sMain.Replace(s61, CString());
	sMain.Replace(s43, CString());


	for (int i=0; i<colors.GetSize(); i++)
	{
		char sIndex[80];
		char sColor[80];

		_ltoa(i+1, sIndex, 10);
		_ltoa(colors[i], sColor, 16);
		
		sMain.Replace(
			CString(s31) + sIndex, 
			CString(s37) + sColor + s19);
		sMain.Replace(
			CString(s39) + sIndex + s18, 
			CString(s37) + sColor + s19);
		sMain.Replace(
			CString(s39) + sIndex, 
			CString(s37) + sColor + s19);
	}

	n = sMain.Find(s45);
	if (n > -1)
		sMain = sMain.Left(n-2);
	
	n = sMain.Find(s41);
	if (n > -1)
		sMain = sMain.Left(n-2);
	sMain.Replace(s43, CString());

	return sMain;
	
}