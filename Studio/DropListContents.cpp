// DropListContents.cpp : implementation file
//

#include "stdafx.h"
#include "DropListContents.h"


/////////////////////////////////////////////////////////////////////////////
// CDropListContents dialog

CDropListContents::CDropListContents(CWnd* pParent /*=NULL*/)
	: CDialog(CDropListContents::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDropListContents)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDropListContents::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDropListContents)
	DDX_Control(pDX, IDC_EDIT, m_ListEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDropListContents, CDialog)
	//{{AFX_MSG_MAP(CDropListContents)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDropListContents message handlers

void CDropListContents::OnOK() 
{
	for (int i=0; i<m_ListEdit.GetLineCount(); i++)
	{
		CString sText;			
		int cb;         
		TCHAR *pch; 
		pch = sText.GetBuffer(_MAX_PATH); 
		
		cb = m_ListEdit.GetLine(i, pch, _MAX_PATH); 
		pch[cb] = _T('\0'); 
		sText.ReleaseBuffer(); 
		
		if (sText.GetLength() > 0)
			sStrings.Add(sText);
	}
	CDialog::OnOK();
}

const char sNewLine[] = "\r\n";

BOOL CDropListContents::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString sText;
	for (int i=0; i<sStrings.GetSize();i++)
	{
		 sText = sText + sStrings[i];

		 if (i<sStrings.GetSize() -1)
			 sText += sNewLine;
	}
	sStrings.RemoveAll();

	m_ListEdit.SetWindowText(sText);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}
