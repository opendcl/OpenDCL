// PropertyWizard.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyWizard.h"
#include "StudioWorkspace.h"
#include "Geometry.h"
#include "ColorsPage.h"
#include "ComboBoxPage.h"
#include "TextBoxFilters.h"
#include "FontPropPage.h"
#include "ButtonStyles.h"
#include "ImageListPage.h"
#include "TabsPane.h"
#include "SortTabs.h"
#include "ColumnsPage.h"
#include "ToolTipsPage.h"
#include "ProgressBarPage.h"


static CString GetPropertyWizardCaption( TDclControlPtr pDclControl )
{
	CString sCaption;
	CString sFmt = theWorkspace.LoadResourceString( IDS_PROPERTYWIZARD );
	if( !sFmt.IsEmpty() )
		sCaption.Format( sFmt, (LPCTSTR)pDclControl->GetStringProperty( Prop::Name ) );	
	return sCaption;
}

// CPropertyWizard

CPropertyWizard::CPropertyWizard( TDclControlPtr pDclControl, PropPage nDefaultPage )
: CPropertySheet( GetPropertyWizardCaption( pDclControl ), NULL, 0 )
, mpDclControl( pDclControl )
, mnDefaultPage( nDefaultPage )
, mpGeometryPage( NULL )
, mpBackColorsPage( NULL )
, mpForeColorsPage( NULL )
, mpAltColorsPage( NULL )
, mpComboBoxPage( NULL )
, mpTextBoxPage( NULL )
, mpFontPage( NULL )
, mpButtonPage( NULL )
, mpImageListPage( NULL )
, mpTabs( NULL )
, mpSortTabs( NULL )
, mpColumnsPage( NULL )
, mpToolTipsPage( NULL )
, mpProgressPage( NULL )
{
	Initialize();
}

CPropertyWizard::~CPropertyWizard()
{
	delete mpGeometryPage;
	delete mpBackColorsPage;
	delete mpForeColorsPage;
	delete mpAltColorsPage;
	delete mpComboBoxPage;
	delete mpTextBoxPage;
	delete mpFontPage;
	delete mpButtonPage;
	delete mpImageListPage;
	delete mpTabs;
	delete mpSortTabs;
	delete mpColumnsPage;
	delete mpToolTipsPage;
	delete mpProgressPage;
}

bool CPropertyWizard::Initialize(void)
{
	if( !mpDclControl )
		return false;

	// Geometry page
	if( mpDclControl->GetPropertyObject( Prop::UseTopFromBottom ) )
	{
		mpGeometryPage = new CGeometry( mpDclControl );
		AddPage( mpGeometryPage );
	}

	// Background Color page
	if( mpDclControl->GetPropertyObject( Prop::BackgroundColor ) )
	{
		mpBackColorsPage = new CColorsPage( Prop::BackgroundColor, mpDclControl );
		AddPage( mpBackColorsPage );
	}

	// Foreground Color page
	if( mpDclControl->GetPropertyObject( Prop::ForegroundColor ) )
	{
		mpForeColorsPage = new CColorsPage( Prop::ForegroundColor, mpDclControl );
		AddPage( mpForeColorsPage );
	}

	// Alternate Color page
	if( mpDclControl->GetPropertyObject( Prop::AlternatingColor ) )
	{
		mpAltColorsPage = new CColorsPage( Prop::AlternatingColor, mpDclControl );
		AddPage( mpAltColorsPage );
	}

	// Combo Box Styles page
	if( mpDclControl->GetPropertyObject( Prop::ComboBoxStyle ) )
	{
		mpComboBoxPage = new CComboBoxPage( mpDclControl );
		AddPage( mpComboBoxPage );
	}

	// Text Box Filter page
	if( mpDclControl->GetPropertyObject( Prop::FilterStyle ) )
	{
		mpTextBoxPage = new CTextBoxFilters( mpDclControl );
		AddPage( mpTextBoxPage );
	}

	// Font page
	if( mpDclControl->GetPropertyObject( Prop::FontName ) )
	{
		mpFontPage = new CFontPropertyPage( mpDclControl );
		AddPage( mpFontPage );
	}

	// Button Styles page
	if( mpDclControl->GetPropertyObject( Prop::ButtonStyle ) )
	{
		mpButtonPage = new CButtonStyles( mpDclControl );
		AddPage( mpButtonPage );
	}

	// Image List page
	if( mpDclControl->GetPropertyObject( Prop::ImageList ) )
	{
		mpImageListPage = new CImageListPage( mpDclControl );
		AddPage( mpImageListPage );
	}

	// Tabs & Tab Sort pages
	if( mpDclControl->GetPropertyObject( Prop::TabsCaption ) )
	{
		mpTabs = new CTabsPane( mpDclControl, mpImageListPage->GetImageList() );
		AddPage( mpTabs );
		mpSortTabs = new CSortTabs( mpDclControl, mpTabs );
		AddPage( mpSortTabs );
	}

	// Columns page
	if( mpDclControl->GetPropertyObject( Prop::ColumnCaptions ) )
	{
		TPropertyPtr pListViewStyle = mpDclControl->GetPropertyObject(Prop::ListViewStyle);
		if( !pListViewStyle || pListViewStyle->GetLongValue() >= 3 )
		{
			mpColumnsPage = new CColumnsPage( mpDclControl, mpImageListPage );
			AddPage( mpColumnsPage );
		}
	}

	// Tool Tips page
	if( mpDclControl->GetPropertyObject( Prop::ToolTipTitle ) )
	{
		mpToolTipsPage = new CToolTipsPage( mpDclControl );
		AddPage( mpToolTipsPage );
	}

	// Progress Bar page
	if( mpDclControl->GetPropertyObject( Prop::DisplayPercentage ) )
	{
		mpProgressPage = new CProgressBarPage( mpDclControl );
		AddPage( mpProgressPage );
	}

	switch( mnDefaultPage )
	{
	case Geometry: SetActivePage( mpGeometryPage ); break;
	case BackColor: SetActivePage( mpBackColorsPage ); break;
	case ForeColor: SetActivePage( mpForeColorsPage ); break;
	case AltColor: SetActivePage( mpAltColorsPage ); break;
	case ComboBoxStyle: SetActivePage( mpComboBoxPage ); break;
	case TextBoxFilter: SetActivePage( mpTextBoxPage ); break;
	case Font: SetActivePage( mpFontPage ); break;
	case ButtonStyle: SetActivePage( mpButtonPage ); break;
	case ImageList: SetActivePage( mpImageListPage ); break;
	case Tabs: SetActivePage( mpTabs ); break;
	case SortTab: SetActivePage( mpSortTabs ); break;
	case Columns: SetActivePage( mpColumnsPage ); break;
	case Tooltip: SetActivePage( mpToolTipsPage ); break;
	case ProgressBar: SetActivePage( mpProgressPage ); break;
	}

	return true;
}


BEGIN_MESSAGE_MAP(CPropertyWizard, CPropertySheet)
END_MESSAGE_MAP()

// CPropertyWizard message handlers

INT_PTR CPropertyWizard::DoModal()
{
	AutoUndoGroup UndoGroup( mpDclControl->GetUndoManager(), IDS_UNDO_CHANGEPROPERTIES );
	return __super::DoModal();
}
