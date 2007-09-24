// Methods_FileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_FileDlg.h"
#include "DclControlObject.h"
#include "MethodLexicon.h"
#include "ArgumentsRetrieval.h"
#include "UserMessageId.h"
#include "CustomFileDialog.h"


int FileDlgGetFileName()
{
	int nArg=0;
	TDclControlPtr pControl = GetControlArxObject(sFileDlgGetFileName, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CCustomFileDialog *pFileDlg = (CCustomFileDialog*)pControl->GetWindow();
	acedRetStr(pFileDlg->GetFileName());
	return 0;
}
int FileDlgGetFileTitle()
{
	int nArg=0;
	TDclControlPtr pControl = GetControlArxObject(sFileDlgGetFileTitle, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CCustomFileDialog *pFileDlg = (CCustomFileDialog*)pControl->GetWindow();
	acedRetStr(pFileDlg->GetFileTitle());
	
	return 0;
}
int FileDlgGetFileExt()
{
	int nArg=0;
	TDclControlPtr pControl = GetControlArxObject(sFileDlgGetFileTitle, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CCustomFileDialog *pFileDlg = (CCustomFileDialog*)pControl->GetWindow();
	acedRetStr(pFileDlg->GetFileExt());
	return 0;
}

int FileDlgGetPathName()
{
	int nArg=0;
	TDclControlPtr pControl = GetControlArxObject(sFileDlgGetPathName, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CCustomFileDialog *pFileDlg = (CCustomFileDialog*)pControl->GetWindow();
	acedRetStr(pFileDlg->GetPathName());	
	return 0;
}
int FileDlgGetFolderPath()
{
	int nArg=0;
	TDclControlPtr pControl = GetControlArxObject(sFileDlgGetFolderPath, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CCustomFileDialog *pFileDlg = (CCustomFileDialog*)pControl->GetWindow();
	acedRetStr(pFileDlg->GetFolderPath());
	return 0;
}
	
int FileDlgGetFolderName()
{
	int nArg=0;
	TDclControlPtr pControl = GetControlArxObject(sFileDlgGetFolderName, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CCustomFileDialog *pFileDlg = (CCustomFileDialog*)pControl->GetWindow();
	acedRetStr(pFileDlg->GetFolderPath().MakeReverse().SpanExcluding(_T("\\/:")).MakeReverse());
	return 0;
}

int FileDlgGetSelectionCount()
{
	int nArg=0;
	TDclControlPtr pControl = GetControlArxObject(sFileDlgGetSelectionCount, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CCustomFileDialog *pFileDlg = (CCustomFileDialog*)pControl->GetWindow();
	CWnd* pMainWnd = pFileDlg->GetParent()->GetDlgItem(lst2);
	if (pMainWnd == NULL)
		return 0;
	CListCtrl* pFileListCtrl = (CListCtrl*)(pMainWnd->GetDlgItem(1));
	acedRetInt(pFileListCtrl->GetSelectedCount());
	return 0;
}
int FileDlgGetFileNameList()
{
	int nArg=0;
	TDclControlPtr pControl = GetControlArxObject(sFileDlgGetFileNameList, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CCustomFileDialog *pFileDlg = (CCustomFileDialog*)pControl->GetWindow();
	CWnd* pMainWnd = pFileDlg->GetParent()->GetDlgItem(lst2);
	if (pMainWnd == NULL)
		return 0;
	CListCtrl* pFileListCtrl = (CListCtrl*)(pMainWnd->GetDlgItem(1));

	struct resbuf* prbFileList = NULL;
	struct resbuf* prbTail = NULL;
	POSITION pos = pFileListCtrl->GetFirstSelectedItemPosition();
	while( pos )
	{
		resbuf* prbFile = acutNewRb(RTSTR);
		int idxSelected = pFileListCtrl->GetNextSelectedItem(pos);
		acutNewString(pFileListCtrl->GetItemText(idxSelected, 0), prbFile->resval.rstring);
		if (!prbTail)
		{
			prbFileList = prbFile;
			prbTail = prbFileList;
		}
		else
		{
			prbTail->rbnext = prbFile;
			prbTail = prbTail->rbnext;
		}
	}
	acedRetList(prbFileList);
	acutRelRb(prbFileList);

	return 0;
}

int FileDlgSetOkButtonText()
{
	int nArg=0;
	TDclControlPtr pControl = GetControlArxObject(sFileDlgSetOkButtonText, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CString sCaption;
	if (!GetStringArgument(nArg, &sCaption, sFileDlgSetOkButtonText))
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CCustomFileDialog *pFileDlg = (CCustomFileDialog*)pControl->GetWindow();
	
	pFileDlg->GetParent()->GetDlgItem(IDOK)->SetWindowText(sCaption);
	acedRetNil();
	return 0;
}
