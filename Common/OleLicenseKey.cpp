// OleLicenseKey.cpp : implementation file
//

#include "stdafx.h"
#include "OleLicenseKey.h"


BOOL RequestLicenseKey(CString &strLicenseKey, CLSID clsid)
{
	CComBSTR bstrLicenseKey;
	CComPtr<IClassFactory2> pClassFactory;

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
					if(bstrLicenseKey.Length() == 0)
					{
						strLicenseKey = L"<Object returned a NULL license key>";
						return FALSE;
					}
					else
					{
						strLicenseKey = bstrLicenseKey;
						return TRUE;
					}
				}
				else
				{
					// Requesting the License key failed.
					switch(hr)
					{
					case E_NOTIMPL:
						strLicenseKey = L"<The object's class factory does not support run-time license keys>";
						return FALSE;
						break;

					case E_UNEXPECTED:
						strLicenseKey = L"<An unexpected error occurred when requesting the run-time license key>";
						return FALSE;
						break;

					case E_OUTOFMEMORY:
						strLicenseKey = L"<The object's class factory was unable to allocate the license key>";
						return FALSE;
						break;

					case CLASS_E_NOTLICENSED:
						strLicenseKey = L"<The object's class factory supports run-time licensing, but the current machine\r\n"
														L"itself is not licensed. Thus, a run-time key is not available on this machine>";
						return FALSE;
						break;

					default:
						strLicenseKey = L"<An unknown error occurred when requesting the license key>";
						return FALSE;
						break;
					}
				}
			}
			else
			{
				strLicenseKey = L"<The object has no runtime license key>";
				return FALSE;
			}
		}
		else
		{
	   strLicenseKey = L"<Unable to get the licensing capabilities of the object's class factory>";
		 return FALSE;
		}
	}
	else
	{
		strLicenseKey = L"<Unable to get the IClassFactory2 interface from the specified object>";
		return FALSE;
	}
	return FALSE;
}
