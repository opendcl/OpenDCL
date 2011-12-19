// GridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GridCtrl.h"
#include "ControlPane.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "FolderComboBox.h"
#include "ButtonEditCtrl.h"
#include "ComboEditCtrl.h"
#include "DateTimeEditCtrl.h"
#include "ImageComboEditCtrl.h"
#include "TextBoxEditCtrl.h"
#include "ToggleEditCtrl.h"
#include "RadioEditCtrl.h"
#include "AngleFilter.h"
#include "CurrencyFilter.h"
#include "DateFilter.h"
#include "IntegerFilter.h"
#include "LowerCaseFilter.h"
#include "MultilineFilter.h"
#include "NumericFilter.h"
#include "PasswordFilter.h"
#include "PercentageFilter.h"
#include "SymbolNameFilter.h"
#include "TimeFilter.h"
#include "UpperCaseFilter.h"
#include "ComboFilter.h"
#include "Workspace.h"
#include "AcadColorTable.h"
#include "Resource.h"
#include <algorithm>

#define BP_PUSHBUTTON			0x00000001
#define BP_RADIOBUTTON			0x00000002
#define BP_CHECKBOX				0x00000003

#define PBS_NORMAL				0x00000001
#define PBS_HOT					0x00000002
#define PBS_PRESSED				0x00000003
#define PBS_DISABLED			0x00000004
#define PBS_DEFAULTED			0x00000005

#define RBS_UNCHECKEDNORMAL		0x00000001
#define RBS_UNCHECKEDHOT		0x00000002
#define RBS_UNCHECKEDPRESSED	0x00000003
#define RBS_UNCHECKEDDISABLED	0x00000004
#define RBS_CHECKEDNORMAL		0x00000005
#define RBS_CHECKEDHOT			0x00000006
#define RBS_CHECKEDPRESSED		0x00000007
#define RBS_CHECKEDDISABLED		0x00000008

#define HP_HEADERITEM			0x00000001

#if (_WIN32_WINNT < 0x501)
typedef struct tagNMLVSCROLL
{
    NMHDR   hdr;
    int     dx;
    int     dy;
} NMLVSCROLL, *LPNMLVSCROLL;

#define LVN_BEGINSCROLL          (LVN_FIRST-80)          
#define LVN_ENDSCROLL            (LVN_FIRST-81)
#endif //(_WIN32_WINNT < 0x501)

#ifndef LVS_EX_LABELTIP
#define LVS_EX_LABELTIP 0x00004000
#endif


struct TCellState { CString text; UINT state; _CellData data; };


class CDriveComboDropdownListEditCtrl : public CFolderComboBox, public CGridCellEditCtrl
{
	class CClippingWnd : public CStatic
	{
		CGridCtrl* mpGridCtrl;
	public:
		CClippingWnd( CGridCtrl* pGridCtrl ) : mpGridCtrl( pGridCtrl ) {}
	protected:
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
			{
				LRESULT lResult = __super::WindowProc( message, wParam, lParam );
				if( message == WM_COMMAND )
				{
					if( HIWORD(wParam) == CBN_KILLFOCUS )
						return mpGridCtrl->SendMessage( WM_COMMAND, wParam, lParam );
				}
				return lResult;
			}
	} mClippingWnd;
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left, rcCell.top, rcCell.right, rcCell.top + 120 );
		}
public:
	CDriveComboDropdownListEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, UINT nID = 100 )
		: CFolderComboBox()
		, CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		, mClippingWnd( pGridCtrl )
		{
			CRect rcCell = pGridCtrl->GetCellRect( nRow, nCol );
			rcCell.DeflateRect( 2, 2 );
			CRect rcCtrl = CalcRect( rcCell );
			mClippingWnd.Create( _T(""), WS_CHILD, rcCell, pGridCtrl );
			rcCtrl.MoveToXY( 0, 0 );
			Create( &mClippingWnd,
							rcCtrl,
							0,
							100 );
			SetFont( pGridCtrl->GetFont() );
			GetWindowRect( &rcCtrl );
			mClippingWnd.ScreenToClient( &rcCtrl );
			CRect rcClip;
			rcClip.IntersectRect( &rcCtrl, &CRect( 0, 0, rcCell.Width(), rcCell.Height() ) );
			mClippingWnd.SetWindowPos( NULL, 0, 0, rcClip.Width(), rcClip.Height(),
																 (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING) );
			rcCtrl.MoveToY( rcCtrl.top + rcClip.Height() - rcCtrl.Height() );
			SetWindowPos( NULL, 0, rcCtrl.top, 0, 0,
										(SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING) );
			mClippingWnd.ShowWindow( SW_SHOW );
			if( GetCount() == 0 )
			{
				CRect rcWnd;
				GetWindowRect( &rcWnd );
				SetWindowPos( NULL, 0, 0, rcWnd.Width(), rcWnd.Height(), (SWP_NOZORDER | SWP_NOMOVE) );
			}
			CString sText = pGridCtrl->GetCellText( nRow, nCol );
			SelectPath( sText );
			//SetWindowText( sText );
			//int idxMatch = FindStringExact( 0, sText );
			//if( idxMatch >= 0 )
			//	SetCurSel( idxMatch );
			SetFocus();
		}
	virtual ~CDriveComboDropdownListEditCtrl()
		{
			CString sText = GetSelectedPath();
			mpGridCtrl->SetCellText( mnRow, mnCol, sText );
			mClippingWnd.DestroyWindow();
		}
};


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl

CGridCtrl::CGridCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mcColumns( 0 )
, mbHasRowHeader( pTemplate->GetBooleanProperty( Prop::RowHeader ) )
, mbHasGridLines( pTemplate->GetBooleanProperty( Prop::GridLines ) )
, mbAlternateColumnColors( pTemplate->GetLongProperty( Prop::AlternateOrient ) != 0 )
, mclrAlternate( pTemplate->GetColorProperty( Prop::AlternatingColor ) )
, mpCellEditCtrl( NULL )
, mnRowHeight( -1 )
, mbIgnoreChange( false )
{
	mColorService.SetForegroundColor( GetSysColor(COLOR_BTNTEXT) );
	mOptionButtonImageList.Create( 13, 13, ILC_COLOR8 | ILC_MASK, 2, 1 );
	CBitmap bmpNon;
	bmpNon.LoadBitmap( IDB_OPTBTN );
	mOptionButtonImageList.Add( &bmpNon, RGB(255,0,255) );
	CBitmap bmpSel;
	bmpSel.LoadBitmap( IDB_OPTBTNSEL );
	mOptionButtonImageList.Add( &bmpSel, RGB(255,0,255) );

	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CGridCtrl::~CGridCtrl()
{
	HideEditControls();
}

//static
const UINT& CGridCtrl::refWM_CHECKFOCUS()
{
	static const UINT WM_CHECKFOCUS = RegisterWindowMessage( _T("OpenDCL.Grid.CheckFocus") );
	return WM_CHECKFOCUS;
}

#undef SubclassWindow
bool CGridCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);
	if( bSuccess )
	{
	#ifdef _UNICODE
		BOOL bUnicode = TRUE;
	#else
		BOOL bUnicode = FALSE;
	#endif
		SendMessage( CCM_SETUNICODEFORMAT, (WPARAM)bUnicode, 0 );
	}

	if( bSuccess )
	{
		DWORD dwExStyle = GetExtendedStyle();
		dwExStyle |= (LVS_EX_LABELTIP /*| LVS_EX_SUBITEMIMAGES*/);
		SetExtendedStyle( dwExStyle );
	}

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CGridCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_SHAREIMAGELISTS;
	if( !mpTemplate->GetBooleanProperty( Prop::ColHeader ) )
		dwStyle |= LVS_NOCOLUMNHEADER;
	switch( mpTemplate->GetLongProperty( Prop::ListViewSort ) )
	{
	case 1:
		dwStyle |= LVS_SORTDESCENDING;
		break;
	case 2:
		dwStyle |= LVS_SORTASCENDING;
		break;
	}
	if( !mpTemplate->GetBooleanProperty( Prop::LabelWrap ) )
		dwStyle |= LVS_NOLABELWRAP;
	if( mpTemplate->GetBooleanProperty( Prop::ShowSelectAlways ) )
		dwStyle |= LVS_SHOWSELALWAYS;
	if( !mpTemplate->GetBooleanProperty( Prop::MultiSelect ) )
		dwStyle |= LVS_SINGLESEL;
	return dwStyle;
}

bool CGridCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::ImageList:
		{
			TImageListPtr pImageList = mpTemplate->GetImageList();
			if (pImageList && pImageList->GetImageList().GetSafeHandle())
			{
				pImageList->GetImageList().SetBkColor( CLR_NONE );
				SetImageList( &pImageList->GetImageList(), TVSIL_NORMAL );
				SetImageList( &pImageList->GetImageList(), LVSIL_SMALL );
			}
			else
			{
				mDefaultImageList.DeleteImageList();
				mDefaultImageList.Create( 1, (mnRowHeight <= 0)? 24 : mnRowHeight, ILC_COLOR, 1, 1 );
				mDefaultImageList.SetBkColor( CLR_NONE );
				SetImageList( &mDefaultImageList, TVSIL_NORMAL );
				SetImageList( &mDefaultImageList, LVSIL_SMALL );
			}
		}
		break;
	//case Prop::FullRowSelect:
	//	{
	//		DWORD dwExStyle = GetExtendedStyle();
	//		if( pProp->GetBooleanValue() )
	//			dwExStyle |= LVS_EX_FULLROWSELECT;
	//		else
	//			dwExStyle &= ~LVS_EX_FULLROWSELECT;
	//		SetExtendedStyle( dwExStyle );
	//		OnNeedRepaint();
	//	}
	//	break;
	case Prop::GridLines:
		{
			DWORD dwExStyle = GetExtendedStyle();
			if( pProp->GetBooleanValue() )
				dwExStyle |= LVS_EX_GRIDLINES;
			else
				dwExStyle &= ~LVS_EX_GRIDLINES;
			SetExtendedStyle( dwExStyle );
			OnNeedRepaint();
		}
		break;
	case Prop::RowHeight:
		{
			if( !m_hWnd )
				break;
			// row height only changes when Windows sends WM_MEASUREITEM, which can be triggered by WM_WINDOWPOSCHANGED
			long lNewHeight = pProp->GetLongValue();
			if( lNewHeight <= 0 )
				lNewHeight = 24;
			if( mnRowHeight == lNewHeight )
				break;
			mnRowHeight = lNewHeight;
			ApplyProperty( mpTemplate->GetPropertyObject( Prop::ImageList ) );
			CRect rcWnd;
			GetWindowRect( &rcWnd );
			WINDOWPOS wp =
			{
				m_hWnd,
				NULL,
				0,
				0,
				rcWnd.Width(),
				rcWnd.Height(),
				(SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER),
			};
			SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );
		}
		break;
	case Prop::ColHeader:
		if( pProp->GetBooleanValue() )
			ModifyStyle( LVS_NOCOLUMNHEADER, 0, SWP_FRAMECHANGED );
		else
			ModifyStyle( 0, LVS_NOCOLUMNHEADER, SWP_FRAMECHANGED );
		OnFrameChanged();
		break;
	case Prop::RowHeader:
		mbHasRowHeader = pProp->GetBooleanValue();
		OnNeedRepaint();
		break;
	case Prop::ColumnWidths:
		if( !IsEnumeratingProperties() )
		{
			const PropVal::TIntArray* prnWidths = pProp->GetConstIntArrayPtr();	
			size_t idxMax = prnWidths? prnWidths->size() : 0;
			for( int idxColumn = idxMax - 1; idxColumn >= 0; --idxColumn )
				SetColumnWidth( idxColumn, prnWidths->at( idxColumn ) );
		}
		else
			SetupColumns();
		OnNeedRepaint();
		break;
	case Prop::ColumnCaptions:
		if( !IsEnumeratingProperties() )
			SetupColumns();
		break;
	case Prop::ColumnImages:
		if( !IsEnumeratingProperties() )
			SetupColumns();
		break;
	case Prop::ColumnAlignments:
		if( !IsEnumeratingProperties() )
			SetupColumns();
		break;
	case Prop::ColumnStyles:
		OnNeedRepaint();
		break;
	case Prop::ColumnDefaultImages:
		break;
	case Prop::ColumnAlternateImages:
		break;
	case Prop::AlternateOrient:
		mbAlternateColumnColors = (pProp->GetLongValue() != 0);
		OnNeedRepaint();
		break;
	case Prop::AlternatingColor:
		mclrAlternate = GetRGBColor( pProp->GetLongValue() );
		OnNeedRepaint();
		break;
	}
	return !bFailed;
}

void CGridCtrl::SetCurCell( int nRow, int nCol )
{
	HideEditControls();
	InvalidateRect( GetCurCellRect() );
	if( mbHasRowHeader && nCol == 0 )
		nCol = -1;
	if( mCurrentCell.row() != nRow || mCurrentCell.col() != nCol )
	{
		mCurrentCell.set( nRow, nCol );
		EnsureVisible( nRow, FALSE );
		CRect rcCell = GetCurCellRect();
		//if( rcCell.left < 0 )
		//{
		//	UpdateWindow();
		//	ScrollWindow( -rcCell.left, 0 );
		//	SetScrollPos( SB_HORZ, GetScrollPos( SB_HORZ ) + rcCell.left );
		//}
		//else
		//{
		//	CRect rcClient;
		//	GetClientRect( &rcClient );
		//	if( rcCell.right > rcClient.right )
		//	{
		//		UpdateWindow();
		//		ScrollWindow( -rcCell.left, 0 );
		//		SetScrollPos( SB_HORZ, GetScrollPos( SB_HORZ ) + rcCell.left );
		//	}
		//}
		InvalidateRect( GetCurCellRect() );
		OnSelectionChanged();
	}
}

enum Grid::CellStyle CGridCtrl::GetCurCellStyle()
{
	return GetEffectiveCellStyle( mCurrentCell.row(), mCurrentCell.col() );
}

enum Grid::CellStyle CGridCtrl::GetCellStyle( int nRow, int nCol )
{
	const _CellData* pCellData = GetCellData( nRow, nCol );
	if( pCellData )
		return pCellData->mType;
	return Grid::Undefined;
}

enum Grid::CellStyle CGridCtrl::GetEffectiveCellStyle( int nRow, int nCol )
{
	const _CellData* pCellData = GetCellData( nRow, nCol );
	if( pCellData && pCellData->mType != Grid::Undefined )
		return pCellData->mType;

	// otherwise return the default style for the column
	const PropVal::TIntArray* pColStyles = mpTemplate->GetPropertyObject( Prop::ColumnStyles )->GetConstIntArrayPtr();
	if( pColStyles )
	{
		if( nCol >= 0 && (size_t)nCol < pColStyles->size() )
			return (Grid::CellStyle)pColStyles->at( nCol );
	}

	return Grid::Undefined;
}

const _RowData* CGridCtrl::GetRowData( size_t nRow ) const
{
	if( nRow < (int)mRowData.size() )
		return &mRowData.at( nRow );
	return NULL;
}

const _CellData* CGridCtrl::GetCellData( size_t nRow, size_t nCol ) const
{
	const _RowData* pRowData = GetRowData( nRow );
	if( !pRowData )
		return NULL;
	if( nCol < pRowData->mCellData.size() )
	{
		const _CellData& CellData = pRowData->mCellData.at( nCol );
		//if( CellData.mType != Grid_Undefined )
		//	return &CellData;
		return &CellData;
	}
	return NULL;
}

_RowData& CGridCtrl::GetRowDataRef( size_t nRow )
{
	if( nRow >= mRowData.size() )
		mRowData.resize( nRow + 1 );
	return mRowData.at( nRow );
}

_CellData& CGridCtrl::GetCellDataRef( size_t nRow, size_t nCol )
{
	_RowData& RowData = GetRowDataRef( nRow );
	if( nCol >= RowData.mCellData.size() )
		RowData.mCellData.resize( nCol + 1 );
	return RowData.mCellData.at( nCol );
}

CString CGridCtrl::GetCellText( int nRow, int nCol ) const
{
	return __super::GetItemText( nRow, nCol );
}

CString CGridCtrl::GetCurCellText() const
{
	return __super::GetItemText( mCurrentCell.row(), mCurrentCell.col() );
}

bool CGridCtrl::SetCellText( int nRow, int nCol, LPCTSTR pszText )
{
	bool bSuccess = (__super::SetItemText( nRow, nCol, pszText ) != FALSE);
	switch( GetEffectiveCellStyle( nRow, nCol ) )
	{
	case Grid::ArrowHead:
	case Grid::AcadColors:
	case Grid::ImageDropList:
	case Grid::AcadColorCell:
	case Grid::TrueColorCell:
	case Grid::LineWeightCell:
	case Grid::LinetypeCell:
	case Grid::DirectoryCell:
	case Grid::DwgFilesCell:
		SetCellImages( nRow, nCol, GetCellListImage( nRow, nCol, pszText ) );
		break;
	}
	return bSuccess;
}

void CGridCtrl::SetCellStyle( int nRow, int nCol, Grid::CellStyle nStyle, int image /*= -1*/, int altImage /*= -1*/,
															LPCTSTR pszListText /*= NULL*/ )
{
	_CellData& CellData = GetCellDataRef( nRow, nCol );
	CellData.mType = nStyle;
	CellData.midxImage = image;
	CellData.midxAltImage = altImage;
	CellData.mrnComboImage.clear();
	CellData.mrsComboList.clear();
	if( pszListText && *pszListText )
		CellData.mrsComboList.push_back( pszListText );
	switch(nStyle)
	{
	case Grid::CheckBoxes:
		{
			LVITEM lvi = { LVIF_STATE, nRow, nCol, (1 << 12), LVIS_STATEIMAGEMASK };
			SetItem( &lvi );
			break;
		}	
	case Grid::OptionButtons:
		{
			LVITEM lvi = { LVIF_STATE, nRow, nCol, (1 << 12), LVIS_STATEIMAGEMASK };
			SetItem( &lvi );
			break;
		}	
	case Grid::SwitchableIcons:
		{
			LVITEM lvi = { LVIF_STATE, nRow, nCol, (altImage << 12), LVIS_STATEIMAGEMASK };
			SetItem( &lvi );
			break;
		}	
	case Grid::UpperCase:
	case Grid::UpperCase_Combo:
		{
			CString sText = GetCellText( nRow, nCol );
			sText.MakeUpper();
			SetCellText( nRow, nCol, sText );
			break;
		}	
	case Grid::LowerCase:
	case Grid::LowerCase_Combo:
		{
			CString sText = GetCellText( nRow, nCol );
			sText.MakeLower();
			SetCellText( nRow, nCol, sText );
			break;
		}
	}
}

void CGridCtrl::SetCurCellText( LPCTSTR pszText )
{
	if( mCurrentCell )
		SetCellText( mCurrentCell.row(), mCurrentCell.col(), pszText );
}

int CGridCtrl::GetCellImage( int nRow, int nCol )  
{
	//LV_ITEM lvItem = { LVIF_IMAGE, nRow, nCol };
	//GetItem( &lvItem );
	//return lvItem.iImage;
	const _CellData* pCellData = GetCellData( nRow, nCol );
	if( pCellData )
		return pCellData->midxImage;
	return -1;
}

int CGridCtrl::GetCellAltImage( int nRow, int nCol )  
{
	//LV_ITEM lvItem = { LVIF_IMAGE, nRow, nCol };
	//GetItem( &lvItem );
	//return lvItem.iImage;
	const _CellData* pCellData = GetCellData( nRow, nCol );
	if( pCellData )
		return pCellData->midxAltImage;
	return -1;
}

void CGridCtrl::SetCellImages( int nRow, int nCol, int nImage, int nAltImage/* = -2*/ )  
{
	//LV_ITEM lvItem = { LVIF_IMAGE, nRow, nCol };
	//lvItem.iImage = nImage;	
	//SetItem( &lvItem );
	_CellData& CellData = GetCellDataRef( nRow, nCol );
	CellData.midxImage = nImage;
	if( nAltImage >= -1 )
		CellData.midxAltImage = nAltImage;
	InvalidateCell( nRow, nCol );
}

int CGridCtrl::GetCellUncheckedImage( int nRow, int nCol )
{
	int nImage = -1;
	const _CellData* pCellData = GetCellData( nRow, nCol );
	if( pCellData )
		nImage = pCellData->midxImage;
	else
	{
		const PropVal::TIntArray* prnDefImage = mpTemplate->GetPropertyObject( Prop::ColumnDefaultImages )->GetConstIntArrayPtr();
		if( prnDefImage )
		{
			if( (size_t)nCol < prnDefImage->size() )
				nImage = prnDefImage->at( nCol );
		}
	}
	if( nImage < 0 )
		nImage = 0;
	return nImage;
}

int CGridCtrl::GetCellCheckedImage( int nRow, int nCol )
{
	int nImage = -1;
	const _CellData* pCellData = GetCellData( nRow, nCol );
	if( pCellData )
		nImage = pCellData->midxAltImage;
	else
	{
		const PropVal::TIntArray* prnAltImage = mpTemplate->GetPropertyObject( Prop::ColumnAlternateImages )->GetConstIntArrayPtr();
		if( prnAltImage )
		{
			if( (size_t)nCol < prnAltImage->size() )
				nImage = prnAltImage->at( nCol );
		}
	}
	if( nImage < 0 )
		nImage = 1;
	return nImage;
}

bool CGridCtrl::IsCellChecked( int nRow, int nCol )
{
	return (GetCellState( nRow, nCol ) - 1) == GetCellCheckedImage( nRow, nCol );
}

bool CGridCtrl::SetCellChecked( int nRow, int nCol, bool bChecked )
{
	return (SetCellState( nRow, nCol, (bChecked? GetCellCheckedImage( nRow, nCol ) : GetCellUncheckedImage( nRow, nCol )) + 1 ));
}

bool CGridCtrl::SetCellListData( int nRow, int nCol, const CArray<int, int>& rnImage, const CStringArray& rsList )
{
	_CellData& CellData = GetCellDataRef( nRow, nCol );
	std::vector< int >& rImage = CellData.mrnComboImage;
	rImage.clear();
	for( INT_PTR idx = 0; idx < rnImage.GetCount(); ++idx )
		rImage.push_back( rnImage.GetAt( idx ) );
	std::vector< tstring >& rList = CellData.mrsComboList;
	rList.clear();
	for( INT_PTR idx = 0; idx < rsList.GetCount(); ++idx )
	{
		CString sText = rsList.GetAt( idx );
		switch( GetEffectiveCellStyle( nRow, nCol ) )
		{
		case Grid::UpperCase:
		case Grid::UpperCase_Combo:
			sText.MakeUpper();
			break;
		case Grid::LowerCase:
		case Grid::LowerCase_Combo:
			sText.MakeLower();					
			break;
		}
		rList.push_back( (LPCTSTR)sText );
	}
	return true;
}

bool CGridCtrl::GetCellListData( int nRow, int nCol, CArray<int, int>& rnImage, CStringArray& rsList )
{
	rnImage.RemoveAll();
	rsList.RemoveAll();
	const _CellData* pCellData = GetCellData( nRow, nCol );
	if( !pCellData )
		return true;
	const std::vector< int >& ridxImage = pCellData->mrnComboImage;
	const std::vector< tstring >& rsText = pCellData->mrsComboList;
	for( std::vector< int >::const_iterator iter = ridxImage.begin(); iter != ridxImage.end(); ++iter )
		rnImage.Add( *iter );
	for( std::vector< tstring >::const_iterator iter = rsText.begin(); iter != rsText.end(); ++iter )
		rsList.Add( iter->c_str() );
	return true;
}

bool CGridCtrl::GetCellComboListItems( int nRow, int nCol,
																			 std::vector< tstring >& rsList,
																			 std::vector< int >& ridxImage )
{
	const _CellData* pCellData = GetCellData( nRow, nCol );
	if( pCellData && pCellData->mType != Grid::Undefined )
	{
		ridxImage = pCellData->mrnComboImage;
		rsList = pCellData->mrsComboList;
		return true;
	}
	const PropVal::TCStringArrayList* pItemList =
		mpTemplate->GetPropertyObject( Prop::ColumnListItems )->GetConstStringArrayListPtr();	
	if( pItemList && nCol < (int)pItemList->size() )
	{
		rsList.clear();
		const PropVal::TCStringArray& rStrings = pItemList->at( nCol );
		for( PropVal::TCStringArray::const_iterator iter = rStrings.begin(); iter != rStrings.end(); ++iter )
			rsList.push_back( (LPCTSTR)*iter );
	}
	const PropVal::TIntArrayList* pImageList =
		mpTemplate->GetPropertyObject( Prop::ColumnListImages )->GetConstIntArrayListPtr();
	if( pImageList && nCol < (int)pImageList->size() )
		ridxImage = pImageList->at( nCol );
	return true;
}

int CGridCtrl::GetCellListImage( int nRow, int nCol, LPCTSTR pszListText )
{
	std::vector< tstring > rList;
	std::vector< int > rImage;
	GetCellComboListItems( nRow, nCol, rList, rImage );
	size_t idxText = std::find( rList.begin(), rList.end(), pszListText ) - rList.begin();
	if( idxText < rImage.size() )
		return rImage.at( idxText );
	return -1;
}

CRect CGridCtrl::GetCurSelRect()
{
	if( !mCurrentCell )
		return CRect();
	CRect rcCell = GetCellRect( mCurrentCell.row(), mCurrentCell.col(), LVIR_SELECTBOUNDS );
	if( mCurrentCell.col() < 0 )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		rcCell.left = rcClient.left;
		rcCell.right = rcClient.right;
	}
	return rcCell;
}

CRect CGridCtrl::GetCellRect(	int nRow, int nCol, int area /*= LVIR_BOUNDS*/  )
{
	ASSERT( nRow >= 0 && nRow < GetItemCount() );
	CRect rcCell;
	if( nCol <= 0 )
	{
		GetItemRect( nRow, rcCell, area );
		if( nCol == 0 )
			rcCell.right = GetColumnWidth( 0 );
	}
	else
		GetSubItemRect( nRow, nCol, area, rcCell );
	return rcCell;
}

CRect CGridCtrl::GetCurCellRect( int area /*= LVIR_BOUNDS*/ )
{
	if( !mCurrentCell )
		return CRect();
	return GetCellRect( mCurrentCell.row(), mCurrentCell.col(), area );
}

BOOL CGridCtrl::SetItemData( int nRow, DWORD_PTR dwData )
{
	GetRowDataRef( nRow ).mnItemData = dwData;
	return TRUE;
}

DWORD_PTR CGridCtrl::GetItemData( int nRow ) const
{
	const _RowData* pRowData = GetRowData( nRow );
	if( !pRowData )
		return 0;
	return pRowData->mnItemData;
}

int CGridCtrl::InsertItem( int nRow, LPCTSTR lpszText, int nImageIndex /*= -1*/ )
{
	int nNewItem = __super::InsertItem( nRow, lpszText );
	if( mRowData.size() >= (size_t)nNewItem )
		mRowData.insert( mRowData.begin() + (size_t)nNewItem, _RowData() );
	SetCellImages( nNewItem, 0, nImageIndex );
	InvalidateCell( nRow, -1 );
	return nNewItem;
}

int CGridCtrl::InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat /*= LVCFMT_LEFT*/,
														 int nWidth /*= -1*/, int nImageIndex /*= -1*/ )
{
	if( nCol < 0 )
		return -1;
	CString sHeading = lpszColumnHeading;
	if( nWidth < 0 )
		nWidth = GetStringWidth( lpszColumnHeading ) + 20;
	LVCOLUMN lvColumn = 
	{
		(LVCF_FMT | LVCF_TEXT | LVCF_WIDTH),
		nFormat,
		nWidth,
		sHeading.LockBuffer(),
		sHeading.GetLength() + 1,
		nCol,
		nImageIndex,
	};
	if( nImageIndex >= 0 )
		lvColumn.mask |= LVCF_IMAGE;
	int nRet = __super::InsertColumn( nCol, &lvColumn );
	if( nRet >= 0 )
	{
		++mcColumns;
		size_t ctRows = mRowData.size();
		for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
		{
			_ColumnData& ColumnData = mRowData.at( idxRow ).mCellData;
			if( ColumnData.size() > (size_t)nRet )
				ColumnData.insert( ColumnData.begin() + (size_t)nRet, _CellData( Grid::Runtime ) );
			else
			{
				ColumnData.resize( (size_t)nRet );
				ColumnData.push_back( _CellData( Grid::Runtime ) );
			}
		}
		if( !mbIgnoreChange )
		{
			PropVal::TIntArray* prnWidths = mpTemplate->GetPropertyObject( Prop::ColumnWidths )->GetIntArrayPtr();
			if( prnWidths && prnWidths->size() >= (size_t)nRet )
				prnWidths->insert( prnWidths->begin() + nRet, nWidth );
			PropVal::TCStringArray* prsCaptions = mpTemplate->GetPropertyObject( Prop::ColumnCaptions )->GetStringArrayPtr();	
			if( prsCaptions && prsCaptions->size() >= (size_t)nRet )
				prsCaptions->insert( prsCaptions->begin() + nRet, sHeading );
			PropVal::TIntArray* prnImages = mpTemplate->GetPropertyObject( Prop::ColumnImages )->GetIntArrayPtr();	
			if( prnImages && prnImages->size() >= (size_t)nRet )
				prnImages->insert( prnImages->begin() + nRet, nImageIndex );
			PropVal::TIntArray* prnAlignment = mpTemplate->GetPropertyObject( Prop::ColumnAlignments )->GetIntArrayPtr();	
			if( prnAlignment && prnAlignment->size() >= (size_t)nRet )
				prnAlignment->insert( prnAlignment->begin() + nRet, nFormat );
			PropVal::TIntArray* prnStyles = mpTemplate->GetPropertyObject( Prop::ColumnStyles )->GetIntArrayPtr();	
			if( prnStyles && prnStyles->size() >= (size_t)nRet )
				prnStyles->insert( prnStyles->begin() + nCol, Grid::Runtime );
			PropVal::TIntArray* prnDefImages = mpTemplate->GetPropertyObject( Prop::ColumnDefaultImages )->GetIntArrayPtr();	
			if( prnDefImages && prnDefImages->size() >= (size_t)nRet )
				prnDefImages->insert( prnDefImages->begin() + nCol, -1 );
			PropVal::TIntArray* prnAltImages = mpTemplate->GetPropertyObject( Prop::ColumnAlternateImages )->GetIntArrayPtr();	
			if( prnAltImages && prnAltImages->size() >= (size_t)nRet )
				prnAltImages->insert( prnAltImages->begin() + nCol, -1 );
			PropVal::TCStringArrayList* prnListItems = mpTemplate->GetPropertyObject( Prop::ColumnListItems )->GetStringArrayListPtr();	
			if( prnListItems && prnListItems->size() >= (size_t)nRet )
				prnListItems->insert( prnListItems->begin() + nCol, PropVal::TCStringArray() );
			PropVal::TIntArrayList* prnListImages = mpTemplate->GetPropertyObject( Prop::ColumnListImages )->GetIntArrayListPtr();	
			if( prnListImages && prnListImages->size() >= (size_t)nRet )
				prnListImages->insert( prnListImages->begin() + nCol, PropVal::TIntArray() );
		}
	}
	OnNeedRepaint();
	return nRet;
}

BOOL CGridCtrl::DeleteItem( int nRow )
{
	if( nRow < 0 )
		return FALSE;
	if( !__super::DeleteItem( nRow ) )
		return FALSE;
	if( mCurrentCell.row() == nRow )
		SetCurCell( -1, -1 );
	if( mRowData.size() > (size_t)nRow )
		mRowData.erase( mRowData.begin() + (size_t)nRow );
	return TRUE;
}

BOOL CGridCtrl::DeleteAllItems()
{
	if( !__super::DeleteAllItems() )
		return FALSE;
	SetCurCell( -1, -1 );
	mRowData.clear();
	return TRUE;
}

BOOL CGridCtrl::DeleteColumn( int nCol )
{
	if( nCol < 0 )
		return FALSE;
	if( !__super::DeleteColumn( nCol ) )
		return FALSE;
	if( mCurrentCell.col() == nCol )
		SetCurCell( -1, -1 );
	size_t ctRows = mRowData.size();
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		_ColumnData& ColumnData = mRowData.at( idxRow ).mCellData;
		if( ColumnData.size() > (size_t)nCol )
			ColumnData.erase( ColumnData.begin() + (size_t)nCol );
	}
	if( mcColumns > 0 )
		--mcColumns;
	if( !mbIgnoreChange )
	{
		PropVal::TIntArray* prnWidths = mpTemplate->GetPropertyObject( Prop::ColumnWidths )->GetIntArrayPtr();
		if( prnWidths && prnWidths->size() > (size_t)nCol )
			prnWidths->erase( prnWidths->begin() + nCol );
		PropVal::TCStringArray* prsCaptions = mpTemplate->GetPropertyObject( Prop::ColumnCaptions )->GetStringArrayPtr();	
		if( prsCaptions && prsCaptions->size() > (size_t)nCol )
			prsCaptions->erase( prsCaptions->begin() + nCol );
		PropVal::TIntArray* prnImages = mpTemplate->GetPropertyObject( Prop::ColumnImages )->GetIntArrayPtr();	
		if( prnImages && prnImages->size() > (size_t)nCol )
			prnImages->erase( prnImages->begin() + nCol );
		PropVal::TIntArray* prnAlignment = mpTemplate->GetPropertyObject( Prop::ColumnAlignments )->GetIntArrayPtr();	
		if( prnAlignment && prnAlignment->size() > (size_t)nCol )
			prnAlignment->erase( prnAlignment->begin() + nCol );
		PropVal::TIntArray* prnStyles = mpTemplate->GetPropertyObject( Prop::ColumnStyles )->GetIntArrayPtr();	
		if( prnStyles && prnStyles->size() > (size_t)nCol )
			prnStyles->erase( prnStyles->begin() + nCol );
		PropVal::TIntArray* prnDefImages = mpTemplate->GetPropertyObject( Prop::ColumnDefaultImages )->GetIntArrayPtr();	
		if( prnDefImages && prnDefImages->size() > (size_t)nCol )
			prnDefImages->erase( prnDefImages->begin() + nCol );
		PropVal::TIntArray* prnAltImages = mpTemplate->GetPropertyObject( Prop::ColumnAlternateImages )->GetIntArrayPtr();	
		if( prnAltImages && prnAltImages->size() > (size_t)nCol )
			prnAltImages->erase( prnAltImages->begin() + nCol );
		PropVal::TCStringArrayList* prnListItems = mpTemplate->GetPropertyObject( Prop::ColumnListItems )->GetStringArrayListPtr();	
		if( prnListItems && prnListItems->size() > (size_t)nCol )
			prnListItems->erase( prnListItems->begin() + nCol );
		PropVal::TIntArrayList* prnListImages = mpTemplate->GetPropertyObject( Prop::ColumnListImages )->GetIntArrayListPtr();	
		if( prnListImages && prnListImages->size() > (size_t)nCol )
			prnListImages->erase( prnListImages->begin() + nCol );
	}
	return TRUE;
}

void CGridCtrl::InvalidateCell( int nRow, int nCol ) 
{
	CRect rc = GetCellRect( nRow, nCol );
	InvalidateRect( rc, TRUE );
}

void CGridCtrl::SetupColumns()
{
	mbIgnoreChange = true;
	for( int idxColumn = mcColumns; idxColumn >= 0; --idxColumn )
		DeleteColumn( idxColumn );
	mcColumns = 0;
	const PropVal::TIntArray* prnWidths = mpTemplate->GetPropertyObject( Prop::ColumnWidths )->GetConstIntArrayPtr();	
	const PropVal::TCStringArray* prsCaptions = mpTemplate->GetPropertyObject( Prop::ColumnCaptions )->GetConstStringArrayPtr();	
	const PropVal::TIntArray* prnImages = mpTemplate->GetPropertyObject( Prop::ColumnImages )->GetConstIntArrayPtr();	
	const PropVal::TIntArray* prnAlignment = mpTemplate->GetPropertyObject( Prop::ColumnAlignments )->GetConstIntArrayPtr();	
	size_t idxMax = prsCaptions? prsCaptions->size() : 0;
	if( prnWidths && prnWidths->size() > idxMax )
		idxMax = prnWidths->size();
	if( prnImages && prnImages->size() > idxMax )
		idxMax = prnImages->size();
	if( prnAlignment && prnAlignment->size() > idxMax )
		idxMax = prnAlignment->size();
	for( size_t idxColumn = 0; idxColumn < idxMax; ++idxColumn )
	{
		int nAlignment = HDF_LEFT;
		if( prnAlignment && idxColumn < prnAlignment->size() )
		{
			switch( prnAlignment->at( idxColumn ) )
			{					
			case 1:
				nAlignment = HDF_CENTER;
				break;
			case 2:
				nAlignment = HDF_RIGHT;
				break;				
			default:
				nAlignment = HDF_LEFT;
				break;
			}
		}
		CString sCaption;
		if( prsCaptions && idxColumn < prsCaptions->size() )
			sCaption = prsCaptions->at( idxColumn );
		InsertColumn( idxColumn, sCaption, nAlignment,
									((prnWidths && idxColumn < prnWidths->size())? prnWidths->at( idxColumn ) : 50),
									((prnImages && idxColumn < prnImages->size())? prnImages->at( idxColumn ) : -1) );
	}
	mbIgnoreChange = false;
}

void CGridCtrl::HideEditControls()
{
	if( mpCellEditCtrl )
		OnEndEditCurCell();
}

void CGridCtrl::OnEditCurCell()
{
	HideEditControls();
	if( !mCurrentCell || (mbHasRowHeader && mCurrentCell.col() == 0) )
	{
		SetCurCell( mCurrentCell.row(), -1 );
		return;
	}
	mpCellEditCtrl = CreateEditControl( mCurrentCell.row(), mCurrentCell.col() );
	NMLVDISPINFO lvdi = { m_hWnd, GetControlId(), LVN_BEGINLABELEDIT, 0, mCurrentCell.row(), mCurrentCell.col(), };
	CWnd* pParent = GetParent();
	if( pParent )
		pParent->SendMessage( WM_NOTIFY, (WPARAM)GetControlId(), (LPARAM)&lvdi );
}

void CGridCtrl::OnEndEditCurCell()
{
	CGridCellEditCtrl* pCellEditCtrl = mpCellEditCtrl;
	mpCellEditCtrl = NULL;
	NMLVDISPINFO lvdi = { m_hWnd, GetControlId(), LVN_ENDLABELEDIT, 0, mCurrentCell.row(), mCurrentCell.col(), };
	CWnd* pParent = GetParent();
	if( pParent )
		pParent->SendMessage( WM_NOTIFY, (WPARAM)GetControlId(), (LPARAM)&lvdi );
	delete pCellEditCtrl;
}

CGridCellEditCtrl* CGridCtrl::CreateEditControl( int nRow, int nCol )
{
	switch( GetEffectiveCellStyle( nRow, nCol ) )
	{
		case Grid::CheckBoxes: return new CToggleEditCtrl( this, nRow, nCol );
		case Grid::OptionButtons: return new CRadioEditCtrl( this, nRow, nCol );
		case Grid::SwitchableIcons: return new CToggleEditCtrl( this, nRow, nCol );
		case Grid::EllipsesButtons: return new CButtonEditCtrl( this, nRow, nCol, _T("..."), ID_CELLBUTTON );
		case Grid::PickButtons: return new CButtonEditCtrl( this, nRow, nCol, IDI_PICKSMALL, ID_CELLBUTTON );
		case Grid::Strings: return new CTextBoxEditCtrl( this, nRow, nCol );
		case Grid::AngleUnits: return new CTextBoxEditCtrl( this, nRow, nCol, new CAngleFilter );
		case Grid::Integers: return new CTextBoxEditCtrl( this, nRow, nCol, new CIntegerFilter );
		case Grid::Units: return new CTextBoxEditCtrl( this, nRow, nCol, new CNumericFilter );
		case Grid::UpperCase: return new CTextBoxEditCtrl( this, nRow, nCol, new CUpperCaseFilter );
		case Grid::LowerCase: return new CTextBoxEditCtrl( this, nRow, nCol, new CLowerCaseFilter );
		case Grid::Password: return new CTextBoxEditCtrl( this, nRow, nCol, new CPasswordFilter );
		case Grid::MultiLine: return new CTextBoxEditCtrl( this, nRow, nCol, new CMultilineFilter );
		case Grid::Currency: return new CTextBoxEditCtrl( this, nRow, nCol, new CCurrencyFilter );
		case Grid::Date: return new CDateTimeEditCtrl( this, nRow, nCol, true );
		case Grid::Time: return new CDateTimeEditCtrl( this, nRow, nCol, false );
		case Grid::Percentage: return new CTextBoxEditCtrl( this, nRow, nCol, new CPercentageFilter );
		case Grid::DropDown: return new CComboDropdownListEditCtrl( this, nRow, nCol );
		case Grid::ArrowHead:
		case Grid::AcadColors:
		case Grid::TextStyleList:
		case Grid::PlotStyleNames:
		case Grid::PlotStyleTables:
		case Grid::PlotterList:
		case Grid::Fonts:
			return new CComboDropdownListEditCtrl( this, nRow, nCol );
		case Grid::DriveList: return new CDriveComboDropdownListEditCtrl( this, nRow, nCol );
		case Grid::LayerList:
		case Grid::DimStyleList:
			return new CComboDropdownListEditCtrl( this, nRow, nCol );
		case Grid::ImageDropList: return new CImageComboDropdownListEditCtrl( this, nRow, nCol );
		case Grid::AcadColorCell: return new CTextBoxEditCtrl( this, nRow, nCol );
		case Grid::TrueColorCell: return new CTextBoxEditCtrl( this, nRow, nCol );
		case Grid::LineWeightCell: return new CTextBoxEditCtrl( this, nRow, nCol );
		case Grid::LinetypeCell: return new CTextBoxEditCtrl( this, nRow, nCol );
		case Grid::DirectoryCell: return new CButtonEditCtrl( this, nRow, nCol, IDI_FOLDER, ID_CELLBUTTON );
		case Grid::DwgFilesCell: return new CButtonEditCtrl( this, nRow, nCol, IDI_FOLDER, ID_CELLBUTTON );
		case Grid::Strings_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol );
		case Grid::AngleUnits_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CComboFilter( new CAngleFilter ) );
		case Grid::Integers_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CComboFilter( new CIntegerFilter ) );
		case Grid::Units_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol );
		case Grid::UpperCase_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CComboFilter( new CUpperCaseFilter ) );
		case Grid::LowerCase_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CComboFilter( new CLowerCaseFilter ) );
		case Grid::Symbols: return new CTextBoxEditCtrl( this, nRow, nCol, new CSymbolNameFilter );
		case Grid::Symbols_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CComboFilter( new CSymbolNameFilter ) );
		//default: return new CTextBoxEditCtrl( this, nRow, nCol );
	}
	return NULL;
}

void CGridCtrl::MoveUp() 
{
	int nRow = mCurrentCell.row();
	if( nRow <= 0 )
		return;
	SetCurCell( nRow - 1, mCurrentCell.col() );
}

void CGridCtrl::MoveDown() 
{
	int nRow = mCurrentCell.row() + 1;
	if( nRow >= GetItemCount() )
		return;
	SetCurCell( nRow, mCurrentCell.col() );
}

void CGridCtrl::MoveLeft() 
{
	int nCol = mCurrentCell.col();
	if( nCol <= 0 )
		return;
	if( nCol == 1 && mbHasRowHeader )
		return;
	SetCurCell( mCurrentCell.row(), nCol - 1 );
}

void CGridCtrl::MoveRight() 
{
	int nCol = mCurrentCell.col() + 1;
	if( nCol <= (mbHasRowHeader? 1 : 0) )
		return;
	if( (size_t)nCol >= mcColumns )
		return;
	SetCurCell( mCurrentCell.row(), nCol );
}

bool CGridCtrl::CellHitTest( const CPoint& point, int& nRow, int& nCol )
{
	for( int idxRow = GetItemCount() - 1; idxRow >= 0; --idxRow )
	{
		CRect rcRow;
		GetItemRect( idxRow, &rcRow, LVIR_BOUNDS );
		if( rcRow.PtInRect( point ) )
		{
			CPoint ptTest( point );
			for( int idxColumn = mcColumns - 1; idxColumn >= 0; --idxColumn )
			{
				if( GetCellRect( idxRow, idxColumn, LVIR_BOUNDS ).PtInRect( ptTest ) )
				{
					nRow = idxRow;
					nCol = idxColumn;
					return true;
				}
			}
		}
	}
	return false;
}

void CGridCtrl::SetCellTextImage( int nRow, int nCol, LPCTSTR pszText, int nImage )  
{
	if( nRow < 0 || nCol < ((nImage >= 0)? 1 : 0) ) //CListCtrl won't allow images in the first column
		return;
	SetCellText( nRow, nCol, pszText );
	SetCellImages( nRow, nCol, nImage );
}

UINT CGridCtrl::GetCellState( int nRow, int nCol )
{
	LVITEM lv = { LVIF_STATE, nRow, nCol, 0, LVIS_STATEIMAGEMASK, };
	if( !GetItem( &lv ) )
		return 0;
	return (lv.state >> 12);
}

bool CGridCtrl::SetCellState( int nRow, int nCol, UINT nState )
{
	LVITEM lv = { LVIF_STATE, nRow, nCol, nState << 12, LVIS_STATEIMAGEMASK, };
	return (SetItem( &lv ) != FALSE);
}

bool CGridCtrl::ToggleCellState( int nRow, int nCol )
{
	int nCheckedImage = GetCellCheckedImage( nRow, nCol ) + 1;
	int nUncheckedImage = GetCellUncheckedImage( nRow, nCol ) + 1;
	LVITEM lv = { LVIF_STATE, nRow, nCol, 0, LVIS_STATEIMAGEMASK, };
	if( !GetItem( &lv ) )
		return false;
	if( (lv.state >> 12) != nCheckedImage )
		lv.state = (nCheckedImage << 12);
	else
		lv.state = (nUncheckedImage << 12);
	if( !SetItem( &lv ) )
		return false;
	return true;
}

void CGridCtrl::DrawCell( int nRow, int nCol, const CRect& rectCell, CDC& cdc )
{
	Grid::CellStyle nCellStyle = GetEffectiveCellStyle( nRow, nCol );
	LV_ITEM lvi = { LVIF_STATE, nRow, nCol, 0, (UINT)-1, };
  GetItem( &lvi );
	bool bHighlight = false;
	//bool bHighlight = ((GetFocus() == this || (GetStyle() & LVS_SHOWSELALWAYS)) &&
	//									 ((lvi.state & LVIS_DROPHILITED) || (lvi.state & LVIS_SELECTED)));

  CRect rcBounds = GetCellRect( nRow, nCol, LVIR_BOUNDS );
	CRect rcLabel = GetCellRect( nRow, nCol, LVIR_LABEL ); 
	CRect rcIcon = GetCellRect( nRow, nCol, LVIR_ICON );
  CString sLabel = GetCellText( nRow, nCol );

	bool bWordWrap = (rcLabel.Height() >= ((cdc.GetTextExtent( _T(" "), 1 ).cy * 2) + 1));

	COLORREF crBackground = mColorService.GetBackgroundColor();
	if( !mbAlternateColumnColors )
	{
		if( (nRow % 2) != 0 )
			crBackground = mclrAlternate;
	}
	else
	{
		int nStart = mbHasRowHeader? 1 : 0;
		if( (nCol % 2) != nStart )
			crBackground = mclrAlternate;
	}

	//Draw cell background and border
	if( nCol == 0 && mbHasRowHeader )
	{
		cdc.SetBkColor( ::GetSysColor( COLOR_BTNFACE ) );
		cdc.SetTextColor( ::GetSysColor( COLOR_BTNTEXT ) );
		CRect rcHeader = rcBounds;
		HTHEME hTheme = NULL;
		CThemeHelperST* pTheme = GetThemeHelper();
		if( pTheme )
			hTheme = pTheme->OpenThemeData( GetSafeHwnd(), L"HEADER" );
		if( !hTheme )
		{			
			CBrush br( ::GetSysColor( COLOR_BTNFACE ) );
			cdc.FillRect( &rcHeader, &br );
			cdc.DrawEdge( &rcHeader, BDR_RAISEDINNER, BF_RECT );
		}    
		else
		{
			pTheme->DrawThemeBackground( hTheme, m_hWnd, cdc.GetSafeHdc(), HP_HEADERITEM, 0, &rcHeader, NULL );
			pTheme->CloseThemeData( hTheme );
			CPen penHighlight( PS_SOLID, 1, ::GetSysColor( COLOR_BTNHIGHLIGHT ) );
			CPen* pOldPen = cdc.SelectObject( &penHighlight );
			cdc.MoveTo( rcHeader.left, rcHeader.top );
			cdc.LineTo( rcHeader.right, rcHeader.top );
			cdc.SelectObject( pOldPen );
		}
		cdc.SetBkColor( crBackground );
		cdc.SetTextColor( mColorService.GetForegroundColor() ); 
	}
	else
	{
		if( bHighlight )
		{
			CBrush br( ::GetSysColor( COLOR_HIGHLIGHT ) );
			cdc.FillRect( rcBounds, &br );
			cdc.SetBkColor( ::GetSysColor( COLOR_HIGHLIGHT ) );
			cdc.SetTextColor( ::GetSysColor( COLOR_HIGHLIGHTTEXT ) );
		}
		else
		{
			CBrush br( crBackground );
			cdc.FillRect( rcBounds, &br ); 
			cdc.SetBkColor( crBackground );
			cdc.SetTextColor( mColorService.GetForegroundColor() ); 
		}
		if (mbHasGridLines)
		{
			CPen penBackground( PS_SOLID, 1, ::GetSysColor( COLOR_BTNFACE ) );
			CPen* pOldPen = cdc.SelectObject( &penBackground );
			cdc.MoveTo( rcBounds.left, rcBounds.bottom - 1 );
			cdc.LineTo( rcBounds.right, rcBounds.bottom - 1 );
			cdc.LineTo( rcBounds.right, rcBounds.top );
			cdc.SelectObject( pOldPen );
		}
	}

	//Draw icon if necessary
	cdc.SetBkMode( TRANSPARENT );
	switch( nCellStyle )
	{
	case Grid::CheckBoxes:
		{
			rcIcon += CSize( 4, 4 );
			rcIcon.right = rcIcon.left + 14;
			rcIcon.bottom = rcIcon.top + 14;
			if( sLabel.IsEmpty() )
				rcIcon.MoveToX( rcBounds.left + (rcBounds.Width() - 14) / 2 ); //center the icon in an empty cell
			bool bChecked = (((lvi.state & LVIS_STATEIMAGEMASK) >> 12) > 1);
			DrawCheckBox( cdc, rcIcon, bChecked, bHighlight );
			rcLabel.left = rcIcon.right + 4; //shift label rect to leave space for image
		}
		break;
	case Grid::OptionButtons:
		{
			rcIcon += CSize( 4, 4 );
			rcIcon.right = rcIcon.left + 14;
			rcIcon.bottom = rcIcon.top + 14;
			if( sLabel.IsEmpty() )
				rcIcon.MoveToX( rcBounds.left + (rcBounds.Width() - 14) / 2 ); //center the icon in an empty cell
			bool bChecked = (((lvi.state & LVIS_STATEIMAGEMASK) >> 12) > 1);
			DrawOptionButton( cdc, rcIcon, bChecked, bHighlight );
			rcLabel.left = rcIcon.right + 4; //shift label rect to leave space for image
		}
		break;
	case Grid::ArrowHead:
		{
			rcIcon += CSize( 4, 4 );
			rcIcon.right = rcIcon.left + 14;
			rcIcon.bottom = rcIcon.top + 14;
			int nCellImage = GetCellImage( nRow, nCol );
			DrawArrow( cdc, rcIcon, nCellImage, sLabel );
			rcLabel.left = rcIcon.right + 4; //shift label rect to leave space for image
		}
		break;
	case Grid::AcadColors:
		{
			rcIcon += CSize( 4, 4 );
			rcIcon.right = rcIcon.left + 14;
			rcIcon.bottom = rcIcon.top + 14;
			int nCellImage = GetCellImage( nRow, nCol );
			DrawColor( cdc, rcIcon, nCellImage, sLabel );
			rcLabel.left = rcIcon.right + 4; //shift label rect to leave space for image
		}
		break;
	case Grid::Fonts:
		{
			rcIcon += CSize( 4, 4 );
			rcIcon.right = rcIcon.left + 14;
			rcIcon.bottom = rcIcon.top + 14;
			int nFontImage = (GetCellText( nRow, nCol ).Right( 4 ) == _T(".shx")? 1 : 0);
			DrawFontIcons( cdc, rcIcon, nFontImage, sLabel );
			rcLabel.left = rcIcon.right + 4; //shift label rect to leave space for image
		}
		break;
	case Grid::AcadColorCell:
	case Grid::TrueColorCell:
		{
			rcIcon += CSize( 4, 4 );
			rcIcon.right = rcIcon.left + 14;
			rcIcon.bottom = rcIcon.top + 14;
			int nCellImage = GetCellImage( nRow, nCol );
			DrawColor( cdc, rcIcon, nCellImage, sLabel );
			rcLabel.left = rcIcon.right + 4; //shift label rect to leave space for image
		}
		break;
	case Grid::LineWeightCell:
		{
			rcIcon += CSize( 4, 4 );
			rcIcon.right = rcIcon.left + 48;
			rcIcon.bottom = rcIcon.top + 14;
			int nCellImage = GetCellImage( nRow, nCol );;
			DrawLineWeight( cdc, rcIcon, nCellImage, sLabel );
			rcLabel.left = rcIcon.right + 1; //shift label rect to leave space for image
		}
		break;
	default:
		{
			int nCellImage = GetCellImage( nRow, nCol );;
			if( nCellStyle == Grid::SwitchableIcons )
			{
				nCellImage = ((lvi.state & LVIS_STATEIMAGEMASK) >> 12) - 1;
				int nCheckedImage = GetCellCheckedImage( nRow, nCol );
				int nUncheckedImage = GetCellUncheckedImage( nRow, nCol );
				if( nCheckedImage >= 0 && nCellImage != nCheckedImage )
					nCellImage = nUncheckedImage;
			}

			if( nCellImage > -1 ) 
			{
				CImageList* pImageList = GetImageList( LVSIL_SMALL );
				if( pImageList ) 
				{
					IMAGEINFO inf;
					pImageList->GetImageInfo( nCellImage, &inf );
					rcIcon.right = rcIcon.left + inf.rcImage.right - inf.rcImage.left;
					rcIcon.bottom = rcIcon.top + inf.rcImage.bottom - inf.rcImage.top;
					if( sLabel.IsEmpty() )
						rcIcon.MoveToX( rcBounds.left + (rcBounds.Width() - rcIcon.Width()) / 2 ); //center the icon in an empty cell
					pImageList->Draw( &cdc,
														nCellImage, CPoint( rcIcon.left + 1, rcIcon.top + 1 ),
														(bHighlight? ILD_BLEND50 : 0) | ILD_TRANSPARENT | (lvi.state & LVIS_OVERLAYMASK) ); 
				}
				rcLabel.left = rcIcon.right + 4; //shift label rect to leave space for image
			}
			else
				rcLabel.left = rcIcon.left;
		}
		break;
	}

	if( !sLabel.IsEmpty() )
	{
		rcLabel.top += 4;
		rcLabel.left += 4;
		cdc.SetBkMode( TRANSPARENT );
		UINT fWordBreak = (bWordWrap? DT_WORDBREAK : (DT_SINGLELINE | DT_END_ELLIPSIS));
		if( nCellStyle == Grid::Password )
		{
			int cchLabel = sLabel.GetLength();
			sLabel.Empty();
			for( int cch = cchLabel; cch > 0; --cch )
				sLabel += _T('*');
		}
    UINT fAlignment = 0;
		const PropVal::TIntArray* pColAlignments = mpTemplate->GetPropertyObject( Prop::ColumnAlignments )->GetConstIntArrayPtr();
		if( pColAlignments && nCol >= 0 && (size_t)nCol < pColAlignments->size() )
			fAlignment |= pColAlignments->at( nCol );
		cdc.DrawText( sLabel, -1, &rcLabel, (DT_NOPREFIX | DT_NOCLIP | fAlignment | fWordBreak) );
	}

	//Draw focus rect if cell is current cell
	if( mCurrentCell.row() == nRow && mCurrentCell.col() == nCol )
	{
		CRect rcFocus = rcBounds;
		rcFocus.DeflateRect( 1, 1 );
		cdc.DrawFocusRect( &rcFocus );
	}
}

void CGridCtrl::DrawOptionButton( CDC& cdc, const CRect& rcIcon, bool bPressed, bool bHighlight )
{
	CRect rc = rcIcon;
	rc.right = rc.left + 14;
	if( rc.right > rcIcon.right )
		rc.right = rcIcon.right;
	rc.bottom = rc.top + 14;
	if( rc.bottom > rcIcon.bottom )
		rc.bottom = rcIcon.bottom;
	//CThemeHelperST* pTheme = GetThemeHelper();
	//HTHEME hTheme = pTheme? pTheme->OpenThemeData(GetSafeHwnd(), L"BUTTON") : NULL;
	//if (hTheme != NULL)
	//{
	//	// draw the round option button
	//	if( bPressed )
	//		pTheme->DrawThemeBackground( hTheme, m_hWnd, cdc.GetSafeHdc(), BP_RADIOBUTTON, 
	//																 (bSelected? RBS_CHECKEDNORMAL : RBS_CHECKEDNORMAL), &rc, NULL );
	//	else
	//		pTheme->DrawThemeBackground(hTheme, m_hWnd, cdc.GetSafeHdc(),
	//			BP_RADIOBUTTON, 
	//			(bSelected ? RBS_UNCHECKEDNORMAL : RBS_UNCHECKEDNORMAL), &rc, NULL);
	//	pTheme->CloseThemeData(hTheme);

	//	//// draw top left corner, this is required to paint of the
	//	//// greyish background because the option button is round but is 
	//	//// drawn as a square.
	//	//CPen penBackground( PS_SOLID, 1, pDC->GetBkColor() );
	//	//CPen* pOldPen = pDC->SelectObject( &penBackground );
	//	//int nX = rc.left;
	//	//int nY = rc.top;
	//	//pDC->MoveTo( nX, nY );
	//	//pDC->LineTo( nX + 4 + bSelected, nY );
	//	//pDC->MoveTo( nX, nY );
	//	//pDC->LineTo( nX, nY + 4 + bSelected );
	//	//pDC->SetPixel( nX + 1, nY + 1, pDC->GetBkColor() );
	//	//pDC->SetPixel( nX + 1 + bSelected, nY + 1, pDC->GetBkColor() );
	//	//pDC->SetPixel( nX + 1, nY + 1 + bSelected, pDC->GetBkColor() );
	//	//pDC->MoveTo( nX + 9 - bSelected, nY );
	//	//pDC->LineTo( nX + 16, nY );
	//	//pDC->MoveTo( nX + 11 - bSelected, nY + 1 );
	//	//pDC->LineTo( nX + 16, nY + 1 );
	//	//pDC->MoveTo( nX + 13 - bSelected, nY + 2 );
	//	//pDC->LineTo( nX + 16, nY + 2 );
	//	//pDC->MoveTo( nX + 13, nY + 2 );
	//	//pDC->LineTo( nX + 13, nY + 5 + bSelected );
	//	//pDC->MoveTo( nX + 12, nY + 2 - bSelected );
	//	//pDC->LineTo( nX + 12, nY + 4 + bSelected );
	//	//if (bSelected)
	//	//	pDC->SetPixel( nX + 11, nY + 2, pDC->GetBkColor() );
	//	//nY += 13;
	//	//pDC->MoveTo( nX, nY - 2 );
	//	//pDC->LineTo( nX, nY - 5 - bSelected );
	//	//pDC->MoveTo( nX, nY );
	//	//pDC->LineTo( nX + 5 + bSelected, nY );
	//	//pDC->MoveTo( nX, nY - 1 );
	//	//pDC->LineTo( nX + 4 + bSelected, nY - 1 );
	//	//pDC->MoveTo( nX, nY - 2 );
	//	//pDC->LineTo( nX + 2 + bSelected, nY - 2 );
	//	//if (bSelected)
	//	//	pDC->SetPixel( nX + 1, nY - 3, pDC->GetBkColor() );
	//	//nX += 13;
	//	//pDC->MoveTo( nX, nY );
	//	//pDC->LineTo( nX - 10 - bSelected, nY );
	//	//pDC->MoveTo( nX, nY - 1 );
	//	//pDC->LineTo( nX - 5 - bSelected, nY - 1 );
	//	//pDC->MoveTo( nX, nY - 2 );
	//	//pDC->LineTo( nX - 3 - bSelected, nY - 2 );
	//	//pDC->MoveTo( nX, nY - 3 );
	//	//pDC->LineTo( nX, nY - 10 - bSelected );
	//	//pDC->MoveTo( nX - 1, nY - 3 );
	//	//pDC->LineTo( nX - 1, nY - 5 - bSelected );
	//	//if (bSelected)
	//	//	pDC->SetPixel( nX - 2, nY - 3, pDC->GetBkColor() );
	//	//pDC->SelectObject( &pOldPen );
	//}
	//else
	{
		mOptionButtonImageList.Draw( &cdc, (bPressed? 1 : 0), rc.TopLeft(), ILD_TRANSPARENT );
	}
}

void CGridCtrl::DrawCheckBox( CDC& cdc, const CRect& rcIcon, bool bPressed, bool bHighlight )
{
	CRect rc = rcIcon;
	rc.right = rc.left + 14;
	if( rc.right > rcIcon.right )
		rc.right = rcIcon.right;
	rc.bottom = rc.top + 14;
	if( rc.bottom > rcIcon.bottom )
		rc.bottom = rcIcon.bottom;
	CThemeHelperST* pTheme = GetThemeHelper();
	HTHEME hTheme = pTheme? pTheme->OpenThemeData(GetSafeHwnd(), L"BUTTON") : NULL;
	if( hTheme )
	{
		pTheme->DrawThemeBackground( hTheme, m_hWnd, cdc.GetSafeHdc(), BP_CHECKBOX,
																 (bPressed? RBS_CHECKEDNORMAL : RBS_UNCHECKEDNORMAL), &rc, NULL );
		pTheme->CloseThemeData( hTheme );
	}
	else
	{
		cdc.DrawEdge( &rc, EDGE_SUNKEN, BF_RECT );
		rc.DeflateRect( 2, 2 );
		CBrush brFill( cdc.GetBkColor() );
		cdc.FillRect( &rc, &brFill );

		if( bPressed )
		{
			rc.InflateRect( 4, 4 );
			CPen penBlack( PS_SOLID, 1, RGB(0,0,0) );
			CPen* pOldPen = cdc.SelectObject( &penBlack );
			cdc.MoveTo( rc.left + 5, rc.top + 7 );
			cdc.LineTo( rc.left + 5, rc.top + 10 );
			cdc.MoveTo( rc.left + 6, rc.top + 8 );
			cdc.LineTo( rc.left + 6, rc.top + 11 );
			cdc.MoveTo( rc.left + 7, rc.top + 9 );
			cdc.LineTo( rc.left + 7, rc.top + 12 );
			cdc.MoveTo( rc.left + 8, rc.top + 8 );
			cdc.LineTo( rc.left + 8, rc.top + 11 );
			cdc.MoveTo( rc.left + 9, rc.top + 7 );
			cdc.LineTo( rc.left + 9, rc.top + 10 );
			cdc.MoveTo( rc.left + 10, rc.top + 6 );
			cdc.LineTo( rc.left + 10, rc.top + 9 );
			cdc.MoveTo( rc.left + 11, rc.top + 5 );
			cdc.LineTo( rc.left + 11, rc.top + 8 );
			cdc.SelectObject( &pOldPen );
		}
	}
}

void CGridCtrl::DrawColor( CDC& cdc, const CRect& rcIcon, int nColor, const CString& sText )
{
	COLORREF crFill = RGB(255,255,255);
	if( nColor >= 0 && nColor <= 256 )
		crFill = GetRGBColor( nColor );
	else if( !sText.IsEmpty() )					
	{
		int idxComma = sText.Find( _T(",") );
		if( idxComma < 0 )
			crFill = GetRGBColor( _tstol( sText ) );
		else
		{
			CString sRed = sText.Left( idxComma );
			CString sGreen = sText.Mid( idxComma + 1 );
			idxComma = sGreen.Find( _T(",") );
			CString sBlue = sGreen.Mid( idxComma + 1 );
			sGreen = sGreen.Left( idxComma );
			crFill = RGB(_tstol( sRed ), _tstol( sGreen ), _tstol( sBlue ) );
		}
	}
	cdc.Rectangle( &rcIcon );
	CRect rcFill = rcIcon;
	rcFill.DeflateRect( 1, 1 );
	CBrush brFill( crFill );
	cdc.FillRect( &rcFill, &brFill );
}

void CGridCtrl::DrawArrow( CDC& cdc, const CRect& rcIcon, int nArrow, const CString& sText )
{
	if( nArrow < 0 || nArrow > 20 )
		return;
	if( !mArrowImageList.m_hImageList )
	{
		HINSTANCE hOldRes = AfxGetResourceHandle();
		AfxSetResourceHandle( theWorkspace.GetLocalResourceModule() );
		mArrowImageList.Create( IDB_ARROWHEADS, 11, 1, CLR_NONE );
		AfxSetResourceHandle( hOldRes );
	}
	mArrowImageList.Draw( &cdc, nArrow, rcIcon.TopLeft(), ILD_TRANSPARENT );
}

void CGridCtrl::DrawFontIcons( CDC& cdc, const CRect& rcIcon, int nImage, const CString& sText )
{
	if( nImage < 0 || nImage > 1 )
		return;
	if( !mFontImageList.m_hImageList )
	{
		HMODULE hmodRes = theWorkspace.GetLocalResourceModule();
		mFontImageList.Create( 15, 13, ILC_COLOR32 | ILC_MASK, 2, 1 );
		mFontImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE(IDI_TTFONT) ) );
		mFontImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE(IDI_SHXFONT) ) );
	}
	mFontImageList.Draw( &cdc, nImage, rcIcon.TopLeft(), ILD_TRANSPARENT );
}

void CGridCtrl::DrawLineWeight( CDC& cdc, const CRect& rcIcon, int LW, const CString& sText )
{
	if( LW < 30 )
	{
		CPen penLW( PS_SOLID, 1, cdc.GetTextColor() );
		CPen* pOldPen = cdc.SelectObject( &penLW );
		cdc.MoveTo( rcIcon.left, rcIcon.top + 6 );
		cdc.LineTo( rcIcon.right, rcIcon.top + 6 );
		cdc.SelectObject( pOldPen );
		return;
	}
	CRect rc( rcIcon );
	switch( LW )
	{
		case 30:
		case 35:
		{
			rc.top += 5;
			rc.bottom = rc.top + 3;
			break;
		}
		case 40:
		{
			rc.top += 5;
			rc.bottom = rc.top + 4;	
			break;
		}
		case 50:
		case 53:
		{
			rc.top += 4;
			rc.bottom = rc.top + 5;
			break;
		}
		case 60:
		{
			rc.top += 4;
			rc.bottom = rc.top + 6;
			break;
		}
		case 70:
		{
			rc.top += 3;
			rc.bottom = rc.top + 7;
			break;
		}
		case 80:
		{
			rc.top += 3;
			rc.bottom = rc.top + 8;
			break;
		}
		case 90:
		{
			rc.top += 3;
			rc.bottom = rc.top + 9;
			break;
		}
		case 100:
		case 106:
		{
			rc.top += 3;
			rc.bottom = rc.top + 10;
			break;
		}
		case 120:
		{
			rc.top += 2;
			rc.bottom = rc.top + 12;
			break;
		}
		default:
		{
			rc.bottom = rc.top + 14;
			break;
		}
	}
	CBrush brFill( cdc.GetTextColor() );
	cdc.FillRect( &rc, &brFill );
}

bool CGridCtrl::SortTextItems( int nCol, bool bAscending )
{
	if( nCol < 0 || (size_t)nCol >= mcColumns )
		return false;
	size_t ctRows = mRowData.size();
	int ctItems = GetItemCount();
	if( (int)ctRows > ctItems )
		ctRows = ctItems;
	if( ctRows <= 1 )
		return true;
	std::vector< UINT > rnSortXForm;
	rnSortXForm.resize( ctRows );
	size_t idx = ctRows;
	while( idx-- > 0 )
		rnSortXForm[idx] = idx;
	bool bAlreadySorted = true;
	bool bDoneSorting = false;
	while( !bDoneSorting )
	{
		bDoneSorting = true;
		for( size_t idxRow = 1; idxRow < ctRows; ++idxRow )
		{
			size_t idxLo = rnSortXForm[idxRow - 1];
			size_t idxHi = rnSortXForm[idxRow];
			CString sHi = GetCellText( idxHi, nCol );
			CString sLo = GetCellText( idxLo, nCol );
			bool bOrdered = true;
			if( bAscending )
			{
				if( sLo > sHi )
					bOrdered = false;
			}
			else
			{
				if( sHi > sLo )
					bOrdered = false;
			}
			if( !bOrdered )
			{
				bDoneSorting = false;
				bAlreadySorted = false;
				rnSortXForm[idxRow] = idxLo;
				rnSortXForm[idxRow - 1] = idxHi;
			}
		}
	}
	if( bAlreadySorted )
		return true; //nothing to do
	SetRedraw( FALSE );
	std::vector< _RowData > rTempRowData;
	rTempRowData.swap( mRowData );
	typedef std::vector< TCellState > TRowState;
	std::vector< TRowState > rGridState;
	rGridState.resize( ctRows );
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		TRowState& RowState = rGridState[idxRow];
		RowState.resize( mcColumns );
		for( size_t idxCol = 0; idxCol < mcColumns; ++idxCol )
		{
			TCellState& CellState = RowState[idxCol];
			CellState.text = GetCellText( idxRow, idxCol );
			CellState.state = GetCellState( idxRow, idxCol );
			CellState.data = GetCellDataRef( idxRow, idxCol );
		}
	}
	mRowData.resize( ctRows );
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		size_t idxOldRow = rnSortXForm[idxRow];
		TRowState& RowState = rGridState[idxOldRow];
		for( size_t idxCol = 0; idxCol < mcColumns; ++idxCol )
		{
			TCellState& CellState = RowState[idxCol];
			SetCellText( idxRow, idxCol, CellState.text );
			SetCellState( idxRow, idxCol, CellState.state );
			GetCellDataRef( idxRow, idxCol ) = CellState.data;
		}
		mRowData[idxRow] = rTempRowData[idxOldRow];
	}
	SetRedraw( TRUE );
	OnNeedRepaint();
	return true;
}

bool CGridCtrl::SortNumericItems( int nCol, bool bAscending )
{
	if( nCol < 0 || (size_t)nCol >= mcColumns )
		return false;
	size_t ctRows = mRowData.size();
	int ctItems = GetItemCount();
	if( (int)ctRows > ctItems )
		ctRows = ctItems;
	if( ctRows <= 1 )
		return true;
	std::vector< size_t > rnSortXForm;
	rnSortXForm.resize( ctRows );
	size_t idx = ctRows;
	while( idx-- > 0 )
		rnSortXForm[idx] = idx;
	bool bAlreadySorted = true;
	bool bDoneSorting = false;
	while( !bDoneSorting )
	{
		bDoneSorting = true;
		for( size_t idxRow = 1; idxRow < ctRows; ++idxRow )
		{
			size_t idxLo = rnSortXForm[idxRow - 1];
			size_t idxHi = rnSortXForm[idxRow];
			CString sLo = GetCellText( idxLo, nCol );
			CString sHi = GetCellText( idxHi, nCol );
			long nLo = GetCellState( idxLo, nCol );
			long nHi = GetCellState( idxHi, nCol );
			if( nLo == nHi )
			{
				nLo = GetCellImage( idxLo, nCol );
				nHi = GetCellImage( idxHi, nCol );
				if( nLo == nHi )
				{
					nLo = _tstol( GetCellText( idxLo, nCol ) );
					nHi = _tstol( GetCellText( idxHi, nCol ) );
				}
			}
			bool bOrdered = true;
			if( bAscending )
			{
				if( nLo > nHi )
					bOrdered = false;
			}
			else
			{
				if( nHi > nLo )
					bOrdered = false;
			}
			if( !bOrdered )
			{
				bDoneSorting = false;
				bAlreadySorted = false;
				rnSortXForm[idxRow] = idxLo;
				rnSortXForm[idxRow - 1] = idxHi;
			}
		}
	}
	if( bAlreadySorted )
		return true; //nothing to do
	SetRedraw( FALSE );
	std::vector< _RowData > rTempRowData;
	rTempRowData.swap( mRowData );
	typedef std::vector< TCellState > TRowState;
	std::vector< TRowState > rGridState;
	rGridState.resize( ctRows );
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		TRowState& RowState = rGridState[idxRow];
		RowState.resize( mcColumns );
		for( size_t idxCol = 0; idxCol < mcColumns; ++idxCol )
		{
			TCellState& CellState = RowState[idxCol];
			CellState.text = GetCellText( idxRow, idxCol );
			CellState.state = GetCellState( idxRow, idxCol );
			CellState.data = GetCellDataRef( idxRow, idxCol );
		}
	}
	mRowData.resize( ctRows );
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		size_t idxOldRow = rnSortXForm[idxRow];
		TRowState& RowState = rGridState[idxOldRow];
		for( size_t idxCol = 0; idxCol < mcColumns; ++idxCol )
		{
			TCellState& CellState = RowState[idxCol];
			SetCellText( idxRow, idxCol, CellState.text );
			SetCellState( idxRow, idxCol, CellState.state );
			GetCellDataRef( idxRow, idxCol ) = CellState.data;
		}
		mRowData[idxRow] = rTempRowData[idxOldRow];
	}
	SetRedraw( TRUE );
	OnNeedRepaint();
	return true;
}


BEGIN_MESSAGE_MAP(CGridCtrl, CListCtrl)
	ON_WM_NCCALCSIZE()	
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL_REFLECT()
	ON_WM_VSCROLL_REFLECT()
	ON_NOTIFY_REFLECT(LVN_BEGINSCROLL, &CGridCtrl::OnLvnBeginScroll)
	ON_REGISTERED_MESSAGE(refWM_CHECKFOCUS(), &CGridCtrl::OnCheckFocus)
	ON_WM_GETDLGCODE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl message handlers

void CGridCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( lpDrawItemStruct->CtlType != ODT_LISTVIEW )
		return;
  int nRow = lpDrawItemStruct->itemID;
	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	pDC->SetBkColor( mColorService.GetBackgroundColor() );
	pDC->SetTextColor( mColorService.GetForegroundColor() );

	LONG nRightEdge = lpDrawItemStruct->rcItem.right;
	int nCol = -1;
	while( (size_t)(++nCol) < mcColumns )
	{
		CRect rectCell = GetCellRect( nRow, nCol );
		if( rectCell.left >= nRightEdge )
			break;
		CRect rcClip;
		pDC->GetClipBox( &rcClip );
		pDC->IntersectClipRect( &rectCell );
		DrawCell( nRow, nCol, rectCell, *pDC );
		pDC->SelectClipRgn( NULL );
	}

	//Draw focus rect if row is current
	if( mCurrentCell.row() == nRow && mCurrentCell.col() == -1 )
	{
		CRect rcFocus;
		GetItemRect( nRow, &rcFocus, LVIR_BOUNDS );
		rcFocus.DeflateRect( 1, 1 );
		pDC->DrawFocusRect( &rcFocus );
	}
	return;
}

void CGridCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	UpdateWindow();	
	__super::OnNcCalcSize(bCalcValidRects, lpncsp);	
}

BOOL CGridCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown(nFlags, point);

	int nRow = -1;
	int nCol = -1;
	if( !CellHitTest( point, nRow, nCol ) )
	{
		HideEditControls();
		return;
	}
	if( nRow != mCurrentCell.row() || nCol != mCurrentCell.col() )
		SetCurCell( nRow, nCol );
	OnEditCurCell();
}

void CGridCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = mnRowHeight;
}

void CGridCtrl::HScroll(UINT nSBCode, UINT nPos) 
{
	if( GetFocus() != this) 
		SetFocus();
	PostMessage( refWM_CHECKFOCUS(), 0, 0 );
}

void CGridCtrl::VScroll(UINT nSBCode, UINT nPos) 
{
	if( GetFocus() != this) 
		SetFocus();
	PostMessage( refWM_CHECKFOCUS(), 0, 0 );
}

void CGridCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CGridCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bResult = __super::OnNotify(wParam, lParam, pResult);
	if( ((NMHDR*)lParam)->code == NM_KILLFOCUS )
		PostMessage( refWM_CHECKFOCUS(), 0, 0 );
	return bResult;
}

BOOL CGridCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL bResult = __super::OnCommand(wParam, lParam);
	if( lParam ) //child control notification?
	{
		switch( HIWORD(wParam) )
		{
		case BN_KILLFOCUS:
		case EN_KILLFOCUS:
		case CBN_KILLFOCUS:
		case LBN_KILLFOCUS:
			PostMessage( refWM_CHECKFOCUS(), 0, 0 );
			break;
		}
	}
	return bResult;
}

void CGridCtrl::OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	HideEditControls();
	*pResult = 0;
}

LRESULT CGridCtrl::OnCheckFocus( WPARAM wParam, LPARAM lParam )
{
	CWnd* pFocusWnd = GetFocus();
	if( !pFocusWnd )
		HideEditControls();
	else
	{
		CWnd* pFocusParent = pFocusWnd->GetParent();
		if( pFocusParent != this )
		{
			//need to account for CDateTimeCtrl popup window in WinXP (it is a popup owned by the dialog)
			//and AutoCAD color dialog (owned by disabled main AutoCAD window)
			CWnd* pMainWnd = AfxGetMainWnd();
			if( !pMainWnd || pMainWnd->IsWindowEnabled() )
			{
				if( pFocusParent != GetControlPane()->GetHostDialog() ||
						(pFocusWnd->GetStyle() & WS_POPUP) == 0 )
					HideEditControls();
			}
		}
	}
	return 0;
}

BOOL CGridCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

UINT CGridCtrl::OnGetDlgCode()
{
	UINT nResult = __super::OnGetDlgCode();
	if( !mpTemplate->GetStringProperty( Prop::EventReturnPressed ).IsEmpty() ||
			!mpTemplate->GetStringProperty( Prop::EventKeyDown ).IsEmpty() ||
			!mpTemplate->GetStringProperty( Prop::EventKeyUp ).IsEmpty() )
		nResult |= DLGC_WANTALLKEYS;
	return nResult;
}
