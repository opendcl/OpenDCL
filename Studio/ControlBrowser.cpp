// ControlBrowser.cpp : implementation file
//

#include "stdafx.h"
#include "ControlBrowser.h"
#include "Workspace.h"
#include "DclControlObject.h"
#include "DclFormObject.h"
#include "UndoManager.h"
#include "ControlName.h"
#include "StdioUnicodeFile.h"
#include "LoadArgs.h"
#include "VARUtils.h"
#include "AxMethodDescriptor.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "AxContainerCtrl.h"


class CTreeNode
{
	CString msName;
public:
	CTreeNode( LPCTSTR pszName ) : msName( pszName ) {}
	virtual ~CTreeNode() {}

	virtual void onSelected( CControlBrowser& Browser )
		{
			CString sRtf =
				_T("{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang4105{\\fonttbl{\\f0\\fnil\\fcharset0 Times New Roman;}}\r\n")
				_T("{\\colortbl ;\\red255\\green0\\blue0;\\red0\\green0\\blue255;\\red0\\green128\\blue0;}\r\n")
				_T("\\viewkind4\\uc1\\pard\\f0\\fs20 ");
			sRtf += description();
			sRtf += _T(" \\par \r\n}");
			Browser.SetDescription( sRtf );
			CString sCopy1Caption = copy1Caption();
			CWnd* pCopy1Wnd = Browser.GetDlgItem( IDC_COPY1 );
			pCopy1Wnd->ShowWindow( sCopy1Caption.IsEmpty()? SW_HIDE : SW_SHOW );
			Browser.SetCopy1Lisp( copy1Lisp() );
			CString sCopy2Caption = copy2Caption();
			CWnd* pCopy2Wnd = Browser.GetDlgItem( IDC_COPY2 );
			pCopy2Wnd->ShowWindow( sCopy2Caption.IsEmpty()? SW_HIDE : SW_SHOW );
			pCopy2Wnd->SetWindowText( sCopy2Caption );
			Browser.SetCopy2Lisp( copy2Lisp() );
		}

	virtual LPCTSTR name() const { return msName; }
	virtual int image() const { return -1; }
	virtual CString description() const { return NULL; }
	virtual CString copy1Caption() const { return NULL; }
	virtual CString copy1Lisp() const { return NULL; }
	virtual CString copy2Caption() const { return NULL; }
	virtual CString copy2Lisp() const { return NULL; }
	virtual bool addChildItems( CControlBrowser& Browser, HTREEITEM hParent ) { return true; }
	static bool addMethodsFileChildItems( CControlBrowser& Browser, HTREEITEM hParent, LPCTSTR pszFilename );
};

class CDclControlNode : public CTreeNode
{
	TDclControlPtr mpDclControl;
public:
	CDclControlNode( TDclControlPtr pDclControl )
		: CTreeNode( GetControlName( pDclControl ) )
		, mpDclControl( pDclControl )
		{}
	CDclControlNode( TDclControlPtr pDclControl, LPCTSTR pszName )
		: CTreeNode( pszName )
		, mpDclControl( pDclControl )
		{}
	virtual ~CDclControlNode() {}

	virtual int image() const { return 5; }
	virtual bool addChildItems( CControlBrowser& Browser, HTREEITEM hParent );

	virtual bool addPropertyChildItems( CControlBrowser& Browser, HTREEITEM hParent, CAxContainerCtrl* pAxCont = NULL );
};

class CDclFormNode : public CDclControlNode
{
	TDclFormPtr mpDclForm;

	static CString GetName( TDclFormPtr pDclForm )
		{
			switch( pDclForm->GetType() )
			{
			case FrmModalDlg: return theWorkspace.LoadResourceString( IDS_MODALFORM );
			case FrmModelessDlg: return theWorkspace.LoadResourceString( IDS_MODELESSFORM );
			case FrmDockableDlg: return theWorkspace.LoadResourceString( IDS_DOCKABLEFORM );
			case FrmConfigTab: return theWorkspace.LoadResourceString( IDS_CONFIGTAB );
			case FrmFileDlg: return theWorkspace.LoadResourceString( IDS_FILEDLG );
			case FrmPaletteDlg: return theWorkspace.LoadResourceString( IDS_PALETTEFORM );
			}
			return NULL;
		}

public:
	CDclFormNode( TDclFormPtr pDclForm )
		: CDclControlNode( pDclForm->GetControlProperties(), GetName( pDclForm ) )
		, mpDclForm( pDclForm )
		{}
	virtual ~CDclFormNode() {}
	virtual bool addChildItems( CControlBrowser& Browser, HTREEITEM hParent );
};

class CPropertyNode : public CTreeNode
{
	TPropertyPtr mpProp;
	CString msDescription;
	CString msCopy1Lisp;
	CString msCopy1Caption;
	CString msCopy2Lisp;
	CString msCopy2Caption;

public:
	CPropertyNode( TPropertyPtr pProp, bool bInitialize = true )
		: CTreeNode( pProp->GetName() )
		, mpProp( pProp )
		{
			if( bInitialize )
				initialize();
		}
	virtual ~CPropertyNode() {}
	virtual TPropertyPtr prop() const { return mpProp; }

	virtual int image() const { return 4; }
	virtual CString description() const { return ReplaceParams( msDescription ); }
	virtual CString copy1Caption() const { return msCopy1Caption; }
	virtual CString copy1Lisp() const { return ReplaceParams( msCopy1Lisp ); }
	virtual CString copy2Caption() const { return msCopy2Caption; }
	virtual CString copy2Lisp() const { return ReplaceParams( msCopy2Lisp ); }

protected:
	void setDescription( const CString& sDesc ) { msDescription = sDesc; }
	void setCopy1Caption( const CString& sCaption ) { msCopy1Caption = sCaption; }
	void setCopy1Lisp( const CString& sLisp ) { msCopy1Lisp = sLisp; }
	void setCopy2Caption( const CString& sCaption ) { msCopy2Caption = sCaption; }
	void setCopy2Lisp( const CString& sLisp ) { msCopy2Lisp = sLisp; }

	virtual CString propType() const
		{
			switch( prop()->GetType() )
			{
			case PropLong: return theWorkspace.LoadResourceString( IDS_LONG );
			case PropEnum: return theWorkspace.LoadResourceString( IDS_INT );
			case PropPicture: return theWorkspace.LoadResourceString( IDS_INT );
			case PropString: return theWorkspace.LoadResourceString( IDS_STRING );
			case PropDouble: return theWorkspace.LoadResourceString( IDS_REAL );
			case PropBool: return theWorkspace.LoadResourceString( IDS_BOOLEAN );
			};
			return NULL;
		}
	virtual CString propDesc() const
		{
			CString sDesc = prop()->GetDocumentationDesc();
			sDesc.Replace( _T("\\"), _T("\\\\") );
			return sDesc;
		}
	virtual CString propProcessGetDesc()
		{
			setCopy2Caption( theWorkspace.LoadResourceString( IDS_COPYGETTOCLIP ) );
			CString sApiName = GetPropertyApiName( prop()->GetID() );
			CString sDefun;
			sDefun.Format( _T("(\\cf2 Setq \\cf1 Value \\cf0 (\\cf2 dcl_Control_Get%s \\cf0 \\cf3<CONTROL>\\cf0))"), sApiName );
			CString sLisp;
			sLisp.Format( _T("(Setq Value (dcl_Control_Get%s <CONTROL>))"), sApiName );
			setCopy2Lisp( sLisp );
			return sDefun;
		}
	virtual CString propProcessPutDesc()
		{
			setCopy1Caption( theWorkspace.LoadResourceString( IDS_COPYTOCLIP ) );
			CString sApiName = GetPropertyApiName( prop()->GetID() );
			CString sDefun;
			sDefun.Format( _T("(\\cf2 dcl_Control_Set%s \\cf0 \\cf3<CONTROL>\\cf0\\par\\tab\\cf1 NewValue [%s %s]\\cf0)"), sApiName, theWorkspace.LoadResourceString( IDS_AS ), propType() );
			CString sLisp;
			sLisp.Format( _T("(dcl_Control_Get%s <CONTROL> \r\n\tNewValue [%s %s])"), sApiName, theWorkspace.LoadResourceString( IDS_AS ), propType() );
			setCopy1Lisp( sLisp );
			return sDefun;
		}
	virtual CString propProcessExtraDesc() { return NULL; }
	virtual bool isDesignTimeOnly()
		{
			switch( prop()->GetOwnerControl()->GetType() )
			{
			case CtlFileDlgCtrl: return true;
			}
			return false;
		}
	virtual bool isHidden()
		{
			bool bHidden = mpProp->IsHidden();
			switch( prop()->GetID() )
			{
			case Prop::FontSize:
			case Prop::FontBold:
			case Prop::FontItalic:
			case Prop::FontStrikeout:
			case Prop::FontUnderline:
				bHidden = false; //these hidden properties should be treated like normal properties
				break;
			}
			return bHidden;
		}
	virtual bool isRuntimeNotAllowed()
		{
			bool bAllowed = true;
			switch( prop()->GetID() )
			{
			case Prop::ComboBoxStyle:
			case Prop::ButtonStyle:
			case Prop::FilterStyle:
			case Prop::MultiRow:
			case Prop::Orientation:
			case Prop::SelectionStyle:
			case Prop::BlockListStyle:
			case Prop::DockableSides:
			case Prop::DefaultDockedSide:
				bAllowed = false;
				break;
			}
			return !bAllowed;
		}

protected:
	virtual void initialize()
		{
			TDclControlPtr pDclControl = prop()->GetOwnerControl();
			CString sTitle;
			sTitle.Format( _T("\\b0 %s \\b %s \\b0 \\cf0"), theWorkspace.LoadResourceString( IDS_PROPERTY ), name() );
			CString sVarType = propType();
			if( !sVarType.IsEmpty() )
				sTitle += _T("\\cf1 ") + theWorkspace.LoadResourceString( IDS_AS ) + _T(" ") + sVarType + _T("\\cf0 ");
			CString sDesc = propDesc();
			if( sDesc.IsEmpty() )
				sDesc = theWorkspace.LoadResourceString( IDS_DESCNOTSET );
			if( isDesignTimeOnly() )
			{
				sDesc += _T("\\par\\par ");
				sDesc += theWorkspace.LoadResourceString( IDS_DESIGNTIMEONLY );
			}
			if( isHidden() )
			{
				sDesc += _T("\\par\\par ");
				sDesc += theWorkspace.LoadResourceString( IDS_HIDDENPROP );
			}
			else if( isRuntimeNotAllowed() )
			{
				sDesc += _T("\\par\\par ");
				sDesc += theWorkspace.LoadResourceString( IDS_RUNTIMENOTALLOWEDCTRL );
			}
			else
			{
				CString sPutDesc = propProcessPutDesc();
				if( !sPutDesc.IsEmpty() )
				{
					sDesc += _T("\\par\\par \\b ") + theWorkspace.LoadResourceString( IDS_ALSPS ) + _T("\\b0\\par");
					sDesc += sPutDesc;
				}
				CString sGetDesc = propProcessGetDesc();
				if( !sGetDesc.IsEmpty() )
				{
					sDesc += _T("\\par\\par \\b ") + theWorkspace.LoadResourceString( IDS_ALGPS ) + _T("\\b0\\par");
					sDesc += sGetDesc;
				}
				CString sExtraDesc = propProcessExtraDesc();
				if( !sExtraDesc.IsEmpty() )
				{
					sDesc += _T("\\par\\par ");
					sDesc += sExtraDesc;
				}
			}
			msDescription = sTitle + _T("\\par\\par ") + sDesc;
		}

protected:
	CString ReplaceParams( const CString& sText ) const
		{
			TDclControlPtr pDclControl = prop()->GetOwnerControl();
			CString sNew = sText;
			TProjectPtr pProject = pDclControl->GetOwnerProject();
			if( pProject )
				sNew.Replace( _T("<PROJECTNAME>"), pProject->GetKeyName() );
			sNew.Replace( _T("<CONTROL>"), pDclControl->GetVarName() );
			return sNew;
		}
};

class CMethodNode : public CTreeNode
{
	TDclControlPtr mpDclControl;
	CString msDescription;
	CString msLisp;
	CString msCopyCaption;
public:
	CMethodNode( TDclControlPtr pDclControl, LPCTSTR pszName )
		: CTreeNode( pszName )
		, mpDclControl( pDclControl )
		{}
	virtual ~CMethodNode() {}
	virtual TDclControlPtr control() const { return mpDclControl; }

	virtual int image() const { return 3; }
	virtual CString description() const { return ReplaceParams( msDescription ); }
	virtual CString copy2Caption() const { return msCopyCaption; }
	virtual CString copy2Lisp() const { return ReplaceParams( msLisp ); }

	void setDescription( const CString& sDesc ) { msDescription = sDesc; }
	void setCopyLisp( const CString& sLisp ) { msLisp = sLisp; }
	void setCopyCaption( const CString& sCopyCaption ) { msCopyCaption = sCopyCaption; }

protected:
	CString ReplaceParams( const CString& sText ) const
		{
			CString sNew = sText;
			TProjectPtr pProject = mpDclControl->GetOwnerProject();
			if( pProject )
				sNew.Replace( _T("<PROJECTNAME>"), pProject->GetKeyName() );
			sNew.Replace( _T("<CONTROL>"), mpDclControl->GetVarName() );
			return sNew;
		}
};

class CEventNode : public CTreeNode
{
	TPropertyPtr mpProp;
	CString msDescription;
	CString msCopyLisp;
	CString msCopyCaption;
public:
	CEventNode( TPropertyPtr pProp, bool bInitialize = true )
		: CTreeNode( pProp->GetType() == PropActiveXEvent? pProp->GetConstAxInterfaceDescriptorPtr()->GetName() : pProp->GetName() )
		, mpProp( pProp )
		{
			if( bInitialize )
				initialize();
		}
	virtual ~CEventNode() {}
	virtual TPropertyPtr prop() const { return mpProp; }

	virtual int image() const { return 2; }
	virtual CString description() const { return ReplaceParams( msDescription ); }
	virtual CString copy2Caption() const { return msCopyCaption; }
	virtual CString copy2Lisp() const { return ReplaceParams( msCopyLisp ); }

	void setDescription( const CString& sDesc ) { msDescription = sDesc; }
	void setCopyLisp( const CString& sLisp ) { msCopyLisp = sLisp; }
	void setCopyCaption( const CString& sCaption ) { msCopyCaption = sCaption; }

protected:
	void initialize()
		{
			CString sEventName = GetPropertyName( prop()->GetID() );
			CString sTitle = _T("\\b0 ") + theWorkspace.LoadResourceString( IDS_EVENTTITLE ) + _T(" \\b ") + (sEventName.IsEmpty()? mpProp->GetName() : sEventName) + _T(" \\b0 \\cf0");

			CString sDesc;
			CString sEventArgs;
			LoadArgsNDesc( prop(), sEventArgs, sDesc );
			
			CString sDefun = _T("\\par (\\cf3 defun\\cf0  ");
			CString sLisp = _T("(defun ");
			CString sFuncName = mpProp->GetStringValue();
			if( sFuncName.IsEmpty() )
				sFuncName = _T("c:<CONTROL>_On") + sEventName;
			if( !sEventArgs.IsEmpty() )
			{
				sLisp += sFuncName + _T(" (") + sEventArgs + _T(" /)\r\n)\r\n");
				sDefun += _T("\\cf2  ") + sFuncName + _T("\\cf0  (\\cf1 ") + sEventArgs + _T("\\cf0  /)\\par )\\par");
			}
			else
			{
				sLisp += sFuncName + _T(" ()\r\n)\r\n");
				sDefun += _T("\\cf2 ") + sFuncName + _T("\\cf0 ()\\par )\\par");
			}
			sDesc += _T("\\par ");
			sDesc += sDefun;
			setCopyCaption( theWorkspace.LoadResourceString( IDS_COPYTOCLIP ) );
			setDescription( sTitle + _T("\\par\\par ") + sDesc );
			setCopyLisp( sLisp );
		}

protected:
	CString ReplaceParams( const CString& sText ) const
		{
			TDclControlPtr pDclControl = prop()->GetOwnerControl();
			CString sNew = sText;
			TProjectPtr pProject = pDclControl->GetOwnerProject();
			if( pProject )
				sNew.Replace( _T("<PROJECTNAME>"), pProject->GetKeyName() );
			sNew.Replace( _T("<CONTROL>"), pDclControl->GetVarName() );
			return sNew;
		}
};

class CAxObjectNode : public CDclControlNode
{
	CAxContainerCtrl* mpAxCont;

public:
	CAxObjectNode( CAxContainerCtrl* pAxCont, TDclControlPtr pAxInfo )
		: CDclControlNode( pAxInfo, pAxInfo->GetAxTypeName() )
		, mpAxCont( pAxCont )
		{}
	virtual ~CAxObjectNode() {}

	virtual int image() const { return 5; }
	virtual bool addChildItems( CControlBrowser& Browser, HTREEITEM hParent )
		{
			addMethodsFileChildItems( Browser, hParent, _T("AxObjects.mth") );
			addPropertyChildItems( Browser, hParent, mpAxCont );
			return true;
		}
};

class CAxPropertyNode : public CPropertyNode
{
	const AxInterfaceDescriptor* mpIDesc;
	CAxContainerCtrl* mpAxCont;

public:
	CAxPropertyNode( TPropertyPtr pProp, const AxInterfaceDescriptor* pIDesc,
									 CAxContainerCtrl* pAxCont = NULL, bool bInitialize = true )
		: CPropertyNode( pProp, false )
		, mpIDesc( pIDesc )
		, mpAxCont( pAxCont )
		{
			if( bInitialize )
				initialize();
		}
	virtual ~CAxPropertyNode() {}
	CAxContainerCtrl* axCont() const { return mpAxCont; }

	virtual bool addChildItems( CControlBrowser& Browser, HTREEITEM hParent )
		{
			if( mpIDesc->GetGuid() == GUID_NULL )
				return true;
			if( mpIDesc->GetEnumDescriptor() )
				return true;
			CAxContainerCtrl* pAxCont = axCont();
			if( !pAxCont )
				return false;
			TDclControlPtr pNewItem = new CDclControlObject( _CtlInvalid, NULL );
			if( !pAxCont->ExtractComponentsFromTLB( pNewItem, mpIDesc->GetGuid() ) )
				return false;
			if( pNewItem->GetAxTypeName().IsEmpty() )
				return false; //don't display unnamed objects
			Browser.InsertItem( TVI_ROOT, new CAxObjectNode( pAxCont, pNewItem ) );
			return true;
		}

protected:
	virtual CString propType() const
		{
			AxPropertyDescriptor* pAxPropGet = mpIDesc->GetGetDescriptor();
			if( pAxPropGet )
			{
				if( !pAxPropGet->GetEnum().empty() )
					return VARTYPEtoString( VT_I4 ); //use Long type for enums
				return pAxPropGet->GetTypeDisplayName();
			}
			AxPropertyDescriptor* pAxPropPut = mpIDesc->GetPutDescriptor();
			if( pAxPropPut )
			{
				if( !pAxPropPut->GetEnum().empty() )
					return VARTYPEtoString( VT_I4 ); //use Long type for enums
				return pAxPropPut->GetTypeDisplayName();
			}
			return NULL;
		}
	virtual CString propGetArgList() const
		{
			CString sArgList;
			AxPropertyDescriptor* pPropGet = mpIDesc->GetProp();
			if( !pPropGet )
				pPropGet = mpIDesc->GetPropGet();
			if( pPropGet )
			{
				const std::vector< AxArg >& rArgs = pPropGet->GetArgs();
				for( std::vector< AxArg >::const_iterator iter = rArgs.begin();
						 iter != rArgs.end();
						 ++iter )
				{
					const AxArg& arg = *iter;
					CString sArg = arg.name;
					CString sType = VARTYPEtoString( arg.vt );
					sArg += _T(" ");
					if( sType.IsEmpty() )
						sArg += theWorkspace.LoadResourceString( IDS_OPTIONALNILASB );
					else
						sArg += _T("[") + theWorkspace.LoadResourceString( IDS_AS ) + _T(" ") + sType + _T("]");
					sArgList += _T(" ") + sArg;
				}
			}
			return sArgList;
		}
	virtual CString propPutArgList() const
		{
			CString sArgList;
			AxPropertyDescriptor* pPropPut = mpIDesc->GetPropPut();
			if( !pPropPut )
				pPropPut = mpIDesc->GetPropPutRef();
			if( pPropPut )
			{
				const std::vector< AxArg >& rArgs = pPropPut->GetArgs();
				for( std::vector< AxArg >::const_iterator iter = rArgs.begin();
						 iter != rArgs.end();
						 ++iter )
				{
					const AxArg& arg = *iter;
					CString sArg = arg.name;
					CString sType = VARTYPEtoString( arg.vt );
					sArg += _T(" ");
					if( sType.IsEmpty() )
						sArg += theWorkspace.LoadResourceString( IDS_OPTIONALNILASB );
					else
						sArg += _T("[") + theWorkspace.LoadResourceString( IDS_AS ) + _T(" ") + sType + _T("]");
					sArgList += _T(" ") + sArg;
				}
			}
			return sArgList;
		}
	virtual CString propProcessGetDesc()
		{
			CString sDefun;
			if( mpIDesc->GetProp() || mpIDesc->GetPropGet() )
			{
				setCopy2Caption( theWorkspace.LoadResourceString( IDS_COPYGETTOCLIP ) );
				CString sArgsDesc;
				CString sArgsLisp = propGetArgList();
				if( !sArgsLisp.IsEmpty() )
					sArgsDesc.Format( _T("\\cf1 %s\\cf0"), (LPCTSTR)sArgsLisp );
				CString sApiName = name();
				CString sLisp;
				bool bOleObject = !prop()->GetOwnerControl()->GetOwnerForm();
				if( bOleObject )
				{
					sDefun.Format( _T("(\\cf2 Setq \\cf1 Value \\cf0 (\\cf2 dcl_AxObject_GetProperty\\cf0\\cf1\\b\\i  OleObject \\cf0\\i0\\cf3 \"%s\"\\cf0\\b0 %s))"), (LPCTSTR)sApiName, (LPCTSTR)sArgsDesc );
					sLisp.Format( _T("(Setq Value (dcl_AxObject_GetProperty OleObject \"%s\"%s))"), (LPCTSTR)sApiName, (LPCTSTR)sArgsLisp );
				}
				else
				{
					sDefun.Format( _T("(\\cf2 Setq \\cf1 Value \\cf0 (\\cf2 dcl_AxControl_GetProperty \\cf0 \\cf3<CONTROL> \\b1 \"%s\"\\cf0\\b0 %s))"), (LPCTSTR)sApiName, (LPCTSTR)sArgsDesc );
					sLisp.Format( _T("(Setq Value (dcl_AxControl_GetProperty <CONTROL> \"%s\"%s))"), (LPCTSTR)sApiName, (LPCTSTR)sArgsLisp );
				}
				setCopy2Lisp( sLisp );
			}
			return sDefun;
		}
	virtual CString propProcessPutDesc()
		{
			CString sDefun;
			if( mpIDesc->GetPropPut() || mpIDesc->GetPropPutRef() )
			{
				setCopy1Caption( theWorkspace.LoadResourceString( IDS_COPYTOCLIP ) );
				CString sArgsDesc;
				CString sArgsLisp = propGetArgList();
				if( !sArgsLisp.IsEmpty() )
					sArgsDesc.Format( _T("\\cf1 %s\\cf0"), (LPCTSTR)sArgsLisp );
				CString sApiName = name();
				CString sNewValArgDesc;
				CString sNewValArgLisp;
				if( mpIDesc->GetGuid() == GUID_COLOR )
				{
					sNewValArgDesc += _T(" ");
					sNewValArgDesc += theWorkspace.LoadResourceString( IDS_REDCOLORDESC2 );
					sNewValArgLisp += _T(" ");
					sNewValArgLisp += theWorkspace.LoadResourceString( IDS_REDCOLORDESC );
				}
				else
				{
					sNewValArgDesc.Format( _T(" \\cf1 NewValue [%s %s]\\cf0"), (LPCTSTR)theWorkspace.LoadResourceString( IDS_AS ), (LPCTSTR)propType() );
					sNewValArgLisp.Format( _T(" NewValue [%s %s]"), (LPCTSTR)theWorkspace.LoadResourceString( IDS_AS ), (LPCTSTR)propType() );
				}
				CString sLisp;
				bool bOleObject = !prop()->GetOwnerControl()->GetOwnerForm();
				if( bOleObject )
				{
					sDefun.Format( _T("(\\cf2 dcl_AxObject_SetProperty\\cf0\\cf1\\b\\i  OleObject \\cf0\\i0\\cf3 \"%s\"\\cf0\\b0 %s %s)"), (LPCTSTR)sApiName, (LPCTSTR)sArgsDesc, (LPCTSTR)sNewValArgDesc );
					sLisp.Format( _T("(dcl_AxObject_SetProperty OleObject \"%s\"%s %s)"), (LPCTSTR)sApiName, (LPCTSTR)sNewValArgLisp, (LPCTSTR)sArgsLisp );
				}
				else
				{
					sDefun.Format( _T("(\\cf2 dcl_AxControl_SetProperty \\cf0 \\cf3<CONTROL> \\b1 \"%s\"\\cf0\\b0 %s %s)"), (LPCTSTR)sApiName, (LPCTSTR)sArgsDesc, (LPCTSTR)sNewValArgDesc );
					sLisp.Format( _T("(dcl_AxControl_SetProperty <CONTROL> \"%s\"%s %s)"), (LPCTSTR)sApiName, (LPCTSTR)sNewValArgLisp, (LPCTSTR)sArgsLisp );
				}
				setCopy1Lisp( sLisp );
				if( mpIDesc->GetGuid() == GUID_COLOR )
					sDefun += theWorkspace.LoadResourceString( IDS_SETCOLORDESC );
			}
			return sDefun;
		}
	virtual CString propProcessExtraDesc()
		{
			CString sExtraInfo;
			switch( mpIDesc->GetType() )
			{
			case VT_DISPATCH:
			case VT_UNKNOWN:
				if( mpIDesc->GetGuid() != GUID_NULL )
				{
					CString sFmt;
					sFmt.Format( theWorkspace.LoadResourceString( IDS_WHENFIN2 ), (LPCTSTR)propType() );
					sExtraInfo += sFmt;
				}
				else
					sExtraInfo += theWorkspace.LoadResourceString( IDS_WHENFIN );
				break;
			}
			return sExtraInfo;
		}
	virtual bool isDesignTimeOnly() { return false; }
	virtual bool isHidden() { return prop()->IsHidden(); }
	virtual bool isRuntimeNotAllowed() { return false; }
};

class CAxMethodNode : public CMethodNode
{
	const AxMethodDescriptor* mpMethodDesc;
	CAxContainerCtrl* mpAxCont;

public:
	CAxMethodNode( TPropertyPtr pProp, const AxMethodDescriptor* pMethodDesc,
								 CAxContainerCtrl* pAxCont = NULL, bool bInitialize = true )
		: CMethodNode( pProp->GetOwnerControl(), pMethodDesc->GetName() )
		, mpMethodDesc( pMethodDesc )
		, mpAxCont( pAxCont )
		{
			if( bInitialize )
				initialize();
		}
	virtual ~CAxMethodNode() {}
	CAxContainerCtrl* axCont() const { return mpAxCont; }

	virtual bool addChildItems( CControlBrowser& Browser, HTREEITEM hParent )
		{
			if( mpMethodDesc->GetReturnGuid() == GUID_NULL )
				return true;
			CAxContainerCtrl* pAxCont = axCont();
			if( !pAxCont )
				return false;
			TDclControlPtr pNewItem = new CDclControlObject( _CtlInvalid, NULL );
			if( !pAxCont->ExtractComponentsFromTLB( pNewItem, mpMethodDesc->GetReturnGuid() ) )
				return false;
			if( pNewItem->GetAxTypeName().IsEmpty() )
				return false; //don't display unnamed objects
			Browser.InsertItem( TVI_ROOT, new CAxObjectNode( pAxCont, pNewItem ) );
			return true;
		}

protected:
	virtual CString methodType() const
		{
			return mpMethodDesc->GetReturnTypeDisplayName();
		}
	virtual CString methodDesc() const
		{
			CString sDesc = mpMethodDesc->GetDesc();
			sDesc.Replace( _T("\\"), _T("\\\\") );
			return sDesc;
		}
	virtual CString methodArgList() const
		{
			CString sArgList;
			const std::vector< AxArg >& rArgs = mpMethodDesc->GetArgs();
			for( std::vector< AxArg >::const_iterator iter = rArgs.begin();
					 iter != rArgs.end();
					 ++iter )
			{
				const AxArg& arg = *iter;
				CString sArg = arg.name;
				CString sType = VARTYPEtoString( arg.vt );
				sArg += _T(" ");
				if( sType.IsEmpty() )
					sArg += theWorkspace.LoadResourceString( IDS_OPTIONALNIL );
				else
					sArg += _T("[") + theWorkspace.LoadResourceString( IDS_AS ) + _T(" ") + sType + _T("]");
				sArgList += _T("\\par\\tab ") + sArg;
			}
			return sArgList;
		}
	virtual CString methodProcessResultDesc( const CString& sReturnType )
		{
			setCopyCaption( theWorkspace.LoadResourceString( IDS_COPYTOCLIP ) );
			CString sArgsDesc;
			CString sArgsLisp;
			CString sArgList = methodArgList();
			if( !sArgList.IsEmpty() )
			{
				sArgsDesc.Format( _T("\\cf1 %s\\cf0"), (LPCTSTR)sArgList );
				sArgsLisp = sArgList;
				sArgsLisp.Replace( _T("\\par\\tab "), _T(" ") );
			}
			CString sApiName = name();
			CString sDefun;
			CString sLisp;
			bool bOleObject = !control()->GetOwnerForm();
			if( bOleObject )
			{
				sDefun.Format( _T("(\\cf2 Setq \\cf1 Value \\cf0 (\\cf2 dcl_AxObject_DoMethod\\cf1\\b\\i  OleObject \\cf0\\i0\\cf3 \"%s\"\\cf0\\b0 %s))"), (LPCTSTR)sApiName, (LPCTSTR)sArgsDesc );
				sLisp.Format( _T("(Setq Value (dcl_AxObject_DoMethod OleObject \"%s\"%s))"), (LPCTSTR)sApiName, (LPCTSTR)sArgsLisp );
			}
			else
			{
				sDefun.Format( _T("(\\cf2 Setq \\cf1 Value \\cf0 (\\cf2 dcl_AxControl_DoMethod\\cf0\\cf3  <CONTROL> \\b\"%s\"\\b0 %s\\cf0))"), (LPCTSTR)sApiName, (LPCTSTR)sArgsDesc );
				sLisp.Format( _T("(Setq Value (dcl_AxControl_DoMethod <CONTROL> \"%s\"%s))"), (LPCTSTR)sApiName, (LPCTSTR)sArgsLisp );
			}
			setCopyLisp( sLisp );
			return sDefun;
		}
	virtual CString methodProcessDesc()
		{
			setCopyCaption( theWorkspace.LoadResourceString( IDS_COPYTOCLIP ) );
			CString sArgsDesc;
			CString sArgsLisp;
			CString sArgList = methodArgList();
			if( !sArgList.IsEmpty() )
			{
				sArgsDesc.Format( _T("\\cf1 %s\\cf0"), (LPCTSTR)sArgList );
				sArgsLisp = sArgList;
				sArgsLisp.Replace( _T("\\par\\tab "), _T(" ") );
			}
			CString sApiName = name();
			CString sDefun;
			CString sLisp;
			bool bOleObject = !control()->GetOwnerForm();
			if( bOleObject )
			{
				sDefun.Format( _T("(\\cf2 dcl_AxObject_DoMethod\\cf1\\b\\i  OleObject \\cf0\\i0\\cf3 \"%s\"\\cf0\\b0 %s)"), (LPCTSTR)sApiName, (LPCTSTR)sArgsDesc );
				sLisp.Format( _T("(dcl_AxObject_DoMethod OleObject \"%s\"%s)"), (LPCTSTR)sApiName, (LPCTSTR)sArgsLisp );
			}
			else
			{
				sDefun.Format( _T("(\\cf2 dcl_AxControl_DoMethod\\cf0\\cf3  <CONTROL> \\b\"%s\"\\b0 %s\\cf0)"), (LPCTSTR)sApiName, (LPCTSTR)sArgsDesc );
				sLisp.Format( _T("(dcl_AxControl_DoMethod <CONTROL> \"%s\"%s)"), (LPCTSTR)sApiName, (LPCTSTR)sArgsLisp );
			}
			setCopyLisp( sLisp );
			return sDefun;
		}

protected:
	virtual void initialize()
		{
			TDclControlPtr pDclControl = control();
			CString sTitle;
			sTitle.Format( _T("\\b0 %s \\b %s \\b0 \\cf0"), theWorkspace.LoadResourceString( IDS_METHOD ), name() );
			CString sVarType = methodType();
			if( !sVarType.IsEmpty() )
				sTitle += _T("\\cf1 ") + theWorkspace.LoadResourceString( IDS_AS ) + _T(" ") + sVarType + _T("\\cf0 ");
			CString sDesc = methodDesc();
			CString sDefun;
			bool bReturningOLEObject = (mpMethodDesc->GetReturnGuid() == GUID_NULL? false : true);
			if( sVarType.IsEmpty() )
			{
				if( bReturningOLEObject )
				{
					sDesc += _T("\\par\\par ");
					sDesc += theWorkspace.LoadResourceString( IDS_OLENOTE1 );
				}
				sDefun = methodProcessDesc();
			}
			else
			{
				if( bReturningOLEObject )
				{
					sDesc += _T("\\par\\par ");
					sDesc += theWorkspace.LoadResourceString( IDS_OLENOTE2 );
					if( mpMethodDesc->GetReturnType() != VT_DISPATCH )
						sDesc += sVarType;
					sDesc += theWorkspace.LoadResourceString( IDS_OLENOTE3 );
				}
				sDefun = methodProcessResultDesc( sVarType );
			}
			if( !sDefun.IsEmpty() )
			{
				sDesc += _T("\\par\\par \\b ") + theWorkspace.LoadResourceString( IDS_ALISPSYN ) + _T("\\b0\\par");
				sDesc += sDefun;
			}
			sDesc += _T("\\par\\par ") + theWorkspace.LoadResourceString( IDS_FORMOREINFO );
			if( pDclControl->IsMicrosoftActiveXCtrl() )
				sDesc += theWorkspace.LoadResourceString( IDS_TOFINDIT );
			setDescription( sTitle + _T("\\par\\par ") + sDesc );
		}
};

class CAxEventNode : public CEventNode
{
	const AxEventDescriptor* mpEventDesc;
	CAxContainerCtrl* mpAxCont;

public:
	CAxEventNode( TPropertyPtr pProp, const AxEventDescriptor* pEventDesc,
								CAxContainerCtrl* pAxCont = NULL, bool bInitialize = true )
		: CEventNode( pProp, false )
		, mpEventDesc( pEventDesc )
		, mpAxCont( pAxCont )
		{
			if( bInitialize )
				initialize();
		}
	virtual ~CAxEventNode() {}
	CAxContainerCtrl* axCont() const { return mpAxCont; }

protected:
	virtual CString eventArgList() const
		{
			CString sArgList;
			const std::vector< AxArg >& rArgs = mpEventDesc->GetArgs();
			for( std::vector< AxArg >::const_iterator iter = rArgs.begin();
					 iter != rArgs.end();
					 ++iter )
			{
				const AxArg& arg = *iter;
				CString sArg = arg.name;
				CString sType = VARTYPEtoString( arg.vt );
				if( !sType.IsEmpty() )
					sArg += _T(" [") + theWorkspace.LoadResourceString( IDS_AS ) + _T(" ") + sType + _T("]");
				sArgList += _T(" ") + sArg;
			}
			return sArgList;
		}
	virtual void initialize()
		{
			const AxEventDescriptor* pAxEvent = prop()->GetConstAxInterfaceDescriptorPtr()->GetEvent();
			CString sEventName = pAxEvent->GetName();
			CString sArgsDesc;
			CString sArgsLisp;
			CString sArgList = eventArgList();
			if( !sArgList.IsEmpty() )
			{
				sArgsDesc.Format( _T("\\cf1%s\\cf0  /"), (LPCTSTR)sArgList );
				sArgsLisp = sArgList;
				sArgsLisp.TrimLeft();
				sArgsLisp += _T(" /");
			}
			else
			{
				sArgsDesc = _T("/");
				sArgsLisp = _T("/");
			}
			CString sDefun;
			sDefun.Format( _T("\\par (\\cf3 defun\\cf0  \\cf2 c:<CONTROL>_On%s\\cf0  (%s)\\par )\\par"), (LPCTSTR)sEventName, (LPCTSTR)sArgsDesc );
			CString sLisp;
			sLisp.Format( _T("(defun c:<CONTROL>_On%s (%s))"), (LPCTSTR)sEventName, (LPCTSTR)sArgsLisp );
			CString sTitle;
			sTitle.Format( _T("\\b0 %s \\b %s\\b0\\cf0"), theWorkspace.LoadResourceString( IDS_EVENTTITLE ), (LPCTSTR)sEventName );
			CString sDesc = pAxEvent->GetDesc();
			sDesc += _T("\\par ");
			sDesc += sDefun;
			setDescription( sTitle + _T("\\par\\par ") + sDesc );
			setCopyCaption( theWorkspace.LoadResourceString( IDS_COPYTOCLIP ) );
			setCopyLisp( sLisp );
		}
};


bool CDclControlNode::addChildItems( CControlBrowser& Browser, HTREEITEM hParent )
{
	CAxContainerCtrl* pAxCont = NULL;
	CDialogControl* pDlgControl = mpDclControl->GetControlInstance();
	if( pDlgControl )
		pAxCont = pDlgControl->GetActiveXCtrl();
	addPropertyChildItems( Browser, hParent, pAxCont );
	ControlType type = mpDclControl->GetType();
	if( type != CtlFileDlgCtrl )
		addMethodsFileChildItems( Browser, hParent, _T("AllCtrls.mth") );
	switch( type )
	{
		case CtlFileDlgCtrl:
			addMethodsFileChildItems( Browser, hParent, _T("FileDlgCtrl.mth") );
			break;
		case CtlTabStrip:
			addMethodsFileChildItems( Browser, hParent, _T("Tab.mth") );
			break;
		case CtlListBox:
			addMethodsFileChildItems( Browser, hParent, _T("ListBox.mth") );
			break;
		case CtlListView:
			addMethodsFileChildItems( Browser, hParent, _T("ListView.mth") );
			break;
		case CtlGrid:
			addMethodsFileChildItems( Browser, hParent, _T("Grid.mth") );
			break;
		case CtlImageComboBox:
			addMethodsFileChildItems( Browser, hParent, _T("ImageComboBox.mth") );
			break;
		case CtlAnimate:
			addMethodsFileChildItems( Browser, hParent, _T("AnimationCtrl.mth") );
			break;
		case CtlDwgList:
			addMethodsFileChildItems( Browser, hParent, _T("DwgList.mth") );
			break;
		case CtlBlockList:
			addMethodsFileChildItems( Browser, hParent, _T("BlockList.mth") );
			break;
		case CtlPictureBox:
			addMethodsFileChildItems( Browser, hParent, _T("PictureBox.mth") );
			break;
		case CtlOptionList:
			addMethodsFileChildItems( Browser, hParent, _T("OptionList.mth") );
			break;
		case CtlBlockView:
			addMethodsFileChildItems( Browser, hParent, _T("BlockView.mth") );
			break;
		case CtlHatch:
			addMethodsFileChildItems( Browser, hParent, _T("Hatch.mth") );
			break;
		case CtlSlideView:
			addMethodsFileChildItems( Browser, hParent, _T("SlideView.mth") );
			break;
		case CtlTree:
			addMethodsFileChildItems( Browser, hParent, _T("TreeCtrl.mth") );
			break;
		case CtlComboBox:
			addMethodsFileChildItems( Browser, hParent, _T("ComboBox.mth") );
			break;
		case CtlTextBox:
			addMethodsFileChildItems( Browser, hParent, _T("TextBox.mth") );
			break;
		case CtlDwgPreview:
			addMethodsFileChildItems( Browser, hParent, _T("DwgPreview.mth") );
			break;
		case CtlActiveX:
			addMethodsFileChildItems( Browser, hParent, _T("AxCtrls.mth") );
			break;
		case CtlMonth:
			addMethodsFileChildItems( Browser, hParent, _T("Month.mth") );
			break;
		case CtlGraphicButton:
			addMethodsFileChildItems( Browser, hParent, _T("GrphicBtn.mth") );
			break;
		case CtlHtmlCtrl:
			addMethodsFileChildItems( Browser, hParent, _T("Html.mth") );
			break;
	};
	return true;
}

bool CDclControlNode::addPropertyChildItems( CControlBrowser& Browser, HTREEITEM hParent, CAxContainerCtrl* pAxCont /*= NULL*/ )
{
	const TPropertyList& Props = mpDclControl->GetPropertyList();
	for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
	{
		TPropertyPtr pProp = (*iter);
		Prop::Id id = pProp->GetID();
		switch( id )
		{
		case Prop::Name:
		case Prop::GlobalVarName:
			continue;
		}
		switch( pProp->GetType() )
		{
		case PropEvent:
			Browser.InsertItem( hParent, new CEventNode( pProp ) );
			break;
		case PropActiveXEvent:
			Browser.InsertItem( hParent, new CAxEventNode( pProp, pProp->GetConstAxInterfaceDescriptorPtr()->GetEvent(), pAxCont ) );
			break;
		case PropActiveXMethods:
			{
				for( int idx = pProp->size() - 1; idx >= 0; --idx )
				{
					RefCountedPtr< AxMethodDescriptor > pDesc =
						pProp->GetConstAxInterfaceDescriptorPtr()->GetMethods()->at( idx );
					Browser.InsertItem( hParent, new CAxMethodNode( pProp, pDesc, pAxCont ) );
				}
			}
			break;
		case PropActiveXPropPages:
		case PropCustom:
		case PropImageList:
		case PropStringArray:
		case PropIntArray:
			break;
		case PropActiveXProp:
		case PropActiveXRunTime:
		case PropActiveXEnum:
			{
				if( pProp->IsHidden() )
					continue;
				const AxInterfaceDescriptor* pPropDesc = pProp->GetConstAxInterfaceDescriptorPtr();
				if( !pPropDesc )
					continue;
				Browser.InsertItem( hParent, new CAxPropertyNode( pProp, pPropDesc, pAxCont ) );
			}
			break;
		default:
			if( pProp->IsHidden() )
				continue;
			Browser.InsertItem( hParent, new CPropertyNode( pProp ) );
			break;
		};
	}
	return true;
}

bool CDclFormNode::addChildItems( CControlBrowser& Browser, HTREEITEM hParent )
{
	addPropertyChildItems( Browser, hParent );
	addMethodsFileChildItems( Browser, hParent, _T("Forms.mth") );
	return true;
}

bool CTreeNode::addMethodsFileChildItems( CControlBrowser& Browser,
																					HTREEITEM hParent,
																					LPCTSTR pszFilename )
{
	CString sMethodFile = theWorkspace.FindFile( pszFilename );
	if( sMethodFile.IsEmpty() )
		//try it without path (probably won't help, but at least there will be a filename in the error message)
		sMethodFile = pszFilename;

	try
	{
		CStdioUnicodeFile fMthFile( sMethodFile, CFile::shareDenyWrite | CFile::modeRead );
		CString sLine;
		fMthFile.ReadString( sLine );
		while( sLine != _T("[End of File]") && fMthFile.GetPosition() < fMthFile.GetLength() )
		{
			if( sLine == _T("[Method]") )
			{
				CString sMethodName;
				fMthFile.ReadString( sMethodName );
				CMethodNode* pItem = new CMethodNode( Browser.GetMainControl(), sMethodName );
				HTREEITEM hItem = Browser.InsertItem( hParent, pItem );
				assert( hItem != NULL );
				if( !hItem )
					return false;

				CString sNameForm;
				fMthFile.ReadString( sNameForm );	// should be [Prefix] or [Function]
				fMthFile.ReadString( sLine );	
				CString sFuncName = sLine;
				if( sNameForm == _T("[Prefix]") )
					sFuncName += sMethodName;
				fMthFile.ReadString( sLine );	// get past the [Desc]
				CString sDesc;
				while( sLine.Left( 10 ) != _T("[Arguments") &&
							 sLine != _T("[Argument]") &&
							 fMthFile.GetPosition() < fMthFile.GetLength() )
				{
					fMthFile.ReadString( sLine );	
					if( sLine.Left( 10 ) != _T("[Arguments") && sLine != _T("[Argument]") )
						sDesc += sLine;			
				}
				CString sTitle;
				CString sLisp;
				CString sDefun = _T("\\par ");
				bool bHasReturn = false;
				// setup the return type
				if( sLine.Left( 18 ) == _T("[Arguments Return]") )
				{
					// here we must setup the sTitle (first line) so it will show the Method + function short name + as something for return.
					sTitle = _T("\\b0 ") + theWorkspace.LoadResourceString( IDS_METHOD ) + _T(" \\b ") + sMethodName + _T(" \\b0 \\cf1 ") + sLine.Mid(19) + _T("\\cf0");
					sDefun += _T("(\\cf2 Setq \\cf1 Value \\cf0 (\\cf2") + sFuncName + _T("\\cf0 ");
					sLisp = _T("(Setq Value (") + sFuncName;
					bHasReturn = true;
				}
				else
				{
					sTitle = _T("\\b0 ") + theWorkspace.LoadResourceString( IDS_METHOD ) + _T(" \\b ") + sMethodName + _T(" \\b0 \\cf0");
					sDefun += _T("(\\cf2") + sFuncName + _T("\\cf0 ");
					sLisp = _T("(") + sFuncName;
				}
				fMthFile.ReadString( sLine );	// now we can get past the [Arguments] 
				// here we loop through all the remaining arguments and add to them to the tail of the autolisp syntax structure
				while( sLine != _T("[Method]") && !sLine.IsEmpty() && sLine != _T("[End of File]") )
				{
					static const CString sAs = theWorkspace.LoadResourceString( IDS_AS );
					static const CString sControlName = _T("<CONTROL>");
					if( sLine == sControlName )
					{
						sDefun += _T(" \\cf3") + sControlName + _T("\\cf0");
						sLisp += _T(" ") + sControlName;
					}						
					else if( sLine == _T("ProjectName") )
					{
						static const CString sProjectName = _T("<PROJECTNAME>");
						sDefun += _T(" \\cf3\"") + sProjectName + _T("\"\\cf0");
						sLisp += _T(" \"") + sProjectName + _T("\"");
					}						
					else if( sLine.Left( 1 ) == _T("[") && sLine.Mid( 1, sAs.GetLength() ) == sAs )
					{
						sDefun += _T(" \\cf5\\i  ") + sLine + _T("\\i0\\cf0 ");
						sLisp += _T(" ") + sLine;
					}
					else if( sLine == _T("[Begin List]") )
					{
						sDefun += _T(" \\cf0 (list");
						sLisp += _T(" (list");
					}
					else if( sLine == _T("[End List]") )
					{
						sDefun += _T("\\cf0)");
						sLisp += _T(")");
					}
					else if( !sLine.Trim().IsEmpty() ) 
					{
						sDefun += _T(" \\cf1  ") + sLine ;
						// if the last char was not a ( add a space before we add the argument
						if( sLisp.Right( 1 ) != _T("(") )
							sLisp += _T(' ');
						sLisp += sLine;
					}
					if( fMthFile.GetPosition() >= fMthFile.GetLength() )
						break;
					fMthFile.ReadString( sLine ); // now we can get past the [Arguments] 				
				}
				sDefun += _T("\\cf0)");
				sLisp += _T(")");
				if( bHasReturn )
				{
					sDefun += _T(')');
					sLisp += _T(')');
				}
				sDefun += _T("\\par \\par ");
				sLisp.Replace( _T("\\i0 "), _T("") );
				sLisp.Replace( _T("\\i0"), _T("") );
				sLisp.Replace( _T("\\i "), _T("") );
				sLisp.Replace( _T("\\i"), _T("") );
				sLisp.Replace( _T("\\par "), _T("\r\n") ); //change RTF paragraph breaks to ASCII linefeeds
				sLisp.Replace( _T("\\par"), _T("\r\n") ); //change RTF paragraph breaks to ASCII linefeeds
				sLisp.Replace( _T("\\tab "), _T("\t") ); //change RTF tabs to ASCII tabs
				sLisp.Replace( _T("\\tab"), _T("\t") ); //change RTF tabs to ASCII tabs

				sDesc = sTitle + _T(" \\par \\par ") + sDesc + _T(" \\par ");
				sDesc += _T(" \\par \\b ") + theWorkspace.LoadResourceString( IDS_ALISPSYN ) + _T("\\b0 ");
				sDesc += sDefun;
				pItem->setDescription( sDesc );
				pItem->setCopyLisp( sLisp );
				pItem->setCopyCaption( theWorkspace.LoadResourceString( IDS_COPYTOCLIP ) );
			}
			if( fMthFile.GetPosition() >= fMthFile.GetLength() )
				break;
			fMthFile.ReadString(sLine);
		}
	}
	catch( CFileException* e )
	{
		e->ReportError();
		e->Delete();
	}
	return true;
}


// CControlBrowser dialog

CControlBrowser::CControlBrowser(TDclControlPtr pDclControl, CWnd* pParent /*=NULL*/)
	: CResizableDialog(CControlBrowser::IDD, pParent)
	, mpDclControl( pDclControl )
	, mszPrevious( -1, -1 )
{

}

CControlBrowser::~CControlBrowser()
{
}

HTREEITEM CControlBrowser::InsertItem( HTREEITEM hParent, CTreeNode* pItem )
{
	assert( hParent != NULL );
	assert( pItem != NULL );
	CString sName = pItem->name();
	assert( !sName.IsEmpty() );
	if( sName.IsEmpty() )
	{
		delete pItem; //caller relinquished ownership
		return NULL;
	}
	if( hParent == TVI_ROOT )
	{
		for( HTREEITEM hCursor = mObjectTree.GetChildItem( hParent );
				 hCursor;
				 hCursor = mObjectTree.GetNextSiblingItem( hCursor ) )
		{
			if( sName == mObjectTree.GetItemText( hCursor ) )
			{
				delete pItem; //caller relinquished ownership
				return NULL; //already exists
			}
		}
	}
	int nImage = pItem->image();
	HTREEITEM hItem = mObjectTree.InsertItem( sName, nImage, nImage, hParent, TVI_SORT );
	assert( hItem != NULL );
	if( !hItem )
	{
		delete pItem; //caller relinquished ownership
		return NULL;
	}
	mObjectTree.SetItemData( hItem, (DWORD_PTR)pItem );
	pItem->addChildItems( *this, hItem );
	return hItem;
}

void CControlBrowser::RemoveItem( HTREEITEM hTarget )
{
	assert( hTarget != NULL );
	CTreeNode* pItem = (CTreeNode*)mObjectTree.GetItemData( hTarget );
	delete pItem;
	mObjectTree.DeleteItem( hTarget );
}

bool CControlBrowser::OnBeginClipboardCopy() 
{
	if( !theWorkspace.GetActiveDocument()->GetPathName().IsEmpty() )
		return true;
	int nWhatNext = MessageBox( theWorkspace.LoadResourceString(IDS_RENAMEPROJECT),
															theWorkspace.GetAppKey(),
															MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1 );
	if( nWhatNext != IDNO )
		return false;
	if( nWhatNext == IDYES )
	{
		if( !theWorkspace.GetActiveDocument()->DoFileSave() )
			return false;
		HTREEITEM hItem = mObjectTree.GetSelectedItem();
		if( hItem != NULL )
		{
			CTreeNode* pItem = (CTreeNode*)mObjectTree.GetItemData( hItem );
			if( pItem )
				pItem->onSelected( *this );
		}
	}
	return true;
}

void CControlBrowser::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT, mDescription);
	DDX_Control(pDX, IDC_CONTROLTREE, mObjectTree);
}


BEGIN_MESSAGE_MAP(CControlBrowser, CResizableDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_CONTROLTREE, CControlBrowser::OnSelchanged)
	ON_BN_CLICKED(IDC_COPY1, OnCopy1)
	ON_BN_CLICKED(IDC_COPY2, OnCopy2)
END_MESSAGE_MAP()


// CControlBrowser message handlers

INT_PTR CControlBrowser::DoModal()
{
	DisableUndoManager DisableUndo( mpDclControl->GetUndoManager() );
	return __super::DoModal();
}

void CControlBrowser::OnDestroy() 
{
	HTREEITEM hCurrent = NULL;
	while( (hCurrent = mObjectTree.GetChildItem( TVI_ROOT )) != NULL )
	{
		HTREEITEM hChild = NULL;
		while( (hChild = mObjectTree.GetChildItem( hCurrent )) != NULL )
			RemoveItem( hChild );
		RemoveItem( hCurrent );
	}
	__super::OnDestroy();
}

BOOL CControlBrowser::OnInitDialog()
{
	__super::OnInitDialog();

	mImageList.Create( 16, 16, ILC_COLOR | ILC_MASK, 1, 1 );
	HMODULE hmodRes = theWorkspace.GetLocalResourceModule();
	mImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE( IDI_ENUMS ) ) );
	mImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE( IDI_ENUM ) ) );
	mImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE( IDI_EVENT ) ) );
	mImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE( IDI_METHOD ) ) );
	mImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE( IDI_PROPERTY ) ) );
	mImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE( IDI_OLEOBJECT ) ) );
	mImageList.SetBkColor( mObjectTree.GetBkColor() );
	mObjectTree.SetImageList( &mImageList, TVSIL_NORMAL );
	EnableSaveRestore( _T("ObjectBrowser"), _T("Size") );

	CTreeNode* pMain = NULL;
	if( mpDclControl->GetType() == _CtlForm )
		pMain = new CDclFormNode( mpDclControl->GetOwnerForm() );
	else
		pMain = new CDclControlNode( mpDclControl );
	mObjectTree.Expand( InsertItem( TVI_ROOT, pMain ), TVE_EXPAND );

	ShowSizeGrip( TRUE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CControlBrowser::OnSize(UINT nType, int cx, int cy)
{

	if( mszPrevious.cx < 0 )
	{
		CRect rcDlg;
		GetClientRect( &rcDlg );
		mszPrevious.cx = rcDlg.Width();
		mszPrevious.cy = rcDlg.Height();
	}
	int nDeltaX = cx - mszPrevious.cx;
	int nDeltaY = cy - mszPrevious.cy;
	mszPrevious.cx = cx;
	mszPrevious.cy = cy;

	__super::OnSize(nType, cx, cy);

	if( nType != SIZE_RESTORED || (nDeltaX == 0 && nDeltaY == 0) )
		return;

	CRect rcCtrl;
	mObjectTree.GetWindowRect( &rcCtrl );
	ScreenToClient( &rcCtrl );
	rcCtrl.bottom += nDeltaY;
	mObjectTree.MoveWindow( &rcCtrl );

	mDescription.GetWindowRect( &rcCtrl );
	ScreenToClient( &rcCtrl );
	rcCtrl.right += nDeltaX;
	rcCtrl.bottom += nDeltaY;
	mDescription.MoveWindow( &rcCtrl );

	CWnd* pCtrl = GetDlgItem( IDC_METHDEF );
	if( pCtrl )
	{
		pCtrl->GetWindowRect( &rcCtrl );
		ScreenToClient( &rcCtrl );
		rcCtrl.right += nDeltaX;
		pCtrl->MoveWindow( &rcCtrl );
	}

	pCtrl = GetDlgItem( IDOK );
	if( pCtrl )
	{
		pCtrl->GetWindowRect( &rcCtrl );
		ScreenToClient( &rcCtrl );
		rcCtrl.left += nDeltaX;
		rcCtrl.right += nDeltaX;
		pCtrl->MoveWindow( &rcCtrl );
	}

	int nDeltaHalfX = nDeltaX / 2;
	CRect rcCopy1;
	CRect rcCopy2;
	CWnd* pCopy1Wnd = GetDlgItem( IDC_COPY1 );
	CWnd* pCopy2Wnd = GetDlgItem( IDC_COPY2 );
	if( pCopy1Wnd )
		pCopy1Wnd->GetWindowRect( &rcCopy1 );
	if( pCopy2Wnd )
		pCopy2Wnd->GetWindowRect( &rcCopy2 );
	if( pCopy1Wnd && pCopy2Wnd )
		nDeltaHalfX -= (rcCopy1.Width() - rcCopy2.Width()) / 2;
	if( pCopy1Wnd )
	{
		ScreenToClient( &rcCopy1 );
		rcCopy1.right += nDeltaHalfX;
		rcCopy1.top += nDeltaY;
		rcCopy1.bottom += nDeltaY;
		pCopy1Wnd->MoveWindow( &rcCopy1 );
	}

	if( pCopy2Wnd )
	{
		ScreenToClient( &rcCopy2 );
		rcCopy2.left += nDeltaHalfX;
		rcCopy2.right += nDeltaX;
		rcCopy2.top += nDeltaY;
		rcCopy2.bottom += nDeltaY;
		pCopy2Wnd->MoveWindow( &rcCopy2 );
	}
}

void CControlBrowser::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if( hItem == NULL )
		hItem = mObjectTree.GetSelectedItem();
	if( hItem != NULL )
	{
		CTreeNode* pItem = (CTreeNode*)mObjectTree.GetItemData( hItem );
		if( pItem )
			pItem->onSelected( *this );
	}
	*pResult = 0;
}

void CControlBrowser::OnCopy1() 
{
	if( !OnBeginClipboardCopy() )
		return;
	CStringA source( msCopy1Lisp ); 
	if( OpenClipboard() )
	{
		EmptyClipboard();
		HGLOBAL clipbuffer = GlobalAlloc( GMEM_DDESHARE, source.GetLength() + 1 );
		CHAR* buffer = (CHAR*)GlobalLock( clipbuffer );
		lstrcpynA( buffer, (LPCSTR)source, source.GetLength() + 1 );
		GlobalUnlock( clipbuffer );
		SetClipboardData( CF_TEXT, clipbuffer );
		CloseClipboard();
	}
}

void CControlBrowser::OnCopy2() 
{
	if( !OnBeginClipboardCopy() )
		return;
	CStringA source( msCopy2Lisp ); 
	if( OpenClipboard() )
	{
		EmptyClipboard();
		HGLOBAL clipbuffer = GlobalAlloc( GMEM_DDESHARE, source.GetLength() + 1 );
		CHAR* buffer = (CHAR*)GlobalLock( clipbuffer );
		lstrcpynA( buffer, (LPCSTR)source, source.GetLength() + 1 );
		GlobalUnlock( clipbuffer );
		SetClipboardData( CF_TEXT, clipbuffer );
		CloseClipboard();
	}
}
