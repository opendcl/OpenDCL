// ArxDwgListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxDwgListCtrl.h"
#include "Resource.h"
#include "Workspace.h"
#include "InvokeMethod.h"
#include "ControlPane.h"
#include "DialogObject.h"
#include "ArxFolderComboCtrl.h"
#include "PropertyIds.h"
#include "DclControlTemplate.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include <set>


typedef struct _compare_i //case insensitive comparison functor
{
	bool operator() (const CString& lhs, const CString& rhs) const
	{
		CString sLhs( lhs );
		CString sRhs( rhs );
		sLhs.MakeUpper();
		sRhs.MakeUpper();
		return (sLhs < sRhs);
	}
} compare_i;


/////////////////////////////////////////////////////////////////////////////
// CArxDwgListCtrl

CArxDwgListCtrl::CArxDwgListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CListBoxCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mDragDropService( this )
, mBlockInsertDropTarget( this )
, mnRowHeight( -1 )
{
	m_pDirComboBox = NULL;
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxDwgListCtrl::~CArxDwgListCtrl()
{
}

LPCTSTR CArxDwgListCtrl::GetDragTextPrefix() const
{
	static CString sDragPrefix;
	sDragPrefix = msPath;
	sDragPrefix.TrimRight( _T("\\/") );
	sDragPrefix += _T('\\');
	return sDragPrefix;
}

bool CArxDwgListCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	if( bSuccess )
		Dir( NULL );

	return bSuccess;
}

DWORD CArxDwgListCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();
	dwStyle &= ~(DWORD)(LBS_USETABSTOPS);
	dwStyle |= LBS_OWNERDRAWVARIABLE;
	return dwStyle;
}

void CArxDwgListCtrl::HandleDpiChanged()
{
	__super::HandleDpiChanged();
	ApplyProperty( mpTemplate->GetPropertyObject( Prop::RowHeight ) );
}

bool CArxDwgListCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::RowHeight:
		{
			mnRowHeight = FromDIP( pProp->GetLongValue() );
			OnNeedRepaint( true );
			break;
		}
	}
	return !bFailed;
}

void CArxDwgListCtrl::ClearThumbnailList()
{
	mmapThumbnail.RemoveAll();
}

void CArxDwgListCtrl::Dir(LPCTSTR pszDir) 
{
	ResetContent();
	ClearThumbnailList();

	CString sPath;
	DWORD cchPath = GetFullPathName( pszDir? pszDir : _T("."), MAX_PATH, sPath.GetBuffer( MAX_PATH + 1 ), NULL );
	sPath.ReleaseBuffer( cchPath );
	sPath = sPath.TrimRight( _T("\\/") );
	msPath = sPath;

	std::set< CString, compare_i > setFolders;
	CFileFind finderDirectory;
	BOOL bResult = finderDirectory.FindFile( sPath + _T("\\*.*") );
	while( bResult )
	{	
		bResult = finderDirectory.FindNextFile();
		if( !finderDirectory.IsDirectory() )
			continue;
		if( finderDirectory.IsDots() )
			continue;
		setFolders.insert( finderDirectory.GetFileName() );
	}
	finderDirectory.Close();

	std::set< CString, compare_i > setDwgFiles;
	CFileFind finderDwgFile;
	bResult = finderDwgFile.FindFile( sPath + _T("\\*.dwg") );
	while( bResult )
	{	
		bResult = finderDwgFile.FindNextFile();
		if( finderDwgFile.IsDirectory() )
			continue;
		if( finderDwgFile.IsDots() )
			continue;
		setDwgFiles.insert( finderDwgFile.GetFileName() );
	}
	finderDwgFile.Close();

	for( std::set< CString, compare_i >::const_iterator iter = setFolders.begin(); iter != setFolders.end(); ++iter )
	{
		int nItemIdx = AddString( *iter );
		SetItemData( nItemIdx, 0 );
		SetItemHeight( nItemIdx, 17 );
	}

	for( std::set< CString, compare_i >::const_iterator iter = setDwgFiles.begin(); iter != setDwgFiles.end(); ++iter )
	{
		int nItemIdx = AddString( *iter );
		SetItemData( nItemIdx, 1 );
		CDwgThumbnail* pThumbnail = new CDwgThumbnail( sPath + _T('\\') + *iter );
		int nHeight = mnRowHeight;
		if( nHeight <= 0 )
		{
			nHeight = pThumbnail->GetBitmapSize().cy;
			if( nHeight <= 0 )
				nHeight = 100;
			nHeight += 25; //add space for filename text
		}
		SetItemHeight( nItemIdx, nHeight );
		mmapThumbnail[*iter] = pThumbnail;
	}
}


BEGIN_MESSAGE_MAP(CArxDwgListCtrl, CListBoxCtrl)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnCbnSelchange)
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxDwgListCtrl message handlers

void CArxDwgListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);

	if (GetCount() == 0)
		return;
	if (lpDrawItemStruct->itemID < (UINT)GetTopIndex())
		return;
	CRect rc(lpDrawItemStruct->rcItem);

	// Lets make a CDC for ease of use
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	ASSERT(pDC); // Attached failed
	int nIndexDC = pDC->SaveDC();

	COLORREF clrBackground = GetSysColor(COLOR_WINDOW);
	COLORREF clrForeground = GetSysColor(COLOR_BTNTEXT);

	bool bSelected = ((GetCurSel() == lpDrawItemStruct->itemID) || (lpDrawItemStruct->itemState & ODS_SELECTED && GetFocus() == this));
	if( bSelected )
	{
		clrBackground = GetSysColor(COLOR_HIGHLIGHT);
		clrForeground = GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else
	{
		CAcadColorService* pColorService = GetColorService();
		if( pColorService )
		{
			if( pColorService->IsBackgroundTransparent() )
			{
				CDialogObject* pHostDlg = mpControlPane->GetDialogObject();
				if( pHostDlg )
				{
					CAcadColorService* pDlgColor = pHostDlg->GetColorService();
					if( pDlgColor )
						clrBackground = (pDlgColor->GetBackgroundColor() & 0xFFFFFF);
				}
			}
			else if( !pColorService->IsBackgroundNotSet() )
				clrBackground = pColorService->GetBackgroundColor();
		}
	}

	pDC->FillSolidRect( &rc, clrBackground );

	CString sFilename;
	GetText(lpDrawItemStruct->itemID, sFilename);

	CString sPath = msPath;
	if( !sPath.IsEmpty() && (sPath.GetAt( sPath.GetLength() - 1 ) != _T('\\')) )
		sPath += _T('\\');
	sPath += sFilename;
	SHFILEINFO sfi;
	SHGetFileInfo( sPath, 0, &sfi, sizeof(sfi), SHGFI_ICON | (bSelected? SHGFI_SELECTED : 0) );
	DrawIconEx( pDC->m_hDC, rc.left + 1, rc.top + 1, sfi.hIcon, 16, 16, 0, NULL, DI_NORMAL );
	DestroyIcon( sfi.hIcon );

	CRect rcText = rc;
	rcText.left += 18;
	rcText.top += 2;

	if( GetItemData(lpDrawItemStruct->itemID) != 0 )
	{
		CSize szText;
		GetTextExtentPoint32(pDC->m_hDC, sFilename, sFilename.GetLength(), &szText);
		CRect rcImage( rcText );
		rcImage.top += (szText.cy + 1);
		rcImage.bottom -= 2;
		CDwgThumbnailPtr ThumbnailPtr;
		if( mmapThumbnail.Lookup( sFilename, ThumbnailPtr ) )
			ThumbnailPtr->Render( pDC, rcImage, clrBackground );
	}
	
	pDC->SelectObject(GetFont());
	pDC->SetTextColor(clrForeground);
	pDC->SetBkColor(clrBackground);
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText( sFilename, &rcText, DT_TOP|DT_LEFT );

	pDC->RestoreDC(nIndexDC);
}

BOOL CArxDwgListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CArxDwgListCtrl::OnDblclk() 
{
	CString sSelText;
	
	int nSel = GetCurSel();
	GetText(nSel, sSelText);
	if (GetItemData(nSel) == 0)
	{
		msPath.TrimRight( _T("\\/") );
		msPath	+= _T("\\");
		msPath += sSelText;
		Dir(msPath);
		if (m_pDirComboBox != NULL)
		{
			m_pDirComboBox->AddPath(msPath);
		}	
		GetArxServices()->HandleEvent( Prop::EventFolderChanged, false, args_S( msPath ) );
	}
	GetArxServices()->HandleEvent( Prop::EventDblClicked );
}


void CArxDwgListCtrl::OnDestroy() 
{
	__super::OnDestroy();
	ClearThumbnailList();
}

int CArxDwgListCtrl::HitTest(CPoint point) 
{
	CRect rc;

	for (int i=0; i<GetCount(); i++)
	{
		GetItemRect(i, &rc);

		if (rc.PtInRect(point) == TRUE)
			return i;
	}

	return -1;
}


void CArxDwgListCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus(pOldWnd);
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
}


void CArxDwgListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, ToDIP( point.x ), ToDIP( point.y ) ) );
	__super::OnMouseMove(nFlags, point);
}

void CArxDwgListCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxDwgListCtrl::OnCbnSelchange() 
{
	__super::OnCbnSelchange();

	int nSelCount = GetSelCount();
	
	if (nSelCount > -1)
		GetArxServices()->HandleEvent( Prop::EventSelChanged, args_NS( nSelCount, _T("") ) );
	else
	{
		CString sText;
		int nCurSel = GetCurSel();
		if( nCurSel >= 0 )
			GetText( nCurSel, sText );
		GetArxServices()->HandleEvent( Prop::EventSelChanged, args_NS( 1, sText ) );
	}   	
}
