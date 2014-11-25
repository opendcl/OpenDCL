// CProject.cpp : implementation file
//

#include "stdafx.h"
#include "Project.h"
#include "Workspace.h"
#include "PropertyIds.h"
#include "Resource.h"
#include "DclControlTemplate.h"
#include "PropertyObject.h"
#include "DclControlProp.h"
#include "UndoManager.h"
#include "AxTypeUtils.h"
#include "ArchiveEx.h"
#include "AxContainerCtrl.h"
#include "StgFile.h"
#include "Filing.h"
#include "DclFormTemplate.h"
#include "DialogObject.h"
#include "DclPicture.h"
#include "ControlTypes.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxMethodDescriptor.h"
#include "Base64.h"
#include <fstream>
#include <stdio.h>


static CString StripPathFromFileName( LPCTSTR pszFilePath )
{
	CString sShortName = pszFilePath;
	sShortName.MakeReverse();
	sShortName = sShortName.SpanExcluding(_T("\\/:"));
	sShortName.MakeReverse();
	return sShortName;
}


static CString CreateUniqueName() 
{
	CString sUniqueName;
	UUID uuid;
#ifdef _UNICODE
	RPC_WSTR pUUID;
#else
	RPC_CSTR pUUID;
#endif
	UuidCreate(&uuid);
	UuidToString(&uuid, &pUUID);
	sUniqueName = (LPCTSTR)pUUID;
	return sUniqueName;
}


bool CheckSignature( CFile& File, LPCSTR pszSignature )
{
	if( !pszSignature || !*pszSignature )
		return false;
	File.SeekToBegin();
	int cchSig = lstrlenA( pszSignature );
	char* pszLine1 = new char[cchSig + 1];
	UINT_PTR cbRead = File.Read( pszLine1, cchSig );
	pszLine1[cbRead] = '\0';
	bool bMatch = (lstrcmpA( pszLine1, pszSignature ) == 0);
	delete[] pszLine1;
	return bMatch;
}


/////////////////////////////////////////////////////////////////////////////
// CProject

CProject::CProject()
{
	Initialize();
}

CProject::CProject( LPCTSTR pszKeyName )
: msKeyName( pszKeyName )
{
	Initialize();
}

CProject::~CProject()
{
}

void CProject::Initialize()
{
	msPassword.Empty();
	msLispFileName.Empty();	
	mnAutoCADVersion = theWorkspace.GetMinSupportedAcadVersion();
}

void CProject::OnModified()
{
	theWorkspace.SetModified();
}

void CProject::SetKeyName( LPCTSTR pszKeyName )
{
	if( !pszKeyName )
		return;
	CString sKey = pszKeyName;
	sKey = sKey.MakeReverse().SpanExcluding( _T("\\/:") ).MakeReverse().SpanExcluding( _T(".") );
	sKey.Replace(_T(' '), _T('|'));
	msKeyName = sKey;
	OnModified();
}

void CProject::DeleteForm( TDclFormPtr pDclForm )
{
	// delete child forms first
	TDclFormList ChildForms;
	if( FindChildForms( pDclForm, ChildForms ) )
	{
		for( TDclFormList::reverse_iterator iter = ChildForms.rbegin(); iter != ChildForms.rend(); ++iter )
			DeleteForm( *iter );
		DeleteForm( pDclForm );
	}
	for( TDclFormList::iterator iter = mDclForms.begin(); iter != mDclForms.end(); ++iter )
	{
		if( (*iter) != pDclForm )
			continue;
		CDialogObject* pDlgObject = pDclForm->GetFormInstance();
		if( pDlgObject )
			pDlgObject->CloseDialog();
		else
		{
			CWnd* pFormWnd = pDclForm->GetFormWindow();
			if( pFormWnd ) // if the form has a view open
				pFormWnd->DestroyWindow(); // close the view
		}
		CUndoManager* pUndoManager = GetUndoManager();
		if( pUndoManager )
			pUndoManager->DeleteForm( pDclForm );
		mDclForms.erase( iter ); //remove the form from the list
		OnModified();
		break;
	}
}

TDclFormPtr CProject::AddForm( FormType nType, LPCTSTR pszName )
{
	TDclFormPtr pNewDclForm = new CDclFormObject( this, nType );
	pNewDclForm->SetUniqueName( CreateUniqueName() );
	AddInitializedForm( pNewDclForm, pszName );
	return pNewDclForm;
}

TDclFormPtr CProject::AddForm( FormType nType, TDclFormPtr pParentForm )
{
	assert( pParentForm != NULL );
	assert( pParentForm->GetProject() == (const CProject*)this );
	TDclFormPtr pNewDclForm = new CDclFormObject( this, nType );
	pNewDclForm->SetUniqueName( pParentForm->GetUniqueName() );
	pNewDclForm->SetParentForm( pParentForm );
	AddInitializedForm( pNewDclForm );
	return pNewDclForm;
}

bool CProject::FindChildForms( TDclFormPtr pParentForm, TDclFormList& ChildForms ) const
{
	bool bFoundOne = false;
	for( TDclFormList::const_iterator iter = mDclForms.begin(); iter != mDclForms.end(); ++iter )
	{
		TDclFormPtr pTestParentForm = (*iter)->GetParentForm();
		if( pParentForm == pTestParentForm )
		{
			ChildForms.push_back( *iter );
			bFoundOne = true;
		}
	}
	return bFoundOne;
}

void CProject::AddInitializedForm( TDclFormPtr pForm, LPCTSTR pszName /*= NULL*/ )
{
	if( !pForm )
		return;
	CUndoManager* pUndoManager = GetUndoManager();
	if( pUndoManager )
	{
		if( pUndoManager->enabled() )
			pUndoManager->setEnabled( false );
		else
			pUndoManager = NULL;
	}
	mDclForms.push_back( pForm );
	AddDefaultProperties( pForm->GetControlProperties(), -1, -1 ); //add properties to the new dcl form object
	if( pszName && *pszName )
	{
		TDclControlPtr pFormProps = pForm->GetControlProperties();
		pFormProps->SetStringProperty( Prop::Name, pszName );
		pFormProps->SetStringProperty( Prop::TitleBarText, pForm->GetKeyName() );
	}
	if( pUndoManager )
	{
		pUndoManager->setEnabled();
		pUndoManager->AddForm( pForm );
	}
	OnModified();
}

void CProject::SetGlobalVariableNames( LPCTSTR pszRootName /*= NULL*/ )
{
	if( pszRootName )
		SetKeyName( pszRootName );
	for( TDclFormList::const_iterator iter = mDclForms.begin(); iter != mDclForms.end(); ++iter )
		(*iter)->SetGlobalVariableName( pszRootName, true );
	OnModified();
}

void CProject::ClearGlobalVariableNames()
{
	for( TDclFormList::const_iterator iter = mDclForms.begin(); iter != mDclForms.end(); ++iter )
		(*iter)->ClearGlobalVariableName( true );
	OnModified();
}

TDclFormPtr CProject::GetRefCountedPtr( CDclFormObject* pDclForm ) const
{
	for( TDclFormList::const_iterator iter = mDclForms.begin(); iter != mDclForms.end(); ++iter )
	{
		if( (*iter) == pDclForm )
			return *iter;
	}
	return TDclFormLockedPtr( pDclForm );
}

bool CProject::AddPicture( TPicturePtr pPicture ) 
{
	assert( pPicture != NULL );
	if( !pPicture )
		return false;
	OnModified();
	UINT nID = pPicture->GetID();
	bool bFound = (mPictures.find( nID ) != mPictures.end());
	mPictures[nID] = pPicture;
	if( bFound )
	{
		//update any instances of controls or forms referencing the updated picture
		for( TDclFormList::const_iterator iterForm = mDclForms.begin(); iterForm != mDclForms.end(); ++iterForm )
		{
			const TDclControlList& Controls = (*iterForm)->GetControlList();
			for( TDclControlList::const_iterator iterControl = Controls.begin(); iterControl != Controls.end(); ++iterControl )
			{
				if( !(*iterControl)->GetControlInstance() )
					continue; //no instance of this control, so skip it
				const TPropertyList& Props = (*iterControl)->GetPropertyList();
				for( TPropertyList::const_iterator iterProp = Props.begin(); iterProp != Props.end(); ++iterProp )
				{
					if( (*iterProp)->GetType() == PropPicture && (*iterProp)->GetLongValue() == nID )
						(*iterControl)->GetControlInstance()->ApplyProperty( (*iterProp) ); //apply the updated picture
				}
			}
		}
	}
	return true;
}

void CProject::DeletePicture( UINT_PTR nID )
{
	bool bFound = (mPictures.find( nID ) != mPictures.end());
	if( !bFound )
		return;
	mPictures.erase( nID );
	OnModified();

	//remove all references to this picture before deleting it
	for( TDclFormList::const_iterator iterForm = mDclForms.begin(); iterForm != mDclForms.end(); ++iterForm )
	{
		const TDclControlList& Controls = (*iterForm)->GetControlList();
		for( TDclControlList::const_iterator iterControl = Controls.begin(); iterControl != Controls.end(); ++iterControl )
		{
			TPropertyList& Props = (*iterControl)->GetPropertyList();
			for( TPropertyList::iterator iterProp = Props.begin(); iterProp != Props.end(); ++iterProp )
			{
				if( (*iterProp)->GetType() == PropPicture && (*iterProp)->GetLongValue() == nID )
				{
					(*iterProp)->SetShortValue( -1 ); //reset the picture ID
					if( (*iterControl)->GetControlInstance() )
						(*iterControl)->GetControlInstance()->ApplyProperty( (*iterProp) ); //apply the deleted picture
				}
			}
		}
	}
}

bool CProject::ExportPictureFile( UINT_PTR nID, LPCTSTR szFile )
{
	TPicturePtr pPic = FindPicture( nID );
	if( !pPic )
		return false;
	return pPic->Export( szFile );
	return true;
}

bool CProject::LoadPictureFile( UINT_PTR nID, LPCTSTR szFile )
{
	TPicturePtr pPic = FindPicture( nID );
	if( !pPic )
	{
		pPic = new CDclPicture( nID );
		AddPicture( pPic );
	}
	pPic->LoadFile( szFile );
	return true;
}

TDclFormPtr CProject::FindDclForm( LPCTSTR pszDclFormName ) const
{
	for( TDclFormList::const_iterator iterForm = mDclForms.begin(); iterForm != mDclForms.end(); ++iterForm )
	{
		if( (*iterForm)->GetKeyName().CompareNoCase( pszDclFormName ) == 0 )
			return (*iterForm);
	}
	return NULL;
}

TDclFormPtr CProject::FindDclFormWithVarName( LPCTSTR pszVarName ) const
{
	for( TDclFormList::const_iterator iterForm = mDclForms.begin(); iterForm != mDclForms.end(); ++iterForm )
	{
		if( (*iterForm)->GetVarName().CompareNoCase( pszVarName ) == 0 )
			return (*iterForm);
	}
	return NULL;
}

TDclControlPtr CProject::FindControlWithVarName( LPCTSTR pszVarName ) const
{
	for( TDclFormList::const_iterator iterForm = mDclForms.begin(); iterForm != mDclForms.end(); ++iterForm )
	{
		TDclControlPtr pControl = (*iterForm)->FindControlWithVarName( pszVarName );
		if( pControl )
			return pControl;
	}
	return NULL;
}

TDclFormPtr CProject::FindParentDclForm( LPCTSTR pszParentUniqueName ) const
{
	for( TDclFormList::const_iterator iterForm = mDclForms.begin(); iterForm != mDclForms.end(); ++iterForm )
	{
		if( (*iterForm)->GetUniqueName() == pszParentUniqueName )
			return (*iterForm);
	}
	return NULL;
}

TDclFormPtr CProject::FindDclTabChildForm( LPCTSTR pszParentUniqueName, size_t nTabIndex ) const
{
	for( TDclFormList::const_iterator iterForm = mDclForms.begin(); iterForm != mDclForms.end(); ++iterForm )
	{
		if ((*iterForm)->GetParentName() == pszParentUniqueName && (*iterForm)->GetTabIndex() == nTabIndex)
			return (*iterForm);
	}
	return NULL;
}

HBITMAP CProject::CloneBitmap( UINT nID, CSize &sz ) const
{
	TPicturePtr pPicture = FindPicture( nID );
	if( !pPicture )
		return NULL;
	sz.SetSize( pPicture->GetWidth(), pPicture->GetHeight() );
	return pPicture->CloneBitmap();
}

HICON CProject::CloneIcon(UINT nID) const
{
	TPicturePtr pPicture = FindPicture( nID );
	if( !pPicture )
		return NULL;
	return pPicture->CloneIcon();
}

TPicturePtr CProject::FindPicture( UINT nID ) const
{
	TPictureMap::const_iterator iter = mPictures.find( nID );
	if( iter != mPictures.end() )
		return iter->second;
	return NULL;
}

bool CProject::GetPictureSize( UINT nID, CSize& size ) const
{
	TPicturePtr pPicture = FindPicture( nID );
	if( !pPicture )
		return false;
	if (!pPicture->IsValid())
		return false;
	size.cx = pPicture->GetWidth();
	size.cy = pPicture->GetHeight();
	return true;
}


bool CProject::IsInUse() const
{
	for( TDclFormList::const_iterator iterForm = mDclForms.begin(); iterForm != mDclForms.end(); ++iterForm )
	{
		if( (*iterForm)->GetFormInstance() )
			return true; //there is an active dialog using this form
	}
	return false;
}


IOStatus CProject::ReadFromFile( LPCTSTR pszFilePath )
{
	DisableUndoManager DisableUndo( GetUndoManager() );
	CString sExt = CString( pszFilePath ).Right( 4 );
	if( sExt.CompareNoCase( _T(".lsp") ) == 0 )
	{
		try
		{
			CStdioFile SrcFile( pszFilePath, CFile::modeRead );
			if (!SrcFile)
				return statFileNotFound;

			msProjectFilePath = SrcFile.GetFilePath();
			msBaseFileName = StripPathFromFileName( msProjectFilePath );

			CStringA sRawData;
			size_t cbData = (size_t)SrcFile.GetLength();
			static const WORD wUnicodeSentinel = 0xfeff;
			WORD wSentinel = 0;
			SrcFile.Read( &wSentinel, sizeof(wSentinel) );
			if( wSentinel == wUnicodeSentinel )
			{
				cbData -= sizeof(wSentinel);
				CStringW sDataW;
				cbData = SrcFile.Read( sDataW.GetBuffer( cbData / sizeof(WCHAR) ), cbData );
				sDataW.ReleaseBufferSetLength( cbData / sizeof(WCHAR) );
				if( cbData == 0 )
					return statReadFailed;
				sRawData = sDataW;
			}
			else
			{
				SrcFile.SeekToBegin();
				UINT cchData = SrcFile.Read( sRawData.GetBuffer( (int)SrcFile.GetLength() ), (UINT)SrcFile.GetLength() );
				sRawData.ReleaseBufferSetLength( cchData );
				if( cchData == 0 )
					return statReadFailed;
			}
			std::string sData = base64_decode( (LPCSTR)sRawData );
			if( sData.length() == 0 )
				return statReadFailed;
			CMemFile Data( sData.length() );
			Data.Write( sData.c_str(), sData.length() );
			Data.SeekToBegin();
			try
			{
				CArchiveEx ar( &Data, CArchive::load, NULL, _T("ObjectDCL"), TRUE);
				Serialize( ar );
				ar.Close();
			}
			catch(CException* e)
			{
				e->Delete();
				return statInvalidFormat;
			}
			catch(...)
			{
				return statInvalidFormat;
			}
		}
		catch(CException* e)
		{
			e->Delete();
			return statFileNotFound;
		}
		catch(...)
		{
			return statFileNotFound;
		}
		return statOK;
	}

	CFile SrcFile;
	try
	{
		SrcFile.Open( pszFilePath, CFile::modeRead | CFile::shareDenyWrite | CFile::osSequentialScan );
		if (!SrcFile)
			return statFileNotFound;

		msProjectFilePath = SrcFile.GetFilePath();
		msBaseFileName = StripPathFromFileName( msProjectFilePath );

		if( CheckSignature( SrcFile, "ObjectDCL Project" ) ||
				CheckSignature( SrcFile, "OpenDCL Project" ) )
		{ //this file is in plain text format
			SrcFile.Close();
			IOStatus stat = ReadFromTextFile( pszFilePath );
			if( msKeyName.IsEmpty() )
				SetKeyName( msBaseFileName.SpanExcluding( _T(".") ) );
			return stat;
		}

		SrcFile.SeekToBegin();
		try
		{
			CArchiveEx archSource(&SrcFile, CArchive::load | CArchive::bNoFlushOnDelete, NULL, _T("ObjectDCL"), TRUE);
			archSource.m_pDocument = theWorkspace.GetActiveDocument();
			archSource.m_bForceFlat = FALSE;
			if( SrcFile.GetLength() != 0 )
				Serialize(archSource);     // load me
			if( msKeyName.IsEmpty() )
				SetKeyName( msBaseFileName.SpanExcluding( _T(".") ) );
		}
		catch(CException* e)
		{
			e->Delete();
			return statInvalidFormat;
		}
		catch(...)
		{
			SrcFile.Close();
			return statInvalidFormat;
		}
	}
	catch(CException* e)
	{
		e->Delete();
		return statFileNotFound;
	}
	catch(...)
	{
		SrcFile.Close();
		return statFileNotFound;
	}
	SrcFile.Close();
	return statOK;
}


IOStatus CProject::WriteToFile( LPCTSTR pszFilePath )
{
	try
	{
		CString sExt = CString( pszFilePath ).Right( 4 );
		if( sExt.CompareNoCase( _T(".lsp") ) == 0 )
		{
			CStdioFile DestFile( pszFilePath, CFile::modeWrite | CFile::typeBinary | CFile::shareExclusive | CFile::modeCreate );
			if (!DestFile)
				return statFileNotFound;
			CMemFile Data( 0x10000 );
			CArchiveEx ar( &Data, CArchive::store | CArchive::bNoFlushOnDelete, NULL, _T("ObjectDCL"), TRUE);
			Serialize( ar );
			ar.Close();
			UINT cbData = (UINT)Data.GetLength();
			if( cbData == 0 )
				return statWriteFailed;
			BYTE* pbData = Data.Detach();
			CStringA sRawData( base64_encode( pbData, cbData ).c_str() );
			Data.Attach( pbData, cbData );
			Data.Close();
			sRawData.Replace( "\r\n", "\"\r\n\"" );
			CStringA sFormattedData;
			sFormattedData.Format( "'(\"%s\")", (LPCSTR)sRawData );
			DestFile.Write( (LPCSTR)sFormattedData, sFormattedData.GetLength() );
			DestFile.Flush();
		}
		else
		{
			CFile DestFile( pszFilePath, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate );
			if (!DestFile)
				return statFileNotFound;
			{
				CArchiveEx archSave(&DestFile, CArchive::store, NULL, _T("ObjectDCL"), TRUE);
				Serialize(archSave);
				archSave.Close();
				msProjectFilePath = DestFile.GetFilePath();
			}
			DestFile.Flush();
		}
	}
	catch(CException* e)
	{
		e->Delete();
		return statFail;
	}
	catch(...)
	{
		return statFail;
	}
	return statOK;
}


// These classes are used for deserializing OLE controls from versions 8/9 and 10/11/12 project archives
// (the class must have the same name as the original, since the original code used MFC's
// typesafe serialization mechanism, which writes the class name to the archive) [ORW]
class CArxControlObject : public CObject
{ CDclControlTemplate mCtrl; public: CArxControlObject() : mCtrl(NULL) {} DECLARE_SERIAL(CArxControlObject);	virtual void Serialize(CArchive& ar);	};
	IMPLEMENT_SERIAL(CArxControlObject, CObject, 1);
	void CArxControlObject::Serialize(CArchive& ar) { mCtrl.Serialize(ar); }
class CDclControlObject : public CArxControlObject
{ public: CDclControlObject() {} DECLARE_SERIAL(CDclControlObject); };
	IMPLEMENT_SERIAL(CDclControlObject, CObject, 1);


void CProject::Serialize(CArchive& ar)
{
	BYTE nThisVersion = GetCurrentSaveVersion();

	if (ar.IsStoring())
	{
		ar << nThisVersion;
		ar << msPassword;
		ar << msLispFileName;
		ar << msKeyName; //project key

		ar << unsigned long(mDclForms.size());
		ar << mnAutoCADVersion;

		for( TDclFormList::const_iterator iter = mDclForms.begin(); iter != mDclForms.end(); ++iter )
		{
			(*iter)->Serialize(ar);
		#ifdef _DEBUG
			(*iter)->dumpDebugger( false );
		#endif
		}

		ar << unsigned long(mPictures.size());
		for( TPictureMap::const_iterator iter = mPictures.begin(); iter != mPictures.end(); ++iter )
			iter->second->Serialize( ar );
	}
	else
	{		
		ar >> nThisVersion;
		if( nThisVersion <= 14 )
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

		if( nThisVersion < 12 )
		{
			BOOL bHasPassword;
			ar >> bHasPassword; //discard
			// this is a fix for an older password protect scheme.
			if (nThisVersion != 2 && nThisVersion != 3)
				msPassword.Empty();
		}
		ar >> msPassword;		
		ar >> msLispFileName;

		// lets check here if we need to add the lisp file extension
		CString sNone = theWorkspace.LoadResourceString(IDS_NONE);
		if (!msLispFileName.IsEmpty())
		{
			if (msLispFileName == sNone ||
					msLispFileName.CompareNoCase( _T(".lsp") ) == 0 ||
					msLispFileName.CompareNoCase( sNone + _T(".lsp") ) == 0 )
				msLispFileName.Empty();
			else if (msLispFileName.Right(4).CompareNoCase(_T(".lsp")) != 0)
				msLispFileName += _T(".lsp");
		}

		CString sKeyName;
		if (nThisVersion >= 10)
			ar >> sKeyName; //this is the project key name
		else
		{
			if (nThisVersion >= 5)
				ar >> sKeyName; //this is the full project file path as saved
			else
				sKeyName = msLispFileName;
			sKeyName = StripPathFromFileName(sKeyName).SpanExcluding(_T("."));
			sKeyName.Replace( _T(' '), _T('_') );
		}
		if( msKeyName.IsEmpty() ) //if it's already set, don't change it
			SetKeyName( sKeyName );

		if( nThisVersion < 12 && nThisVersion >= 6 )
		{
			CString sDistFileName;
			ar >> sDistFileName; //discard
		}

		// get counter for dcl forms
		unsigned long ctForms = 0;
		if( nThisVersion < 11 )
		{
			short nCount;
			ar >> nCount;
			ctForms = (unsigned long)nCount;
		}
		else
			ar >> ctForms;
		
		if( nThisVersion < 12 )
		{
			int nPurchaseState;
			ar >> nPurchaseState; //discard
		}

		if (nThisVersion >= 4)
			ar >> mnAutoCADVersion;
		else
			// otherwise set the default to release nDeAcadVersion
			mnAutoCADVersion = theWorkspace.GetMinSupportedAcadVersion();

		// do loop to navigate dcl forms
		while (ctForms-- > 0)
		{
			TDclFormPtr pDclForm = new CDclFormObject( this );
			pDclForm->Serialize(ar);

			if( pDclForm->GetType() == FrmTabPage )
			{
				//Some projects for whatever reason have parentless tab page forms. This code removes
				//those forms so they don't confuse the project list in the editor. [ORW]
				if( !pDclForm->GetParentForm() )
					continue;

				//Re-index tab pages with a duplicate tab index, otherwise the duplicate will appear
				//as a phantom tab.
				TDclFormList SiblingTabPages;
				if( FindChildForms( pDclForm->GetParentForm(), SiblingTabPages ) )
				{
					bool bDuplicateIndex = false;
					for( TDclFormList::const_iterator iter = SiblingTabPages.begin(); iter != SiblingTabPages.end(); ++iter )
					{
						TDclFormPtr pForm = (*iter);
						if( pForm->GetTabIndex() == pDclForm->GetTabIndex() )
						{
							bDuplicateIndex = true;
							break;
						}
					}
					if( bDuplicateIndex )
						continue; //just ignore the dupe
				}
			}

			mDclForms.push_back(pDclForm);
		#ifdef _DEBUG
			pDclForm->dumpDebugger( false );
		#endif
		}

		if (nThisVersion >= 7)
		{
			if( nThisVersion <= 9 )
			{
				CStringArray rsActiveXFiles;
				rsActiveXFiles.Serialize(ar); //discard it
			}
			else if( nThisVersion <= 13 )
			{
				unsigned long ctAxFiles;
				ar >> ctAxFiles;
				for( unsigned long idx = 0; idx < ctAxFiles; ++idx )
				{
					CString sFile;
					ar >> sFile; //discard it
				}
			}
		}

		if( nThisVersion >= 8 )
		{
			if( nThisVersion <= 9 )
			{
				CTypedPtrList< CObList, CArxControlObject* > listDiscardOleControls;
				listDiscardOleControls.Serialize(ar);
				POSITION pos = listDiscardOleControls.GetHeadPosition();
				while( pos )
					delete listDiscardOleControls.GetNext( pos );
			}
			else if( nThisVersion <= 12 )
			{
				unsigned long ctOleControls;
				ar >> ctOleControls;
				CDclControlObject discard;
				for( unsigned long idx = 0; idx < ctOleControls; ++idx )
					discard.Serialize( ar );
			}
		}

		mPictures.clear();
		if( nThisVersion < 9 )
		{
			try
			{
				CList< CDclPicture* > Pictures;
				Pictures.Serialize( ar );
				POSITION pos = Pictures.GetHeadPosition();
				while( pos )
				{
					CDclPicture* pPict = Pictures.GetNext( pos );
					if( !pPict )
						continue; //skip empty picture objects
					if( pPict->GetID() <= -1 || pPict->GetID() >= 3000 )
					{ //looks corrupted
						//delete pPict;
						continue;
					}
					mPictures[pPict->GetID()] = pPict;
				}
			}
			catch(...) {}
		}
		else
		{
			unsigned long nCount = 0;
			if( nThisVersion < 10 )
			{
				short sCount;
				ar >> sCount;
				nCount = (unsigned long)sCount;
			}
			else
				ar >> nCount;

			while( nCount-- > 0 )
			{
				TPicturePtr pPict = new CDclPicture( -1 );
				pPict->Serialize( ar );
				mPictures[pPict->GetID()] = pPict;							
			}
		}
	}
}

IOStatus CProject::ReadFromTextFile( LPCTSTR lpszFilePath ) 
{
	IOStatus stat = statOK;
	std::ifstream sFile(lpszFilePath, std::ios::in);
	InitFilerGlobals(); //Init the globals before reading anything from the file.
	CStringA sSig = readLine(sFile);
	if (sSig != "OpenDCL Project" && sSig != "ObjectDCL Project")
		stat = statInvalidFormat;
	else
	{
		int nThisVersion;
		if (!readInt(sFile, nThisVersion))
			stat = statInvalidFormat;
		else
		{
			switch (nThisVersion)
			{
				case 9 : stat = ReadFromTextFile9(sFile, lpszFilePath);
					break;
				default : stat = statInvalidFormat;
			}
		}
	}
	sFile.close();
	return stat;
}

IOStatus CProject::ReadFromTextFile(std::ifstream &sFile, const CString &fileName)
{
	//Init the globals before reading anything from the file.
	InitFilerGlobals();

	if (readLine(sFile) != "OpenDCL Project") return statInvalidFormat;

	int nThisVersion;
	if (!readInt(sFile, nThisVersion)) return statInvalidFormat;

	switch (nThisVersion) {
		case 9 : return ReadFromTextFile9(sFile, fileName);
	}
	return statInvalidFormat;
}

IOStatus CProject::ReadFromTextFile9(std::ifstream &sFile, const CString &fileName)
{
	int nCount;

	BOOL bHasPassword;
	if (!readBOOL(sFile, bHasPassword)) return statInvalidFormat; //discard
	CStringA sPassword;
	if (!readString(sFile, sPassword)) return statInvalidFormat;
	msPassword = sPassword;
	CStringA sLispFileName;
	if (!readString(sFile, sLispFileName)) return statInvalidFormat;
	msLispFileName = sLispFileName;
	CString sNone = theWorkspace.LoadResourceString(IDS_NONE);
	if (!msLispFileName.IsEmpty())
	{
		if (msLispFileName == sNone ||
				msLispFileName.CompareNoCase( _T(".lsp") ) == 0 ||
				msLispFileName.CompareNoCase( sNone + _T(".lsp") ) == 0 )
			msLispFileName.Empty();
		else if (msLispFileName.Right(4).CompareNoCase(_T(".lsp")) != 0)
			msLispFileName += _T(".lsp");
	}

	// this is the original project file path; use it to construct the project key
	CStringA sKeyNameA;
	if (!readString(sFile, sKeyNameA)) return statInvalidFormat;
	if (!sKeyNameA.IsEmpty())
	{
		CString sKeyName( sKeyNameA );
		sKeyName = StripPathFromFileName(sKeyName).SpanExcluding(_T("."));
		sKeyName.Replace( _T(' '), _T('_') );
		SetKeyName( sKeyName );
	}

	// this is used for a distribution file with multiple projects in it.
	CStringA sDistFileName;
	if (!readString(sFile, sDistFileName)) return statInvalidFormat; //discard

	// get counter for dcl forms
	if (!readInt(sFile, nCount)) return statInvalidFormat;
	int nPurchaseState;
	if (!readInt(sFile, nPurchaseState)) return statInvalidFormat; //discard

	if (!readInt(sFile, (int&)mnAutoCADVersion)) return statInvalidFormat;

	// do loop to navigate dcl forms
	while (nCount-- > 0)
	{
		// get current Dcl form
		TDclFormPtr pDclForm = new CDclFormObject( this );

		// get dcl form into archive
		IOStatus stat = pDclForm->ReadFromTextFile(sFile, fileName);
		if (stat != statOK) return stat;

		// add this dcl form to the list object
		mDclForms.push_back(pDclForm);

	}	

	int iCount;
	if (!readInt(sFile, iCount)) return statInvalidFormat;
	for (int i = 0; i < iCount; i++) {
		CStringA str;
		if (!readString(sFile, str)) return statInvalidFormat;
	}

	if (!readInt(sFile, iCount)) return statInvalidFormat;
	CDclControlTemplate* pOleControl = new CDclControlTemplate( NULL );
	for (int i = 0; i < iCount; i++) {
		IOStatus stat = pOleControl->ReadFromTextFile(sFile, fileName);
		if (stat != statOK) return stat;
		delete pOleControl;
	}

	// set counter
	if (!readInt(sFile, nCount)) return statInvalidFormat;

	mPictures.clear();
	// do loop to navigate images
	while (nCount-- > 0)
	{
		// get current images
		TPicturePtr pPict = new CDclPicture( -1 );
		if( !pPict )
			continue;
		try
		{	
			// get image into archive
			IOStatus stat = pPict->ReadFromTextFile(sFile, fileName);
			if (stat != statOK) return stat;

			// add this image to the list object
			mPictures[pPict->GetID()] = pPict;
		}
		catch(...)
		{
			// do nothing
		}
	}
	return statOK;
}


#ifdef _DIAGNOSTIC
LPCTSTR CProject::toString() const
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("CProject [%s] (%s forms, %s images)"),
																	 (LPCTSTR)GetKeyName(),
																	 asString( mDclForms.size() ),
																	 asString( mPictures.size() ) );
	return buf;
}

void CProject::dump( bool bDeep /*= true*/ ) const
{
	CString sOut;
	sOut.Format( _T("############################################################\r\n%s\r\n"), toString() );
	theWorkspace.DisplayStatus( sOut );
	if( !bDeep )
		return;
	for( TDclFormList::const_iterator iter = mDclForms.begin(); iter != mDclForms.end(); ++iter )
		(*iter)->dump( true );
	sOut.Format( _T("#####    End of project [%s]    #####\r\n"), (LPCTSTR)GetKeyName() );
	theWorkspace.DisplayStatus( sOut );
}
#endif


#ifdef _DEBUG
void CProject::dumpDebugger( bool bDeep /*= true*/ ) const
{
	TraceFmt( _T("############################################################\r\n%s\r\n"), toString() );
	if( !bDeep )
		return;
	for( TDclFormList::const_iterator iter = mDclForms.begin(); iter != mDclForms.end(); ++iter )
		(*iter)->dumpDebugger( true );
	TraceFmt( _T("#####    End of project [%s]    #####\r\n"), (LPCTSTR)GetKeyName() );
}
#endif
