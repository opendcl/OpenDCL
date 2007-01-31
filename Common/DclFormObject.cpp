// DclFormObject.cpp : implementation file
//

#include "stdafx.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "UserMessageID.h"
#include "PropertyIds.h"
#include "PropertyObject.h"
#include "ArchiveEx.h"
#include "StgFile.h"
#include "Filing.h"
#include "ControlTypes.h"
#include "Project.h"

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
// CDclFormObject

IMPLEMENT_SERIAL(CDclFormObject, CObject, 1)

CDclFormObject::CDclFormObject()
: mpProject( NULL )
, mType( VdclInvalid )
{
	m_pChildWnd = NULL;
	m_pMdiChildWnd = NULL;
	m_htiTreeItem = NULL;
	m_TabIndex = nNotSet;
	m_bDeleted = false;
	m_bLoaded = false;
}

CDclFormObject::CDclFormObject( CProject* Project, DclFormType type /*= VdclInvalid*/ )
: mpProject( Project )
, mType( type )
{
	m_pChildWnd = NULL;
	m_pMdiChildWnd = NULL;
	m_htiTreeItem = NULL;
	m_TabIndex = nNotSet;
	m_bDeleted = false;
	m_bLoaded = false;
}

CDclFormObject::~CDclFormObject()
{
}


bool CDclFormObject::DeleteControl(long nIndex)
{
	POSITION pos = mDclControls.FindIndex(nIndex);

	// get the object for deletion
	CDclControlObject *pControl = mDclControls.GetAt(pos);

	// remove the object
	mDclControls.RemoveAt(pos);

	// clear the properties
	pControl->ClearProperties();

	// delete the object
	delete pControl;

	return true;
}


bool CDclFormObject::CanWeDeleteForm() const
{
	CString sTitle;
	CString sQuestion;

	// set counter for ArxControls
	INT_PTR nCount = mDclControls.GetCount();
	int nTotal = nNotSet; // please note it must be set to nNotSet to remove the arx control object that holds the properties of the dcl form

	// set start position for navigating ArxControls
	POSITION pos = mDclControls.GetHeadPosition();
	
	// do loop to navigate Arx Controls
	while (pos != NULL)
	{
		// get current ArxControlObject
		CDclControlObject* pControl = mDclControls.GetNext(pos);

		// count the controls
		if (pControl->m_Delete == FALSE)
		{
			nTotal++;
		}

		// if the control has at a tab strip control in it we must ask the user.
		if (pControl->GetType() == CtlTabStrip)
		{
			//sQuestion = theWorkspace.LoadResourceString(IDS_QDELETEFORMWTAB);
			sQuestion = _T("Are you sure you want to remove this dialog box?\r\nDeleting it will remove the Tab control and all it's Tab pane forms with their controls.");
			if(MessageBox(::GetActiveWindow(), sQuestion, _T("ObjectDCL"), MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2) == 6)
				return true;
			else
				return false;
		}

		// increment counter
		nCount--;
	}

	// if there is more than one control we must ask the user.
	if (nTotal > 0)
	{
		//sQuestion = theWorkspace.LoadResourceString(IDS_QDELETEFORM);
		sQuestion = _T("Deleting this tab will permanently delete the controls drawn on it's form.\r\nAre you sure you wish to delete this tab and it's child controls?");
		if(MessageBox(::GetActiveWindow(), sQuestion, _T("ObjectDCL"), MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2) == 6)
			return true;
		else
			return false;
	}

	return true;
}
int CDclFormObject::CountDeletedControls() const
{
	// set counter for ArxControls
	INT_PTR nCount = mDclControls.GetCount();
	int nDeleted = 0;

	// set start position for navigating ArxControls
	POSITION pos = mDclControls.GetHeadPosition();
	
	// do loop to navigate Arx Controls
	while (pos != NULL)
	{
		// get current ArxControlObject
		CDclControlObject* pControl = mDclControls.GetNext(pos);

		if (pControl->m_Delete)
			nDeleted++;

		// increment counter
		nCount--;
	}

	return nDeleted;
}

void CDclFormObject::UpdateGlobalVariable(CString sProjectName)	
{	
	if (mDclControls.GetCount() <= 1)
		return;

	CPropertyObject *pPropName = GetControlProperties()->GetPropertyObject(nName);
	
	POSITION pos = mDclControls.GetHeadPosition();
	while(pos != NULL)
	{
		CDclControlObject *pCtrl = mDclControls.GetNext(pos);
		if (pCtrl != NULL && pPropName != NULL)
		{
			pCtrl->UpdateGlobalVariable(pPropName->GetStringValue(), sProjectName);
		}
	}
}

void CDclFormObject::ForceUpdateGlobalVariable(CString sFormName)	
{	
	if (mDclControls.GetCount() <= 1)
		return;
	
	POSITION pos = mDclControls.GetHeadPosition();
	while(pos != NULL)
	{
		CDclControlObject *pCtrl = mDclControls.GetNext(pos);
		if (pCtrl != NULL)
		{
			pCtrl->ForceUpdateGlobalVariable(sFormName);
		}
	}
}

int CDclFormObject::CountDeletedImageLists() const
{
	// set counter for m_ImageListCollection
	INT_PTR nCount = m_ImageListCollection.GetCount();
	int nDeleted = 0;

	// set start position for navigating m_ImageListCollection
	POSITION pos = m_ImageListCollection.GetHeadPosition();
	
	// do loop to navigate m_ImageListCollection
	while (pos != NULL)
	{
		// get current m_ImageListCollection
		CImageListObject* pImageListObject = m_ImageListCollection.GetNext(pos);

		if (pImageListObject->m_Delete)
			nDeleted++;

		// increment counter
		nCount--;
	}
	//ASSERT(nCount == 0);

	return nDeleted;
}


CString CDclFormObject::GetDclFormTitle() const
{
	try
	{
		const CDclControlObject* pControl = GetControlProperties();
		assert(pControl != NULL);
		if (pControl)
			return pControl->GetStrProperty(nTitleBarText);
	}
	catch(...)
	{
	}
	return CString();
}

long CDclFormObject::GetDclFormTitleBarIcon()
{
	CDclControlObject* pControl = GetControlProperties();
	return pControl->GetLngProperty(nIcon);
}


void CDclFormObject::SaveSS(int n, int nType, CStgFile &FileStg, CDocument *pDoc)
{
	CString sKey;
	sKey.Format(_T("%d.dialog%d"), n, nType);
	FileStg.Open( sKey, CFile::modeCreate | CFile::modeWrite); 

	CArchiveEx arDcl(&FileStg, CArchive::store | CArchive::bNoFlushOnDelete, NULL, activeProject->GetPassword(), TRUE);
	arDcl.m_pDocument = pDoc;
	arDcl.m_bForceFlat = FALSE;

	// put dcl form into archive
	Serialize(arDcl);

	arDcl.Close();			

	FileStg.Close();	// close the stream
}

CDclFormObject* CDclFormObject::ReadSS(int n, int nType, CStgFile &FileStg, CDocument *pDoc)
{
	CString sKey;
	sKey.Format(_T("%d.dialog%d"), n, nType);
	FileStg.Open( sKey, CFile::modeRead | CFile::shareDenyWrite); 

	CArchiveEx arDcl(&FileStg, CArchive::load | CArchive::bNoFlushOnDelete, NULL, activeProject->GetPassword(), TRUE);
	
	// get current Dcl form
	CDclFormObject* pDclForm = new CDclFormObject;
		
	// get dcl form into archive
	pDclForm->Serialize(arDcl);
		
	arDcl.Close();			

	FileStg.Close();	// close the stream

	return pDclForm;
}

IOStatus CDclFormObject::WriteToTextFile(FILE* pFile, const CString &fileName) const
{
  POSITION pos;	
  int nCount;

  fprintf(pFile, "\nCDclFormObject");
  writeInt(pFile, 4);

  // put this dcl form's name and type into archive
  writeString(pFile, msName);
  writeLong(pFile, mType);

  writeString(pFile, m_UniqueName);
  writeString(pFile, m_ParentName);
  writeShort(pFile, m_TabIndex);

  if (m_UUID.GetLength() == 0)
  {
    UUID uuid;
    unsigned char *pUUID;
    UuidCreate(&uuid);
    UuidToStringA(&uuid, &pUUID);
    const_cast<CDclFormObject*>(this)->m_UUID = CString(pUUID);
  }
  writeString(pFile, m_UUID);
  writeBOOL(pFile, m_bUsesClientRect);

  // set counter for ArxControls
  nCount = (int)mDclControls.GetCount() - CountDeletedControls();

  writeInt(pFile, nCount);

  // set start position for navigating ArxControls
  pos = mDclControls.GetHeadPosition();
  // do loop to navigate Arx Controls
  while (pos != NULL)
  {
    // get current ArxControlObject
    CDclControlObject* pControl = mDclControls.GetNext(pos);

    if (pControl->m_Delete == FALSE)
    {
      // put dcl form into archive
      pControl->WriteToTextFile(pFile, fileName);
    }
  }		

  // set counter for Image lists
  nCount = (int)m_ImageListCollection.GetCount();
  writeInt(pFile, nCount);

  // set start position for navigating ArxControls
  pos = m_ImageListCollection.GetHeadPosition();

  // do loop to navigate images
  while (pos != NULL)
  {
    // get current Imagelist
    CImageListObject* pImage = m_ImageListCollection.GetNext(pos);

    if (pImage == NULL || pImage->m_Delete < 0 || pImage->m_Delete > 1) {
      writeBool(pFile, true);
    } else
    {
      writeBool(pFile, false);
      // put dcl form into archive
      pImage->WriteToTextFile(pFile, fileName);
    }
  }
	return statOK;
}

void CDclFormObject::Serialize(CArchive& ar)
{
	CObject::Serialize( ar );
	// create a position variable to hold the counter increment
	POSITION pos;	
	short nCount;
	DWORD nThisVersion = 4;

	if (ar.IsStoring())
	{
		ar << nThisVersion;
		
		// put this dcl form's name and type into archive
		ar << msName;
		ar << mType;
		
		ar << m_UniqueName;
		ar << m_ParentName;
		ar << m_TabIndex;

		if (m_UUID.GetLength() == 0)
		{
			UUID uuid;
		#ifdef _UNICODE
			RPC_WSTR pUUID;
		#else
			RPC_CSTR pUUID;
		#endif
			UuidCreate(&uuid);
			UuidToString(&uuid, &pUUID);
			m_UUID = (LPCTSTR)pUUID;
		}
		ar << m_UUID;
		ar << m_bUsesClientRect;

		// set counter for ArxControls
		nCount = (WORD)mDclControls.GetCount() - CountDeletedControls();

		ar << nCount;

		// set start position for navigating ArxControls
		pos = mDclControls.GetHeadPosition();
		// do loop to navigate Arx Controls
		while (pos != NULL)
		{
			// get current ArxControlObject
			CDclControlObject* pControl = mDclControls.GetNext(pos);

			if (pControl->m_Delete == FALSE)
			{
				// put dcl form into archive
				pControl->Serialize(ar);
			}
		}		
		
		// set counter for Image lists
		nCount = (WORD)m_ImageListCollection.GetCount();

		ar << nCount;

		// set start position for navigating ArxControls
		pos = m_ImageListCollection.GetHeadPosition();
		
		// do loop to navigate Arx Controls
		while (pos != NULL)
		{
			// get current magelist
			CImageListObject* pImage = m_ImageListCollection.GetNext(pos);

			if (pImage == NULL || pImage->m_Delete < 0 || pImage->m_Delete > 1)				
				ar << TRUE;
			else
			{
				ar << FALSE;
				// put dcl form into archive
				pImage->Serialize(ar);
			}
		}				
		
	}
	else
	{	
		ar >> nThisVersion;
		if (nThisVersion > GetCurrentSaveVersion())
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );
		
		// Get this dcl form's name and type from archive
		ar >> msName;
		long lType;
		ar >> lType;
		if (lType < nNotSet)
			mType = VdclInvalid;
		else
			mType = (DclFormType)lType;
		ar >> m_UniqueName;
		ar >> m_ParentName;
		ar >> m_TabIndex;

		if (nThisVersion >= 2)
		{
			ar >> m_UUID;
		}

		if (nThisVersion >= 4)
		{
			ar >> m_bUsesClientRect;
		}
		else
		{
			m_bUsesClientRect = FALSE;
		}

		
		// get counter for arx controls
		ar >> nCount;	

		mDclControls.RemoveAll();

		// set start position for navigating arx controls
		pos = mDclControls.GetHeadPosition();
		// do loop to navigate ArxControls
		while (nCount-- > 0)
		{
			// get current ArxControlObject
			CDclControlObject* pControl = new CDclControlObject( this );
			
			// get dcl form into archive
			pControl->Serialize(ar);

			//pControl->ForceUpdateGlobalVariable(msName);
	
			// add that ArxControlObject to the list object
			mDclControls.AddTail(pControl);	
			
			if (mType == VdclModal)
			{				
				CPropertyObject* pProp = pControl->GetPropertyObject(nEventInvoke);
				if (pProp != NULL)
				{
					POSITION pos = pControl->m_PropertyList.Find(pProp, NULL);
					pControl->m_PropertyList.RemoveAt(pos);
					delete pProp;
				}
			}
		}

		if (nThisVersion >= 3)
		{		
			ar >> nCount;

			// do loop to navigate Image Lists
			while (nCount-- > 0)
			{
				BOOL bNull;
				ar >> bNull;

				if (bNull == FALSE)
				{
					// create a new image list object
					CImageListObject* pImage = new CImageListObject;

					// put dcl form into archive
					pImage->Serialize(ar);

					// add that ImageList to the list object
					m_ImageListCollection.AddTail(pImage);
				}
			}
			if (nCount > 0)
			{
				pos = mDclControls.GetHeadPosition();
				// do loop to navigate ArxControls
				while (pos != NULL)
				{
					// get current ArxControlObject
					CDclControlObject* pControl = mDclControls.GetNext(pos);
					
					CPropertyObject *pImageProp = pControl->GetPropertyObject(nImageList); 
					if (pImageProp != NULL && pImageProp->m_ImageListIndex >= 0)
					{
						POSITION pos2 = m_ImageListCollection.FindIndex(pImageProp->m_ImageListIndex);
						if (pos2 != NULL)
						{
							CImageListObject *pImageList = m_ImageListCollection.GetAt(pos2);
							if (pImageList != NULL)
							{
								pControl->m_pImageList = new CImageListObject;
								pControl->m_pImageList->Copy(pImageList);
							}
						}
					}
				}			
			}
		}
		
		if (mDclControls.GetCount() > 0)
		{
			CDclControlObject* pControl = GetControlProperties();		

			switch (mType)
			{
			case VdclModal:
				{				
				CPropertyObject* pProp = pControl->GetPropertyObject(nEventInvoke);
				if (pProp != NULL)
				{
					POSITION pos = pControl->m_PropertyList.Find(pProp, NULL);
					pControl->m_PropertyList.RemoveAt(pos);
					delete pProp;
				}
				//break;  This break was missing -- maybe intentional, I can't tell for sure [ORW]
				}
			case VdclModeless:
				{
				if (!pControl->GetPropertyObject(nMinDialogWidth))
					AddControlProperty(pControl, nMinDialogWidth, _T("50"), PropLong);
				if (!pControl->GetPropertyObject(nMinDialogHeight))
					AddControlProperty(pControl, nMinDialogHeight, _T("50"), PropLong);
				
				if (!pControl->GetPropertyObject(nMaxDialogWidth))
					AddControlProperty(pControl, nMaxDialogWidth, _T("1000"), PropLong);
				if (!pControl->GetPropertyObject(nMaxDialogHeight))
					AddControlProperty(pControl, nMaxDialogHeight, _T("1000"), PropLong);
				break;
				}
			case VdclDockable:
				{
				if (pControl->GetPropertyObject(nResizable) == NULL )
					AddControlProperty(pControl, nResizable, _T("True"), PropBool);

				CPropertyObject* pProp = pControl->GetPropertyObject(nMinDialogWidth);
				if (pProp != NULL)
				{
					POSITION pos = pControl->m_PropertyList.Find(pProp, NULL);
					pControl->m_PropertyList.RemoveAt(pos);
					delete pProp;
				}
				pProp = pControl->GetPropertyObject(nMinDialogHeight);
				if (pProp != NULL)
				{
					POSITION pos = pControl->m_PropertyList.Find(pProp, NULL);
					pControl->m_PropertyList.RemoveAt(pos);
					delete pProp;
				}
				pProp = pControl->GetPropertyObject(nMaxDialogWidth);
				if (pProp != NULL)
				{
					POSITION pos = pControl->m_PropertyList.Find(pProp, NULL);
					pControl->m_PropertyList.RemoveAt(pos);
					delete pProp;
				}
				
				break;		
				}
			}
		}
	}

	if (nThisVersion <= 2)	
		m_ImageListCollection.Serialize(ar);
}


void CDclFormObject::IncrementPictureId(int nIdIncrement)
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for clipboard
	int nCount = mDclControls.GetCount()- 1;
	
	while(nCount >= 0)
	{
		// get position
		pos = mDclControls.FindIndex(nCount);
		// get current property
		CDclControlObject* pControlForm = mDclControls.GetAt(pos);
		
		int nPictureId = pControlForm->GetLngProperty(nPicture);
		if (nPictureId > 0)
			pControlForm->SetLngProperty(nPicture, pControlForm->GetLngProperty(nPicture) + nIdIncrement);

		int nPressedPictureId = pControlForm->GetLngProperty(nPressedPicture);
		if (nPressedPictureId > 0)
			pControlForm->SetLngProperty(nPressedPicture, pControlForm->GetLngProperty(nPressedPicture) + nIdIncrement);

		int nIconId = pControlForm->GetLngProperty(nIcon);
		if (nIconId > 0)
			pControlForm->SetLngProperty(nIcon, pControlForm->GetLngProperty(nIcon) + nIdIncrement);

		// increment counter
		nCount--;
	}
	
}


void CDclFormObject::EnsureIsLoaded()
{
	// if already loaded.
	if (m_bLoaded)
		return;

	CStgFile FileStg;	// instance the CF wrapper

	if (FileStg.OpenStg( m_sFileName ) == FALSE)	// creates the storage
		return;	
	
	FileStg.Open(m_sSubFileName, CFile::modeRead | CFile::shareDenyWrite); 

	CArchiveEx arDcl(&FileStg, CArchive::load | CArchive::bNoFlushOnDelete, NULL, activeProject->GetPassword(), TRUE);
	
	// get dcl form into archive
	Serialize(arDcl);
	m_bLoaded = true;
		
	arDcl.Close();			

	FileStg.Close();	// close the stream

	FileStg.CloseStg();	// close the storage file

}


void CDclFormObject::ClearControls()
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for clipboard
	int nCount = mDclControls.GetCount()- 1;
	
	while(nCount >= 0)
	{
		// get position
		pos = mDclControls.FindIndex(nCount);
		// get current property
		CDclControlObject* pControlForm = mDclControls.GetAt(pos);
		if (pControlForm != NULL)
		{
			// clear properties in this control
			pControlForm->ClearProperties();

			mDclControls.RemoveAt(pos);
			delete pControlForm;
		}
		// increment counter
		nCount--;
	}
			
	// set counter for clipboard
	int nImageCount = m_ImageListCollection.GetCount()- 1;
	while(nImageCount >= 0)
	{
		// get pos position
		pos = m_ImageListCollection.FindIndex(nImageCount);
	
		// get current property
		CImageListObject* pImageList = m_ImageListCollection.GetAt(pos);
		// clear properties in this control
		pImageList->m_ImageSize.cx = 0;
		pImageList->m_ImageSize.cy = 0;
		pImageList->m_ImageList.DeleteImageList();
		m_ImageListCollection.RemoveAt(pos);
		try
		{
			delete pImageList;
		}
		catch(...)
		{
		}
		// increment counter
		nImageCount--;
	}
}

void CDclFormObject::ClearR14Events()
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for clipboard
	int nCount = mDclControls.GetCount()- 1;
	
	while(nCount >= 0)
	{
		// get position
		pos = mDclControls.FindIndex(nCount);
		
		// get current property
		CDclControlObject* pControlForm = mDclControls.GetAt(pos);
		// clear properties in this control
		pControlForm->ClearR14Events();
		
		// increment counter
		nCount--;
	}

	if (mType == VdclDockable)
	{
		CDclControlObject* pControlForm = GetControlProperties();
		pControlForm->RemoveProperty(nResizable);
	}
	
}

IOStatus CDclFormObject::ReadFromTextFile(std::ifstream &sFile, const CString &fileName)
{
  CString sObject;
  if (readLine(sFile) != "CDclFormObject") return statInvalidFormat;
  int iVersion;
  if (!readInt(sFile, iVersion)) return statInvalidFormat;

  bool rVal = true;
  switch (iVersion) {
    case 4 : 
      return ReadFromTextFile4(sFile, fileName);
      break;
	}

  return statInvalidFormat;
}

IOStatus CDclFormObject::ReadFromTextFile4(std::ifstream &sFile, const CString &fileName)
{
  // Get this dcl form's name and type from archive
  if (!readString(sFile, msName)) return statInvalidFormat;
	long lType;
  if (!readLong(sFile, lType)) return statInvalidFormat;
  if (lType < nNotSet)
		mType = VdclInvalid;
	else
		mType = (DclFormType)lType;
  if (!readString(sFile, m_UniqueName)) return statInvalidFormat;
  if (!readString(sFile, m_ParentName)) return statInvalidFormat;
  if (!readShort(sFile, m_TabIndex)) return statInvalidFormat;

  if (!readString(sFile, m_UUID)) return statInvalidFormat;
  if (!readBOOL(sFile, m_bUsesClientRect)) return statInvalidFormat;

  // get counter for arx controls
  int nCount;
  if (!readInt(sFile, nCount)) return statInvalidFormat;

  mDclControls.RemoveAll();

  // set start position for navigating arx controls
  POSITION pos = mDclControls.GetHeadPosition();
  // do loop to navigate ArxControls
  while (nCount-- > 0)
  {
    // get current ArxControlObject
    CDclControlObject* pControl = new CDclControlObject( this );

		IOStatus stat = pControl->ReadFromTextFile(sFile, fileName);
    if (stat != statOK) return stat;

    // add that ArxControlObject to the list object
    mDclControls.AddTail(pControl);	

    if (mType == VdclModal)
    {				
      CPropertyObject* pProp = pControl->GetPropertyObject(nEventInvoke);
      if (pProp != NULL)
      {
        POSITION pos = pControl->m_PropertyList.Find(pProp, NULL);
        pControl->m_PropertyList.RemoveAt(pos);
        delete pProp;
      }
    }
  }

  if (!readInt(sFile, nCount)) return statInvalidFormat;
  // do loop to navigate Image Lists
  while (nCount-- > 0)
  {
    BOOL bNull;
    if (!readBOOL(sFile, bNull)) return statInvalidFormat;

    if (bNull == FALSE)
    {
      // create a new image list object
      CImageListObject* pImage = new CImageListObject;

			IOStatus stat = pImage->ReadFromTextFile(sFile, fileName);
			if (stat != statOK) return stat;

      // add that ImageList to the list object
      m_ImageListCollection.AddTail(pImage);
    }
  }
	//This code will never execute; was it supposed to? [ORW]
  //if (nCount > 0)
  //{
  //  pos = mDclControls.GetHeadPosition();
  //  // do loop to navigate ArxControls
  //  while (pos != NULL)
  //  {
  //    // get current ArxControlObject
  //    CDclControlObject* pControl = mDclControls.GetNext(pos);
  //    CPropertyObject *pImageProp = pControl->GetPropertyObject(nImageList); 
  //    if (pImageProp != NULL && pImageProp->m_ImageListIndex >= 0)
  //    {
  //      POSITION pos2 = m_ImageListCollection.FindIndex(pImageProp->m_ImageListIndex);
  //      if (pos2 != NULL)
  //      {
  //        CImageListObject *pImageList = m_ImageListCollection.GetAt(pos2);
  //        if (pImageList != NULL)
  //        {
  //          pControl->m_pImageList = new CImageListObject;
  //          pControl->m_pImageList->Copy(pImageList);
  //        }
  //      }
  //    }
  //  }			
  //}

#ifdef EDITOR
  if (mDclControls.GetCount() > 0)
  {
    CDclControlObject* pControl = GetControlProperties();		

    switch (mType)
    {
    case VdclModal:
      {				
        CPropertyObject* pProp = pControl->GetPropertyObject(nEventInvoke);
        if (pProp != NULL)
        {
          POSITION pos = pControl->m_PropertyList.Find(pProp, NULL);
          pControl->m_PropertyList.RemoveAt(pos);
          delete pProp;
        }
      }
    case VdclModeless:
      {
        CString sText = _T("50");
        if (!pControl->GetPropertyObject(nMinDialogWidth))
          AddControlProperty(pControl, nMinDialogWidth, sText, PropLong);
        if (!pControl->GetPropertyObject(nMinDialogHeight))
          AddControlProperty(pControl, nMinDialogHeight, sText, PropLong);

        sText=_T("1000");
        if (!pControl->GetPropertyObject(nMaxDialogWidth))
          AddControlProperty(pControl, nMaxDialogWidth, sText, PropLong);
        if (!pControl->GetPropertyObject(nMaxDialogHeight))
          AddControlProperty(pControl, nMaxDialogHeight, sText, PropLong);
        break;
      }
    case VdclDockable:
      {
        CString sTrue = _T("True");
        if (pControl->GetPropertyObject(nResizable) == NULL)
          AddControlProperty(pControl, nResizable, sTrue, PropBool);

        CPropertyObject* pProp = pControl->GetPropertyObject(nMinDialogWidth);
        if (pProp != NULL)
        {
          POSITION pos = pControl->m_PropertyList.Find(pProp, NULL);
          pControl->m_PropertyList.RemoveAt(pos);
          delete pProp;
        }
        pProp = pControl->GetPropertyObject(nMinDialogHeight);
        if (pProp != NULL)
        {
          POSITION pos = pControl->m_PropertyList.Find(pProp, NULL);
          pControl->m_PropertyList.RemoveAt(pos);
          delete pProp;
        }
        pProp = pControl->GetPropertyObject(nMaxDialogWidth);
        if (pProp != NULL)
        {
          POSITION pos = pControl->m_PropertyList.Find(pProp, NULL);
          pControl->m_PropertyList.RemoveAt(pos);
          delete pProp;
        }

        break;		
      }
    }
  }
#endif
  return statOK;
}

UUID CDclFormObject::getUUID() const
{
  UUID uuid;
#ifdef _UNICODE
  UuidFromString((RPC_WSTR)(LPCWSTR)m_UUID, &uuid);
#else
  UuidFromString((RPC_CSTR)(LPCSTR)m_UUID, &uuid);
#endif
  return uuid;
}

CString CDclFormObject::GetKeyName() const
{
	if (mType == VdclTabForm)
	{
		POSITION pos = mpProject->GetDclFormList().GetHeadPosition();
		while (pos != NULL)
		{
			CDclFormObject *pDcl = mpProject->GetDclFormList().GetNext(pos);
			if (pDcl != NULL)
			{
				if (this->m_ParentName == pDcl->m_UniqueName)
					return pDcl->GetKeyName();
			}			
		}
	}
	const CDclControlObject* pControl = GetControlProperties();
	if (!pControl)
		return CString(); //properties have not yet been added
	return pControl->GetStrProperty(nName);
}

CString CDclFormObject::GetKeyPath() const
{
	return mpProject->GetKeyName() + _T('_') + GetKeyName();
}

INT_PTR CDclFormObject::GetControlCount() const
{
	return mDclControls.GetCount();
}

CDclControlObject* CDclFormObject::CreateControlProperties()
{
	if( !mDclControls.IsEmpty() )
		return mDclControls.GetHead();
	CDclControlObject* pProps = new CDclControlObject(this);
	pProps->m_Id = CtlFormPropHolder;
	mDclControls.AddHead(pProps);
	return pProps;
}

const CDclControlObject* CDclFormObject::GetControlProperties() const
{
	if( mDclControls.IsEmpty() )
		return NULL;
	return mDclControls.GetHead();
}

CDclControlObject* CDclFormObject::GetControlProperties()
{
	if( mDclControls.IsEmpty() )
		return NULL;
	return mDclControls.GetHead();
}

CDclControlObject* CDclFormObject::FindControl( LPCTSTR pszControlName ) const
{
	POSITION pos = mDclControls.GetHeadPosition();
	while (pos != NULL)
	{
		CDclControlObject* pControl = mDclControls.GetNext(pos);
		if (pControl->GetStrProperty(nName) == pszControlName)
			return pControl;
	}
	return NULL;
}

CDclControlObject* CDclFormObject::FindControl( LPCTSTR pszControlName, ControlTypes eType ) const
{
	CDclControlObject* pControl = FindControl( pszControlName );
	if( pControl && pControl->GetType() == eType )
		return pControl;
	return NULL;
}

CDclControlObject* CDclFormObject::FindFirstControlOfType( ControlTypes eType ) const
{
	POSITION pos = mDclControls.GetHeadPosition();
	while (pos != NULL)
	{
		CDclControlObject* pControl = mDclControls.GetNext(pos);
		if (pControl->GetType() == eType)
			return pControl;
	}
	return NULL;
}

bool CDclFormObject::FindControls( ControlTypes eType, CList< CDclControlObject* >& Results ) const
{
	POSITION pos = mDclControls.GetHeadPosition();
	while (pos != NULL)
	{
		CDclControlObject* pControl = mDclControls.GetNext(pos);
		if (pControl->GetType() == eType)
			Results.AddTail(pControl);
	}
	return true;
}

bool CDclFormObject::AddControlFonts( CFontCollection& Fonts ) const
{
	POSITION posControl = mDclControls.GetHeadPosition();
	while (posControl != NULL)
	{
		CDclControlObject *pCtrl = mDclControls.GetNext(posControl);
		assert(pCtrl != NULL);
		if(pCtrl)
			Fonts.GetFont(pCtrl, NULL);
	}
	return true;
}

void CDclFormObject::ZOrderFrontAddTabControls()
{
	POSITION pos = mDclControls.GetHeadPosition();
	mDclControls.GetNext(pos); //skip the properties control
	while (pos)
	{
		CDclControlObject *pControl = mDclControls.GetNext(pos);
		CWnd* pWnd = pControl->m_pWnd; //pParent->GetDlgItem(pControl->m_Id);
		if (pWnd != NULL)
			pWnd->SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	}
}
