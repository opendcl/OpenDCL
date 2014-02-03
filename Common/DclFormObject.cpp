// DclFormObject.cpp : implementation file
//

#include "stdafx.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "UndoManager.h"
#include "PropertyIds.h"
#include "PropertyObject.h"
#include "ArchiveEx.h"
#include "StgFile.h"
#include "Filing.h"
#include "ControlTypes.h"
#include "Workspace.h"
#include "DclControlProp.h"
#include "DialogObject.h"
#include "ControlPane.h"
#include "FontCollection.h"


//moving all image lists from the form to individual controls while reading older ODC files
static void MoveImageListsToControls( std::vector< CImageListObject* >& rImageLists, const CDclFormObject* pDclForm )
{
	const TDclControlList& Controls = pDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		const TPropertyList& Props = pDclControl->GetPropertyList();
		for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
		{
			if( (*iter)->GetType() == PropImageList )
			{//we have a winner
				short idx = (*iter)->GetShortValue();
				if( idx >= 0 && (size_t)idx < rImageLists.size() )
					pDclControl->SetImageList( new CImageListObject( *rImageLists.at( idx ) ) );
				pDclControl->RemoveProperty( (*iter)->GetID() );
				break; //stop looking and ignore any additional PropImageList properties, since there should only be one
			}
		}
	}
	size_t idx = rImageLists.size();
	while( idx-- > 0 )
		delete rImageLists.at( idx );
	rImageLists.clear();
}


/////////////////////////////////////////////////////////////////////////////
// CDclFormObject

CDclFormObject::CDclFormObject()
: mpProject( NULL )
, mType( _FrmInvalid )
, mnTabIndex( -1 )
, mpParentForm( NULL )
, mpDlgObject( NULL )
{
	m_htiTreeItem = NULL;
}

CDclFormObject::CDclFormObject( CProject* pProject, FormType type /*= _FrmInvalid*/ )
: mpProject( pProject )
, mType( type )
, mnTabIndex( -1 )
, mpParentForm( NULL )
, mpDlgObject( NULL )
{
	m_htiTreeItem = NULL;
	CreateControlProperties();
}

CDclFormObject::~CDclFormObject()
{
	assert( mpDlgObject == NULL );
	ClearControls();
}

//static
size_t CDclFormObject::GetCurrentControlIndex( TDclControlPtr pDclControl )
{
	TDclFormPtr pForm = pDclControl->GetOwnerForm();
	if( !pForm )
		return 0;
	size_t idxControl = 0;
	for( TDclControlList::const_iterator iter = pForm->mDclControls.begin();
			 iter != pForm->mDclControls.end();
			 ++iter, ++idxControl )
	{
		if( pDclControl == (*iter) )
			return idxControl;
	}
	return 0;
}

void CDclFormObject::OnModified()
{
	theWorkspace.SetModified();
}

void CDclFormObject::SetParentForm( TDclFormPtr pParentForm )
{
	mpParentForm = pParentForm;
	if( !pParentForm )
		return;
	msUniqueName = pParentForm->GetUniqueName();
	OnModified();
}

void CDclFormObject::SetParentForm( LPCTSTR pszParentUniqueName )
{
	CString sNewParentName = pszParentUniqueName;
	if( sNewParentName.IsEmpty() )
		return; //calling with an empty name is an error; use the other SetParentForm() to clear the parent
	TDclFormPtr pParentForm = mpProject->FindParentDclForm( sNewParentName );
	if( pParentForm )
	{
		SetParentForm( pParentForm );
		OnModified();
	}
}

void CDclFormObject::SetFormInstance( CDialogObject* pDlgObject )
{
	//note: an assertion here indicates more than 1 instance of the form; perhaps 
	//an earlier instance didn't get destroyed when it should have been?
	assert( mpDlgObject == NULL || pDlgObject == NULL );
	mpDlgObject = pDlgObject;
}


void CDclFormObject::AddControl( TDclControlPtr pDclControl, bool bAssignNewID /*= false*/, bool bToTopOfZOrder /*= true*/ )
{
	if( !pDclControl )
		return;
	if( bToTopOfZOrder || mDclControls.empty() )
		mDclControls.push_back( pDclControl );
	else
		mDclControls.insert( ++mDclControls.begin(), pDclControl );
	CUndoManager* pUndoManager = GetUndoManager();
	if( pUndoManager )
		pUndoManager->AddControl( pDclControl );
	OnModified();
	if( bAssignNewID || pDclControl->GetID() < 0 )
		pDclControl->SetID( GetUniqueControlId() );
}


TDclControlPtr CDclFormObject::AddControl( ControlType type, LPCTSTR pszKeyName, const CRect& rcControl, bool bToTopOfZOrder /*= true*/ )
{
	CUndoManager* pUndoManager = GetUndoManager();
	if( pUndoManager )
	{
		if( pUndoManager->enabled() )
			pUndoManager->setEnabled( false );
		else
			pUndoManager = NULL;
	}
	TDclControlPtr pNewControl = new CDclControlObject( type, this, pszKeyName );
	AddDefaultProperties( pNewControl, rcControl.Width(), rcControl.Height() );
	if( pUndoManager )
		pUndoManager->setEnabled( true );
	AddControl( pNewControl, true, bToTopOfZOrder );
	return pNewControl;
}


void CDclFormObject::DeleteControl( TDclControlPtr& pDclControl )
{
	if( pDclControl->GetType() == CtlTabStrip )
	{ //delete children also
		TProjectPtr pProject = pDclControl->GetOwnerProject();
		if( pProject )
		{
			TDclFormList ChildForms;
			if( pProject->FindChildForms( pDclControl->GetOwnerForm(), ChildForms ) )
			{
				for( TDclFormList::reverse_iterator iter = ChildForms.rbegin(); iter != ChildForms.rend(); ++iter )
					pProject->DeleteForm( *iter );
			}
		}
	}
	CUndoManager* pUndoManager = GetUndoManager();
	TDclControlList::iterator iter = mDclControls.begin();
	while( iter != mDclControls.end() )
	{
		TDclControlList::iterator iterAt = iter++;
		if( pDclControl != (*iterAt) )
			continue;
		OnModified();
		if( pUndoManager )
		{
			pUndoManager->DeleteControl( pDclControl );
		}
		mDclControls.erase( iterAt );
		pDclControl = NULL;
		break;
	}
}


bool CDclFormObject::ReorderControl( TDclControlPtr pDclControl, bool bToFront )
{
	assert( pDclControl != NULL );
	assert( pDclControl->GetOwnerForm() == (const CDclFormObject*)this );
	TDclControlList::iterator iter = mDclControls.begin();
	while( iter != mDclControls.end() )
	{
		TDclControlList::iterator iterAt = iter++;
		if( pDclControl != (*iterAt) )
			continue;
		CUndoManager* pUndoManager = GetUndoManager();
		if( pUndoManager )
			pUndoManager->ReorderControl( pDclControl );
		mDclControls.erase( iterAt );
		if( bToFront )
			mDclControls.insert( ++mDclControls.begin(), pDclControl );
		else
			mDclControls.push_back( pDclControl );
		OnModified();
		return true;
	}
	return false;
}


bool CDclFormObject::ReorderControl( TDclControlPtr pDclControl, size_t idxNew )
{
	assert( idxNew > 0 ); //should never try to insert at index zero -- that is reserved for form properties!
	assert( idxNew <= mDclControls.size() );
	assert( pDclControl != NULL );
	assert( pDclControl->GetOwnerForm() == (const CDclFormObject*)this );
	if( idxNew > mDclControls.size() )
		return false;
	TDclControlList::iterator iter = mDclControls.begin();
	while( iter != mDclControls.end() )
	{
		TDclControlList::iterator iterAt = iter++;
		if( pDclControl != (*iterAt) )
			continue;
		CUndoManager* pUndoManager = GetUndoManager();
		if( pUndoManager )
			pUndoManager->ReorderControl( pDclControl );
		mDclControls.erase( iterAt );
		TDclControlList::iterator iterInsert = mDclControls.begin();
		while( idxNew-- > 0 ) ++iterInsert;
		mDclControls.insert( iterInsert, pDclControl );
		OnModified();
		return true;
	}
	return false;
}

void CDclFormObject::SetGlobalVariableName( LPCTSTR pszRootName /*= NULL*/, bool bUpdateChildren /*= true*/ )	
{
	OnModified();
	CString sRootName = pszRootName;
	if( sRootName.IsEmpty() )
		sRootName = mpProject->GetKeyName();
	CString sFormName = sRootName + _T('/') + GetKeyName();
	GetControlProperties()->AddStringProperty( Prop::VarName, PropString, sFormName, true );
	if( !bUpdateChildren )
		return;
	for( TDclControlList::iterator iter = ++mDclControls.begin(); iter != mDclControls.end(); ++iter )
		(*iter)->SetGlobalVariableName( sFormName );
}

void CDclFormObject::ClearGlobalVariableName( bool bUpdateChildren /*= true*/ )	
{	
	OnModified();
	GetControlProperties()->SetStringProperty( Prop::VarName, NULL );
	if( !bUpdateChildren )
		return;
	for( TDclControlList::iterator iter = ++mDclControls.begin(); iter != mDclControls.end(); ++iter )
		(*iter)->ClearGlobalVariableName();
}

void CDclFormObject::ResetEventNames( bool bUpdateChildren /*= true*/ )	
{
	OnModified();
	GetControlProperties()->ResetEventNames();
	if( !bUpdateChildren )
		return;
	for( TDclControlList::iterator iter = ++mDclControls.begin(); iter != mDclControls.end(); ++iter )
		(*iter)->ResetEventNames();
}

TDclFormPtr CDclFormObject::AddChildForm( FormType type )
{
	TDclFormPtr pDclForm = mpProject->AddForm( type, TDclFormLockedPtr( this ) );
	if( pDclForm )
		OnModified();
	return pDclForm;
}

void CDclFormObject::SetTabIndex( short nIndex )
{
	if( mnTabIndex == nIndex )
		return;
	mnTabIndex = nIndex;
	OnModified();
}

LPCTSTR CDclFormObject::GetTitleText() const
{
	const TDclControlPtr pControl = GetControlProperties();
	assert(pControl != NULL);
	if (pControl)
		return pControl->GetStringProperty(Prop::TitleBarText);
	return NULL;
}

UINT_PTR CDclFormObject::GetTitleBarIcon()
{
	TDclControlPtr pControl = GetControlProperties();
	return (UINT_PTR)pControl->GetLongProperty(Prop::TitleBarIcon);
}

UINT_PTR CDclFormObject::GetUniqueControlId()
{
	int nHighest = 10;
	for( TDclControlList::const_iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter )
	{
		int nID = (*iter)->GetID();
		if (nID > nHighest)
			nHighest = nID;
	}
	return (UINT_PTR)nHighest + 1;
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
//    const_cast<TDclFormPtr>(this)->msUUID = CString(pUUID);
//  }
//  writeString(pFile, msUUID);
//  writeBOOL(pFile, mbUsesClientRect);
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
//    TDclControlPtr pControl = mDclControls.GetNext(pos);
//
//    if (!pControl->IsDeleted())
//      pControl->WriteToTextFile(pFile, fileName);
//  }		
//	return statOK;
//}

void CDclFormObject::Serialize(CArchive& ar)
{
	BYTE nThisVersion = GetCurrentSaveVersion();

	if (ar.IsStoring())
	{
		ar << nThisVersion;
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
		ar << (WORD)mDclControls.size();
		for( TDclControlList::iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter )
		{
			(*iter)->Serialize(ar);
		#ifdef _DIAGNOSTIC
			if( (GetAsyncKeyState( VK_CONTROL ) & KF_UP) == KF_UP )
				TraceFmt( _T("> %s\r\n"), (*iter)->toString() );
		#endif //_DIAGNOSTIC
		}		
	}
	else
	{
		OnModified();
		ar >> nThisVersion;
		if( nThisVersion <= 7 )
		{
			BYTE bSkip;
			ar >> bSkip;
			assert( bSkip == 0 );
			ar >> bSkip;
			assert( bSkip == 0 );
			ar >> bSkip;
			assert( bSkip == 0 );
		}
		if (nThisVersion > GetCurrentSaveVersion())
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );
		if (nThisVersion < 9)
		{
			CString sNameDiscard;
			ar >> sNameDiscard;
		}
		long lType;
		ar >> lType;
		if (lType < -1)
			mType = _FrmInvalid;
		else
			mType = (FormType)lType;
		ar >> msUniqueName;
		CString sParentName;
		ar >> sParentName;
		SetParentForm( sParentName );
		ar >> mnTabIndex;

		if (nThisVersion >= 2)
			ar >> msUUID;
		else
			msUUID.Empty();

		if( nThisVersion >= 4 && nThisVersion <= 6 )
		{
			if( nThisVersion < 6 )
			{
				BOOL bUsesClientRect;
				ar >> bUsesClientRect;
			}
			else
			{
				bool bUsesClientRect;
				ar >> bUsesClientRect;
			}
		}

		unsigned short nCount;
		ar >> nCount;	
		ClearControls();
		while (nCount-- > 0)
		{
			TDclControlPtr pDclControl = new CDclControlObject( this );
			pDclControl->Serialize(ar);

			//Check for another control with the same ID; assign a new one if there's a collision
			bool bAssignNewID = false;
			int nID = pDclControl->GetID();
			for (TDclControlList::iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter)
			{
				if ((*iter)->GetID() == nID)
				{
					if (pDclControl->GetType() != CtlSplitter)
						bAssignNewID = true;
					else if ((*iter)->GetType() == CtlSplitter)
						bAssignNewID = true;
					else
					{
						UINT_PTR nNewID = GetUniqueControlId();
						if (nID == nNewID)
							++nNewID;
						(*iter)->SetID( nNewID );
					}
					break;
				}
			}

			AddControl( pDclControl, bAssignNewID );
			#ifdef _DIAGNOSTIC
				if( (GetAsyncKeyState( VK_CONTROL ) & KF_UP) == KF_UP )
					TraceFmt( _T("< %s\r\n"), pDclControl->toString() );
			#endif //_DIAGNOSTIC
			// Add new properties that have been added since this file was created
			AddDefaultProperties( pDclControl );
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
	}
}

TDclControlPtr CDclFormObject::GetRefCountedPtr( CDclControlObject* pDclControl ) const
{
	for( TDclControlList::const_iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter )
	{
		if( (*iter) == pDclControl )
			return *iter;
	}
	return TDclControlLockedPtr( pDclControl );
}

void CDclFormObject::ClearControls()
{
	if( mDclControls.empty() )
		return;
#ifdef _DEBUG
	TDclControlList::iterator iter = mDclControls.end();
	do
	{
		--iter;
		assert( (*iter) != NULL );
		assert( (*iter)->GetControlInstance() == NULL ); //there should be no outstanding control instances!
	}
	while( iter != mDclControls.begin() );
#endif //_DEBUG
	mDclControls.clear();
}

IOStatus CDclFormObject::ReadFromTextFile(std::ifstream &sFile, const CString &fileName)
{
	CString sObject;
	if (readLine(sFile) != "CDclFormObject") return statInvalidFormat;
	int iVersion;
	if (!readInt(sFile, iVersion)) return statInvalidFormat;

	switch (iVersion) {
		case 4 : 
			return ReadFromTextFile4(sFile, fileName);
			break;
	}

	return statInvalidFormat;
}

IOStatus CDclFormObject::ReadFromTextFile4(std::ifstream &sFile, const CString &fileName)
{
	OnModified();
	// Get this dcl form's name and type from archive
	CStringA sNameDiscard;
	if (!readString(sFile, sNameDiscard)) return statInvalidFormat;
	long lType;
	if (!readLong(sFile, lType)) return statInvalidFormat;
	if (lType < -1)
		mType = _FrmInvalid;
	else
		mType = (FormType)lType;
	CStringA sUniqueName;
	if (!readString(sFile, sUniqueName)) return statInvalidFormat;
	msUniqueName = sUniqueName;
	CStringA sParentName;
	if (!readString(sFile, sParentName)) return statInvalidFormat;
	SetParentForm( CString( sParentName ) );
	if (!readShort(sFile, mnTabIndex)) return statInvalidFormat;
	CStringA sUUID;
	if (!readString(sFile, sUUID)) return statInvalidFormat;
	msUUID = sUUID;
	BOOL bUsesClientRect;
	if (!readBOOL(sFile, bUsesClientRect)) return statInvalidFormat;

	// get counter for arx controls
	int nCount;
	if (!readInt(sFile, nCount)) return statInvalidFormat;

	ClearControls();;

	while (nCount-- > 0)
	{
		TDclControlPtr pControl = new CDclControlObject( this );

		IOStatus stat = pControl->ReadFromTextFile(sFile, fileName);
		if (stat != statOK) return stat;

		AddDefaultProperties( pControl );
		AddControl(pControl);

		if (mType == FrmModalDlg)
			pControl->RemoveProperty(Prop::EventInvoke);
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

	if( !mDclControls.empty() )
	{
		TDclControlPtr pControl = GetControlProperties();		

		switch (mType)
		{
		case FrmModalDlg:
			pControl->RemoveProperty( Prop::EventInvoke );
			//break;  This break was missing -- maybe intentional, I can't tell for sure [ORW]
		case FrmModelessDlg:
			pControl->AddLongProperty( Prop::MinDialogWidth, PropLong, 0 );
			pControl->AddLongProperty( Prop::MinDialogHeight, PropLong, 0 );
			pControl->AddLongProperty( Prop::MaxDialogWidth, PropLong, 0 );
			pControl->AddLongProperty( Prop::MaxDialogHeight, PropLong, 0 );
			break;
		//case FrmControlBar:
		//	pControl->AddBooleanProperty( Prop::AllowResizing, PropBool, true );
		//	pControl->RemoveProperty( Prop::MinDialogWidth );
		//	pControl->RemoveProperty( Prop::MinDialogHeight );
		//	pControl->RemoveProperty( Prop::MaxDialogWidth );
		//	pControl->RemoveProperty( Prop::MaxDialogHeight );
		//	break;		
		}
	}
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
	const TDclControlPtr pControlProps = GetControlProperties();
	assert( pControlProps != NULL );
	if (!pControlProps)
		return CString(); //properties have not yet been added!
	CString sControlName = pControlProps->GetStringProperty(Prop::Name);
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
	return mpProject->GetKeyName() + _T('/') + GetKeyName();
}

CString CDclFormObject::GetVarName() const
{
	const TDclControlPtr pControlProps = GetControlProperties();
	assert( pControlProps != NULL );
	if (!pControlProps)
		return CString(); //properties have not yet been added!
	return pControlProps->GetVarName();
}

bool CDclFormObject::IsModeless() const
{
	switch( mType )
	{
	case _FrmInvalid: return false;
	case FrmModalDlg: return false;
	case FrmModelessDlg: return true;
	case FrmControlBar: return true;
	case FrmOptionsTab: return false;
	case FrmTabPage: return true;
	case FrmFileDlg: return false;
	case FrmPaletteDlg: return true;
	}
	return false;
}

CWnd* CDclFormObject::GetFormWindow() const
{
	if( !mpDlgObject )
		return NULL;
	return mpDlgObject->GetControlPane()->GetHostDialog();
}

TDclControlPtr CDclFormObject::CreateControlProperties()
{
	if( !mDclControls.empty() )
		return mDclControls.front();
	TDclControlPtr pProps = new CDclControlObject( this );
	mDclControls.push_back( pProps );
	return pProps;
}

const TDclControlPtr CDclFormObject::GetControlProperties() const
{
	if( mDclControls.empty() )
		return NULL;
	return mDclControls.front();
}

TDclControlPtr CDclFormObject::GetControlProperties()
{
	if( mDclControls.empty() )
		return NULL;
	return mDclControls.front();
}

CSize CDclFormObject::GetFormSize() const
{
	return CSize( GetControlProperties()->GetLongProperty(Prop::Width), GetControlProperties()->GetLongProperty(Prop::Height) );
}

TDclControlPtr CDclFormObject::FindControl( UINT nID, bool brecursive /*= true*/ ) const
{
	for( TDclControlList::const_iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter )
	{
		if( (*iter)->GetID() == nID )
			return *iter;
	}
	if( brecursive )
	{
		TDclFormList ChildForms;
		if( mpProject->FindChildForms( TDclFormLockedPtr( const_cast< CDclFormObject* >( this ) ), ChildForms ) )
		{
			for( TDclFormList::const_iterator iter = ChildForms.begin(); iter != ChildForms.end(); ++iter )
			{
				TDclControlPtr pCtrl = (*iter)->FindControl( nID );
				if( pCtrl )
					return pCtrl;
			}
		}
	}
	return NULL;
}

TDclControlPtr CDclFormObject::FindControl( LPCTSTR pszControlName ) const
{
	for( TDclControlList::const_iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter )
	{
		if( (*iter)->GetKeyName().CompareNoCase( pszControlName ) == 0 )
			return *iter;
	}
	TDclFormList ChildForms;
	if( mpProject->FindChildForms( TDclFormLockedPtr( const_cast< CDclFormObject* >( this ) ), ChildForms ) )
	{
		for( TDclFormList::const_iterator iter = ChildForms.begin(); iter != ChildForms.end(); ++iter )
		{
			TDclControlPtr pCtrl = (*iter)->FindControl( pszControlName );
			if( pCtrl )
				return pCtrl;
		}
	}
	return NULL;
}

TDclControlPtr CDclFormObject::FindControl( LPCTSTR pszControlName, ControlType eType ) const
{
	for( TDclControlList::const_iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter )
	{
		if( (*iter)->GetKeyName().CompareNoCase( pszControlName ) == 0 )
		{
			if( (*iter)->GetType() == eType )
				return *iter;
			break;
		}
	}
	TDclFormList ChildForms;
	if( mpProject->FindChildForms( TDclFormLockedPtr( const_cast< CDclFormObject* >( this ) ), ChildForms ) )
	{
		for( TDclFormList::const_iterator iter = ChildForms.begin(); iter != ChildForms.end(); ++iter )
		{
			TDclControlPtr pCtrl = (*iter)->FindControl( pszControlName, eType );
			if( pCtrl )
				return pCtrl;
		}
	}
	return NULL;
}

TDclControlPtr CDclFormObject::FindFirstControlOfType( ControlType eType ) const
{
	for( TDclControlList::const_iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter )
	{
		if( (*iter)->GetType() == eType )
			return *iter;
	}
	return NULL;
}

bool CDclFormObject::FindControls( ControlType eType, TDclControlList& Results ) const
{
	Results.clear();
	for( TDclControlList::const_iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter )
	{
		if( (*iter)->GetType() == eType )
			Results.push_back( (*iter) );
	}
	return true;
}

TDclControlPtr CDclFormObject::FindControlWithVarName( LPCTSTR pszVarName ) const
{
	for( TDclControlList::const_iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter )
	{
		if( (*iter)->GetStringProperty( Prop::VarName ).CompareNoCase( pszVarName ) == 0 )
			return *iter;
	}
	return NULL;
}

bool CDclFormObject::GetControlFonts( CFontCollection& Fonts ) const
{
	for( TDclControlList::const_iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter )
		Fonts.GetFont( (*iter), NULL );
	return true;
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
	for( TDclControlList::const_iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter )
		(*iter)->dump( true );
	theWorkspace.DisplayStatus( _T("========================================\r\n") );
}
#endif


#ifdef _DEBUG
void CDclFormObject::dumpDebugger( bool bDeep /*= true*/ ) const
{
	TraceFmt( _T("%s\r\n"), toString() );
	if( !bDeep )
		return;
	for( TDclControlList::const_iterator iter = mDclControls.begin(); iter != mDclControls.end(); ++iter )
		(*iter)->dumpDebugger( true );
	Trace( _T("========================================\r\n") );
}
#endif
