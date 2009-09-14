// PictureFolder.cpp : implementation file
//

#include "stdafx.h"
#include "PictureFolder.h"
#include "Project.h"
#include "PictureObject.h"
#include "PreviewFileDlg.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PropertyIds.h"


static void HIMETRICtoDP(HDC hdc, LPSIZE lpSize)
{
	// map against logical inch for non-constrained mapping modes
	int cxPerInch, cyPerInch;
	cxPerInch = GetDeviceCaps(hdc, LOGPIXELSX);
	cyPerInch = GetDeviceCaps(hdc, LOGPIXELSY);

	ASSERT(cxPerInch != 0 && cyPerInch != 0);
	lpSize->cx = MulDiv(lpSize->cx, cxPerInch, 2540);
	lpSize->cy = MulDiv(lpSize->cy, cyPerInch, 2540);
}


/////////////////////////////////////////////////////////////////////////////
// CPictureFolder dialog

CPictureFolder::CPictureFolder(TStudioProjectPtr pProject, CWnd* pParent /*=NULL*/)
: CDialog(CPictureFolder::IDD, pParent)
, mpProject( pProject )
{
}

CPictureFolder::~CPictureFolder()
{
	while( !mlistPicsToAdd.empty() )
		mlistPicsToAdd.pop_back();
}

void CPictureFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURELIST, m_PictureList);
	DDX_Control(pDX, IDC_PICTURE, m_PictureBox);
}


BEGIN_MESSAGE_MAP(CPictureFolder, CDialog)
	ON_BN_CLICKED(IDADD, OnAdd)
	ON_BN_CLICKED(IDDELETE, OnDelete)
	ON_LBN_SELCHANGE(IDC_PICTURELIST, OnSelchangePicturelist)
	ON_BN_CLICKED(IDUPDATE, OnUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPictureFolder message handlers

void CPictureFolder::OnAdd() 
{
	MultiFileDialog();
}

void CPictureFolder::OnDelete() 
{
	int nCurSel = m_PictureList.GetCurSel();

	// if nothing selected
	if (nCurSel == -1)
		return;

	int nCurSelId = m_PictureList.GetItemData(nCurSel);
	m_PictureList.DeleteString( nCurSel );
	if( m_PictureList.GetCount() > nCurSel )
		m_PictureList.SetCurSel( nCurSel );
	else if (m_PictureList.GetCount() > 0)
		m_PictureList.SetCurSel( nCurSel - 1 );
	else
		m_PictureList.SetCurSel( -1 );
	m_PictureBox.SetPicture( GetSelectedPictureObject() );
	for( std::list< TPicturePtr >::const_iterator iterPict = mlistPicsToAdd.begin();
			 iterPict != mlistPicsToAdd.end();
			 ++iterPict )
	{
		if( (*iterPict)->GetID() == nCurSelId )
		{
			mlistPicsToAdd.remove( *iterPict );
			return;
		}
	}
	msetIdsToDelete.insert( nCurSelId );
}

void CPictureFolder::OnOK() 
{
	m_PictureBox.SetPictureBlank();

	for( std::list< TPicturePtr >::const_iterator iterNewPic = mlistPicsToAdd.begin();
			 iterNewPic != mlistPicsToAdd.end();
			 ++iterNewPic )
	{
		//std::set< int >::const_iterator iterPic = msetIdsToDelete.find( (*iterNewPic)->GetID() );
		//if( iterPic != msetIdsToDelete.end() )
		//	msetIdsToDelete.erase( iterPic );
		msetIdsToDelete.erase( (*iterNewPic)->GetID() );
	}
	for( std::set< int >::const_iterator iterToDelete = msetIdsToDelete.begin();
			 iterToDelete != msetIdsToDelete.end();
			 ++iterToDelete )
	{
		mpProject->DeletePicture( *iterToDelete );
	}
	while( !mlistPicsToAdd.empty() )
	{
		mpProject->AddPicture( mlistPicsToAdd.back() );
		mlistPicsToAdd.pop_back();
	}

	CDialog::OnOK();
}

void CPictureFolder::OnCancel() 
{
	m_PictureBox.SetPictureBlank();
	CDialog::OnCancel();
}

BOOL CPictureFolder::OnInitDialog() 
{
	CDialog::OnInitDialog();
	const TPictureMap& Pictures = mpProject->GetPictureMap();
	for( TPictureMap::const_iterator iter = Pictures.begin(); iter != Pictures.end(); ++iter )
	{
		UINT nId = iter->first;
		CString sID;
		sID.Format( _T("%u"), nId );
		m_PictureList.SetItemData( m_PictureList.AddString( sID ), nId );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CPictureFolder::MultiFileDialog() 
{
	// create the open dialog box
	CPreviewFileDlg BrowseWnd(
		TRUE, 
		NULL,
		NULL, 
		OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		theWorkspace.LoadResourceString( IDS_IMAGEFILEFILTER ),
		this);

	TCHAR szFileBuf[8192] = _T("");
	BrowseWnd.m_ofn.nMaxFile = _elements(szFileBuf);
	BrowseWnd.m_ofn.lpstrFile = szFileBuf;

	if( BrowseWnd.DoModal() == IDOK )   
	{
		int nLargestId;
		// change this line later for smallest available id
		if (m_PictureList.GetCount() > 0)
			nLargestId = m_PictureList.GetItemData( m_PictureList.GetCount() - 1 ) + 1;
		else
			nLargestId = 100;

		for( POSITION pos = BrowseWnd.GetStartPosition(); pos; )
		{
			// load the picture into the picture list collection
			int nID = nLargestId++;
			TPicturePtr pPict = new CPictureObject( nID, BrowseWnd.GetNextPathName( pos ) );
			if( !pPict->IsValid() )
				continue;
			for( std::list< TPicturePtr >::const_iterator iterPict = mlistPicsToAdd.begin();
					 iterPict != mlistPicsToAdd.end();
					 ++iterPict )
			{
				if( (*iterPict)->GetID() == nID )
				{
					mlistPicsToAdd.remove( *iterPict );
					break;
				}
			}
			mlistPicsToAdd.push_back( pPict );
			CString sVal;
			sVal.Format( _T("%d"), nID );
			m_PictureList.SetItemData( m_PictureList.AddString( sVal ), nID );
		}
	}
}

void CPictureFolder::UpdateSingleFileDialog() 
{
	int nCurSelId = m_PictureList.GetItemData( m_PictureList.GetCurSel() );

	// create the open dialog box
	CPreviewFileDlg BrowseWnd(
		TRUE, 
		NULL,
		NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		theWorkspace.LoadResourceString( IDS_IMAGEFILEFILTER ),
		this );

	if( BrowseWnd.DoModal() == IDOK )
	{
		TPicturePtr pPict = new CPictureObject( nCurSelId, BrowseWnd.GetPathName() );
		if( !pPict->IsValid() )
			return;
		for( std::list< TPicturePtr >::const_iterator iterPict = mlistPicsToAdd.begin();
				 iterPict != mlistPicsToAdd.end();
				 ++iterPict )
		{
			if( (*iterPict)->GetID() == nCurSelId )
			{
				mlistPicsToAdd.remove( *iterPict );
				break;
			}
		}
		mlistPicsToAdd.push_back( pPict );
		m_PictureBox.SetPicture( pPict );
	}
}

TPicturePtr CPictureFolder::GetSelectedPictureObject()
{
	if( m_PictureList.GetCurSel() == -1 )
		return NULL;
	int nID = m_PictureList.GetItemData( m_PictureList.GetCurSel() );
	for( std::list< TPicturePtr >::const_iterator iterPict = mlistPicsToAdd.begin();
			 iterPict != mlistPicsToAdd.end();
			 ++iterPict )
	{
		if( (*iterPict)->GetID() == nID )
			return (*iterPict);
	}
	return mpProject->FindPicture( nID );
}

void CPictureFolder::OnSelchangePicturelist() 
{
	m_PictureBox.SetPicture( GetSelectedPictureObject() );
}

void CPictureFolder::OnUpdate() 
{
	UpdateSingleFileDialog();
	OnSelchangePicturelist();
}
