// ChmLib.cpp : implementation file
//

#include "stdafx.h"
#include "ChmLib.h"

#define INITGUID
#include <guiddef.h>

DEFINE_GUID(CLSID_ITStorage, 0x5d02926a, 0x212e, 0x11d0, 0x9d, 0xf9, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xec);
//DEFINE_GUID(IID_ITStorage, 0x88cc31de, 0x27ab, 0x11d0, 0x9d, 0xf9, 0x0, 0xa0, 0xc9, 0x22, 0xe6, 0xec);

typedef struct _ITS_Control_Data
{
  UINT cdwControlData;
  UINT adwControlData[1];
} ITS_Control_Data, *PITS_Control_Data;  

enum ECompactionLev 
{ 
  COMPACT_DATA = 0, 
  COMPACT_DATA_AND_PATH
};

DECLARE_INTERFACE_( __declspec(uuid("88cc31de-27ab-11d0-9df9-00a0c922e6ec")) IITStorage, IUnknown)
{
  STDMETHOD(StgCreateDocfile) 
		(const WCHAR* pwcsName, DWORD grfMode, DWORD reserved, IStorage** ppstgOpen) PURE;
  STDMETHOD(StgCreateDocfileOnILockBytes) 
		(ILockBytes * plkbyt, DWORD grfMode, DWORD reserved, IStorage ** ppstgOpen) PURE;
  STDMETHOD(StgIsStorageFile) 
		(const WCHAR * pwcsName) PURE;
  STDMETHOD(StgIsStorageILockBytes) 
		(ILockBytes * plkbyt) PURE;
  STDMETHOD(StgOpenStorage) 
		(const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage ** ppstgOpen) PURE;
  STDMETHOD(StgOpenStorageOnILockBytes)
    (ILockBytes * plkbyt, IStorage * pStgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage ** ppstgOpen ) PURE;
  STDMETHOD(StgSetTimes)
		(WCHAR const * lpszName,  FILETIME const * pctime, FILETIME const * patime, FILETIME const * pmtime) PURE;
  STDMETHOD(SetControlData)
		(PITS_Control_Data pControlData) PURE;
  STDMETHOD(DefaultControlData)
		(PITS_Control_Data *ppControlData) PURE;
  STDMETHOD(Compact)
		(const WCHAR* pwcsName, ECompactionLev iLev) PURE;
};



static HRESULT OpenChmStream( /*in*/LPCTSTR pszFilename, /*out*/IStream** ppStream )
{
	CComPtr< IITStorage > pITStorage;
	HRESULT hr = pITStorage.CoCreateInstance( CLSID_ITStorage, NULL, CLSCTX_INPROC_SERVER );
	if( FAILED(hr) )
		return hr;

	CStringW sHelpFile( AfxGetApp()->m_pszHelpFilePath );
	CComPtr< IStorage > pStorage;
	hr = pITStorage->StgOpenStorage( sHelpFile, NULL, STGM_READ | STGM_SHARE_DENY_WRITE, 
																	 NULL, 0, &pStorage);
	if( FAILED(hr) )
		return hr;

	CStringW sFilename( pszFilename );
	sFilename.Replace( L'\\', L'/' );
	CStringW sPath;
	if( sFilename.Left( 1 ) != L'/' )
		sPath = L'/';
	sPath += sFilename;

	return pStorage->OpenStream( sPath, NULL, STGM_READ, 0, ppStream );
}


bool IsChmFile( LPCTSTR pszFilename )
{
	CComPtr< IStream > pStream;
	HRESULT hr = OpenChmStream( pszFilename, &pStream );
	return (SUCCEEDED(hr));
}


bool IsChmFolder( LPCTSTR pszFolder )
{
	CComPtr< IITStorage > pITStorage;
	HRESULT hr = pITStorage.CoCreateInstance( CLSID_ITStorage, NULL, CLSCTX_INPROC_SERVER );
	if( FAILED(hr) )
		return false;

	CStringW sHelpFile( AfxGetApp()->m_pszHelpFilePath );
	CComPtr< IStorage > pStorage;
	hr = pITStorage->StgOpenStorage( sHelpFile, NULL, STGM_READ | STGM_SHARE_DENY_WRITE, 
																	 NULL, 0, &pStorage);
	if( FAILED(hr) )
		return false;

	CStringW sFolder( pszFolder );
	sFolder.Replace( L'\\', L'/' );
	CStringW sPath;
	if( sFolder.Left( 1 ) != L'/' )
		sPath = L'/';
	sPath += sFolder;
	CComPtr< IStorage > pStgFolder;
	hr = pStorage->OpenStorage( sPath, NULL, STGM_READ, NULL, 0, &pStgFolder );
	if( FAILED(hr) )
		return false;
	return true;
}


bool ReadChmFile( LPCTSTR pszFilename, CString& sContent )
{
	CComPtr< IStream > pStream;
	HRESULT hr = OpenChmStream( pszFilename, &pStream );
	if( FAILED(hr) )
		return false;
	STATSTG entry = {0};
	hr = pStream->Stat( &entry, STATFLAG_NONAME );
	if( FAILED(hr) )
		return false;
	ULONG cbRead = 0;
	ULONG cbSize = (ULONG)entry.cbSize.QuadPart;
	CStringA sContentA;
	hr = pStream->Read( sContentA.GetBuffer( cbSize ), cbSize, &cbRead );
	if( SUCCEEDED(hr) )
	{
		sContentA.ReleaseBuffer( int(cbRead) );
		sContent = sContentA;
	}
	return (!sContent.IsEmpty());
}


bool ReadChmBinary( LPCTSTR pszFilename, CByteArray& buf )
{
	CComPtr< IStream > pStream;
	HRESULT hr = OpenChmStream( pszFilename, &pStream );
	if( FAILED(hr) )
		return false;
	STATSTG entry = {0};
	hr = pStream->Stat( &entry, STATFLAG_NONAME );
	if( FAILED(hr) )
		return false;
	ULONG cbRead = 0;
	ULONG cbSize = (ULONG)entry.cbSize.QuadPart;
	buf.SetSize( int(cbSize) );
	hr = pStream->Read( buf.GetData(), cbSize, &cbRead );
	if( FAILED(hr) || cbRead == 0 )
	{
		buf.RemoveAll();
		return false;
	}
	buf.SetSize( int(cbRead) );
	return true;
}


bool EnumChmFolder( /*in*/LPCTSTR pszFolder, /*out*/CStringArray& rsStreams )
{
	CComPtr< IITStorage > pITStorage;
	HRESULT hr = pITStorage.CoCreateInstance( CLSID_ITStorage, NULL, CLSCTX_INPROC_SERVER );
	if( FAILED(hr) )
		return false;

	CStringW sHelpFile( AfxGetApp()->m_pszHelpFilePath );
	CComPtr< IStorage > pStorage;
	hr = pITStorage->StgOpenStorage( sHelpFile, NULL, STGM_READ | STGM_SHARE_DENY_WRITE, 
																	 NULL, 0, &pStorage);
	if( FAILED(hr) )
		return false;

	CStringW sFolder( pszFolder );
	sFolder.Replace( L'\\', L'/' );
	CStringW sPath;
	if( sFolder.Left( 1 ) != L'/' )
		sPath = L'/';
	sPath += sFolder;
	CComPtr< IStorage > pStgFolder;
	hr = pStorage->OpenStorage( sPath, NULL, STGM_READ, NULL, 0, &pStgFolder );
	if( FAILED(hr) )
		return false;
	CComPtr< IEnumSTATSTG > pEnum;
	hr = pStgFolder->EnumElements( 0, 0, 0, &pEnum );
	if( FAILED(hr) )
		return false;
	hr = pEnum->Reset();
	if( FAILED(hr) )
		return false;
	rsStreams.RemoveAll();
	ULONG ctFetched = 0;
	STATSTG stg = {};
	while( SUCCEEDED( pEnum->Next( 1, &stg, &ctFetched ) ) && ctFetched == 1 )
	{
		if( stg.type != STGTY_STREAM )
			continue;
		rsStreams.Add( CString( stg.pwcsName ) );
	}

	return true;
}
