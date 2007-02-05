// Methods_Hatch.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Hatch.h"
#include "DclControlObject.h"
#include "MethodLexicon.h"
#include "ArgumentsRetrieval.h"
#include "GsPreviewCtrl.h"
#include "ControlTypes.h"


int Hatch_Clear()
{
	CWnd *pControl = GetControlPointer(CtlHatch, sHatch_Clear);
	
	if (pControl == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CGsPreviewCtrl *pCtrl = (CGsPreviewCtrl*)pControl;

	if (pCtrl != NULL)
	{
		pCtrl->clearAll();	
		pCtrl->m_Line[0] = NULL;
		pCtrl->m_Line[1] = NULL;
		pCtrl->m_Line[2] = NULL;
		pCtrl->m_Line[3] = NULL;			
		pCtrl->m_BlockName = CString();
		pCtrl->m_FileName = CString();
		pCtrl->m_bSelectedRect = false;
		if (pCtrl->m_pLoadedDwg != NULL)
		{
			delete pCtrl->m_pLoadedDwg;
			pCtrl->m_pLoadedDwg = NULL;
		}
		pCtrl->RedrawWindow();
	}

	acedRetVoid();
	return 0;
}



int Hatch_SetPattern()
{
	CString sPattern;
	
	CDclControlObject *pArx = GetLispInput(sHatch_SetPattern, sPattern);

	if (pArx == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}

	CGsPreviewCtrl *pBlock = (CGsPreviewCtrl*)pArx->GetWindow();
	
	if (pBlock != NULL)
	{
		pBlock->clearAll();	
		pBlock->m_Line[0] = NULL;
		pBlock->m_Line[1] = NULL;
		pBlock->m_Line[2] = NULL;
		pBlock->m_Line[3] = NULL;			
		pBlock->m_BlockName = CString();
		pBlock->m_FileName = CString();
		pBlock->m_bSelectedRect = false;
		if (pBlock->m_pLoadedDwg != NULL)
		{
			delete pBlock->m_pLoadedDwg;
			pBlock->m_pLoadedDwg = NULL;
		}
		pBlock->RedrawWindow();
	}

	pBlock->DisplayHatchPattern(sPattern);

	acedRetVoid();	
	return 0;

}
