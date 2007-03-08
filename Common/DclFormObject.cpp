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


static void AddControlProperty(CDclControlObject *pControl, PropertyId nID, LPCTSTR strValue, PropertyType ValueType)
{
	// find the insert position for this new property
	POSITION InsertPos = pControl->FindPropertyInsertPos(nID, false);

	// create new property object pointer to pass to AddTail method
	RefCountedPtr< CPropertyObject > pPropertyObect = new CPropertyObject( ValueType, 0, nID );
	pPropertyObect->SetStringValue(strValue);
	pPropertyObect->SetHidden(false);

	// reset the name to the new value
	if (InsertPos == NULL)
		pControl->GetPropertyList().AddTail(pPropertyObect);
	else
		pControl->GetPropertyList().InsertAfter(InsertPos, pPropertyObect);
}


//moving all image lists from the form to individual controls while reading older ODC files
static void MoveImageListsToControls( std::vector< CImageListObject* >& rImageLists, CDclFormObject* pDclForm )
{
	POSITION pos = pDclForm->GetControlList().GetHeadPosition();
	while( pos )
	{
		CDclControlObject* pDclControl = pDclForm->GetControlList().GetNext( pos );
		POSITION posProp = pDclControl->GetPropertyList().GetHeadPosition();
		while( posProp )
		{
			RefCountedPtr< CPropertyObject > pProp = pDclControl->GetPropertyList().GetNext( posProp ); 
			if( pProp->GetType() == PropImageList )
			{//we have a winner
				short idx = pProp->GetShortValue();
				if( idx >= 0 && (size_t)idx < rImageLists.size() )
					pDclControl->SetImageList( new CImageListObject( *rImageLists.at( idx ) ) );
				pDclControl->RemoveProperty( pProp->GetID() );
				break; //stop looking and ignore any additional PropImageList properties, since there should only be one
			}
		}
	}
	size_t idx = rImageLists.size();
	while( idx-- > 0 )
		delete rImageLists.at( idx );
}


/////////////////////////////////////////////////////////////////////////////
// CDclFormObject

IMPLEMENT_SERIAL(CDclFormObject, CObject, 1)

CDclFormObject::CDclFormObject()
: mpProject( NULL )
, mType( VdclInvalid )
, mnTabIndex( -1 )
, mpParentForm( NULL )
, mpDlgObject( NULL )
{
	m_bUsesClientRect = TRUE;
	m_pChildWnd = NULL;
	m_pMdiChildWnd = NULL;
	m_htiTreeItem = NULL;
	m_bDeleted = false;
	m_bLoaded = false;
}

CDclFormObject::CDclFormObject( CProject* Project, DclFormType type /*= VdclInvalid*/ )
: mpProject( Project )
, mType( type )
, mnTabIndex( -1 )
, mpParentForm( NULL )
, mpDlgObject( NULL )
{
	m_bUsesClientRect = TRUE;
	m_pChildWnd = NULL;
	m_pMdiChildWnd = NULL;
	m_htiTreeItem = NULL;
	m_bDeleted = false;
	m_bLoaded = false;
	CreateControlProperties();
}

CDclFormObject::~CDclFormObject()
{
	ClearControls();
}


void CDclFormObject::SetParentForm( CDclFormObject* pParentForm )
{
	mpParentForm = pParentForm;
	if( pParentForm )
		msUniqueName = pParentForm->GetUniqueName();
}


void CDclFormObject::SetParentForm( LPCTSTR pszParentUniqueName )
{
	CString sNewParentName = pszParentUniqueName;
	if( sNewParentName.IsEmpty() )
		return; //calling with an empty name is an error; use the other SetParentForm() to clear the parent
	CDclFormObject* pParentForm = mpProject->FindParentDclForm( sNewParentName );
	if( pParentForm )
		SetParentForm( pParentForm );
}

void CDclFormObject::SetFormInstance( CDialogObject* pDlgObject )
{
	//note: an assertion here indicates more than 1 instance of the form; perhaps 
	//an earlier instance didn't get destroyed when it should have been?
	assert( mpDlgObject == NULL || pDlgObject == NULL );
	mpDlgObject = pDlgObject;
}


void CDclFormObject::AddControl( CDclControlObject* pDclControl )
{
	mDclControls.AddTail( pDclControl );
	INT_PTR idxNewControl = mDclControls.GetCount() - 1;
	pDclControl->SetZOrder( idxNewControl );
	if( pDclControl->GetID() < 0 )
		pDclControl->SetID( idxNewControl );
	pDclControl->m_PurchaseState = mpProject->GetPurchaseState();
}


CDclControlObject* CDclFormObject::AddControl( ControlType type, LPCTSTR pszKeyName )
{
	CDclControlObject* pNewControl = new CDclControlObject( type, this, pszKeyName );
	AddControl( pNewControl );
	return pNewControl;
}


void CDclFormObject::DeleteControl( CDclControlObject*& pDclControl )
{
	POSITION pos = mDclControls.Find( pDclControl );
	if( pos )
	{
		POSITION posDelete = pos;
		INT_PTR idxCurrent = pDclControl->GetZOrder();
		do //reindex remaining controls
		{
			CDclControlObject* pDclControl = mDclControls.GetNext( pos );
			assert( pDclControl != NULL );
			pDclControl->SetZOrder( idxCurrent++ );
		}
		while( pos );
		mDclControls.RemoveAt( posDelete );
		delete pDclControl;
	}
}


void CDclFormObject::PurgeDeletedControls()
{
	INT_PTR idxCurrent = 0;
	POSITION pos = mDclControls.GetHeadPosition();
	while( pos )
	{
		POSITION posAt = pos;
		CDclControlObject* pDclControl = mDclControls.GetNext( pos );
		assert( pDclControl != NULL );
		if( pDclControl && pDclControl->m_Delete )
		{
			mDclControls.RemoveAt(posAt);
			delete pDclControl;
		}
		pDclControl->SetZOrder( idxCurrent++ );
	}
}


void CDclFormObject::PurgeDeletedImageLists()
{
	if( mImageLists.empty() )
		return;
	std::vector< CImageListObject >::iterator iter = mImageLists.end();
	do
	{
		if( --iter->m_Delete )
			mImageLists.erase( iter );
	}
	while( iter != mImageLists.begin() );
}


bool CDclFormObject::ReorderControl( CDclControlObject* pDclControl, bool bToFront, bool bDeferReindexing /*= false*/ )
{
	assert( pDclControl != NULL );
	assert( pDclControl->GetOwnerForm() == this );
	POSITION pos = mDclControls.Find( pDclControl );
	assert( pos != NULL );
	if( !pos )
		return false;
	mDclControls.RemoveAt( pos );
	if( bToFront )
		mDclControls.InsertAfter( mDclControls.GetHeadPosition(), pDclControl ); //insert at second position (skipping form properties)
	else
		mDclControls.AddTail( pDclControl );
	if( !bDeferReindexing )
		ReindexControls();
	return true;
}


bool CDclFormObject::ReorderControl( CDclControlObject* pDclControl, size_t idxNew, bool bDeferReindexing /*= false*/ )
{
	assert( idxNew > 0 ); //should never try to insert at index zero -- that is reserved for form properties!
	assert( pDclControl != NULL );
	assert( pDclControl->GetOwnerForm() == this );
	POSITION pos = mDclControls.Find( pDclControl );
	assert( pos != NULL );
	if( !pos )
		return false;
	mDclControls.RemoveAt( pos );
	pos = mDclControls.FindIndex( idxNew );
	if( pos )
		mDclControls.InsertAfter( pos, pDclControl ); //insert at new position
	else
		mDclControls.AddTail( pDclControl );
	if( !bDeferReindexing )
		ReindexControls();
	return true;
}


void CDclFormObject::ReindexControls()
{
	INT_PTR idxCurrent = 0;
	POSITION pos = mDclControls.GetHeadPosition();
	while( pos )
	{
		CDclControlObject* pDclControl = mDclControls.GetNext( pos );
		assert( pDclControl != NULL );
		pDclControl->SetZOrder( idxCurrent++ );
	}
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
	INT_PTR ctDeleted = 0;
	POSITION pos = mDclControls.GetHeadPosition();
	while (pos)
	{
		if (mDclControls.GetNext(pos)->m_Delete)
			ctDeleted++;
	}
	return ctDeleted;
}

void CDclFormObject::SetGlobalVariableName( LPCTSTR pszRootName /*= NULL*/, bool bUpdateChildren /*= true*/ )	
{	
	CString sRootName = pszRootName;
	if( sRootName.IsEmpty() )
		sRootName = mpProject->GetKeyName();
	CString sFormName = sRootName + _T('_') + GetKeyName();
	GetControlProperties()->AddStringProperty( nGlobalVarName, PropString, sFormName, true );

	if( !bUpdateChildren )
		return;

	POSITION pos = mDclControls.GetHeadPosition();
	mDclControls.GetNext(pos); //skip the form properties control
	while( pos )
		mDclControls.GetNext(pos)->SetGlobalVariableName( sFormName );
}

void CDclFormObject::ClearGlobalVariableName( bool bUpdateChildren /*= true*/ )	
{	
	GetControlProperties()->SetStringProperty( nGlobalVarName, NULL );

	if( !bUpdateChildren )
		return;

	POSITION pos = mDclControls.GetHeadPosition();
	mDclControls.GetNext(pos); //skip the form properties control
	while( pos )
		mDclControls.GetNext(pos)->ClearGlobalVariableName();
}

size_t CDclFormObject::CountDeletedImageLists() const
{
	size_t ctDeleted = 0;
	size_t idx = mImageLists.size();
	while( idx-- > 0 )
	{
		if( mImageLists.at( idx ).m_Delete )
			++ctDeleted;
	}
	return ctDeleted;
}

CDclFormObject* CDclFormObject::AddChildForm( DclFormType type )
{
	return mpProject->AddForm( type, this );
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

//IOStatus CDclFormObject::WriteToTextFile(FILE* pFile, const CString &fileName) const
//{
//  POSITION pos;	
//  int nCount;
//
//  fprintf(pFile, "\nCDclFormObject");
//  writeInt(pFile, 4);
//
//  // put this dcl form's name and type into archive
//  writeString(pFile, msName);
//  writeLong(pFile, mType);
//
//  writeString(pFile, msUniqueName);
//  writeString(pFile, GetParentName());
//  writeShort(pFile, mnTabIndex);
//
//  if (msUUID.GetLength() == 0)
//  {
//    UUID uuid;
//    unsigned char *pUUID;
//    UuidCreate(&uuid);
//    UuidToStringA(&uuid, &pUUID);
//    const_cast<CDclFormObject*>(this)->msUUID = CString(pUUID);
//  }
//  writeString(pFile, msUUID);
//  writeBOOL(pFile, m_bUsesClientRect);
//
//  // set counter for ArxControls
//  nCount = (int)mDclControls.GetCount() - CountDeletedControls();
//
//  writeInt(pFile, nCount);
//
//  // set start position for navigating ArxControls
//  pos = mDclControls.GetHeadPosition();
//  // do loop to navigate Arx Controls
//  while (pos != NULL)
//  {
//    // get current ArxControlObject
//    CDclControlObject* pControl = mDclControls.GetNext(pos);
//
//    if (pControl->m_Delete == FALSE)
//    {
//      // put dcl form into archive
//      pControl->WriteToTextFile(pFile, fileName);
//    }
//  }		
//	return statOK;
//}

void CDclFormObject::Serialize(CArchive& ar)
{
	CObject::Serialize( ar );
	// create a position variable to hold the counter increment
	POSITION pos;	
	short nCount;
	DWORD nThisVersion = GetCurrentSaveVersion();

	if (ar.IsStoring())
	{
		ar << nThisVersion;
		
		// put this dcl form's name and type into archive
		ar << msName;
		ar << mType;
		
		ar << msUniqueName;
		ar << CString(GetParentName());
		ar << mnTabIndex;

		if (msUUID.GetLength() == 0)
		{
			UUID uuid;
		#ifdef _UNICODE
			RPC_WSTR pUUID;
		#else
			RPC_CSTR pUUID;
		#endif
			UuidCreate(&uuid);
			UuidToString(&uuid, &pUUID);
			msUUID = (LPCTSTR)pUUID;
		}
		ar << msUUID;
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
				TraceFmt( _T("> %s\r\n"), pControl->toString() );
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
		ar >> msUniqueName;
		CString sParentName;
		ar >> sParentName;
		SetParentForm( sParentName );
		ar >> mnTabIndex;

		if (nThisVersion >= 2)
			ar >> msUUID;
		else
			msUUID.Empty();

		if (nThisVersion >= 4)
			ar >> m_bUsesClientRect;
		else
			m_bUsesClientRect = FALSE;

		// get counter for dcl controls
		ar >> nCount;	
		mDclControls.RemoveAll();
		while (nCount-- > 0)
		{
			CDclControlObject* pControl = new CDclControlObject( this );
			pControl->Serialize(ar);
			AddControl( pControl );
			TraceFmt( _T("< %s\r\n"), pControl->toString() );
			if (mType == VdclModal)
			{				
				RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyObject(nEventInvoke);
				if (pProp != NULL)
				{
					POSITION pos = pControl->GetPropertyList().Find(pProp);
					pControl->GetPropertyList().RemoveAt(pos);
				}
			}
		}

		//ensure that we have a form properties control at the head of the list
		CreateControlProperties();

		if( nThisVersion < 5 ) //removed image list collection at version 5
		{
			std::vector< CImageListObject* > rImageLists;
			if (nThisVersion == 3 || nThisVersion == 4)
			{
				short ctImageLists;
				ar >> ctImageLists;
				for( size_t idx = 0; idx < (size_t)ctImageLists; ++idx )
				{
					BOOL bNull;
					ar >> bNull;
					if( bNull )
						continue;
					CImageListObject* pImageList = new CImageListObject;
					pImageList->Serialize( ar );
					rImageLists.push_back( pImageList );
				}
			}
			else // nThisVersion < 3
			{
				CTypedPtrList< CObList, CImageListObject* > ImageLists;
				ImageLists.Serialize(ar);
				POSITION pos = ImageLists.GetHeadPosition();
				while( pos )
				{
					POSITION posAt = pos;
					CImageListObject* pImageList = ImageLists.GetNext( pos );
					if( pImageList )
					{
						rImageLists.push_back( pImageList );
						ImageLists.RemoveAt( posAt );
					}
				}
			}
			if( !rImageLists.empty() )
				MoveImageListsToControls( rImageLists, this ); //moving all control image lists to the individual controls
		}
		
		if (mDclControls.GetCount() > 0)
		{
			CDclControlObject* pControl = GetControlProperties();

			switch (mType)
			{
			case VdclModal:
				{				
				RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyObject(nEventInvoke);
				if (pProp != NULL)
				{
					POSITION pos = pControl->GetPropertyList().Find(pProp, NULL);
					pControl->GetPropertyList().RemoveAt(pos);
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

				RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyObject(nMinDialogWidth);
				if (pProp != NULL)
				{
					POSITION pos = pControl->GetPropertyList().Find(pProp, NULL);
					pControl->GetPropertyList().RemoveAt(pos);
				}
				pProp = pControl->GetPropertyObject(nMinDialogHeight);
				if (pProp != NULL)
				{
					POSITION pos = pControl->GetPropertyList().Find(pProp, NULL);
					pControl->GetPropertyList().RemoveAt(pos);
				}
				pProp = pControl->GetPropertyObject(nMaxDialogWidth);
				if (pProp != NULL)
				{
					POSITION pos = pControl->GetPropertyList().Find(pProp, NULL);
					pControl->GetPropertyList().RemoveAt(pos);
				}
				
				break;		
				}
			}
		}
	}
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
			pControlForm->SetLongProperty(nPicture, pControlForm->GetLngProperty(nPicture) + nIdIncrement);

		int nPressedPictureId = pControlForm->GetLngProperty(nPressedPicture);
		if (nPressedPictureId > 0)
			pControlForm->SetLongProperty(nPressedPicture, pControlForm->GetLngProperty(nPressedPicture) + nIdIncrement);

		int nIconId = pControlForm->GetLngProperty(nIcon);
		if (nIconId > 0)
			pControlForm->SetLongProperty(nIcon, pControlForm->GetLngProperty(nIcon) + nIdIncrement);

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

	CArchiveEx arDcl(&FileStg, CArchive::load | CArchive::bNoFlushOnDelete, NULL, mpProject->GetPassword(), TRUE);
	
	// get dcl form into archive
	Serialize(arDcl);
	m_bLoaded = true;
		
	arDcl.Close();			

	FileStg.Close();	// close the stream

	FileStg.CloseStg();	// close the storage file

}


void CDclFormObject::ClearControls()
{
	POSITION posControl = mDclControls.GetTailPosition();
	while(posControl)
	{
		CDclControlObject* pCtrl = mDclControls.GetPrev(posControl);
		assert( pCtrl != NULL );
		assert( pCtrl->GetControlInstance() == NULL ); //there should be no outstanding control instances!
		delete pCtrl;
	}
	mDclControls.RemoveAll();
}

void CDclFormObject::ClearR14Events()
{
	POSITION pos = mDclControls.GetTailPosition();
	while(pos != NULL)
	{
		CDclControlObject* pCtrl = mDclControls.GetPrev(pos);
		assert( pCtrl != NULL );
		if( pCtrl )
			pCtrl->ClearR14Events();
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
  if (!readString(sFile, msUniqueName)) return statInvalidFormat;
	CString sParentName;
  if (!readString(sFile, sParentName)) return statInvalidFormat;
	SetParentForm( sParentName );
  if (!readShort(sFile, mnTabIndex)) return statInvalidFormat;

  if (!readString(sFile, msUUID)) return statInvalidFormat;
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
    AddControl(pControl);	

    if (mType == VdclModal)
    {				
      RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyObject(nEventInvoke);
      if (pProp != NULL)
      {
        POSITION pos = pControl->GetPropertyList().Find(pProp, NULL);
        pControl->GetPropertyList().RemoveAt(pos);
      }
    }
  }

	int ctImageLists;
  if (!readInt(sFile, ctImageLists)) return statInvalidFormat;
	std::vector< CImageListObject* > rImageLists;
	for( size_t idx = 0; idx < (size_t)ctImageLists; ++idx )
	{
		BOOL bNull;
		if (!readBOOL(sFile, bNull)) return statInvalidFormat;
		if( bNull )
			continue;
		CImageListObject* pImageList = new CImageListObject;
		IOStatus stat = pImageList->ReadFromTextFile( sFile, fileName );
    if (stat != statOK)
		{
			delete pImageList;
			size_t idx = rImageLists.size();
			while( idx-- > 0 )
				delete rImageLists.at( idx );
			return stat;
		}
		rImageLists.push_back( pImageList );
	}
	if( !rImageLists.empty() )
		MoveImageListsToControls( rImageLists, this ); //moving all control image lists to the individual controls

#ifdef EDITOR
  if (mDclControls.GetCount() > 0)
  {
    CDclControlObject* pControl = GetControlProperties();		

    switch (mType)
    {
    case VdclModal:
      {				
        RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyObject(nEventInvoke);
        if (pProp != NULL)
        {
          POSITION pos = pControl->GetPropertyList().Find(pProp, NULL);
          pControl->GetPropertyList().RemoveAt(pos);
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

        RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyObject(nMinDialogWidth);
        if (pProp != NULL)
        {
          POSITION pos = pControl->GetPropertyList().Find(pProp, NULL);
          pControl->GetPropertyList().RemoveAt(pos);
        }
        pProp = pControl->GetPropertyObject(nMinDialogHeight);
        if (pProp != NULL)
        {
          POSITION pos = pControl->GetPropertyList().Find(pProp, NULL);
          pControl->GetPropertyList().RemoveAt(pos);
        }
        pProp = pControl->GetPropertyObject(nMaxDialogWidth);
        if (pProp != NULL)
        {
          POSITION pos = pControl->GetPropertyList().Find(pProp, NULL);
          pControl->GetPropertyList().RemoveAt(pos);
        }

        break;		
      }
    }
  }
#endif
	m_bDeleted = false;
	return statOK;
}

UUID CDclFormObject::GetUUID() const
{
  UUID uuid;
#ifdef _UNICODE
  UuidFromString((RPC_WSTR)(LPCWSTR)msUUID, &uuid);
#else
  UuidFromString((RPC_CSTR)(LPCSTR)msUUID, &uuid);
#endif
  return uuid;
}

CString CDclFormObject::GetKeyName() const
{
	const CDclControlObject* pControlProps = GetControlProperties();
	assert( pControlProps != NULL );
	if (!pControlProps)
		return CString(); //properties have not yet been added!
	CString sControlName = pControlProps->GetStrProperty(nName);
	if( sControlName.IsEmpty() )
	{
		if( mpParentForm )
			sControlName = mpParentForm->GetKeyName();
		else
			sControlName = msUniqueName;
	}
	return sControlName;
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
		return CreateControlProperties();
	return mDclControls.GetHead();
}

CSize CDclFormObject::GetFormSize() const
{
	return CSize( GetControlProperties()->GetLngProperty(nWidth), GetControlProperties()->GetLngProperty(nHeight) );
}

CDclControlObject* CDclFormObject::FindControl( LPCTSTR pszControlName ) const
{
	POSITION pos = mDclControls.GetHeadPosition();
	while( pos )
	{
		CDclControlObject* pControl = mDclControls.GetNext( pos );
		if( pControl->GetKeyName().CompareNoCase( pszControlName ) == 0 )
			return pControl;
	}
	return NULL;
}

CDclControlObject* CDclFormObject::FindControl( LPCTSTR pszControlName, ControlType eType ) const
{
	CDclControlObject* pControl = FindControl( pszControlName );
	if( pControl && pControl->GetType() == eType )
		return pControl;
	return NULL;
}

CDclControlObject* CDclFormObject::FindFirstControlOfType( ControlType eType ) const
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

bool CDclFormObject::FindControls( ControlType eType, CList< CDclControlObject* >& Results ) const
{
	POSITION pos = mDclControls.GetHeadPosition();
	while (pos != NULL)
	{
		CDclControlObject* pControl = mDclControls.GetNext( pos );
		if( pControl->GetType() == eType )
			Results.AddTail(pControl);
	}
	return true;
}

CDclControlObject* CDclFormObject::FindControlWithVarName( LPCTSTR pszVarName ) const
{
	POSITION pos = mDclControls.GetHeadPosition();
	while( pos )
	{
		CDclControlObject* pControl = mDclControls.GetNext( pos );
		if( pControl->GetStrProperty( nGlobalVarName ).CompareNoCase( pszVarName ) == 0 )
			return pControl;
	}
	return NULL;
}

bool CDclFormObject::GetControlFonts( CFontCollection& Fonts ) const
{
	POSITION pos = mDclControls.GetHeadPosition();
	while( pos )
	{
		CDclControlObject *pCtrl = mDclControls.GetNext( pos );
		assert( pCtrl != NULL );
		Fonts.GetFont( pCtrl, NULL );
	}
	return true;
}

void CDclFormObject::ZOrderFrontAddTabControls()
{
	POSITION pos = mDclControls.GetHeadPosition();
	mDclControls.GetNext( pos ); //skip the properties control
	while( pos )
	{
		CDclControlObject *pControl = mDclControls.GetNext( pos );
		CWnd* pWnd = pControl->GetWindow();
		if (pWnd != NULL)
			pWnd->SetWindowPos( &CWnd::wndTop, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE );
	}
}


#ifdef _DIAGNOSTIC
LPCTSTR CDclFormObject::toString() const
{
	CString sInstance;
	if( mpDlgObject )
		sInstance.Format( _T(" (DlgObject: %s)"), asString( mpDlgObject ) );
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("CDclControlobject [%s: %s%s]"), asString( mType ), GetKeyPath(), (LPCTSTR)sInstance );
	return buf;
}

void CDclFormObject::dump( bool bDeep /*= true*/ ) const
{
	CString sOut;
	sOut.Format( _T("%s\r\n"), toString() );
	theWorkspace.DisplayStatus( sOut );
	if( !bDeep )
		return;
	POSITION pos = mDclControls.GetHeadPosition();
	while( pos )
		mDclControls.GetNext( pos )->dump( true );
	theWorkspace.DisplayStatus( _T("========================================\r\n") );
}
#endif


#ifdef _DEBUG
void CDclFormObject::dumpDebugger( bool bDeep /*= true*/ ) const
{
	TraceFmt( _T("%s\r\n"), toString() );
	if( !bDeep )
		return;
	POSITION pos = mDclControls.GetHeadPosition();
	while( pos )
		mDclControls.GetNext( pos )->dumpDebugger( true );
	Trace( _T("========================================\r\n") );
}
#endif
