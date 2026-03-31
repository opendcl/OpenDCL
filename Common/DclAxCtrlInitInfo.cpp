#include "StdAfx.h"
#include "DclAxCtrlInitInfo.h"
#include "AxTypeUtils.h"
#include "ArchiveEx.h"
#include "Filing.h"


CDclAxCtrlInitInfo::CDclAxCtrlInitInfo()
	: mClsid( CLSID_NULL )
{
}

CDclAxCtrlInitInfo::CDclAxCtrlInitInfo( const CDclAxCtrlInitInfo& Src )
	: mClsid( Src.mClsid )
	, msProgId( Src.msProgId )
	, msLicenseKey( Src.msLicenseKey )
	, mpStream( Src.mpStream )
{
}

CDclAxCtrlInitInfo::CDclAxCtrlInitInfo( const CLSID& clsid, LPCTSTR pszLicenseKey /*= NULL*/, LPCTSTR pszProgId /*= NULL*/ )
	: mClsid( clsid )
	, msProgId( (pszProgId && *pszProgId)? pszProgId : (LPCTSTR)GetAxProgId( clsid ) )
	, msLicenseKey( pszLicenseKey )
{
}

CDclAxCtrlInitInfo::~CDclAxCtrlInitInfo()
{	
}

CDclAxCtrlInitInfo& CDclAxCtrlInitInfo::operator =( const CDclAxCtrlInitInfo& Src )
{
	mClsid = Src.mClsid;
	msProgId = Src.msProgId;
	msLicenseKey = Src.msLicenseKey;
	mpStream = Src.mpStream;
	return *this;
}

bool CDclAxCtrlInitInfo::IsNull() const
{
	return (mClsid == CLSID_NULL && msProgId.IsEmpty());
}

CString CDclAxCtrlInitInfo::GetDisplayName() const
{
	if( !msTypeDisplayName.IsEmpty() )
		return msTypeDisplayName;
	if( !msProgId.IsEmpty() )
		return GetAxShortTypeName( msProgId );
	return GetAxShortTypeName( mClsid );
}

void CDclAxCtrlInitInfo::Serialize(CArchive& ar)
{
	DWORD nThisVersion = GetCurrentSaveVersion();
	if (ar.IsStoring())
	{
		ar << nThisVersion;		

		SerializeCLSID(ar, mClsid);
		ar << msProgId;
		ar << msLicenseKey;
		try
		{		
			ar << bool(mpStream != NULL);
			if (mpStream)
			{
				ULARGE_INTEGER iSeekPtr;
				LARGE_INTEGER nZeroDisp = { 0, 0 };
				HRESULT hr = mpStream->Seek( nZeroDisp, STREAM_SEEK_END, &iSeekPtr );
				ULONG cbStream = ULONG( iSeekPtr.QuadPart );
				ULONG nBytesLeft = cbStream;
					
				// store the bytes left
				ar << nBytesLeft;
					
				hr = mpStream->Seek( nZeroDisp, STREAM_SEEK_SET, NULL );
				ULONG nBytesRead;
				BYTE abData[512];
				while( nBytesLeft > 0 )
				{
					hr = mpStream->Read( abData, std::min<ULONG>( nBytesLeft, sizeof( abData ) ), &nBytesRead );
					if ( nBytesRead > 0 )
						ar.Write( abData, nBytesRead );
					nBytesLeft -= nBytesRead;
				}
			}
		}
		catch( CFileException* pException )
		{
			pException->Delete();
		}
	}
	else
	{
		ar >> nThisVersion;
		if (nThisVersion > GetCurrentSaveVersion())
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );

		SerializeCLSID(ar, mClsid);
		ar >> msProgId;
		ar >> msLicenseKey;
		mpStream = NULL;
		bool bHasStream;
		ar >> bHasStream;
		if( bHasStream )
		{
			HRESULT hResult = CreateStreamOnHGlobal( NULL, TRUE, &mpStream );
			if( FAILED( hResult ) )
				return;

			ULONG cbStream;
			ar >> cbStream;
			while( cbStream > 0 )
			{
				BYTE abData[512];
				ULONG nBytesToRead = std::min<ULONG>( cbStream, sizeof( abData ) );
				ar.Read( abData, nBytesToRead );
				mpStream->Write( abData, nBytesToRead, NULL );
				cbStream -= nBytesToRead;
			}

			LARGE_INTEGER nZeroDisp = { 0, 0 };
			mpStream->Seek( nZeroDisp, STREAM_SEEK_SET, NULL );
		}
		msTypeDisplayName.Empty();
	}
}
