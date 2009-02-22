#pragma once

#include "PtrTypes.h"

class CGeometry;
class CColorsPage;
class CComboBoxPage;
class CTextBoxFilters;
class CFontPropertyPage;
class CButtonStyles;
class CImageListPage;
class CTabsPane;
class CSortTabs;
class CColumnsPage;
class CToolTipsPage;
class CProgressBarPage;


// CPropertyWizard

class CPropertyWizard : public CPropertySheet
{
public:
	enum PropPage { _None, Geometry, BackColor, ForeColor, AltColor, ComboBoxStyle, TextBoxFilter,
									Font, ButtonStyle, ImageList, Tabs, SortTab, Columns, Tooltip, ProgressBar, };

private:
	TDclControlPtr mpDclControl;
	PropPage mnDefaultPage;
	CGeometry* mpGeometryPage;
	CColorsPage* mpBackColorsPage;
	CColorsPage* mpForeColorsPage;
	CColorsPage* mpAltColorsPage;
	CComboBoxPage* mpComboBoxPage;
	CTextBoxFilters* mpTextBoxPage;
	CFontPropertyPage* mpFontPage;
	CButtonStyles* mpButtonPage;
	CImageListPage* mpImageListPage;
	CTabsPane* mpTabs;
	CSortTabs* mpSortTabs;
	CColumnsPage* mpColumnsPage;
	CToolTipsPage* mpToolTipsPage;
	CProgressBarPage* mpProgressPage;

public:
	CPropertyWizard( TDclControlPtr pDclControl, PropPage nDefaultPage = _None );
	virtual ~CPropertyWizard();

public:
	virtual INT_PTR DoModal();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	bool Initialize(void);

protected:
	DECLARE_MESSAGE_MAP()
};


