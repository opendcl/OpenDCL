// Methods_Animate.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Animate.h"
#include "DclControlObject.h"
#include "ArgumentsRetrieval.h"
#include "ArxWorkspace.h"
#include "MethodLexicon.h"
#include "ControlTypes.h"


int AnimateCtrl_Load()
{
	CString sFileName;
	
	TDclControlPtr pArx = GetLispInput(_T("Animate_Load"), sFileName);
	
	if (pArx == NULL)	
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	CAnimateCtrl *pCtrl = (CAnimateCtrl*)pArx->GetWindow();
	
	if (_tcsicmp(sFileName.Right(4), _T(".avi")) != 0)
		sFileName += _T(".avi");

	CString sPath = theWorkspace.FindFile( sFileName ); 
	if( sPath.IsEmpty() )
	{
		const TProjectPtr pProject = pArx->GetOwnerProject();
		if (pProject)
		{
			CString sAvi = sFileName;
			int n = pProject->GetBaseFileName().Find(_T("\\"));
			if (n == -1)
				n = pProject->GetBaseFileName().Find(_T("/"));

			int nNext = n;
			while (nNext > -1)
			{
				nNext = pProject->GetBaseFileName().Find(_T("\\"), n+1);
				if (nNext == -1)
					nNext = pProject->GetBaseFileName().Find(_T("/"), n+1);
				if (nNext > -1)
					n = nNext;	
			}

			sAvi = pProject->GetBaseFileName().Right(n+1) + sAvi;
			if (_tcsicmp(sAvi.Left(4), _T(".avi")) != 0)
				sAvi += _T(".avi");

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
	TDclControlPtr pArx = GetLispInput(_T("Animate_Seek"), nFrame);
	
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
	CWnd *pControl = GetControlPointer(CtlTabStrip, _T("Animate_Close"), &nArg);

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
	CWnd *pControl = GetControlPointer(CtlTabStrip, _T("Animate_Stop"), &nArg);

	if (pControl == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}
	

	((CAnimateCtrl*)pControl)->Stop();
	
	acedRetVoid();
	return 0;
}