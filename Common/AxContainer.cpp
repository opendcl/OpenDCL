// AxContainer.cpp : implementation file
//

#include "stdafx.h"
#include "AxContainer.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxMethodDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "OleLicenseKey.h"
#include "OleFont.h"
#include "PictureObject.h"
#include "Project.h"
#include "Workspace.h"
#include "VarUtils.h"
#include "PropertyIds.h"
#include "SharedRes.h"
#include "ToolTips.h"

static const int MAX_CALLING_ARGUMENTS = 16;


static CString BuildParam(VARTYPE vt, BOOL bUseAsType, BOOL bNotByVal,
				   LPCTSTR szName, BOOL optional, LPVOID pFunc /*=NULL*/) 
{
	CString sDecl(_T(""));
	CString sType = VARTYPEtoString(vt);
	if (sType.IsEmpty())
		return _T("");

	return sDecl;
}


static void SetRefType(VARTYPE &vtType, ITypeInfo* TheInfo, HREFTYPE hreftype, CLSID *pClsid = NULL)
{
	ITypeInfo *TheRefType = NULL;
	if ((hreftype != -1) &&
		SUCCEEDED(TheInfo->GetRefTypeInfo(hreftype, &TheRefType)))
	{
		LPTYPEATTR pTA;
		if (SUCCEEDED(TheRefType->GetTypeAttr(&pTA)))
		{
			switch (pTA->typekind)
			{
			case TKIND_ALIAS: // color, etc.
				if ((pTA->tdescAlias.vt == VT_USERDEFINED) && (hreftype < VT_USERDEFINED))
					vtType = (VARTYPE) hreftype;
				else
					vtType = pTA->tdescAlias.vt;

				if (vtType == VT_USERDEFINED)
					SetRefType(vtType, TheRefType, 
						(pTA->tdescAlias.vt == VT_PTR) ? 
							pTA->tdescAlias.lptdesc->hreftype : pTA->tdescAlias.hreftype, pClsid);
				break;
			case TKIND_DISPATCH: // font, etc.
				vtType = VT_DISPATCH;
				if (pClsid != NULL)
					*pClsid = pTA->guid;
				break;
			case TKIND_ENUM: {
				vtType = VT_I4; // better than nothing
				VARDESC *pVarDesc;
				if (pTA->cVars > 0) { // try to determine real type
					if(SUCCEEDED(TheRefType->GetVarDesc(0, &pVarDesc ))) {
#ifdef MAINWIN
						vtType = pVarDesc->lpvarValue->n2.vt;
#else
						vtType = pVarDesc->lpvarValue->vt;
#endif
						TheRefType->ReleaseVarDesc(pVarDesc);
					}
				}
				} break;
			}
		}
		TheRefType->Release();
	}
}


// function type to return script language dependent strings
typedef CString (*TFScriptSymSet)(UINT nID);
#define SCRTYPE_COMMA 8
#define SCRTYPE_LPAREN 9
#define SCRTYPE_RPAREN 10

static CString FuncDescToString(ITypeInfo *lpTypeInfo, FUNCDESC *pFuncDesc,
						 BOOL bUseAsType)
{
    ASSERT(lpTypeInfo);
	BOOL bError = FALSE;
	
	short cParams = pFuncDesc->cParams ;
	short cParamsOpt = (short)abs(pFuncDesc->cParamsOpt) ;

	// Get the names of the function and it's parameters into rgbstrNames.
	// cNames gets the number of parameters + 1.
	//
    BSTR rgbstrNames[MAX_CALLING_ARGUMENTS] ;
    int cNames ;
	HRESULT hr = lpTypeInfo->GetNames(pFuncDesc->memid,
		rgbstrNames, MAX_CALLING_ARGUMENTS, (UINT FAR*) &cNames);
	if (SUCCEEDED( hr ))
	{
	    // For each required parameter
	    //
		CString lpsz;
#ifdef _WIN32
		char szBuf[256];    // NUU
#endif
    int n;	    
    for (n = 0 ; n < cParams - cParamsOpt ; n++ )
	    {
	        if (n+1 < cNames)
			{
#ifdef _WIN32
				WideCharToMultiByte(0,0,rgbstrNames[n+1],-1,
					szBuf,256,NULL,NULL);
				lpsz = (LPCSTR) szBuf;
#else
	            lpsz = rgbstrNames[n+1] ;
#endif
			}
	        else
	            lpsz.Empty();
	        
			// HRS
			const ELEMDESC& e = pFuncDesc->lprgelemdescParam[n];
			BOOL bNotByVal = (e.tdesc.vt == VT_PTR);
			VARTYPE vt = (bNotByVal ? e.tdesc.lptdesc->vt : e.tdesc.vt);
			if (vt == VT_USERDEFINED) 
				SetRefType(vt, lpTypeInfo,
					(bNotByVal) ? e.tdesc.lptdesc->hreftype : e.tdesc.hreftype);
			CString sDecl = BuildParam(vt, bUseAsType, bNotByVal, lpsz, FALSE, NULL);
	        if (sDecl.IsEmpty()) bError = TRUE;
			//sParams += sDecl;
        
	        //if (n+1 < cNames)
	          //  SysFreeString( rgbstrNames[n+1] ) ;
            
	        
	    }
    
	    // For each optional parameter
	    //
	    for (n = cParams - cParamsOpt ; n < cParams ; n++)
	    {
	        if (n+1 < cNames) 
			{
#ifdef _WIN32
				WideCharToMultiByte(0,0,rgbstrNames[n+1],-1,
					szBuf,256,NULL,NULL);
				lpsz = (LPCSTR) szBuf;
#else
	            lpsz = rgbstrNames[n+1] ;
#endif
			}
	        else
	            lpsz.Empty();
        
			// HRS
			const ELEMDESC& e = pFuncDesc->lprgelemdescParam[n];
			BOOL bNotByVal = (e.tdesc.vt == VT_PTR);
			VARTYPE vt = (bNotByVal ? e.tdesc.lptdesc->vt : e.tdesc.vt);
			CString sDecl = BuildParam(vt, bUseAsType, bNotByVal, lpsz, TRUE, NULL);

			// bError = TRUE; // optional parameters not supported
	        if (sDecl.IsEmpty()) bError = TRUE;
			//sParams += sDecl;

			//if (n+1 < cNames)
	          //  SysFreeString( rgbstrNames[n+1] ) ;
	    }
	}
	return _T("");
}


void CAxContainer::GetRefGuid(AxPropertyDescriptor* pAProp, ITypeInfo* TheInfo, HREFTYPE hreftype, AxPropertyDescriptor *pProperty)
{
	ITypeInfo *TheRefType = NULL;
	if ((hreftype != -1) &&
		SUCCEEDED(TheInfo->GetRefTypeInfo(hreftype, &TheRefType)))
	{
		LPTYPEATTR pTA;
		if (SUCCEEDED(TheRefType->GetTypeAttr(&pTA)))
		{
			pAProp->Guid = pTA->guid;
			switch (pTA->typekind)
			{
			case TKIND_ALIAS: // color, etc.
				if ((pTA->tdescAlias.vt == VT_USERDEFINED) &&
					(hreftype < VT_USERDEFINED))
					pAProp->Type = (VARTYPE) hreftype;
				else
					pAProp->Type = pTA->tdescAlias.vt;

				if (pAProp->Type == VT_USERDEFINED)
					GetRefGuid(
						pAProp,
						TheRefType, 
						((pTA->tdescAlias.vt == VT_PTR) ? pTA->tdescAlias.lptdesc->hreftype : pTA->tdescAlias.hreftype),
						pProperty);
				break;
			case TKIND_DISPATCH: // font, etc.
				pAProp->Type = VT_DISPATCH;
				pAProp->Guid = pTA->guid;
				mpParent->GetProject()->AddOleObject(pTA->guid, this); // add this OLE object
				break;
			case TKIND_ENUM: {
					VARDESC *pVarDesc;
					pAProp->NumEnum = pTA->cVars;
					pAProp->ArrEnum = new AxPropertyEnum[pTA->cVars];
					for (int i = 0; i < pTA->cVars; i++) { 
						if(SUCCEEDED(TheRefType->GetVarDesc( i, &pVarDesc ))) {
	   						BSTR  bstrName;
							if (SUCCEEDED(TheRefType->GetDocumentation(pVarDesc->memid,
								&bstrName, NULL,NULL,NULL )) && bstrName) 
							{   
#ifdef MAINWIN
								if (i == 0) pAProp->Type = pVarDesc->lpvarValue->n2.vt;
#else
								if (i == 0) pAProp->Type = pVarDesc->lpvarValue->vt;
#endif
								// bstrName, pVarDesc->lpvarValue;
								pAProp->ArrEnum[i].Name = bstrName;
								pAProp->ArrEnum[i].Var = *pVarDesc->lpvarValue;
								//SysFreeString(bstrName);
							}
							TheRefType->ReleaseVarDesc(pVarDesc);
						}
					}
				}
			}
		}
		TheRefType->Release();
	}
}


static void PerPropertyBrowsing(LPOLEOBJECT pIObject, AxPropertyDescriptor* pAProp)
{
	if ((pAProp->Type != VT_I2) &&
		(pAProp->Type != VT_UI2) &&
		(pAProp->Type != VT_I4) &&
		(pAProp->Type != VT_UI2))
		return; // wrong type

	if (pIObject == NULL)
		return;

	IPerPropertyBrowsing* pPpb = NULL;
	HRESULT hr =
		pIObject->QueryInterface(IID_IPerPropertyBrowsing, (void **) &pPpb);
	if (!SUCCEEDED(hr))
		return; // not supported

	int i;
	CALPOLESTR pStr;
	CADWORD cadword;
	hr = pPpb->GetPredefinedStrings(pAProp->Id, &pStr, &cadword);
	if (!SUCCEEDED(hr) || (hr == S_FALSE) ||
		(pStr.cElems == 1) ||
		(pStr.cElems != cadword.cElems))
	{
		pPpb->Release();
		return; // unexpected state
		goto Cleanup;
	}

	pAProp->NumEnum = pStr.cElems;
	pAProp->ArrEnum = new AxPropertyEnum[pAProp->NumEnum];
	for (i = 0; i < pAProp->NumEnum; i++)
	{
		VARIANT vt;
		hr = pPpb->GetPredefinedValue(pAProp->Id, cadword.pElems[i], &vt); 
		ASSERT(SUCCEEDED(hr));
		
		pAProp->ArrEnum[i].Name = pStr.pElems[i]; //buf;
		pAProp->ArrEnum[i].Var = vt;
	}

	pAProp->NumEnum = - pAProp->NumEnum; // - => don't use 0 -, 1 -, etc.

Cleanup:;
	CoTaskMemFree((void *)cadword.pElems);
	for (i=0; i < (int) pStr.cElems; i++) 
		CoTaskMemFree((void *)pStr.pElems[i]);  
	CoTaskMemFree((void *)pStr.pElems); 

	pPpb->Release();
}
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

static BOOL ObjectTypeInfo(LPUNKNOWN pObj, LPTYPEINFO *ppITypeInfo)
{
    HRESULT             hr;
    LPPROVIDECLASSINFO  pIProvideClassInfo;
    
    if (NULL==pObj || NULL==ppITypeInfo)
        return FALSE;

    *ppITypeInfo=NULL;

    /*
     * Get the object's IProvideClassInfo and call the GetClassInfo
     * method therein.  This will give us back the ITypeInfo for
     * the entire object.
     */

    hr=pObj->QueryInterface(IID_IProvideClassInfo
        , (LPVOID *)&pIProvideClassInfo);

    if (FAILED(hr))
		return FALSE;

	hr=pIProvideClassInfo->GetClassInfo(ppITypeInfo);
	pIProvideClassInfo->Release();

    return SUCCEEDED(hr);
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

static BOOL ObjectTypeInfoEvents(LPUNKNOWN pObj, LPTYPEINFO *ppITypeInfo)
    {
    HRESULT             hr;
    LPTYPEINFO          pITypeInfoAll;
    LPTYPEATTR          pTA;

    if (NULL==pObj || NULL==
		ppITypeInfo)
        return FALSE;

    if (!ObjectTypeInfo(pObj, &pITypeInfoAll))
        return FALSE;

    /*
     * We have the object's overall ITypeInfo in pITypeInfoAll.
     * Now get the type attributes which will tell us the number of
     * individual interfaces in this type library.  We then loop
     * through the "implementation types" for all those interfaces
     * calling GetImplTypeFlags, looking for the default source.
     */

    *ppITypeInfo=NULL;  //Use this to determine success

    if (SUCCEEDED(pITypeInfoAll->GetTypeAttr(&pTA)))
        {
        UINT        i;
        int         iFlags;

        for (i=0; i < pTA->cImplTypes; i++)
            {
            //Get the implementation type for this interface
            hr=pITypeInfoAll->GetImplTypeFlags(i, &iFlags);

            if (FAILED(hr))
                continue;

            if ((iFlags & IMPLTYPEFLAG_FDEFAULT)
                && (iFlags & IMPLTYPEFLAG_FSOURCE))
                {
                HREFTYPE    hRefType=NULL;

                /*
                 * This is the interface we want.  Get a handle to
                 * the type description from which we can then get
                 * the ITypeInfo.
                 */
                pITypeInfoAll->GetRefTypeOfImplType(i, &hRefType);
                hr=pITypeInfoAll->GetRefTypeInfo(hRefType
                    , ppITypeInfo);

                break;
                }
            }

        pITypeInfoAll->ReleaseTypeAttr(pTA);
        }

    pITypeInfoAll->Release();
    return (NULL!=*ppITypeInfo);
    }

/*
 * ObjectTypeInfoProperties
 *
 * Purpose:
 *  Retrieves the type property information from an object.  This is
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

static BOOL ObjectTypeInfoProperties(LPUNKNOWN pObj, LPTYPEINFO *ppITypeInfo)
{
    HRESULT             hr;
    LPTYPEINFO          pITypeInfoAll;
    LPTYPEATTR          pTA;

    if (NULL==pObj || NULL==ppITypeInfo)
        return FALSE;

    if (!ObjectTypeInfo(pObj, &pITypeInfoAll))
	{
		// there is another way to obtain typeinfo
		// (for simple CCmdTarget based objects)
		LPDISPATCH pDispatch = NULL;

		if (FAILED(pObj->QueryInterface(IID_IDispatch, (LPVOID *)&pDispatch)))
			return FALSE;

		pDispatch->GetTypeInfo(0, ::GetUserDefaultLCID(), ppITypeInfo);

		pDispatch->Release();
		return (*ppITypeInfo != NULL);
	}

    /*
     * We have the object's overall ITypeInfo in pITypeInfoAll.
     * Now get the type attributes which will tell us the number of
     * individual interfaces in this type library.  We then loop
     * through the "implementation types" for all those interfaces
     * calling GetImplTypeFlags, looking for the default source.
     */

    *ppITypeInfo=NULL;  //Use this to determine success

    if (SUCCEEDED(pITypeInfoAll->GetTypeAttr(&pTA)))
        {
        UINT        i;
        int         iFlags;

        for (i=0; i < pTA->cImplTypes; i++)
            {
            //Get the implementation type for this interface
            hr=pITypeInfoAll->GetImplTypeFlags(i, &iFlags);

            if (FAILED(hr))
                continue;

            if (!((iFlags & IMPLTYPEFLAG_FDEFAULT)
                && (iFlags & IMPLTYPEFLAG_FSOURCE)))
                {
                HREFTYPE    hRefType=NULL;

                /*
                 * This is the interface we want.  Get a handle to
                 * the type description from which we can then get
                 * the ITypeInfo.
                 */
                pITypeInfoAll->GetRefTypeOfImplType(i, &hRefType);
                hr=pITypeInfoAll->GetRefTypeInfo(hRefType
                    , ppITypeInfo);

                break;
                }
            }

        pITypeInfoAll->ReleaseTypeAttr(pTA);
        }

    pITypeInfoAll->Release();
    return (NULL!=*ppITypeInfo);
}

UINT CAxContainer::ExtractEventInfo(CDclControlObject *pControl, LPOLEOBJECT pIObject, BOOL bUseAsType)
{
	ITypeInfo *TheInfo = NULL;
	ObjectTypeInfoEvents(pIObject,&TheInfo);
	return ExtractEventInfo(pControl, TheInfo, bUseAsType);	
}

UINT CAxContainer::ExtractEventInfo(CDclControlObject *pControl, ITypeInfo *TheInfo, BOOL bUseAsType)
{
	int nNumberEvents = 0;
	int nNumberRealEvents = 0;

  	TYPEATTR *TheAttr;
  	
	if(TheInfo)	 {
		if(SUCCEEDED(TheInfo->GetTypeAttr(&TheAttr))) {
			nNumberEvents = TheAttr->cFuncs;
			TheInfo->ReleaseTypeAttr(TheAttr);
		}
	   FUNCDESC *pFuncDesc;  


	   for(int i = 0; i < (int) nNumberEvents; i++) { 
			if(SUCCEEDED(TheInfo->GetFuncDesc( i, &pFuncDesc )))
			{  
				CComBSTR            bstrName;
				CComBSTR            bstrDesc;
				// create a new event desc object
				std::auto_ptr< AxEventDescriptor > axEventDesc( new AxEventDescriptor );

				axEventDesc->Id = pFuncDesc->memid;
				if (SUCCEEDED(TheInfo->GetDocumentation(pFuncDesc->memid, &bstrName,
					&bstrDesc,NULL,NULL )) && bstrName) 
				{   
					axEventDesc->Name =  bstrName;
					axEventDesc->DocumentationDesc = bstrDesc;
				}

				BSTR pbstrNames[MAX_CALLING_ARGUMENTS+1];
				UINT nNames;
				for( UINT iName = 0; iName < MAX_CALLING_ARGUMENTS+1; iName++ )
					pbstrNames[iName] = NULL;
				TheInfo->GetNames(pFuncDesc->memid, pbstrNames, pFuncDesc->cParams+1, &nNames);

				axEventDesc->nArgs = pFuncDesc->cParams;
				// copy data types
				ASSERT(pFuncDesc->cParams < MAX_CALLING_ARGUMENTS);
				for (UINT n = 0 ; n < (UINT)pFuncDesc->cParams + 1; n++ )
				{
					if( n < (UINT)pFuncDesc->cParams )
					{
						const ELEMDESC &e = pFuncDesc->lprgelemdescParam[n];
						BOOL bNotByVal = (e.tdesc.vt == VT_PTR);
						VARTYPE vt = ((bNotByVal) ? e.tdesc.lptdesc->vt : e.tdesc.vt);

						if (vt == VT_USERDEFINED) 
							SetRefType(
								vt, 
								TheInfo,
								(bNotByVal) ? e.tdesc.lptdesc->hreftype : e.tdesc.hreftype,
								&axEventDesc->CallingArgClsids[n]);

						axEventDesc->CallingArgs[n] = vt;
						if( pbstrNames[n] != NULL )	
							axEventDesc->CallingArgNames[n] = CString(pbstrNames[n+1]);
					}
					SysFreeString( pbstrNames[n] );
				}
				axEventDesc->CallingArgs[pFuncDesc->cParams] = 0;

				// HRS
				axEventDesc->Params = FuncDescToString(TheInfo, pFuncDesc, bUseAsType);

				// create the new property
				RefCountedPtr< CPropertyObject > pProp = new CPropertyObject(PropActiveXEvent);
				// set the pointer to the ActiveX's Get property CMethodInfo object
				pProp->GetAxInterfaceDescriptorPtr()->SetEvent( axEventDesc.release() );
				// set the property as hidden so it won't show up in the property list, 
				// but in the events list instead.
				pProp->SetHidden(true);
				
				// add the property to the property list
				pControl->GetPropertyList().AddTail(pProp);
				
				TheInfo->ReleaseFuncDesc( pFuncDesc ) ;
 			}
		}
		TheInfo->Release();
	}
	
	return nNumberEvents;
}


UINT CAxContainer::ExtractMethodInfo(CDclControlObject *pControl, LPOLEOBJECT pIObject)
{
	ITypeInfo *TheInfo = NULL;
	ObjectTypeInfoProperties(pIObject,&TheInfo);
	return ExtractMethodInfo(pControl, TheInfo);
}

UINT CAxContainer::ExtractMethodInfo(CDclControlObject *pControl, ITypeInfo *TheInfo)
{
	int nNumberMethods = 0;

	int NumberFuncs = 0;
	int i,j;

	RefCountedPtr< CPropertyObject > pProp;
	TYPEATTR *TheAttr;
	
	// create the new property
	pProp = new CPropertyObject(PropActiveXMethods);
	// get the head position
	POSITION pos = pControl->GetPropertyList().GetHeadPosition();
	// move forward once so it is placed after the (ActiveX PropertyObject) property list item
	// and add the new property
	pos = pControl->GetPropertyList().InsertAfter(pos, pProp);
	// set the id of the control
	pProp->SetID(nObjectBrowser);
	

	if(TheInfo)
	{
		if(SUCCEEDED(TheInfo->GetTypeAttr(&TheAttr))) {
			NumberFuncs = TheAttr->cFuncs;
			TheInfo->ReleaseTypeAttr(TheAttr);
		}
		// first count those functions which are methods as
		// determined by INVOKEKIND == DISPATCH_METHOD
		FUNCDESC *pFuncDesc;
		nNumberMethods = 0;  
		for(i = 0; i < NumberFuncs; i++)
		{ 
			if(SUCCEEDED(TheInfo->GetFuncDesc( i, &pFuncDesc )))
			{  
				if ((pFuncDesc->invkind == DISPATCH_METHOD) && 
					(pFuncDesc->cParams < MAX_CALLING_ARGUMENTS) &&	// HRS
					((LONG) pFuncDesc->memid < (LONG) (1<<16))) // HRS
					 nNumberMethods++;

				TheInfo->ReleaseFuncDesc( pFuncDesc ) ;
			}
		}

    if (nNumberMethods == 0)
			return 0;

		std::auto_ptr< std::vector< RefCountedPtr< AxMethodDescriptor > > > prMethods( new std::vector< RefCountedPtr< AxMethodDescriptor > > );
		
		// Array properties are functions
		i = 0; 
		for(j = 0; j < NumberFuncs; j++)
		{ 
			if(SUCCEEDED(TheInfo->GetFuncDesc( j, &pFuncDesc )))
			{  
				CComBSTR  bstr;
				TheInfo->GetDocumentation(pFuncDesc->memid, &bstr, NULL, NULL, NULL);
				
				// Only count methods not GET/SET properties
				if ((pFuncDesc->invkind == DISPATCH_METHOD) &&
					(pFuncDesc->cParams < MAX_CALLING_ARGUMENTS) &&
					((LONG) pFuncDesc->memid < (LONG) (1<<16))) 
				{
					RefCountedPtr< AxMethodDescriptor > pAxMethod = new AxMethodDescriptor;
				  bool CanSet = true; 
		    	CComBSTR  bstrName; 
					CComBSTR  bstrDesc; 
					pAxMethod->Id = pFuncDesc->memid;
					if (SUCCEEDED(TheInfo->GetDocumentation(pFuncDesc->memid, 
						&bstrName,&bstrDesc,NULL,NULL ))) 
					{   
						pAxMethod->Name = CString(bstrName);
						pAxMethod->Desc = CString(bstrDesc);
					}
					
					// copy data types
				    ASSERT(pFuncDesc->cParams < MAX_CALLING_ARGUMENTS);
					
					// set the number of parameters
					pAxMethod->nParamQty = pFuncDesc->cParams;
					
					BSTR pbstrNames[MAX_CALLING_ARGUMENTS+1];
					UINT nNames;
					for( UINT iName = 0; iName < MAX_CALLING_ARGUMENTS+1; iName++ )
						pbstrNames[iName] = NULL;
					TheInfo->GetNames(pFuncDesc->memid, pbstrNames, pFuncDesc->cParams+1, &nNames);

					// loop through the parameters
					for ( UINT n = 0 ; n < (UINT)pFuncDesc->cParams + 1; n++ )
					{
						if( n < (UINT)pFuncDesc->cParams )
						{
							const ELEMDESC &e = pFuncDesc->lprgelemdescParam[n];
							BOOL bNotByVal = (e.tdesc.vt == VT_PTR);
							VARTYPE vt = ((bNotByVal) ? e.tdesc.lptdesc->vt : e.tdesc.vt);

							if (vt == VT_USERDEFINED) 
								SetRefType(
									vt, 
									TheInfo,
									(bNotByVal) ? e.tdesc.lptdesc->hreftype : e.tdesc.hreftype,
									&pAxMethod->CallingArgClsids[n]);

							pAxMethod->CallingArgs[n]  = vt;
							if( pbstrNames[n] != NULL )							
								pAxMethod->CallingArgNames[n] = CString(pbstrNames[n+1]);
						}
						SysFreeString( pbstrNames[n] );
					}
					pAxMethod->CallingArgs[pFuncDesc->cParams] = 0;
					
					// HRS
					pAxMethod->Params = FuncDescToString(TheInfo, pFuncDesc, TRUE);
					
					// pick up return type
					const ELEMDESC& e = pFuncDesc->elemdescFunc; 
					BOOL bNotByVal = (e.tdesc.vt == VT_PTR);
					VARTYPE vt = ((bNotByVal) ? e.tdesc.lptdesc->vt : e.tdesc.vt);
				
					if (pFuncDesc->wFuncFlags & (FUNCFLAG_FNONBROWSABLE|FUNCFLAG_FHIDDEN))
						CanSet = false; 

					if (pAxMethod->Name.Left(1) == _T('_')) 
						pAxMethod = NULL;
					else if (CanSet == true || pAxMethod->Id == -552) 
					{
						if (vt == VT_USERDEFINED) 
						{
							SetRefType(
								vt, 
								TheInfo,
								(bNotByVal) ? e.tdesc.lptdesc->hreftype : e.tdesc.hreftype,
								&pAxMethod->ReturnGuid);						
						}
						pAxMethod->ReturnType = vt;
						prMethods->push_back(pAxMethod);
					}
					i++;
				}  // if(pFuncDesc->i
				TheInfo->ReleaseFuncDesc( pFuncDesc ) ;
 			}  // if(SUCCEEDED
		}
		// All done
		TheInfo->Release();
		pProp->GetAxInterfaceDescriptorPtr()->SetMethods( prMethods.release() );
	} // if(TheInfo)
	
	return nNumberMethods;
}


static void AddAxProp(CDclControlObject *pControl, long lId, CString sName, CString sDesc, VARTYPE vt, bool bGet=true, bool bSet=true)
{
	RefCountedPtr< CPropertyObject > pProp = new CPropertyObject(PropActiveXProp);
	// create the activeX property descriptor that the CPropertyObject object will hold
	
	if (bSet)
	{
		AxPropertyDescriptor* axPropPut( new AxPropertyDescriptor );
		axPropPut->Id = lId;
		axPropPut->Name = sName;
		axPropPut->DocumentationDesc = sDesc;
		axPropPut->invKind = INVOKE_PROPERTYPUT;
		axPropPut->Type = vt;
		pProp->GetAxInterfaceDescriptorPtr()->SetPropPut( axPropPut );	
	}

	if (bGet)
	{
		AxPropertyDescriptor* axPropGet( new AxPropertyDescriptor );
		axPropGet->Id = lId;
		axPropGet->Name = sName;
		axPropGet->DocumentationDesc = sDesc;
		axPropGet->invKind = INVOKE_PROPERTYGET;
		axPropGet->Type = vt;
		pProp->GetAxInterfaceDescriptorPtr()->SetPropGet( axPropGet );	
	}

	pControl->GetPropertyList().AddTail(pProp);
	
}
static void SetupFont(CDclControlObject *pControl)
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
static void SetupPicture(CDclControlObject *pControl)
{
	AddAxProp(pControl, 0, _T("Handle"), _T("Retrieve the picture Handle."), VT_I4, true, false);
	AddAxProp(pControl, 2, _T("HPal"), _T("Retrieve or set the Handle to the Palette."), VT_I4);
	AddAxProp(pControl, 3, _T("Type"), _T("Retrieve the picture type."), VT_I2, true, false);
	AddAxProp(pControl, 4, _T("Width"), _T("Retrieve the picture Width."), VT_I4, true, false);
	AddAxProp(pControl, 5, _T("Height"), _T("Retrieve the picture Height."), VT_I4, true, false);	
}

BOOL CAxContainer::ExtractComponentsFromTLB(CDclControlObject *pControl, CLSID clsid)
{
	
	long lTypeInfoCount = 0;
	CComBSTR bstrName;
	BOOL bSuccess = TRUE;


	if (m_pTypeLib == NULL)
		return FALSE;
	
	//Get the type information count
	lTypeInfoCount = m_pTypeLib->GetTypeInfoCount();


	//Make sure that we have type information
	if(lTypeInfoCount == 0)
	{
		bSuccess = FALSE;
	}
	
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
			
			m_pTypeLib->GetTypeInfo(lIter, &TheInfo);
			if (TheInfo != NULL)
			{
				TheInfo->GetTypeAttr(&TheAttr);
				
				if (clsid == TheAttr->guid)
				{
					ExtractPropertyInfo(pControl, TheInfo, TRUE);
					ExtractMethodInfo(pControl, TheInfo);	
					
					CComBSTR bstrDoc;
					m_pTypeLib->GetDocumentation(lIter, &bstrName, &bstrDoc, NULL, NULL);
					if(bstrName)
						pControl->SetAxTypeName( CString(bstrName) );
				}				
			}			
		}               
	}

	return TRUE;
	
}

UINT CAxContainer::ExtractPropertyInfo(CDclControlObject *pControl, LPOLEOBJECT pIObject, BOOL bEnumList /*=FALSE*/)
{

	ITypeInfo *TheInfo = NULL;
	ObjectTypeInfoProperties(pIObject,&TheInfo);

	// extract the type library for later use.
	TheInfo->GetContainingTypeLib(&m_pTypeLib, &m_nTypeLibCount);	
	
	return ExtractPropertyInfo(pControl, TheInfo, bEnumList, pIObject);
}


UINT CAxContainer::ExtractPropertyInfo(CDclControlObject *pControl, ITypeInfo *TheInfo, BOOL bEnumList, LPOLEOBJECT pIObject)
{
	int nNumberProperties = 0;
	UINT NSingleProperties;
	UINT NumberFuncs;
	UINT i,j;
	TYPEATTR *TheAttr;
	
	
	if(TheInfo)	 {
		if(SUCCEEDED(TheInfo->GetTypeAttr(&TheAttr))) {
			NSingleProperties = TheAttr->cVars;
			NumberFuncs = TheAttr->cFuncs;
			TheInfo->ReleaseTypeAttr(TheAttr);
		}
		// first count those functions which are methods as
		// determined by INVOKEKIND == DISPATCH_METHOD
	   FUNCDESC *pFuncDesc;
	   nNumberProperties = NSingleProperties;  
	   for(i = 0; i < NumberFuncs; i++) { 
			if(SUCCEEDED(TheInfo->GetFuncDesc( i, &pFuncDesc )))
			{ 
				// not a method means it is a property 
				// Only count methods are GET/SET properties
				if (pFuncDesc->invkind == DISPATCH_PROPERTYGET ||
					pFuncDesc->invkind == DISPATCH_PROPERTYPUT ||
					pFuncDesc->invkind == DISPATCH_PROPERTYPUTREF
					)
					 nNumberProperties++;
				TheInfo->ReleaseFuncDesc( pFuncDesc ) ;
 			}
		}
		// now allocate some space
		if(nNumberProperties == 0)	{
			return 0;
		}
		// All variables are properties
	   VARDESC *pVarDesc; 
	   CString sWidth = _T("Width");
	   //sWidth = theWorkspace.LoadResourceString(IDS_PROP_WIDTH);
	   CString sHeight = _T("Height");
	   //sHeight = theWorkspace.LoadResourceString(IDS_PROP_HEIGHT);
	   for(i = 0; i < NSingleProperties; i++) { 
			if(SUCCEEDED(TheInfo->GetVarDesc( i, &pVarDesc ))) {
				CComBSTR  bstrName;
				CComBSTR  bstrDesc;
				
				//TheInfo->GetFuncDesc(i, &pFuncDesc);
			
				// create the activeX property descriptor that the CPropertyObject object will hold
				std::auto_ptr< AxPropertyDescriptor > axPropDesc( new AxPropertyDescriptor );
				
				axPropDesc->Id = pVarDesc->memid;
				axPropDesc->invKind = pFuncDesc->invkind;
				int nParams = pFuncDesc->cParams;
				long memid = pFuncDesc->memid;
				if (SUCCEEDED(TheInfo->GetDocumentation(pVarDesc->memid, &bstrName,
					&bstrDesc,NULL,NULL )) && bstrName) 
				{   
					axPropDesc->Name  =  bstrName;	
					
					if (axPropDesc->Name == sWidth)
					{
						axPropDesc->Name = sWidth + _T('2');
					}
					if (axPropDesc->Name == sHeight)
					{
						axPropDesc->Name = sHeight + _T('2');
					}
					
					axPropDesc->DocumentationDesc = bstrDesc;

					if (axPropDesc->Name == _T("FontSize"))
					{
						if (axPropDesc->DocumentationDesc == _T(""))
							axPropDesc->DocumentationDesc = _T("Indicates the size of the font to be used. ");
						axPropDesc->DocumentationDesc += _T("\\par \\par \\b1Note: \\b0 To properly calculate the font size, multiply your new font size by 10000.");
					}					
					
					ELEMDESC& e = pVarDesc->elemdescVar;
					BOOL bNotByVal = (e.tdesc.vt == VT_PTR);
					axPropDesc->Type = (bNotByVal ? e.tdesc.lptdesc->vt : e.tdesc.vt);
				  //  SysFreeString( bstrName ) ; 
				//	SysFreeString( bstrDesc ) ; 					 

					axPropDesc->CanSet = TRUE;
					
					BSTR pbstrNames[MAX_CALLING_ARGUMENTS+1];
					UINT nNames;
					for( int iName = 0; iName < int(UINT( nParams+1)); iName++ )
						pbstrNames[iName] = NULL;
					TheInfo->GetNames(memid, pbstrNames, MAX_CALLING_ARGUMENTS+1, &nNames);

					// loop through the parameters
					for ( UINT n = 0 ; n < (UINT)pFuncDesc->cParams + 1; n++ )
					{
						if( n < (UINT)pFuncDesc->cParams )
						{
							const ELEMDESC &e = pFuncDesc->lprgelemdescParam[n];
							BOOL bNotByVal = (e.tdesc.vt == VT_PTR);
							VARTYPE vt = ((bNotByVal) ? e.tdesc.lptdesc->vt : e.tdesc.vt);

							if (vt == VT_USERDEFINED) 
								SetRefType(vt, TheInfo,
									(bNotByVal) ? e.tdesc.lptdesc->hreftype : e.tdesc.hreftype, 
									&axPropDesc->CallingArgClsids[n]);

							axPropDesc->CallingArgs[n]  = vt;
							if( pbstrNames[n] != NULL )	
								axPropDesc->CallingArgNames[n] = CString(pbstrNames[n+1]);
						}
						SysFreeString( pbstrNames[n] );
					}
					
					axPropDesc->CallingArgs[nParams] = 0;

					if (pVarDesc->wVarFlags & (VARFLAG_FNONBROWSABLE|VARFLAG_FHIDDEN))
						axPropDesc->CanSet = FALSE;

					PerPropertyBrowsing(pIObject, axPropDesc.get()); // HRS
					if (axPropDesc->Type == VT_USERDEFINED)
						GetRefGuid(axPropDesc.get(), TheInfo, // HRS
							(bNotByVal ? e.tdesc.lptdesc->hreftype : e.tdesc.hreftype),
							axPropDesc.get());
				}
				TheInfo->ReleaseVarDesc( pVarDesc ) ;

				// find an instance of the property with the name of the one we have.
				// we do this so that the CPropertyObject will hold all INVOKE_PROPERTYGET, 
				// INVOKE_PROPERTYPUT and INVOKE_PROPERTYPUTREF ActiveX properties of the same 
				// name inside one CPropertyObject object.
				RefCountedPtr< CPropertyObject >pProp = pControl->FindProperty(axPropDesc->Name);
				
				// default the insertion flag to false so this property won't be inserted more than once					
				bool bInsert = false;
				
				if (pProp == NULL)	
				{
					// since this property type has not yet been created set the flag so this CPropertyObject
					// will get inserted into the property list
					bInsert = true;
				}
				
				if (axPropDesc->CanSet == FALSE)
				{
					pProp = new CPropertyObject(PropActiveXRunTime);
					pProp->SetHidden(true);
				}
				else					
					// set the property type as an ActiveX property pointer/holder/handler
					pProp = new CPropertyObject(PropActiveXProp);
				
				pProp->SetStringValue( axPropDesc->Name );
				pProp->GetAxInterfaceDescriptorPtr()->SetProp( axPropDesc.release() );
				if (axPropDesc->NumParams > 1)
				{
					pProp->SetType(PropActiveXRunTime); 
					pProp->SetHidden(true);
				}
			
				

				// here we insert the property
				if (bInsert)
				{
					// get the position where we are to add the property
					POSITION pos = pControl->FindPropertyInsertPos(pProp->GetName(), false);
					
					// if the property is NOT an illegal property we can include it in the list to be displayed 
					// in the CPropertyListCtrl control.
					// check where the property is to be inserted.
					if (pos == NULL)
						// add the property to the property list
						pControl->GetPropertyList().AddTail(pProp);
					else					
						// add the property to the property list
						pControl->GetPropertyList().InsertAfter(pos, pProp);
				}
			
			 }

		}
		// Array properties are functions
	   for(j = 0; j < NumberFuncs; j++) { 
			if(SUCCEEDED(TheInfo->GetFuncDesc( j, &pFuncDesc )))
			{  
				CComBSTR bstrName;
				CComBSTR  bstrDesc;
				bool bCanInsert = true;
				TheInfo->GetDocumentation(pFuncDesc->memid, &bstrName, NULL, NULL, NULL);
						
				// Only count methods are GET/SET properties
				if (pFuncDesc->invkind == DISPATCH_PROPERTYGET ||
					pFuncDesc->invkind == DISPATCH_PROPERTYPUT ||
					pFuncDesc->invkind == DISPATCH_PROPERTYPUTREF)
				{
					// create the activeX property descriptor that the CPropertyObject object will hold
					std::auto_ptr< AxPropertyDescriptor > axPropDesc( new AxPropertyDescriptor );
				
					axPropDesc->Id = pFuncDesc->memid;
					if (SUCCEEDED(TheInfo->GetDocumentation(pFuncDesc->memid, &bstrName,
						&bstrDesc,NULL,NULL )) && bstrName) 
					{   
						axPropDesc->invKind = pFuncDesc->invkind;
						axPropDesc->NumParams = pFuncDesc->cParams;
						axPropDesc->Name = bstrName;
						axPropDesc->DocumentationDesc = bstrDesc;
						
						if (axPropDesc->Name == sWidth)
							axPropDesc->Name = sWidth + _T('2');
						if (axPropDesc->Name == sHeight)
							axPropDesc->Name = sHeight + _T('2');

						if (axPropDesc->Name == _T("FontSize"))
						{
							if (axPropDesc->DocumentationDesc == _T(""))
								axPropDesc->DocumentationDesc = _T("Indicates the size of the font to be used. ");
							axPropDesc->DocumentationDesc += _T("\\par \\par \\b1Note: \\b0 To properly calculate the font size, multiply your new font size by 10000.");
						}
					
						BSTR pbstrNames[MAX_CALLING_ARGUMENTS + 1];
						UINT nNames;
						for( UINT iName = 0; iName < MAX_CALLING_ARGUMENTS+1; iName++ )
							pbstrNames[iName] = NULL;
						TheInfo->GetNames(pFuncDesc->memid, pbstrNames, MAX_CALLING_ARGUMENTS + 1, &nNames);

						// loop through the parameters
						for ( UINT n = 0 ; n < nNames; n++ )
						{
							const ELEMDESC &e = pFuncDesc->lprgelemdescParam[n];
							BOOL bNotByVal = (e.tdesc.vt == VT_PTR);
							VARTYPE vt = ((bNotByVal) ? e.tdesc.lptdesc->vt : e.tdesc.vt);

							if (vt == VT_USERDEFINED) 
								SetRefType(vt, TheInfo,
									(bNotByVal) ? e.tdesc.lptdesc->hreftype : e.tdesc.hreftype, 
									&axPropDesc->CallingArgClsids[n]);

							axPropDesc->CallingArgs[n]  = vt;
							if( pbstrNames[n] != NULL && n < nNames - 1 )	
								axPropDesc->CallingArgNames[n] = CString(pbstrNames[n+1]);
							SysFreeString( pbstrNames[n] ) ;
						}						

						axPropDesc->CallingArgs[pFuncDesc->cParams] = 0;

						ELEMDESC& e = pFuncDesc->elemdescFunc;
						BOOL bNotByVal = (e.tdesc.vt == VT_PTR);
						axPropDesc->Type = (bNotByVal ? e.tdesc.lptdesc->vt : e.tdesc.vt);
						// This is not strictly true - might be method
						axPropDesc->IsArray = TRUE;
						axPropDesc->CanSet = TRUE;
						// if the flags are set to this combination we can add it but not show it in the 
						// property list box
						if (pFuncDesc->wFuncFlags & (FUNCFLAG_FNONBROWSABLE|FUNCFLAG_FHIDDEN))
							axPropDesc->CanSet = FALSE; 
						// if this is the setting for the property, we plain just can't show it because
						// it's for internal use only.
						if (pFuncDesc->wFuncFlags == FUNCFLAG_FHIDDEN)
							bCanInsert = false; 
						PerPropertyBrowsing(pIObject, axPropDesc.get());
						if (axPropDesc->Type == VT_USERDEFINED)
							GetRefGuid(axPropDesc.get(), TheInfo, 
							(bNotByVal ? e.tdesc.lptdesc->hreftype : e.tdesc.hreftype),
							axPropDesc.get());
					}
					i++;

					// some properties we just plain can't add.
					if (bCanInsert)
					{
						// default the insertion flag to false so this property won't be inserted more than once					
						bool bInsert = false;
						
						// find an instance of the property with the name of the one we have.
						// we do this so that the CPropertyObject will hold all INVOKE_PROPERTYGET, 
						// INVOKE_PROPERTYPUT and INVOKE_PROPERTYPUTREF ActiveX properties of the same 
						// name inside one CPropertyObject object.
						RefCountedPtr< CPropertyObject > pProp = pControl->FindProperty(axPropDesc->Name);
						
						if (pProp == NULL)	
						{
							// since this property type has not yet been created set the flag so this CPropertyObject
							// will get inserted into the property list
							bInsert = true;
						
							// set the property type as an ActiveX property pointer/holder/handler
							if (axPropDesc->CanSet == FALSE)
							{
								pProp = new CPropertyObject(PropActiveXRunTime);
								pProp->SetHidden(true);
							}
							else
								pProp = new CPropertyObject(PropActiveXProp);
							pProp->SetStringValue(axPropDesc->Name);
						}
					
						switch( axPropDesc->invKind )
						{
						case INVOKE_PROPERTYGET:
							if (axPropDesc->NumParams > 0)
							{
								pProp->SetType(PropActiveXRunTime);						
								pProp->SetHidden(true);
							}
							pProp->GetAxInterfaceDescriptorPtr()->SetPropGet( axPropDesc.release() );
							break;
						case INVOKE_PROPERTYPUT:
							if (axPropDesc->NumParams > 1)
							{
								pProp->SetType(PropActiveXRunTime);	
								pProp->SetHidden(true);
							}
							pProp->GetAxInterfaceDescriptorPtr()->SetPropPut( axPropDesc.release() );
							break;
						case INVOKE_PROPERTYPUTREF:
							if (axPropDesc->NumParams > 1)
							{
								pProp->SetType(PropActiveXRunTime);	
								pProp->SetHidden(true);
							}
							pProp->GetAxInterfaceDescriptorPtr()->SetPropPutRef( axPropDesc.release() );
							break;
						default:
							if (!pProp->GetAxInterfaceDescriptorPtr()->GetProp())
								pProp->GetAxInterfaceDescriptorPtr()->SetProp( axPropDesc.release() );
							break;
						}
						// here we insert the property
						if (bInsert)
						{
							// get the position where we are to add the property
							POSITION pos = pControl->FindPropertyInsertPos(pProp->GetAxInterfaceDescriptorPtr()->GetName(), false);
							
							// if the property is NOT an illegal property we can include it in the list to be displayed 
							// in the CPropertyListCtrl control.
							// check where the property is to be inserted.
							if (pos == NULL)
								pControl->GetPropertyList().AddTail(pProp);
							else					
								pControl->GetPropertyList().InsertAfter(pos, pProp);						
						}
					}
				}
				TheInfo->ReleaseFuncDesc( pFuncDesc ) ;
 			}
		}
		// All done
		TheInfo->Release();
	}
	return nNumberProperties;
}

/////////////////////////////////////////////////////////////////////////////
// CAxContainer


CAxContainer::CAxContainer(CDclFormObject* pParent)
: mpParent( pParent )
, mpOleControl( NULL )
{
	m_nTypeLibCount = 0;
	m_pTypeLib = NULL;
	m_bInvokeWithSendString = FALSE;
}

CAxContainer::~CAxContainer()
{
}


BEGIN_MESSAGE_MAP(CAxContainer, CWnd)
	//{{AFX_MSG_MAP(CAxContainer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAxContainer message handlers

IDispatch *CAxContainer::GetOleIDispatch()
{	
	IDispatch *pDispatch = NULL;
	try
	{
		IUnknown *Unknown = GetControlUnknown();
		if (FAILED(Unknown->QueryInterface(IID_IDispatch, (void**) &pDispatch)))
			return NULL;
	}
	catch(...)
	{
		return NULL;
	}
	return pDispatch;
}

IOleObject *CAxContainer::GetIOleObject()
{	
	IUnknown *Unknown = GetControlUnknown();
	IOleObject *pOleObject = NULL;
	if (FAILED(Unknown->QueryInterface(IID_IOleObject, (void**) &pOleObject)))
		return NULL;

	return pOleObject;
}
	

void CAxContainer::Initialize()
{
	
	IOleObject *pOleObject = GetIOleObject();
	
	ExtractPropertyInfo(mpOleControl, pOleObject, TRUE);
	
	ExtractMethodInfo(mpOleControl, pOleObject);
	
	ExtractEventInfo(mpOleControl, pOleObject, FALSE);

	pOleObject->Release();

}

BOOL CAxContainer::CreateCtrl(CLSID Clsid, CDclControlObject *pControl, int nID, CWnd *pParent)
{
	mpOleControl = pControl;
	USES_CONVERSION;
	
	// get the rectangle of the new control
	CRect ArxRect;
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	pControl->m_clsid = Clsid;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE;//|WS_CLIPSIBLINGS|WS_CLIPCHILDREN;

	/*if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;*/

	// 3. setup license key (if used)
	BSTR bstrLicenseKey = NULL;
	if (!pControl->m_sLicenseKey.IsEmpty())
	{
		bstrLicenseKey = pControl->m_sLicenseKey.AllocSysString();
	}
	
	COleStreamFile *pOleStreamFile = NULL;
	
	if (pControl->m_pStream != NULL)
	{	
		pOleStreamFile = new COleStreamFile(pControl->GetLoadStream());
	}
	BOOL m_bActiveXCtrl = FALSE;
	BOOL bSuccess;
	try
	{		
		if (!pControl->m_sLicenseKey.IsEmpty())
		{
			m_bActiveXCtrl = CreateControl(Clsid, NULL, dwStyle,
								ArxRect, pParent, nID,
								pOleStreamFile, FALSE, bstrLicenseKey);
			if (m_bActiveXCtrl == FALSE)			
			{
				m_bActiveXCtrl = CreateControl(Clsid, NULL, dwStyle,
									ArxRect, pParent, nID,
									NULL, FALSE, bstrLicenseKey);
				if (m_bActiveXCtrl == TRUE)
					pControl->ClearStream();
			}
			if (m_bActiveXCtrl == FALSE)
				m_bActiveXCtrl = CreateControl(Clsid, NULL, dwStyle,
									ArxRect, pParent, nID,
									pOleStreamFile, FALSE, NULL);
			if (m_bActiveXCtrl == FALSE)
			{
				pControl->ClearStream();
			}
		}
		else
		{			
			m_bActiveXCtrl = 0;

			if (pOleStreamFile != NULL)
			{
				if (CreateControl(Clsid, NULL, dwStyle,
						ArxRect, pParent, nID,
						pOleStreamFile, FALSE, NULL) == TRUE)
				{
					m_bActiveXCtrl = TRUE;
				}
				else
				{
					m_bActiveXCtrl = CreateControl(Clsid, NULL, dwStyle, ArxRect, pParent, nID);
				}
			}
			else
			{
				m_bActiveXCtrl = CreateControl(Clsid, NULL, dwStyle, ArxRect, pParent, nID);
			}
			ShowWindow(TRUE);
		}		
	}
	
	catch(...)
	{
		bSuccess = FALSE;
	}
	if (bstrLicenseKey)
		::SysFreeString(bstrLicenseKey);
	if (pOleStreamFile != NULL)
		delete pOleStreamFile;

	switch (pControl->GetLngProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}

	InitToolTip();
	SetToolTipEx(this, mToolTip, pControl);

	return TRUE;
}


void CAxContainer::InitToolTip()
{
	if (mToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		mToolTip.Create(this);
		// Create inactive
		mToolTip.Activate(FALSE);
	}
} // End of InitToolTip


BOOL CAxContainer::CreateCtrl(CLSID Clsid, CDclControlObject *pControl, const RECT& rect, int nID, CWnd *pParent, bool bAddPropInfo)
{
	mpOleControl = pControl;
	USES_CONVERSION;
	BOOL m_bActiveXCtrl;	
	pControl = pControl;
	CRect rc(0,0,1,1);
	rc = rect;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE;
	
	BSTR bstrLicenseKey = NULL;

	if (pControl->m_bLicenseChecked == FALSE)
	{
		if (RequestLicenseKey(pControl->m_sLicenseKey, Clsid) == FALSE)
			pControl->m_sLicenseKey = CString();
		else
			// the m_bLicenseChecked is used to specify if the license string has been attempted to be extracted
			// doesn't mean it has a license, the length of m_sLicenseKey means it has a license.
			pControl->m_bLicenseChecked = TRUE;
	}


	if (!pControl->m_sLicenseKey.IsEmpty())
	{
		pControl->m_bLicenseChecked = TRUE;
		bstrLicenseKey = pControl->m_sLicenseKey.AllocSysString();
	}
	
	COleStreamFile *pOleStreamFile = NULL;
	
	// if properties have been set already.
	if (!bAddPropInfo && pControl->m_pStream)
		pOleStreamFile = new COleStreamFile(pControl->GetLoadStream());

	TRY
	{
		if (!pControl->m_sLicenseKey.IsEmpty())
		{
			m_bActiveXCtrl = CreateControl(Clsid, NULL, dwStyle,
								rc, pParent, nID,
								pOleStreamFile, FALSE, bstrLicenseKey);
			if (m_bActiveXCtrl == FALSE)			
			{
				m_bActiveXCtrl = CreateControl(Clsid, NULL, dwStyle,
									rc, pParent, nID,
									NULL, FALSE, bstrLicenseKey);
				if (m_bActiveXCtrl == TRUE)
				{
					pControl->ClearStream();
					CString sMsg;
					CString sMsg2;
					sMsg = theWorkspace.LoadResourceString(IDS_THE);
					sMsg2 = theWorkspace.LoadResourceString(IDS_RESETPROP);
					sMsg += pControl->GetStrProperty(nName) + sMsg2;
					MessageBox(sMsg, NULL, MB_ICONEXCLAMATION);					
				}
			}
			if (m_bActiveXCtrl == FALSE)
				m_bActiveXCtrl = CreateControl(Clsid, NULL, dwStyle,
									rc, pParent, nID,
									pOleStreamFile, FALSE, NULL);
			if (m_bActiveXCtrl == FALSE)
			{
				pControl->ClearStream();
				CString sMsg;
				sMsg = theWorkspace.LoadResourceString(IDS_NOTLICENCEDAX);
				MessageBox(sMsg, NULL, MB_ICONEXCLAMATION);
			}
		}
		else
		{			
			if (pOleStreamFile != NULL)
			{
				m_bActiveXCtrl =
					CreateControl(Clsid, NULL, dwStyle,
						rc, pParent, nID,
						pOleStreamFile, FALSE, NULL);
				if (!m_bActiveXCtrl)
				{
					
					CString sMsg;
					CString sMsg2;
					sMsg = theWorkspace.LoadResourceString(IDS_THE);
					sMsg2 = theWorkspace.LoadResourceString(IDS_RESETPROP);
					sMsg += pControl->GetStrProperty(nName) + sMsg2;
					MessageBox(sMsg, NULL, MB_ICONEXCLAMATION);
					
					m_bActiveXCtrl = CreateControl(Clsid, NULL, dwStyle, rc, pParent, nID);
				}				
			}			
			else			
			{
				m_bActiveXCtrl = CreateControl(Clsid, NULL, dwStyle, rc, pParent, nID);
			}
		}
		
	}
	CATCH_ALL(e)
	{
		m_bActiveXCtrl = FALSE;
	}
	END_CATCH_ALL;

	//if (bstrLicenseKey)
	//	::SysFreeString(bstrLicenseKey);
	if (!bAddPropInfo || !m_bActiveXCtrl)
	{
		if (pOleStreamFile != NULL)
			delete pOleStreamFile;
	}

	
	// if properties have not bee set yet.
	if (bAddPropInfo && m_bActiveXCtrl)
	{
		// call the method to load and get all the properties
		Initialize(); 
	}
	pControl->m_clsid = Clsid;
	return m_bActiveXCtrl;

}


HRESULT CAxContainer::GetProperty(AxPropertyDescriptor *axProp, CString &strReturnValue)
{
	
   DISPPARAMS dpParams;
   HRESULT hResult = -1;
   UINT iArgErr;
   COleVariant *pvarParams = NULL;
   
   if (axProp->NumParams > 0)
		pvarParams = new COleVariant[axProp->NumParams];

   COleVariant varResult;
   IDispatch *pDispatch = NULL;

   dpParams.rgvarg = pvarParams;
   dpParams.cArgs = axProp->NumParams;

   dpParams.rgdispidNamedArgs = NULL;
   dpParams.cNamedArgs = 0;
  
   varResult.Clear();
   BOOL bException = FALSE;

   TRY
   {
      pDispatch = GetOleIDispatch();
			EXCEPINFO excepInfo = {0};
	  hResult = pDispatch->Invoke( axProp->Id, IID_NULL,
		 GetUserDefaultLCID(), WORD( INVOKE_PROPERTYGET),
		 &dpParams, &varResult, &excepInfo, &iArgErr );
	  pDispatch->Release();
	  if( FAILED( hResult ) )
	  {
	     if( hResult == DISP_E_EXCEPTION )
		 {
		    bException = TRUE;
		 }
		 AfxThrowOleException( hResult );
	  }
	  // get the variant value as a string
	  strReturnValue = VariantToString( varResult );
	  
	  // check the variant to see if it is a bool
	  switch (varResult.vt)
	  {
		case VT_BOOL:
		case VT_UI1:
		case VT_I1:
		  CString sFirstChar = strReturnValue.Left(1);
		  sFirstChar.MakeUpper();
		  strReturnValue.MakeLower();
		  strReturnValue = sFirstChar + strReturnValue.Mid(1);		  
		  break;
	  }
   }
   CATCH( COleException, e )
   {
   }
   END_CATCH

   if (pvarParams != NULL)
		delete[] pvarParams;

   return hResult;

}

void CAxContainer::SetBoolProperty(AxPropertyDescriptor *axProp, CString sNewValue)
{
	BOOL bNewValue = (sNewValue.CompareNoCase(_T("TRUE")) == 0);
	static BYTE parms[] = VTS_BOOL;
	COleDispatchDriver pDisp(GetOleIDispatch());
	pDisp.InvokeHelper(axProp->Id, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, bNewValue);
}

		
		
void CAxContainer::SetProperty(AxPropertyDescriptor *axProp, CString sNewValue)
{
   USES_CONVERSION;
	DISPPARAMS dpParams;
	int nParams = 1;
	COleVariant varParams;   
	CString sTrue;
			
    dpParams.cArgs = nParams;
	
	VARTYPE vt = axProp->CallingArgs[0]; // set the variant type
		
    varParams = sNewValue;
	if (vt == 0)
	{
		vt = axProp->Type;
	}
    if (axProp->Type == VT_VOID)
	{
		vt = axProp->CallingArgs[0];
	}

	VARIANT* pArg = new VARIANT[nParams];
	ASSERT(pArg != NULL);   // should have thrown exception
	dpParams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dpParams.cArgs);

	pArg += dpParams.cArgs - 1;   // params go in opposite order
	CString sValue;

	// do a special case for bool properties
	if (nParams == 1 && vt == VT_BOOL)
	{
		SetBoolProperty(axProp, sNewValue);
		return;
	}

	for (int i =0; i<nParams; i++)
	{
		ASSERT(pArg >= dpParams.rgvarg);
		if (vt == 0)
			vt = axProp->Type;
		pArg->vt = vt;
		if (pArg->vt & VT_MFCBYREF)
		{
			pArg->vt &= ~VT_MFCBYREF;
			pArg->vt |= VT_BYREF;
		}
		switch (pArg->vt)
		{
		case VT_BOOL:
			if (varParams.bstrVal == sTrue)
			{
				pArg->boolVal = 1;
				pArg->bVal = 1;
				pArg->iVal = 1;
			}
			else
			{
				pArg->boolVal = 0;
				pArg->bVal = 0;
			}
			pArg->vt = VT_BOOL;
			break;
		case VT_UI1:
			if (varParams.bstrVal == sTrue)
			{
				pArg->bVal = 1;
			}
			else
			{
				pArg->bVal = 0;
			}
			pArg->vt = VT_UI1;
			break;
		case VT_I2:
			sValue = varParams.bstrVal;
			pArg->iVal = (short)_tstol(sValue);// varParams.iVal;
			break;
		case VT_I4:
			sValue = varParams.bstrVal;
			pArg->lVal =  _tstol(sValue);// varParams.lVal;
			break;
		case VT_R4:
			sValue = varParams.bstrVal;
			pArg->fltVal = (float)_tstof(sValue);// varParams.fltVal;
			break;
		case VT_R8:
			sValue = varParams.bstrVal;
			pArg->dblVal = _tstof(sValue);// varParams.dblVal;
			break;
		case VT_DATE:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->date =  varParams.date;
			break;
		case VT_CY:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->cyVal =  varParams.cyVal;
			break;
		case VT_BSTR:
			{
				LPCOLESTR lpsz = varParams.bstrVal;
				pArg->bstrVal = ::SysAllocString(lpsz);
				if (lpsz != NULL && pArg->bstrVal == NULL)
					AfxThrowMemoryException();
			}
			break;
#if !defined(_UNICODE) && !defined(OLE2ANSI)
		case VT_BSTRA:
			{
				//LPCSTR lpsz =  varParams.bstrVal;
				pArg->bstrVal = varParams.bstrVal;//::SysAllocString(T2COLE(lpsz));
				if (pArg->bstrVal == NULL)
					AfxThrowMemoryException();
				pArg->vt = VT_BSTR;
			}
			break;
#endif
		case VT_DISPATCH:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->pdispVal =  varParams.pdispVal;
			break;
		case VT_ERROR:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->scode =  varParams.scode;
			break;
		case VT_VARIANT:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			*pArg = varParams;
			break;
		case VT_UNKNOWN:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->punkVal = varParams.punkVal;
			break;

		case VT_I2|VT_BYREF:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->piVal = varParams.piVal;
			break;
		case VT_UI1|VT_BYREF:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->pbVal = varParams.pbVal;
			break;
		case VT_I4|VT_BYREF:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->plVal = varParams.plVal;
			break;
		case VT_R4|VT_BYREF:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->pfltVal = varParams.pfltVal;
			break;
		case VT_R8|VT_BYREF:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->pdblVal = varParams.pdblVal;
			break;
		case VT_DATE|VT_BYREF:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->pdate = varParams.pdate;
			break;
		case VT_CY|VT_BYREF:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->pcyVal = varParams.pcyVal;
			break;
		case VT_BSTR|VT_BYREF:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->pbstrVal = varParams.pbstrVal;
			break;
		case VT_DISPATCH|VT_BYREF:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->ppdispVal = varParams.ppdispVal;
			break;
		case VT_ERROR|VT_BYREF:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->pscode = varParams.pscode;
			break;
		case VT_BOOL|VT_BYREF:
			{
				if (varParams.bstrVal == sTrue)
				{
					pArg->boolVal = 1;					
				}
				else
				{
					pArg->boolVal = 0;
				}
				pArg->pboolVal = &pArg->boolVal;
				//pArg->pboolVal = varParams.pboolVal;
			}
			break;
		case VT_VARIANT|VT_BYREF:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->pvarVal = varParams.pvarVal;
			break;
		case VT_UNKNOWN|VT_BYREF:
			varParams.ChangeType( VTIToVT( pArg->vt ) );		  
			pArg->ppunkVal = varParams.ppunkVal;
			break;

		default:
			//ASSERT(FALSE);  // unknown type!
			break;
		}
		--pArg; // get ready to fill next argument
	}
   //
   WORD wFlags = axProp->invKind;//INVOKE_PROPERTYPUT;

   if( wFlags != INVOKE_PROPERTYPUT && 
	   wFlags != DISPATCH_PROPERTYPUT &&
	   wFlags != INVOKE_PROPERTYPUTREF)
   wFlags = INVOKE_PROPERTYPUT;
   
	DISPID dispidPropset;
	if (axProp->invKind == INVOKE_PROPERTYPUT)
	{
		dispidPropset =  DISPID_PROPERTYPUT;
	}
	else if (axProp->invKind == INVOKE_PROPERTYPUTREF)
	{
		dispidPropset =  DISPID_PROPERTYPUT;
	}
	else
	{
		dispidPropset =  DISPID_PROPERTYPUT;
	}

	dpParams.rgdispidNamedArgs = &dispidPropset;
	dpParams.cNamedArgs = nParams;

   
   TRY
   {
		IDispatch *m_lpDispatch = NULL;
		m_lpDispatch = GetOleIDispatch();
		COleVariant varResult;
		varResult.Clear();
		
		HRESULT hResult = m_lpDispatch->Invoke(axProp->Id, GUID_NULL, LOCALE_USER_DEFAULT, 
			wFlags, &dpParams, &varResult, NULL, NULL);
		
		m_lpDispatch->Release();
		if( FAILED( hResult ) )
		{	   
			 AfxThrowOleException( hResult );
		}
   }
   CATCH( COleException, e )
   {
   }
   END_CATCH

	   // cleanup any arguments that need cleanup
	if (dpParams.cArgs != 0)
	{
		VARIANT* pArg = dpParams.rgvarg + dpParams.cArgs - 1;
		for (int i =0; i<nParams; i++)
		{
			switch ((VARTYPE)axProp->CallingArgs[i])
			{
#if !defined(_UNICODE) && !defined(OLE2ANSI)
			case VT_BSTRA:
#endif
			case VT_BSTR:
				VariantClear(pArg);
				break;
			}
			--pArg;
		}
	}
	delete[] dpParams.rgvarg;	
	
	
	mpOleControl->SaveToStream(this);
}



IDispatch * CAxContainer::GetChildIDispatch(DISPID dispid)
{
	LPDISPATCH pDispatch;
	InvokeHelper(dispid, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&pDispatch, NULL);
	return pDispatch;
}


COleFont CAxContainer::GetFont(DISPID dispid)
{
	LPDISPATCH pDispatch;
	InvokeHelper(dispid, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&pDispatch, NULL);
	return COleFont(pDispatch);
}


void CAxContainer::SetFont(DISPID dispid, LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(dispid, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
	
	mpOleControl->SaveToStream(this);
}

void CAxContainer::SetPicture(DISPID dispid, LPDISPATCH newValue, WORD flag)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(dispid, flag, VT_EMPTY, NULL, parms,
		 newValue);

	mpOleControl->SaveToStream(this);
}

unsigned long CAxContainer::GetColor(DISPID dispid)
{
	unsigned long result;
	InvokeHelper(dispid, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}



void CAxContainer::SetColor(DISPID dispid, unsigned long propVal)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(dispid, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 propVal);

	mpOleControl->SaveToStream(this);	
}

void CAxContainer::LoadPictureFile(DISPID dispid, CString sFile, WORD flag)
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


void CAxContainer::LoadPicture(DISPID dispid, int nId)
{
	if (nId == -1)
	{
		SetPicture(dispid, NULL, DISPATCH_PROPERTYPUT);
		Invalidate();
		return;
	}
	
	// set start position for navigating Pictures
	POSITION pos = mpParent->GetProject()->GetPictureList().GetHeadPosition();

	// do loop to navigate Pictures
	while (pos != NULL)
	{
		// get current Picture in list
		const CPictureObject* pPicture = mpParent->GetProject()->GetPictureList().GetNext(pos);
		
		if (pPicture->GetID() == nId)
		{
			IDispatch *pPicDisp = const_cast<CPictureHolder*>(&pPicture->GetPicture())->GetPictureDispatch();
			if (pPicDisp != NULL)
			{
				SetPicture(dispid, pPicDisp, DISPATCH_PROPERTYPUT);
				Invalidate();
			}
		}
	}	
}


BOOL CAxContainer::GetPropertyPageCLSIDs( CArray< CLSID,
   CLSID& >& aclsidPages )
{
   ISpecifyPropertyPages * pSpecify = NULL;
   CAUUID pages;
   HRESULT hResult;
   ULONG iPage;

   IOleObject *lpOleObject = GetIOleObject();
	if (lpOleObject == NULL) 
	{
		return FALSE;
	}

	hResult = lpOleObject->QueryInterface(IID_ISpecifyPropertyPages, (void **) &pSpecify); 
   
	if( pSpecify != NULL )
    {
	  pages.cElems = 0;
	  pages.pElems = NULL;
	  hResult = pSpecify->GetPages( &pages );
	  if( FAILED( hResult ) )
	  {
		  lpOleObject->Release();
		  pSpecify->Release();
		 return( FALSE );
	  }

	  for( iPage = 0; iPage < pages.cElems; iPage++ )
	  {
		 aclsidPages.Add( pages.pElems[iPage] );
	  }

	  CoTaskMemFree( pages.pElems );
   }

	
	lpOleObject->Release();   
	pSpecify->Release();
	
   return( TRUE );
}


void CAxContainer::ShowPropertyPages()
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
   {
	  return;
   }
	aclsidPages.RemoveAll();
	tSuccess = GetPropertyPageCLSIDs( aclsidPages );
	if( !tSuccess )
	{
		return;
	}

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
	  USES_CONVERSION;
	  int iObject;
	  IUnknown** ppObjects;
	  CLSID* pclsidCommonPages;
	  CString strCaption;

	  ppObjects = (IUnknown**)_alloca( 1*sizeof(IUnknown*));
	  iObject = 0;
	  ppObjects[iObject] = GetOleIDispatch();
	  iObject++;

	  pclsidCommonPages = (CLSID*)_alloca( aclsidCommonPages.GetSize()*sizeof(CLSID));

	  for( iPage = 0; iPage < aclsidCommonPages.GetSize(); iPage++ )
	  {
		 pclsidCommonPages[iPage] = aclsidCommonPages[iPage];
	  }

	  strCaption = mpOleControl->GetStrProperty(nName);
	  
	  OleCreatePropertyFrame( m_hWnd, 0, 0, T2COLE( strCaption ),
		 1, ppObjects, (ULONG)aclsidCommonPages.GetSize(),
		 pclsidCommonPages, GetUserDefaultLCID(), 0, NULL );

	  //WINBUG: OleCreatePropertyFrame doesn't return focus to its parent when
	  // it closes
	  SetFocus();
   }
   
   mpOleControl->SaveToStream(this);
   
}

HRESULT CAxContainer::SaveToStream( IStream* pStream )
{
   HRESULT hResult;
   IPersistStream* pPersistStream = NULL;
   IPersistStreamInit* pPersistStreamInit = NULL;
   IPersist* pPersist = NULL;
   CLSID clsid;

   
   IOleObject *m_lpObject = GetIOleObject();

   hResult = m_lpObject->QueryInterface( IID_IPersist, (void**)&pPersist );
   if( FAILED( hResult ) )
   {
	  // Some control implementers forget that the IPersist* interfaces are all
	  // derived from IPersist.  If they forget to allow a QI for IPersist, let
	  // them know the error of their ways, and just go for IPersistStorage to
	  // get the IPersist interface.
	  hResult = m_lpObject->QueryInterface( IID_IPersistStorage,
		 (void**)&pPersist );
	  if( FAILED( hResult ) )
	  {
		m_lpObject->Release();

		if (pPersistStream != NULL)
			pPersistStream->Release();
		if (pPersistStreamInit != NULL)
			pPersistStreamInit->Release();
		if (pPersist != NULL)
			pPersist->Release();
		return( hResult );
	  }	  
   }

   hResult = pPersist->GetClassID( &clsid );
   if (!FAILED( hResult ))
		hResult = WriteClassStm( pStream, clsid );
   if( FAILED( hResult ) )
   {
		m_lpObject->Release();	   
		if (pPersistStream != NULL)
			pPersistStream->Release();
		if (pPersistStreamInit != NULL)
			pPersistStreamInit->Release();
		if (pPersist != NULL)
			pPersist->Release();
	   return( hResult );
   }

   hResult = m_lpObject->QueryInterface( IID_IPersistStream,
	  (void**)&pPersistStream );
   if( SUCCEEDED( hResult ) )
   {
	  hResult = pPersistStream->Save( pStream, TRUE );	  
   }
   else
   {
	  hResult = m_lpObject->QueryInterface( IID_IPersistStreamInit,
		 (void**)&pPersistStreamInit );
	  if( FAILED( hResult ) )
	  {
		   m_lpObject->Release();
		   if (pPersistStream != NULL)
				pPersistStream->Release();
		   if (pPersistStreamInit != NULL)
				pPersistStreamInit->Release();
		   if (pPersist != NULL)
				pPersist->Release();
		  return( hResult );
	  }
	  hResult = pPersistStreamInit->Save( pStream, TRUE );
   }
   if( FAILED( hResult ) )
   {
	   m_lpObject->Release();
	   
	   if (pPersistStream != NULL)
			pPersistStream->Release();
	   if (pPersistStreamInit != NULL)
			pPersistStreamInit->Release();
	   if (pPersist != NULL)
			pPersist->Release();
	   return( hResult );
   }

   m_lpObject->Release();
   if (pPersistStream != NULL)
		pPersistStream->Release();
   if (pPersistStreamInit != NULL)
		pPersistStreamInit->Release();
   if (pPersist != NULL)
		pPersist->Release();

	theWorkspace.SetModified(true);

   return( S_OK );

}

void CAxContainer::SetRefImageList(DISPID dispid, LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(dispid, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms,
		 newValue);
}

void CAxContainer::SetImageList(DISPID dispid, LPDISPATCH newValue)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(dispid, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

void CAxContainer::SetTooltipText( LPCTSTR pszText )
{
	SetToolTipEx( this, mToolTip, mpOleControl );
}
