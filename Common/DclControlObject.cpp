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
	// find the insert position for this new property
	POSITION InsertPos = pControl->FindPropertyInsertPos(nID, false);

	// create new property object pointer to pass to AddTail method
	RefCountedPtr< CPropertyObject > pPropertyObect = new CPropertyObject( ValueType, 0, nID );

	// assign values
	pPropertyObect->SetStringValue(strValue);
	pPropertyObect->SetHidden(false);

	// reset the name to the new value
	if (InsertPos == NULL)
		pControl->GetPropertyList().AddTail(pPropertyObect);
	else
		pControl->GetPropertyList().InsertAfter(InsertPos, pPropertyObect);
}


/////////////////////////////////////////////////////////////////////////////
// CDclControlObject

IMPLEMENT_SERIAL(CDclControlObject, CObject, 1)

CDclControlObject::CDclControlObject()
: mpOwner( NULL )
, mpDlgControl( NULL )
{
	m_nTotalBytes = 0;
	m_rcOldPosition.SetRect(0,0,0,0);
	mType = CtlFormPropHolder;
	m_Delete = FALSE;
	m_pCtrlHolder = NULL;
	m_sLicenseKey.Empty();
	m_sBaseCode.Empty();
	m_bLicenseChecked = FALSE;
	::memset(&m_clsid, 0, sizeof(GUID));	
	m_pStream = NULL;	
	m_pImageList = NULL;

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
	m_Id = -1;
}

CDclControlObject::CDclControlObject(CDclFormObject* pOwner)
: mpOwner( pOwner )
, mpDlgControl( NULL )
{
	m_nTotalBytes = 0;
	m_rcOldPosition.SetRect(0,0,0,0);
	mType = CtlFormPropHolder;
	m_Delete = FALSE;
	m_pCtrlHolder = NULL;
	m_sLicenseKey.Empty();
	m_sBaseCode.Empty();
	m_bLicenseChecked = FALSE;
	::memset(&m_clsid, 0, sizeof(GUID));	
	m_pStream = NULL;	
	m_pImageList = NULL;

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
	m_Id = -1;
}

CDclControlObject::CDclControlObject(ControlType type, CDclFormObject* pOwner, LPCTSTR pszName /*= NULL*/)
: mpOwner( pOwner )
, mType( type )
, mpDlgControl( NULL )
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
	m_pImageList = NULL;

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
	m_Id = -1;
}

CDclControlObject::~CDclControlObject()
{
	ClearProperties();
	ClearStream();
}

//CDclControlObject::CDclControlObject(CDclControlObject const & other) 
//: mpOwner( other.GetOwnerForm() )
//{
//	m_rcOldPosition.SetRect(0,0,0,0);
//	m_nIsMicrosoftActiveX = other.m_nIsMicrosoftActiveX;
//	m_AxTypeName = other.m_AxTypeName;
//    m_Id = other.m_Id;
//	m_Name = other.m_Name;
//    mType = other.mType;
//	m_clsid = other.m_clsid;
//	m_sBaseCode = other.m_sBaseCode;
//	m_sLicenseKey = other.m_sLicenseKey;
//	m_bLicenseChecked = other.m_bLicenseChecked;
//	m_nTotalBytes = other.m_nTotalBytes;
//	m_pImageList = new CImageListObject;
//	m_pImageList->Copy(other.m_pImageList);	
//	POSITION pos;
//	INT_PTR nCount = other.GetPropertyList().GetCount()-1;
//	while (nCount >= 0)
//	{
//		pos = other.GetPropertyList().FindIndex(nCount);
//		GetPropertyList().AddTail(other.GetPropertyList().GetAt(pos));
//		nCount--;
//	}
//}
//
//CDclControlObject CDclControlObject::operator=(CDclControlObject const & other) 
//{
//	mpOwner = other.GetOwnerForm();
//	m_rcOldPosition.SetRect(0,0,0,0);
//	m_nIsMicrosoftActiveX = other.m_nIsMicrosoftActiveX;
//	m_AxTypeName = other.m_AxTypeName;
//    m_Id = other.m_Id;
//	m_Name = other.m_Name;
//    mType = other.mType;
//
//	m_clsid = other.m_clsid;
//	m_sBaseCode = other.m_sBaseCode;
//	m_sLicenseKey = other.m_sLicenseKey;
//	m_bLicenseChecked = other.m_bLicenseChecked;
//	m_nTotalBytes = other.m_nTotalBytes;
//	m_pImageList = new CImageListObject;
//	m_pImageList->Copy(other.m_pImageList);	
//	POSITION pos;
//	INT_PTR nCount = other.GetPropertyList().GetCount()-1;
//	while (nCount >= 0)
//	{
//		
//		RefCountedPtr< CPropertyObject > pDestProp = new CPropertyObject;
//		pos = other.GetPropertyList().FindIndex(nCount);
//		RefCountedPtr< CPropertyObject > pSourceProp = other.GetPropertyList().GetAt(pos);
//		pDestProp->Copy(pSourceProp);
//		GetPropertyList().AddTail(pDestProp);
//		nCount--;
//	}
//
//    return *this;
//}
//
//void CDclControlObject::Copy(CDclControlObject *other) 
//{
//	m_rcOldPosition.SetRect(0,0,0,0);
//	m_nIsMicrosoftActiveX = other->m_nIsMicrosoftActiveX;
//	m_AxTypeName = other->m_AxTypeName;
//    m_Id = other->m_Id;
//	m_Name = other->m_Name;
//    mType = other->mType;
//
//	m_clsid = other->m_clsid;
//	m_sBaseCode = other->m_sBaseCode;
//	m_sLicenseKey = other->m_sLicenseKey;
//	m_bLicenseChecked = other->m_bLicenseChecked;
//	m_nTotalBytes = other->m_nTotalBytes;
//	m_pImageList = new CImageListObject;
//	m_pImageList->Copy(other->m_pImageList);
//	POSITION pos;
//	int nCount = 0;
//
//	while (nCount < other->GetPropertyList().GetCount())
//	{		
//		RefCountedPtr< CPropertyObject > pDestProp = new CPropertyObject;
//		pos = other->GetPropertyList().FindIndex(nCount);
//		RefCountedPtr< CPropertyObject > pSourceProp = other->GetPropertyList().GetAt(pos);
//		pDestProp->Copy(pSourceProp);
//		GetPropertyList().AddTail(pDestProp);
//		nCount++;
//	}
//
//}


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

  // just added.
  writeULONG(pFile, ULONG(GetControlInstance()->GetControlId()));

  // serialize the image if it exists
  if (m_pImageList != NULL)
  {
    bImageList = TRUE;
    writeBOOL(pFile, bImageList);	
    m_pImageList->WriteToTextFile(pFile, fileName);
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
  nCount = (int)GetPropertyList().GetCount();

  writeInt(pFile, nCount);

  // set start position for navigating objects
  pos = GetPropertyList().GetHeadPosition();
  // do loop to navigate objects
  while (pos != NULL)
  {
    // get current object
    RefCountedPtr< CPropertyObject > pProp = GetPropertyList().GetNext(pos);

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

		// just added.
		ar << m_Id;

		// serialize the image if it exists
		ar << BOOL(m_pImageList != NULL);
		if (m_pImageList != NULL)
			m_pImageList->Serialize(ar);
			
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
		ar << (WORD)GetPropertyList().GetCount();
		pos = GetPropertyList().GetHeadPosition();
		while (pos != NULL)
		{
			RefCountedPtr< CPropertyObject > pProp = GetPropertyList().GetNext(pos);
			pProp->Serialize(ar);
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
			ar >> m_Id;
		delete m_pImageList;
		m_pImageList = NULL;
		if (nThisVersion >= 5)
		{
			ar >> bImageList;
			if (bImageList == TRUE)
			{
				m_pImageList = new CImageListObject();
				m_pImageList->Serialize(ar);				
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
			GetPropertyList().RemoveAll();
			while (nCount-- > 0)
			{
				RefCountedPtr< CPropertyObject > pProp = new CPropertyObject( PropInvalid );
				pProp->Serialize(ar);
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
				//TraceFmt( _T("Read property [%08X, type = %d, id = %d], string value = %s\r\n"),
				//					(ULONG)&*pProp, (int)pProp->GetType(), (int)pProp->GetID(), (LPCTSTR)pProp->GetStringValue() );
				GetPropertyList().AddTail(pProp);		
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
		GetPropertyList().Serialize(ar);
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
		INT_PTR nCount = GetPropertyList().GetCount() - 1;
		while (nCount > 0)
		{		
			pos = GetPropertyList().FindIndex(nCount);
			RefCountedPtr< CPropertyObject > pSourceProp = GetPropertyList().GetAt(pos);
			
			
			if (mType == CtlGrid)
			{
				if (pSourceProp->GetID() == nDragnDropToAutoCAD)
					GetPropertyList().RemoveAt(pos);
				if (pSourceProp->GetID() == nDragnDropFromControl)
					GetPropertyList().RemoveAt(pos);
				if (pSourceProp->GetID() == nDragnDropFromAutoCAD)
					GetPropertyList().RemoveAt(pos);
				if (pSourceProp->GetID() == nDragnDropBegin)
					GetPropertyList().RemoveAt(pos);
				if (pSourceProp->GetID() == nDragnDropAllowBegin)
					GetPropertyList().RemoveAt(pos);
				if (pSourceProp->GetID() == nDragnDropAllowDrop)
					GetPropertyList().RemoveAt(pos);
				if (pSourceProp->GetID() == nEventReturnPressed)
					GetPropertyList().RemoveAt(pos);
			}
			
			if (pSourceProp->GetID() == nAcadColor && mType == CtlFrame)
				GetPropertyList().RemoveAt(pos);
			if (pSourceProp->GetID() == nForeColor && mType == CtlFrame)
				GetPropertyList().RemoveAt(pos);
			if (pSourceProp->GetID() == nTabOrder)
				GetPropertyList().RemoveAt(pos);
			if (pSourceProp->GetID() == nLabelColor)
				GetPropertyList().RemoveAt(pos);
			if (pSourceProp->GetID() == nEventDblClicked && mType == CtlStdButton)
				GetPropertyList().RemoveAt(pos);
			if (pSourceProp->GetID() == nEventDblClicked && mType == CtlGraphicButton)
				GetPropertyList().RemoveAt(pos);
			
			/*if (pSourceProp->GetID() == nComboBoxStyle && mType == CtlComboBox)
					pSourceProp->GetLongValue() = 2;*/
			if (pSourceProp->GetID() == nHScrollBar && mType == CtlListBox)
				GetPropertyList().RemoveAt(pos);
			if (pSourceProp->GetID() == nAllowOrbiting && pSourceProp->GetType() == PropBool)
				GetPropertyList().RemoveAt(pos);
			
			if (pSourceProp->GetID() == nToolTipText && mType == CtlLabel)
				GetPropertyList().RemoveAt(pos);
			
			if (pSourceProp->GetID() == nTabLabelAlign)
				GetPropertyList().RemoveAt(pos);
			if (pSourceProp->GetID() == nTabSelected)
				GetPropertyList().RemoveAt(pos);
			
			nCount--;
		}
	}
	
}

size_t CDclControlObject::CountPropertyListItems(PropertyId nID)
{
	POSITION pos = GetPropertyList().GetHeadPosition();	
	while (pos)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(pos);
		if (pProperty->GetID() == nID)
			return pProperty->size(); // return the value
	}
	return 0;
}

CString CDclControlObject::GetPropertyListItem(PropertyId nID, size_t nIndex)
{
	POSITION pos = GetPropertyList().GetHeadPosition();	
	while (pos)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(pos);
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

RefCountedPtr< CPropertyObject > CDclControlObject::GetPropertyObject(PropertyId nID) const
{
	POSITION posProp = GetPropertyList().GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(posProp);
		if (pProperty && pProperty->GetID() == nID)
			return pProperty;
	}
	return NULL;
}

RefCountedPtr< CPropertyObject > CDclControlObject::GetActiveXPropertyObject(CString sName) const
{
	POSITION posProp = GetPropertyList().GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(posProp);
		if (pProperty && pProperty->GetName() == sName)
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
		GetPropertyList().AddTail( pProp );
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
		GetPropertyList().AddTail( pProp );
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
		GetPropertyList().AddTail( pProp );
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
	POSITION posProp = GetPropertyList().GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
			return pProperty->GetStdProperty();
	}
	return CString();
}

long CDclControlObject::GetImageListIndex()
{
	POSITION posProp = GetPropertyList().GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nImageList)
			return pProperty->GetShortValue();
	}
	return nNotSet;
}


void CDclControlObject::SetImageListIndex(PropertyId nIndex)
{	
	POSITION posProp = GetPropertyList().GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nImageList)
		{
			pProperty->SetShortValue(nIndex);
			return;
		}
	}
}


long CDclControlObject::GetLngProperty(PropertyId nID) const
{
	POSITION posProp = GetPropertyList().GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(posProp);
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
	POSITION posProp = GetPropertyList().GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(posProp);
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
	POSITION posProp = GetPropertyList().GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
			return GetRGBColor(pProperty->GetLongValue());
	}
	return RGB(0,0,0);
}
	
bool CDclControlObject::GetBoolProperty(PropertyId nID) const
{
	POSITION posProp = GetPropertyList().GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
		{
			//some boolean properties are read and written as enums,
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
	while (nCount < GetPropertyList().GetCount())
	{
		// get position
		pos = GetPropertyList().FindIndex(nCount);
		// get current PropertyObject
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetAt(pos);
		// if the name of property matches
		if (pProperty->GetID() == nID)
		{
			if (nID == nObjectBrowser && nCount > 4)
			{
				const_cast<CDclControlObject*>(this)->GetPropertyList().RemoveAt(pos);
				pos = GetPropertyList().GetHeadPosition();
				const_cast<CDclControlObject*>(this)->GetPropertyList().InsertAfter(pos, pProperty);
				return 1;
			}
			if (nID == nGlobalVarName && nCount > 5)
			{
				const_cast<CDclControlObject*>(this)->GetPropertyList().RemoveAt(pos);
				pos = GetPropertyList().GetHeadPosition();
				GetPropertyList().GetNext(pos);
				const_cast<CDclControlObject*>(this)->GetPropertyList().InsertAfter(pos, pProperty);
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

RefCountedPtr< CPropertyObject > CDclControlObject::FindProperty(CString sName)
{
	POSITION posProp = GetPropertyList().GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetName() == sName)
			return pProperty;
	}
	return NULL;
}
RefCountedPtr< CPropertyObject > CDclControlObject::GetMethods() { 
	POSITION posProp = GetPropertyList().GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetType() == PropActiveXMethods)
			return pProperty;
	}
	return NULL;
}
POSITION CDclControlObject::FindPropertyInsertPos(PropertyId nID, bool bHidden) const
{
	if (nID == nObjectBrowser)
		return GetPropertyList().GetHeadPosition();		
	return FindPropertyInsertPos(GetPropertyName(nID), bHidden);
}

POSITION CDclControlObject::FindPropertyInsertPos(CString sName, bool bHidden) const
{
	// create a position variable to hold the counter increment
	POSITION pos;	
	CString ThisPropText;
	CString QueryPropText = sName;
	
	// set counter for Properties begining at the end of the list
	INT_PTR nCount = GetPropertyList().GetCount() - 1;
	
	if (nCount == nNotSet)
		return NULL;

	// do loop to navigate Arx Controls	
	while (nCount >= 0)
	{
		// get position
		pos = GetPropertyList().FindIndex(nCount);
		if (pos == NULL)
			return NULL;

		// get current PropertyObject
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetAt(pos);
		
		ThisPropText = pProperty->GetName();
		// if the name of property matches
		if (ThisPropText < QueryPropText &&
			pProperty->IsHidden() == bHidden &&
			!ThisPropText.IsEmpty())
		{
			// return the value
			return pos;
		}
		// increment counter
		nCount--;
	}
	nCount = 0;
	
	return NULL;
}

void CDclControlObject::ClearProperties()
{
	// delete the image list item if it exists
	if (m_pImageList != NULL)
	{
		//delete m_pImageList; //causes exception when unloading grid with images -- I didn't check why, just commented this [ORW]
		m_pImageList = NULL;
	}

#ifdef _DEBUG
	POSITION posProp = GetPropertyList().GetHeadPosition();
	while (posProp)
	{
		RefCountedPtr< CPropertyObject > pProperty = GetPropertyList().GetNext(posProp);
		assert(pProperty != NULL);
		assert(!IsBadWritePtr(pProperty, sizeof(CPropertyObject)));
	}
#endif
	GetPropertyList().RemoveAll();
}

void CDclControlObject::ResetProperty(PropertyId nId)
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for CPropertyObject
	int nCount = GetPropertyList().GetCount()-1;

	while(nCount >= 0)
	{
		// get position
		pos = GetPropertyList().FindIndex(nCount);
		
		// get current property
		RefCountedPtr< CPropertyObject > pPropObject = GetPropertyList().GetAt(pos);
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
	int nCount = GetPropertyList().GetCount()-1;

	while(nCount >= 0)
	{
		// get position
		pos = GetPropertyList().FindIndex(nCount);
		
		// get current property
		RefCountedPtr< CPropertyObject > pPropObject = GetPropertyList().GetAt(pos);
		if (pPropObject->GetID() == nId)
		{
			GetPropertyList().RemoveAt(pos);
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
	int nCount = GetPropertyList().GetCount()-1;

	while(nCount >= 0)
	{
		// get position
		pos = GetPropertyList().FindIndex(nCount);
		
		if (pos != NULL)
		{
			// get current property
			RefCountedPtr< CPropertyObject > pPropObject = GetPropertyList().GetAt(pos);
			
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
					GetPropertyList().RemoveAt(pos);
					break;
				}
			}
			switch (pPropObject->GetID())
			{
			case nDragnDropFromAutoCAD:
				GetPropertyList().RemoveAt(pos);
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

	ULONG ulID;
  if (!readInt(sFile, (int&)ulID)) return statInvalidFormat;
	m_Id = UINT(ulID);

	delete m_pImageList;
	m_pImageList = NULL;
  bool bImageList;
  if (!readBool(sFile, bImageList)) return statInvalidFormat;
  if (bImageList) {
    m_pImageList = new CImageListObject();
		IOStatus stat = m_pImageList->ReadFromTextFile(sFile, fileName);
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

  GetPropertyList().RemoveAll();

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
    GetPropertyList().AddTail(pProp);		
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
void CDclControlObject::dump( bool bDeep /*= true*/ ) const
{
	CString sInstance;
	if( mpDlgControl )
		sInstance.Format( _T(" (DlgControl: %s)"), asString( mpDlgControl ) );
	CString sOut;
	sOut.Format( _T("CDclControlobject [%s: %s%s]\r\n"), asString( mType ), GetKeyPath(), (LPCTSTR)sInstance );
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
	CString sInstance;
	if( mpDlgControl )
		sInstance.Format( _T(" (DlgControl: %s)"), asString( mpDlgControl ) );
	TraceFmt( _T("CDclControlobject [%s: %s%s]\r\n"), asString( mType ), GetKeyPath(), (LPCTSTR)sInstance );
	if( !bDeep )
		return;
	for( INT_PTR idx = 0; idx < mProperties.GetCount(); ++idx )
		TraceFmt( _T("  #%4s: %s\r\n"), asString( idx ), mProperties.GetAt( mProperties.FindIndex( idx ) )->toString() );
}
#endif
