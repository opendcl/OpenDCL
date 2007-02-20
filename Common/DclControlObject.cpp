// DclControlObject.cpp : implementation file
//

#include "stdafx.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ImageListObject.h"
#include "AxContainer.h"
#include "AxInterfaceDescriptor.h"
#include "Filing.h"
#include "PurchaseMode.h"
#include "Workspace.h"
#include "Project.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "DialogControl.h"
#include "PropertyNames.h"
#include "AcadColorTable.h"


static const int nNotSet = -1;


static void AddControlProperty(CDclControlObject *pControl, PropertyId nID, LPCTSTR strValue, PropertyType ValueType)
{
	// create new property object pointer to pass to AddTail method
	RefCountedPtr< CPropertyObject > pProp = new CPropertyObject( ValueType, 0, nID );
	pProp->SetStringValue(strValue);
	pProp->SetHidden(false);
	pControl->InsertNamedProperty( pProp );
}


/////////////////////////////////////////////////////////////////////////////
// CDclControlObject

IMPLEMENT_SERIAL(CDclControlObject, CObject, 1)

CDclControlObject::CDclControlObject()
: mpOwner( NULL )
, mType( CtlFormPropHolder )
, mpDlgControl( NULL )
, mnID( -1 )
, midxZOrder( -1 )
{
	m_nTotalBytes = 0;
	m_rcOldPosition.SetRect(0,0,0,0);
	m_Delete = FALSE;
	m_pCtrlHolder = NULL;
	m_sLicenseKey.Empty();
	m_sBaseCode.Empty();
	m_bLicenseChecked = FALSE;
	::memset(&m_clsid, 0, sizeof(GUID));	
	m_pStream = NULL;	
	m_pAxWnd = NULL;

  m_pVisible = NULL;

  m_pLeft = NULL;
  m_pTop = NULL;
  m_pWidth = NULL;
  m_pHeight = NULL;

  m_pOffsetLeft = NULL;
  m_pOffsetTop = NULL;
  m_pOffsetRight = NULL;
  m_pOffsetBottom = NULL;

  m_pUseLeftOffset = NULL;
  m_pUseTopOffset = NULL;
  m_pUseRightOffset = NULL;
  m_pUseBottomOffset = NULL;

	m_bEventsAsAction = false;
}

CDclControlObject::CDclControlObject(CDclFormObject* pOwner)
: mpOwner( pOwner )
, mType( CtlFormPropHolder )
, mpDlgControl( NULL )
, mnID( -1 )
, midxZOrder( -1 )
{
	m_nTotalBytes = 0;
	m_rcOldPosition.SetRect(0,0,0,0);
	m_Delete = FALSE;
	m_pCtrlHolder = NULL;
	m_sLicenseKey.Empty();
	m_sBaseCode.Empty();
	m_bLicenseChecked = FALSE;
	::memset(&m_clsid, 0, sizeof(GUID));	
	m_pStream = NULL;	
	m_pAxWnd = NULL;

  m_pVisible = NULL;

  m_pLeft = NULL;
  m_pTop = NULL;
  m_pWidth = NULL;
  m_pHeight = NULL;

  m_pOffsetLeft = NULL;
  m_pOffsetTop = NULL;
  m_pOffsetRight = NULL;
  m_pOffsetBottom = NULL;

  m_pUseLeftOffset = NULL;
  m_pUseTopOffset = NULL;
  m_pUseRightOffset = NULL;
  m_pUseBottomOffset = NULL;

	m_bEventsAsAction = false;
}

CDclControlObject::CDclControlObject(ControlType type, CDclFormObject* pOwner, LPCTSTR pszName /*= NULL*/)
: mpOwner( pOwner )
, mType( type )
, mpDlgControl( NULL )
, mnID( -1 )
, midxZOrder( -1 )
{
	if( pszName )
		AddStringProperty(nName, PropString, pszName);
	m_nTotalBytes = 0;
	m_rcOldPosition.SetRect(0,0,0,0);
	m_Delete = FALSE;
	m_pCtrlHolder = NULL;
	m_sLicenseKey.Empty();
	m_sBaseCode.Empty();
	m_bLicenseChecked = FALSE;
	::memset(&m_clsid, 0, sizeof(GUID));	
	m_pStream = NULL;	
	m_pAxWnd = NULL;

  m_pVisible = NULL;

  m_pLeft = NULL;
  m_pTop = NULL;
  m_pWidth = NULL;
  m_pHeight = NULL;

  m_pOffsetLeft = NULL;
  m_pOffsetTop = NULL;
  m_pOffsetRight = NULL;
  m_pOffsetBottom = NULL;

  m_pUseLeftOffset = NULL;
  m_pUseTopOffset = NULL;
  m_pUseRightOffset = NULL;
  m_pUseBottomOffset = NULL;

	m_bEventsAsAction = false;
}

CDclControlObject::~CDclControlObject()
{
	ClearProperties();
	ClearStream();
}


CWnd* CDclControlObject::GetWindow() const
{
	return mpDlgControl? mpDlgControl->GetControl() : NULL;
}


void CDclControlObject::SetControlInstance( CDialogControl* pDlgControl )
{
	//note: an assertion here indicates more than 1 instance of the control; perhaps 
	//an earlier instance didn't get destroyed when it should have been?
	assert( mpDlgControl == NULL || pDlgControl == NULL );
	mpDlgControl = pDlgControl;
}


void CDclControlObject::SaveToStream(CAxContainer *pCtrl)
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

IOStatus CDclControlObject::WriteToTextFile(FILE* pFile, const CString &fileName) const
{
  fprintf(pFile, "\nCArxControlObject");

  int nCount;
  BOOL bImageList;
  POSITION pos;

  int nThisVersion = 6;
  writeInt(pFile, nThisVersion);
  writeString(pFile, msAxTypeName);
  writeLong(pFile, mType);
  writeShort(pFile, m_ClientHeight);
  //m_PurchaseState = nCurrentPurchaseMode;
  writeInt(pFile, m_PurchaseState);
  writeInt(pFile, mnID);

  // serialize the image if it exists
  if (mpImageList != NULL)
  {
    bImageList = TRUE;
    writeBOOL(pFile, bImageList);	
    mpImageList->WriteToTextFile(pFile, fileName);
  }
  else
  {
    bImageList = FALSE;			
    writeBOOL(pFile, bImageList);
  }

  if (mType == CtlActiveX)
  {
    // save the activeX info.
    writeString(pFile, m_sLicenseKey);
    writeString(pFile, m_sBaseCode);
    writeBOOL(pFile, m_bLicenseChecked);
    writeCLSID(pFile, m_clsid);
    try
    {		
      LARGE_INTEGER nDisplacement;
      ULONG nBytesRead;
      ULONG nBytesLeft;
      ULARGE_INTEGER iSeekPtr;
      HRESULT hr;

      nDisplacement.QuadPart = 0;

      if (m_pStream) {
        hr = m_pStream->Seek( nDisplacement, STREAM_SEEK_CUR, &iSeekPtr );
        nBytesLeft = ULONG( iSeekPtr.QuadPart );

        if (nBytesLeft == 0 && m_nTotalBytes > 0)
          nBytesLeft = m_nTotalBytes;

        hr = m_pStream->Seek( nDisplacement, STREAM_SEEK_SET, NULL );

        BYTE* abData = new BYTE[nBytesLeft];
        hr = m_pStream->Read( abData, nBytesLeft, &nBytesRead );
        if ( nBytesRead > 0 ) {
          writeBool(pFile, true);
          writeBits(pFile, abData, nBytesRead);
        } else {
          writeBool(pFile, false);
        }
      } else {
        writeBool(pFile, false);
      }
    }
    catch( CFileException* pException )
    {
      pException->Delete();
    }
  }
  // set counter for ArxControls
  nCount = (int)mProperties.GetCount();

  writeInt(pFile, nCount);

  // set start position for navigating objects
  pos = mProperties.GetHeadPosition();
  // do loop to navigate objects
  while (pos != NULL)
  {
    // get current object
    RefCountedPtr< CPropertyObject > pProp = mProperties.GetNext(pos);

    // put object into archive
    pProp->WriteToTextFile(pFile);

    // increment counter
    nCount--;
  }
	return statOK;
}

void CDclControlObject::Serialize(CArchive& ar)
{
	DWORD nThisVersion = GetCurrentSaveVersion();
	short nCount;
	BOOL bImageList;
	POSITION pos;
	CObject::Serialize( ar );
	if (ar.IsStoring())
	{
		ar << unsigned long(nThisVersion);
		ar << msAxTypeName;
		ar << long(mType);
		ar << m_ClientHeight;
		m_PurchaseState = nCurrentPurchaseMode;
		ar << m_PurchaseState;
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
						hr = m_pStream->Read( abData, min( nBytesLeft, sizeof( abData ) ), &nBytesRead );
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
		ar << (WORD)mProperties.GetCount();
		pos = mProperties.GetHeadPosition();
		while (pos != NULL)
		{
			RefCountedPtr< CPropertyObject > pProp = mProperties.GetNext(pos);
			pProp->Serialize(ar);
			TraceFmt( _T("> %s\r\n"), pProp->toString() );
		}
	}
	else
	{
		// clear these temp storage variables
		ar >> nThisVersion;
		if (nThisVersion > GetCurrentSaveVersion())
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );

		m_rcOldPosition.SetRect(0,0,0,0);
		ar >> msAxTypeName;
		long lType;
		ar >> lType;
		mType = (ControlType)lType;
		ar >> m_ClientHeight;
		ar >> m_PurchaseState;

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
				if( mpImageList->m_Delete )
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
							nBytesToRead = min( nBytesLeft, sizeof( abData ) );
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

		if (nThisVersion >= 2)
		{
			// get counter for objects
			ar >> nCount;		
			mProperties.RemoveAll();
			while (nCount-- > 0)
			{
				RefCountedPtr< CPropertyObject > pProp = new CPropertyObject( PropInvalid );
				pProp->Serialize(ar);
				mProperties.AddTail(pProp);		
				TraceFmt( _T("< %s\r\n"), pProp->toString() );
			}

			// here we are going to add the font of this object to the font collection
			// before any pictures are loaded or any dialogs are displayed.
			theWorkspace.GetFontCollection().GetFont(this, NULL);

			m_pVisible = GetPropertyObject(nVisible);

			m_pLeft = GetPropertyObject(nLeft);
			m_pTop = GetPropertyObject(nTop);
			m_pWidth = GetPropertyObject(nWidth);
			m_pHeight = GetPropertyObject(nHeight);

			m_pOffsetLeft = GetPropertyObject(nLeftFromRight);
			m_pOffsetTop = GetPropertyObject(nTopFromBottom);
			m_pOffsetRight = GetPropertyObject(nRightFromRight);
			m_pOffsetBottom = GetPropertyObject(nBottomFromBottom);

			m_pUseLeftOffset = GetPropertyObject(nUseLeftFromRight);
			m_pUseTopOffset = GetPropertyObject(nUseTopFromBottom);
			m_pUseRightOffset = GetPropertyObject(nUseRightFromRight);
			m_pUseBottomOffset = GetPropertyObject(nUseBottomFromBottom);
		}
	}

	if (nThisVersion == 1)
	{
		mProperties.Serialize(ar);
		// here were are going to add the font of this object to the font collection
		// before any pictures are loaded or any dialogs are displayed.
		theWorkspace.GetFontCollection().GetFont(this, NULL);
	}

	if (m_PurchaseState < 0)
		m_PurchaseState = 0;

	// this is done to remove any tool tip text properties 	
	if (!ar.IsStoring())
	{
		POSITION pos;
		INT_PTR nCount = mProperties.GetCount() - 1;
		while (nCount > 0)
		{		
			pos = mProperties.FindIndex(nCount);
			RefCountedPtr< CPropertyObject > pSourceProp = mProperties.GetAt(pos);
			
			
			if (mType == CtlGrid)
			{
				if (pSourceProp->GetID() == nDragnDropToAutoCAD)
					mProperties.RemoveAt(pos);
				if (pSourceProp->GetID() == nDragnDropFromControl)
					mProperties.RemoveAt(pos);
				if (pSourceProp->GetID() == nDragnDropFromAutoCAD)
					mProperties.RemoveAt(pos);
				if (pSourceProp->GetID() == nDragnDropBegin)
					mProperties.RemoveAt(pos);
				if (pSourceProp->GetID() == nDragnDropAllowBegin)
					mProperties.RemoveAt(pos);
				if (pSourceProp->GetID() == nDragnDropAllowDrop)
					mProperties.RemoveAt(pos);
				if (pSourceProp->GetID() == nEventReturnPressed)
					mProperties.RemoveAt(pos);
			}
			
			if (pSourceProp->GetID() == nAcadColor && mType == CtlFrame)
				mProperties.RemoveAt(pos);
			if (pSourceProp->GetID() == nForeColor && mType == CtlFrame)
				mProperties.RemoveAt(pos);
			if (pSourceProp->GetID() == nTabOrder)
				mProperties.RemoveAt(pos);
			if (pSourceProp->GetID() == nLabelColor)
				mProperties.RemoveAt(pos);
			if (pSourceProp->GetID() == nEventDblClicked && mType == CtlStdButton)
				mProperties.RemoveAt(pos);
			if (pSourceProp->GetID() == nEventDblClicked && mType == CtlGraphicButton)
				mProperties.RemoveAt(pos);
			
			/*if (pSourceProp->GetID() == nComboBoxStyle && mType == CtlComboBox)
					pSourceProp->GetLongValue() = 2;*/
			if (pSourceProp->GetID() == nHScrollBar && mType == CtlListBox)
				mProperties.RemoveAt(pos);
			if (pSourceProp->GetID() == nAllowOrbiting && pSourceProp->GetType() == PropBool)
				mProperties.RemoveAt(pos);
			
			if (pSourceProp->GetID() == nToolTipText && mType == CtlLabel)
				mProperties.RemoveAt(pos);
			
			if (pSourceProp->GetID() == nTabLabelAlign)
				mProperties.RemoveAt(pos);
			if (pSourceProp->GetID() == nTabSelected)
				mProperties.RemoveAt(pos);
			
			nCount--;
		}
	}
	
}

size_t CDclControlObject::CountPropertyListItems(PropertyId nID)
{
	POSITION pos = mProperties.GetHeadPosition();	
	while (pos)
	{
		RefCountedPtr< CPropertyObject > pProperty = mProperties.GetNext(pos);
		if (pProperty->GetID() == nID)
			return pProperty->size(); // return the value
	}
	return 0;
}

CString CDclControlObject::GetPropertyListItem(PropertyId nID, size_t nIndex)
{
	POSITION pos = mProperties.GetHeadPosition();	
	while (pos)
	{
		RefCountedPtr< CPropertyObject > pProperty = mProperties.GetNext(pos);
		if (pProperty->GetID() == nID)
		{
			CString sValue;
			if (pProperty->GetType() == PropStringArray && nIndex < pProperty->size())
				sValue = pProperty->GetStringArrayPtr()->at(nIndex);
			else if( pProperty->GetType() == PropIntArray && nIndex < pProperty->size() )
				sValue.Format( _T("%d"), pProperty->GetIntArrayPtr()->at(nIndex) );
			return sValue;
		}
	}
	return CString();
}

RefCountedPtr< CPropertyObject > CDclControlObject::GetPropertyObject( PropertyId nID ) const
{
	POSITION posProp = mProperties.GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = mProperties.GetNext(posProp);
		if (pProperty && pProperty->GetID() == nID)
			return pProperty;
	}
	return NULL;
}

RefCountedPtr< CPropertyObject > CDclControlObject::FindPropertyObject( LPCTSTR pszName ) const
{
	POSITION posProp = mProperties.GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = mProperties.GetNext(posProp);
		if (pProperty && pProperty->GetName() == pszName)
			return pProperty;
	}
	return NULL;
}

void CDclControlObject::SetGlobalVariableName( LPCTSTR pszParentName /*= NULL*/ )	
{	
	CString sParentName = pszParentName;
	if( sParentName.IsEmpty() )
		sParentName = mpOwner->GetKeyPath();
	CString sControlName = sParentName + _T('_') + GetKeyName();
	AddStringProperty( nGlobalVarName, PropString, sControlName, true );
}

void CDclControlObject::ClearGlobalVariableName()	
{	
	SetStringProperty( nGlobalVarName, NULL );
}
//
//bool CDclControlObject::UpdateGlobalVariable(CString sDclFormName, LPCTSTR pszProjectName /*= NULL*/)	
//{
//	RefCountedPtr< CPropertyObject > pPropVar = GetPropertyObject(nGlobalVarName);
//	RefCountedPtr< CPropertyObject > pPropName = GetPropertyObject(nName);
//	
//	// if no nGlobalVarName exists, we must exit here.
//	if (pPropVar == NULL)
//		return false;
//
//	CString sUnitled = _T("Untitled"); //should use empty string to represent untitled [ORW]
//	CString sVarName = pPropVar->GetStringValue();
//	if (sVarName.Left(1) == _T("_") || sVarName.Left(sUnitled.GetLength()) == sUnitled)
//	{
//		CString sProjectName( pszProjectName );
//		if (sProjectName.IsEmpty())
//			sProjectName = mpOwner->GetProject()->GetKeyName();
//
//		if (mType <= 0 || mType > CtlFileDlgCtrl) 
//			// create the name for a dialog box
//			pPropVar->SetStringValue(sProjectName + _T('_') + pPropName->GetStringValue());
//		else
//			// create the name for a control
//			pPropVar->SetStringValue(sProjectName + _T('_') + sDclFormName + _T('_') + pPropName->GetStringValue());
//		return true;
//	}
//	return false;
//}
//
//void CDclControlObject::ForceUpdateGlobalVariable(CString sDclFormName)	
//{
//	RefCountedPtr< CPropertyObject > pPropVar = GetPropertyObject(nGlobalVarName);
//	RefCountedPtr< CPropertyObject > pPropName = GetPropertyObject(nName);
//	
//	// if no nGlobalVarName exists, we must exit here.
//	if (pPropVar == NULL)
//		return;
//
//	if (mType <= 0 || mType > CtlFileDlgCtrl) 
//		// create the name for a dialog box
//		pPropVar->SetStringValue(mpOwner->GetProject()->GetKeyName() + _T('_') + pPropName->GetStringValue());
//	else	
//		// create the name for a control
//		pPropVar->SetStringValue(mpOwner->GetProject()->GetKeyName() + _T('_') + sDclFormName + _T('_') + pPropName->GetStringValue());	
//}

bool CDclControlObject::SetStringProperty( PropertyId nID, LPCTSTR pszValue )	
{
	RefCountedPtr< CPropertyObject > pProp = GetPropertyObject( nID );
	if( !pProp )
		return false;
	return pProp->SetStringValue( pszValue );
}

RefCountedPtr< CPropertyObject > CDclControlObject::AddStringProperty( PropertyId nID,
																																			 PropertyType type /*= PropString*/,
																																			 LPCTSTR pszValue /*= NULL*/,
																																			 bool bResetExisting /*= false*/ )
{
	RefCountedPtr< CPropertyObject > pProp = GetPropertyObject( nID );
	if( !pProp )
	{
		pProp = new CPropertyObject( type, 0, nID );
		mProperties.AddTail( pProp );
		pProp->SetStringValue( pszValue );
	}
	else if( bResetExisting )
	{
		pProp->SetType( type );
		pProp->SetStringValue( pszValue );
	}
	return pProp;
}

bool CDclControlObject::SetBooleanProperty( PropertyId nID, bool bValue /*= true*/ )	
{
	RefCountedPtr< CPropertyObject > pProp = GetPropertyObject( nID );
	if( !pProp )
		return false;
	return pProp->SetBooleanValue( bValue );
}

RefCountedPtr< CPropertyObject > CDclControlObject::AddBooleanProperty( PropertyId nID,
																																				PropertyType type /*= PropBool*/,
																																				bool bValue /*= true*/,
																																				bool bResetExisting /*= false*/ )
{
	RefCountedPtr< CPropertyObject > pProp = GetPropertyObject( nID );
	if( !pProp )
	{
		pProp = new CPropertyObject( type, 0, nID );
		mProperties.AddTail( pProp );
		pProp->SetBooleanValue( bValue );
	}
	else if( bResetExisting )
	{
		pProp->SetType( type );
		pProp->SetBooleanValue( bValue );
	}
	return pProp;
}

bool CDclControlObject::SetLongProperty( PropertyId nID, long lValue /*= -1*/ )	
{
	RefCountedPtr< CPropertyObject > pProp = GetPropertyObject( nID );
	if( !pProp )
		return false;
	return pProp->SetLongValue( lValue );
}

RefCountedPtr< CPropertyObject > CDclControlObject::AddLongProperty( PropertyId nID,
																																		 PropertyType type /*= PropLong*/,
																																		 long lValue /*= -1*/,
																																		 bool bResetExisting /*= false*/ )
{
	RefCountedPtr< CPropertyObject > pProp = GetPropertyObject( nID );
	if( !pProp )
	{
		pProp = new CPropertyObject( type, 0, nID );
		mProperties.AddTail( pProp );
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
	
	const_cast<CDclControlObject*>(this)->msAxTypeName = sName; //cache it
	return sName;
}

CString CDclControlObject::GetStrProperty(PropertyId nID) const
{
	POSITION posProp = mProperties.GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = mProperties.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
			return pProperty->GetStdProperty();
	}
	return CString();
}

long CDclControlObject::GetLngProperty(PropertyId nID) const
{
	POSITION posProp = mProperties.GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = mProperties.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
		{
			//some boolean properties are read and written as longs,
			//so that case gets special handling here [ORW]
			if (pProperty->GetType() == PropBool)
			{
				TraceFmt( _T("* 'PropLong' value requested from 'PropBool' property (ID: %d)! [%s]\r\n"), nID, _T(__FUNCTION__));
				return (long)pProperty->GetBooleanValue();
			}
			return pProperty->GetLongValue();
		}
	}
	return nNotSet;
}

void CDclControlObject::SetColorProperty(PropertyId nID, COLORREF color)
{
	POSITION posProp = mProperties.GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = mProperties.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
		{
			pProperty->SetOLEColorValue(color);
			return;
		}
	}
}

COLORREF CDclControlObject::GetColorProperty(PropertyId nID) const
{
	POSITION posProp = mProperties.GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = mProperties.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
			return GetRGBColor(pProperty->GetLongValue());
	}
	return RGB(0,0,0);
}
	
bool CDclControlObject::GetBoolProperty(PropertyId nID) const
{
	POSITION posProp = mProperties.GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = mProperties.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
		{
			//some enum properties are read and written as booleans,
			//so that case gets special handling here [ORW]
			if (pProperty->GetType() == PropEnum)
			{
				TraceFmt( _T("* 'PropBool' value requested from 'PropEnum' property (ID: %d)! [%s]\r\n"), nID, _T(__FUNCTION__));
				return (pProperty->GetEnumValue() != 0);
			}
			return pProperty->GetBooleanValue();
		}
	}
	return false;
}

short CDclControlObject::FindPropertyIndex(PropertyId nID) const
{
	// create a position variable to hold the counter increment
	POSITION pos;	
	int nCount;
	
	// set counter for Properties
	nCount = 0;

	// do loop to navigate Arx Controls	
	while (nCount < mProperties.GetCount())
	{
		// get position
		pos = mProperties.FindIndex(nCount);
		// get current PropertyObject
		RefCountedPtr< CPropertyObject > pProperty = mProperties.GetAt(pos);
		// if the name of property matches
		if (pProperty->GetID() == nID)
		{
			if (nID == nObjectBrowser && nCount > 4)
			{
				const_cast<CDclControlObject*>(this)->mProperties.RemoveAt(pos);
				pos = mProperties.GetHeadPosition();
				const_cast<CDclControlObject*>(this)->mProperties.InsertAfter(pos, pProperty);
				return 1;
			}
			if (nID == nGlobalVarName && nCount > 5)
			{
				const_cast<CDclControlObject*>(this)->mProperties.RemoveAt(pos);
				pos = mProperties.GetHeadPosition();
				mProperties.GetNext(pos);
				const_cast<CDclControlObject*>(this)->mProperties.InsertAfter(pos, pProperty);
				return 2;
			}

			// return the value
			return nCount;
		}
		// increment counter
		nCount++;
	}
	nCount = 0;
	//ASSERT(nCount == 0);
	
	return nNotSet;
}

RefCountedPtr< CPropertyObject > CDclControlObject::GetMethods() const
{ 
	POSITION posProp = mProperties.GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = mProperties.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetType() == PropActiveXMethods)
			return pProperty;
	}
	return NULL;
}

POSITION CDclControlObject::FindPropertyInsertPos( PropertyId nID, bool bHidden ) const
{
	if (nID == nObjectBrowser)
		return mProperties.GetHeadPosition();		
	return FindPropertyInsertPos(GetPropertyName(nID), bHidden);
}

POSITION CDclControlObject::FindPropertyInsertPos( LPCTSTR pszName, bool bHidden ) const
{
	assert( pszName && *pszName );
	if( !pszName || !*pszName )
		return NULL;

	POSITION pos = mProperties.GetTailPosition();
	while( pos )
	{
		POSITION posAt = pos;
		RefCountedPtr< CPropertyObject > pProp = mProperties.GetPrev( pos );
		if( pProp->IsHidden() != bHidden )
			continue;
		assert( pProp->GetName() != pszName ); //trying to insert a property with a duplicate name?
		if( pProp->GetName() < pszName )
			return posAt;
	}
	return NULL;
}

bool CDclControlObject::InsertNamedProperty( RefCountedPtr< CPropertyObject > pProp )
{
	assert( pProp != NULL );
	if( !pProp )
		return false;

	POSITION pos = FindPropertyInsertPos( pProp->GetName(), pProp->IsHidden() );
	if( pos )
		mProperties.InsertAfter( pos, pProp );
	else
		mProperties.AddTail( pProp );
	return true;
}

void CDclControlObject::ClearProperties()
{
#ifdef _DEBUG
	POSITION posProp = mProperties.GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = mProperties.GetNext(posProp);
		assert(pProperty != NULL);
		assert(!IsBadWritePtr(pProperty, sizeof(CPropertyObject)));
	}
#endif
	mProperties.RemoveAll();
}

void CDclControlObject::ResetProperty(PropertyId nId)
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for CPropertyObject
	int nCount = mProperties.GetCount()-1;

	while(nCount >= 0)
	{
		// get position
		pos = mProperties.FindIndex(nCount);
		
		// get current property
		RefCountedPtr< CPropertyObject > pPropObject = mProperties.GetAt(pos);
		if (pPropObject->GetID() == nId)
		{
			// clear any lists in this control
			pPropObject->SetStringValue(NULL);
			return;
		}
		// increment counter
		nCount--;
	}
}

void CDclControlObject::RemoveProperty(PropertyId nId)
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for CPropertyObject
	int nCount = mProperties.GetCount()-1;

	while(nCount >= 0)
	{
		// get position
		pos = mProperties.FindIndex(nCount);
		
		// get current property
		RefCountedPtr< CPropertyObject > pPropObject = mProperties.GetAt(pos);
		if (pPropObject->GetID() == nId)
		{
			mProperties.RemoveAt(pos);
			return;
		}
		// increment counter
		nCount--;
	}
}

void CDclControlObject::ClearR14Events()
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for CPropertyObject
	int nCount = mProperties.GetCount()-1;

	while(nCount >= 0)
	{
		// get position
		pos = mProperties.FindIndex(nCount);
		
		if (pos != NULL)
		{
			// get current property
			RefCountedPtr< CPropertyObject > pPropObject = mProperties.GetAt(pos);
			
			if (mType == CtlTextBox)
			{
				switch (pPropObject->GetID())
				{
				case nEventUpdate:
				case nEventMaxText:
				case nEventEditChanged:
				case nEventSetFocus:
				case nEventKeyDown:
				case nEventKeyUp:
				case nEventReturnPressed:
				case nEventInvoke:
					mProperties.RemoveAt(pos);
					break;
				}
			}
			switch (pPropObject->GetID())
			{
			case nDragnDropFromAutoCAD:
				mProperties.RemoveAt(pos);
				break;
			}
		}
		
		// increment counter
		nCount--;
	}
	////ASSERT(nCount == 0);
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
//#ifdef EDITOR
  // clear these temp storage variables
  m_rcOldPosition.SetRect(0,0,0,0);
//#endif

  if (!readString(sFile, msAxTypeName)) return statInvalidFormat;
	long lType;
  if (!readLong(sFile, lType)) return statInvalidFormat;
	mType = (ControlType)lType;
  if (!readShort(sFile, m_ClientHeight)) return statInvalidFormat;
  if (!readInt(sFile, m_PurchaseState)) return statInvalidFormat;
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

  mProperties.RemoveAll();

  // do loop to navigate objects
  while (nCount-- > 0)
  {
    // get current object
    RefCountedPtr< CPropertyObject > pProp = new CPropertyObject(PropInvalid);

    // get object from archive
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

    // add that ArxControlObject to the list object
    mProperties.AddTail(pProp);		
  }

  // here were are going to add the font of this object to the font collection
  // before any pictures are loaded or any dialogs are displayed.
  CFontCollection *pFontCol = &theWorkspace.GetFontCollection();
  pFontCol->GetFont(this, NULL);

//#ifndef EDITOR
  // set the position property pointers
  //m_pTabOrder = NULL;
  m_pVisible = GetPropertyObject(nVisible);

  m_pLeft = GetPropertyObject(nLeft);
  m_pTop = GetPropertyObject(nTop);
  m_pWidth = GetPropertyObject(nWidth);
  m_pHeight = GetPropertyObject(nHeight);

  m_pOffsetLeft = GetPropertyObject(nLeftFromRight);
  m_pOffsetTop = GetPropertyObject(nTopFromBottom);
  m_pOffsetRight = GetPropertyObject(nRightFromRight);
  m_pOffsetBottom = GetPropertyObject(nBottomFromBottom);

  m_pUseLeftOffset = GetPropertyObject(nUseLeftFromRight);
  m_pUseTopOffset = GetPropertyObject(nUseTopFromBottom);
  m_pUseRightOffset = GetPropertyObject(nUseRightFromRight);
  m_pUseBottomOffset = GetPropertyObject(nUseBottomFromBottom);
//#endif

  return statOK;
}

CString CDclControlObject::GetKeyName() const
{
	return GetStrProperty( nName );
}

CString CDclControlObject::GetKeyPath() const
{
	CString sPath = GetKeyName();
	if( mpOwner )
		sPath = mpOwner->GetKeyPath() + _T('_') + sPath;
	return sPath;
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
	for( INT_PTR idx = 0; idx < mProperties.GetCount(); ++idx )
	{
		CString sProp;
		sProp.Format( _T("  #%4s: %s\r\n"), asString( idx ), mProperties.GetAt( mProperties.FindIndex( idx ) )->toString() );
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
	for( INT_PTR idx = 0; idx < mProperties.GetCount(); ++idx )
		TraceFmt( _T("  #%4s: %s\r\n"), asString( idx ), mProperties.GetAt( mProperties.FindIndex( idx ) )->toString() );
}
#endif
