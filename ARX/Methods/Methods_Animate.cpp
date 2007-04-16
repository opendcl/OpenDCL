// Methods_Animate.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Animate.h"
#include "DclControlObject.h"
#include "ArgumentsRetrieval.h"
#include "ArxProject.h"
#include "MethodLexicon.h"
#include "ControlTypes.h"


const TCHAR s1[] = _T("\\");
const TCHAR s2[] = _T("/");
const TCHAR s3[] = _T(".avi");


int AnimateCtrl_Load()
{
	CString sFileName;
	
	CDclControlObject *pArx = GetLispInput(sAnimate_Load, sFileName);
	
	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	CAnimateCtrl *pCtrl = (CAnimateCtrl*)pArx->GetWindow();
	
	if (_tcsicmp(sFileName.Right(4), s3) != 0)
		sFileName += s3;

	CString sPath = theWorkspace.FindFile( sFileName ); 
	if( sPath.IsEmpty() )
	{
		const CProject *pProject = pArx->GetOwnerProject();
		if (pProject)
		{
			CString sAvi = sFileName;
			int n = pProject->GetBaseFileName().Find(s1);
			if (n == -1)
				n = pProject->GetBaseFileName().Find(s2);

			int nNext = n;
			while (nNext > -1)
			{
				nNext = pProject->GetBaseFileName().Find(s1, n+1);
				if (nNext == -1)
					nNext = pProject->GetBaseFileName().Find(s2, n+1);
				if (nNext > -1)
					n = nNext;	
			}

			sAvi = pProject->GetBaseFileName().Right(n+1) + sAvi;
			if (_tcsicmp(sAvi.Left(4), s3) != 0)
				sAvi += s3;

			sFileName = sAvi;
		}
		else
			sFileName = sPath;
	}	
	else
		sFileName = sPath;

	
	
	pCtrl->Open(sFileName);
	
	acedRetVoid();
	return 0;
}


int AnimateCtrl_Seek()
{
	int nFrame;
	CDclControlObject *pArx = GetLispInput(sAnimate_Load, nFrame);
	
	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	CAnimateCtrl *pCtrl = (CAnimateCtrl*)pArx->GetWindow();
	pCtrl->Seek(nFrame);
	
	acedRetVoid();
	return 0;
} 
int AnimateCtrl_Close()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTabStrip, sAnimate_Close, &nArg);

	if (pControl == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}
	

	((CAnimateCtrl*)pControl)->Close();
	
	acedRetVoid();
	return 0;
}


int AnimateCtrl_Stop()
{	
	int nArg;
	CWnd *pControl = GetControlPointer(CtlTabStrip, sAnimate_Stop, &nArg);

	if (pControl == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}
	

	((CAnimateCtrl*)pControl)->Stop();
	
	acedRetVoid();
	return 0;
}