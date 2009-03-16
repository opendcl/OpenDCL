// ControlBrowser.cpp : implementation file
//

#include "stdafx.h"
#include "ControlBrowser.h"
#include "Workspace.h"
#include "DclControlObject.h"
#include "DclFormObject.h"
#include "UndoManager.h"
#include "ControlName.h"
#include "FormName.h"
#include "StdioUnicodeFile.h"
#include "VARUtils.h"
#include "AxMethodDescriptor.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "AxContainerCtrl.h"
#include "OpenDCL.h"
#include "ChmLib.h"


static CString ConstructTypeNameHtml( LPCTSTR pszTypeName, LPCTSTR pszDisplayName = NULL )
{
	CString sTypeNameHtml( pszTypeName );
	if( pszTypeName && *pszTypeName )
	{
		CString sFilename;
		sFilename.Format( _T("chm://Reference/DataType/%s.htm"), (LPCTSTR)pszTypeName );
		if( IsChmFile( sFilename ) )
		{
			CString sLinkedType;
			sLinkedType.Format( _T("<a href=\"../DataType/%s.htm\">%s</a>"), pszTypeName, (pszDisplayName && *pszTypeName)? pszDisplayName : pszTypeName );
			sTypeNameHtml = sLinkedType;
		}
	}
	return sTypeNameHtml;
}


static void CompactWhiteSpace( CString& sHtml )
{
	sHtml.Trim( _T(" \t") );
	sHtml.Remove( _T('\r') );
	sHtml.Remove( _T('\n') );
	CString sCompact;
	int nSpace = 0;
	do
	{
		sCompact += sHtml.Tokenize( _T(" \t"), nSpace );
		sCompact += _T(" ");
	} while( nSpace >= 0 );
	sHtml = sCompact.Trim();
}


class CTreeNode
{
	CString msName;
public:
	CTreeNode( LPCTSTR pszName ) : msName( pszName ) {}
	virtual ~CTreeNode() {}

	virtual void onSelected( CControlBrowser& Browser )
		{
			std::map< CString, CString > params;
			Browser.SetDescription( description( params ), params );
		}

	virtual LPCTSTR name() const { return msName; }
	virtual int image() const { return -1; }
	virtual CString description( std::map< CString, CString >& params ) const { return NULL; }
	virtual bool addChildItems( CControlBrowser& Browser, HTREEITEM hParent ) { return true; }
	static bool addMethods( CControlBrowser& Browser, HTREEITEM hParent, LPCTSTR pszControlType, CString& sMethodsHtml );
	static bool addEvent( CControlBrowser& Browser, HTREEITEM hParent, TPropertyPtr pEvent );
};

class CDclControlNode : public CTreeNode
{
	TDclControlPtr mpDclControl;
public:
	CDclControlNode( TDclControlPtr pDclControl )
		: CTreeNode( GetControlDisplayName( pDclControl ) )
		, mpDclControl( pDclControl )
		{}
	CDclControlNode( TDclControlPtr pDclControl, LPCTSTR pszName )
		: CTreeNode( pszName )
		, mpDclControl( pDclControl )
		{}
	virtual ~CDclControlNode() {}

	virtual int image() const { return 5; }
	virtual bool addChildItems( CControlBrowser& Browser, HTREEITEM hParent );
	virtual CString description( std::map< CString, CString >& params ) const
		{
			CString sPath;
			sPath.Format( _T("chm://Reference/Control/%s.htm"),
										(LPCTSTR)GetControlApiName( mpDclControl ) );
			return sPath;
		}

	virtual bool addPropertyChildItems( CControlBrowser& Browser, HTREEITEM hParent, CAxContainerCtrl* pAxCont = NULL );
};

class CDclFormNode : public CDclControlNode
{
	TDclFormPtr mpDclForm;

public:
	CDclFormNode( TDclFormPtr pDclForm )
		: CDclControlNode( pDclForm->GetControlProperties(), GetFormDisplayName( pDclForm ) )
		, mpDclForm( pDclForm )
		{}
	virtual ~CDclFormNode() {}
	virtual CString description( std::map< CString, CString >& params ) const
		{
			CString sPath;
			sPath.Format( _T("chm://Reference/Form/%s.htm"),
										(LPCTSTR)GetFormApiName( mpDclForm ) );
			return sPath;
		}
	virtual bool addChildItems( CControlBrowser& Browser, HTREEITEM hParent );
};

class CDclProjectNode : public CTreeNode
{
	TProjectPtr mpProject;

public:
	CDclProjectNode( TProjectPtr pProject )
		: CTreeNode( pProject->GetKeyName() )
		, mpProject( pProject )
		{}
	virtual ~CDclProjectNode() {}
	virtual int image() const { return 1; }
	virtual CString description( std::map< CString, CString >& params ) const
		{
			return _T("chm://Reference/DataType/Project.htm");
		}
	virtual bool addChildItems( CControlBrowser& Browser, HTREEITEM hParent );
};

class CPropertyNode : public CTreeNode
{
	TPropertyPtr mpProp;

public:
	CPropertyNode( TPropertyPtr pProp )
		: CTreeNode( pProp->GetName() )
		, mpProp( pProp )
		{
		}
	virtual ~CPropertyNode() {}
	virtual TPropertyPtr prop() const { return mpProp; }

	virtual int image() const { return 4; }
	virtual CString description( std::map< CString, CString >& params ) const
		{
			CString sPath;
			sPath.Format( _T("chm://Reference/Property/%s.htm"),
										(LPCTSTR)GetPropertyApiName( mpProp->GetID() ) );
			return sPath;
		}

protected:
	virtual bool isRuntimeNotAllowed()
		{
			bool bAllowed = true;
			switch( prop()->GetID() )
			{
			case Prop::ComboBoxStyle:
			case Prop::GraphicButtonStyle:
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
};

class CMethodNode : public CTreeNode
{
	TDclControlPtr mpDclControl;
	CString msFilename;

public:
	CMethodNode( TDclControlPtr pDclControl, LPCTSTR pszName, LPCTSTR pszFilename )
		: CTreeNode( pszName )
		, mpDclControl( pDclControl )
		, msFilename( pszFilename )
		{}
	virtual ~CMethodNode() {}
	virtual TDclControlPtr control() const { return mpDclControl; }

	virtual int image() const { return 3; }
	virtual CString description( std::map< CString, CString >& params ) const
		{
			CString sPath;
			sPath.Format( _T("chm://Reference/Method/%s"), (LPCTSTR)msFilename );
			return sPath;
		}
};

class CEventNode : public CTreeNode
{
	TPropertyPtr mpProp;
	CString msFilename;

public:
	CEventNode( TPropertyPtr pProp, LPCTSTR pszFilename )
		: CTreeNode( pProp->GetType() == PropActiveXEvent? pProp->GetConstAxInterfaceDescriptorPtr()->GetName() : pProp->GetName() )
		, mpProp( pProp )
		, msFilename( pszFilename )
		{
		}
	virtual ~CEventNode() {}
	virtual TPropertyPtr prop() const { return mpProp; }

	virtual int image() const { return 2; }
	virtual CString description( std::map< CString, CString >& params ) const
		{
			params[_T("<CONTROL-NAME>")] = mpProp->GetOwnerControl()->GetVarName();
			CString sPath;
			sPath.Format( _T("chm://Reference/Event/%s"),
										(LPCTSTR)msFilename );
			return sPath;
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
			CString sMethodsHtml;
			addMethods( Browser, hParent, _T("AxObject"), sMethodsHtml );
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
									 CAxContainerCtrl* pAxCont = NULL )
		: CPropertyNode( pProp )
		, mpIDesc( pIDesc )
		, mpAxCont( pAxCont )
		{
		}
	virtual ~CAxPropertyNode() {}
	CAxContainerCtrl* axCont() const { return mpAxCont; }

	virtual CString description( std::map< CString, CString >& params ) const
		{
			CString sName = name();
			params[_T("{TITLE}")] = sName;
			params[_T("{FRIENDLYNAME}")] = sName;
			params[_T("{APINAME}")] = sName;
			params[_T("{TYPE}")] = ConstructTypeNameHtml( propType() );
			params[_T("{RESTRICTIONS}")] = _T("&nbsp;");
			bool bAxControl = (prop()->GetOwnerControl()->GetType() == CtlActiveX);
			CString sAx = bAxControl? _T("AxControl") : _T("AxObject");
			CString sOb = bAxControl? prop()->GetOwnerControl()->GetVarName() : _T("&lt;AXOBJECT&gt;");
			CString sObType = ConstructTypeNameHtml( bAxControl? _T("Control") : _T("AxObject"), sOb );
			CString sGetFunction;
			sGetFunction.Format(
				_T("(dcl_%s_Get <i>%s</i> <font color=\"brown\">\"%s\"</font>%s)"),
				(LPCTSTR)sAx, (LPCTSTR)sObType, (LPCTSTR)sName, (LPCTSTR)propGetArgList() );
			params[_T("{GETPROPFUNCTION}")] = sGetFunction;
			CString sSetFunction = _T("&nbsp;");
			if( mpIDesc->GetPropPut() || mpIDesc->GetPropPutRef() )
				sSetFunction.Format(
					_T("(dcl_%s_Put <i>%s</i> <font color=\"brown\">\"%s\"</font>%s)"),
					(LPCTSTR)sAx, (LPCTSTR)sObType, (LPCTSTR)sName, (LPCTSTR)propPutArgList() );
			params[_T("{SETPROPFUNCTION}")] = sSetFunction;
			params[_T("{APPLIESTO}")] = _T("&nbsp;");
			CString sDesc;
			sDesc.Format( _T("<p>%s</p>%s"), (LPCTSTR)mpIDesc->GetDesc(), (LPCTSTR)propDescExtra() );
			params[_T("{DESCRIPTION}")] = sDesc;
			return _T("chm://Reference/Property/@Template.htm");
		}
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
				return pAxPropGet->GetTypeDisplayName();
			AxPropertyDescriptor* pAxPropPut = mpIDesc->GetPutDescriptor();
			if( pAxPropPut )
				return pAxPropPut->GetTypeDisplayName();
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
					if( sArg.IsEmpty() )
						sArg = _T("Arg");
					CString sType = ConstructTypeNameHtml( AxTypeToDisplayableLispType( arg.vt, arg.clsid ) );
					if( sType.IsEmpty() )
						sType = _T("??");
					CString sArgHtml;
					if( arg.optional )
						sArgHtml.Format( _T(" <font color=\"red\">{%s [%s %s]}</font>"),
														 (LPCTSTR)sArg,
														 (LPCTSTR)theWorkspace.LoadResourceString( IDS_AS ),
														 (LPCTSTR)sType );
					else
						sArgHtml.Format( _T(" <font color=\"red\">%s [%s %s]</font>"),
														 (LPCTSTR)sArg,
														 (LPCTSTR)theWorkspace.LoadResourceString( IDS_AS ),
														 (LPCTSTR)sType );
					sArgList += sArgHtml;
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
					bool bLastArg = ((iter + 1) == rArgs.end());
					const AxArg& arg = *iter;
					CString sArg = arg.name;
					if( sArg.IsEmpty() )
						sArg = bLastArg? _T("NewValue") : _T("Arg");
					CString sType = AxTypeToDisplayableLispType( arg.vt, arg.clsid );
					if( sType.IsEmpty() )
						sType = bLastArg? propType() : _T("??");
					else if( bLastArg )
					{
						if( sType == _T("AxObject") )
							sType = propType();
						else if( sType == _T("Long") && propType() == _T("OLEColor") )
							sType = _T("OLEColor"); //it's really an OLEColor
					}
					CString sTypeHtml = ConstructTypeNameHtml( sType );
					CString sArgHtml;
					if( arg.optional )
						sArgHtml.Format( _T(" <font color=\"red\">{%s [%s %s]}</font>"),
														 (LPCTSTR)sArg,
														 (LPCTSTR)theWorkspace.LoadResourceString( IDS_AS ),
														 (LPCTSTR)sTypeHtml );
					else
						sArgHtml.Format( _T(" <font color=\"red\">%s [%s %s]</font>"),
														 (LPCTSTR)sArg,
														 (LPCTSTR)theWorkspace.LoadResourceString( IDS_AS ),
														 (LPCTSTR)sTypeHtml );
					sArgList += sArgHtml;
				}
			}
			return sArgList;
		}
	virtual CString propDescExtra() const
		{
			CString sExtraInfo;
			switch( mpIDesc->GetType() )
			{
			case VT_DISPATCH:
			case VT_UNKNOWN:
				{
					CString sObjName = _T("ActiveX Object");
					if( mpIDesc->GetGuid() != GUID_NULL )
						sObjName = propType();
					CString sFmt;
					sFmt.Format( theWorkspace.LoadResourceString( IDS_AXNOTERELEASEOBJECT ), (LPCTSTR)sObjName );
					sExtraInfo += sFmt;
				}
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
								 CAxContainerCtrl* pAxCont = NULL )
		: CMethodNode( pProp->GetOwnerControl(), pMethodDesc->GetName(), NULL )
		, mpMethodDesc( pMethodDesc )
		, mpAxCont( pAxCont )
		{
		}
	virtual ~CAxMethodNode() {}
	CAxContainerCtrl* axCont() const { return mpAxCont; }

	virtual CString description( std::map< CString, CString >& params ) const
		{
			CString sName = name();
			params[_T("{TITLE}")] = sName;
			params[_T("{FUNCTIONNAME}")] = sName;
			params[_T("{TYPE}")] = ConstructTypeNameHtml( methodType() );
			bool bAxControl = (control()->GetType() == CtlActiveX);
			CString sAx = bAxControl? _T("AxControl") : _T("AxObject");
			CString sOb = bAxControl? control()->GetVarName() : _T("&lt;AXOBJECT&gt;");
			CString sObType = ConstructTypeNameHtml( bAxControl? _T("Control") : _T("AxObject"), sOb );
			CString sFunction;
			sFunction.Format(
				_T("(dcl_%s_Invoke <i>%s</i> <font color=\"brown\">\"%s\"</font>%s)"),
				(LPCTSTR)sAx, (LPCTSTR)sObType, (LPCTSTR)sName, (LPCTSTR)methodArgList() );
			params[_T("{FUNCTION}")] = sFunction;
			params[_T("{APPLIESTO}")] = _T("&nbsp;");
			CString sDesc;
			sDesc.Format( _T("<p>%s</p>%s"), (LPCTSTR)methodDesc(), (LPCTSTR)methodDescExtra() );
			params[_T("{DESCRIPTION}")] = sDesc;
			return _T("chm://Reference/Method/@Template.htm");
		}
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
				if( sArg.IsEmpty() )
					sArg = _T("Arg");
				CString sType = ConstructTypeNameHtml( AxTypeToDisplayableLispType( arg.vt, arg.clsid ) );
				if( sType.IsEmpty() )
					sType = _T("??");
				CString sArgHtml;
				if( arg.optional )
					sArgHtml.Format( _T(" <font color=\"red\">{%s [%s %s]}</font>"),
													 (LPCTSTR)sArg,
													 (LPCTSTR)theWorkspace.LoadResourceString( IDS_AS ),
													 (LPCTSTR)sType );
				else
					sArgHtml.Format( _T(" <font color=\"red\">%s [%s %s]</font>"),
													 (LPCTSTR)sArg,
													 (LPCTSTR)theWorkspace.LoadResourceString( IDS_AS ),
													 (LPCTSTR)sType );
				sArgList += sArgHtml;
			}
			return sArgList;
		}
	virtual CString methodDescExtra() const
		{
			CString sExtraInfo;
			switch( mpMethodDesc->GetReturnType() )
			{
			case VT_DISPATCH:
			case VT_UNKNOWN:
				{
					CString sObjName = _T("ActiveX Object");
					if( mpMethodDesc->GetReturnGuid() != GUID_NULL )
						sObjName = methodType();
					CString sFmt;
					sFmt.Format( theWorkspace.LoadResourceString( IDS_AXNOTERELEASEOBJECT ), (LPCTSTR)sObjName );
					sExtraInfo += sFmt;
				}
				break;
			}
			sExtraInfo += _T("<p>");
			sExtraInfo += theWorkspace.LoadResourceString( IDS_AXMETHODMOREINFO );
			if( control()->IsMicrosoftActiveXCtrl() )
				sExtraInfo += theWorkspace.LoadResourceString( IDS_AXMETHODMSDN );
			sExtraInfo += _T("</p>");
			return sExtraInfo;
		}
};

class CAxEventNode : public CEventNode
{
	const AxEventDescriptor* mpEventDesc;
	CAxContainerCtrl* mpAxCont;

public:
	CAxEventNode( TPropertyPtr pProp, const AxEventDescriptor* pEventDesc,
								CAxContainerCtrl* pAxCont = NULL )
		: CEventNode( pProp, NULL )
		, mpEventDesc( pEventDesc )
		, mpAxCont( pAxCont )
		{
		}
	virtual ~CAxEventNode() {}
	CAxContainerCtrl* axCont() const { return mpAxCont; }

protected:

	virtual CString description( std::map< CString, CString >& params ) const
		{
			CString sName = name();
			params[_T("{TITLE}")] = sName;
			params[_T("{TYPE}")] = _T("&nbsp;");
			CString sOb = prop()->GetOwnerControl()->GetVarName();
			CString sFunctionName;
			sFunctionName.Format( _T("c:%s_On%s"), (LPCTSTR)sOb, (LPCTSTR)sName );
			params[_T("{FUNCTIONNAME}")] = sFunctionName;
			CString sEventArgList = eventArgList().Trim();
			if( sEventArgList.IsEmpty() )
				sEventArgList = _T("/");
			else
			{
				CString sFont = _T("<font color=\"red\">");
				sFont += sEventArgList;
				sFont += _T("</font> /");
				sEventArgList = sFont;
			}
			CString sFunction;
			sFunction.Format(
				_T("(<font color=\"brown\">defun</font> %s (%s)<br />)"),
				(LPCTSTR)sFunctionName, (LPCTSTR)sEventArgList );
			params[_T("{FUNCTION}")] = sFunction;
			params[_T("{APPLIESTO}")] = _T("&nbsp;");
			CString sDesc;
			sDesc.Format( _T("<p>%s</p>"), (LPCTSTR)mpEventDesc->GetDesc() );
			params[_T("{DESCRIPTION}")] = sDesc;
			return _T("chm://Reference/Event/@Template.htm");
		}
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
				if( sArg.IsEmpty() )
					sArg = _T("Arg");
				CString sType = ConstructTypeNameHtml( AxTypeToDisplayableLispType( arg.vt, arg.clsid ) );
				if( sType.IsEmpty() )
					sType = _T("??");
				CString sArgHtml;
				sArgHtml.Format( _T(" <font color=\"red\">%s [%s %s]</font>"),
												 (LPCTSTR)sArg,
												 (LPCTSTR)theWorkspace.LoadResourceString( IDS_AS ),
												 (LPCTSTR)sType );
				sArgList += sArgHtml;
			}
			return sArgList;
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
	CString sMethodsHtml;
	addMethods( Browser, hParent, GetControlApiName( type ), sMethodsHtml );
	if( type != CtlFileExplorer )
		addMethods( Browser, hParent, _T("Control"), sMethodsHtml );
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
		case Prop::VarName:
			continue;
		}
		switch( pProp->GetType() )
		{
		case PropEvent:
			addEvent( Browser, hParent, pProp );
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
		//case PropImageList:
		//case PropStringArray:
		//case PropIntArray:
			break;
		case PropActiveXProp:
		case PropActiveXRunTime:
		case PropActiveXEnum:
			{
				//if( pProp->IsHidden() )
				//	continue;
				const AxInterfaceDescriptor* pPropDesc = pProp->GetConstAxInterfaceDescriptorPtr();
				if( !pPropDesc )
					continue;
				Browser.InsertItem( hParent, new CAxPropertyNode( pProp, pPropDesc, pAxCont ) );
			}
			break;
		default:
			bool bHidden = pProp->IsHidden();
			switch( pProp->GetID() )
			{
			case Prop::FontSize:
			case Prop::FontBold:
			case Prop::FontItalic:
			case Prop::FontStrikeout:
			case Prop::FontUnderline:
			case Prop::ToolTipPicture:
			case Prop::ToolTipBalloon:
			case Prop::ToolTipTitleColor:
			case Prop::ToolTipLine:
			case Prop::ToolTipTitle:
				bHidden = false; //these hidden properties should be treated like normal properties
				break;
			}
			if( bHidden )
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
	CString sMethodsHtml;
	addMethods( Browser, hParent, _T("Form"), sMethodsHtml );
	//addMethods( Browser, hParent, _T("Project"), sMethodsHtml );
	CDclProjectNode* pItem = new CDclProjectNode( mpDclForm->GetProject() );
	HTREEITEM hItem = Browser.InsertItem( TVI_ROOT, pItem );
	assert( hItem != NULL );
	return true;
}

bool CDclProjectNode::addChildItems( CControlBrowser& Browser, HTREEITEM hParent )
{
	CString sMethodsHtml;
	addMethods( Browser, hParent, _T("Project"), sMethodsHtml );
	return true;
}

bool CTreeNode::addMethods( CControlBrowser& Browser, HTREEITEM hParent, LPCTSTR pszControlType, CString& sMethodsHtml )
{
	if( sMethodsHtml.IsEmpty() )
	{
		if( !ReadChmFile( _T("/Reference/Method/Index.htm"), sMethodsHtml ) )
			return false;
	}

	int nControlMethods = sMethodsHtml.Find( CString( _T("<a name=\"") ) + pszControlType + _T("\"") );
	if( nControlMethods >= 0 )
	{
		int nStart = sMethodsHtml.Find( _T("<ul"), nControlMethods );
		if( nStart >= nControlMethods )
		{
			int nEnd = sMethodsHtml.Find( _T("</ul>"), nStart );
			if( nEnd > nStart )
			{
				for( int nLineStart = sMethodsHtml.Find( _T("<li"), nStart );
						 nLineStart < nEnd;
						 nLineStart = sMethodsHtml.Find( _T("<li"), nLineStart + 4 ) )
				{
					int nLineEnd = sMethodsHtml.Find( _T("</li>"), nLineStart );
					if( nLineEnd < nLineStart )
						nLineEnd = sMethodsHtml.Find( _T("<li"), nLineStart );
					if( nLineEnd < nLineStart )
						break;
					CString sLine = sMethodsHtml.Mid( nLineStart, nLineEnd - nLineStart );
					int nMethodFile = sLine.Find( _T("href=\"") );
					if( nMethodFile < 0 )
						continue;
					sLine = sLine.Mid( nMethodFile + 6 );
					CString sMethodFile = sLine.SpanExcluding( _T("\"") );
					sLine = sLine.Mid( sLine.SpanExcluding( _T(">") ).GetLength() + 1 );
					CString sMethodName = sLine.SpanExcluding( _T("<") );
					CompactWhiteSpace( sMethodName );
					if( sMethodName == _T("Dump") )
						continue; //don't need to show this one in the browser

					CMethodNode* pItem = new CMethodNode( Browser.GetMainControl(), sMethodName, sMethodFile );
					HTREEITEM hItem = Browser.InsertItem( hParent, pItem );
					assert( hItem != NULL );
					if( !hItem )
						return false;
				}
			}
		}
	}
	return true;
}

bool CTreeNode::addEvent( CControlBrowser& Browser, HTREEITEM hParent, TPropertyPtr pEvent )
{
	TDclControlPtr pDclControl = pEvent->GetOwnerControl();
	CString sControlFile;
	if( pDclControl->GetType() == _CtlForm )
		sControlFile.Format( _T("/Reference/Form/%s.htm"), (LPCTSTR)GetFormApiName( pDclControl->GetOwnerForm() ) );
	else
		sControlFile.Format( _T("/Reference/Control/%s.htm"), (LPCTSTR)GetControlApiName( pDclControl ) );
	CString sControlHtml;
	if( !ReadChmFile( sControlFile, sControlHtml ) )
		return false;

	CString sTargetEvent = pEvent->GetApiName();
	CString sFilename = sTargetEvent + _T(".htm");
	int nControlMethods = sControlHtml.Find( CString( _T("<a name=\"Events\"") ) );
	if( nControlMethods >= 0 )
	{
		int nStart = sControlHtml.Find( _T("<table"), nControlMethods );
		if( nStart >= nControlMethods )
		{
			int nEnd = sControlHtml.Find( _T("</table>"), nStart );
			if( nEnd > nStart )
			{
				for( int nLineStart = sControlHtml.Find( _T("<li"), nStart );
						 nLineStart >= 0 && nLineStart < nEnd;
						 nLineStart = sControlHtml.Find( _T("<li"), nLineStart + 4 ) )
				{
					int nLineEnd = sControlHtml.Find( _T("</li>"), nLineStart );
					if( nLineEnd < nLineStart )
						nLineEnd = sControlHtml.Find( _T("<li"), nLineStart );
					if( nLineEnd < nLineStart )
						break;
					CString sLine = sControlHtml.Mid( nLineStart, nLineEnd - nLineStart );
					int nEventFile = sLine.Find( _T("href=\"") );
					if( nEventFile < 0 )
						continue;
					sLine = sLine.Mid( nEventFile + 6 );
					CString sHref = sLine.SpanExcluding( _T("\"") );
					sLine = sLine.Mid( sLine.SpanExcluding( _T(">") ).GetLength() + 1 );
					CString sEventName = sLine.SpanExcluding( _T(" <") );
					CompactWhiteSpace( sEventName );
					if( sEventName.CompareNoCase( sTargetEvent ) == 0 )
					{
						sFilename = sHref.MakeReverse().SpanExcluding( _T("\\/") ).MakeReverse();
						break;
					}
				}
			}
		}
	}
	HTREEITEM hItem = Browser.InsertItem( hParent, new CEventNode( pEvent, sFilename ) );
	assert( hItem != NULL );
	if( !hItem )
		return false;
	return true;
}


// CControlBrowser dialog

CControlBrowser::CControlBrowser( TDclControlPtr pDclControl, CWnd* pParent /*=NULL*/ )
	: CResizableDialog( CControlBrowser::IDD, pParent )
	, mpDclControl( pDclControl )
	, mnInitialProp( Prop::_Private )
	, mDescription( *this )
	, mbClosing( false )
	, mszPrevious( -1, -1 )
{
}

CControlBrowser::CControlBrowser( TPropertyPtr pProp, CWnd* pParent /*=NULL*/ )
	: CResizableDialog( CControlBrowser::IDD, pParent )
	, mpDclControl( pProp->GetOwnerControl() )
	, mnInitialProp( pProp->GetID() )
	, mDescription( *this )
	, mbClosing( false )
	, mszPrevious( -1, -1 )
{
}

CControlBrowser::~CControlBrowser()
{
}

void CControlBrowser::NoNavigateBrowser::OnDocumentComplete(LPCTSTR lpszURL)
{
	mbEnableNavigate = false;
	__super::OnDocumentComplete( lpszURL );

	ReplaceText( _T("<CONTROL>"), mBrowser.GetMainControl()->GetVarName() );
	TProjectPtr pProject = mBrowser.GetMainControl()->GetOwnerProject();
	if( pProject )
	{
		CString sProjectKey = _T("\"");
		sProjectKey += pProject->GetKeyName();
		sProjectKey += _T("\"");
		ReplaceText( _T("<PROJECT>"), sProjectKey );
	}
	mBrowser.OnDocumentLoaded();
}

void CControlBrowser::NoNavigateBrowser::OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel)
{
	mbEnableNavigate = false;
	__super::OnNavigateError( lpszURL, lpszFrame, dwError, pbCancel );
	//TODO: Cancel the default error page and display a custom localized error page
	//assert( pbCancel != NULL );
	//*pbCancel = TRUE;
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

void CControlBrowser::SetDescription( LPCTSTR pszDescription, const std::map< CString, CString >& params )
{
	mParams = params;
	if( CompareString( LOCALE_INVARIANT, NORM_IGNORECASE, _T("file://"), 7, pszDescription, 7 ) == CSTR_EQUAL )
		mDescription.Navigate( pszDescription );
	else if( CompareString( LOCALE_INVARIANT, NORM_IGNORECASE, _T("chm://"), 6, pszDescription, 6 ) == CSTR_EQUAL )
	{
		CString sChmUrl;
		sChmUrl.Format( _T("mk:@MSITStore:%s::%s"), AfxGetApp()->m_pszHelpFilePath, (LPCTSTR)CString( pszDescription ).Mid( 5 ) );
		mDescription.Navigate( sChmUrl );
	}
	else
		mDescription.LoadHtmlCode( pszDescription );
}

void CControlBrowser::OnDocumentLoaded()
{
	for( std::map< CString, CString >::const_iterator iter = mParams.begin();
			 iter != mParams.end();
			 ++iter )
	{
		mDescription.ReplaceText( iter->first, iter->second );
	}
}

void CControlBrowser::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DESCRIPTIONHTML, mDescription);
	DDX_Control(pDX, IDC_CONTROLTREE, mObjectTree);
}


BEGIN_MESSAGE_MAP(CControlBrowser, CResizableDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_CONTROLTREE, CControlBrowser::OnSelchanged)
END_MESSAGE_MAP()


// CControlBrowser message handlers

INT_PTR CControlBrowser::DoModal()
{
	DisableUndoManager DisableUndo( mpDclControl->GetUndoManager() );
	return __super::DoModal();
}

void CControlBrowser::OnDestroy() 
{
	mbClosing = true;
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
	SetIcon( LoadIcon( theWorkspace.GetLocalResourceModule(), MAKEINTRESOURCE( IDI_CONTROLBROWSER ) ), FALSE );
	__super::OnInitDialog();

	mImageList.Create( 16, 16, ILC_COLOR | ILC_MASK, 1, 1 );
	HMODULE hmodRes = theWorkspace.GetLocalResourceModule();
	mImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE( IDI_ENUMS ) ) );
	mImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE( IDI_ENUM ) ) );
	mImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE( IDI_EVENT ) ) );
	mImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE( IDI_METHOD ) ) );
	mImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE( IDI_PROPERTY ) ) );
	mImageList.Add( LoadIcon( hmodRes, MAKEINTRESOURCE( IDI_AXOBJECT ) ) );
	mImageList.SetBkColor( mObjectTree.GetBkColor() );
	mObjectTree.SetImageList( &mImageList, TVSIL_NORMAL );
	EnableSaveRestore( _T("ControlBrowser"), _T("Size") );

	CTreeNode* pMain = NULL;
	if( mpDclControl->GetType() == _CtlForm )
		pMain = new CDclFormNode( mpDclControl->GetOwnerForm() );
	else
		pMain = new CDclControlNode( mpDclControl );
	HTREEITEM htiControl = InsertItem( TVI_ROOT, pMain );
	mObjectTree.Expand( htiControl, TVE_EXPAND );
	if( mnInitialProp != Prop::_Private )
	{
		CString sPropName = GetPropertyName( mnInitialProp );
		HTREEITEM hCursor = NULL;
		for( hCursor = mObjectTree.GetChildItem( htiControl );
				 hCursor;
				 hCursor = mObjectTree.GetNextSiblingItem( hCursor ) )
		{
			int nImage = -1;
			int nSelImage = -1;
			mObjectTree.GetItemImage( hCursor, nImage, nSelImage );
			if( (nImage == 4 || nImage == 2) && sPropName == mObjectTree.GetItemText( hCursor ) )
			{
				mObjectTree.SelectItem( hCursor );
				break;
			}
		}
		if( !hCursor )
			mObjectTree.SelectItem( htiControl );
	}
	else
		mObjectTree.SelectItem( htiControl );

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
}

void CControlBrowser::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if( !mbClosing )
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
	}
	*pResult = 0;
}
