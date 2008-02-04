// PropertyGridCtrl.h : header file
//

#pragma once

#include "PtrTypes.h"
#include "PropertyNames.h"
#include "ThemeHelperST.h"
#include <map>
#include <vector>
#include <string>

class CPropertyEditCtrl;
class CPropertiesTabPane;
class CStudioUndoManager;


/////////////////////////////////////////////////////////////////////////////
// CPropertyGridCtrl window

class CPropertyGridCtrl : public CListCtrl
{
	CThemeHelperST mThemeHelper;
	CPropertiesTabPane* mpParent;
public:
	typedef std::vector< TPropertyPtr > TPropertySet;
private:
	typedef std::map< CString, TPropertySet > TPropertyMap;
	typedef std::vector< CString > TPropIndex;
	const std::vector< TDclControlPtr >& mControls;
	TPropertyMap mProperties;
	TPropIndex mrPropIndex; //used to quickly look up a property set from a list index
	CPropertyEditCtrl* mpPropertyEditCtrl;

protected:
	static const UINT& refWM_CHECKFOCUS();

// Construction
public:
	CPropertyGridCtrl( CPropertiesTabPane* pParent, const std::vector< TDclControlPtr >& ActiveControls );
	virtual ~CPropertyGridCtrl();

public:
	bool Create( CWnd* pParentWnd, const CRect& rcWnd, UINT nID );
	void OnActivateDclControl( TDclControlPtr pDclControl );
	Prop::Id GetPropertyId( size_t idxCell );
	PropertyType GetPropertyType( size_t idxCell );
	TPropertySet& GetPropertySet( size_t idxCell );
	CRect GetEditRect( size_t idxCell );
	CStudioUndoManager* GetUndoManager() const;
	void HideEditControls();
	void RecalcLayout();

protected:
	bool IsEditing() const { return (mpPropertyEditCtrl != NULL); }
	int GetCurItem() const;
	CRect GetCellRect( int nRow, int nCol, int area = LVIR_BOUNDS );
	bool CellHitTest( const CPoint& point, int& nRow, int& nCol );
	CThemeHelperST* GetThemeHelper() { return &mThemeHelper; }

// Overridables
protected:
	virtual void OnSelectionChanged() {}
	virtual void OnEditCurCell();
	virtual void OnEndEditCurCell();
	virtual CPropertyEditCtrl* CreateEditControl( size_t idxCell );

// Operations
protected:
	void DrawBooleanProperty( CDC& cdc, const CRect& rcIcon, int nState );
	void DrawColor( CDC& cdc, const CRect& rcIcon, int nColor, const CString& sText );

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnCheckFocus( WPARAM wParam, LPARAM lParam );
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnProperties();
	afx_msg void OnFontProperties();
	afx_msg void OnForeColorProperties();
	afx_msg void OnBackColorProperties();
	afx_msg void OnAltColorProperties();
	afx_msg void OnImageListProperties();
	afx_msg void OnObjectbrowser();
	afx_msg void OnAxProperties();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};
