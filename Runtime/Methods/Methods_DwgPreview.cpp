// Methods_DwgPreview.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_DwgPreview.h"
#include "MethodLexicon.h"
#include "ArgumentsRetrieval.h"
#include "DwgPreviewCtrl.h"
#include "ErrorLexicon.h"
#include "ControlTypes.h"
#include "Workspace.h"


int DwgPreview_LoadDwg()
{
	int nArg=0;
	CWnd *pControl = GetControlPointer(
		CtlDwgPreview,
		sDwgPreview_LoadDwg, 
		&nArg);

	CString sFileName;

	if (!GetStringArgument(nArg, &sFileName, sDwgPreview_LoadDwg) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CString sPath = theWorkspace.FindFile( sFileName ); 
	if( sPath.IsEmpty() )
		return 0;

	((CDwgPreviewCtrl*)pControl)->LoadDwg(sFileName);
	// return nil
	acedRetVoid();
	return 0;
}

int DwgPreview_GetDwgName()
{
	CWnd *pControl = GetControlPointer(
		CtlDwgPreview,
		sDwgPreview_GetDwgName);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	acedRetStr(((CDwgPreviewCtrl*)pControl)->m_filename);
	return 0;
}



int DwgPreview_SetHighLight()
{
	int nArg = 0;
	CWnd *pControl = GetControlPointer(
		CtlDwgPreview,
		sDwgPreview_SetHighLight, 
		&nArg);

	int nColor;

	if (!GetIntArgument(nArg, &nColor, sDwgPreview_SetHighLight) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	
	((CDwgPreviewCtrl*)pControl)->SetHighLight(nColor);
	
	// return nil
	acedRetVoid();
	return 0;
}

int DwgPreview_RemoveHighLight()
{
	CWnd *pControl = GetControlPointer(
		CtlDwgPreview,
		sDwgPreview_RemoveHighLight);

	if (pControl != NULL)
		((CDwgPreviewCtrl*)pControl)->RemoveHighLight();
	
	// return nil
	acedRetVoid();
	return 0;
}



int DwgPreview_Clear()
{
	CWnd *pControl = GetControlPointer(
		CtlDwgPreview,
		sDwgPreview_Clear);

	if (pControl != NULL)
	{
		((CDwgPreviewCtrl*)pControl)->m_filename = "";
		((CDwgPreviewCtrl*)pControl)->m_bSelectedRect = false;
		pControl->Invalidate();
	}
	// return nil
	acedRetVoid();
	return 0;
}