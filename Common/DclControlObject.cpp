// DclControlObject.cpp : implementation file
//

#include "stdafx.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ImageListObject.h"
#include "AxContainer.h"
#include "Filing.h"
#include "PurchaseMode.h"
#include "Workspace.h"
#include "Project.h"
#include "PropertyIds.h"
#include "ControlTypes.h"


static const int nNotSet = -1;


static void AddControlProperty(CDclControlObject *pControl, int nID, LPCTSTR strValue, PropertyTypes ValueType)
{
	// find the insert position for this new property
	POSITION InsertPos = pControl->FindPropertyInsertPos(nID, false);

	// create new property object pointer to pass to AddTail method
	CPropertyObject* pPropertyObect = new CPropertyObject;

	// assign values
	pPropertyObect->SetID(nID);
	pPropertyObect->AddProperty(ValueType, strValue);
	pPropertyObect->SetHidden(false);

	// reset the name to the new value
	if (InsertPos == NULL)
		pControl->m_PropertyList.AddTail(pPropertyObect);
	else
		pControl->m_PropertyList.InsertAfter(InsertPos, pPropertyObect);
}


/////////////////////////////////////////////////////////////////////////////
// CDclControlObject

IMPLEMENT_SERIAL(CDclControlObject, CObject, 1)

CDclControlObject::CDclControlObject()
: mpOwner( NULL )
{
	m_nTotalBytes = 0;
	m_nIsMicrosoftActiveX = nNotSet;
	m_AxTypeName.Empty();
	m_rcOldPosition.SetRect(0,0,0,0);
	mType = CtlFormPropHolder;
	m_Id = nNotSet;
	m_Delete = FALSE;
	m_bControlCreated = false;
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
	m_pWnd = NULL;
}

CDclControlObject::CDclControlObject(CDclFormObject* pOwner)
: mpOwner( pOwner )
{
	m_nTotalBytes = 0;
	m_nIsMicrosoftActiveX = nNotSet;
	m_AxTypeName.Empty();
	m_rcOldPosition.SetRect(0,0,0,0);
	mType = CtlFormPropHolder;
	m_Id = nNotSet;
	m_Delete = FALSE;
	m_bControlCreated = false;
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
	m_pWnd = NULL;
}

CDclControlObject::CDclControlObject(ControlTypes type, CDclFormObject* pOwner, LPCTSTR pszName /*= NULL*/)
: mpOwner( pOwner )
, mType( type )
{
	if( pszName )
		SetStrProperty(nName, pszName);
	m_nTotalBytes = 0;
	m_nIsMicrosoftActiveX = nNotSet;
	m_AxTypeName.Empty();
	m_rcOldPosition.SetRect(0,0,0,0);
	m_Id = nNotSet;
	m_Delete = FALSE;
	m_PropertyList.RemoveAll();
	m_bControlCreated = false;
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
	m_pWnd = NULL;
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
//	INT_PTR nCount = other.m_PropertyList.GetCount()-1;
//	while (nCount >= 0)
//	{
//		pos = other.m_PropertyList.FindIndex(nCount);
//		m_PropertyList.AddTail(other.m_PropertyList.GetAt(pos));
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
//	INT_PTR nCount = other.m_PropertyList.GetCount()-1;
//	while (nCount >= 0)
//	{
//		
//		CPropertyObject *pDestProp = new CPropertyObject;
//		pos = other.m_PropertyList.FindIndex(nCount);
//		CPropertyObject *pSourceProp = other.m_PropertyList.GetAt(pos);
//		pDestProp->Copy(pSourceProp);
//		m_PropertyList.AddTail(pDestProp);
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
//	while (nCount < other->m_PropertyList.GetCount())
//	{		
//		CPropertyObject *pDestProp = new CPropertyObject;
//		pos = other->m_PropertyList.FindIndex(nCount);
//		CPropertyObject *pSourceProp = other->m_PropertyList.GetAt(pos);
//		pDestProp->Copy(pSourceProp);
//		m_PropertyList.AddTail(pDestProp);
//		nCount++;
//	}
//
//}

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
  writeString(pFile, m_Name);
  writeLong(pFile, mType);
  writeShort(pFile, m_ClientHeight);
  //m_PurchaseState = nCurrentPurchaseMode;
  writeInt(pFile, m_PurchaseState);

  // just added.
  writeInt(pFile, m_Id);

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
  nCount = (int)m_PropertyList.GetCount();

  writeInt(pFile, nCount);

  // set start position for navigating objects
  pos = m_PropertyList.GetHeadPosition();
  // do loop to navigate objects
  while (pos != NULL)
  {
    // get current object
    CPropertyObject* pProp = m_PropertyList.GetNext(pos);

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
		ar << m_Name;
		ar << long(mType);
		ar << m_ClientHeight;
		m_PurchaseState = nCurrentPurchaseMode;
		ar << m_PurchaseState;

		// just added.
		ar << m_Id;

		// serialize the image if it exists
		if (m_pImageList != NULL)
		{
			bImageList = TRUE;
			ar << bImageList;					
			m_pImageList->Serialize(ar);
		}
		else
		{
			bImageList = FALSE;			
			ar << bImageList;		
		}
			
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
		// set counter for ArxControls
		nCount = (WORD)m_PropertyList.GetCount();

		ar << nCount;

		// set start position for navigating objects
		pos = m_PropertyList.GetHeadPosition();
		// do loop to navigate objects
		while (pos != NULL)
		{
			// get current object
			CPropertyObject* pProp = m_PropertyList.GetNext(pos);

			// put object into archive
			pProp->Serialize(ar);
			
			// increment counter
			nCount--;
		}
	}
	else
	{
		// clear these temp storage variables
		ar >> nThisVersion;
		if (nThisVersion > GetCurrentSaveVersion())
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );

		m_rcOldPosition.SetRect(0,0,0,0);
		m_AxTypeName.Empty();
		m_nIsMicrosoftActiveX = nNotSet;
		ar >> m_Name;
		long lType;
		ar >> lType;
		mType = (ControlTypes)lType;
		ar >> m_ClientHeight;
		ar >> m_PurchaseState;

		if (nThisVersion >= 6)
		{
			ar >> m_Id;
		}
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
					{
						m_pStream = NULL;
					}
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

			m_PropertyList.RemoveAll();
			
			// do loop to navigate objects
			while (nCount-- > 0)
			{
				// get current object
				CPropertyObject* pProp = new CPropertyObject;
				
				// get object from archive
				pProp->Serialize(ar);
		
				// add that ArxControlObject to the list object
				m_PropertyList.AddTail(pProp);		
			}

			// here were are going to add the font of this object to the font collection
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
		m_PropertyList.Serialize(ar);
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
		INT_PTR nCount = m_PropertyList.GetCount() - 1;
		while (nCount > 0)
		{		
			pos = m_PropertyList.FindIndex(nCount);
			CPropertyObject *pSourceProp = m_PropertyList.GetAt(pos);
			
			
			if (mType == CtlGrid)
			{
				if (pSourceProp->GetID() == nDragnDropToAutoCAD)
				{
					m_PropertyList.RemoveAt(pos);
					delete pSourceProp;					
				}
				if (pSourceProp->GetID() == nDragnDropFromControl)
				{
					m_PropertyList.RemoveAt(pos);
					delete pSourceProp;					
				}
				if (pSourceProp->GetID() == nDragnDropFromAutoCAD)
				{
					m_PropertyList.RemoveAt(pos);
					delete pSourceProp;					
				}
				if (pSourceProp->GetID() == nDragnDropBegin)
				{
					m_PropertyList.RemoveAt(pos);
					delete pSourceProp;					
				}
				if (pSourceProp->GetID() == nDragnDropAllowBegin)
				{
					m_PropertyList.RemoveAt(pos);
					delete pSourceProp;					
				}
				if (pSourceProp->GetID() == nDragnDropAllowDrop)
				{
					m_PropertyList.RemoveAt(pos);
					delete pSourceProp;					
				}
				if (pSourceProp->GetID() == nEventReturnPressed)
				{
					m_PropertyList.RemoveAt(pos);
					delete pSourceProp;					
				}
			}
			
			if (pSourceProp->GetID() == nAcadColor && mType == CtlFrame)
			{
				m_PropertyList.RemoveAt(pos);
				delete pSourceProp;					
			}
			if (pSourceProp->GetID() == nForeColor && mType == CtlFrame)
			{
				m_PropertyList.RemoveAt(pos);
				delete pSourceProp;					
			}
			if (pSourceProp->GetID() == nTabOrder)
			{
				m_PropertyList.RemoveAt(pos);
				delete pSourceProp;					
			}
			if (pSourceProp->GetID() == nLabelColor)
			{
				m_PropertyList.RemoveAt(pos);
				delete pSourceProp;					
			}
			if (pSourceProp->GetID() == nEventDblClicked && mType == CtlStdButton)
			{
				m_PropertyList.RemoveAt(pos);
				delete pSourceProp;					
			}
			if (pSourceProp->GetID() == nEventDblClicked && mType == CtlGraphicButton)
			{
				m_PropertyList.RemoveAt(pos);
				delete pSourceProp;					
			}
			
			/*if (pSourceProp->GetID() == nComboBoxStyle && mType == CtlComboBox)
			{
				pSourceProp->GetLongValue() = 2
			}*/
			if (pSourceProp->GetID() == nHScrollBar && mType == CtlListBox)
			{
				m_PropertyList.RemoveAt(pos);
				delete pSourceProp;					
			}
			if (pSourceProp->GetID() == nAllowOrbiting &&
				pSourceProp->GetType() == PropBool
				)
			{
				m_PropertyList.RemoveAt(pos);
				delete pSourceProp;					
			}
			
			if (pSourceProp->GetID() == nToolTipText && mType == CtlLabel)
			{
				m_PropertyList.RemoveAt(pos);
				delete pSourceProp;					
			}
			
			if (pSourceProp->GetID() == nTabLabelAlign)
			{
				m_PropertyList.RemoveAt(pos);
				delete pSourceProp;					
			}
			if (pSourceProp->GetID() == nTabSelected)
			{
				m_PropertyList.RemoveAt(pos);
				delete pSourceProp;					
			}
			
			nCount--;
		}
	}
	
}

INT_PTR CDclControlObject::CountPropertyListItems(int nID)
{
	// create a position variable to hold the counter increment
	POSITION pos;	
	int nCount;
	
	// set counter for Properties
	nCount = 0;

	// set start position for navigating Properties
	pos = m_PropertyList.GetHeadPosition();

	// do loop to navigate Arx Controls	
	while (nCount < m_PropertyList.GetCount())
	{
		// get position
		pos = m_PropertyList.FindIndex(nCount);
		// get current PropertyObject
		CPropertyObject* pProperty = m_PropertyList.GetNext(pos);
		// if the name of property matches
		if (pProperty->GetID() == nID)
		{
			// return the value			
			if (pProperty->m_intList.GetSize() > 0)
				return pProperty->m_intList.GetSize();
			else
				return pProperty->m_stringList.GetCount();
		}
		// increment counter
		nCount++;
	}
	nCount = 0;
	////ASSERT(nCount == 0);
	return nNotSet;
}

CString CDclControlObject::GetPropertyListItem(int nID, int nIndex)
{
	// create a position variable to hold the counter increment
	POSITION pos;	
	int nCount;
	
	// set counter for Properties
	nCount = 0;

	// set start position for navigating Properties
	pos = m_PropertyList.GetHeadPosition();

	// do loop to navigate Arx Controls	
	while (nCount < m_PropertyList.GetCount())
	{
		// get position
		pos = m_PropertyList.FindIndex(nCount);
		// get current PropertyObject
		CPropertyObject* pProperty = m_PropertyList.GetNext(pos);
		// if the name of property matches
		if (pProperty->GetID() == nID)
		{
			if (pProperty->m_intList.GetSize() > 0)
			{
				// return the value			
				CString sValue;
				sValue.Format(_T("%d"), pProperty->m_intList[nIndex]);
				return sValue;
			}
			if (pProperty->m_stringList.GetCount() > 0)
			{
				// return the value			
				POSITION pos = pProperty->m_stringList.FindIndex(nIndex);
				return pProperty->m_stringList.GetAt(pos);
			}
		}
		// increment counter
		nCount++;
	}
	nCount = 0;
	////ASSERT(nCount == 0);
	return CString();
}

CPropertyObject* CDclControlObject::GetPropertyObject(int nID) const
{
	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
		if (pProperty && pProperty->GetID() == nID)
			return pProperty;
	}
	return NULL;
}

CPropertyObject* CDclControlObject::GetActiveXPropertyObject(CString sName) const
{
	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
		if (pProperty && pProperty->GetName() == sName)
			return pProperty;
	}
	return NULL;
}

bool CDclControlObject::UpdateGlobalVariable(CString sDclFormName, LPCTSTR pszProjectName /*= NULL*/)	
{
	CPropertyObject *pPropVar = GetPropertyObject(nGlobalVarName);
	CPropertyObject *pPropName = GetPropertyObject(nName);
	
	// if not nGlobalVarName exists, we must exit here.
	if (pPropVar == NULL)
		return false;

	CString sUnitled = _T("Untitled"); //should use empty string to represent untitled [ORW]
	if (pPropVar->GetStringValue().Left(sUnitled.GetLength()) == sUnitled)
	{
		CString sProjectName( pszProjectName );
		if (sProjectName.IsEmpty())
			sProjectName = activeProject->GetBaseName();

		if (mType <= 0 || mType > CtlFileDlgCtrl) 
			// create the name for a dialog box
			pPropVar->SetStringValue(sProjectName + _T('_') + pPropName->GetStringValue());
		else
			// create the name for a control
			pPropVar->SetStringValue(sProjectName + _T('_') + sDclFormName + _T('_') + pPropName->GetStringValue());
		return true;
	}
	return false;
}

void CDclControlObject::ForceUpdateGlobalVariable(CString sDclFormName)	
{
	CPropertyObject *pPropVar = GetPropertyObject(nGlobalVarName);
	CPropertyObject *pPropName = GetPropertyObject(nName);
	
	// if no nGlobalVarName exists, we must exit here.
	if (pPropVar == NULL)
		return;

	if (mType <= 0 || mType > CtlFileDlgCtrl) 
		// create the name for a dialog box
		pPropVar->SetStringValue(activeProject->GetKeyName() + _T('_') + pPropName->GetStringValue());
	else	
		// create the name for a control
		pPropVar->SetStringValue(activeProject->GetKeyName() + _T('_') + sDclFormName + _T('_') + pPropName->GetStringValue());	
}

void CDclControlObject::SetStrProperty(int nID, CString sValue)	
{
	// create a position variable to hold the counter increment
	POSITION pos;	
	int nCount;
	
	// set counter for Properties
	nCount = 0;

	// set start position for navigating Properties
	pos = m_PropertyList.GetHeadPosition();

	// do loop to navigate Arx Controls	
	while (nCount < m_PropertyList.GetCount())
	{
		// get position
		pos = m_PropertyList.FindIndex(nCount);
		// get current PropertyObject
		CPropertyObject* pProperty = m_PropertyList.GetNext(pos);
		// if the name of property matches
		if (pProperty->GetID() == nID)
		{
			// set the value			
			pProperty->SetStringValue(sValue);
			return;
		}
		// increment counter
		nCount++;
	}
	nCount = 0;
	////ASSERT(nCount == 0);

}

BOOL CDclControlObject::IsMicrosoftActiveXCtrl() const
{	
	CString sName;
	
	if (m_nIsMicrosoftActiveX == 1)
		return TRUE;
	if (m_nIsMicrosoftActiveX == 0)
		return TRUE;

	try
	{
		// get the ProgId
		LPOLESTR olestr;	
		HRESULT hResult = OleRegGetUserType( m_clsid, USERCLASSTYPE_FULL, &olestr );
		sName = olestr;
	}
	catch(...)
	{
		return FALSE;
	}

	const_cast<CDclControlObject*>(this)->m_nIsMicrosoftActiveX = ((sName.Left(9) == _T("Microsoft")));
	return m_nIsMicrosoftActiveX;
}

CString CDclControlObject::GetActiveXTypeName() const
{
	CString sName;
	
	if (!m_AxTypeName.IsEmpty())
		return m_AxTypeName;

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
	sName = sName.SpanIncluding(_T("0123456789."));
	sName = sName.SpanExcluding(_T("."));
	sName.MakeReverse();
	
	const_cast<CDclControlObject*>(this)->m_AxTypeName = sName;
	return sName;
}

CString CDclControlObject::GetStrProperty(int nID) const
{
	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
			return pProperty->GetStdProperty();
	}
	return CString();
}

long CDclControlObject::GetImageListIndex()
{
	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nImageList)
			return pProperty->m_ImageListIndex;
	}
	return nNotSet;
}


void CDclControlObject::SetImageListIndex(int nIndex)
{	
	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nImageList)
		{
			pProperty->m_ImageListIndex = nIndex;
			return;
		}
	}
}


long CDclControlObject::GetLngProperty(int nID) const
{
	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
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

void CDclControlObject::SetLngProperty(int nID, long lValue)
{
	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
		{
			//some boolean properties are read and written as longs,
			//so that case gets special handling here [ORW]
			if (pProperty->GetType() == PropBool)
			{
				TraceFmt( _T("* 'PropLong' value written to 'PropBool' property (ID: %d)! [%s]\r\n"), nID, _T(__FUNCTION__));
				return pProperty->SetBooleanValue(lValue != 0);
			}
			return pProperty->SetLongValue(lValue);
		}
	}
}

void CDclControlObject::SetColorProperty(int nID, COLORREF color)
{
	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
			return pProperty->SetOLEColorValue(color);
	}
}

COLORREF CDclControlObject::GetColorProperty(int nID) const
{
	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
			return pProperty->GetOLEColorValue();
	}
	return RGB(0,0,0);
}

void CDclControlObject::SetBoolProperty(int nID, bool bValue)
{
	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetID() == nID)
			return pProperty->SetBooleanValue(bValue);
	}
}
	
bool CDclControlObject::GetBoolProperty(int nID) const
{
	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
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

short CDclControlObject::FindPropertyIndex(int nID) const
{
	// create a position variable to hold the counter increment
	POSITION pos;	
	int nCount;
	
	// set counter for Properties
	nCount = 0;

	// do loop to navigate Arx Controls	
	while (nCount < m_PropertyList.GetCount())
	{
		// get position
		pos = m_PropertyList.FindIndex(nCount);
		// get current PropertyObject
		CPropertyObject* pProperty = m_PropertyList.GetAt(pos);
		// if the name of property matches
		if (pProperty->GetID() == nID)
		{
			if (nID == nObjectBrowser && nCount > 4)
			{
				const_cast<CDclControlObject*>(this)->m_PropertyList.RemoveAt(pos);
				pos = m_PropertyList.GetHeadPosition();
				const_cast<CDclControlObject*>(this)->m_PropertyList.InsertAfter(pos, pProperty);
				return 1;
			}
			if (nID == nGlobalVarName && nCount > 5)
			{
				const_cast<CDclControlObject*>(this)->m_PropertyList.RemoveAt(pos);
				pos = m_PropertyList.GetHeadPosition();
				m_PropertyList.GetNext(pos);
				const_cast<CDclControlObject*>(this)->m_PropertyList.InsertAfter(pos, pProperty);
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

CPropertyObject* CDclControlObject::FindProperty(CString sName)
{
	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
		assert(pProperty != NULL);
		if (pProperty->GetName() == sName)
			return pProperty;
	}
	return NULL;
}

POSITION CDclControlObject::FindPropertyInsertPos(int nID, bool bHidden) const
{
	if (nID == nObjectBrowser)
		return m_PropertyList.GetHeadPosition();		

	CString QueryPropText;
	int nResourceId = nID + 210; //hardcoding the property start index, need to fix that [ORW]
	QueryPropText = theWorkspace.LoadResourceString(nResourceId);
	return FindPropertyInsertPos(QueryPropText, bHidden);
}

POSITION CDclControlObject::FindPropertyInsertPos(CString sName, bool bHidden) const
{
	// create a position variable to hold the counter increment
	POSITION pos;	
	CString ThisPropText;
	CString QueryPropText = sName;
	
	// set counter for Properties begining at the end of the list
	INT_PTR nCount = m_PropertyList.GetCount() - 1;
	
	if (nCount == nNotSet)
		return NULL;

	// do loop to navigate Arx Controls	
	while (nCount >= 0)
	{
		// get position
		pos = m_PropertyList.FindIndex(nCount);
		if (pos == NULL)
			return NULL;

		// get current PropertyObject
		CPropertyObject* pProperty = m_PropertyList.GetAt(pos);
		
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
		delete m_pImageList;
		m_pImageList = NULL;
	}

	POSITION posProp = m_PropertyList.GetHeadPosition();
	while (posProp)
	{
		CPropertyObject* pProperty = m_PropertyList.GetNext(posProp);
		assert(pProperty != NULL);
		delete pProperty;
	}
	m_PropertyList.RemoveAll();
}

void CDclControlObject::ResetProperty(int nId)
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for CPropertyObject
	int nCount = m_PropertyList.GetCount()-1;

	while(nCount >= 0)
	{
		// get position
		pos = m_PropertyList.FindIndex(nCount);
		
		// get current property
		CPropertyObject* pPropObject = m_PropertyList.GetAt(pos);
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

void CDclControlObject::RemoveProperty(int nId)
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for CPropertyObject
	int nCount = m_PropertyList.GetCount()-1;

	while(nCount >= 0)
	{
		// get position
		pos = m_PropertyList.FindIndex(nCount);
		
		// get current property
		CPropertyObject* pPropObject = m_PropertyList.GetAt(pos);
		if (pPropObject->GetID() == nId)
		{
			// clear any lists in this control
			pPropObject->ClearList();
			m_PropertyList.RemoveAt(pos);
			delete pPropObject;
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
	int nCount = m_PropertyList.GetCount()-1;

	while(nCount >= 0)
	{
		// get position
		pos = m_PropertyList.FindIndex(nCount);
		
		if (pos != NULL)
		{
			// get current property
			CPropertyObject* pPropObject = m_PropertyList.GetAt(pos);
			
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
					m_PropertyList.RemoveAt(pos);
					delete pPropObject;
					break;
				}
			}
			switch (pPropObject->GetID())
			{
			case nDragnDropFromAutoCAD:
				m_PropertyList.RemoveAt(pos);
				delete pPropObject;
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
  if (readLine(sFile) != "CDclControlObject") return statInvalidFormat;
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
  m_AxTypeName.IsEmpty();
  m_nIsMicrosoftActiveX = nNotSet;
//#endif

  if (!readString(sFile, m_Name)) return statInvalidFormat;
	long lType;
  if (!readLong(sFile, lType)) return statInvalidFormat;
	mType = (ControlTypes)lType;
  if (!readShort(sFile, m_ClientHeight)) return statInvalidFormat;
  if (!readInt(sFile, m_PurchaseState)) return statInvalidFormat;

  if (!readInt(sFile, m_Id)) return statInvalidFormat;

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

  m_PropertyList.RemoveAll();

  // do loop to navigate objects
  while (nCount-- > 0)
  {
    // get current object
    CPropertyObject* pProp = new CPropertyObject;

    // get object from archive
		IOStatus stat = pProp->ReadFromTextFile(sFile);
    if (stat != statOK) return stat;

    // add that ArxControlObject to the list object
    m_PropertyList.AddTail(pProp);		
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
