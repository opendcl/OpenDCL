// PropertyGridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyGridCtrl.h"
#include "PropertiesTabPane.h"
#include "DclControlTemplate.h"
#include "StudioWorkspace.h"
#include "StudioDialogControl.h"
#include "StudioFrame.h"
#include "DclPicture.h"
#include "Resource.h"
#include "ThemeHelperST.h"
#include "StudioUndoManager.h"
#include "AxContainerCtrl.h"
#include "PropEnumNames.h"
#include "ControlBrowser.h"
#include "FilteredEditCtrl.h"
#include "DynamicButtonCtrl.h"
#include "UnsignedIntegerFilter.h"
#include "IntegerFilter.h"
#include "NumericFilter.h"
#include "SymbolNameFilter.h"
#include "AxInterfaceDescriptor.h"
#include <MSStkPPg.H>

static VARTYPE GetActiveXPropType( TPropertyPtr pProp );
static bool IsBooleanProperty( TPropertyPtr pProp );


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

//#define BP_PUSHBUTTON			0x00000001
//#define BP_RADIOBUTTON			0x00000002
#define BP_CHECKBOX				0x00000003

//#define PBS_NORMAL				0x00000001
//#define PBS_HOT					0x00000002
//#define PBS_PRESSED				0x00000003
//#define PBS_DISABLED			0x00000004
//#define PBS_DEFAULTED			0x00000005

#define RBS_UNCHECKEDNORMAL		0x00000001
//#define RBS_UNCHECKEDHOT		0x00000002
#define RBS_UNCHECKEDPRESSED	0x00000003
#define RBS_UNCHECKEDDISABLED	0x00000004
#define RBS_CHECKEDNORMAL		0x00000005
//#define RBS_CHECKEDHOT			0x00000006
#define RBS_CHECKEDPRESSED		0x00000007
#define RBS_CHECKEDDISABLED		0x00000008


#define PGIS_BOOLEAN 1
#define PGIS_CHECKED 2
#define PGIS_INDETERMINATE 4
#define PGIS_DISPLAYONLY 8


class CIntegerOffsetFilter : public CIntegerFilter
{

public:
	CIntegerOffsetFilter() {}
	virtual ~CIntegerOffsetFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("0123456789-@");
			return sFilter;
		}
};


class CIntegerListFilter : public CInputFilter
{

public:
	CIntegerListFilter() {}
	virtual ~CIntegerListFilter() {}

// CInputFilter overrides
public:
	const CString& GetFilter() const
		{
			static const CString sFilter = _T("0123456789-|");
			return sFilter;
		}
};


class CPropertyEditCtrl
{
	bool mbChanged;
	size_t mctInitialUndoActions;
	bool mbCancelled;
protected:
	CPropertyGridCtrl* mpGridCtrl;
	size_t midxCell;
	CStudioUndoManager* mpUndoManager;
public:
	CPropertyEditCtrl( CPropertyGridCtrl* pGridCtrl, size_t idxCell )
		: mpGridCtrl( pGridCtrl )
		, midxCell( idxCell )
		, mpUndoManager( pGridCtrl->GetUndoManager() )
		, mbChanged( false )
		, mctInitialUndoActions( 0 )
		, mbCancelled( false )
		{
			if( mpUndoManager )
			{
				mpUndoManager->BeginGroup( theWorkspace.LoadResourceString( IDS_UNDO_CHANGEPROPERTY ) );
				mctInitialUndoActions = mpUndoManager->size();
			}
		}
	virtual ~CPropertyEditCtrl()
		{
			if( !mbCancelled )
			{
				if( mpUndoManager )
				{
					if( !mbChanged )
						mbChanged = (mpUndoManager->size() > mctInitialUndoActions); //in case something changed some other way
					mpUndoManager->EndGroup();
					if( !mbChanged )
					{
						CStudioUndoManager* pUndoManager = mpUndoManager;
						mpUndoManager = NULL;
						pUndoManager->Undo(); //remove undo marks if nothing was changed
					}
				}
			}
		}
	size_t GetCell() const { return midxCell; }
	bool IsCancelled() const { return mbCancelled; }
	bool IsChanged() const { return mbChanged; }
	void SetChanged( bool bChanged = true ) { mbChanged = bChanged; }
	bool IsVaried() const
		{
			return ((mpGridCtrl->GetItemState( midxCell, LVIS_STATEIMAGEMASK ) & INDEXTOSTATEIMAGEMASK(PGIS_INDETERMINATE)) != 0);
		}
	virtual CWnd* GetControlWnd() = 0;
	virtual void OnChanged() //derived class must call this function to mark changed property
		{
			mbChanged = true;
		}
	virtual void OnApply() //must override in derived class to apply new property value
		{
			mbChanged = false;
			if( mpUndoManager && mpUndoManager->IsMultiplePropsChanged() )
				theStudioWorkspace.ActivateDclControl( theStudioWorkspace.GetActiveDclControl() );
		}
	virtual void OnCancel() //must override in derived class to revert to original property value
		{
			mbCancelled = true;
			if( mpUndoManager )
			{
				CStudioUndoManager* pUndoManager = mpUndoManager;
				mpUndoManager = NULL;
				pUndoManager->EndGroup();
				pUndoManager->Undo(); //this should cause the edit control to be destroyed!
			}
			else
				mpGridCtrl->HideEditControls();
		}
};

typedef CPropertyEditCtrl* (*F_EditControlCreator)( CPropertyGridCtrl* pGridCtrl, size_t idxCell, bool bMultiple );

class CCommandButtonEditCtrl : public CDynamicButtonCtrl, public CPropertyEditCtrl
{
	CThemeHelperST mThemeHelper;
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left + 2, rcCell.top, rcCell.right - 2, rcCell.bottom - 1 );
		}
public:
	CCommandButtonEditCtrl( CPropertyGridCtrl* pGridCtrl, size_t idxCell, UINT nCommandId )
		: CPropertyEditCtrl( pGridCtrl, idxCell )
		, CDynamicButtonCtrl( pGridCtrl, CalcRect( pGridCtrl->GetEditRect( idxCell ) ), nCommandId )
		{
			SetThemeHelper( &mThemeHelper );
			SetFlat( FALSE );
			CString sValue = mpGridCtrl->GetItemText( idxCell, 1 );
			if( sValue.IsEmpty() )
				SetWindowText( _T("...") );
			else
				SetWindowText( sValue );
			SetFocus();
		}
	virtual ~CCommandButtonEditCtrl()
		{
			if( !IsCancelled() )
			{
				CString sOldValue = mpGridCtrl->GetItemText( midxCell, 1 );
				CString sNewValue = mpGridCtrl->GetDisplayableValue( midxCell );
				if( sOldValue != sNewValue )
				{
					mpGridCtrl->SetItemText( midxCell, 1, sNewValue );
					SetChanged();
				};
			}
			DestroyWindow();
		}
	virtual CWnd* GetControlWnd() { return this; }
	template< UINT CommandId >
		static CPropertyEditCtrl* Create( CPropertyGridCtrl* pGridCtrl, size_t idxCell, bool bMultiple )
			{ return new CCommandButtonEditCtrl( pGridCtrl, idxCell, CommandId ); }
	virtual BOOL PreTranslateMessage( MSG* pMsg )
		{
			if( __super::PreTranslateMessage( pMsg ) )
				return TRUE;
			if( TranslateMessage( pMsg ) )
			{ // don't send user input events up the chain
				DispatchMessage( pMsg );
				return TRUE;
			}
			return FALSE;
		}
};


class CFilteredTextEditCtrl : public CFilteredEditCtrl, public CPropertyEditCtrl
{
	bool mbUnderConstruction;
	CString msPreviousValue;
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left + 6, rcCell.top + 2, rcCell.right - 1, rcCell.bottom - 1 );
		}
public:
	CFilteredTextEditCtrl( CPropertyGridCtrl* pGridCtrl, size_t idxCell, CInputFilter* pFilter = NULL, DWORD dwStyle = GetDefaultWndStyle(), UINT nID = 100 )
		: CFilteredEditCtrl( pGridCtrl, CalcRect( pGridCtrl->GetEditRect( idxCell ) ), nID, pFilter, dwStyle )
		, CPropertyEditCtrl( pGridCtrl, idxCell )
		, mbUnderConstruction( true )
		{
			SetFont( pGridCtrl->GetFont() );
			CString sValue;
			if( !IsVaried() )
				sValue = mpGridCtrl->GetItemText( idxCell, 1 );
			SetWindowText( sValue );
			SetFocus();
			mbUnderConstruction = false;
		}
	virtual ~CFilteredTextEditCtrl()
		{
			if( !IsCancelled() && IsChanged() )
				OnApply();
			DestroyWindow();
		}
	virtual CWnd* GetControlWnd() { return this; }
	class NullInputFilter{};
	template< typename TInputFilter >
		static CPropertyEditCtrl* Create( CPropertyGridCtrl* pGridCtrl, size_t idxCell, bool bMultiple )
			{ return new CFilteredTextEditCtrl( pGridCtrl, idxCell, new TInputFilter ); }
	template<>
		static CPropertyEditCtrl* Create< NullInputFilter >( CPropertyGridCtrl* pGridCtrl, size_t idxCell, bool bMultiple )
			{ return new CFilteredTextEditCtrl( pGridCtrl, idxCell ); }
	virtual void ApplyValue( LPCTSTR pszValue )
		{
			SetChanged( false );
			mpGridCtrl->SetItemText( midxCell, 1, pszValue );
			CPropertyGridCtrl::TPropertySet PropSet = mpGridCtrl->GetPropertySet( midxCell );
			for( CPropertyGridCtrl::TPropertySet::iterator iter = PropSet.begin();
					 iter != PropSet.end();
					 ++iter )
			{
				TPropertyPtr pProp = *iter;
				TDclControlPtr pDclControl = pProp->GetOwnerControl();
				pProp->SetStringValue( pszValue );
				CDialogControl* pDlgControl = pDclControl? pDclControl->GetControlInstance() : NULL;
				if( pDlgControl )
					CStudioDialogControl::UpdateProperty( pDclControl, pProp->GetID() );
			}
		}
	virtual void OnApply() //must override in derived class to apply new property value
		{
			CString sText;
			GetWindowText( sText );
			CInputFilter* pFilter = GetInputFilter();
			if( pFilter )
			{
				if( !pFilter->OnValidateInput( sText ) )
				{
					sText = pFilter->OnBadInput();
					pFilter->ConvertForDisplay( sText );
				}
			}
			ApplyValue( sText );
			__super::OnApply();
		}
	virtual void OnCancel() //must override in derived class to revert to original property value
		{
			__super::OnCancel();
		}
	virtual BOOL PreTranslateMessage( MSG* pMsg )
		{
			if( pMsg->message == WM_KEYDOWN )
			{
				switch( pMsg->wParam )
				{
					case VK_RETURN:
						pMsg->wParam = 0;
						assert( m_hWnd != NULL );
						OnApply();
						return TRUE;
					case VK_ESCAPE:
						pMsg->wParam = 0;
						OnCancel();
						return TRUE;
				}
			}
			if( __super::PreTranslateMessage( pMsg ) )
				return TRUE;
			if( TranslateMessage( pMsg ) )
			{ // don't send user input events up the chain
				DispatchMessage( pMsg );
				return TRUE;
			}
			return FALSE;
		}

protected:
	DECLARE_MESSAGE_MAP();
	afx_msg void OnChange()
		{
			CString sValue;
			GetWindowText( sValue );
			bool bChanged = sValue != msPreviousValue;
			msPreviousValue = sValue;
			if( bChanged && !mbUnderConstruction )
				OnChanged();
		}
};

BEGIN_MESSAGE_MAP(CFilteredTextEditCtrl, CFilteredEditCtrl)
	ON_CONTROL_REFLECT(EN_CHANGE, &CFilteredTextEditCtrl::OnChange)
END_MESSAGE_MAP()


class CControlNameEditCtrl : public CFilteredTextEditCtrl
{
public:
	CControlNameEditCtrl( CPropertyGridCtrl* pGridCtrl, size_t idxCell, CInputFilter* pFilter = NULL, DWORD dwStyle = GetDefaultWndStyle(), UINT nID = 100 )
		: CFilteredTextEditCtrl( pGridCtrl, idxCell, pFilter, dwStyle, nID )
		{
		}
	virtual ~CControlNameEditCtrl()
		{
		}
	static CPropertyEditCtrl* Create( CPropertyGridCtrl* pGridCtrl, size_t idxCell, bool bMultiple )
		{ return new CControlNameEditCtrl( pGridCtrl, idxCell ); }
	void ApplyValue( LPCTSTR pszValue )
		{
			CPropertyGridCtrl::TPropertySet PropSet = mpGridCtrl->GetPropertySet( midxCell );
			assert( PropSet.size() == 1 );
			TPropertyPtr pProp = PropSet.front();
			TDclControlPtr pDclControl = pProp->GetOwnerControl();
			if( pDclControl )
			{
				if( pDclControl->GetType() == _CtlForm )
				{
					TDclFormPtr pCheckForm = pDclControl->GetOwnerProject()->FindDclForm( pszValue );
					if( pCheckForm && pCheckForm != pDclControl->GetOwnerForm() )
					{
						OnCancel();
						theWorkspace.DisplayAlert( IDS_FORMBADNAME );
						return;
					}
				}
				else
				{
					TDclControlPtr pCheckControl = pDclControl->GetOwnerForm()->FindControl( pszValue );
					if( pCheckControl && pCheckControl != pDclControl )
					{
						OnCancel();
						theWorkspace.DisplayAlert( IDS_CTRLBADNAME );
						return;
					}
				}
			}
			__super::ApplyValue( pszValue );
			theStudioWorkspace.GetTabOrderPane()->Invalidate();
		}
};


class CBooleanCheckBoxCtrl : public CButton, public CPropertyEditCtrl
{
	CString msTrue;
	CString msFalse;
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left + 4, rcCell.top, rcCell.right - 1, rcCell.bottom - 1 );
		}
public:
	CBooleanCheckBoxCtrl( CPropertyGridCtrl* pGridCtrl, size_t idxCell )
		: CButton()
		, CPropertyEditCtrl( pGridCtrl, idxCell )
		, msTrue( theWorkspace.LoadResourceString( IDS_TRUE ) )
		, msFalse( theWorkspace.LoadResourceString( IDS_FALSE ) )
		{
			CString sValue = mpGridCtrl->GetItemText( idxCell, 1 );
			__super::Create( sValue, WS_CHILD | WS_VISIBLE | BS_3STATE, CalcRect( pGridCtrl->GetEditRect( idxCell ) ), pGridCtrl, 100 );
			SetFont( pGridCtrl->GetFont() );
			int nState = pGridCtrl->GetItemState( idxCell, LVIS_STATEIMAGEMASK );
			int nBtnState = BST_UNCHECKED;
			if( (nState & INDEXTOSTATEIMAGEMASK(PGIS_INDETERMINATE)) != 0 )
				nBtnState = BST_INDETERMINATE;
			else if( (nState & INDEXTOSTATEIMAGEMASK(PGIS_CHECKED)) != 0 )
				nBtnState = BST_CHECKED;
			SetCheck( nBtnState );
			SetFocus();
		}
	virtual ~CBooleanCheckBoxCtrl()
		{
			if( !IsCancelled() && IsChanged() )
				OnApply();
			DestroyWindow();
		}
	virtual CWnd* GetControlWnd() { return this; }
	static CPropertyEditCtrl* Create( CPropertyGridCtrl* pGridCtrl, size_t idxCell, bool bMultiple )
		{ return new CBooleanCheckBoxCtrl( pGridCtrl, idxCell ); }
	void ApplyValue( bool bValue )
		{
			SetChanged( false );
			SetWindowText( bValue? msTrue : msFalse );
			mpGridCtrl->SetItemText( midxCell, 1, bValue? msTrue : msFalse );
			int nNewState = 0;
			if( GetCheck() == BST_CHECKED )
				nNewState = INDEXTOSTATEIMAGEMASK(PGIS_CHECKED);
			mpGridCtrl->SetItemState( midxCell, nNewState, INDEXTOSTATEIMAGEMASK(PGIS_CHECKED | PGIS_INDETERMINATE) );
			CPropertyGridCtrl::TPropertySet PropSet = mpGridCtrl->GetPropertySet( midxCell );
			for( CPropertyGridCtrl::TPropertySet::iterator iter = PropSet.begin();
					 iter != PropSet.end();
					 ++iter )
			{
				TPropertyPtr pProp = *iter;
				pProp->SetBooleanValue( bValue );
				TDclControlPtr pDclControl = pProp->GetOwnerControl();
				if( pDclControl )
				{
					CDialogControl* pDlgControl = pDclControl->GetControlInstance();
					if( pDlgControl )
						CStudioDialogControl::UpdateProperty( pDclControl, pProp->GetID() );
				}
			}
		}
	virtual void OnApply() //must override in derived class to apply new property value
		{
			ApplyValue( GetCheck() == BST_CHECKED );
			__super::OnApply();
		}
	virtual void OnCancel() //must override in derived class to revert to original property value
		{
			__super::OnCancel();
		}
	virtual BOOL PreTranslateMessage( MSG* pMsg )
		{
			if( __super::PreTranslateMessage( pMsg ) )
				return TRUE;
			if( TranslateMessage( pMsg ) )
			{ // don't send user input events up the chain
				DispatchMessage( pMsg );
				return TRUE;
			}
			return FALSE;
		}

protected:
	DECLARE_MESSAGE_MAP();
	afx_msg void OnClicked()
		{
			SetCheck( (GetCheck() == BST_UNCHECKED)? BST_CHECKED : BST_UNCHECKED );
			OnChanged();
			OnApply();
		}
};

BEGIN_MESSAGE_MAP(CBooleanCheckBoxCtrl, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CBooleanCheckBoxCtrl::OnClicked)
END_MESSAGE_MAP()


class CEnumComboBoxCtrl : public CComboBox, public CPropertyEditCtrl
{
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left + 1, rcCell.top, rcCell.right - 1, rcCell.bottom - 1 );
		}
public:
	CEnumComboBoxCtrl( CPropertyGridCtrl* pGridCtrl, size_t idxCell )
		: CComboBox()
		, CPropertyEditCtrl( pGridCtrl, idxCell )
		{
			CString sValue = mpGridCtrl->GetItemText( idxCell, 1 );
			__super::Create( WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, CalcRect( pGridCtrl->GetEditRect( idxCell ) ), pGridCtrl, 100 );
			SetFont( pGridCtrl->GetFont() );
			ControlType nCtrlType = _CtlInvalid;
			TDclControlPtr pActiveControl = theStudioWorkspace.GetActiveDclControl();
			if( pActiveControl )
				nCtrlType = pActiveControl->GetType();
			Prop::Id id = pGridCtrl->GetPropertyId( idxCell );
			if( id == Prop::_Private )
			{
				CPropertyGridCtrl::TPropertySet& PropertySet = pGridCtrl->GetPropertySet( idxCell );
				if( !PropertySet.empty() )
				{
					TPropertyPtr pFirstProp = PropertySet.front();
					const AxInterfaceDescriptor* pIDesc = pFirstProp->GetConstAxInterfaceDescriptorPtr();
					if( pIDesc )
					{
						AxPropertyDescriptor* pPropDesc = pIDesc->GetGetDescriptor();
						if( pPropDesc )
						{
							const std::vector< AxPropertyEnum >& Enums( pPropDesc->GetEnum() );
							for( std::vector< AxPropertyEnum >::const_iterator iter = Enums.begin();
									 iter != Enums.end();
									 ++iter )
							{
								const AxPropertyEnum& Enum( *iter );
								int nItem = AddString( Enum.Name );
								LONG lValue = _variant_t( Enum.Var );
								SetItemData( nItem, lValue );
							}
						}
					}
				}
			}
			else
			{
				const TEnumNames& Names = GetPropEnumNames( id, nCtrlType );
				for( TEnumNames::const_iterator iter = Names.begin(); iter != Names.end(); ++iter )
				{
					int nItem = AddString( *iter );
					SetItemData( nItem, nItem );
				}
			}
			int nState = pGridCtrl->GetItemState( idxCell, LVIS_STATEIMAGEMASK );
			if( (nState & INDEXTOSTATEIMAGEMASK(PGIS_INDETERMINATE)) != 0 )
				SetCurSel( -1 );
			else
				SelectString( 0, sValue );
			SetFocus();
		}
	virtual ~CEnumComboBoxCtrl()
		{
			if( !IsCancelled() && IsChanged() )
				OnApply();
			DestroyWindow();
		}
	virtual CWnd* GetControlWnd() { return this; }
	static CPropertyEditCtrl* Create( CPropertyGridCtrl* pGridCtrl, size_t idxCell, bool bMultiple )
		{ return new CEnumComboBoxCtrl( pGridCtrl, idxCell ); }
	void ApplyValue( int nValue )
		{
			SetChanged( false );
			CString sValue;
			GetLBText( nValue, sValue );
			mpGridCtrl->SetItemText( midxCell, 1, sValue );
			CPropertyGridCtrl::TPropertySet PropSet = mpGridCtrl->GetPropertySet( midxCell );
			for( CPropertyGridCtrl::TPropertySet::iterator iter = PropSet.begin();
					 iter != PropSet.end();
					 ++iter )
			{
				TPropertyPtr pProp = *iter;
				pProp->SetLongValue( GetItemData( nValue ) );
				TDclControlPtr pDclControl = pProp->GetOwnerControl();
				if( pDclControl )
				{
					CDialogControl* pDlgControl = pDclControl->GetControlInstance();
					if( pDlgControl )
						CStudioDialogControl::UpdateProperty( pDclControl, pProp->GetID() );
				}
			}
		}
	virtual void OnApply() //must override in derived class to apply new property value
		{
			ApplyValue( GetCurSel() );
			__super::OnApply();
		}
	virtual void OnCancel() //must override in derived class to revert to original property value
		{
			__super::OnCancel();
		}
	virtual BOOL PreTranslateMessage( MSG* pMsg )
		{
			if( __super::PreTranslateMessage( pMsg ) )
				return TRUE;
			if( TranslateMessage( pMsg ) )
			{ // don't send user input events up the chain
				DispatchMessage( pMsg );
				return TRUE;
			}
			return FALSE;
		}

protected:
	DECLARE_MESSAGE_MAP();
	afx_msg void OnSelEndOk()
		{
			OnChanged();
			OnApply();
		}
};

BEGIN_MESSAGE_MAP(CEnumComboBoxCtrl, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELENDOK, &CEnumComboBoxCtrl::OnSelEndOk)
END_MESSAGE_MAP()


class CPicFolderComboBoxCtrl : public CComboBox, public CPropertyEditCtrl
{
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left + 1, rcCell.top, rcCell.right - 1, rcCell.bottom - 1 );
		}
public:
	CPicFolderComboBoxCtrl( CPropertyGridCtrl* pGridCtrl, size_t idxCell )
		: CComboBox()
		, CPropertyEditCtrl( pGridCtrl, idxCell )
		{
			CString sValue = pGridCtrl->GetItemText( idxCell, 1 );
			__super::Create( WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, CalcRect( pGridCtrl->GetEditRect( idxCell ) ), pGridCtrl, 100 );
			SetFont( pGridCtrl->GetFont() );
			AddString( theStudioWorkspace.LoadResourceString( IDS_NONE ) );
			TStudioProjectPtr pProject = theStudioWorkspace.GetActiveProject();
			const TPictureMap& Pictures = pProject->GetPictureMap();
			for( TPictureMap::const_iterator iter = Pictures.begin(); iter != Pictures.end(); ++iter )
			{
				UINT nId = iter->first;
				CString sID;
				sID.Format( _T("%u"), nId );
				AddString( sID );
			}
			int nState = pGridCtrl->GetItemState( idxCell, LVIS_STATEIMAGEMASK );
			if( (nState & INDEXTOSTATEIMAGEMASK(PGIS_INDETERMINATE)) != 0 )
				SetCurSel( -1 );
			else
				SelectString( 0, sValue );
			SetFocus();
		}
	virtual ~CPicFolderComboBoxCtrl()
		{
			if( !IsCancelled() && IsChanged() )
				OnApply();
			DestroyWindow();
		}
	virtual CWnd* GetControlWnd() { return this; }
	static CPropertyEditCtrl* Create( CPropertyGridCtrl* pGridCtrl, size_t idxCell, bool bMultiple )
		{ return new CPicFolderComboBoxCtrl( pGridCtrl, idxCell ); }
	void ApplyValue( int nValue )
		{
			SetChanged( false );
			CString sValue;
			if( GetCurSel() > 0 )
				GetLBText( nValue, sValue );
			mpGridCtrl->SetItemText( midxCell, 1, sValue );
			CPropertyGridCtrl::TPropertySet PropSet = mpGridCtrl->GetPropertySet( midxCell );
			for( CPropertyGridCtrl::TPropertySet::iterator iter = PropSet.begin();
					 iter != PropSet.end();
					 ++iter )
			{
				TPropertyPtr pProp = *iter;
				pProp->SetStringValue( sValue );
				TDclControlPtr pDclControl = pProp->GetOwnerControl();
				if( pDclControl )
				{
					CDialogControl* pDlgControl = pDclControl->GetControlInstance();
					if( pDlgControl )
						CStudioDialogControl::UpdateProperty( pDclControl, pProp->GetID() );
				}
			}
		}
	virtual void OnApply() //must override in derived class to apply new property value
		{
			ApplyValue( GetCurSel() );
			__super::OnApply();
		}
	virtual void OnCancel() //must override in derived class to revert to original property value
		{
			__super::OnCancel();
		}
	virtual BOOL PreTranslateMessage( MSG* pMsg )
		{
			if( __super::PreTranslateMessage( pMsg ) )
				return TRUE;
			if( TranslateMessage( pMsg ) )
			{ // don't send user input events up the chain
				DispatchMessage( pMsg );
				return TRUE;
			}
			return FALSE;
		}

protected:
	DECLARE_MESSAGE_MAP();
	afx_msg void OnSelEndOk()
		{
			OnChanged();
			OnApply();
		}
};

BEGIN_MESSAGE_MAP(CPicFolderComboBoxCtrl, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELENDOK, &CPicFolderComboBoxCtrl::OnSelEndOk)
END_MESSAGE_MAP()


static const F_EditControlCreator PF_UnfilteredTextEditCreator =
	&CFilteredTextEditCtrl::Create< CFilteredTextEditCtrl::NullInputFilter >;

static CPropertyEditCtrl* PF_CreateActiveXPropEditor( CPropertyGridCtrl* pGridCtrl, size_t idxCell, bool bMultiple )
{
	CPropertyGridCtrl::TPropertySet& PropertySet = pGridCtrl->GetPropertySet( idxCell );
	if( PropertySet.empty() )
		return NULL;
	TPropertyPtr pFirstProp = PropertySet.front();
	const AxInterfaceDescriptor* pIDesc = pFirstProp->GetConstAxInterfaceDescriptorPtr();
	if( pIDesc )
	{
		AxPropertyDescriptor* pPropDesc = pIDesc->GetGetDescriptor();
		if( pPropDesc )
		{
			if( pPropDesc->GetGuid() == GUID_COLOR )
			{
				static const F_EditControlCreator PF_OleColorTextEditCreator =
					&CFilteredTextEditCtrl::Create< CIntegerFilter >;
				bool bSupportsColorPropPage = true;
				bool bSupportsStockColorPropPage = true;
				for( CPropertyGridCtrl::TPropertySet::const_iterator iter = PropertySet.begin();
						 iter != PropertySet.end();
						 ++iter )
				{
					TPropertyPtr pProp = *iter;
					const AxInterfaceDescriptor* pIDesc = pFirstProp->GetConstAxInterfaceDescriptorPtr();
					if( !pIDesc )
						return PF_OleColorTextEditCreator( pGridCtrl, idxCell, bMultiple );
					AxPropertyDescriptor* pPropDesc = pIDesc->GetGetDescriptor();
					if( !pPropDesc )
						return PF_OleColorTextEditCreator( pGridCtrl, idxCell, bMultiple );
					TDclControlPtr pControl = pProp->GetOwnerControl();
					if( !pControl )
						return PF_OleColorTextEditCreator( pGridCtrl, idxCell, bMultiple );
					CDialogControl* pDlgControl = pControl->GetControlInstance();
					if( !pDlgControl )
						return PF_OleColorTextEditCreator( pGridCtrl, idxCell, bMultiple );
					CAxContainerCtrl* pAxCtrl = pDlgControl->GetActiveXCtrl();
					if( !pAxCtrl )
						return PF_OleColorTextEditCreator( pGridCtrl, idxCell, bMultiple );
					CArray< CLSID, CLSID& > rclsidPages;
					if( !pAxCtrl->GetPropertyPageCLSIDs( rclsidPages ) )
						return PF_OleColorTextEditCreator( pGridCtrl, idxCell, bMultiple );
					bool bFoundColorPropPage = false;
					bool bFoundStockColorPropPage = false;
					for( INT_PTR idx = rclsidPages.GetUpperBound(); idx >= 0; --idx )
					{
						const CLSID& clsid = rclsidPages.GetAt( idx );
						if( clsid == CLSID_CColorPropPage )
						{
							bFoundColorPropPage = true;
							if( bFoundStockColorPropPage )
								break;
						}
						else if( clsid == CLSID_StockColorPage )
						{
							bFoundStockColorPropPage = true;
							if( bFoundColorPropPage )
								break;
						}
					}
					if( !bFoundColorPropPage )
						bSupportsColorPropPage = false;
					if( !bFoundStockColorPropPage )
						bSupportsStockColorPropPage = false;
					if( !bSupportsColorPropPage && !bSupportsStockColorPropPage )
						return PF_OleColorTextEditCreator( pGridCtrl, idxCell, bMultiple );
				}
				if( bSupportsStockColorPropPage )
					return CCommandButtonEditCtrl::Create< ID_AXSTOCKCOLORPROPERTYPAGE >( pGridCtrl, idxCell, bMultiple );
				return CCommandButtonEditCtrl::Create< ID_AXCOLORPROPERTYPAGE >( pGridCtrl, idxCell, bMultiple );
			}
			else if( pPropDesc->GetGuid() == IID_IFontDisp )
			{
				bool bSupportsFontPropPage = true;
				bool bSupportsStockFontPropPage = true;
				for( CPropertyGridCtrl::TPropertySet::const_iterator iter = PropertySet.begin();
						 iter != PropertySet.end();
						 ++iter )
				{
					TPropertyPtr pProp = *iter;
					const AxInterfaceDescriptor* pIDesc = pFirstProp->GetConstAxInterfaceDescriptorPtr();
					if( !pIDesc )
						return PF_UnfilteredTextEditCreator( pGridCtrl, idxCell, bMultiple );
					AxPropertyDescriptor* pPropDesc = pIDesc->GetGetDescriptor();
					if( !pPropDesc )
						return PF_UnfilteredTextEditCreator( pGridCtrl, idxCell, bMultiple );
					TDclControlPtr pControl = pProp->GetOwnerControl();
					if( !pControl )
						return PF_UnfilteredTextEditCreator( pGridCtrl, idxCell, bMultiple );
					CDialogControl* pDlgControl = pControl->GetControlInstance();
					if( !pDlgControl )
						return PF_UnfilteredTextEditCreator( pGridCtrl, idxCell, bMultiple );
					CAxContainerCtrl* pAxCtrl = pDlgControl->GetActiveXCtrl();
					if( !pAxCtrl )
						return PF_UnfilteredTextEditCreator( pGridCtrl, idxCell, bMultiple );
					CArray< CLSID, CLSID& > rclsidPages;
					if( !pAxCtrl->GetPropertyPageCLSIDs( rclsidPages ) )
						return PF_UnfilteredTextEditCreator( pGridCtrl, idxCell, bMultiple );
					bool bFoundFontPropPage = false;
					bool bFoundStockFontPropPage = false;
					for( INT_PTR idx = rclsidPages.GetUpperBound(); idx >= 0; --idx )
					{
						const CLSID& clsid = rclsidPages.GetAt( idx );
						if( clsid == CLSID_CFontPropPage )
						{
							bFoundFontPropPage = true;
							if( bFoundStockFontPropPage )
								break;
						}
						else if( clsid == CLSID_StockFontPage )
						{
							bFoundStockFontPropPage = true;
							if( bFoundFontPropPage )
								break;
						}
					}
					if( !bFoundFontPropPage )
						bSupportsFontPropPage = false;
					if( !bFoundStockFontPropPage )
						bSupportsStockFontPropPage = false;
					if( !bSupportsFontPropPage && !bSupportsStockFontPropPage )
						return PF_UnfilteredTextEditCreator( pGridCtrl, idxCell, bMultiple );
				}
				if( bSupportsStockFontPropPage )
					return CCommandButtonEditCtrl::Create< ID_AXSTOCKFONTPROPERTYPAGE >( pGridCtrl, idxCell, bMultiple );
				return CCommandButtonEditCtrl::Create< ID_AXFONTPROPERTYPAGE >( pGridCtrl, idxCell, bMultiple );
			}
			else if( pPropDesc->GetGuid() == IID_IPictureDisp )
			{
				bool bSupportsPicturePropPage = true;
				bool bSupportsStockPicturePropPage = true;
				for( CPropertyGridCtrl::TPropertySet::const_iterator iter = PropertySet.begin();
						 iter != PropertySet.end();
						 ++iter )
				{
					TPropertyPtr pProp = *iter;
					const AxInterfaceDescriptor* pIDesc = pFirstProp->GetConstAxInterfaceDescriptorPtr();
					if( !pIDesc )
						return NULL;
					AxPropertyDescriptor* pPropDesc = pIDesc->GetGetDescriptor();
					if( !pPropDesc )
						return NULL;
					TDclControlPtr pControl = pProp->GetOwnerControl();
					if( !pControl )
						return NULL;
					CDialogControl* pDlgControl = pControl->GetControlInstance();
					if( !pDlgControl )
						return NULL;
					CAxContainerCtrl* pAxCtrl = pDlgControl->GetActiveXCtrl();
					if( !pAxCtrl )
						return NULL;
					CArray< CLSID, CLSID& > rclsidPages;
					if( !pAxCtrl->GetPropertyPageCLSIDs( rclsidPages ) )
						return NULL;
					bool bFoundPicturePropPage = false;
					bool bFoundStockPicturePropPage = false;
					for( INT_PTR idx = rclsidPages.GetUpperBound(); idx >= 0; --idx )
					{
						const CLSID& clsid = rclsidPages.GetAt( idx );
						if( clsid == CLSID_CPicturePropPage )
						{
							bFoundPicturePropPage = true;
							if( bFoundStockPicturePropPage )
								break;
						}
						else if( clsid == CLSID_StockPicturePage )
						{
							bFoundStockPicturePropPage = true;
							if( bFoundPicturePropPage )
								break;
						}
					}
					if( !bFoundPicturePropPage )
						bSupportsPicturePropPage = false;
					if( !bFoundStockPicturePropPage )
						bSupportsStockPicturePropPage = false;
					if( !bSupportsPicturePropPage && !bSupportsStockPicturePropPage )
						return NULL;
				}
				if( bSupportsStockPicturePropPage )
					return CCommandButtonEditCtrl::Create< ID_AXSTOCKPICTUREPROPERTYPAGE >( pGridCtrl, idxCell, bMultiple );
				return CCommandButtonEditCtrl::Create< ID_AXPICTUREPROPERTYPAGE >( pGridCtrl, idxCell, bMultiple );
			}
			else if( !pPropDesc->GetEnum().empty() )
			{
				return CEnumComboBoxCtrl::Create( pGridCtrl, idxCell, bMultiple );
			}
		}
	}
	switch( (VT_TYPEMASK & GetActiveXPropType( pFirstProp )) )
	{
	case VT_BOOL:
		return CBooleanCheckBoxCtrl::Create( pGridCtrl, idxCell, bMultiple );
	case VT_INT:
	case VT_I1:
	case VT_I2:
	case VT_I4:
	case VT_I8:
		return CFilteredTextEditCtrl::Create< CIntegerOffsetFilter >( pGridCtrl, idxCell, bMultiple );
	case VT_UINT:
	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
	case VT_UI8:
		return CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >( pGridCtrl, idxCell, bMultiple );
	case VT_DISPATCH:
		return bMultiple? NULL : CCommandButtonEditCtrl::Create< ID_AXPROPERTIES >( pGridCtrl, idxCell, bMultiple );
	}
	return PF_UnfilteredTextEditCreator( pGridCtrl, idxCell, bMultiple );
}


static CString GetDisplayablePropertyValue( TPropertyPtr pProperty )
{
	switch( pProperty->GetType() )
	{
	case PropEnum:
		{
			long nVal = pProperty->GetLongValue();
			const TEnumNames& Names = GetPropEnumNames( pProperty->GetID(), pProperty->GetOwnerControl()->GetType() );
			assert( nVal >= 0 && nVal < (long)Names.size() );
			if( nVal < 0 || nVal >= (long)Names.size() )
				return pProperty->GetStringValue();
			return Names.at( nVal );
		}
	case PropActiveXProp:
		{
			AxPropertyDescriptor* pPropDesc = pProperty->GetConstAxInterfaceDescriptorPtr()->GetGetDescriptor();
			if( pPropDesc )
			{
				if( pPropDesc->GetGuid() == GUID_COLOR )
				{
					OLE_COLOR color = pProperty->GetOLEColorValue();
					switch( color >> 24 )
					{
					case 0:
						{
							CString sColor;
							sColor.Format( _T("%d, %d, %d"), GetRValue(color), GetGValue(color), GetBValue(color) );
							return sColor;
						}
						break;
					case 0x80:
						{
							CString sColor;
							sColor.Format( _T("%d"), -(long)GetRValue(color) );
							return sColor;
						}
						break;
					}
				}
				if( !pPropDesc->GetEnum().empty() )
				{
					CString sEnumName = pPropDesc->GetEnumDisplayName( pProperty->GetLongValue() );
					if( !sEnumName.IsEmpty() )
						return sEnumName;
				}
			}
		}
	}
	return pProperty->GetStringValue();
}

static VARTYPE GetActiveXPropType( TPropertyPtr pProp )
{
	switch( pProp->GetType() )
	{
	case PropActiveXProp:
		{
			const AxInterfaceDescriptor* pDescriptor = pProp->GetConstAxInterfaceDescriptorPtr();
			if( !pDescriptor )
				break;
			const AxPropertyDescriptor* pPropDesc = pDescriptor->GetPropGet();
			if( !pPropDesc )
				break;
			if( pPropDesc->GetArgs().size() == 0 )
				return pPropDesc->GetType();
			if( pPropDesc->GetArgs().size() == 1 )
				return pPropDesc->GetArgs().front().vt;
		}
		break;
	}
	return VT_UNKNOWN;
}

static bool IsBooleanProperty( TPropertyPtr pProp )
{
	switch( pProp->GetType() )
	{
	case PropBool:
		return true;
	case PropActiveXProp:
		if( GetActiveXPropType( pProp ) == VT_BOOL )
			return true;
		break;
	}
	return false;
}

static F_EditControlCreator GetEditControlCreator( Prop::Id id, PropertyType type, bool bMultiple )
{
	switch( id )
	{
	case Prop::ActiveXPropPages: if( bMultiple ) return NULL; return &CCommandButtonEditCtrl::Create< ID_AXPROPERTIES >;
	case Prop::AllowResizing: return &CBooleanCheckBoxCtrl::Create;
	case Prop::AlternatingColor: if( bMultiple ) return &CFilteredTextEditCtrl::Create< CIntegerFilter >; return &CCommandButtonEditCtrl::Create< ID_ALTCOLORPROPERTIES >;
	case Prop::AlternateOrient: return &CEnumComboBoxCtrl::Create;
	case Prop::AsReadOnly: return &CBooleanCheckBoxCtrl::Create;
	case Prop::AutoArrange: return &CBooleanCheckBoxCtrl::Create;
	case Prop::AutoHScroll: return &CBooleanCheckBoxCtrl::Create;
	case Prop::AutoVScroll: return &CBooleanCheckBoxCtrl::Create;
	case Prop::AutoSize: return &CBooleanCheckBoxCtrl::Create;
	case Prop::AutoWrap: return &CBooleanCheckBoxCtrl::Create;
	case Prop::BackgroundColor: if( bMultiple ) return &CFilteredTextEditCtrl::Create< CIntegerFilter >; return &CCommandButtonEditCtrl::Create< ID_BACKCOLORPROPERTIES >;
	case Prop::BeginGroup: return &CBooleanCheckBoxCtrl::Create;
	case Prop::BlockName: return &CFilteredTextEditCtrl::Create< CSymbolNameFilter >;
	case Prop::BlockListStyle: return &CEnumComboBoxCtrl::Create;
	case Prop::BorderStyle: return &CEnumComboBoxCtrl::Create;
	case Prop::BottomFromBottom: return &CFilteredTextEditCtrl::Create< CIntegerOffsetFilter >;
	case Prop::BtnCaption: return PF_UnfilteredTextEditCreator;
	case Prop::BtnToolTips: return PF_UnfilteredTextEditCreator;
	case Prop::Caption: return PF_UnfilteredTextEditCreator;
	case Prop::CaptionMinute: return PF_UnfilteredTextEditCreator;
	case Prop::CaptionMinutes: return PF_UnfilteredTextEditCreator;
	case Prop::CaptionRemaining: return PF_UnfilteredTextEditCreator;
	case Prop::CaptionSecond: return PF_UnfilteredTextEditCreator;
	case Prop::CaptionSeconds: return PF_UnfilteredTextEditCreator;
	case Prop::ColHeader: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ColumnWidth: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::ComboBoxStyle: return &CEnumComboBoxCtrl::Create;
	case Prop::ControlBrowser: if( bMultiple ) return NULL; return &CCommandButtonEditCtrl::Create< ID_CONTROLBROWSER >;
	case Prop::CreationPrompt: return &CBooleanCheckBoxCtrl::Create;
	case Prop::Custom: if( bMultiple ) return NULL; return &CCommandButtonEditCtrl::Create< ID_PROPERTIES >;
	case Prop::CurSelIndex: return &CFilteredTextEditCtrl::Create< CIntegerFilter >;
	case Prop::DisableNoScroll: return &CBooleanCheckBoxCtrl::Create;
	case Prop::DockableSides: return &CEnumComboBoxCtrl::Create;
	case Prop::DragnDropAllowBegin: return &CBooleanCheckBoxCtrl::Create;
	case Prop::DragnDropAllowDrop: return &CBooleanCheckBoxCtrl::Create;
	case Prop::DropDownHeight: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::EditLabels: return &CBooleanCheckBoxCtrl::Create;
	case Prop::Enabled: return &CBooleanCheckBoxCtrl::Create;
	case Prop::EventInvoke: return &CEnumComboBoxCtrl::Create;
	case Prop::ExtCanBeDiff: return &CBooleanCheckBoxCtrl::Create;
	case Prop::FileDlgStyle: return &CEnumComboBoxCtrl::Create;
	case Prop::FileMustExist: return &CBooleanCheckBoxCtrl::Create;
	case Prop::Filter: return PF_UnfilteredTextEditCreator;
	case Prop::FilterStyle: return &CEnumComboBoxCtrl::Create;
	case Prop::FontName: if( bMultiple ) return NULL; return &CCommandButtonEditCtrl::Create< ID_FONTPROPERTIES >;
	case Prop::ForegroundColor: if( bMultiple ) return &CFilteredTextEditCtrl::Create< CIntegerFilter >; return &CCommandButtonEditCtrl::Create< ID_FORECOLORPROPERTIES >;
	case Prop::FullRowSelect: return &CBooleanCheckBoxCtrl::Create;
	case Prop::GraphicButtonStyle: return &CEnumComboBoxCtrl::Create;
	case Prop::GridLines: return &CBooleanCheckBoxCtrl::Create;
	case Prop::HasButtons: return &CBooleanCheckBoxCtrl::Create;
	case Prop::HasLines: return &CBooleanCheckBoxCtrl::Create;
	case Prop::HatchScale: return &CFilteredTextEditCtrl::Create< CNumericFilter >;
	case Prop::Height: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::HScrollBar: return &CBooleanCheckBoxCtrl::Create;
	case Prop::Hyperlink: return PF_UnfilteredTextEditCreator;
	case Prop::IconXSpacing: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::IconYSpacing: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::ImageList: if( bMultiple ) return NULL; return &CCommandButtonEditCtrl::Create< ID_IMAGELISTPROPERTIES >;
	case Prop::Indent: return &CFilteredTextEditCtrl::Create< CNumericFilter >;
	case Prop::InsertOrXref: return &CEnumComboBoxCtrl::Create;
	case Prop::InterfaceMode: return &CEnumComboBoxCtrl::Create;
	case Prop::IsTabStop: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ItemData: return &CFilteredTextEditCtrl::Create< CIntegerListFilter >;
	case Prop::Justification: return &CEnumComboBoxCtrl::Create;
	case Prop::KeepFocus: return &CBooleanCheckBoxCtrl::Create;
	case Prop::LabelAlignment: return &CEnumComboBoxCtrl::Create;
	case Prop::LabelWrap: return &CBooleanCheckBoxCtrl::Create;
	case Prop::LargeChange: return &CFilteredTextEditCtrl::Create< CIntegerFilter >;
	case Prop::Left: return &CFilteredTextEditCtrl::Create< CIntegerOffsetFilter >;
	case Prop::LeftFromRight: return &CFilteredTextEditCtrl::Create< CIntegerOffsetFilter >;
	case Prop::LinesAtRoot: return &CBooleanCheckBoxCtrl::Create;
	case Prop::List: return PF_UnfilteredTextEditCreator;
	case Prop::ListImages: return &CFilteredTextEditCtrl::Create< CIntegerListFilter >;
	case Prop::ListViewIconAlign: return &CEnumComboBoxCtrl::Create;
	case Prop::ListViewSort: return &CEnumComboBoxCtrl::Create;
	case Prop::ListViewStyle: return &CEnumComboBoxCtrl::Create;
	case Prop::MarginLeft: return &CFilteredTextEditCtrl::Create< CIntegerFilter >;
	case Prop::MarginRight: return &CFilteredTextEditCtrl::Create< CIntegerFilter >;
	case Prop::MaxDialogWidth: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::MaxDialogHeight: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::MaxValue: return &CFilteredTextEditCtrl::Create< CIntegerFilter >;
	case Prop::MinDialogWidth: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::MinDialogHeight: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::MinTabWidth: return &CFilteredTextEditCtrl::Create< CIntegerFilter >;
	case Prop::MinValue: return &CFilteredTextEditCtrl::Create< CIntegerFilter >;
	case Prop::MouseOverPicture: return &CPicFolderComboBoxCtrl::Create;
	case Prop::MultiColumn: return &CBooleanCheckBoxCtrl::Create;
	case Prop::MultiRow: return &CBooleanCheckBoxCtrl::Create;
	case Prop::MultiSelect: return &CBooleanCheckBoxCtrl::Create;
	case Prop::Name: if( bMultiple ) return NULL; return &CControlNameEditCtrl::Create;
	case Prop::NoIntegralHeight: return &CBooleanCheckBoxCtrl::Create;
	case Prop::OptionsTabCaption: return PF_UnfilteredTextEditCreator;
	case Prop::Orientation: return &CEnumComboBoxCtrl::Create;
	case Prop::OverWritePrompt: return &CBooleanCheckBoxCtrl::Create;
	case Prop::PathMustExist: return &CBooleanCheckBoxCtrl::Create;
	case Prop::Picture: return &CPicFolderComboBoxCtrl::Create;
	case Prop::ProgressLegend: return &CEnumComboBoxCtrl::Create;
	case Prop::ReadOnly: return &CBooleanCheckBoxCtrl::Create;
	case Prop::RenderMode: return &CEnumComboBoxCtrl::Create;
	case Prop::ReturnAsTab: return &CBooleanCheckBoxCtrl::Create;
	case Prop::RightFromRight: return &CFilteredTextEditCtrl::Create< CIntegerOffsetFilter >;
	case Prop::RowHeader: return &CBooleanCheckBoxCtrl::Create;
	case Prop::RowHeight: return &CFilteredTextEditCtrl::Create< CIntegerFilter >;
	case Prop::SelectionStyle: return &CEnumComboBoxCtrl::Create;
	case Prop::ShowCancel: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ShowHelp: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ShowNameLabel: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ShowNameTextBox: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ShowOK: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ShowOrbitCircles: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ShowReadOnlyCheckBox: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ShowSelectAlways: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ShowTicks: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ShowTypeComboBox: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ShowTypeLabel: return &CBooleanCheckBoxCtrl::Create;
	case Prop::SingleClickExpand: return &CBooleanCheckBoxCtrl::Create;
	case Prop::SmallChange: return &CFilteredTextEditCtrl::Create< CIntegerFilter >;
	case Prop::SmoothProgress: return &CBooleanCheckBoxCtrl::Create;
	case Prop::Sorted: return &CBooleanCheckBoxCtrl::Create;
	case Prop::SplitterMax: return &CFilteredTextEditCtrl::Create< CIntegerOffsetFilter >;
	case Prop::SplitterMin: return &CFilteredTextEditCtrl::Create< CIntegerOffsetFilter >;
	case Prop::SplitterStyle: return &CEnumComboBoxCtrl::Create;
	case Prop::TabFixedWidth: return &CBooleanCheckBoxCtrl::Create;
	case Prop::TabJustification: return &CEnumComboBoxCtrl::Create;
	case Prop::TabsCaption: return PF_UnfilteredTextEditCreator;
	case Prop::TabsImageList: return &CFilteredTextEditCtrl::Create< CIntegerListFilter >;
	case Prop::TabsTTT: return PF_UnfilteredTextEditCreator;
	case Prop::TabStyle: return &CEnumComboBoxCtrl::Create;
	case Prop::Text: return PF_UnfilteredTextEditCreator;
	case Prop::TextLimit: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::TickFrequency: return &CFilteredTextEditCtrl::Create< CIntegerFilter >;
	case Prop::TitleBar: return &CBooleanCheckBoxCtrl::Create;
	case Prop::TitleBarIcon: return &CPicFolderComboBoxCtrl::Create;
	case Prop::TitleBarText: return PF_UnfilteredTextEditCreator;
	case Prop::Top: return &CFilteredTextEditCtrl::Create< CIntegerOffsetFilter >;
	case Prop::ToolTipAviFileName: return PF_UnfilteredTextEditCreator;
	case Prop::ToolTipBalloon: return &CBooleanCheckBoxCtrl::Create;
	//case Prop::ToolTipBody: return PF_UnfilteredTextEditCreator;
	case Prop::ToolTipBody: if( bMultiple ) return NULL; return &CCommandButtonEditCtrl::Create< ID_TOOLTIPPROPERTIES >;
	case Prop::ToolTipLine: return &CBooleanCheckBoxCtrl::Create;
	case Prop::ToolTipPicture: return &CPicFolderComboBoxCtrl::Create;
	case Prop::ToolTipTitle: return PF_UnfilteredTextEditCreator;
	case Prop::ToolTipTitleColor: return &CFilteredTextEditCtrl::Create< CIntegerFilter >;
	case Prop::TopFromBottom: return &CFilteredTextEditCtrl::Create< CIntegerOffsetFilter >;
	case Prop::URLLinkType: return &CEnumComboBoxCtrl::Create;
	case Prop::UseBottomFromBottom: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::UseLeftFromRight: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::UseRightFromRight: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::UseTopFromBottom: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::UseVisualStyle: return &CBooleanCheckBoxCtrl::Create;
	case Prop::Value: if( type == PropLong ) return &CFilteredTextEditCtrl::Create< CIntegerOffsetFilter >; return &CEnumComboBoxCtrl::Create;
	case Prop::VarName: if( bMultiple ) return NULL; return &CFilteredTextEditCtrl::Create< CSymbolNameFilter >;
	case Prop::Visible: return &CBooleanCheckBoxCtrl::Create;
	case Prop::VScrollBar: return &CBooleanCheckBoxCtrl::Create;
	case Prop::Width: return &CFilteredTextEditCtrl::Create< CUnsignedIntegerFilter >;
	case Prop::_Private: return PF_CreateActiveXPropEditor;
	};
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPropertyGridCtrl

CPropertyGridCtrl::CPropertyGridCtrl( CPropertiesTabPane* pParent, const std::vector< TDclControlPtr >& ActiveControls )
: mControls( ActiveControls )
, mpParent( pParent )
, mpPropertyEditCtrl( NULL )
{
}

CPropertyGridCtrl::~CPropertyGridCtrl()
{
	HideEditControls();
}

//static
const UINT& CPropertyGridCtrl::refWM_CHECKFOCUS()
{
	static const UINT WM_CHECKFOCUS = RegisterWindowMessage( _T("OpenDCL.Grid.CheckFocus") );
	return WM_CHECKFOCUS;
}

#undef SubclassWindow
#define LVS_EX_LABELTIP         0x00004000 // listview unfolds partly hidden labels if it does not have infotip text
bool CPropertyGridCtrl::Create( CWnd* pParentWnd, const CRect& rcWnd, UINT nID )
{
	DWORD dwStyle = LVS_OWNERDRAWFIXED | LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_NOSORTHEADER | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
	if( !__super::Create( WS_CHILD | WS_VISIBLE | dwStyle, rcWnd, pParentWnd, nID ) )
		return FALSE;
	SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_LABELTIP );
	InsertColumn( 0, _T("Property"), LVCFMT_LEFT, 60 );
	InsertColumn( 1, _T("Value"), LVCFMT_LEFT, 60 );
	OnActivateDclControl( NULL );
	return true;
}

CStudioUndoManager* CPropertyGridCtrl::GetUndoManager() const
{
	if( mControls.empty() )
		return NULL;
	return (CStudioUndoManager*)mControls.front()->GetUndoManager();
}

void CPropertyGridCtrl::OnActivateDclControl( TDclControlPtr pDclControl )
{
	HideEditControls();
	DeleteAllItems();
	mProperties.clear();
	mrPropIndex.clear();
	if( !pDclControl )
		return;

	for( std::vector< TDclControlPtr >::const_iterator iterControl = mControls.begin();
			 iterControl != mControls.end();
			 ++iterControl )
	{
		TPropertyList& PropList = (*iterControl)->GetPropertyList();
		for( TPropertyList::const_iterator iterProp = PropList.begin();
				 iterProp != PropList.end();
				 ++iterProp )
		{
			TPropertyPtr pProp = *iterProp;
			if( pProp->IsHidden() )
				continue;
			if( pProp->GetType() == PropActiveXProp )
			{
				AxPropertyDescriptor* pPropPutDesc = pProp->GetConstAxInterfaceDescriptorPtr()->GetPutDescriptor();
				if( pPropPutDesc && pPropPutDesc->GetArgs().size() != 1 )
					continue;
				AxPropertyDescriptor* pPropGetDesc = pProp->GetConstAxInterfaceDescriptorPtr()->GetGetDescriptor();
				if( pPropGetDesc && !pPropGetDesc->GetArgs().empty() )
					continue;
				if( !pPropPutDesc && pPropGetDesc->GetType() == VT_DISPATCH )
					continue;
			}
			mProperties[pProp->GetName()].push_back( pProp );
		}
	}
	//bool bNoPics = (pDclControl->GetOwnerProject()->GetPictureMap().empty());
	for( TPropertyMap::const_iterator iterPropName = mProperties.begin();
			 iterPropName != mProperties.end();
			 ++iterPropName )
	{
		const TPropertySet& PropSet = iterPropName->second;
		assert( !PropSet.empty() );
		if( PropSet.empty() )
			continue;
		CString sPropName = iterPropName->first;
		int idxProp = mrPropIndex.size();
		mrPropIndex.push_back( sPropName );
		InsertItem( idxProp, sPropName );
		TPropertyPtr pFirstProp = PropSet.front();
		CString sPropVal = ::GetDisplayablePropertyValue( pFirstProp );
		UINT nState = 0;
		if( IsBooleanProperty( pFirstProp ) )
		{
			nState |= PGIS_BOOLEAN;
			if( pFirstProp->GetBooleanValue() )
				nState |= PGIS_CHECKED;
		}
		for( TPropertySet::const_iterator iterProp = PropSet.begin() + 1;
				 iterProp != PropSet.end();
				 ++iterProp )
		{
			if( sPropVal == GetDisplayablePropertyValue( *iterProp ) )
				continue;
			//found one property with a different value, so display indeterminate
			nState = PGIS_INDETERMINATE;
			sPropVal = theWorkspace.LoadResourceString( IDS_VARIES );
			break; //no need to keep checking
		}
		switch( pFirstProp->GetID() )
		{
		case Prop::ControlBrowser:
		case Prop::Name:
		case Prop::VarName:
		case Prop::ActiveXPropPages:
			if( PropSet.size() > 1 )
				nState |= PGIS_DISPLAYONLY;
			break;
		//case Prop::TitleBarIcon:
		//case Prop::Picture:
		//case Prop::MouseOverPicture:
		//case Prop::ToolTipPicture:
		//	if( bNoPics )
		//		nState |= PGIS_DISPLAYONLY;
		//	break;
		case Prop::_Private:
			{
				const AxInterfaceDescriptor* pDescriptor = pFirstProp->GetConstAxInterfaceDescriptorPtr();
				if( !pDescriptor )
				{
					nState |= PGIS_DISPLAYONLY;
					break;
				}
				AxPropertyDescriptor* pPropDesc = pDescriptor->GetPutDescriptor();
				if( !pPropDesc )
				{
					nState |= PGIS_DISPLAYONLY;
					break;
				}
				const AxArg& arg = pPropDesc->GetArgs().front();
				switch( (VT_TYPEMASK & arg.vt) )
				{ //if it's not a type we can set in the property grid, make it display-only
				case VT_I2:
				case VT_I4:
				case VT_R4:
				case VT_R8:
				case VT_CY:
				case VT_BSTR:
				case VT_BOOL:
				case VT_I1:
				case VT_UI1:
				case VT_UI2:
				case VT_UI4:
				case VT_I8:
				case VT_UI8:
				case VT_INT:
				case VT_UINT:
					break;
				case VT_DISPATCH:
					if( arg.clsid == GUID_COLOR )
						break;
					if( arg.clsid == IID_IPictureDisp )
						break;
					if( arg.clsid == IID_IFontDisp )
						break;
					nState |= PGIS_DISPLAYONLY;
					break;
				default:
					nState |= PGIS_DISPLAYONLY;
					break;
				}
			}
			break;
		}
		if( pFirstProp->IsReadOnly() )
			nState |= PGIS_DISPLAYONLY;
		SetItemState( idxProp, INDEXTOSTATEIMAGEMASK(nState), LVIS_STATEIMAGEMASK );
		SetItemText( idxProp, 1, sPropVal );
	}
	RecalcLayout();
}

void CPropertyGridCtrl::HideEditControls()
{
	if( mpPropertyEditCtrl )
		OnEndEditCurCell();
}

void CPropertyGridCtrl::RecalcLayout()
{
	CRect rcClient;
	GetClientRect( &rcClient );
	int nWidth = rcClient.Width();
	if( nWidth < 100 )
		nWidth = 100;
	SetColumnWidth( 1, nWidth / 2 );
	SetColumnWidth( 0, nWidth / 2 );
	Invalidate();
}

int CPropertyGridCtrl::GetCurItem() const
{
	POSITION pos = GetFirstSelectedItemPosition();
	return GetNextSelectedItem( pos );
}

void CPropertyGridCtrl::OnEditCurCell()
{
	HideEditControls();
	int nCurCell = GetCurItem();
	if( nCurCell < 0 )
		return;
	mpPropertyEditCtrl = CreateEditControl( nCurCell );
}

void CPropertyGridCtrl::OnEndEditCurCell()
{
	CPropertyEditCtrl* pCellEditCtrl = mpPropertyEditCtrl;
	mpPropertyEditCtrl = NULL;
	if( pCellEditCtrl )
	{
		size_t idxCell = pCellEditCtrl->GetCell();
		delete pCellEditCtrl;
		CRect rcCell;
		GetSubItemRect( idxCell, 1, LVIR_BOUNDS, rcCell );
		InvalidateRect( &rcCell );
	}
}

CPropertyEditCtrl* CPropertyGridCtrl::CreateEditControl( size_t idxCell )
{
	Prop::Id id = GetPropertyId( idxCell );
	PropertyType type = GetPropertyType( idxCell );
	UINT nState = GetItemState( idxCell, LVIS_STATEIMAGEMASK );
	if( (nState & INDEXTOSTATEIMAGEMASK(PGIS_DISPLAYONLY)) != 0 )
		return NULL; //can't edit when in display-only state
	//bool bVaries = ((nState & INDEXTOSTATEIMAGEMASK(PGIS_INDETERMINATE)) != 0);
	bool bMultiple = (GetPropertySet( idxCell ).size() > 1);
	F_EditControlCreator pfCreator = GetEditControlCreator( id, type, bMultiple );
	if( !pfCreator )
		return NULL;
	return pfCreator( this, idxCell, bMultiple );
}

Prop::Id CPropertyGridCtrl::GetPropertyId( size_t idxCell )
{
	TPropertySet& PropSet = GetPropertySet( idxCell );
	assert( !PropSet.empty() );
	if( PropSet.empty() )
		return Prop::_Private;
	return PropSet.front()->GetID();
}

PropertyType CPropertyGridCtrl::GetPropertyType( size_t idxCell )
{
	TPropertySet& PropSet = GetPropertySet( idxCell );
	assert( !PropSet.empty() );
	if( PropSet.empty() )
		return PropInvalid;
	return PropSet.front()->GetType();
}

CPropertyGridCtrl::TPropertySet& CPropertyGridCtrl::GetPropertySet( size_t idxCell )
{
	assert( idxCell < mrPropIndex.size() );
	return mProperties[mrPropIndex[idxCell]];
}

CRect CPropertyGridCtrl::GetEditRect(	size_t idxCell  )
{
	ASSERT( idxCell < (size_t)GetItemCount() );
	CRect rcCell;
	GetSubItemRect( idxCell, 1, LVIR_BOUNDS, rcCell );
	return rcCell;
}

CString CPropertyGridCtrl::GetDisplayableValue( size_t idxCell )
{
	const TPropertySet& PropSet = GetPropertySet( idxCell );
	if( PropSet.empty() )
		return NULL;
	TPropertyPtr pFirstProp = PropSet.front();
	CString sPropVal = GetDisplayablePropertyValue( pFirstProp );
	for( TPropertySet::const_iterator iterProp = PropSet.begin() + 1;
			 iterProp != PropSet.end();
			 ++iterProp )
	{
		if( sPropVal == GetDisplayablePropertyValue( *iterProp ) )
			continue;
		//found one property with a different value, so display indeterminate
		sPropVal = theWorkspace.LoadResourceString( IDS_VARIES );
		break; //no need to keep checking
	}
	return sPropVal;
}

CRect CPropertyGridCtrl::GetCellRect( int nRow, int nCol, int area /*= LVIR_BOUNDS*/ )
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

bool CPropertyGridCtrl::CellHitTest( const CPoint& point, int& nRow, int& nCol )
{
	for( int idxRow = GetItemCount() - 1; idxRow >= 0; --idxRow )
	{
		CRect rcRow;
		GetItemRect( idxRow, &rcRow, LVIR_BOUNDS );
		if( rcRow.PtInRect( point ) )
		{
			nRow = idxRow;
			if( GetCellRect( idxRow, 1, LVIR_BOUNDS ).PtInRect( point ) )
				nCol = 1;
			else
				nCol = 0;
			return true;
		}
	}
	return false;
}

void CPropertyGridCtrl::DrawBooleanProperty( CDC& cdc, const CRect& rcIcon, int nState )
{
	cdc.SaveDC();
	cdc.SetBkMode( TRANSPARENT );
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
																 nState, &rc, NULL );
		pTheme->CloseThemeData( hTheme );
	}
	else
	{
		cdc.DrawEdge( &rc, EDGE_SUNKEN, BF_RECT );
		rc.DeflateRect( 2, 2 );
		COLORREF crFill = cdc.GetBkColor();
		if( nState == RBS_UNCHECKEDDISABLED || nState == RBS_CHECKEDDISABLED )
			crFill = GetSysColor( COLOR_GRAYTEXT );
		cdc.FillSolidRect( &rc, crFill );

		if( nState == RBS_CHECKEDNORMAL || nState == RBS_CHECKEDDISABLED || nState == RBS_CHECKEDPRESSED )
		{
			rc.InflateRect( 4, 4 );
			COLORREF crCheck = RGB(0,0,0);
			if( nState == RBS_CHECKEDPRESSED )
				crCheck = RGB(64,64,64);
			CPen penCheck( PS_SOLID, 1, crCheck );
			CPen* pOldPen = cdc.SelectObject( &penCheck );
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
	rc = rcIcon;
	if( rc.Width() > 16 )
	{
		rc.left += 16;
		CString sText;
		if( nState == RBS_UNCHECKEDPRESSED || nState == RBS_CHECKEDPRESSED )
			sText = theWorkspace.LoadResourceString( IDS_VARIES );
		else if( nState == RBS_CHECKEDNORMAL || nState == RBS_CHECKEDDISABLED )
			sText = theWorkspace.LoadResourceString( IDS_TRUE );
		else
			sText = theWorkspace.LoadResourceString( IDS_FALSE );
		cdc.DrawText( sText, -1, &rc, DT_NOPREFIX | DT_LEFT | DT_SINGLELINE );
	}
	cdc.RestoreDC( -1 );
}

void CPropertyGridCtrl::DrawColor( CDC& cdc, const CRect& rcIcon, int nColor, const CString& sText )
{
/*
	COLORREF crFill = RGB(255,255,255);
	if( nColor >= 0 && nColor <= 256 )
		crFill = GetRGBColor( nColor );
	else if( !sText.IsEmpty() )
	{
		int idxComma = sText.Find( _T(",") );
		CString sRed = sText.Left( idxComma );
		CString sGreen = sText.Mid( idxComma + 1 );
		idxComma = sGreen.Find( _T(",") );
		CString sBlue = sGreen.Mid( idxComma + 1 );
		sGreen = sGreen.Left( idxComma );
		crFill = RGB(_tstol( sRed ), _tstol( sGreen ), _tstol( sBlue ) );
	}
	cdc.Rectangle( &rcIcon );
	CRect rcFill = rcIcon;
	rcFill.DeflateRect( 1, 1 );
	CBrush brFill( crFill );
	cdc.FillRect( &rcFill, &brFill );
*/
}

void CPropertyGridCtrl::ShowPropertyPages( ULONG ctPages, CLSID FAR* lpPages, LPCTSTR pszCaption )
{
	if( !mpPropertyEditCtrl )
		return;
	TPropertySet& PropertySet = GetPropertySet( mpPropertyEditCtrl->GetCell() );
	if( PropertySet.empty() )
		return;
	CArray< IUnknown*, IUnknown* > rpUnknown;
	HWND hwndHost = NULL;
	DISPID dispid = DISPID_UNKNOWN;
	CArray< CLSID, CLSID& > rPages;
	for( TPropertySet::const_iterator iter = PropertySet.begin();
			 iter != PropertySet.end();
			 ++iter )
	{
		TPropertyPtr pProp = *iter;
		if( dispid == DISPID_UNKNOWN )
		{
			const AxInterfaceDescriptor* pPropDesc = pProp->GetConstAxInterfaceDescriptorPtr();
			if( pPropDesc )
			{
				AxPropertyDescriptor* pPutDesc = pPropDesc->GetPutDescriptor();
				if( pPutDesc )
					dispid = pPutDesc->GetDispId();
			}
		}
		TDclControlPtr pControl = pProp->GetOwnerControl();
		if( pControl )
		{
			CDialogControl* pDlgControl = pControl->GetControlInstance();
			if( pDlgControl )
			{
				if( !hwndHost )
					hwndHost = pDlgControl->GetControlPane()->GetHostDialog()->m_hWnd;
				CAxContainerCtrl* pAxCtrl = pDlgControl->GetActiveXCtrl();
				if( pAxCtrl )
				{
					if( ctPages == 0 )
					{
						CArray< CLSID, CLSID& > rCtrlPages;
						if( !pAxCtrl->GetPropertyPageCLSIDs( rCtrlPages ) || rCtrlPages.IsEmpty() )
							return;
						if( rPages.IsEmpty() )
							rPages.Append( rCtrlPages );
						else
						{
							for( INT_PTR idx = rPages.GetUpperBound(); idx >= 0; --idx )
							{
								const CLSID& clsid = rPages.GetAt( idx );
								INT_PTR idxCtrlPage;
								for( idxCtrlPage = rCtrlPages.GetUpperBound(); idxCtrlPage >= 0; --idxCtrlPage )
								{
									if( rCtrlPages.GetAt( idxCtrlPage ) != clsid )
										continue;
									break;
								}
								if( idxCtrlPage >= 0 )
									continue;
								rPages.RemoveAt( idx );
								if( rPages.IsEmpty() )
									return;
							}
						}
					}
					IUnknown* pUnknown = pAxCtrl->GetControlUnknown(); //not ref counted; no need to addref/release
					if( pUnknown )
						rpUnknown.Add( pUnknown );
				}
			}
		}
	}
	CWnd* pActiveWnd = GetActiveWindow();
	CLSID FAR * pPropPages = lpPages;
	ULONG ctPropPages = ctPages;
	if( ctPropPages == 0 )
	{
		ctPropPages = rPages.GetCount();
		pPropPages = rPages.GetData();
	}
	OCPFIPARAMS params =
	{
		sizeof(OCPFIPARAMS),
		hwndHost,
		0,
		0,
		bstr_t( pszCaption ),
		rpUnknown.GetSize(),
		rpUnknown.GetData(),
		ctPropPages,
		pPropPages,
		GetUserDefaultLCID(),
		dispid,
	};
	if( S_OK == OleCreatePropertyFrameIndirect( &params ) )
	{
		if( pActiveWnd )
			pActiveWnd->SetActiveWindow();
		for( TPropertySet::const_iterator iter = PropertySet.begin();
				 iter != PropertySet.end();
				 ++iter )
		{
			TPropertyPtr pProp = *iter;
			TDclControlPtr pControl = pProp->GetOwnerControl();
			if( pControl )
			{
				CDialogControl* pDlgControl = pControl->GetControlInstance();
				if( pDlgControl )
				{
					if( !hwndHost )
						hwndHost = pDlgControl->GetControlPane()->GetHostDialog()->m_hWnd;
					CAxContainerCtrl* pAxCtrl = pDlgControl->GetActiveXCtrl();
					if( pAxCtrl )
						pAxCtrl->SaveToStream();
				}
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CPropertyGridCtrl, CListCtrl)
	ON_WM_NCCALCSIZE()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_WINDOWPOSCHANGING()
	ON_REGISTERED_MESSAGE(refWM_CHECKFOCUS(), &CPropertyGridCtrl::OnCheckFocus)
	ON_NOTIFY_REFLECT(LVN_BEGINSCROLL, &CPropertyGridCtrl::OnLvnBeginScroll)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CPropertyGridCtrl::OnLvnItemchanged)
	ON_COMMAND(ID_PROPERTIES, &CPropertyGridCtrl::OnProperties)
	ON_COMMAND(ID_FONTPROPERTIES, &CPropertyGridCtrl::OnFontProperties)
	ON_COMMAND(ID_TOOLTIPPROPERTIES, &CPropertyGridCtrl::OnTooltipProperties)
	ON_COMMAND(ID_FORECOLORPROPERTIES, &CPropertyGridCtrl::OnForeColorProperties)
	ON_COMMAND(ID_BACKCOLORPROPERTIES, &CPropertyGridCtrl::OnBackColorProperties)
	ON_COMMAND(ID_ALTCOLORPROPERTIES, &CPropertyGridCtrl::OnAltColorProperties)
	ON_COMMAND(ID_IMAGELISTPROPERTIES, &CPropertyGridCtrl::OnImageListProperties)
	ON_COMMAND(ID_CONTROLBROWSER, &CPropertyGridCtrl::OnObjectbrowser)
	ON_COMMAND(ID_AXPROPERTIES, &CPropertyGridCtrl::OnAxProperties)
	ON_COMMAND(ID_AXCOLORPROPERTYPAGE, &CPropertyGridCtrl::OnAxColorPropertyPage)
	ON_COMMAND(ID_AXSTOCKCOLORPROPERTYPAGE, &CPropertyGridCtrl::OnAxStockColorPropertyPage)
	ON_COMMAND(ID_AXFONTPROPERTYPAGE, &CPropertyGridCtrl::OnAxFontPropertyPage)
	ON_COMMAND(ID_AXSTOCKFONTPROPERTYPAGE, &CPropertyGridCtrl::OnAxStockFontPropertyPage)
	ON_COMMAND(ID_AXPICTUREPROPERTYPAGE, &CPropertyGridCtrl::OnAxPicturePropertyPage)
	ON_COMMAND(ID_AXSTOCKPICTUREPROPERTYPAGE, &CPropertyGridCtrl::OnAxStockPicturePropertyPage)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyGridCtrl message handlers

void CPropertyGridCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	HideEditControls();
	UpdateWindow();
	__super::OnNcCalcSize(bCalcValidRects, lpncsp);
}

BOOL CPropertyGridCtrl::PreTranslateMessage(MSG* pMsg)
{
	if( __super::PreTranslateMessage(pMsg) )
		return TRUE;
	if( pMsg->message == WM_KEYDOWN )
	{
		switch( pMsg->wParam )
		{
		case VK_UP:
			{
				if( IsEditing() )
					HideEditControls();
				int idxCurCell = GetCurItem();
				if( idxCurCell < 0 )
					idxCurCell = GetItemCount();
				if( idxCurCell > 0 )
				{
					SetHotItem( --idxCurCell );
					SetItemState( idxCurCell, LVIS_SELECTED, LVIS_SELECTED );
				}
			}
			return TRUE;
		case VK_DOWN:
			{
				if( IsEditing() )
					HideEditControls();
				int idxCurCell = GetCurItem();
				if( idxCurCell < GetItemCount() - 1 )
				{
					SetHotItem( ++idxCurCell );
					SetItemState( idxCurCell, LVIS_SELECTED, LVIS_SELECTED );
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

void CPropertyGridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nRow = -1;
	int nCol = -1;
	if( CellHitTest( point, nRow, nCol ) )
	{
		SetHotItem( nRow );
		SetItemState( nRow, LVIS_SELECTED, LVIS_SELECTED );
		OnEditCurCell();
		if( mpPropertyEditCtrl )
		{
			CWnd* pCtrlWnd = mpPropertyEditCtrl->GetControlWnd();
			if( pCtrlWnd )
			{
				CPoint ptCtrl = point;
				ClientToScreen( &ptCtrl );
				pCtrlWnd->ScreenToClient( &ptCtrl );
				CRect rcClient;
				pCtrlWnd->GetClientRect( &rcClient );
				if( rcClient.PtInRect( ptCtrl ) )
					pCtrlWnd->SendMessage( WM_LBUTTONDOWN, nFlags, MAKELPARAM(ptCtrl.x, ptCtrl.y) );
			}
		}
		return;
	}
	HideEditControls();
	__super::OnLButtonDown(nFlags, point);
}

void CPropertyGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nRow = -1;
	int nCol = -1;
	if( !CellHitTest( point, nRow, nCol ) || nRow < 0 || nCol != 0 )
	{
		__super::OnLButtonDblClk(nFlags, point);
		return;
	}
	TPropertySet Props = GetPropertySet( nRow );
	if( Props.size() != 1 )
	{
		__super::OnLButtonDblClk(nFlags, point);
		return;
	}
	HideEditControls();
	CControlBrowser ControlBrowserDlg( Props.front() );
	ControlBrowserDlg.DoModal();
}

void CPropertyGridCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 18;
}

void CPropertyGridCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( GetFocus() != this)
		SetFocus();
	PostMessage( refWM_CHECKFOCUS(), 0, 0 );
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPropertyGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( GetFocus() != this)
		SetFocus();
	PostMessage( refWM_CHECKFOCUS(), 0, 0 );
	__super::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CPropertyGridCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bResult = __super::OnNotify(wParam, lParam, pResult);
	if( ((NMHDR*)lParam)->code == NM_KILLFOCUS )
		PostMessage( refWM_CHECKFOCUS(), 0, 0 );
	return bResult;
}

BOOL CPropertyGridCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
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

void CPropertyGridCtrl::OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	PostMessage( refWM_CHECKFOCUS(), 0, 0 );
	*pResult = 0;
}

LRESULT CPropertyGridCtrl::OnCheckFocus( WPARAM wParam, LPARAM lParam )
{
	CWnd* pFocusWnd = GetFocus();
	if( !pFocusWnd || pFocusWnd->GetParent() != this )
		HideEditControls();
	return 0;
}

void CPropertyGridCtrl::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if( (lpwndpos->flags & SWP_NOSIZE) == 0 )
	{
		int nWidth = lpwndpos->cx;
		int ctItems = GetItemCount();
		if( ctItems > 0 )
		{
			CRect rcItem;
			GetItemRect( 0, &rcItem, LVIR_BOUNDS );
			if( (rcItem.Height() * ctItems) > lpwndpos->cy )
				nWidth -= GetSystemMetrics( SM_CXVSCROLL );
		}
		if( nWidth < 100 )
			nWidth = 100;
		SetColumnWidth( 0, nWidth / 2 );
		SetColumnWidth( 1, nWidth / 2 );
		Invalidate();
	}
	__super::OnWindowPosChanging(lpwndpos);
}

void CPropertyGridCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( lpDrawItemStruct->CtlType != ODT_LISTVIEW )
		return;
	int nRow = lpDrawItemStruct->itemID;
	UINT nItemState = lpDrawItemStruct->itemState;
	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );

	pDC->SaveDC();
	COLORREF crBackground = pDC->GetBkColor();
	pDC->SetBkMode( TRANSPARENT );
	if( (nItemState & LVIS_FOCUSED) )
		crBackground = GetSysColor( COLOR_INACTIVECAPTION );
	CRect rcCell = GetCellRect( nRow, 0 );
	CRect rcClip;
	pDC->GetClipBox( &rcClip );
	pDC->IntersectClipRect( &rcCell );
	pDC->FillSolidRect( &rcCell, crBackground );
	pDC->DrawText( GetItemText( nRow, 0 ), -1, &lpDrawItemStruct->rcItem, DT_NOPREFIX | DT_LEFT | DT_SINGLELINE );
	pDC->SelectClipRgn( NULL );
	if( !mpPropertyEditCtrl || mpPropertyEditCtrl->GetCell() != nRow )
	{ //skip drawing if the edit control is currently displayed for this item
		COLORREF crText = pDC->GetTextColor();
		UINT nCustomState = GetItemState( nRow, LVIS_STATEIMAGEMASK );
		if( (nCustomState & INDEXTOSTATEIMAGEMASK(PGIS_DISPLAYONLY)) != 0 )
			pDC->SetTextColor( GetSysColor( COLOR_GRAYTEXT ) );
		//Prop::Id id = GetPropertyId( nRow );
		//PropertyType type = GetPropertyType( nRow );
		bool bVaries = ((nCustomState & INDEXTOSTATEIMAGEMASK(PGIS_INDETERMINATE)) != 0);
		rcCell = GetCellRect( nRow, 1 );
		if( (nCustomState & INDEXTOSTATEIMAGEMASK(PGIS_BOOLEAN)) != 0 )
		{
			if( rcCell.right - rcCell.left > 4 )
				rcCell.left += 4;
			rcCell.top += 2;
			rcCell.bottom -= 1;
			int nDrawState = RBS_UNCHECKEDNORMAL;
			if( bVaries )
				nDrawState = RBS_UNCHECKEDPRESSED;
			else if( (nCustomState & INDEXTOSTATEIMAGEMASK(PGIS_CHECKED)) != 0 )
				nDrawState = RBS_CHECKEDNORMAL;
			DrawBooleanProperty( *pDC, rcCell, nDrawState );
		}
		else
		{
			rcCell.left += 6;
			rcCell.top += 2;
			pDC->DrawText( GetItemText( nRow, 1 ), -1, &rcCell, DT_NOPREFIX | DT_LEFT | DT_SINGLELINE );
		}
		pDC->SetTextColor( crText );
	}

	////Draw focus rect if row is current
	//if( mCurrentCell.row() == nRow && mCurrentCell.col() == -1 )
	//{
	//	CRect rcFocus;
	//	GetItemRect( nRow, &rcFocus, LVIR_BOUNDS );
	//	rcFocus.DeflateRect( 1, 1 );
	//	pDC->DrawFocusRect( &rcFocus );
	//}
	pDC->RestoreDC( -1 );
	return;
}

void CPropertyGridCtrl::OnProperties()
{
	theStudioWorkspace.GetStudioFrame()->PostMessage( WM_COMMAND, ID_PROPERTIES, (LPARAM)0 );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnFontProperties()
{
	theStudioWorkspace.GetStudioFrame()->PostMessage( WM_COMMAND, ID_FONTPROPERTIES, (LPARAM)0 );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnTooltipProperties()
{
	theStudioWorkspace.GetStudioFrame()->PostMessage( WM_COMMAND, ID_TOOLTIPPROPERTIES, (LPARAM)0 );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnForeColorProperties()
{
	theStudioWorkspace.GetStudioFrame()->PostMessage( WM_COMMAND, ID_FORECOLORPROPERTIES, (LPARAM)0 );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnBackColorProperties()
{
	theStudioWorkspace.GetStudioFrame()->PostMessage( WM_COMMAND, ID_BACKCOLORPROPERTIES, (LPARAM)0 );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnAltColorProperties()
{
	theStudioWorkspace.GetStudioFrame()->PostMessage( WM_COMMAND, ID_ALTCOLORPROPERTIES, (LPARAM)0 );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnImageListProperties()
{
	theStudioWorkspace.GetStudioFrame()->PostMessage( WM_COMMAND, ID_IMAGELISTPROPERTIES, (LPARAM)0 );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnObjectbrowser()
{
	theStudioWorkspace.GetStudioFrame()->PostMessage( WM_COMMAND, ID_CONTROLBROWSER, (LPARAM)0 );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnAxProperties()
{
	ShowPropertyPages( 0, NULL, NULL );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnAxColorPropertyPage()
{
	ShowPropertyPages( 1, &CLSID( CLSID_CColorPropPage ), NULL );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnAxStockColorPropertyPage()
{
	ShowPropertyPages( 1, &CLSID( CLSID_StockColorPage ), NULL );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnAxFontPropertyPage()
{
	ShowPropertyPages( 1, &CLSID( CLSID_CFontPropPage ), NULL );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnAxStockFontPropertyPage()
{
	ShowPropertyPages( 1, &CLSID( CLSID_StockFontPage ), NULL );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnAxPicturePropertyPage()
{
	ShowPropertyPages( 1, &CLSID( CLSID_CPicturePropPage ), NULL );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnAxStockPicturePropertyPage()
{
	ShowPropertyPages( 1, &CLSID( CLSID_StockPicturePage ), NULL );
	OnEndEditCurCell();
}

void CPropertyGridCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if( pNMLV->uNewState & LVIS_SELECTED )
	{
		TPropertySet& PropSet = GetPropertySet( pNMLV->iItem );
		assert( !PropSet.empty() );
		if( !PropSet.empty() )
			mpParent->ActivateProperty( PropSet.front() );
	}
	*pResult = 0;
}
