// ProjectPasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProjectPasswordDlg.h"


// CProjectPasswordDlg dialog

IMPLEMENT_DYNAMIC(CProjectPasswordDlg, CDialog)

CProjectPasswordDlg::CProjectPasswordDlg(LPCTSTR pszPassword, CWnd* pParent /*=NULL*/)
	: CDialog(CProjectPasswordDlg::IDD, pParent)
	, msPassword( pszPassword )
{

}

CProjectPasswordDlg::~CProjectPasswordDlg()
{
}

void CProjectPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PASSWORD, msPassword);
}


BEGIN_MESSAGE_MAP(CProjectPasswordDlg, CDialog)
END_MESSAGE_MAP()


// CProjectPasswordDlg message handlers
