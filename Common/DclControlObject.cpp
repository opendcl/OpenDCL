// DclControlObject.cpp : implementation file
//

#include "stdafx.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ImageListObject.h"
#include "AxContainerCtrl.h"
#include "AxInterfaceDescriptor.h"
#include "Filing.h"
#include "Workspace.h"
#include "Project.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "DialogControl.h"
#include "PropertyNames.h"
#include "AcadColorTable.h"
#include "DclControlProp.h"


/////////////////////////////////////////////////////////////////////////////
// CDclControlObject

IMPLEMENT_SERIAL(CDclControlObject, CObject, 1)

CDclControlObject::CDclControlObject()
: mpOwner( NULL )
, mType( CtlForm )
, mpDlgControl( NULL )
, mnID( -1 )
, midxZOrder( -1 )
, mbDeleted( false )
{
	m_nTotalBytes = 0;
	m_pCtrlHolder = NULL;
	m_sLicenseKey.Empty();
	m_sBaseCode.Empty();
	m_bLicenseChecked = FALSE;
	::memset(&m_clsid, 0, sizeof(GUID));	
	m_pStream = NULL;	
	m_pAxWnd = NULL;
	m_bEventsAsAction = false;
}

CDclControlObject::CDclControlObject(CDclFormObject* pOwner)
: mpOwner( pOwner )
, mType( CtlForm )
, mpDlgControl( NULL )
, mnID( -1 )
, midxZOrder( -1 )
, mbDeleted( false )
{
	m_nTotalBytes = 0;
	m_pCtrlHolder = NULL;
	m_sLicenseKey.Empty();
	m_sBaseCode.Empty();
	m_bLicenseChecked = FALSE;
	::memset(&m_clsid, 0, sizeof(GUID));	
	m_pStream = NULL;	
	m_pAxWnd = NULL;
	m_bEventsAsAction = false;
}

CDclControlObject::CDclControlObject(ControlType type, CDclFormObject* pOwner, LPCTSTR pszName /*= NULL*/)
: mpOwner( pOwner )
, mType( type )
, mpDlgControl( NULL )
, mnID( -1 )
, midxZOrder( -1 )
, mbDeleted( false )
{
	if( pszName )
		AddStringProperty(Prop::Name, PropString, pszName);
	m_nTotalBytes = 0;
	m_pCtrlHolder = NULL;
	m_sLicenseKey.Empty();
	m_sBaseCode.Empty();
	m_bLicenseChecked = FALSE;
	::memset(&m_clsid, 0, sizeof(GUID));	
	m_pStream = NULL;	
	m_pAxWnd = NULL;
	m_bEventsAsAction = false;
}

CDclControlObject::~CDclControlObject()
{
	ClearStream();
}


CWnd* CDclControlObject::GetWindow() const
{
	return mpDlgControl? mpDlgControl->GetControlWnd() : NULL;
}


void CDclControlObject::SetControlInstance( CDialogControl* pDlgControl )
{
	//note: an assertion here indicates more than 1 instance of the control; perhaps 
	//an earlier instance didn't get destroyed when it should have been?
	assert( mpDlgControl == NULL || pDlgControl == NULL );
	mpDlgControl = pDlgControl;
}


void CDclControlObject::SaveToStream(CAxContainerCtrl *pCtrl)
{
	CComPtr<IStream> pStream;
	HRESULT hr = CreateStreamOnHGlobal( NULL, TRUE, &pStream );
	if( FAILED( hr ) || !pStream )
		return;
	hr = pCtrl->SaveToStream( pStream );
	if( FAILED( hr ) )
		return;

#ifdef _DEBUG
	ULARGE_INTEGER iSeekPtr;
	LARGE_INTEGER nDisplacement;
	nDisplacement.QuadPart = 0;
	hr = pStream->Seek( nDisplacement, STREAM_SEEK_CUR, &iSeekPtr );
	ASSERT( SUCCEEDED( hr ) );
	ASSERT( iSeekPtr.QuadPart > 0 );
#endif
	m_pStream = pStream;
}

IStream *CDclControlObject::GetLoadStream()
{
	LARGE_INTEGER nDisplacement;
   
	if (!m_pStream)
	   return NULL;
   
	nDisplacement.QuadPart = 0;
	m_pStream->Seek( nDisplacement, STREAM_SEEK_SET, NULL );

	HRESULT hr = ReadClassStm( m_pStream, &m_clsid );	  
	ASSERT( SUCCEEDED( hr ) );
	if( FAILED(hr) )
		return NULL;
	return m_pStream;
}

void CDclControlObject::ClearStream()
{
	m_pStream = NULL;
}

//IOStatus CDclControlObject::WriteToTextFile(FILE* pFile, const CString &fileName) const
//{
//  fprintf(pFile, "\nCArxControlObject");
//
//  int nCount;
//  BOOL bImageList;
//  POSITION pos;
//
//  int nThisVersion = 6;
//  writeInt(pFile, nThisVersion);
//  writeString(pFile, msAxTypeName);
//  writeLong(pFile, mType);
//  writeShort(pFile, m_ClientHeight);
//  writeInt(pFile, 400);
//  writeInt(pFile, mnID);
//
//  // serialize the image if it exists
//  if (mpImageList != NULL)
//  {
//    bImageList = TRUE;
//    writeBOOL(pFile, bImageList);	
//    mpImageList->WriteToTextFile(pFile, fileName);
//  }
//  else
//  {
//    bImageList = FALSE;			
//    writeBOOL(pFile, bImageList);
//  }
//
//  if (mType == CtlActiveX)
//  {
//    // save the activeX info.
//    writeString(pFile, m_sLicenseKey);
//    writeString(pFile, m_sBaseCode);
//    writeBOOL(pFile, m_bLicenseChecked);
//    writeCLSID(pFile, m_clsid);
//    try
//    {		
//      LARGE_INTEGER nDisplacement;
//      ULONG nBytesRead;
//      ULONG nBytesLeft;
//      ULARGE_INTEGER iSeekPtr;
//      HRESULT hr;
//
//      nDisplacement.QuadPart = 0;
//
//      if (m_pStream) {
//        hr = m_pStream->Seek( nDisplacement, STREAM_SEEK_CUR, &iSeekPtr );
//        nBytesLeft = ULONG( iSeekPtr.QuadPart );
//
//        if (nBytesLeft == 0 && m_nTotalBytes > 0)
//          nBytesLeft = m_nTotalBytes;
//
//        hr = m_pStream->Seek( nDisplacement, STREAM_SEEK_SET, NULL );
//
//        BYTE* abData = new BYTE[nBytesLeft];
//        hr = m_pStream->Read( abData, nBytesLeft, &nBytesRead );
//        if ( nBytesRead > 0 ) {
//          writeBool(pFile, true);
//          writeBits(pFile, abData, nBytesRead);
//        } else {
//          writeBool(pFile, false);
//        }
//      } else {
//        writeBool(pFile, false);
//      }
//    }
//    catch( CFileException* pException )
//    {
//      pException->Delete();
//    }
//  }
//  // set counter for ArxControls
//  nCount = (int)mProperties.GetCount();
//
//  writeInt(pFile, nCount);
//
//  // set start position for navigating objects
//  pos = mProperties.GetHeadPosition();
//  // do loop to navigate objects
//  while (pos != NULL)
//  {
//    // get current object
//    TPropertyPtr pProp = mProperties.GetNext(pos);
//
//    // put object into archive
//    pProp->WriteToTextFile(pFile);
//
//    // increment counter
//    nCount--;
//  }
//	return statOK;
//}

void CDclControlObject::Serialize(CArchive& ar)
{
	DWORD nThisVersion = GetCurrentSaveVersion();
	short nCount;
	BOOL bImageList;
	CObject::Serialize( ar );
	if (ar.IsStoring())
	{
		ar << unsigned long(nThisVersion);
		ar << msAxTypeName;
		ar << long(mType);
		ar << mnID;

		// serialize the image if it exists
		ar << BOOL(mpImageList != NULL);
		if (mpImageList != NULL)
			mpImageList->Serialize(ar);
			
		if (mType == CtlActiveX)
		{
			// save the activeX info.
			ar << m_sLicenseKey;
			ar << m_sBaseCode;
			ar << m_bLicenseChecked;
			SerializeCLSID(ar, m_clsid);
			try
			{		
				LARGE_INTEGER nDisplacement;
				ULONG nBytesRead;
				BYTE abData[512];
 				ULONG nBytesLeft;
				ULARGE_INTEGER iSeekPtr;
				HRESULT hr;

				nDisplacement.QuadPart = 0;

				BOOL bHasStream = (!!m_pStream);
				ar << bHasStream;
				if (m_pStream)
				{
					hr = m_pStream->Seek( nDisplacement, STREAM_SEEK_CUR, &iSeekPtr );
					nBytesLeft = ULONG( iSeekPtr.QuadPart );
					
					if (nBytesLeft == 0 && m_nTotalBytes > 0)
						nBytesLeft = m_nTotalBytes;
					
					// store the bytes left
					ar << nBytesLeft;
					
					hr = m_pStream->Seek( nDisplacement, STREAM_SEEK_SET, NULL );
					while( nBytesLeft > 0 )
					{
						hr = m_pStream->Read( abData, std::min<ULONG>( nBytesLeft, sizeof( abData ) ), &nBytesRead );
						if ( nBytesRead > 0 )
						{
							ar.Write( abData, nBytesRead );
						}
						nBytesLeft -= nBytesRead;
					}
				}
			}
			catch( CFileException* pException )
			{
				pException->Delete();
			}
		}
		// write property list
		ar << (WORD)mProperties.size();
		for( TPropertyList::iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
		{
			(*iter)->Serialize(ar);
			#ifdef _DIAGNOSTIC
				if( (GetAsyncKeyState( VK_CONTROL ) & 0x8000) == 0x8000 )
					TraceFmt( _T("> %s\r\n"), (*iter)->toString() );
			#endif //_DIAGNOSTIC
		}
	}
	else
	{
		// clear these temp storage variables
		ar >> nThisVersion;
		if (nThisVersion > GetCurrentSaveVersion())
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );

		ar >> msAxTypeName;
		long lType;
		ar >> lType;
		mType = (ControlType)lType;
		if( mType == -1 )
			mType = CtlForm; //correct control type for controls from older ODC files
		if (nThisVersion < 9)
		{
			short nClientHeight;
			ar >> nClientHeight; //discard
		}
		if (nThisVersion < 8)
		{
			int nPurchaseState;
			ar >> nPurchaseState; //discard
		}

		if (nThisVersion >= 6)
			ar >> mnID;
		mpImageList = NULL;
		if (nThisVersion >= 5)
		{
			ar >> bImageList;
			if (bImageList == TRUE)
			{
				mpImageList = new CImageListObject();
				mpImageList->Serialize(ar);		
				if( mpImageList->IsDeleted() )
					mpImageList = NULL;
			}
		}

		if (nThisVersion >= 3)
		{
			if (mType == CtlActiveX)
			{
				// get the activeX info.
				ar >> m_sLicenseKey;
				ar >> m_sBaseCode;
				if (nThisVersion >= 4)
					ar >> m_bLicenseChecked;

				SerializeCLSID(ar, m_clsid);

				LARGE_INTEGER nDisplacement;
				ULONG nBytesLeft;
				BYTE abData[512];
   
				ClearStream();
				
				try
				{
					ULONG nBytesToRead;
					BOOL bHasStream;
   
					ar >> bHasStream;

					if (bHasStream)
					{
						HRESULT hResult = CreateStreamOnHGlobal( NULL, TRUE, &m_pStream );

						if( FAILED( hResult ) )
							return;

						// get the bytes left
						ar >> m_nTotalBytes;
						nBytesLeft = m_nTotalBytes;
					
						while( nBytesLeft > 0 )
						{
							nBytesToRead = std::min<ULONG>( nBytesLeft, sizeof( abData ) );
							ar.Read( abData, nBytesToRead );
							m_pStream->Write( abData, nBytesToRead, NULL );
							nBytesLeft -= nBytesToRead;
						}

						nDisplacement.QuadPart = 0;
						m_pStream->Seek( nDisplacement, STREAM_SEEK_SET, NULL );						
					}
					else
						m_pStream = NULL;
				}
				catch( CFileException* pException )
				{
					pException->Delete();
					return;
				}						
			}	
		}

		mProperties.clear();
		if (nThisVersion >= 2)
		{
			// get counter for objects
			ar >> nCount;		
			while (nCount-- > 0)
			{
				TPropertyPtr pProp = new CPropertyObject( TDclControlLockedPtr( this ), PropInvalid );
				pProp->Serialize(ar);
				InsertNamedProperty( pProp );
			#ifdef _DIAGNOSTIC
				if( (GetAsyncKeyState( VK_CONTROL ) & 0x8000) == 0x8000 )
					TraceFmt( _T("< %s\r\n"), pProp->toString() );
			#endif //_DIAGNOSTIC
			}
		}
		else
		{
			CList< CPropertyObject* > listProps;
			listProps.Serialize(ar);
			POSITION pos = listProps.GetHeadPosition();
			while( pos )
			{
				CPropertyObject* pProp = listProps.GetNext( pos );
				mProperties.push_back( pProp );
			}
		}
		// here were are going to add the font of this object to the font collection
		// before any pictures are loaded or any dialogs are displayed.
		theWorkspace.GetFontCollection().GetFont( TDclControlLockedPtr( this ), NULL );
	}

	if (!ar.IsStoring())
	{
		// remove any properties that shouldn't have been persisted or that were added erroneously in the past
		DclFormType eFormType = VdclInvalid;
		if( mpOwner )
		{
			TDclFormPtr pOwnerForm = mpOwner;
			while( pOwnerForm->GetParentForm() )
				pOwnerForm = pOwnerForm->GetParentForm(); //get top level parent
			eFormType = pOwnerForm->GetType();
		}
		TPropertyList::iterator iter = mProperties.begin();
		while( iter != mProperties.end() )
		{
			TPropertyList::iterator iterAt = iter++;
			Prop::Id nID = (*iterAt)->GetID();
			switch( mType )
			{
			case CtlComboBox:
				switch( nID )
				{
				case Prop::EventUpdate:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlFrame:
				switch( nID )
				{
				case Prop::BackgroundColor:
				case Prop::ForegroundColor:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlGraphicButton:
				switch( nID )
				{
				case Prop::EventDblClicked:
				case Prop::ImageList:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlGrid:
				switch( nID )
				{
				case Prop::DragnDropToAutoCAD:
				case Prop::DragnDropFromControl:
				case Prop::DragnDropFromAutoCAD:
				case Prop::DragnDropBegin:
				case Prop::DragnDropAllowBegin:
				case Prop::DragnDropAllowDrop:
				case Prop::EventReturnPressed:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlImageComboBox:
				switch( nID )
				{
				case Prop::EventUpdate:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlLabel:
				switch( nID )
				{
				case Prop::ToolTipTitle:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlListBox:
				switch( nID )
				{
				case Prop::HScrollBar:
				case Prop::EventClicked:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlOptionList:
				switch( nID )
				{
				case Prop::NoIntegralHeight:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlPictureBox:
				switch( nID )
				{
				case Prop::ImageList:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlStdButton:
				switch( nID )
				{
				case Prop::EventDblClicked:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlForm:
				if( mpOwner && mpOwner->GetParentForm() )
				{
					switch( nID )
					{
					case Prop::Name:
					case Prop::ObjectBrowser:
					case Prop::GlobalVarName:
					case Prop::UseBottomFromBottom:
					case Prop::UseTopFromBottom:
						mProperties.erase( iterAt ); //tab pages should not have these properties
						continue;
					}
				}
				switch( nID )
				{
				case Prop::ImageList:
					mProperties.erase( iterAt );
					continue;
				}
				switch( eFormType )
				{
				case VdclConfigTab:
					switch( nID )
					{
					case Prop::Custom:
					case Prop::DockableSides:
					case Prop::EventOnHelp:
					case Prop::EventInvoke:
						mProperties.erase( iterAt );
						continue;
					}
					break;
				case VdclModal:
					switch( nID )
					{
					case Prop::EventInvoke:
						mProperties.erase( iterAt );
						continue;
					}
					break;
				case VdclModeless:
					switch( nID )
					{
					case Prop::Icon:
						mProperties.erase( iterAt );
						continue;
					}
					break;
				case VdclDockable:
					switch( nID )
					{
					case Prop::MinDialogWidth:
					case Prop::MinDialogHeight:
					case Prop::MaxDialogWidth:
					case Prop::MaxDialogHeight:
						mProperties.erase( iterAt );
						continue;
					}
					break;
				}
				break;
			}
			switch( nID )
			{
			case Prop::IconYSpacing: //erroneously added as an event in OpenDCL 4.1 Alpha 4
				if( (*iterAt)->GetType() == PropEvent )
				{
					if( GetPropertyObject( Prop::DragnDropToAutoCAD ) )
						mProperties.erase( iterAt );
					else
						(*iterAt)->SetID( Prop::DragnDropToAutoCAD );
				}
				else if( (*iterAt)->IsHidden() )
				{
					long lVal = (*iterAt)->GetLongValue();
					if( lVal == 0 )
						lVal = 32;
					mProperties.erase( iterAt );
					AddLongProperty( Prop::IconYSpacing, PropLong, lVal ); //re-insert it where it belongs
				}
				continue;
			case Prop::TabOrder:
			case Prop::FontColor:
			case Prop::TabLabelAlign:
			case Prop::TabSelected:
				mProperties.erase( iterAt );
				continue;
			case Prop::AllowOrbiting:
				if( (*iterAt)->GetType() == PropBool )
				{
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case Prop::FontSizeStyle:
				{
					long lFontSize = GetLongProperty( Prop::FontSize );
					if( (*iterAt)->GetBooleanValue() )
					{
						if( lFontSize < 0 )
							SetLongProperty( Prop::FontSize, -lFontSize );
					}
					else
					{
						if( lFontSize > 0 )
							SetLongProperty( Prop::FontSize, -lFontSize );
					}
					mProperties.erase( iterAt );
					continue;
				}
				break;
			}
			switch( eFormType )
			{
			case VdclModal:
				switch( nID )
				{
				case Prop::EventInvoke:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			}
		}
	}
}

size_t CDclControlObject::CountPropertyListItems( Prop::Id nID )
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
		return pProp->size();
	return 0;
}

CString CDclControlObject::GetPropertyListItem( Prop::Id nID, size_t nIndex )
{
	CString sValue;
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp && nIndex < pProp->size() )
	{
		if( pProp->GetType() == PropStringArray )
			sValue = pProp->GetStringArrayPtr()->at( nIndex );
		else if( pProp->GetType() == PropIntArray )
			sValue.Format( _T("%d"), pProp->GetIntArrayPtr()->at( nIndex ) );
	}
	return sValue;
}

TPropertyPtr CDclControlObject::GetPropertyObject( Prop::Id nID ) const
{
	for( TPropertyList::const_iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
	{
		if( (*iter)->GetID() == nID )
			return (*iter);
	}
	return NULL;
}

TPropertyPtr CDclControlObject::FindPropertyObject( LPCTSTR pszName ) const
{
	for( TPropertyList::const_iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
	{
		if( (*iter)->GetName() == pszName )
			return (*iter);
	}
	return NULL;
}

void CDclControlObject::SetGlobalVariableName( LPCTSTR pszParentName /*= NULL*/ )	
{	
	CString sParentName = pszParentName;
	if( sParentName.IsEmpty() )
		sParentName = mpOwner->GetKeyPath();
	CString sControlName = sParentName + _T('_') + GetKeyName();
	AddStringProperty( Prop::GlobalVarName, PropString, sControlName, true );
}

void CDclControlObject::ClearGlobalVariableName()	
{	
	SetStringProperty( Prop::GlobalVarName, NULL );
}

bool CDclControlObject::SetStringProperty( Prop::Id nID, LPCTSTR pszValue )	
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( !pProp )
		return false;
	return pProp->SetStringValue( pszValue );
}

TPropertyPtr CDclControlObject::AddStringProperty( Prop::Id nID,
																									 PropertyType type /*= PropString*/,
																									 LPCTSTR pszValue /*= NULL*/,
																									 bool bResetExisting /*= false*/ )
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( !pProp )
	{
		pProp = new CPropertyObject( TDclControlLockedPtr( this ), type, 0, nID );
		InsertNamedProperty( pProp );
		pProp->SetStringValue( pszValue );
	}
	else if( bResetExisting )
	{
		pProp->SetType( type );
		pProp->SetStringValue( pszValue );
	}
	return pProp;
}

bool CDclControlObject::SetBooleanProperty( Prop::Id nID, bool bValue /*= true*/ )	
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( !pProp )
		return false;
	return pProp->SetBooleanValue( bValue );
}

TPropertyPtr CDclControlObject::AddBooleanProperty( Prop::Id nID,
																																				PropertyType type /*= PropBool*/,
																																				bool bValue /*= true*/,
																																				bool bResetExisting /*= false*/ )
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( !pProp )
	{
		pProp = new CPropertyObject( TDclControlLockedPtr( this ), type, 0, nID );
		InsertNamedProperty( pProp );
		pProp->SetBooleanValue( bValue );
	}
	else if( bResetExisting )
	{
		pProp->SetType( type );
		pProp->SetBooleanValue( bValue );
	}
	return pProp;
}

bool CDclControlObject::SetLongProperty( Prop::Id nID, long lValue /*= -1*/ )	
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( !pProp )
		return false;
	return pProp->SetLongValue( lValue );
}

TPropertyPtr CDclControlObject::AddLongProperty( Prop::Id nID,
																								 PropertyType type /*= PropLong*/,
																								 long lValue /*= -1*/,
																								 bool bResetExisting /*= false*/ )
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( !pProp )
	{
		pProp = new CPropertyObject( TDclControlLockedPtr( this ), type, 0, nID );
		InsertNamedProperty( pProp );
		pProp->SetLongValue( lValue );
	}
	else if( bResetExisting )
	{
		pProp->SetType( type );
		pProp->SetLongValue( lValue );
	}
	return pProp;
}

bool CDclControlObject::IsMicrosoftActiveXCtrl() const
{	
	CString sName;
	try
	{
		// get the ProgId
		LPOLESTR olestr;	
		HRESULT hResult = OleRegGetUserType( m_clsid, USERCLASSTYPE_FULL, &olestr );
		sName = olestr;
	}
	catch(...)
	{
		return false;
	}
	return (sName.Left(9) == _T("Microsoft"));
}

CString CDclControlObject::GetActiveXTypeName() const
{
	CString sName;
	
	if (!msAxTypeName.IsEmpty())
		return msAxTypeName;

	try
	{
		// get the ProgId
		WCHAR* pwszProgID = NULL;        
		HRESULT hResult = ProgIDFromCLSID(m_clsid, &pwszProgID);
		if (!FAILED(hResult))
		{
			sName = pwszProgID;
			CoTaskMemFree(pwszProgID);
		}
	}
	catch(...)
	{
	}

	sName.MakeReverse();
	sName = sName.Right(sName.GetLength() - sName.SpanIncluding(_T("0123456789.")).GetLength()).SpanExcluding(_T("."));
	sName.MakeReverse();
	
	const_cast< CDclControlObject* >(this)->msAxTypeName = sName; //cache it
	return sName;
}

CString CDclControlObject::GetStringProperty(Prop::Id nID) const
{
	CString sValue;
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
		sValue = pProp->GetStdProperty();
	return sValue;
}

long CDclControlObject::GetLongProperty(Prop::Id nID) const
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
	{
		//some boolean properties are read and written as longs,
		//so that case gets special handling here [ORW]
		if (pProp->GetType() == PropBool)
		{
			TraceFmt( _T("* 'PropLong' value requested from 'PropBool' property (ID: %d)! [%s]\r\n"), nID, _T(__FUNCTION__));
			return (long)pProp->GetBooleanValue();
		}
		return pProp->GetLongValue();
	}
	return -1;
}

void CDclControlObject::SetColorProperty(Prop::Id nID, COLORREF color)
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
		pProp->SetOLEColorValue(color);
}

COLORREF CDclControlObject::GetColorProperty(Prop::Id nID) const
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
		return GetRGBColor( pProp->GetLongValue() );
	return RGB(0,0,0);
}
	
bool CDclControlObject::GetBooleanProperty(Prop::Id nID) const
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
	{
		//some enum properties are read and written as booleans,
		//so that case gets special handling here [ORW]
		if( pProp->GetType() == PropEnum )
		{
			TraceFmt( _T("* 'PropBool' value requested from 'PropEnum' property (ID: %d)! [%s]\r\n"), nID, _T(__FUNCTION__));
			return (pProp->GetEnumValue() != 0);
		}
		return pProp->GetBooleanValue();
	}
	return false;
}

TPropertyPtr CDclControlObject::GetMethods() const
{ 
	for( TPropertyList::const_iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
	{
		if( (*iter)->GetType() == PropActiveXMethods )
			return (*iter);
	}
	return NULL;
}

TPropertyList::iterator CDclControlObject::FindPropertyInsertPos( Prop::Id nID, bool bHidden )
{
	if( nID == Prop::ObjectBrowser )
		return mProperties.begin();		
	return FindPropertyInsertPos( GetPropertyName(nID), bHidden );
}

TPropertyList::iterator CDclControlObject::FindPropertyInsertPos( LPCTSTR pszName, bool bHidden )
{
	if( !pszName || !*pszName )
		return mProperties.begin();
	if( mProperties.empty() )
		return mProperties.begin();

	TPropertyList::iterator iter = mProperties.end();
	do
	{
		--iter;
		assert( (*iter)->GetName() != pszName ); //trying to insert a property with a duplicate name?
		if( (*iter)->IsHidden() != bHidden )
			continue;
		if( (*iter)->GetName() <= pszName )
			break;
	}
	while( iter != mProperties.begin() );
	return ++iter;
}

bool CDclControlObject::InsertNamedProperty( TPropertyPtr pProp )
{
	assert( pProp != NULL );
	if( !pProp )
		return false;

	TPropertyList::iterator iter = FindPropertyInsertPos( pProp->GetName(), pProp->IsHidden() );
	if( iter != mProperties.end() && pProp->GetName() == (*iter)->GetName() )
	{
		TraceFmt( _T("* Attempt to insert duplicate property (%s) failed!\r\n"), (LPCTSTR)pProp->GetName() );
		return false;
	}
	mProperties.insert( iter, pProp );
	return true;
}

void CDclControlObject::ResetProperty( Prop::Id nId )
{
	TPropertyPtr pProp = GetPropertyObject( nId );
	if( pProp )
		pProp->clear();
}

void CDclControlObject::RemoveProperty( Prop::Id nId )
{
	for( TPropertyList::iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
	{
		if( (*iter)->GetID() == nId )
		{
			mProperties.erase( iter );
			break;
		}
	}
}

IOStatus CDclControlObject::ReadFromTextFile(std::ifstream &sFile, const CString &fileName)
{
  CString sObject;
	CString sClassname = readLine(sFile);
  if ( sClassname != "CDclControlObject" && sClassname != "CArxControlObject") return statInvalidFormat;
  int iVersion;
  if (!readInt(sFile, iVersion)) return statInvalidFormat;

  switch (iVersion) {
    case 6 : 
      return ReadFromTextFile6(sFile, fileName);
      break;
  }

  return statInvalidFormat;
}

IOStatus CDclControlObject::ReadFromTextFile6(std::ifstream &sFile, const CString &fileName)
{
  if (!readString(sFile, msAxTypeName)) return statInvalidFormat;
	long lType;
  if (!readLong(sFile, lType)) return statInvalidFormat;
	mType = (ControlType)lType;
	if( mType == -1 )
		mType = CtlForm; //correct control type for controls from older ODC files
	short nClientHeight; //discard it
  if (!readShort(sFile, nClientHeight)) return statInvalidFormat;
	int nPurchaseState;
  if (!readInt(sFile, nPurchaseState)) return statInvalidFormat; //discard
  if (!readInt(sFile, mnID)) return statInvalidFormat;

	mpImageList = NULL;
  bool bImageList;
  if (!readBool(sFile, bImageList)) return statInvalidFormat;
  if (bImageList) {
    mpImageList = new CImageListObject();
		IOStatus stat = mpImageList->ReadFromTextFile(sFile, fileName);
    if (stat != statOK) return stat;
  }

  if (mType == CtlActiveX)
  {
    // get the activeX info.
    if (!readString(sFile, m_sLicenseKey)) return statInvalidFormat;
    if (!readString(sFile, m_sBaseCode)) return statInvalidFormat;
    BOOL m_bLicenseChecked;
    if (!readBOOL(sFile, m_bLicenseChecked)) return statInvalidFormat;

    if (!readCLSID(sFile, m_clsid)) return statInvalidFormat;

    LARGE_INTEGER nDisplacement;

    ClearStream();

    try
    {
      BOOL bHasStream;

      if (!readBOOL(sFile, bHasStream)) return statInvalidFormat;

      if (bHasStream)
      {
        IStream *pStreamTemp = NULL;
        HRESULT hResult = CreateStreamOnHGlobal( NULL, TRUE, &pStreamTemp);

        if( FAILED( hResult ) ) return statInvalidFormat;

				std::string abData;
        if (!readBits(sFile, abData)) return statInvalidFormat;
        pStreamTemp->Write( abData.c_str(), abData.length(), NULL );

        nDisplacement.QuadPart = 0;
        pStreamTemp->Seek( nDisplacement, STREAM_SEEK_SET, NULL );
        m_pStream = pStreamTemp;
      }
      else
        m_pStream = NULL;
    }
    catch( CFileException* pException )
    {
      pException->Delete();
      return statInvalidFormat;
    }					
  }

  // get counter for objects
  int nCount;
  if (!readInt(sFile, nCount)) return statInvalidFormat;

  mProperties.clear();
  while (nCount-- > 0)
  {
    TPropertyPtr pProp = new CPropertyObject(TDclControlLockedPtr( this ), PropInvalid);
		IOStatus stat = pProp->ReadFromTextFile(sFile);
    if (stat != statOK) return stat;

		if( pProp->GetName().IsEmpty() )
		{
			switch( pProp->GetType() )
			{
			case PropActiveXProp:
			case PropActiveXEnum:
			case PropActiveXEvent:
			case PropActiveXRunTime:
			case PropActiveXMethods:
				pProp->SetStringValue( pProp->GetAxInterfaceDescriptorPtr()->GetName() );
			}
		}
    mProperties.push_back(pProp);		
  }

  // here were are going to add the font of this object to the font collection
  // before any pictures are loaded or any dialogs are displayed.
  CFontCollection *pFontCol = &theWorkspace.GetFontCollection();
  pFontCol->GetFont(TDclControlLockedPtr( this ), NULL);

  return statOK;
}

CString CDclControlObject::GetKeyName() const
{
	return GetStringProperty( Prop::Name );
}

CString CDclControlObject::GetKeyPath() const
{
	CString sPath;
	if( mpOwner )
	{
		sPath = mpOwner->GetKeyPath();
		if( mType != CtlForm )
			sPath += _T('_') + GetKeyName();
	}
	else
		sPath = GetKeyName();
	return sPath;
}

CString CDclControlObject::GetVarName() const
{
	CString sName = GetStringProperty( Prop::GlobalVarName );
	if( sName.IsEmpty() )
		sName = GetKeyPath();
	return sName;
}


#ifdef _DIAGNOSTIC
LPCTSTR CDclControlObject::toString() const
{
	CString sInstance;
	if( mpDlgControl )
		sInstance.Format( _T(" (DlgControl: %s)"), asString( mpDlgControl ) );
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("CDclControlobject [%s: %s%s]"), asString( mType ), GetKeyPath(), (LPCTSTR)sInstance );
	return buf;
}

void CDclControlObject::dump( bool bDeep /*= true*/ ) const
{
	CString sOut;
	sOut.Format( _T("%s\r\n"), toString() );
	theWorkspace.DisplayStatus( sOut );
	if( !bDeep )
		return;
	size_t idx = 0;
	for( TPropertyList::const_iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
	{
		CString sProp;
		sProp.Format( _T("  #%4s: %s\r\n"), asString( idx++ ), (*iter)->toString() );
		theWorkspace.DisplayStatus( sProp );
	}
}
#endif


#ifdef _DEBUG
void CDclControlObject::dumpDebugger( bool bDeep /*= true*/ ) const
{
	TraceFmt( _T("%s\r\n"), toString() );
	if( !bDeep )
		return;
	size_t idx = 0;
	for( TPropertyList::const_iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
		TraceFmt( _T("  #%4s: %s\r\n"), asString( idx++ ), (*iter)->toString() );
}
#endif
