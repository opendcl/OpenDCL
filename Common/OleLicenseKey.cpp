// AxMethodDescriptor.cpp : implementation file
//

#include "stdafx.h"
#include "OleLicenseKey.h"


BOOL RequestLicenseKey(CString &strLicenseKey, CLSID clsid)
{
     USES_CONVERSION;
	CString sText;
		  	     
	 BSTR bstrLicenseKey;
     LPCLASSFACTORY2 pClassFactory;
     BOOL bValidKeyReturned = FALSE;

	// Create an instance of the object and query it for
	//  the IClassFactory2 interface.
	if (SUCCEEDED(CoGetClassObject(clsid, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory2, (LPVOID *)(&pClassFactory))))
	{
	 LICINFO licinfo;

	 // Check to see if this object has a runtime License key.
	 if (SUCCEEDED(pClassFactory->GetLicInfo(&licinfo)))
	 {
	   if (licinfo.fRuntimeKeyAvail)
	   {
		 HRESULT hr;

		 // The object has a runtime License key, so request it.
		 hr = pClassFactory->RequestLicKey(0, &bstrLicenseKey);

		if (SUCCEEDED(hr))
		{
			if(bstrLicenseKey == NULL)
			{
				//sText = theWorkspace.LoadResourceString(IDS_OBJRETURNED);
				//bstrLicenseKey = sText.AllocSysString();
				bstrLicenseKey = ::SysAllocString(L"<Object returned a NULL license key>");
			}
			else
				// You have the license key.
				bValidKeyReturned = TRUE;
		}
		else
		   // Requesting the License key failed.
		   switch(hr)
		   {
			 case E_NOTIMPL:
				 {
		
			   bstrLicenseKey = ::SysAllocString(
					 L"<The object's class factory does not support"
					 L" run-time license keys>");
			   break;
				 }

			 case E_UNEXPECTED:
			   bstrLicenseKey = ::SysAllocString(
					 L"<An unexpected error occurred when requesting the"
					 L" run-time license key>");
			   break;

			 case E_OUTOFMEMORY:
			   bstrLicenseKey = ::SysAllocString(
					 L"<The object's class factory was unable to allocate"
					 L" the license key>");
			   break;

			 case CLASS_E_NOTLICENSED:
			   bstrLicenseKey = ::SysAllocString(
					 L"<The object's class factory supports run-time"
					 L" licensing, but the current machine\r\nitself is"
					 L" not licensed. Thus, a run-time key is not"
					 L" available on this machine>");
			   break;

			 default:
			   bstrLicenseKey = ::SysAllocString(
					 L"<An unknown error occurred when requesting the"
					 L" license key>");
		   }
	   }
	   else
		 bstrLicenseKey = ::SysAllocString(
		   L"<The object has no runtime license key>");
	 }
	 else
	   bstrLicenseKey = ::SysAllocString(
			 L"<Unable to get the licensing capabilities of the object's"
			 L" class factory>");

	 // Make sure you release the reference to the class factory.
	 pClassFactory->Release();
	}
	else
	 bstrLicenseKey = ::SysAllocString(
	   L"<Unable to get the IClassFactory2 interface from the"
	   L" specified object>");
    
	strLicenseKey = bstrLicenseKey;
	//::SysFreeString(bstrLicenseKey);

	// Return a BOOL specifying whether or not you were able to get a
	// valid license key.
	return bValidKeyReturned;
}
