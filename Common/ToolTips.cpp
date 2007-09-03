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


void SetToolTipEx(CWnd *pWnd, CPPToolTip& tooltip, CDclControlObject *pControl)
{
	tooltip.RemoveTool( pWnd );
	tooltip.SetBehaviour( PPTOOLTIP_MULTIPLE_SHOW );

	TPropertyPtr pToolTipTitle = pControl->GetPropertyObject(Prop::ToolTipTitle);	
	TPropertyPtr pToolTipLine = pControl->GetPropertyObject(Prop::ToolTipLine);	
	TPropertyPtr pToolTipBody = pControl->GetPropertyObject(Prop::ToolTipBody);	
	TPropertyPtr pToolTipPicture = pControl->GetPropertyObject(Prop::ToolTipPicture);	
	TPropertyPtr pToolTipAvi = pControl->GetPropertyObject(Prop::ToolTipAviFileName);
	TPropertyPtr pToolTipTitleColor = pControl->GetPropertyObject(Prop::ToolTipTitleColor);
	TPropertyPtr pToolTipBalloon = pControl->GetPropertyObject(Prop::ToolTipBalloon);

	if( pToolTipBalloon && !pToolTipBalloon->GetBooleanValue() )
		tooltip.SetDefaultSizes( FALSE );
	else
		tooltip.SetDefaultSizes( TRUE );

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
			tooltip.AddTool(pWnd, sBody, IDI_HELP);
		else if (nPic == -2)
			tooltip.AddTool(pWnd, sBody, IDI_INFO);
		else if (nPic == -3)
			tooltip.AddTool(pWnd, sBody, IDI_WARN);
		else if (nPic == -4)
			tooltip.AddTool(pWnd, sBody, IDI_X);
		else if (nPic == 0)	
			tooltip.AddTool(pWnd, sBody);
		else if (nPic > 0 && pProject)
		{
			HICON hIcon = pProject->CloneIcon(nPic);
			if (hIcon != NULL)
				tooltip.AddTool(pWnd, sBody, hIcon);
			else				
				tooltip.AddTool(pWnd, sBody);
		}
		else
			tooltip.AddTool(pWnd, sBody);
	}
	else
		tooltip.AddTool(pWnd, sBody);
}

void SetToolTipEx(CWnd *pWnd, CPPToolTip &tooltip, 
				  CString sTitleIn, 
				  CString sMainIn, 
				  int nLine,
				  int nColor, 
				  int nPicture,
				  CString sAvi,
				  CDclControlObject *pControl = NULL)
{	
	if (sTitleIn.IsEmpty() && sMainIn.IsEmpty() && Prop::Picture == 0 && sAvi.IsEmpty())
		return;

	tooltip.SetBehaviour( PPTOOLTIP_MULTIPLE_SHOW );

	if( pControl )
	{
		TPropertyPtr pToolTipBalloon = pControl->GetPropertyObject(Prop::ToolTipBalloon);
		if( pToolTipBalloon && !pToolTipBalloon->GetBooleanValue() )
			tooltip.SetDefaultSizes( FALSE );
	}

	CString sBody = ConstructTooltipHtml( sTitleIn, GetRGBColor( nColor ), (nLine != 0), sMainIn );

	const CProject* pProject = pControl? pControl->GetOwnerProject() : NULL;

	int nPic = Prop::Picture;

	if (nPic == -1)
		tooltip.AddTool(pWnd,  sBody, IDI_HELP);
	else if (nPic == -2)
		tooltip.AddTool(pWnd,  sBody, IDI_INFO);
	else if (nPic == -3)
		tooltip.AddTool(pWnd, sBody, IDI_WARN);
	else if (nPic == -4)
		tooltip.AddTool(pWnd, sBody, IDI_X);
	else if (nPic == 0)	
		tooltip.AddTool(pWnd, sBody);
	else if (nPic > 0 && pProject)
	{
		HICON hIcon = pProject->CloneIcon(nPic);
		if (hIcon != NULL)
			tooltip.AddTool(pWnd, sBody, hIcon);
		else				
			tooltip.AddTool(pWnd, sBody);
	}
	else
		tooltip.AddTool(pWnd, sBody);
}
