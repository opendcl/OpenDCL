// AxContainer.cpp : implementation file
//

#include "stdafx.h"
#include "AxContainerCtrl.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxMethodDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "ControlPane.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "OleLicenseKey.h"
#include "OleFont.h"
#include "PictureObject.h"
#include "Project.h"
#include "Workspace.h"
#include "VarUtils.h"
#include "PropertyIds.h"
#include "Resource.h"


// function type to return script language dependent strings
typedef CString (*TFScriptSymSet)(UINT nID);
#define SCRTYPE_COMMA 8
#define SCRTYPE_LPAREN 9
#define SCRTYPE_RPAREN 10

/*
 * ObjectTypeInfo
 *
 * Purpose:
 *  Retrieves the ITypeInfo for the entire object from which
 *  one can learn the IID of the event set and navigate to the
 *  ITypeInfo for events, among other things.
 */
static bool ObjectTypeInfo( LPUNKNOWN pObj, LPTYPEINFO* ppTypeInfo )
{
	assert( pObj != NULL );
	assert( ppTypeInfo != NULL );
	if( !pObj || !ppTypeInfo )
		return false;

	*ppTypeInfo=NULL;

	//Get the object's IProvideClassInfo and call the GetClassInfo
	//method therein.  This will give us back the ITypeInfo for
	//the entire object.
	CComPtr< IProvideClassInfo > pProviderClassInfo;
	HRESULT hr= pObj->QueryInterface( IID_IProvideClassInfo, reinterpret_cast< void** >(&pProviderClassInfo) );
	assert( SUCCEEDED(hr) );
	if( FAILED(hr) )
		return false;

	hr = pProviderClassInfo->GetClassInfo( ppTypeInfo );
	assert( SUCCEEDED(hr) );
	if( FAILED(hr) )
		return false;
	return true;
}

/*
 * ObjectTypeInfoEvents
 *
 * Purpose:
 *  Retrieves the events type information from an object.  This is
 *  defined to be the "default source" interface in the object's
 *  type library.
 */
static bool ObjectTypeInfoEvents( LPUNKNOWN pObj, LPTYPEINFO* ppInterfaceTypeInfo )
{
	assert( pObj != NULL );
	assert( ppInterfaceTypeInfo != NULL );
	if( !pObj || !ppInterfaceTypeInfo )
		return false;

	*ppInterfaceTypeInfo = NULL;

	CComPtr< ITypeInfo > pTypeInfo;
	if( !ObjectTypeInfo( pObj, &pTypeInfo ) )
		return false;

	//We have the object's overall ITypeInfo in pTypeInfo.
	//Now get the type attributes which will tell us the number of
	//individual interfaces in this type library.  We then loop
	//through the "implementation types" for all those interfaces
	//calling GetImplTypeFlags, looking for the default source.

	LPTYPEATTR pTypeAttr = NULL;
	HRESULT hr = pTypeInfo->GetTypeAttr( &pTypeAttr );
	assert( SUCCEEDED(hr) );
	if( FAILED(hr) )
		return false;
	UINT ctImplTypes = pTypeAttr->cImplTypes;
	pTypeInfo->ReleaseTypeAttr( pTypeAttr );

	//enumerate the interfaces to find the default source interface
	for( UINT idxImplType = 0; idxImplType < ctImplTypes; ++idxImplType )
	{
		INT fImplType = 0;
		hr = pTypeInfo->GetImplTypeFlags( idxImplType, &fImplType );
		assert( SUCCEEDED(hr) );
		if( FAILED(hr) )
			continue;

		if( (fImplType & IMPLTYPEFLAG_FDEFAULT) && (fImplType & IMPLTYPEFLAG_FSOURCE) )
		{
			//this is the interface we want
			HREFTYPE hRefType = NULL;
			hr = pTypeInfo->GetRefTypeOfImplType( idxImplType, &hRefType );
			assert( SUCCEEDED(hr) );
			if( FAILED(hr) )
				continue;
			hr = pTypeInfo->GetRefTypeInfo( hRefType, ppInterfaceTypeInfo );
			assert( SUCCEEDED(hr) );
			if( FAILED(hr) )
				continue;
			return true;
		}
	}
	return false;
}

/*
 * ObjectTypeInfoProperties
 *
 * Purpose:
 *  Retrieves the type property information from an object.
 */
static bool ObjectTypeInfoProperties( LPUNKNOWN pObj, LPTYPEINFO* ppAxTypeInfo )
{
	assert( pObj != NULL );
	assert( ppAxTypeInfo != NULL );
	if( !pObj || !ppAxTypeInfo )
		return false;

	*ppAxTypeInfo = NULL;

	CComPtr< ITypeInfo > pTypeInfo;
	if( !ObjectTypeInfo( pObj, &pTypeInfo ) )
	{
		// there is another way to obtain typeinfo for objects that support IDispatch
		CComPtr< IDispatch > pDispatch;
		HRESULT hr = pObj->QueryInterface( IID_IDispatch, reinterpret_cast< void** >(&pDispatch) );
		assert( SUCCEEDED(hr) );
		if( FAILED(hr) )
			return false;

		hr = pDispatch->GetTypeInfo( 0, ::GetUserDefaultLCID(), ppAxTypeInfo );
		assert( SUCCEEDED(hr) );
		if( FAILED(hr) )
			return false;
		return true;
	}

	LPTYPEATTR pTypeAttr = NULL;
	HRESULT hr = pTypeInfo->GetTypeAttr( &pTypeAttr );
	assert( SUCCEEDED(hr) );
	if( FAILED(hr) )
		return false;
	UINT ctImplTypes = pTypeAttr->cImplTypes;
	pTypeInfo->ReleaseTypeAttr( pTypeAttr );

	//enumerate the interfaces to find the default source interface
	for( UINT idxImplType = 0; idxImplType < ctImplTypes; ++idxImplType )
	{
		INT fImplType = 0;
		hr = pTypeInfo->GetImplTypeFlags( idxImplType, &fImplType );
		assert( SUCCEEDED(hr) );
		if( FAILED(hr) )
			continue;

		if( (fImplType & IMPLTYPEFLAG_FDEFAULT) && !(fImplType & IMPLTYPEFLAG_FSOURCE) )
		{
			//this is the interface we want
			HREFTYPE hRefType = NULL;
			hr = pTypeInfo->GetRefTypeOfImplType( idxImplType, &hRefType );
			assert( SUCCEEDED(hr) );
			if( FAILED(hr) )
				continue;
			hr = pTypeInfo->GetRefTypeInfo( hRefType, ppAxTypeInfo );
			assert( SUCCEEDED(hr) );
			if( FAILED(hr) )
				continue;
			return true;
		}
	}
	return false;
}


static void AddAxProp(TDclControlPtr pControl, long lId, CString sName, CString sDesc, VARTYPE vt, bool bGet=true, bool bSet=true)
{
	TPropertyPtr pProp = new CPropertyObject(pControl, PropActiveXProp);
	AxPropertyDescriptor* axPropPut =
		new AxPropertyDescriptor( lId, sName, sDesc, vt, bSet? INVOKE_PROPERTYPUT : INVOKE_PROPERTYGET );
	pControl->InsertNamedProperty( pProp );
}

static void SetupFont(TDclControlPtr pControl)
{
	AddAxProp(pControl, 0, _T("Name"), _T("Indicates the name of the font to be used."), VT_BSTR);
	AddAxProp(pControl, 2, _T("Size"), _T("Indicates the size of the font to be used. \\par \\par \\b1Note: \\b0 To properly calculate the font size, multiply your new font size by 10000."), VT_CY);
	AddAxProp(pControl, 3, _T("Bold"), _T("Indicates if the font is to be Bold."), VT_BOOL);
	AddAxProp(pControl, 4, _T("Italic"), _T("Indicates if the font is to be Italic."), VT_BOOL);
	AddAxProp(pControl, 5, _T("Underline"), _T("Indicates if the font is to be Underline."), VT_BOOL);
	AddAxProp(pControl, 6, _T("Strikethrough"), _T("Indicates if the font is to be Strikethrough."), VT_BOOL);
	AddAxProp(pControl, 7, _T("Weight"), _T("Indicates the weight of the font to be used."), VT_I2);
	AddAxProp(pControl, 8, _T("Charset"), _T("Indicates the charset of the font to be used."), VT_I2);	
}

static void SetupPicture(TDclControlPtr pControl)
{
	AddAxProp(pControl, 0, _T("Handle"), _T("Retrieve the picture Handle."), VT_I4, true, false);
	AddAxProp(pControl, 2, _T("HPal"), _T("Retrieve or set the Handle to the Palette."), VT_I4);
	AddAxProp(pControl, 3, _T("Type"), _T("Retrieve the picture type."), VT_I2, true, false);
	AddAxProp(pControl, 4, _T("Width"), _T("Retrieve the picture Width."), VT_I4, true, false);
	AddAxProp(pControl, 5, _T("Height"), _T("Retrieve the picture Height."), VT_I4, true, false);	
}


/////////////////////////////////////////////////////////////////////////////
// CAxContainerCtrl

CAxContainerCtrl::CAxContainerCtrl( TDclControlPtr pTemplate,
																		CControlPane* pPane,
																		UINT nID,
																		bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this)
, mpTypeLib( NULL )
, mnTypeLibCount( 0 )
{
	if (bCreate) {
		Create(pPane->GetHostDialog(), nID);
	}
}

CAxContainerCtrl::~CAxContainerCtrl()
{
}

bool CAxContainerCtrl::ParseTypeLibInfo()
{
	CComPtr< IOleObject > pOleObject;
	if( FAILED(GetOleObject( &pOleObject )) )
		return false;
	CComPtr< ITypeInfo > pTypeInfo;
	if( !ObjectTypeInfoProperties( pOleObject, &pTypeInfo ) )
		return false;
	if( FAILED(pTypeInfo->GetContainingTypeLib( &mpTypeLib, &mnTypeLibCount )) )
		return false;
	TDclControlPtr pDclControl = GetTemplate();
	ExtractPropertyInfo( pDclControl, pTypeInfo, pOleObject, true );
	ExtractMethodInfo( pDclControl, pTypeInfo );
	CComPtr< ITypeInfo > pTypeInfoEvents;
	if( !ObjectTypeInfoEvents( pOleObject, &pTypeInfoEvents ) )
		return false;
	ExtractEventInfo( pDclControl, pTypeInfoEvents, false );
	return true;
}

HRESULT CAxContainerCtrl::GetOleDispatch( IDispatch** ppDispatch )
{
	if( !mpDispatch )
	{
		IUnknown* pUnknown = GetControlUnknown();
		if( !pUnknown )
			return E_NOINTERFACE;
		HRESULT hr = pUnknown->QueryInterface( IID_IDispatch, (void**)&mpDispatch );
	}
	*ppDispatch = mpDispatch;
	(*ppDispatch)->AddRef();
	return S_OK;
}

HRESULT CAxContainerCtrl::GetOleObject( IOleObject** ppOleObject )
{	
	IUnknown* pUnknown = GetControlUnknown();
	if( !pUnknown )
		return E_NOINTERFACE;
	return pUnknown->QueryInterface( IID_IOleObject, (void**)ppOleObject );
}

bool CAxContainerCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	DWORD dwStyle = GetWndStyle();

	//setup license key (if used)
	CString sLicenseKey = GetTemplate()->GetAxCtrlLicenseKey();
	CComBSTR bstrLicenseKey;
	if( !sLicenseKey.IsEmpty() )
		bstrLicenseKey = sLicenseKey;

	COleStreamFile* pOleStreamFile = NULL;
	const CComPtr< IStream >& pStream = GetTemplate()->GetIStream();
	if( pStream )
		pOleStreamFile = new COleStreamFile( pStream );

	bool bSuccess = false;
	try
	{
		const CLSID& clsid = GetTemplate()->GetAxCtrlClsid();
		//Create the control, passing the OleStream and license key.
		bSuccess = (FALSE != CreateControl( clsid, NULL, dwStyle, GetWndRect(), pParentWnd, nID,
																				pOleStreamFile, FALSE, bstrLicenseKey ));
		if (!bSuccess && pOleStreamFile)
		{
			//Creation failed.
			//Try again, this time without the OleStream.
			bSuccess = (FALSE != CreateControl( clsid, NULL, dwStyle, GetWndRect(), pParentWnd, nID,
																					NULL, FALSE, bstrLicenseKey ));
		}
		if (!bSuccess && (BSTR)bstrLicenseKey)
		{
			//Creation failed.
			//Try again, this time without the OleStream and without the license key.
			bSuccess = (FALSE != CreateControl( clsid, NULL, dwStyle, GetWndRect(), pParentWnd, nID,
																					NULL, FALSE, NULL ));
		}
	}
	catch(...)
	{
		bSuccess = false;
	}

	if( pOleStreamFile )
		delete pOleStreamFile;

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

bool CAxContainerCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	//switch( pProp->GetID() )
	//{
	//case <property-id>:
	//	{
	//		break;
	//	}
	//}
	return !bFailed;
}

DWORD CAxContainerCtrl::GetWndStyle() const
{
	//ignoring the Prop::IsTabStop property? [ORW]
	return (WS_CHILD | WS_VISIBLE);
}

bool CAxContainerCtrl::SetProperty( TPropertyPtr pProp, LPCTSTR pszValue )
{
	assert( pProp != NULL ); //trying to set a non-existent custom property
	if( !pProp )
		return false;
	const AxInterfaceDescriptor* pAxID = pProp->GetConstAxInterfaceDescriptorPtr();
	assert( pAxID != NULL ); //no interface descriptor for this property
	if( !pAxID )
		return false;
	AxPropertyDescriptor* pPropPutDesc = pAxID->GetPutDescriptor();
	assert( pPropPutDesc != NULL ); //no 'put' method for this property
	if( !pPropPutDesc )
		return false;
	if( FAILED(SetProperty( pPropPutDesc, &COleVariant( pszValue ), 1 )) )
		return false;
	return true;
}

//[DPR] Recreated for methods_activex.cpp
HRESULT CAxContainerCtrl::GetProperty( const AxPropertyDescriptor* axProp, VARIANTARG* rvarArgs, UINT ctArgs, VARIANT& varResult )
{
	if( !axProp )
		return E_POINTER;
	CComPtr< IDispatch > pDispatch;
	HRESULT hr = GetOleDispatch( &pDispatch );
  if( FAILED(hr) )
		return hr;
	return axProp->Get( pDispatch, rvarArgs, ctArgs, varResult );
}

HRESULT CAxContainerCtrl::GetProperty( const AxPropertyDescriptor* axProp, CString &strReturnValue )
{
	if( !axProp )
		return E_POINTER;
	CComPtr< IDispatch > pDispatch;
	HRESULT hr = GetOleDispatch( &pDispatch );
  if( FAILED(hr) )
		return hr;
	VARIANT varResult;
	VariantClear( &varResult );
	hr = axProp->Get( pDispatch, NULL, 0, varResult );
  if( FAILED(hr) )
		return hr;
	if( varResult.vt == VT_BOOL )
		strReturnValue = (varResult.boolVal != VARIANT_FALSE? _T("True") : _T("False"));
	else
	{
		if( SUCCEEDED(VariantChangeType( &varResult, &varResult, 0, VT_BSTR )) )
			strReturnValue = CString( varResult.bstrVal );
		else
			strReturnValue.Empty();
	}
	return S_OK;
}

HRESULT CAxContainerCtrl::SetProperty( const AxPropertyDescriptor* axProp, const VARIANTARG* rvarArgs, UINT ctArgs )
{
	if( !axProp )
		return E_POINTER;
	CComPtr< IDispatch > pDispatch;
	HRESULT hr = GetOleDispatch( &pDispatch );
  if( FAILED(hr) )
		return hr;
	hr = axProp->Set( pDispatch, rvarArgs, ctArgs );
  if( FAILED(hr) )
		return hr;

	SaveToStream();
	return S_OK;
}

UINT CAxContainerCtrl::ExtractEventInfo(TDclControlPtr pControl, ITypeInfo* pTypeInfo, bool bUseAsType)
{
	assert( pTypeInfo != NULL );
	if( !pTypeInfo )
		return 0;

	UINT ctEvents = 0;

	TYPEATTR* pTypeAttr = NULL;
	HRESULT hr = pTypeInfo->GetTypeAttr( &pTypeAttr );
	assert( SUCCEEDED(hr) );
	if( FAILED(hr) )
		return 0;
	UINT ctVars = pTypeAttr->cVars;
	UINT ctFuncs = pTypeAttr->cFuncs;
	pTypeInfo->ReleaseTypeAttr( pTypeAttr );

	//Add a new property for each Get/Put function
	for( UINT idxFunc = 0; idxFunc < ctFuncs; ++idxFunc )
	{
		FUNCDESC* pFuncDesc = NULL;
		hr = pTypeInfo->GetFuncDesc( idxFunc, &pFuncDesc );
		assert( SUCCEEDED(hr) );
		if( FAILED(hr) )
			continue;
		std::auto_ptr< AxEventDescriptor > pAxEventDesc( new AxEventDescriptor( pFuncDesc, pTypeInfo, bUseAsType ) );
		pTypeInfo->ReleaseFuncDesc( pFuncDesc );

		CString sEventName = pAxEventDesc->GetName();
		if( !sEventName.IsEmpty() )
		{
			TPropertyList& Props = pControl->GetPropertyList();
			bool bFoundIt = false;
			for( TPropertyList::iterator iter = Props.begin(); iter != Props.end(); ++iter )
			{
				if( (*iter)->GetType() == PropActiveXEvent )
				{
					const AxEventDescriptor* pAxEvent = (*iter)->GetConstAxInterfaceDescriptorPtr()->GetEvent();
					if( pAxEvent->GetName() == sEventName )
					{
						bFoundIt = true; //event already exists
						break;
					}
				}
			}
			if( !bFoundIt ) //event must be added to property list
			{
				TPropertyPtr pProp = new CPropertyObject( pControl, PropActiveXEvent );
				pProp->SetHidden();
				Props.push_back( pProp );
				++ctEvents;
				pProp->GetAxInterfaceDescriptorPtr()->SetEvent( pAxEventDesc.release() );
			}
		}
		pTypeInfo->ReleaseFuncDesc( pFuncDesc );
	}
	return ctEvents;
}

UINT CAxContainerCtrl::ExtractMethodInfo(TDclControlPtr pControl, ITypeInfo* pTypeInfo)
{
	assert( pTypeInfo != NULL );
	if( !pTypeInfo )
		return 0;

	UINT ctMethods = 0;

	TYPEATTR* pTypeAttr = NULL;
	HRESULT hr = pTypeInfo->GetTypeAttr( &pTypeAttr );
	assert( SUCCEEDED(hr) );
	if( FAILED(hr) )
		return 0;
	UINT ctVars = pTypeAttr->cVars;
	UINT ctFuncs = pTypeAttr->cFuncs;
	pTypeInfo->ReleaseTypeAttr( pTypeAttr );

	//Remove any existing ActiveX methods properties before inserting the new ones
	TPropertyList& Props = pControl->GetPropertyList();
	TPropertyList::iterator iter = Props.begin();
	while( iter != Props.end() )
	{
		TPropertyList::iterator iterAt = iter++;
		if( (*iterAt)->GetType() == PropActiveXMethods )
			Props.erase( iterAt );
	}

	// create the new property and insert at the second position after the (ActiveX PropertyObject) property item
	TPropertyPtr pProp = new CPropertyObject(pControl, PropActiveXMethods);
	pProp->GetAxInterfaceDescriptorPtr()->SetMethods( new std::vector< RefCountedPtr< AxMethodDescriptor > > );
	pProp->SetID( Prop::ControlBrowser ); // set the id of the new property
	pControl->InsertNamedProperty( pProp );

	//Add the methods
	for( UINT idxFunc = 0; idxFunc < ctFuncs; ++idxFunc )
	{
		FUNCDESC* pFuncDesc = NULL;
		hr = pTypeInfo->GetFuncDesc( idxFunc, &pFuncDesc );
		assert( SUCCEEDED(hr) );
		if( FAILED(hr) )
			continue;
		if( pFuncDesc->invkind == DISPATCH_METHOD && ((LONG)pFuncDesc->memid < (LONG)(1<<16)) )
		{
			std::auto_ptr< AxMethodDescriptor > pAxMethodDesc( new AxMethodDescriptor( pFuncDesc, pTypeInfo ) );
			if( !pAxMethodDesc->GetName().IsEmpty() )
			{
				pProp->GetConstAxInterfaceDescriptorPtr()->GetMethods()->push_back( pAxMethodDesc.release() );
				++ctMethods;
			}
		}
		pTypeInfo->ReleaseFuncDesc( pFuncDesc );
	}
	return ctMethods;
}

UINT CAxContainerCtrl::ExtractPropertyInfo( TDclControlPtr pControl, ITypeInfo* pTypeInfo, LPOLEOBJECT pIObject, bool bEnumList /*=false*/ )
{
	assert( pTypeInfo != NULL );
	if( !pTypeInfo )
		return 0;

	UINT ctProperties = 0;

	TYPEATTR* pTypeAttr = NULL;
	HRESULT hr = pTypeInfo->GetTypeAttr( &pTypeAttr );
	assert( SUCCEEDED(hr) );
	if( FAILED(hr) )
		return 0;
	UINT ctVars = pTypeAttr->cVars;
	UINT ctFuncs = pTypeAttr->cFuncs;
	pTypeInfo->ReleaseTypeAttr( pTypeAttr );

	if( bEnumList )
	{
		//Add a new property for each "Var"
		for( UINT idxVar = 0; idxVar < ctVars; ++idxVar )
		{
			VARDESC* pVarDesc = NULL;
			hr = pTypeInfo->GetVarDesc( idxVar, &pVarDesc );
			assert( SUCCEEDED(hr) );
			if( FAILED(hr) )
				continue;
			std::auto_ptr< AxPropertyDescriptor > pAxPropDesc( new AxPropertyDescriptor( pVarDesc, pTypeInfo ) );
			pTypeInfo->ReleaseVarDesc( pVarDesc );

			if( !pAxPropDesc->GetName().IsEmpty() )
			{
				UINT nDecorator = 2;
				CString sBaseName = pAxPropDesc->GetName();
				CString sPropName = sBaseName;
				TPropertyPtr pProp = pControl->FindPropertyObject( sPropName );
				while( pProp && !pProp->GetConstAxInterfaceDescriptorPtr() )
				{ //found a different type of property with the same name, so decorate the name
					sPropName.Format( _T("%s%d"), (LPCTSTR)sBaseName, nDecorator++ );
					pProp = pControl->FindPropertyObject( sPropName );
				}
				if( !pProp )
				{ //add a new property object
					pProp = new CPropertyObject( pControl, PropActiveXEnum );
					pProp->SetHidden();
					pProp->SetName( sPropName );
					pControl->InsertNamedProperty( pProp );
				}
				pProp->GetAxInterfaceDescriptorPtr()->SetProp( pAxPropDesc.release() );
				++ctProperties;
			}
		}
	}

	//Add a new property for each Get/Put function
	for( UINT idxFunc = 0; idxFunc < ctFuncs; ++idxFunc )
	{
		FUNCDESC* pFuncDesc = NULL;
		hr = pTypeInfo->GetFuncDesc( idxFunc, &pFuncDesc );
		assert( SUCCEEDED(hr) );
		if( FAILED(hr) )
			continue;
		if( (pFuncDesc->invkind == INVOKE_PROPERTYGET ||
				 pFuncDesc->invkind == INVOKE_PROPERTYPUT ||
				 pFuncDesc->invkind == INVOKE_PROPERTYPUTREF) &&
				(pFuncDesc->wFuncFlags & FUNCFLAG_FHIDDEN) == 0 )
		{
			std::auto_ptr< AxPropertyDescriptor > pAxPropDesc( new AxPropertyDescriptor( pFuncDesc, pTypeInfo, pIObject ) );
			bool bBrowsable = !(pFuncDesc->wFuncFlags & FUNCFLAG_FNONBROWSABLE);

			if( !pAxPropDesc->GetName().IsEmpty() )
			{
				// find an instance of the property with the name of the one we have.
				// we do this so that the CPropertyObject will hold all INVOKE_PROPERTYGET, 
				// INVOKE_PROPERTYPUT and INVOKE_PROPERTYPUTREF ActiveX properties of the same 
				// name inside one CPropertyObject object.

				UINT nDecorator = 2;
				CString sBaseName = pAxPropDesc->GetName();
				CString sPropName = sBaseName;
				TPropertyPtr pProp = pControl->FindPropertyObject( sPropName );
				while( pProp && !pProp->GetConstAxInterfaceDescriptorPtr() )
				{ //found a different type of property with the same name, so decorate the name
					sPropName.Format( _T("%s%d"), (LPCTSTR)sBaseName, nDecorator++ );
					pProp = pControl->FindPropertyObject( sPropName );
				}
				if( !pProp )
				{ //add a new property object
					if( !bBrowsable )
					{
						pProp = new CPropertyObject( pControl, PropActiveXRunTime );
						pProp->SetHidden();
					}
					else
						pProp = new CPropertyObject( pControl, PropActiveXProp );
					pProp->SetName( sPropName );
					pControl->InsertNamedProperty( pProp );
				}
				switch( pAxPropDesc->GetInvKind() )
				{
				case INVOKE_PROPERTYGET:
					pProp->GetAxInterfaceDescriptorPtr()->SetPropGet( pAxPropDesc.release() );
					break;
				case INVOKE_PROPERTYPUT:
					pProp->GetAxInterfaceDescriptorPtr()->SetPropPut( pAxPropDesc.release() );
					break;
				case INVOKE_PROPERTYPUTREF:
					pProp->GetAxInterfaceDescriptorPtr()->SetPropPutRef( pAxPropDesc.release() );
					break;
				default:
					if( !pProp->GetConstAxInterfaceDescriptorPtr()->GetProp() )
						pProp->GetAxInterfaceDescriptorPtr()->SetProp( pAxPropDesc.release() );
					break;
				}
				++ctProperties;
			}
		}
		pTypeInfo->ReleaseFuncDesc( pFuncDesc );
	}
	return ctProperties;
}

bool CAxContainerCtrl::ExtractComponentsFromTLB( TDclControlPtr pDclControl, CLSID clsid )
{
	if( !mpTypeLib )
		return false;
	long lTypeInfoCount = mpTypeLib->GetTypeInfoCount();
	if( lTypeInfoCount == 0 )
		return false; //no type information

	//Get the help string and help file for each TypeInfo
	for( long lIter = 0; lIter < lTypeInfoCount ; lIter++ )
	{
		ITypeInfo *TheInfo = NULL;
		TYPEATTR *TheAttr;
		mpTypeLib->GetTypeInfo( lIter, &TheInfo );
		if( TheInfo != NULL )
		{
			TheInfo->GetTypeAttr( &TheAttr );
			if( TheAttr && clsid == TheAttr->guid )
			{
				ExtractPropertyInfo( pDclControl, TheInfo, NULL, true );
				ExtractMethodInfo( pDclControl, TheInfo );	
				CComBSTR bstrDoc;
				CComBSTR bstrName;
				mpTypeLib->GetDocumentation( lIter, &bstrName, &bstrDoc, NULL, NULL );
				if( bstrName.Length() > 0 )
					pDclControl->SetAxTypeName( CString( bstrName ) );
			}
			TheInfo->ReleaseTypeAttr( TheAttr );
			TheInfo->Release();
		}
	}               
	return true;
}

bool CAxContainerCtrl::GetPropertyPageCLSIDs( CArray< CLSID, CLSID& >& aclsidPages )
{

	CComPtr< IOleObject > pOleObject;
	if( FAILED(GetOleObject( &pOleObject )) )
		return false;

	CComPtr< ISpecifyPropertyPages > pSpecify;
	HRESULT hr = pOleObject->QueryInterface( &pSpecify );
	if( FAILED(hr) )
		return( false );
   
	CAUUID pages = { 0, NULL };
	hr = pSpecify->GetPages( &pages );
	if( FAILED(hr) )
		return( false );

	for( ULONG iPage = 0; iPage < pages.cElems; iPage++ )
		aclsidPages.Add( pages.pElems[iPage] );

	CoTaskMemFree( pages.pElems );
	
	return true;
}

HRESULT CAxContainerCtrl::SaveToStream()
{
	CComPtr< IOleObject > pOleObject;
	HRESULT hr = GetOleObject( &pOleObject );
	if( FAILED(hr) )
		return hr;

	CComPtr< IPersist > pPersist;
	hr = pOleObject->QueryInterface( &pPersist );
	if( FAILED(hr) )
	{
	  // Some control implementers forget that the IPersist* interfaces aren't all
	  // derived from IPersist.  If they forget to allow a QI for IPersist, let
	  // them know the error of their ways, and just go for IPersistStorage to
	  // get the IPersist interface.
	  hr = pOleObject->QueryInterface( IID_IPersistStorage, (void**)&pPersist );
	  if( FAILED(hr) )
			return hr;
	}

	CComPtr< IStream >& pStream = GetTemplate()->GetIStream();

	CLSID clsid;
	hr = pPersist->GetClassID( &clsid );
  if( SUCCEEDED(hr) )
		hr = WriteClassStm( pStream, clsid );
  if( FAILED(hr) )
		return hr;

	CComPtr< IPersistStream > pPersistStream;
	hr = pOleObject->QueryInterface( &pPersistStream );
	if( SUCCEEDED(hr) )
		hr = pPersistStream->Save( pStream, TRUE );	  
	else
	{
		CComPtr< IPersistStreamInit > pPersistStreamInit;
	  hr = pOleObject->QueryInterface( &pPersistStreamInit );
		if( FAILED(hr) )
			return hr;
	  hr = pPersistStreamInit->Save( pStream, TRUE );
	}
	if( FAILED(hr) )
		return hr;

	theWorkspace.SetModified(true);

	return( S_OK );
}

HRESULT CAxContainerCtrl::Invoke( AxMethodDescriptor* axMethod, const VARIANTARG* rvarArgs, UINT ctArgs, VARIANT& varResult )
{
	CComPtr< IDispatch > pDispatch;
	HRESULT hr = GetOleDispatch( &pDispatch );
	if( FAILED(hr) )
		return hr;
	return axMethod->Invoke( pDispatch, rvarArgs, ctArgs, varResult );
}


/////////////////////////////////////////////////////////////////////////////
// CAxContainerCtrl message handlers

BOOL CAxContainerCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

BOOL CAxContainerCtrl::DestroyWindow()
{
	mpDispatch = NULL;
	mpTypeLib = NULL;
	__super::DestroyWindow();
	delete this;
	return TRUE;
}
