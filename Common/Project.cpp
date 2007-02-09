// CProject.cpp : implementation file
//

#include "stdafx.h"
#include "Project.h"
#include "PropertyIds.h"
#include "SharedRes.h"
#include "DclControlObject.h"
#include "OleControlObject.h"
#include "PropertyObject.h"
#include "VarUtils.h"
#include "ArchiveEx.h"
#include "AxContainer.h"
#include "StgFile.h"
#include "Filing.h"
#include "DclFormObject.h"
#include "PictureObject.h"
#include "ControlTypes.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxMethodDescriptor.h"
#include "PurchaseMode.h"
#include "ProjectCollection.h"
#include <fstream>
#include <stdio.h>

#define MAX_CALLING_ARGUMENTS 16

#define RELEASE(lpUnk) do \
	{ if ((lpUnk) != NULL) { (lpUnk)->Release(); (lpUnk) = NULL; } } while (0)

#define DELETE_EXCEPTION(e) do { e->Delete(); } while (0)


static CString LTOA(int nVal)
{
  CString sLong;
	sLong.Format(_T("%d"), nVal);
  return sLong;
}


static CString StripPathFromFileName( LPCTSTR pszFilePath )
{
	CString sShortName = pszFilePath;
	sShortName.MakeReverse();
	sShortName = sShortName.SpanExcluding(_T("\\/:"));
	sShortName.MakeReverse();
	return sShortName;
}

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
// CProject
IMPLEMENT_SERIAL(CProject, CObject, 1)

CProject::CProject()
{
	Initialize();
}

CProject::CProject( LPCTSTR pszKeyName )
: msKeyName( pszKeyName )
{
	Initialize();
}

void CProject::Initialize()
{
  m_ActiveXFiles.RemoveAll();
  m_ActiveXFiles.SetSize(0,1);
  m_nAutoCADVersion = theWorkspace.GetMinSupportedAcadVersion();
  m_bFreeVersion = false;
  m_bHasPassword = FALSE;
  m_sPassword = CString();
  m_LispFileName = CString();	
  m_DistFileName = CString();
  sDclFormCopiedFrom = CString();
  m_PurchaseState = 0;

	CWinApp* pApp = AfxGetApp();
  CString sTitle;
  sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);
  CString sText;

  sText = theWorkspace.LoadResourceString(IDS_DefaultFontName);
  m_sDefaultFontName = pApp->GetProfileString( sTitle, sText, NULL);

  sText = theWorkspace.LoadResourceString(IDS_DefaultFontSize);
  m_nDefaultFontSize = pApp->GetProfileInt( sTitle, sText, 0);

  sText = theWorkspace.LoadResourceString(IDS_DefaultFontItalic);
  m_bDefaultFontItalic = pApp->GetProfileInt( sTitle, sText, 0);

  sText = theWorkspace.LoadResourceString(IDS_DefaultFontUnderLine);
  m_bDefaultFontUnderLine = pApp->GetProfileInt( sTitle, sText, 0);

  sText = theWorkspace.LoadResourceString(IDS_DefaultFontBold);
  m_bDefaultFontBold = pApp->GetProfileInt( sTitle, sText, 0);

  sText = theWorkspace.LoadResourceString(IDS_DefaultFontSizeStyle);
  m_bDefaultFontSizeStyle = pApp->GetProfileInt( sTitle, sText, 1);

  sText = theWorkspace.LoadResourceString(IDS_DEFAULTFONT);
  if (m_sDefaultFontName.IsEmpty() || m_sDefaultFontName.GetLength() == 0)
    m_sDefaultFontName = sText;

  if (m_nDefaultFontSize == 0)
    m_nDefaultFontSize = 8;	
}


CProject::~CProject()
{
  // clear the project
  ClearProject();

  // create a position variable to hold the counter increment
	POSITION pos = mClipBoard.GetHeadPosition();	
  while(pos)
  {
		POSITION posAt = pos;
    CDclControlObject* pControl = mClipBoard.GetNext(pos);
    pControl->ClearProperties(); // clear properties in this control
    mClipBoard.RemoveAt(posAt);
    delete pControl;
  }
}

void CProject::SetPurchaseMode(int nPurchaseMode)
{
  m_PurchaseState = nPurchaseMode;
}

void CProject::SetKeyName( LPCTSTR pszKeyName )
{
	if( !pszKeyName )
		return;
	{
		CString sKey = pszKeyName;
		sKey.Replace(_T(' '), _T('_'));
		msKeyName = sKey;
	}
}

CString CProject::QueryForLispFileName() 
{
  CString strResult;

  CString sFilter;
  sFilter = theWorkspace.LoadResourceString(IDS_AUTOLISPFILE);

  // create the open dialog box
  CFileDialog BrowseWnd(
    TRUE, 
    NULL,
    NULL, 
    OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST,
    sFilter,
    CWnd::GetActiveWindow());

  // proceed to setup the file buffer size
  BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
  TCHAR* pc = new TCHAR[MAX_PATH];
  BrowseWnd.m_ofn.lpstrFile = pc;
  BrowseWnd.m_ofn.lpstrFile[0] = NULL;

  // call method to invoke the file dialog box
  int iReturn = BrowseWnd.DoModal();

  if(iReturn == IDOK)
  {
    strResult = BrowseWnd.GetPathName();
    delete pc;
  }
  else
  {
    strResult = CString();
    delete pc;
    return strResult;
  }

  // update the lsp file name
  m_LispFileName = strResult;

  // lets check here if we need to add the ods file extension
  strResult.MakeLower();

  CString sText;
  sText = theWorkspace.LoadResourceString(IDS_DOTLSP);

  if (strResult.Right(4) != sText)
    m_LispFileName += sText;	

  return strResult;	
}

CString CProject::QueryForOdsFileName() 
{
  CString strResult;

  CString sFilter;
  sFilter = theWorkspace.LoadResourceString(IDS_ODSFILTER);

  // create the open dialog box
  CFileDialog BrowseWnd(
    FALSE, 
    NULL,
    NULL, 
    OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST,
    sFilter,
    CWnd::GetActiveWindow());

  // proceed to setup the file buffer size
  BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
  TCHAR* pc = new TCHAR[MAX_PATH];
  BrowseWnd.m_ofn.lpstrFile = pc;
  BrowseWnd.m_ofn.lpstrFile[0] = NULL;

  // call method to invoke the file dialog box
  int iReturn = BrowseWnd.DoModal();

  if(iReturn == IDOK)
  {
    strResult = BrowseWnd.GetPathName();
    delete pc;
  }
  else
  {
    strResult = CString();
    delete pc;
    return strResult;
  }

  // update the lsp file name
  m_DistFileName = strResult;

  // lets check here if we need to add the ods file extension
  strResult.MakeLower();

  CString sText;
  sText = theWorkspace.LoadResourceString(IDS_DOTODS);

  if (strResult.Right(4) != sText)
    m_DistFileName += sText;


  return strResult;	
}

void CProject::ClearProject()
{
  //msKeyName.Empty(); //this should remain unchanged for the life of the project!
	m_DistFileName.Empty();
	m_LispFileName.Empty();
	m_ShortFileName.Empty();
	sDclFormCopiedFrom.Empty();
	m_LoadSingleDialog.Empty();
	m_sPassword.Empty();
	m_bHasPassword = FALSE;
	m_bFreeVersion = false;
	m_nAutoCADVersion = theWorkspace.GetMinSupportedAcadVersion();

	// clear dcl forms
  POSITION posDclForm = mDclForms.GetHeadPosition();	
	while (posDclForm)
	{
    CDclFormObject* pDclFormObject = mDclForms.GetNext(posDclForm);
		if (pDclFormObject)
			pDclFormObject->ClearControls();
    delete pDclFormObject;
	}
	mDclForms.RemoveAll();

  // clear pictures
  POSITION posPicture = mPictures.GetHeadPosition();	
	while (posPicture)
		try{ delete mPictures.GetNext(posPicture); } catch(...){}
	mPictures.RemoveAll();

  // clear OLE controls
  POSITION posOleControl = mOleControls.GetHeadPosition();	
	while (posOleControl)
	{
    CDclControlObject* pControl = mOleControls.GetNext(posOleControl);
		if (pControl)
		{
			pControl->ClearProperties();
			pControl->ClearStream();
		}
    delete pControl;
	}
	mOleControls.RemoveAll();

	mClipBoard.RemoveAll();
  m_ActiveXFiles.RemoveAll();
}

void CProject::ClearR14Events()
{
  POSITION pos = mDclForms.GetHeadPosition();	
	while (pos)
	{
    CDclFormObject* pDclFormObject = mDclForms.GetNext(pos);
		if (pDclFormObject)
			pDclFormObject->ClearR14Events();
    delete pDclFormObject;
	}
}

void CProject::AddOleObject(const CLSID& clsid, CAxContainer *pAxCont)
{
	if (HasOleObject (clsid))
		return;
  COleControlObject *pObject = new COleControlObject( this, clsid );

  if (clsid == IID_IPictureDisp)
    pObject->SetAxTypeName( theWorkspace.LoadResourceString(IDS_PROP_PICTURE) );
  else if (pObject->m_clsid == IID_IFontDisp)
    pObject->SetAxTypeName( theWorkspace.LoadResourceString(IDS_PROP_LABEL_NAME) );
  else if (pObject->m_clsid == GUID_COLOR)
    pObject->SetAxTypeName( theWorkspace.LoadResourceString(IDS_COLOR) );
  mOleControls.AddTail(pObject);	
  pAxCont->ExtractComponentsFromTLB(pObject, clsid);
}

bool CProject::HasOleObject(const CLSID& clsid)
{
  POSITION pos = mOleControls.GetHeadPosition();
  while (pos != NULL)
  {
    CDclControlObject *pObject = mOleControls.GetNext(pos);
		assert( pObject != NULL );
    if (pObject != NULL && pObject->m_clsid == clsid)
      return true;
  }
  return false;
}

CDclControlObject* CProject::GetOleObject(const CLSID& clsid)
{
  POSITION pos = mOleControls.GetHeadPosition();
  while (pos != NULL)
  {
    CDclControlObject *pObject = mOleControls.GetNext(pos);
		assert( pObject != NULL );
    if (pObject != NULL && pObject->m_clsid == clsid)
      return pObject;
  }
  return NULL;
}

CString CProject::GetOleObjectName(const AxPropertyDescriptor *pProperty)
{
  if (pProperty == NULL)
    return CString();

  POSITION pos = mOleControls.GetHeadPosition();
  while (pos != NULL)
  {
    CDclControlObject *pObject = mOleControls.GetNext(pos);
		assert( pObject != NULL );
    if (pObject != NULL)
    {
      if (pObject->m_clsid == pProperty->Guid)
				return pObject->GetAxTypeName();
      for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
      {
        if (pObject->m_clsid == pProperty->CallingArgClsids[i])
          return pObject->GetAxTypeName();
      }	
    }
  }
  return theWorkspace.LoadResourceString(IDS_OleObject);
}


CDclControlObject *CProject::GetOleObject(const AxEventDescriptor *pEvent)
{	
  if (pEvent == NULL)
    return NULL;

  POSITION pos = mOleControls.GetHeadPosition();
  while (pos != NULL)
  {
    CDclControlObject *pObject = mOleControls.GetNext(pos);
		assert( pObject != NULL );
    if (pObject != NULL)
    {
      for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
      {
        if (pEvent->CallingArgs[i] == VT_DISPATCH || pEvent->CallingArgs[i] == VT_UNKNOWN)
        {
          if (pObject->m_clsid == pEvent->CallingArgClsids[i] )
            return pObject;
        }
      }	
    }
  }
  return NULL;
}

CDclControlObject *CProject::GetOleObject(const AxPropertyDescriptor *pProperty)
{	
  if (pProperty == NULL)
    return NULL;

  POSITION pos = mOleControls.GetHeadPosition();
  while (pos != NULL)
  {
    CDclControlObject *pObject = mOleControls.GetNext(pos);
		assert( pObject != NULL );
    if (pObject != NULL)
    {
      if (pObject->m_clsid == pProperty->Guid)
        return pObject;
      for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
      {
        if (pObject->m_clsid == pProperty->CallingArgClsids[i])
          return pObject;
      }	
    }
  }
  return NULL;
}

CDclControlObject *CProject::GetOleObject(const AxMethodDescriptor *pMethod)
{	
  if (pMethod == NULL)
    return NULL;

  POSITION pos = mOleControls.GetHeadPosition();
  while (pos != NULL)
  {
    CDclControlObject *pObject = mOleControls.GetNext(pos);
		assert( pObject != NULL );
    if (pObject != NULL)
    {
      if (pObject->m_clsid == pMethod->ReturnGuid)
        return pObject;
      for (int i=0; i<MAX_CALLING_ARGUMENTS+1; i++)
      {
        if (pObject->m_clsid == pMethod->CallingArgClsids[i])
          return pObject;
      }	
    }
  }
  return NULL;
}

CDclFormObject* CProject::GetDclForm(CString DclFormName)
{
  POSITION pos;
  int nCount = 0;

  while (nCount < mDclForms.GetCount())
  {
    // always get the first index when clearing a list
    pos = mDclForms.FindIndex(nCount);

    // get the object
    CDclFormObject *pDclForm = mDclForms.GetAt(pos);

    // if this dcl form's name matches the one queried for
    if (pDclForm->GetKeyName() == DclFormName)
      // return the dcl form pointer
      return pDclForm;

    // increment the counter to include the next object
    nCount++;
  }
  return NULL;
}


CDclFormObject* CProject::GetParentDclForm(CString sParentName)
{
	POSITION pos = mDclForms.GetHeadPosition();
	while (pos)
	{
		CDclFormObject* pDclForm = mDclForms.GetNext(pos);
    if (pDclForm->GetUniqueName() == sParentName)
      return pDclForm;
	}
  return NULL;
}


CDclFormObject* CProject::GetDclTabChildForm(CString sDclParentUniqueName, int nTabIndex)
{
	POSITION pos = mDclForms.GetHeadPosition();
	while (pos)
	{
		CDclFormObject* pDclForm = mDclForms.GetNext(pos);
    if (pDclForm->GetParentName() == sDclParentUniqueName && pDclForm->GetTabIndex() == nTabIndex)
      return pDclForm;
	}
  return NULL;
  return NULL;
}

int CProject::CountDeletedForms()
{
  // set counter for mDclForms
  int nCount = mDclForms.GetCount();
  int nDeleted = 0;

  // set start position for navigating mDclForms
  POSITION pos = mDclForms.GetHeadPosition();

  // do loop to navigate mDclForms
  while (pos != NULL)
  {
    // get current mDclForms
    CDclFormObject *pDclForm = mDclForms.GetNext(pos);

    if (pDclForm->m_bDeleted)
      nDeleted++;

    // increment counter
    nCount--;
  }

  return nDeleted;
}


HBITMAP CProject::GetBitmap(UINT nID, CSize &sz) const
{
	HBITMAP hBmp = NULL;
	POSITION pos = mPictures.GetHeadPosition();
	while (pos != NULL)
	{
		// get current Picture in list
		CPictureObject* pPicture = mPictures.GetNext(pos);
		if (pPicture != NULL && pPicture->GetID() == nID)
		{
			if (pPicture->GetPicture().m_pPict != NULL)
			{
				// get the bitmap
				if (pPicture->GetPicType() == PICTYPE_BITMAP)
				{
					pPicture->GetPicture().m_pPict->get_Handle((OLE_HANDLE FAR *) &hBmp);										
					sz.cx = pPicture->GetWidth();
					sz.cy = pPicture->GetHeight();
				}
				if (pPicture->GetBitmap())
					return pPicture->GetBitmap();
			}
		}
	}
	return hBmp;
}


HICON CProject::GetIcon(UINT nID) const
{
	HICON hIcon = NULL;
	POSITION pos = mPictures.GetHeadPosition();
	while (pos != NULL)
	{
		// get current Picture in list
		CPictureObject* pPicture = mPictures.GetNext(pos);
		if (pPicture != NULL && pPicture->GetID() == nID)
		{
			if (pPicture->GetPicture().m_pPict != NULL)
			{
				// get the bitmap
				if (pPicture->GetPicType() == PICTYPE_ICON)
					pPicture->GetPicture().m_pPict->get_Handle((OLE_HANDLE FAR *) &hIcon);		
			}
			if (pPicture->GetIcon() != NULL)
				return pPicture->GetIcon();					
			if (pPicture->m_hLoadedIcon != NULL)
				return pPicture->m_hLoadedIcon;					
			return hIcon;
		}
	}
	return hIcon;
}


bool CProject::GetPictureSize( UINT nID, CSize& size ) const
{
	POSITION pos = mPictures.GetHeadPosition();
	while (pos != NULL)
	{
		CPictureObject* pPicture = mPictures.GetNext(pos);
		assert (pPicture != NULL);
		if (!pPicture || pPicture->GetID() != nID)
			continue;
		if (!pPicture->GetPicture().m_pPict)
			return false;
		size.cx = pPicture->GetWidth();
		size.cy = pPicture->GetHeight();
		return true;
	}
	return false;
}


bool CProject::IsInUse() const
{
	POSITION pos = mDclForms.GetHeadPosition();
	while( pos )
	{
		CDclFormObject* pForm = mDclForms.GetNext( pos );
		assert( pForm != NULL );
		if( !pForm )
			continue;
		if( pForm->GetFormInstance() )
			return true; //there is an active dialog using this form
	}
	return false;
}


// This class is used for deserializing OLE controls from versions 8 and 9 project archives
// (the class must have the same name as the original, since the original code used MFC's
// typesafe serialization mechanism, which writes the class name to the archive) [ORW]
class CArxControlObject : public COleControlObject
{ CArxControlObject() : COleControlObject( NULL ) {} DECLARE_SERIAL(CArxControlObject); };
	IMPLEMENT_SERIAL(CArxControlObject, CObject, 1);


void CProject::Serialize(CArchive& ar)
{
  ULONG nThisVersion = 9;

  CObject::Serialize(ar);
  // create a position variable to hold the counter increment
  POSITION pos;	
  short nCount;

  if (ar.IsStoring())
  {
    ar << 9;
    ar << m_bHasPassword;
    ar << m_sPassword;
    ar << m_LispFileName;
    ar << msKeyName; //project key
    ar << m_DistFileName;
    // set counter for Dcl forms
    nCount = (short)mDclForms.GetCount() - CountDeletedForms();
    ar << nCount;		
    m_PurchaseState = nCurrentPurchaseMode;
    ar << m_PurchaseState;
    ar << m_nAutoCADVersion;

    // set start position for navigating dcl forms
    pos = mDclForms.GetHeadPosition();

    // do loop to navigate dcl forms
    while (pos != NULL)
    {
      // get current Dcl form
      CDclFormObject* pDclFormForm = mDclForms.GetNext(pos);

      if (!pDclFormForm->m_bDeleted)
      {
        // put dcl form into archive
        pDclFormForm->Serialize(ar);
        // increment counter
        nCount--;
      }
    }

    m_ActiveXFiles.Serialize(ar);
    mOleControls.Serialize(ar);

    // set counter 
    nCount = (short)mPictures.GetCount();
    ar << nCount;		

    // set start position for navigating images
    pos = mPictures.GetHeadPosition();

    // do loop to navigate images
    while (pos != NULL)
    {
      // get current image
      CPictureObject* pPictureObj = mPictures.GetNext(pos);

      // put image into archive
      pPictureObj->Serialize(ar);
      // increment counter
      nCount--;			
    }

  }
  else
  {		
    ar >> nThisVersion;
		if (nThisVersion > GetCurrentSaveVersion())
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );

    ar >> m_bHasPassword;
    ar >> m_sPassword;		
    ar >> m_LispFileName;

    // lets check here if we need to add the lisp file extension
		CString sExt;
    sExt = theWorkspace.LoadResourceString(IDS_DOTLSP);
    CString sNone;
    sNone = theWorkspace.LoadResourceString(IDS_NONE);
    if (m_LispFileName.Right(4).CompareNoCase(sExt) != 0 /*.lsp*/ && m_LispFileName != sNone /*"<None>"*/)
      m_LispFileName += sExt;
		else if (m_LispFileName == sExt || m_LispFileName == (sNone + sExt))
			m_LispFileName = sNone; //drop the extraneous extension

		CString sKeyName;
    if (nThisVersion >= 11)
      ar >> sKeyName; //this is the project key name
    else
		{
			if (nThisVersion >= 5)
				ar >> sKeyName; //this is the full project file path as saved
			else
				sKeyName = m_LispFileName;
			sKeyName = StripPathFromFileName(sKeyName).SpanExcluding(_T("."));
			sKeyName.Replace( _T(' '), _T('_') );
		}
		if( msKeyName.IsEmpty() ) //if it's already set, don't change it
			SetKeyName( sKeyName );

    if (nThisVersion >= 6)
    {
      // this is used for a distribution file with multiple projects in it.
      ar >> m_DistFileName;

      // lets check here if we need to add the ods file extension
			sExt = theWorkspace.LoadResourceString(IDS_DOTODS);
      if (m_DistFileName.Right(4).CompareNoCase(sExt) != 0 /*.ods*/ && m_DistFileName != sNone /* "<None>"*/)
        m_DistFileName += sExt;
			else if (m_DistFileName == sExt || m_DistFileName == (sNone + sExt))
				m_DistFileName = sNone; //drop the extraneous extension
    }
    else
      m_DistFileName = sNone;

    // this is a fix for an older password protect scheme.
    if (nThisVersion != 2 && nThisVersion != 3)
    {
      m_bHasPassword = FALSE;
      m_sPassword = CString();
    }

    // get counter for dcl forms
    ar >> nCount;
    ar >> m_PurchaseState;

    if (nThisVersion >= 4)
      ar >> m_nAutoCADVersion;
    else
      // otherwise set the default to release nDeAcadVersion
      m_nAutoCADVersion = theWorkspace.GetMinSupportedAcadVersion();

    // do loop to navigate dcl forms
    while (nCount-- > 0)
    {
      // get current Dcl form
      CDclFormObject* pDclForm = new CDclFormObject( this );

      // get dcl form into archive
      pDclForm->Serialize(ar);

			//pDclForm->UpdateGlobalVariable(GetKeyName());

      // add this dcl form to the list object
      mDclForms.AddTail(pDclForm);

    }		
    if (nThisVersion >= 7)
      m_ActiveXFiles.Serialize(ar);

    if (nThisVersion >= 9) {
      mOleControls.Serialize(ar);
    } else {
			POSITION pos = mOleControls.GetHeadPosition();
			while (pos)
				delete mOleControls.GetNext(pos);
			mOleControls.RemoveAll();
			CTypedPtrList< CObList, CArxControlObject* > listControls;
			listControls.Serialize(ar);
			pos = listControls.GetHeadPosition();
			while (pos) 
				mOleControls.AddTail(listControls.GetNext(pos));
		}


    if (nThisVersion >= 9)
    {
      // set counter
      ar >> nCount;

      mPictures.RemoveAll();


      // do loop to navigate images
      while (nCount-- > 0)
      {
        // get current images
        CPictureObject* pPictureObj = new CPictureObject;

        try
        {	
          // get image into archive
          pPictureObj->Serialize(ar);

          // add this image to the list object
          mPictures.AddTail(pPictureObj);							
        }
        catch(...)
        {
          // do nothing
        }
      }
    }
  }

  if (nThisVersion < 9)
  {
    try
    {
      mPictures.Serialize(ar);
    }
    catch(...)
    {
      // do nothing
    }
  }

  // this has been added to cleanup picture objects with a blank picture.
  if (!ar.IsStoring())
  {
    nCount = mPictures.GetCount() - 1;
    for (int i=nCount; i>= 0; i--)
    {
      pos = mPictures.FindIndex(i);
      if (pos != NULL)
      {
        CPictureObject *pPict = mPictures.GetAt(pos);

        if (pPict == NULL)
          mPictures.RemoveAt(pos);
        if (pPict != NULL)
        {
          if (pPict->GetID() <= -1 || pPict->GetID() >= 3000) //was nPicIdToHight, moved to common
          {
            delete pPict;
            mPictures.RemoveAt(pos);
          }
        }
      }
    }
  }
}

void CProject::SaveDistFile()
{
  CString sNone;
  sNone = theWorkspace.LoadResourceString(IDS_NONE);
  // now save the distribution file if required
  if (!m_DistFileName.IsEmpty() && m_DistFileName != sNone && m_DistFileName != ".ods" && m_DistFileName != "<None>.ods")
  {
    CProjectCollection phProjects;

    phProjects.AddProject(this);

    // declare file variables
    CFileException Exception;
    CFile ThisFile;

    CString sTitle;
    sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);

    // open then file
    if ( !ThisFile.Open(m_DistFileName, CFile::modeCreate | CFile::shareExclusive | CFile::modeWrite, &Exception) )
    {
      CString sTheFile;
      sTheFile = theWorkspace.LoadResourceString(IDS_THEFILE);
      CString sText;
      sText = theWorkspace.LoadResourceString(IDS_COULDNOTOPEN);

      MessageBox (::GetActiveWindow(), sTheFile + m_DistFileName + sText, sTitle, NULL);
      return;
    }

    CArchiveEx ar(&ThisFile, CArchive::store | CArchive::bNoFlushOnDelete, NULL, sTitle, TRUE);

    // Serialize the control
    phProjects.Serialize(ar);
    ar.Close();

    ThisFile.Close();
  }
}

/*
void CProject::SaveSS(CStgFile &FileStg, CDocument *pDoc)
{
  //FileStg.Open( _T("projectData.inf"), CFile::modeCreate | CFile::modeWrite); 
  CArchiveEx ar(&FileStg, CArchive::store | CArchive::bNoFlushOnDelete, NULL, GetPassword(), TRUE);
  ar.m_pDocument = pDoc;
  ar.m_bForceFlat = FALSE;

  ar << 9;
  ar << m_bHasPassword;
  ar << m_sPassword;
  ar << m_LispFileName;
  ar << msKeyName;
  // this is used to store the distribution file name.
  ar << m_DistFileName;
  // set counter for Dcl forms
  int nCount = (WORD)mDclForms.GetCount() - CountDeletedForms();
  ar << nCount;		

  POSITION pos = mDclForms.GetHeadPosition();
  while (pos != NULL)
  {
    CDclFormObject *pDialog = mDclForms.GetNext(pos);
    int nId = pDialog->GetType();
    ar << nId;
    ar << pDialog->GetControlProperties()->GetStrProperty(nGlobalVarName);
  }


  m_PurchaseState = nCurrentPurchaseMode;
  ar << m_PurchaseState;
  ar << m_nAutoCADVersion;


  nCount = m_ActiveXFiles.GetSize();
  ar << nCount;
  if (nCount > 0)
    m_ActiveXFiles.Serialize(ar);

  nCount = mOleControls.GetCount();	
  ar << nCount;
  if (nCount > 0)
    mOleControls.Serialize(ar);

  nCount = mPictures.GetCount();
  ar << nCount;		

  // set start position for navigating images
  pos = mPictures.GetHeadPosition();

  // do loop to navigate images
  while (pos != NULL)
  {
    // get current image
    CPictureObject* pPictureObj = mPictures.GetNext(pos);

    // put image into archive
    ar << pPictureObj->GetID();
  }


  ar.Close();			

  FileStg.Close();	// close the stream

  // set start position for navigating dcl forms
  pos = mDclForms.GetHeadPosition();

  int n=0;
  // do loop to navigate dcl forms
  while (pos != NULL)
  {
    // get current Dcl form
    CDclFormObject* pDclFormForm = mDclForms.GetNext(pos);

    if (!pDclFormForm->m_bDeleted)
    {		
      // put dcl form into archive
      pDclFormForm->SaveSS(n, pDclFormForm->GetType(), FileStg, pDoc);
    }	

    n++;
  }

  // set start position for navigating images
  pos = mPictures.GetHeadPosition();

  // do loop to navigate images
  while (pos != NULL)
  {
    // get current image
    CPictureObject* pPictureObj = mPictures.GetNext(pos);

    // put image into archive
    pPictureObj->SaveSS(FileStg, pDoc);

    // increment counter
    nCount--;			
  }

}


void CProject::ReadSS(CStgFile &FileStg, CDocument *pDoc)
{
  int nThisVersion;
  int nCount;
  WORD nDialogCount;
  CStringArray sDialogs;
  CArray<int, int> nPictureIds;
  CArray<int, int> nTypeIds;

  CArchiveEx ar(&FileStg, CArchive::load | CArchive::bNoFlushOnDelete, NULL, GetPassword(), TRUE);

  ar.m_pDocument = pDoc;
  ar.m_bForceFlat = FALSE;

  ar >> nThisVersion;
  ar >> m_bHasPassword;
  ar >> m_sPassword;		
  ar >> m_LispFileName;
  // lets check here if we need to add the ods file extension
  CString strResult = m_LispFileName;
  strResult.MakeLower();

  CString sText;
  sText = theWorkspace.LoadResourceString(IDS_DOTLSP);

  CString sNone;
  sNone = theWorkspace.LoadResourceString(IDS_NONE);
  sNone = sNone.Left(5);

  if (strResult.Right(4) != sText && strResult != sNone)
    m_LispFileName += sText;

	CString sKeyName;
  ar >> sKeyName;
	if( sKeyName.IsEmpty() )
		sKeyName = m_LispFileName;
	sKeyName = StripPathFromFileName(sKeyName).SpanExcluding(_T("."));
	sKeyName.Replace( _T(' '), _T('_') );
	SetKeyName( sKeyName );

  sText = theWorkspace.LoadResourceString(IDS_DOTODS);

  // this is used for a distribution file with multiple projects in it.
  ar >> m_DistFileName;
  // lets check here if we need to add the ods file extension
  strResult = m_DistFileName;
  strResult.MakeLower();
  if (strResult.Right(4) != sText && strResult != sText)
    m_DistFileName += sText;


  // this is a fix for an older password protect scheme.
  if (nThisVersion != 2 && nThisVersion != 3)
  {
    m_bHasPassword = FALSE;
    m_sPassword = CString();
  }

  // get counter for dcl forms
  ar >> nDialogCount;

  int i;
  for (i=0; i<nDialogCount; i++)
  {
    int nId;
    ar >> nId;
    nTypeIds.Add(nId);

    CString sToBeIgnoredInEditor;
    ar >> sToBeIgnoredInEditor;
  }

  ar >> m_PurchaseState;

  ar >> m_nAutoCADVersion;


  ar >> nCount;
  if (nCount > 0)
    m_ActiveXFiles.Serialize(ar);

  ar >> nCount;
  if (nCount > 0)
    mOleControls.Serialize(ar);

  WORD nPictureCount;
  ar >> nPictureCount;

  for (i=0; i<nPictureCount; i++)
  {
    int nId;
    ar >> nId;
    nPictureIds.Add(nId);
  }

  ar.Close();			

  FileStg.Close();	// close the stream

  // do loop to navigate dcl forms
  for (i=0; i<nDialogCount; i++)
  {
    // load the dialog box.
    CDclFormObject* pDclForm = CDclFormObject::ReadSS(i, nTypeIds[i], FileStg, pDoc);

    // add it to the list
    if (pDclForm)
      mDclForms.AddTail(pDclForm);	
  }

  mPictures.RemoveAll();

  // do loop to navigate images
  for (i=0; i<nPictureCount; i++)
  {
    try
    {	
      // get image into archive
      CPictureObject* pPictureObj = CPictureObject::ReadSS(nPictureIds[i], FileStg, pDoc);

      if (pPictureObj)
        // add this image to the list object
        mPictures.AddTail(pPictureObj);							
    }
    catch(...)
    {
      // do nothing
    }
  }
}
*/

IOStatus CProject::ReadFromTextFile(std::ifstream &sFile, const CString &fileName)
{
  //Init the globals before reading anything from the file.
  InitFilerGlobals();

  if (readLine(sFile) != "ObjectDCL Project") return statInvalidFormat;

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
  POSITION pos;

  if (!readBOOL(sFile, m_bHasPassword)) return statInvalidFormat;
  if (!readString(sFile, m_sPassword)) return statInvalidFormat;
  if (!readString(sFile, m_LispFileName)) return statInvalidFormat;
  // lets check here if we need to add the ods file extension
  CString strResult = m_LispFileName;
  strResult.MakeLower();

  CString sNone;
  sNone = theWorkspace.LoadResourceString(IDS_NONE);
  if (strResult != sNone && strResult.Right(4).CompareNoCase(_T(".lsp")) != 0)
    m_LispFileName += _T(".lsp");

  // this is the original project file path; use it to construct the project key
	CString sKeyName;
  if (!readString(sFile, sKeyName)) return statInvalidFormat;
	if (!sKeyName.IsEmpty())
	{
		sKeyName = StripPathFromFileName(sKeyName).SpanExcluding(_T("."));
		sKeyName.Replace( _T(' '), _T('_') );
		SetKeyName( sKeyName );
	}

  // this is used for a distribution file with multiple projects in it.
  if (!readString(sFile, m_DistFileName)) return statInvalidFormat;

	// lets check here if we need to add the ods file extension
  if (m_DistFileName != sNone &&
			m_DistFileName.Right(4).CompareNoCase(theWorkspace.GetDistributionFileExtension()) != 0)
    m_DistFileName += theWorkspace.GetDistributionFileExtension();

  // get counter for dcl forms
  if (!readInt(sFile, nCount)) return statInvalidFormat;
  if (!readInt(sFile, (int&)m_PurchaseState)) return statInvalidFormat;

  if (!readInt(sFile, (int&)m_nAutoCADVersion)) return statInvalidFormat;

  // do loop to navigate dcl forms
  while (nCount-- > 0)
  {
    // get current Dcl form
    CDclFormObject* pDclForm = new CDclFormObject( this );

    // get dcl form into archive
		IOStatus stat = pDclForm->ReadFromTextFile(sFile, fileName);
    if (stat != statOK) return stat;

    // add this dcl form to the list object
    mDclForms.AddTail(pDclForm);

  }	

  int iCount;
  if (!readInt(sFile, iCount)) return statInvalidFormat;
  for (int i = 0; i < iCount; i++) {
    CString str;
    if (!readString(sFile, str)) return statInvalidFormat;
    m_ActiveXFiles.Add(str);
  }

  if (!readInt(sFile, iCount)) return statInvalidFormat;
  for (int i = 0; i < iCount; i++) {
    COleControlObject* pOleControl = new COleControlObject( this );
		IOStatus stat = pOleControl->ReadFromTextFile(sFile, fileName);
    if (stat != statOK) return stat;
    mOleControls.AddTail(pOleControl);
  }

  // set counter
  if (!readInt(sFile, nCount)) return statInvalidFormat;

  mPictures.RemoveAll();
  // do loop to navigate images
  while (nCount-- > 0)
  {
    // get current images
    CPictureObject* pPictureObj = new CPictureObject;

    try
    {	
      // get image into archive
			IOStatus stat = pPictureObj->ReadFromTextFile(sFile, fileName);
      if (stat != statOK) return stat;

      // add this image to the list object
      mPictures.AddTail(pPictureObj);							
    }
    catch(...)
    {
      // do nothing
    }
  }

  // this has been added to cleanup picture objects with a blank picture.
  nCount = mPictures.GetCount() - 1;
  for (int i=nCount; i>=0; i--)
  {
    pos = mPictures.FindIndex(i);
    if (pos != NULL)
    {
      CPictureObject *pPict = mPictures.GetAt(pos);

      if (pPict == NULL)
        mPictures.RemoveAt(pos);
      if (pPict != NULL)
      {
        if (pPict->GetID() <= -1 || pPict->GetID() >= 3000)
        {
          delete pPict;
          mPictures.RemoveAt(pos);
        }
      }
    }
  }
  return statOK;
}


IOStatus CProject::ReadFromTextFile( LPCTSTR lpszFilePath ) 
{
	IOStatus stat = statOK;
	std::ifstream sFile(lpszFilePath, std::ios::in);
	InitFilerGlobals(); //Init the globals before reading anything from the file.
	if (readLine(sFile) != "ObjectDCL Project")
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


IOStatus CProject::ReadFromFile( LPCTSTR pszFilePath )
{
	CFile SrcFile;
	try
	{
		SrcFile.Open( pszFilePath, CFile::modeRead | CFile::shareDenyWrite | CFile::osSequentialScan );
		if (!SrcFile)
			return statFail;

		msProjectFilePath = SrcFile.GetFilePath();

		//Check the first line -- if it is "ObjectDCL Project", then it is a text file.
		char szTextFileSignature[] = "ObjectDCL Project";
		char szLine1[sizeof(szTextFileSignature)];
		UINT_PTR cbRead = SrcFile.Read(szLine1, sizeof(szLine1) - 1);
		szLine1[cbRead] = '\0';

		//FILE* pTextFile = _tfopen(pszFilePath, _T("r"));
		//if (errno != 0)
		//	return statFileNotFound;
		//char szTextFileSignature[] = "ObjectDCL Project";
		//char szLine1[sizeof(szTextFileSignature)];
		//char* pszResult = fgets(szLine1, sizeof(szLine1), pTextFile);
		//fclose(pTextFile);
		//if (!pszResult)
		//	return statReadFailed;

		if( lstrcmpA( szTextFileSignature, szLine1 ) == 0 )
		{ //this file is in plain text format
			SrcFile.Close();
			IOStatus stat = ReadFromTextFile( pszFilePath );
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
		}
		catch(...)
		{
 			SrcFile.Close();
 			return statInvalidFormat;
		}
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
#ifdef SAVE_IN_TEXT_FORMAT
  FILE* pTextFile = fopen(pszPathName, "w");
  if (errno != 0)
    return FALSE;
	CString sFilename = pszFilePath;
  WriteToTextFile(pTextFile, sFileName);
  fclose(pTextFile);
#else //SAVE_IN_TEXT_FORMAT

	TRY
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
	CATCH_ALL(e)
	{
		DELETE_EXCEPTION(e);
		return statFail;
	}
	END_CATCH_ALL;
#endif //SAVE_IN_TEXT_FORMAT
	return statOK;
}
