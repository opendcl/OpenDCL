// ArxGridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxGridCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"
#include "AutoDocLock.h"
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
#include "UnitsNumericFilter.h"
#include "UpperCaseFilter.h"
#include "ComboFilter.h"
#include "DimStyleComboHandler.h"
#include "FontComboHandler.h"
#include "LayerComboHandler.h"
#include "PrinterComboHandler.h"
#include "TextStyleComboHandler.h"
#include "R2006AcUiMRUComboBoxFixup.h"
#include "DirDialog.h"
#include "Workspace.h"
#include "AcadColorTable.h"
#include "Resource.h"


template< typename TAcUiBase, DWORD _Style = 0, DWORD _AcUiStyle = (DWORD)-1 >
class CAcUiTextBoxEditCtrl : public TAcUiBase, public CGridCellEditCtrl
{
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left + 4, rcCell.top + 4, rcCell.right - 4, rcCell.bottom - 4 );
		}
public:
	typedef TAcUiBase _Base;
public:
	CAcUiTextBoxEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, UINT nID = 100 )
		: TAcUiBase()
		, CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		{
			DWORD dwStyle = (WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_WANTRETURN | ES_LEFT);
			dwStyle |= _Style;
			if( !Create( dwStyle, CalcRect( pGridCtrl->GetCellRect( nRow, nCol ) ), pGridCtrl, nID ) )
				return;
			DWORD dwAcUiStyle = ((_AcUiStyle == (DWORD)-1)?
														(AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS | AC_ES_SHOW_ERRMSG) :
														_AcUiStyle);
			SetStyleMask( dwAcUiStyle );
			SetFont( pGridCtrl->GetFont() );
			SetWindowText( pGridCtrl->GetCellText( nRow, nCol ) );
			SetSel( 0, -1, TRUE );
			SetFocus();
		}
	virtual ~CAcUiTextBoxEditCtrl()
		{
			CString sText;
			GetWindowText( sText );
			ConvertData( sText );
			mpGridCtrl->SetCellText( mnRow, mnCol, sText );
			DestroyWindow();
		}
};
typedef CAcUiTextBoxEditCtrl< CAcUiStringEdit > CAcUiStringEditCtrl;
typedef CAcUiTextBoxEditCtrl< CAcUiNumericEdit > CAcUiUnitsEditCtrl;
typedef CAcUiTextBoxEditCtrl< CAcUiAngleEdit > CAcUiAngleEditCtrl;
typedef CAcUiTextBoxEditCtrl< CAcUiSymbolEdit > CAcUiSymbolEditCtrl;


template< typename TAcUiBase >
class CAcUiComboHelper
{
public:
	static void Init( TAcUiBase* pCombo ) {}
	static int GetCellValue( TAcUiBase* pCombo ) { return -1; }
	static void SetCurSelFromValue( TAcUiBase* pCombo, int nValue, const CString& sText )
		{
			if( nValue >= 0 )
				pCombo->SetCurSel( nValue );
			else
				pCombo->SetCurSel( pCombo->FindString( 0, sText ) );
		}
};

template<>
class CAcUiComboHelper< CAcUiColorComboBox >
{
public:
	static void Init( CAcUiColorComboBox* pCombo ) {}
	static int GetCellValue( CAcUiColorComboBox* pCombo ) { return pCombo->GetCurrentItemColorIndex(); }
	static void SetCurSelFromValue( CAcUiColorComboBox* pCombo, int nValue, const CString& sText )
		{
			pCombo->SetCurSel( (nValue < 0)? -1 : pCombo->AddColorToMRU( nValue ) );
		}
};

template<>
class CAcUiComboHelper< CAcUiLineWeightComboBox >
{
public:
	static void Init( CAcUiLineWeightComboBox* pCombo )
		{
			pCombo->SetUseDefault( TRUE );
		}
	static int GetCellValue( CAcUiLineWeightComboBox* pCombo ) { return pCombo->GetCurrentItemLineWeight(); }
	static void SetCurSelFromValue( CAcUiLineWeightComboBox* pCombo, int nValue, const CString& sText )
		{
			pCombo->SetCurSel( (nValue < 0)? -1 : pCombo->FindItemByLineWeight( nValue ) );
		}
};

template<>
class CAcUiComboHelper< CAcUiArrowHeadComboBox >
{
public:
	static void Init( CAcUiArrowHeadComboBox* pCombo ) { pCombo->SetUseOrigin2( TRUE ); }
	static int GetCellValue( CAcUiArrowHeadComboBox* pCombo ) { return -1; }
	static void SetCurSelFromValue( CAcUiArrowHeadComboBox* pCombo, int nValue, const CString& sText )
		{
			if( nValue >= 0 )
				pCombo->SetCurSel( nValue );
			else
				pCombo->SetCurSel( pCombo->FindString( 0, sText ) );
		}
};

#if (_ACADTARGET >= 17)
template<>
class CAcUiComboHelper< CAcUiLineTypeComboBox >
{
public:
	static void Init( CAcUiLineTypeComboBox* pCombo ) { pCombo->SetUseOther( FALSE ); }
	static int GetCellValue( CAcUiLineTypeComboBox* pCombo ) { return -1; }
	static void SetCurSelFromValue( CAcUiLineTypeComboBox* pCombo, int nValue, const CString& sText )
		{
			if( nValue >= 0 )
				pCombo->SetCurSel( nValue );
			else
				pCombo->SetCurSel( pCombo->FindString( 0, sText ) );
		}
};
#endif //(_ACADTARGET >= 17)

template< typename TAcUiBase, DWORD _Style = 0, DWORD _AcUiStyle = (DWORD)-1 >
class CAcUiComboEditCtrl : public TAcUiBase, public CGridCellEditCtrl
{
	CStatic mClippingWnd;
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left, rcCell.top, rcCell.right, rcCell.top + 120 );
		}
public:
	typedef TAcUiBase _Base;
public:
	CAcUiComboEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, UINT nID = 100 )
		: TAcUiBase()
		, CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		{
			CAcUiComboHelper< TAcUiBase >::Init( this );
			CRect rcCell = pGridCtrl->GetCellRect( nRow, nCol );
			rcCell.DeflateRect( 2, 2 );
			CRect rcCtrl = CalcRect( rcCell );
			mClippingWnd.Create( _T(""), WS_CHILD, rcCell, pGridCtrl );
			rcCtrl.MoveToXY( 0, 0 );
			DWORD dwComboStyle = (WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | CBS_HASSTRINGS);
			dwComboStyle |= _Style;
			if( !Create( dwComboStyle, rcCtrl, &mClippingWnd, 100 ) )
				return;
			SetDroppedWidth( 200 );
			DWORD dwAcUiStyle = ((_AcUiStyle == (DWORD)-1)?
														(AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS | AC_ES_SHOW_ERRMSG) :
														_AcUiStyle);
			SetStyleMask( dwAcUiStyle );
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
			std::vector< tstring > rsComboList;
			std::vector< int > rImage;
			pGridCtrl->GetCellComboListItems( nRow, nCol, rsComboList, rImage );
			int nMinWidth = 0;
			for( size_t idx = 0; idx < rsComboList.size(); ++idx )
			{
				LPCTSTR pszItem = rsComboList.at( idx ).c_str();
				AddString( pszItem );
				int nTextWidth = 0;
				int nTextHeight = 0;
				GetTextExtent( pszItem, nTextWidth, nTextHeight );
				if( nTextWidth > nMinWidth )
					nMinWidth = nTextWidth;
			}
			if( nMinWidth > (rcClip.Width() * 3) )
				nMinWidth = rcClip.Width() * 3;
			if( nMinWidth > 0 )
				SetDroppedWidth( nMinWidth );
			if( GetCount() == 0 )
			{
				CRect rcWnd;
				GetWindowRect( &rcWnd );
				SetWindowPos( NULL, 0, 0, rcWnd.Width(), rcWnd.Height(), (SWP_NOZORDER | SWP_NOMOVE) );
			}
			CString sText = pGridCtrl->GetCellText( nRow, nCol );
			CAcUiComboHelper< TAcUiBase >::SetCurSelFromValue( this, pGridCtrl->GetCellImage( nRow, nCol ), sText );
			mClippingWnd.ShowWindow( SW_SHOW );
			SetFocus();
		}
	virtual ~CAcUiComboEditCtrl()
		{
			CString sText;
			GetWindowText( sText );
			ConvertData( sText );
			mpGridCtrl->SetCellTextImage( mnRow, mnCol, sText, CAcUiComboHelper< TAcUiBase >::GetCellValue( this ) );
			DestroyWindow();
			mClippingWnd.DestroyWindow();
		}
};
typedef CAcUiComboEditCtrl< CAcUiArrowHeadComboBox, (CBS_OWNERDRAWFIXED | CBS_DROPDOWNLIST) > CAcUiArrowComboEditCtrl;
typedef CAcUiComboEditCtrl< CAcUiPlotStyleNamesComboBox, (CBS_OWNERDRAWFIXED | CBS_DROPDOWNLIST) > CAcUiPlotStyleNameComboEditCtrl;
typedef CAcUiComboEditCtrl< CAcUiPlotStyleTablesComboBox, (CBS_OWNERDRAWFIXED | CBS_DROPDOWNLIST) > CAcUiPlotStyleTableComboEditCtrl;
typedef CAcUiComboEditCtrl< CAcUiStringComboBox, CBS_DROPDOWN > CAcUiStringComboEditCtrl;
typedef CAcUiComboEditCtrl< CAcUiNumericComboBox, CBS_DROPDOWN > CAcUiUnitsComboEditCtrl;
typedef CAcUiComboEditCtrl< CAcUiAngleComboBox, CBS_DROPDOWN > CAcUiAngleComboEditCtrl;
typedef CAcUiComboEditCtrl< CAcUiSymbolComboBox, CBS_DROPDOWN > CAcUiSymbolComboEditCtrl;
typedef CAcUiComboEditCtrl< CAcUiColorComboBox, (CBS_OWNERDRAWFIXED | CBS_DROPDOWNLIST) > CAcUiColorComboEditCtrl;
typedef CAcUiComboEditCtrl< CAcUiLineWeightComboBox, (CBS_OWNERDRAWFIXED | CBS_DROPDOWNLIST) > CAcUiLineWeightComboEditCtrl;
#if (_ACADTARGET >= 17)
typedef CAcUiComboEditCtrl< CAcUiLineTypeComboBox, (CBS_OWNERDRAWFIXED | CBS_DROPDOWNLIST) > CAcUiLineTypeComboEditCtrl;
#endif //(_ACADTARGET >= 17)


bool getvar(LPCTSTR pszVarName)
{	
	struct resbuf rsVarVal;
	if (acedGetVar(pszVarName, &rsVarVal) != RTNORM)
		return false;
	if (rsVarVal.restype == RTT)
		return true;
	if (rsVarVal.restype == RTNIL)
		return false;
	return (rsVarVal.restype == RTSHORT && rsVarVal.resval.rint == 1);	
}


static int GetCurrentLayerColor()
{
	CAutoDocReadLock CurDocLock;
	AcDbObjectId idLayer = acdbHostApplicationServices()->workingDatabase()->clayer();
	AcDbLayerTableRecord* pLayer = NULL;
	if( Acad::eOk != acdbOpenObject( pLayer, idLayer, AcDb::kForRead ) )
		return 7;
	int nColor = pLayer->color().colorIndex();
	pLayer->close();
	return nColor;
}


static CString GetColorDisplayName( int nAcadColor )
{
	if( nAcadColor < 0 )
		return CString();
	if( nAcadColor > 256 )
	{
		COLORREF clr = (COLORREF)nAcadColor; //note: BGR instead of RGB!
		AcCmColor color;
		color.setRGB( GetBValue( clr ), GetGValue( clr ), GetRValue( clr ) );
		return color.colorNameForDisplay();
	}
	const ACHAR* rszColors[9];
	accmGetLocalizedColorNames( rszColors );
	switch( nAcadColor )
	{
	case 0: return rszColors[0];
	case 256: return rszColors[8];
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		return rszColors[nAcadColor];
	}
	CString sColor;
	sColor.Format( theWorkspace.LoadResourceString( IDS_COLORDISPLAYTEXT ), nAcadColor );
	return sColor;
}


class CAcadColorEditCtrl : public CGridCellEditCtrl
{
public:
	CAcadColorEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol )
		: CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		{
			int nCellImage = pGridCtrl->GetCellImage( nRow, nCol );
			int nCurLayerColor = GetCurrentLayerColor();
			if( nCellImage < 0 )
				nCellImage = _tstol( pGridCtrl->GetCellText( nRow, nCol ) );
			else if( nCellImage > 256 )
				nCellImage = 256;
			if( acedSetColorDialog( nCellImage, Adesk::kTrue, nCurLayerColor ) )
				pGridCtrl->SetCellTextImage( nRow, nCol, GetColorDisplayName( nCellImage ), nCellImage );
		}
	virtual ~CAcadColorEditCtrl()
		{
		}
};

class CTrueColorEditCtrl : public CGridCellEditCtrl
{
public:
	CTrueColorEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol )
		: CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		{
			int nCellImage = pGridCtrl->GetCellImage( nRow, nCol );
		#if defined(_BRXTARGET)
			resbuf rbT = { NULL, RTT };
		#else
			resbuf rbCurColor = { NULL, RTSHORT };
			rbCurColor.resval.rint = GetCurrentLayerColor();
			resbuf rbT = { &rbCurColor, RTT };
		#endif
			resbuf rbColor = { &rbT };
			resbuf rbFuncName = { &rbColor, RTSTR };
			rbFuncName.resval.rstring = _T("acad_truecolordlg");
			CString sText = pGridCtrl->GetCellText( nRow, nCol );
			if( nCellImage == -1 )
			{
				int idxComma = sText.Find( _T(",") );
				CString sRed = sText.Left( idxComma );
				CString sGreen = sText.Mid( idxComma + 1 );
				idxComma = sGreen.Find( _T(",") );
				CString sBlue = sGreen.Mid( idxComma + 1 );
				sGreen = sGreen.Left( idxComma );
				rbColor.restype = 420;
				rbColor.resval.rlong =
					(long)RGB(_tstol( sBlue ), _tstol( sGreen ), _tstol( sRed ) ); //note: BGR instead of RGB!
			}
			else if( nCellImage == -2 )
			{
				rbColor.restype = 430;
				rbColor.resval.rstring = sText.LockBuffer(); //color book color
			}
			else
			{
				rbColor.restype = RTSHORT;
				rbColor.resval.rint = (nCellImage > 0 && nCellImage < 256)? nCellImage : 7;
			}
			resbuf* prbResult = NULL;
			if( RTNORM == acedInvoke( &rbFuncName, &prbResult ) && prbResult )
			{
				resbuf* prbTrueColor = prbResult->rbnext;
				resbuf* prbColorBook = (prbTrueColor? prbTrueColor->rbnext : NULL);
				if( prbColorBook && prbColorBook->restype == 430 )
					pGridCtrl->SetCellTextImage( nRow, nCol, prbColorBook->resval.rstring, -2 );
				else if( prbTrueColor && prbTrueColor->restype == 420 )
					pGridCtrl->SetCellTextImage( nRow, nCol, GetColorDisplayName( prbTrueColor->resval.rlong ), -1 );
				else
				{
					int nColor = prbResult->resval.rint;
					pGridCtrl->SetCellTextImage( nRow, nCol, GetColorDisplayName( nColor ), nColor );
				}
				acutRelRb( prbResult );
			}
		}
	virtual ~CTrueColorEditCtrl()
		{
		}
};


/////////////////////////////////////////////////////////////////////////////
// CArxGridCtrl

CArxGridCtrl::CArxGridCtrl( TDclControlPtr pTemplate,
														CControlPane* pPane,
														UINT nID,
														bool bCreate /*= true*/ )
: CGridCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxGridCtrl::~CArxGridCtrl()
{
}

bool CArxGridCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

bool CArxGridCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	//switch( pProp->GetID() )
	//{
	//case Prop::DragnDropAllowDrop:
	//	{
	//		SetDragnDrop( pProp->GetBooleanValue() );
	//		break;
	//	}
	//}
	return !bFailed;
}

void CArxGridCtrl::OnEditCurCell()
{
	__super::OnEditCurCell();
	if( !IsEditing() )
		return;
	GetArxServices()->HandleEvent( Prop::EventBeginLabelEdit, args_NN( mCurrentCell.row(), mCurrentCell.col() ) );
}

void CArxGridCtrl::OnEndEditCurCell()
{
	__super::OnEndEditCurCell();
	GetArxServices()->HandleEvent( Prop::EventEndLabelEdit, args_NN( mCurrentCell.row(), mCurrentCell.col() ) );
	if( GetFocus() != this )
		GetArxServices()->HandleEvent( Prop::EventKillFocus );
}


void CArxGridCtrl::DoFileDlg( Grid::CellStyle nStyle ) 
{
	switch (nStyle)
	{
		case Grid::DirectoryCell:
		{			
			CDirDialog dlg( theWorkspace.LoadResourceString(IDS_SELFOLDER), GetItemText(mCurrentCell.row(), mCurrentCell.col()) );
			if (dlg.DoBrowse(this) == TRUE)
				SetCellTextImage( mCurrentCell.row(), mCurrentCell.col(), dlg.GetSelectedFolder(), -1 );
			break;
		}
		case Grid::DwgFilesCell:
		{
			if( !acDocManager->curDocument() )
				break; //ARX docs say you can't call acedGetFileD in zero doc state
			struct resbuf * result;
			CString sFileExt = _T("dwg;dxf");
			CString sFile = GetItemText(mCurrentCell.row(), mCurrentCell.col());
			CString sTitle;
			result = acutNewRb(RTSTR);

			const _CellData* pCellData = GetCellData( mCurrentCell.row(), mCurrentCell.col() );
			if( pCellData && !pCellData->mrsComboList.empty() )
			{
				sFileExt = pCellData->mrsComboList.front().c_str();
				if( pCellData->mrsComboList.size() > 1 )
					sTitle = (++pCellData->mrsComboList.begin())->c_str();
			}
			else
			{
				std::vector< tstring > rsList;
				std::vector< int > ridxImage;
				if( GetCellComboListItems( mCurrentCell.row(), mCurrentCell.col(), rsList, ridxImage ) && !rsList.empty() )
				{
					sFileExt = rsList.front().c_str();
					if( rsList.size() > 1 )
						sTitle = (++rsList.begin())->c_str();
				}
			}
			if( acedGetFileD( sTitle, sFile, sFileExt, 8, result ) == RTNORM )
				SetCellTextImage( mCurrentCell.row(), mCurrentCell.col(), result->resval.rstring, -1 );
			acutRelRb(result);
			break;
		}
	}
}

CGridCellEditCtrl* CArxGridCtrl::CreateEditControl( int nRow, int nCol )
{
	switch( GetCellStyle( nRow, nCol ) )
	{
		//case Grid::CheckBoxes: return new CToggleEditCtrl( this, nRow, nCol );
		//case Grid::OptionButtons: return new CRadioEditCtrl( this, nRow, nCol );
		//case Grid::SwitchableIcons: return new CToggleEditCtrl( this, nRow, nCol );
		//case Grid::EllipsesButtons: return new CButtonEditCtrl( this, nRow, nCol, _T("..."), ID_CELLBUTTON );
		//case Grid::PickButtons: return new CButtonEditCtrl( this, nRow, nCol, IDI_PICKSMALL, ID_CELLBUTTON );
		case Grid::Strings: return new CAcUiStringEditCtrl( this, nRow, nCol );
		case Grid::AngleUnits: return new CAcUiAngleEditCtrl( this, nRow, nCol );
		case Grid::Integers: return new CTextBoxEditCtrl( this, nRow, nCol, new CIntegerFilter );
		case Grid::Units: return new CTextBoxEditCtrl( this, nRow, nCol, new CUnitsNumericFilter );
		case Grid::UpperCase: return new CTextBoxEditCtrl( this, nRow, nCol, new CUpperCaseFilter );
		case Grid::LowerCase: return new CTextBoxEditCtrl( this, nRow, nCol, new CLowerCaseFilter );
		case Grid::Password: return new CTextBoxEditCtrl( this, nRow, nCol, new CPasswordFilter );
		case Grid::MultiLine: return new CTextBoxEditCtrl( this, nRow, nCol, new CMultilineFilter );
		case Grid::Currency: return new CTextBoxEditCtrl( this, nRow, nCol, new CCurrencyFilter );
		//case Grid::Date: return new CDateTimeEditCtrl( this, nRow, nCol, true );
		//case Grid::Time: return new CDateTimeEditCtrl( this, nRow, nCol, false );
		case Grid::Percentage: return new CTextBoxEditCtrl( this, nRow, nCol, new CPercentageFilter );
		//case Grid::DropDown: return new CComboDropdownListEditCtrl( this, nRow, nCol );
		case Grid::ArrowHead: return new CAcUiArrowComboEditCtrl( this, nRow, nCol );
		case Grid::AcadColors: return new CAcUiColorComboEditCtrl( this, nRow, nCol );
		case Grid::TextStyleList: return new CComboDropdownListEditCtrl( this, nRow, nCol, new CTextStyleComboHandler );
		case Grid::PlotStyleNames: return new CAcUiPlotStyleNameComboEditCtrl( this, nRow, nCol );
		case Grid::PlotStyleTables: return new CAcUiPlotStyleTableComboEditCtrl( this, nRow, nCol );
		case Grid::PlotterList: return new CComboDropdownListEditCtrl( this, nRow, nCol, new CPrinterComboHandler );
		case Grid::Fonts: return new CComboDropdownListEditCtrl( this, nRow, nCol, new CFontComboHandler );
		//case Grid::DriveList: return __super::CreateEditControl( nRow, nCol );
		case Grid::LayerList: return new CComboDropdownListEditCtrl( this, nRow, nCol, new CLayerComboHandler );
		case Grid::DimStyleList: return new CComboDropdownListEditCtrl( this, nRow, nCol, new CDimStyleComboHandler );
		//case Grid::ImageDropList: return new CImageComboDropdownListEditCtrl( this, nRow, nCol );
		case Grid::AcadColorCell: return new CAcadColorEditCtrl( this, nRow, nCol );
		case Grid::TrueColorCell: return new CTrueColorEditCtrl( this, nRow, nCol );
		case Grid::LineWeightCell: return new CAcUiLineWeightComboEditCtrl( this, nRow, nCol );
		case Grid::LinetypeCell:
	#if (_ACADTARGET >= 17)
			return new CAcUiLineTypeComboEditCtrl( this, nRow, nCol );
	#else //line type combo was not introduced until R2006
			return new CTextBoxEditCtrl( this, nRow, nCol, new CSymbolNameFilter );
	#endif //(_ACADTARGET >= 17)
		case Grid::DirectoryCell: return new CButtonEditCtrl( this, nRow, nCol, IDI_FOLDER, ID_CELLBUTTON );
		case Grid::DwgFilesCell: return new CButtonEditCtrl( this, nRow, nCol, IDI_FOLDER, ID_CELLBUTTON );
		case Grid::Strings_Combo: return new CAcUiStringComboEditCtrl( this, nRow, nCol );
		case Grid::AngleUnits_Combo: return new CAcUiAngleComboEditCtrl( this, nRow, nCol );
		case Grid::Integers_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CComboFilter( new CIntegerFilter ) );
		case Grid::Units_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CComboFilter( new CUnitsNumericFilter ) );
		//case Grid::UpperCase_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CComboFilter( new CUpperCaseFilter ) );
		//case Grid::LowerCase_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CComboFilter( new CLowerCaseFilter ) );
		case Grid::Symbols: return new CAcUiSymbolEditCtrl( this, nRow, nCol );
		case Grid::Symbols_Combo: return new CAcUiSymbolComboEditCtrl( this, nRow, nCol );
		//default: return new CTextBoxEditCtrl( this, nRow, nCol );
	}
	return __super::CreateEditControl( nRow, nCol );
}

void CArxGridCtrl::DrawColor( CDC& cdc, const CRect& rcIcon, int nColor, const CString& sText )
{
	if( nColor == -2 ) //color book color?
	{
		COLORREF crFill = RGB(255,255,255);
		CString sColorBook = sText.SpanExcluding( _T("$") );
		CString sColorName = sText.Mid( sColorBook.GetLength() + 1 );
		AcCmColor color;
		if( Acad::eOk == accmGetColorFromColorBookName( color, sColorBook, sColorName ) )
			crFill = RGB(color.red(),color.green(),color.blue());
		cdc.Rectangle( &rcIcon );
		CRect rcFill = rcIcon;
		rcFill.DeflateRect( 1, 1 );
		CBrush brFill( crFill );
		cdc.FillRect( &rcFill, &brFill );
	}
	else
		__super::DrawColor( cdc, rcIcon, nColor, sText );
}


BEGIN_MESSAGE_MAP(CArxGridCtrl, CGridCtrl)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()	
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()	
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()	
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()	
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(NM_SETFOCUS, OnSetfocus)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)		
	ON_WM_MOUSEMOVE()
	ON_WM_KEYUP()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CELLBUTTON, OnCellButtonClicked)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxGridCtrl message handlers

void CArxGridCtrl::OnCellButtonClicked(void)
{
	switch( GetCurCellStyle() )
	{
	case Grid::DirectoryCell: return DoFileDlg( Grid::DirectoryCell );
	case Grid::DwgFilesCell: return DoFileDlg( Grid::DwgFilesCell );
	}
	GetArxServices()->HandleEvent( Prop::EventBtnClicked, args_NN( mCurrentCell.row(), mCurrentCell.col() ) );
}

void CArxGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 1, nFlags, point.x, point.y ) );
	__super::OnLButtonDown(nFlags, point);
	if( !mCurrentCell )
		return;
	GetArxServices()->HandleEvent( Prop::EventClicked, args_NN( mCurrentCell.row(), mCurrentCell.col() ) );
}

void CArxGridCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 2, nFlags, point.x, point.y ) );
	__super::OnRButtonDown(nFlags, point);
}

void CArxGridCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 4, nFlags, point.x, point.y ) );
	__super::OnMButtonDown(nFlags, point);
}

void CArxGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( !mCurrentCell )
		return;

	GetArxServices()->HandleEvent( Prop::EventDblClicked, args_NN( mCurrentCell.row(), mCurrentCell.col() ) );
}

void CArxGridCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( mpTemplate )		
		GetArxServices()->HandleEvent( Prop::EventKeyUp, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( mpTemplate )		
		GetArxServices()->HandleEvent( Prop::EventKeyDown, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );

	switch (nChar) 
	{
		case VK_ESCAPE: 
			break;
		case VK_RETURN: 
			OnEditCurCell();
			return;
		case VK_UP: 
		{
			MoveUp();
			return;
		}
		case VK_DOWN: 
		{
			MoveDown();
			return;
		}
		case VK_LEFT: 
		{
			MoveLeft();
			return;
		}
		case VK_RIGHT: 
		{
			MoveRight();
			return;
		}
	}
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxGridCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 1, nFlags, point.x, point.y ) );
	__super::OnLButtonUp(nFlags, point);
}

void CArxGridCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 2, nFlags, point.x, point.y ) );
	__super::OnRButtonUp(nFlags, point);
}

void CArxGridCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 4, nFlags, point.x, point.y ) );
	__super::OnMButtonUp(nFlags, point);
}

void CArxGridCtrl::OnContextMenu( CWnd* pTarget, CPoint point )
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 2, MK_RBUTTON, point.x, point.y ) );
	__super::OnContextMenu(pTarget, point);
}

void CArxGridCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove(nFlags, point);
}

void CArxGridCtrl::OnSelectionChanged() 
{
	GetArxServices()->HandleEvent( Prop::EventSelChanged, args_NN( mCurrentCell.row(), mCurrentCell.col() ) );
}

void CArxGridCtrl::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HideEditControls();
	*pResult = 0;
}

void CArxGridCtrl::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	if (IsWindow(m_hWnd) == TRUE)
		HideEditControls();
}

void CArxGridCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	GetArxServices()->HandleEvent( Prop::EventColumnClick, args_N( pNMListView->iSubItem ) );
	*pResult = 0;
}

void CArxGridCtrl::DrawCell( int nRow, int nCol, const CRect& rectCell, CDC& cdc )
{
	__super::DrawCell( nRow, nCol, rectCell, cdc );
}

void CArxGridCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CGridCtrl::OnKillFocus(pNewWnd);
	if( !pNewWnd || pNewWnd->GetParent() != this )
		GetArxServices()->HandleEvent( Prop::EventKillFocus );
}
