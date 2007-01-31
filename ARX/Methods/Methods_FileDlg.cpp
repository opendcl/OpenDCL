// Methods_FileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_FileDlg.h"
#include "DclControlObject.h"
#include "MethodLexicon.h"
#include "ArgumentsRetrieval.h"
#include "UserMessageId.h"
#include "ParentFileDialog.h"


int FileDlgGetFileName()
{
	int nArg=0;
	CDclControlObject *pControl = GetControlArxObject(sFileDlgGetFileName, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CParentFileDialog *pFileDlg = (CParentFileDialog*)pControl->m_pWnd;
	acedRetStr(pFileDlg->GetFileName());
	return 0;
}
int FileDlgGetFileTitle()
{
	int nArg=0;
	CDclControlObject *pControl = GetControlArxObject(sFileDlgGetFileTitle, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CParentFileDialog *pFileDlg = (CParentFileDialog*)pControl->m_pWnd;
	acedRetStr(pFileDlg->GetFileTitle());
	
	return 0;
}
int FileDlgGetFileExt()
{
	int nArg=0;
	CDclControlObject *pControl = GetControlArxObject(sFileDlgGetFileTitle, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CParentFileDialog *pFileDlg = (CParentFileDialog*)pControl->m_pWnd;
	acedRetStr(pFileDlg->GetFileExt());
	return 0;
}

int FileDlgGetPathName()
{
	int nArg=0;
	CDclControlObject *pControl = GetControlArxObject(sFileDlgGetPathName, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CParentFileDialog *pFileDlg = (CParentFileDialog*)pControl->m_pWnd;
	acedRetStr(pFileDlg->m_sFolderPath);	
	return 0;
}
int FileDlgGetSelectionCount()
{
	int nArg=0;
	CDclControlObject *pControl = GetControlArxObject(sFileDlgGetSelectionCount, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CParentFileDialog *pFileDlg = (CParentFileDialog*)pControl->m_pWnd;
	pFileDlg->ReadListViewNames();
	acedRetInt(pFileDlg->m_pStrList->GetSize());
	return 0;
}
int FileDlgGetFolderPath()
{
	int nArg=0;
	CDclControlObject *pControl = GetControlArxObject(sFileDlgGetFolderPath, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CParentFileDialog *pFileDlg = (CParentFileDialog*)pControl->m_pWnd;
	acedRetStr(pFileDlg->GetFolderPath());
	return 0;
}

const TCHAR sDirSep = _T('\\');
	
int FileDlgGetFolderName()
{
	int nArg=0;
	CDclControlObject *pControl = GetControlArxObject(sFileDlgGetFolderName, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CParentFileDialog *pFileDlg = (CParentFileDialog*)pControl->m_pWnd;
	CString test =pFileDlg->m_sFolderPath;
	int n = test.ReverseFind(sDirSep);
	test = test.Mid(n+1);
	acedRetStr(test);
	return 0;
}

int FileDlgGetFileNameList()
{
	int nArg=0;
	CDclControlObject *pControl = GetControlArxObject(sFileDlgGetFileNameList, &nArg);
	
	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}	

	CParentFileDialog *pFileDlg = (CParentFileDialog*)pControl->m_pWnd;
	pFileDlg->ReadListViewNames();

	int nCount = pFileDlg->m_pStrList->GetSize();

	if (nCount == 0)
	{
		acedRetNil();
		return 0;
	}
	if (nCount == 1)
	{
		acedRetStr(pFileDlg->GetFileName());
		return 0;
	}
	
	// Convert the array to a list that can be returned
	struct resbuf* rbpRetList = acutNewRb(RTSTR);
	struct resbuf* rbpTail = rbpRetList;
 
	for (int i=0; i<nCount; i++)
	{
		// get the text name of the selected line number
		CString sFileName = pFileDlg->m_pStrList->GetAt(i);
		
		acutNewString(sFileName, rbpTail->resval.rstring);
		if ((i+1) < nCount)
		{
			rbpTail->rbnext = acutNewRb(RTSTR);
			rbpTail = rbpTail->rbnext;
		}
 
	}

	acedRetList(rbpRetList);
	acutRelRb(rbpRetList);
	return 0;
}

int FileDlgSetOkButtonText()
{
	int nArg=0;
	CDclControlObject *pControl = GetControlArxObject(sFileDlgSetOkButtonText, &nArg);
	
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

	CParentFileDialog *pFileDlg = (CParentFileDialog*)pControl->m_pWnd;
	
	pFileDlg->GetParent()->GetDlgItem(IDOK)->SetWindowText(sCaption);
	acedRetNil();
	return 0;
}
