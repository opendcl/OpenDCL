// Methods_BlockView.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_BlockView.h"
#include "ArgumentsRetrieval.h"
#include "ControlTypes.h"
#include "MethodLexicon.h"
#include "GsPreviewCtrl.h"
#include "DclControlObject.h"


int BlockView_Clear()
{
	CWnd *pControl = GetControlPointer(CtlBlockView, sBlockView_Clear);

	if (pControl != NULL)
	{
		CGsPreviewCtrl *pCtrl = (CGsPreviewCtrl*)pControl;
		pCtrl->clearAll();				
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


int BlockView_SetHighLight()
{
	int nColor;

	CWnd *pControl = GetArgsControlInt(CtlBlockView,sBlockView_SetHighLight, nColor);
	
	if (pControl == NULL)
	{
		return 0;
	}

	
	((CGsPreviewCtrl*)pControl)->SetHighLight(nColor);
	
	
	acedRetVoid();
	return 0;
}

int BlockView_RemoveHighLight()
{
	CWnd *pControl = GetControlPointer(
		CtlBlockView,
		sBlockView_RemoveHighLight);

	if (pControl != NULL)
		((CGsPreviewCtrl*)pControl)->RemoveHighLight();
	
	
	acedRetVoid();
	return 0;
}

int BlockView_Zoom()
{
	double dZfactor;
	CWnd *pControl = GetArgsControlDouble(CtlBlockView,sBlockView_RemoveHighLight, dZfactor);
	
	if (pControl == NULL)
	{
		return 0;
	}

	((CGsPreviewCtrl*)pControl)->Zoom(dZfactor);
	
	
	acedRetVoid();
	return 0;
}

int BlockView_LoadDwgToScale()
{
	BlockView_LoadDwgAndDisplay(1, sBlockView_LoadDwgToScale);
	return 0;
}
int BlockView_LoadDwg()
{
	BlockView_LoadDwgAndDisplay(0, sBlockView_LoadDwg);
	return 0;
}
int BlockView_PreLoadDwg()
{	
	CString sFileName;
	int nIndex = 0;
	CWnd *pControl = GetControlPointer(CtlBlockView, sBlockView_PreLoadDwg, &nIndex);
	
	if (pControl == NULL || !GetStringArgument(nIndex, &sFileName, sBlockView_PreLoadDwg))
	{		
		acedRetNil();
		return 0;
	}
	
	CGsPreviewCtrl *pCtrl = (CGsPreviewCtrl*)pControl;
	pCtrl->clearAll();				
	pCtrl->m_BlockName = CString();
	pCtrl->m_FileName = CString();
	pCtrl->m_bSelectedRect = false;
	if (pCtrl->m_pLoadedDwg != NULL)
	{
		delete pCtrl->m_pLoadedDwg;
		pCtrl->m_pLoadedDwg = NULL;
	}
	pCtrl->RedrawWindow();
	pCtrl->PreLoadDwg(sFileName);

	acedRetNil();		
	return 0;
}

int BlockView_GetBlockSize()	
{
	CString sBlockName;
	int nIndex = 0;
	CWnd *pControl = GetControlPointer(CtlBlockView, sBlockView_GetBlockSize, &nIndex);
	
	if (pControl == NULL || !GetStringArgument(nIndex, &sBlockName, sBlockView_GetBlockSize))
	{		
		acedRetInt(-1);
		return 0;
	}
	
	((CGsPreviewCtrl*)pControl)->GetBlockSize(sBlockName);
	// no acedRetList here, it's in the GetBlockSize() method.
	return 0;
}

int BlockView_GetDwgSize()
{
	CString sBlockName;
	int nIndex = 0;
	CWnd *pControl = GetControlPointer(CtlBlockView, sBlockView_GetDwgSize, &nIndex);
	
	if (pControl == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}
	
	((CGsPreviewCtrl*)pControl)->GetDwgSize();
	// no acedRetList here, it's in the GetDwgSize() method.
	return 0;
}


int BlockView_GetBlockList()
{	
	int nIndex = 0;
	CWnd *pControl = GetControlPointer(CtlBlockView, sBlockView_GetBlockList, &nIndex);
	
	if (pControl == NULL)
	{		
		acedRetInt(-1);
		return 0;
	}
	
	((CGsPreviewCtrl*)pControl)->GetBlockList();
	// no acedRetList here, it's in the GetBlockList() method.
	return 0;
}

void BlockView_LoadDwgAndDisplay(int nScaleType, CString sDefunName)
{
	CString sFileName;
	double dZoom = 1.0;	
	bool   dZoomExtents = false;
	double dCameraX;
	double dCameraY;
	double dCameraZ;
	double dUpX = 0.0;
	double dUpY = 1.0;
	double dUpZ = 0.0;
	bool bCamera = false;
	int nIndex = 0;
	
	CWnd *pControl = GetControlPointer(CtlBlockView, sDefunName, &nIndex);
	

	if (!GetStringArgument(nIndex, &sFileName, sDefunName) || pControl == NULL)
	{		
		acedRetInt(-1);
		return;
	}
	nIndex++;

	
	if (!FindOptionalDoubleArgument(nIndex, &dCameraX, sDefunName))
	{
		((CGsPreviewCtrl*)pControl)->LoadPreviewDwg(sFileName);
		
		acedRetVoid();
		return;
	}
	nIndex++;

	if(FindOptionalDoubleArgument(nIndex, &dCameraY, sDefunName))
	{
		nIndex++;
		if (FindOptionalDoubleArgument(nIndex, &dCameraZ, sDefunName))
		{
			nIndex++;
			if (!FindOptionalDoubleArgument(nIndex, &dZoom, sDefunName))
				dZoomExtents = true;
			else 
				nIndex++;
			bCamera = true;
		}
		else
		{
			dZoom = dCameraY;
			bCamera = false;
		}
	}
	else
	{
		// if zoom to scale and only the dCameraX has been set.
		if (nScaleType == 1)
		{
			// set the zoom factor to dCameraX and dCameraX = 0 (top)
			dZoom = dCameraX;
			dCameraX = 0;
			bCamera = false;
		}
	}

	try
	{
		// check if the x, y and z coords were specified
		if (bCamera)
		{		
			((CGsPreviewCtrl*)pControl)->LoadPreviewDwg(
				sFileName,
				dZoom,
				dZoomExtents,
				nScaleType,
				dUpX, dUpY, dUpZ,
				dCameraX, dCameraY, dCameraZ);
		}
		else
		{
			int nPreset = int(dCameraX);
			switch(nPreset)
			{
			case 0: // top
				((CGsPreviewCtrl*)pControl)->LoadPreviewDwg(sFileName, dZoom, dZoomExtents, nScaleType, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
				break;
				
			case 1: // bottom
				((CGsPreviewCtrl*)pControl)->LoadPreviewDwg(sFileName, dZoom, dZoomExtents, nScaleType, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0);
				break;
			
			case 2: // right
				((CGsPreviewCtrl*)pControl)->LoadPreviewDwg(sFileName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0);
				break;
				
			case 3: // left
				((CGsPreviewCtrl*)pControl)->LoadPreviewDwg(sFileName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, -1.0, 0.0, 0.0);
				break;
			
			case 4: // front
				((CGsPreviewCtrl*)pControl)->LoadPreviewDwg(sFileName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);
				break;

			case 5: // back
				((CGsPreviewCtrl*)pControl)->LoadPreviewDwg(sFileName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
				break;
			
			case 6: // SW
				((CGsPreviewCtrl*)pControl)->LoadPreviewDwg(sFileName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, -1.0, -1.0, 1.0);
				break;
			
			case 7: // SE
				((CGsPreviewCtrl*)pControl)->LoadPreviewDwg(sFileName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, 1.0, -1.0, 1.0);
				break;
			
			case 8: // NW
				((CGsPreviewCtrl*)pControl)->LoadPreviewDwg(sFileName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, -1.0, 1.0, 1.0);
				break;
			
			case 9: // NE
				((CGsPreviewCtrl*)pControl)->LoadPreviewDwg(sFileName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
				break;
			}
		}	
	}
	catch(...)
	{	
	}

	acedRetVoid();	
}

int BlockView_DisplayBlock(int nScaleType, CString sDefunName)
{
	CString sBlockName;
	double dZoom = 1.0;	
	bool   dZoomExtents = false;
	double dCameraX;
	double dCameraY;
	double dCameraZ;
	double dUpX = 0.0;
	double dUpY = 1.0;
	double dUpZ = 0.0;
	bool bCamera = false;
	int nIndex = 0;
	
	CWnd *pControl = GetControlPointer(CtlBlockView, sDefunName, &nIndex);
	
	if (!GetStringArgument(nIndex, &sBlockName, sDefunName) || pControl == NULL)
	{
		acedRetInt(-1);
		return 0;
	}
	nIndex++;
	
	if (!FindOptionalDoubleArgument(nIndex, &dCameraX, sDefunName))
	{
		((CGsPreviewCtrl*)pControl)->DisplayBlock(sBlockName);
		
		acedRetVoid();
		return 0;
	}
	nIndex++;

	if(FindOptionalDoubleArgument(nIndex, &dCameraY, sDefunName))
	{
		nIndex++;
		if (FindOptionalDoubleArgument(nIndex, &dCameraZ, sDefunName))
		{
			nIndex++;
			if (!FindOptionalDoubleArgument(nIndex, &dZoom, sDefunName))
				dZoomExtents = true;
			else 
				nIndex++;
			bCamera = true;
		}
		else
		{
			dZoom = dCameraY;
			bCamera = false;
		}
	}
	else
	{
		// if zoom to scale and only the dCameraX has been set.
		if (nScaleType == 1)
		{
			// set the zoom factor to dCameraX and dCameraX = 0 (top)
			dZoom = dCameraX;
			dCameraX = 0;
			bCamera = false;
		}
	}
	

	try
	{
		
		// check if the x, y and z coords were specified
		if (bCamera)
		{
			((CGsPreviewCtrl*)pControl)->DisplayBlock(
				sBlockName,
				dZoom,
				dZoomExtents,
				nScaleType,
				dUpX, dUpY, dUpZ,
				dCameraX, dCameraY, dCameraZ);
		}
		else
		{
			int nPreset = int(dCameraX);
			switch(nPreset)
			{
			case 0: // top
				((CGsPreviewCtrl*)pControl)->DisplayBlock(sBlockName, dZoom, dZoomExtents, nScaleType, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
				break;
				
			case 1: // bottom
				((CGsPreviewCtrl*)pControl)->DisplayBlock(sBlockName, dZoom, dZoomExtents, nScaleType, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0);
				break;
			
			case 2: // right
				((CGsPreviewCtrl*)pControl)->DisplayBlock(sBlockName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0);
				break;
				
			case 3: // left
				((CGsPreviewCtrl*)pControl)->DisplayBlock(sBlockName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, -1.0, 0.0, 0.0);
				break;
			
			case 4: // front
				((CGsPreviewCtrl*)pControl)->DisplayBlock(sBlockName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);
				break;

			case 5: // back
				((CGsPreviewCtrl*)pControl)->DisplayBlock(sBlockName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
				break;
			
			case 6: // SW
				((CGsPreviewCtrl*)pControl)->DisplayBlock(sBlockName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, -1.0, -1.0, 1.0);
				break;
			
			case 7: // SE
				((CGsPreviewCtrl*)pControl)->DisplayBlock(sBlockName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, 1.0, -1.0, 1.0);
				break;
			
			case 8: // NW
				((CGsPreviewCtrl*)pControl)->DisplayBlock(sBlockName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, -1.0, 1.0, 1.0);
				break;
			
			case 9: // NE
				((CGsPreviewCtrl*)pControl)->DisplayBlock(sBlockName, dZoom, dZoomExtents, nScaleType, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
				break;
			}
		}
	}
	catch(...)
	{
	}

	acedRetVoid();
	return 0;
}


int BlockView_ViewPaperSpace()
{
	int nScaleType = 0;
	CString sDefunName = sBlockView_ViewPaperSpace;

	double dZoom = 1.0;	
	bool   dZoomExtents = false;
	int nIndex = 0;	

	CWnd *pControl = GetControlPointer(CtlBlockView, sDefunName, &nIndex);
	
//n	((CGsPreviewCtrl*)pControl)->SetAcadColor(-23);
	
	((CGsPreviewCtrl*)pControl)->DisplayBlock(ACDB_PAPER_SPACE, dZoom, dZoomExtents, nScaleType, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	
	acedRetVoid();
	return 0;
}


int BlockView_ViewBlock()
{
	return BlockView_DisplayBlock(0, sBlockView_ViewBlock);
}

int BlockView_ViewBlockToScale()
{
	return BlockView_DisplayBlock(1, sBlockView_ViewBlockToScale);	
}


int BlockView_RefreshBlock()
{
	int nIndex = 0;
	CWnd *pControl = GetControlPointer(CtlBlockView, sBlockView_RefreshBlock, &nIndex);
	
	((CGsPreviewCtrl*)pControl)->UpdateBlock();
	
	acedRetVoid();
	return 0;
}

int BlockView_GetViewInfo()
{
	int nIndex = 0;
	CGsPreviewCtrl *pControl = (CGsPreviewCtrl*)GetControlPointer(CtlBlockView, sBlockView_GetViewInfo, &nIndex);
	
	if (pControl != NULL && pControl->view() != NULL)
	{
		AcGePoint3d camera = pControl->view()->position();
		AcGePoint3d target = pControl->view()->target();
		AcGeVector3d up = pControl->view()->upVector();

		int stat;
		struct resbuf *list;    
		
		int nPerspective;
		if (pControl->view()->isPerspective())
			nPerspective = RTT;
		else
			nPerspective = RTNIL;

		list = acutBuildList(
			RTLB,
				RTLB,
					RTREAL, target[X], 
					RTREAL, target[Y],
					RTREAL, target[Z],
				RTLE,
				RTLB,
					RTREAL, camera[X],
					RTREAL, camera[Y],
					RTREAL, camera[Z],
				RTLE,
				RTLB,
					RTREAL, up[X],
					RTREAL, up[Y],
					RTREAL, up[Z],
				RTLE,
				RTREAL, pControl->view()->fieldWidth(),
				RTREAL, pControl->view()->fieldHeight(),
				nPerspective,
			RTLE,
			RTNONE);

		if (list != NULL) { 	    
			stat = acedRetList(list);		
			acutRelRb(list); 
		} 
		return 0;
	}

	acedRetNil();
	return 0;
}


int BlockView_SetView()
{
	CDclControlObject *pArxObject = NULL;
	struct resbuf *ListData = getLispTargetInput(sBlockView_SetView, pArxObject);

	
	if (pArxObject == NULL)
	{
		acedRetVoid();
		return 0;
	}

	CGsPreviewCtrl *pControl = (CGsPreviewCtrl*)pArxObject->GetWindow();
	
	ListData = ListData->rbnext;
	
	AcGePoint3d camera;
	AcGePoint3d target;
	AcGeVector3d up;

	if (ListData->restype == RT3DPOINT)
	{		
		target[X] = ListData->resval.rpoint[X];
		target[Y] = ListData->resval.rpoint[Y];
		target[Z] = ListData->resval.rpoint[Z];
	}
	else
	{
		acedRetVoid();
		return 0;
	}
	ListData = ListData->rbnext;

	if (ListData->restype == RT3DPOINT)
	{
		camera[X] = ListData->resval.rpoint[X];
		camera[Y] = ListData->resval.rpoint[Y];
		camera[Z] = ListData->resval.rpoint[Z];
	}
	else
	{
		acedRetVoid();
		return 0;
	}

	ListData = ListData->rbnext;

	if (ListData->restype == RT3DPOINT)
	{
		up[X] = ListData->resval.rpoint[X];
		up[Y] = ListData->resval.rpoint[Y];
		up[Z] = ListData->resval.rpoint[Z];
	}
	else
	{
		acedRetVoid();
		return 0;
	}

	double fieldWidth;
	double fieldHeight;

	ListData = ListData->rbnext;
	if (ListData == NULL)
	{
		acedRetVoid();
		return 0;
	}
	GetLispRealInput(ListData, fieldWidth);

	ListData = ListData->rbnext;
	if (ListData == NULL)
	{
		acedRetVoid();
		return 0;
	}
	bool bIsPerspective;
	GetLispRealInput(ListData, fieldHeight);

	ListData = ListData->rbnext;
	if (ListData == NULL)
	{
		bIsPerspective = false;
	}
	else	
		GetLispBoolInput(ListData, bIsPerspective);

	if (pControl != NULL && pControl->view() != NULL)
	{
		AcGePoint3d camera = pControl->view()->position();
	}

	if (pControl->view() != NULL)
	{
		pControl->view()->setView(camera, target, up, fieldWidth, fieldHeight, (bIsPerspective ? AcGsView::kPerspective : AcGsView::kParallel));
		pControl->Invalidate();
	}
	
	acedRetVoid();
	return 0;
}