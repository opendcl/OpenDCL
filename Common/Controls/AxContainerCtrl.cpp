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
 *
 * Parameters:
 *  pObj            LPUNKNOWN of the object
 *  ppITypeInfo     LPTYPEINFO * in which to return the ITypeInfo
 *                  interface for the object's events.
 *
 * Return Value:
 *  BOOL            TRUE if successful, FALSE otherwise.
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
 *
 * Parameters:
 *  pObj            LPUNKNOWN of the object
 *  ppITypeInfo     LPTYPEINFO * in which to return the ITypeInfo
 *                  interface for the object's events.
 *
 * Return Value:
 *  BOOL            TRUE if the event type lib exists, FALSE
 *                  if not or on any other error.
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
 *
 * Parameters:
 *  pObj            LPUNKNOWN of the object
 *  ppTypeInfo      LPTYPEINFO * in which to return the ITypeInfo
 *                  interface for the object's events.
 *
 * Return Value:
 *  BOOL            TRUE if the event type lib exists, FALSE
 *                  if not or on any other error.
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
	ExtractPropertyInfo( GetTemplate(), pOleObject, TRUE );
	ExtractMethodInfo( GetTemplate(), pOleObject);
	ExtractEventInfo( GetTemplate(), pOleObject, FALSE );
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
	CComBSTR bstrLicenseKey;
	if( !GetTemplate()->m_sLicenseKey.IsEmpty() )
		bstrLicenseKey = GetTemplate()->m_sLicenseKey;

	COleStreamFile *pOleStreamFile = NULL;
	if (GetTemplate()->m_pStream != NULL)
		pOleStreamFile = new COleStreamFile(GetTemplate()->GetLoadStream());

	bool bSuccess = false;
	try
	{		
		//Create the control, passing the OleStream and license key.
		bSuccess = (FALSE != CreateControl( GetTemplate()->m_clsid, NULL, dwStyle, GetWndRect(), pParentWnd, nID,
																				pOleStreamFile, FALSE, bstrLicenseKey ));
		if (!bSuccess && pOleStreamFile)
		{
			//Creation failed.
			//Try again, this time without the OleStream.
			bSuccess = (FALSE != CreateControl( GetTemplate()->m_clsid, NULL, dwStyle, GetWndRect(), pParentWnd, nID,
																					NULL, FALSE, bstrLicenseKey ));
		}
		if (!bSuccess && (BSTR)bstrLicenseKey)
		{
			//Creation failed.
			//Try again, this time without the OleStream and without the license key.
			bSuccess = (FALSE != CreateControl( GetTemplate()->m_clsid, NULL, dwStyle, GetWndRect(), pParentWnd, nID,
																					NULL, FALSE, NULL ));
		}
	}
	catch(...)
	{
		bSuccess = false;
	}

	if (pOleStreamFile != NULL)
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
	if( FAILED(SetProperty( pPropPutDesc, pszValue )) )
		return false;
	return true;
}

//[DPR] Recreated for methods_activex.cpp
HRESULT CAxContainerCtrl::GetProperty( AxPropertyDescriptor* axProp, VARIANTARG* rvarArgs, UINT ctArgs, VARIANT& varResult )
{
	if( !axProp )
		return E_POINTER;
	CComPtr< IDispatch > pDispatch;
	HRESULT hr = GetOleDispatch( &pDispatch );
  if( FAILED(hr) )
		return hr;
	return axProp->Get( pDispatch, rvarArgs, ctArgs, varResult );
}

HRESULT CAxContainerCtrl::GetProperty(AxPropertyDescriptor* axProp, CString &strReturnValue)
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

HRESULT CAxContainerCtrl::SetProperty( AxPropertyDescriptor* axProp, const VARIANTARG* rvarArgs, UINT ctArgs )
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

	GetTemplate()->SaveToStream(this);
	return S_OK;
}

HRESULT CAxContainerCtrl::SetProperty( AxPropertyDescriptor* axProp, COleVariant varArg )
{
	return SetProperty( axProp, &varArg, 1 );
}

IDispatch * CAxContainerCtrl::GetChildIDispatch(DISPID dispid)
{
	LPDISPATCH pDispatch;
	InvokeHelper(dispid, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&pDispatch, NULL);
	return pDispatch;
}

COleFont CAxContainerCtrl::GetFont(DISPID dispid)
{
	LPDISPATCH pDispatch;
	InvokeHelper(dispid, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&pDispatch, NULL);
	return COleFont(pDispatch);
}

void CAxContainerCtrl::SetFont(DISPID dispid, LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(dispid, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
	
	GetTemplate()->SaveToStream(this);
}

void CAxContainerCtrl::SetPicture(DISPID dispid, LPDISPATCH newValue, WORD flag)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(dispid, flag, VT_EMPTY, NULL, parms,
		 newValue);

	GetTemplate()->SaveToStream(this);
}

unsigned long CAxContainerCtrl::GetColor(DISPID dispid)
{
	unsigned long result;
	InvokeHelper(dispid, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void CAxContainerCtrl::SetColor(DISPID dispid, unsigned long propVal)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(dispid, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 propVal);

	GetTemplate()->SaveToStream(this);	
}

void CAxContainerCtrl::LoadPictureFile(DISPID dispid, CString sFile, WORD flag)
{
	LPPICTURE		lpPicture;
	lpPicture		= NULL;
	
	if (sFile.IsEmpty())
		return;

	// open file
	HANDLE hFile = CreateFile(sFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	_ASSERTE(INVALID_HANDLE_VALUE != hFile);

	// get file size
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	_ASSERTE(-1 != dwFileSize);

	LPVOID pvData = NULL;
	// alloc memory based on file size
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
	_ASSERTE(NULL != hGlobal);

	pvData = GlobalLock(hGlobal);
	_ASSERTE(NULL != pvData);

	DWORD dwBytesRead = 0;
	// read file and store in global memory
	BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
	_ASSERTE(FALSE != bRead);
	GlobalUnlock(hGlobal);
	CloseHandle(hFile);

	LPSTREAM pstm = NULL;
	// create IStream* from global memory
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
	_ASSERTE(SUCCEEDED(hr) && pstm);
	
	// Create IPicture from image file
	if (lpPicture)
		lpPicture->Release();
	hr = ::OleLoadPicture(pstm, dwFileSize, FALSE, IID_IPicture, (LPVOID *)&lpPicture);
	_ASSERTE(SUCCEEDED(hr) && lpPicture);	

	CPictureHolder phPicture;
	IPicture *ipOld = phPicture.m_pPict;
	phPicture.m_pPict = lpPicture;

	SetPicture(dispid, phPicture.GetPictureDispatch(), flag);
	
	phPicture.m_pPict = ipOld;
	
	pstm->Release();
	Invalidate();
}

UINT CAxContainerCtrl::ExtractEventInfo(TDclControlPtr pControl, LPOLEOBJECT pIObject, bool bUseAsType)
{
	ITypeInfo *TheInfo = NULL;
	ObjectTypeInfoEvents(pIObject,&TheInfo);
	return ExtractEventInfo(pControl, TheInfo, bUseAsType);
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

		if( !pAxEventDesc->GetName().IsEmpty() )
		{
			TPropertyList& Props = pControl->GetPropertyList();
			bool bFoundIt = false;
			for( TPropertyList::iterator iter = Props.begin(); iter != Props.end(); ++iter )
			{
				if( (*iter)->GetType() == PropActiveXEvent )
				{
					const AxEventDescriptor* pAxEvent = (*iter)->GetConstAxInterfaceDescriptorPtr()->GetEvent();
					if( pAxEvent->GetName() == pAxEventDesc->GetName() )
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

UINT CAxContainerCtrl::ExtractMethodInfo(TDclControlPtr pControl, LPOLEOBJECT pIObject)
{
	ITypeInfo *TheInfo = NULL;
	ObjectTypeInfoProperties(pIObject,&TheInfo);
	return ExtractMethodInfo(pControl, TheInfo);
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
	TPropertyList& Props = mpTemplate->GetPropertyList();
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
	pProp->SetID( Prop::ObjectBrowser ); // set the id of the new property
	pControl->InsertNamedProperty( pProp );

	//Add a new property for each Get/Put function
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
			pTypeInfo->ReleaseFuncDesc( pFuncDesc );

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

BOOL CAxContainerCtrl::ExtractComponentsFromTLB(TOleControlPtr pOleControl, CLSID clsid)
{
	long lTypeInfoCount = 0;
	CComBSTR bstrName;
	BOOL bSuccess = TRUE;


	if (mpTypeLib == NULL)
		return FALSE;
	
	//Get the type information count
	lTypeInfoCount = mpTypeLib->GetTypeInfoCount();


	//Make sure that we have type information
	if(lTypeInfoCount == 0)
	{
		bSuccess = FALSE;
	}

	TDclControlLockedPtr pControl = &(*pOleControl);
	
	if (clsid == IID_IFont)
	{
		pControl->SetAxTypeName( _T("Font") );
		SetupFont(pControl);
		return TRUE;
	}
	if (clsid == IID_IFontDisp)
	{
		pControl->SetAxTypeName( _T("Font") );
		SetupFont(pControl);
		return TRUE;
	}
	if (clsid == IID_IPictureDisp)
	{
		pControl->SetAxTypeName( _T("Picture") );
		SetupPicture(pControl);
		return TRUE;
	}

	if(bSuccess)
	{
		
		//Get the help string and help file for each TypeInfo
		for(long lIter = 0; lIter  < lTypeInfoCount ; lIter++)
		{
			ITypeInfo *TheInfo = NULL;
			TYPEATTR *TheAttr;
			
			mpTypeLib->GetTypeInfo(lIter, &TheInfo);
			if (TheInfo != NULL)
			{
				TheInfo->GetTypeAttr(&TheAttr);
				
				if (clsid == TheAttr->guid)
				{
					ExtractPropertyInfo(pControl, TheInfo, NULL, true);
					ExtractMethodInfo(pControl, TheInfo);	
					
					CComBSTR bstrDoc;
					mpTypeLib->GetDocumentation(lIter, &bstrName, &bstrDoc, NULL, NULL);
					if(bstrName)
						pControl->SetAxTypeName( CString(bstrName) );
				}				
			}			
		}               
	}
	return TRUE;
}

UINT CAxContainerCtrl::ExtractPropertyInfo( TDclControlPtr pControl, LPOLEOBJECT pIObject, bool bEnumList /*=false*/ )
{
	CComPtr< ITypeInfo > pTypeInfo;
	ObjectTypeInfoProperties( pIObject, &pTypeInfo );

	// extract the containing type library and save it
	if( FAILED(pTypeInfo->GetContainingTypeLib( &mpTypeLib, &mnTypeLibCount )) )
		return 0;
	return ExtractPropertyInfo( pControl, pTypeInfo, pIObject, bEnumList );
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
					pProp->SetStringValue( sPropName );
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
				pFuncDesc->wFuncFlags != FUNCFLAG_FHIDDEN )
		{
			std::auto_ptr< AxPropertyDescriptor > pAxPropDesc( new AxPropertyDescriptor( pFuncDesc, pTypeInfo, this, pIObject ) );
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
					if( pAxPropDesc->IsReadOnly() || !bBrowsable )
					{
						pProp = new CPropertyObject( pControl, PropActiveXRunTime );
						pProp->SetHidden();
					}
					else
						pProp = new CPropertyObject( pControl, PropActiveXProp );
					pProp->SetStringValue( sPropName );
					if( pAxPropDesc->GetInvKind() == INVOKE_PROPERTYGET && !pAxPropDesc->GetArgs().empty() )
						pProp->SetHidden();
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

void CAxContainerCtrl::LoadPicture(DISPID dispid, int nId)
{
	//[DPR] To convert to Ctrl format, I removed mpParent from this class.
	//This is the only place it was used.
	//In its place, I get the current project from the workspace.
	//I assume that this will work correctly but have not tested it.
	if (nId == -1)
	{
		SetPicture(dispid, NULL, DISPATCH_PROPERTYPUT);
		Invalidate();
		return;
	}

	const CPictureObject* pPicture = mpTemplate->GetOwnerProject()->FindPicture(nId);
	if (pPicture)
	{
		IDispatch *pPicDisp = pPicture->GetPictureDisp();
		if (pPicDisp != NULL)
		{
			SetPicture(dispid, pPicDisp, DISPATCH_PROPERTYPUT);
			Invalidate();
		}
	}
}

BOOL CAxContainerCtrl::GetPropertyPageCLSIDs( CArray< CLSID,
   CLSID& >& aclsidPages )
{
   CAUUID pages;
   ULONG iPage;

	CComPtr< IOleObject > pOleObject;
	if( FAILED(GetOleObject( &pOleObject )) )
		return FALSE;

	CComPtr< ISpecifyPropertyPages > pSpecify;
	HRESULT hr = pOleObject->QueryInterface( &pSpecify ); 
	if( FAILED(hr) )
		return( FALSE );
   
	pages.cElems = 0;
	pages.pElems = NULL;
	hr = pSpecify->GetPages( &pages );
	if( FAILED(hr) )
		return( FALSE );

	for( iPage = 0; iPage < pages.cElems; iPage++ )
		aclsidPages.Add( pages.pElems[iPage] );

	CoTaskMemFree( pages.pElems );
	
	return( TRUE );
}

void CAxContainerCtrl::ShowPropertyPages()
{
	CArray< CLSID, CLSID& > aclsidCommonPages;
	CArray< CLSID, CLSID& > aclsidPages;
	BOOL tSuccess;
	int iPage;
	INT_PTR iCommonPage;
	BOOL tFound;
	CLSID clsidPage;

	tSuccess = GetPropertyPageCLSIDs( aclsidCommonPages );
	if( !tSuccess )
		return;

	aclsidPages.RemoveAll();
	tSuccess = GetPropertyPageCLSIDs( aclsidPages );
	if( !tSuccess )
		return;

	for( iCommonPage = aclsidCommonPages.GetSize()-1; iCommonPage >= 0;	iCommonPage-- )
	{
		clsidPage = aclsidCommonPages[iCommonPage];
		tFound = FALSE;
		for( iPage = 0; iPage < aclsidPages.GetSize(); iPage++ )
		{
			if( IsEqualCLSID( clsidPage, aclsidPages[iPage] ) )
			{
				tFound = TRUE;
			}
		}
		if( !tFound )
		{
			if (iCommonPage!= NULL)
				aclsidCommonPages.RemoveAt( iCommonPage );
		}
	}

	if( aclsidCommonPages.GetSize() > 0 )
	{
		CWnd* pFocusWnd = GetFocus();
		CComPtr< IDispatch > pDispatch;
		HRESULT hr = GetOleDispatch( &pDispatch );
		assert( SUCCEEDED(hr) );
	  CLSID* pclsidCommonPages = (CLSID*)_alloca( aclsidCommonPages.GetSize()*sizeof(CLSID));
	  for( iPage = 0; iPage < aclsidCommonPages.GetSize(); iPage++ )
			pclsidCommonPages[iPage] = aclsidCommonPages[iPage];
	  OleCreatePropertyFrame( m_hWnd, 0, 0, bstr_t( GetTemplate()->GetKeyName() ),
														1, (IUnknown**)&pDispatch.p, (ULONG)aclsidCommonPages.GetSize(),
														pclsidCommonPages, GetUserDefaultLCID(), 0, NULL );

	  //WINBUG: OleCreatePropertyFrame doesn't return focus to its parent when
	  // it closes
		if( pFocusWnd )
			pFocusWnd->SetFocus();
		else
			SetFocus();
	}
   
	GetTemplate()->SaveToStream(this);
}

HRESULT CAxContainerCtrl::SaveToStream( IStream* pStream )
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

void CAxContainerCtrl::SetRefImageList(DISPID dispid, LPDISPATCH newValue)
{
	static BYTE parms[] = VTS_DISPATCH;
	InvokeHelper(dispid, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, newValue);
}

void CAxContainerCtrl::SetImageList(DISPID dispid, LPDISPATCH newValue)
{
	static BYTE parms[] = VTS_DISPATCH;
	InvokeHelper(dispid, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
}

HRESULT CAxContainerCtrl::Invoke( AxMethodDescriptor* axMethod, VARIANTARG* rvarArgs, UINT ctArgs, VARIANT& varResult )
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
	__super::DestroyWindow();
	delete this;
	return true;
}
