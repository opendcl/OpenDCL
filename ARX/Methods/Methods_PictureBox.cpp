// Methods_PictureBox.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_PictureBox.h"
#include "ArgumentsRetrieval.h"
#include "PictureBox.h"
#include "ErrorLexicon.h"
#include "MethodLexicon.h"
#include "ControlTypes.h"
#include "AcadColorTable.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// Methods_PictureBox
int PictureBox_DrawLine()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_DrawLine, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}
	
	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}

	struct resbuf *ListData;
	int nStartX;
	int nStartY;
	int nEndX;
	int nEndY;
	
	
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				
				
				
				acedRetVoid();
				return 0;
			}
			if (ListData->restype == RTLB) 
			{		
			
				// advance to the first list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawLine);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawLine);	
					acedRetInt(-1);  return 0; 
				}

				// get the first argument required
				nStartX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawLine);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawLine);	
					acedRetInt(-1);  return 0; 
				}
				// get the second argument required
				nStartY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawLine);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawLine);	
					acedRetInt(-1);  return 0; 
				}
				// get the third argument required
				nEndX = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawLine);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawLine);	
					acedRetInt(-1);  return 0; 
				}
				// get the fourth argument required
				nEndY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawLine);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype == RTSHORT) 
				{
					// get the next argument required
					int nLineColor = ListData->resval.rint;
					
					((CPictureBox*)pControl)->DrawLine(
						nStartX,
						nStartY,
						nEndX,
						nEndY,
						GetRGBColor(nLineColor));
				}
				else if (ListData->restype == RTLONG) 
				{
					// get the next argument required
					long lLineColor = ListData->resval.rlong;
					
					((CPictureBox*)pControl)->DrawLine(
						nStartX,
						nStartY,
						nEndX,
						nEndY,
						lLineColor);
				}
				else if (ListData->restype == RT3DPOINT) 
				{
					int nRed = (int)ListData->resval.rpoint[X];
					int nGreen = (int)ListData->resval.rpoint[Y];
					int nBlue = (int)ListData->resval.rpoint[Z];
				
					
					((CPictureBox*)pControl)->DrawLine(
						nStartX,
						nStartY,
						nEndX,
						nEndY,
						RGB(nRed, nGreen, nBlue));
				}
				else
				{					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawLine);	
					acedRetInt(-1);  return 0; 
				}
				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sPictureBox_DrawLine);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sPictureBox_DrawLine);			
        acedRetInt(-1);  return 0; 
	}

	
	
	
	acedRetVoid();
	return 0;
}

int PictureBox_DrawPoint()
{	
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_DrawRect, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (!pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}

	int nStartX;
	int nStartY;
	
	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	

	if (ListData->restype != RTSHORT) 
	{
		
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawRect);	
		acedRetInt(-1);  return 0; 
	}
	
	// get the first argument required
	nStartX = ListData->resval.rint;

	// advance to the next list item
	ListData = ListData->rbnext;
	if (ListData == NULL)
	{
		
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawRect);	
		acedRetInt(-1);  return 0; 
	}
	if (ListData->restype != RTSHORT) 
	{
		
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawRect);	
		acedRetInt(-1);  return 0; 
	}
	
	// get the second argument required
	nStartY = ListData->resval.rint;

		// advance to the next list item
	ListData = ListData->rbnext;
	
	if (ListData == NULL)
	{
		
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawRect);	
		acedRetInt(-1);  return 0; 
	}

	if (ListData->restype == RTSHORT) 
	{
		// get the next argument required
		int nLineColor = ListData->resval.rint;
		
		((CPictureBox*)pControl)->DrawPoint(
				nStartX,
				nStartY,
				GetRGBColor(nLineColor));
	}
	else if (ListData->restype == RTLONG) 
	{
		// get the next argument required
		long lLineColor = ListData->resval.rlong;
		
		((CPictureBox*)pControl)->DrawPoint(
			nStartX,
			nStartY,
			lLineColor);		
	}
	else if (ListData->restype == RT3DPOINT) 
	{
		int nRed = (int)ListData->resval.rpoint[X];
		int nGreen = (int)ListData->resval.rpoint[Y];
		int nBlue = (int)ListData->resval.rpoint[Z];
	
		
		((CPictureBox*)pControl)->DrawPoint(
			nStartX,
			nStartY,
			RGB(nRed, nGreen, nBlue));
	}
	else
	{
		
		// inform the programmer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawRect);	
		acedRetInt(-1);  return 0; 
	}				
	
	acedRetVoid();
	return 0;
	

	/*
	int nStartX;
	int nStartY;
	int nLineColor;

	CWnd *pControl = GetArgsControlIntIntInt(
		CtlPictureBox, 
		sPictureBox_DrawPoint, 
		nStartX,
		nStartY,
		GetRGBColor(nLineColor));

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}

	if (!pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}

	((CPictureBox*)pControl)->DrawPoint(
		nStartX,
		nStartY,
		GetRGBColor(nLineColor));
	
	
	acedRetVoid();
	return 0;
	*/
}


int PictureBox_DrawEdge()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_DrawEdge, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}

	int nStartX;
	int nStartY;
	int nEndX;
	int nEndY;
	int nEdge;

	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				
				
				acedRetVoid();
				return 0;
			}
			if (ListData->restype == RTLB) 
			{		
			
				// advance to the first list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawEdge);	
					acedRetInt(-1);  
					return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawEdge);	
					acedRetInt(-1);  return 0; 
				}
				// get the first argument required
				nStartX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawEdge);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawEdge);	
					acedRetInt(-1);  return 0; 
				}
				
				// get the second argument required
				nStartY = ListData->resval.rint;

					// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawEdge);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawEdge);	
					acedRetInt(-1);  return 0; 
				}
				// get the third argument required
				nEndX = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawEdge);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawEdge);	
					acedRetInt(-1);  return 0; 
				}
				// get the fourth argument required
				nEndY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawEdge);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawEdge);	
					acedRetInt(-1);  return 0; 
				}				
				
				// get the next argument required
				nEdge = ListData->resval.rint;

				((CPictureBox*)pControl)->DrawEdge(
					nStartX,
					nStartY,
					nEndX + nStartX + 1,
					nEndY + nStartY + 1,
					nEdge);

				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sPictureBox_DrawEdge);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sPictureBox_DrawEdge);			
        acedRetInt(-1);  return 0; 
	}

	
	
	
	acedRetVoid();
	return 0;
}


int PictureBox_DrawFocusRect()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_DrawFocusRect, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}


	int nStartX;
	int nStartY;
	int nEndX;
	int nEndY;

	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				
				
				acedRetVoid();
				return 0;
			}
			if (ListData->restype == RTLB) 
			{		
			
				// advance to the first list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawFocusRect);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawFocusRect);	
					acedRetInt(-1);  return 0; 
				}
				// get the first argument required
				nStartX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawFocusRect);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawFocusRect);	
					acedRetInt(-1);  return 0; 
				}
				
				// get the second argument required
				nStartY = ListData->resval.rint;

					// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawFocusRect);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawFocusRect);	
					acedRetInt(-1);  return 0; 
				}
				// get the third argument required
				nEndX = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawFocusRect);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawFocusRect);	
					acedRetInt(-1);  return 0; 
				}
				// get the fourth argument required
				nEndY = ListData->resval.rint;

				((CPictureBox*)pControl)->DrawFocusRect(
					nStartX,
					nStartY,
					nEndX + nStartX + 1,
					nEndY + nStartY + 1);

				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sPictureBox_DrawFocusRect);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sPictureBox_DrawFocusRect);			
        acedRetInt(-1);  return 0; 
	}

	
	
	
	acedRetVoid();
	return 0;


}

int PictureBox_DrawRect()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_DrawRect, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (!pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}

	int nStartX;
	int nStartY;
	int nEndX;
	int nEndY;
	
	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				
				
				acedRetVoid();
				return 0;
			}
			if (ListData->restype == RTLB) 
			{		
			
				// advance to the first list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawRect);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawRect);	
					acedRetInt(-1);  return 0; 
				}
				// get the first argument required
				nStartX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawRect);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawRect);	
					acedRetInt(-1);  return 0; 
				}
				
				// get the second argument required
				nStartY = ListData->resval.rint;

					// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawRect);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawRect);	
					acedRetInt(-1);  return 0; 
				}
				// get the third argument required
				nEndX = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawRect);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawRect);	
					acedRetInt(-1);  return 0; 
				}
				// get the fourth argument required
				nEndY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawRect);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype == RTSHORT) 
				{
					// get the next argument required
					int nLineColor = ListData->resval.rint;
					
					((CPictureBox*)pControl)->DrawRect(
						nStartX,
						nStartY,
						nEndX + nStartX + 1,
						nEndY + nStartY + 1,
						GetRGBColor(nLineColor));
				}
				else if (ListData->restype == RTLONG) 
				{
					// get the next argument required
					long lLineColor = ListData->resval.rlong;
					
					((CPictureBox*)pControl)->DrawRect(
						nStartX,
						nStartY,
						nEndX + nStartX + 1,
						nEndY + nStartY + 1,
						lLineColor);
				}
				else if (ListData->restype == RT3DPOINT) 
				{
					int nRed = (int)ListData->resval.rpoint[X];
					int nGreen = (int)ListData->resval.rpoint[Y];
					int nBlue = (int)ListData->resval.rpoint[Z];
				
					
					((CPictureBox*)pControl)->DrawRect(
						nStartX,
						nStartY,
						nEndX + nStartX + 1,
						nEndY + nStartY + 1,
						RGB(nRed, nGreen, nBlue));
				}
				else
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawRect);	
					acedRetInt(-1);  return 0; 
				}				
				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sPictureBox_DrawRect);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sPictureBox_DrawRect);			
        acedRetInt(-1);  return 0; 
	}

	
	
	
	acedRetVoid();
	return 0;
	

}


int PictureBox_DrawFillRect()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_DrawFillRect, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}


	int nStartX;
	int nStartY;
	int nEndX;
	int nEndY;
	
	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				
				
				
				acedRetVoid();
				return 0;
			}
			if (ListData->restype == RTLB) 
			{		
			
				// advance to the first list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawFillRect);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawFillRect);	
					acedRetInt(-1);  return 0; 
				}
				// get the first argument required
				nStartX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawFillRect);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawFillRect);	
					acedRetInt(-1);  return 0; 
				}
				
				// get the second argument required
				nStartY = ListData->resval.rint;

					// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawFillRect);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawFillRect);	
					acedRetInt(-1);  return 0; 
				}
				// get the third argument required
				nEndX = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawFillRect);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawFillRect);	
					acedRetInt(-1);  return 0; 
				}
				// get the fourth argument required
				nEndY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawFillRect);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype == RTSHORT) 
				{
					// get the next argument required
					int nLineColor = ListData->resval.rint;

					((CPictureBox*)pControl)->DrawFillRect(
						nStartX,
						nStartY,
						nEndX + nStartX,
						nEndY + nStartY,
						GetRGBColor(nLineColor));	
				
				}
				else if (ListData->restype == RTLONG) 
				{
					// get the next argument required
					long lLineColor = ListData->resval.rlong;

					((CPictureBox*)pControl)->DrawFillRect(
						nStartX,
						nStartY,
						nEndX + nStartX,
						nEndY + nStartY,
						lLineColor);
				}
				else if (ListData->restype == RT3DPOINT) 
				{
					int nRed = (int)ListData->resval.rpoint[X];
					int nGreen = (int)ListData->resval.rpoint[Y];
					int nBlue = (int)ListData->resval.rpoint[Z];
					
					((CPictureBox*)pControl)->DrawFillRect(
						nStartX,
						nStartY,
						nEndX + nStartX,
						nEndY + nStartY,
						RGB(nRed, nGreen, nBlue));	
				
				}
				else
				{				
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawFillRect);	
					acedRetInt(-1);  return 0; 
				}

				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sPictureBox_DrawFillRect);	
					acedRetInt(-1);  return 0; 
				}
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sPictureBox_DrawFillRect);			
        acedRetInt(-1);  return 0; 
	}
	
	
	
	acedRetVoid();
	return 0;
		
	
}


int PictureBox_Clear()
{
	
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_Clear);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}

    ((CPictureBox*)pControl)->Clear();
	
	
	acedRetVoid();
	return 0;

}

int PictureBox_StoreImage()
{
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_StoreImage);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}
	
	((CPictureBox*)pControl)->CopyDC();
	
	
	
	acedRetVoid();
	return 0;

}

int PictureBox_Refresh()
{
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_Refresh);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}



	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	((CPictureBox*)pControl)->Refresh();
	
	acedRetVoid();
	return 0;

}

int PictureBox_DrawArc()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_DrawArc, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}


	int nCenterX;
	int nCenterY;
	int nRadius;
	int nStartAngleX;
	int nStartAngleY;
	int nEndAngleX;
	int nEndAngleY;
	
	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				
				
				acedRetVoid();
				return 0;
			}
			if (ListData->restype == RTLB) 
			{		
			
				// advance to the first list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}

				// get the first argument required
				nCenterX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				// get the second argument required
				nCenterY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				// get the third argument required
				nRadius = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				// get the next argument required
				nStartAngleX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				// get the next argument required
				nStartAngleY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				// get the next argument required
				nEndAngleX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				// get the next argument required
				nEndAngleY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype == RTSHORT)
				{
					// get the next argument required
					int nLineColor = ListData->resval.rint;

					((CPictureBox*)pControl)->DrawArc(
						nCenterX-nRadius,
						nCenterY-nRadius,
						nCenterX+nRadius,
						nCenterY+nRadius,
						nStartAngleX,
						nStartAngleY,
						nEndAngleX,
						nEndAngleY,	
						GetRGBColor(nLineColor));
				}
				else if (ListData->restype == RTLONG)
				{
					// get the next argument required
					long lLineColor = ListData->resval.rlong;

					((CPictureBox*)pControl)->DrawArc(
						nCenterX-nRadius,
						nCenterY-nRadius,
						nCenterX+nRadius,
						nCenterY+nRadius,
						nStartAngleX,
						nStartAngleY,
						nEndAngleX,
						nEndAngleY,	
						lLineColor);
				}
				else if (ListData->restype == RT3DPOINT) 
				{
					int nRed = (int)ListData->resval.rpoint[X];
					int nGreen = (int)ListData->resval.rpoint[Y];
					int nBlue = (int)ListData->resval.rpoint[Z];
									
					((CPictureBox*)pControl)->DrawArc(
						nCenterX-nRadius,
						nCenterY-nRadius,
						nCenterX+nRadius,
						nCenterY+nRadius,
						nStartAngleX,
						nStartAngleY,
						nEndAngleX,
						nEndAngleY,	
						RGB(nRed, nGreen, nBlue));
				}
				else
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}
				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sPictureBox_DrawArc);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sPictureBox_DrawArc);			
        acedRetInt(-1);  return 0; 
	}

	
	
	
	acedRetVoid();
	return 0;

}
int PictureBox_DrawCircle()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_DrawCircle, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}


	int	nUpperLeftX;
	int	nUpperLeftY;
	int	nLowerRightX;
	int	nLowerRightY;
	
	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				
				
				acedRetVoid();
				return 0;
			}
			if (ListData->restype == RTLB) 
			{		
			
				// advance to the first list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawCircle);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawCircle);	
					acedRetInt(-1);  return 0; 
				}

				// get the first argument required
				nUpperLeftX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawCircle);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawCircle);	
					acedRetInt(-1);  return 0; 
				}
				// get the second argument required
				nUpperLeftY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawCircle);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawCircle);	
					acedRetInt(-1);  return 0; 
				}
				// get the third argument required
				nLowerRightX = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawCircle);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawCircle);	
					acedRetInt(-1);  return 0; 
				}
				// get the fourth argument required
				nLowerRightY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawCircle);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype == RTSHORT) 
				{
					// get the next argument required
					int nLineColor = ListData->resval.rint;

					
					((CPictureBox*)pControl)->DrawCircle(
						nUpperLeftX,
						nUpperLeftY,
						nUpperLeftX + nLowerRightX + 1,
						nUpperLeftY + nLowerRightY + 1,
						GetRGBColor(nLineColor));
				}
				else if (ListData->restype == RTLONG) 
				{
					// get the next argument required
					long lLineColor = ListData->resval.rlong;

					
					((CPictureBox*)pControl)->DrawCircle(
						nUpperLeftX,
						nUpperLeftY,
						nUpperLeftX + nLowerRightX + 1,
						nUpperLeftY + nLowerRightY + 1,
						lLineColor);
				}
				else if (ListData->restype == RT3DPOINT) 
				{
					int nRed = (int)ListData->resval.rpoint[X];
					int nGreen = (int)ListData->resval.rpoint[Y];
					int nBlue = (int)ListData->resval.rpoint[Z];
				
					
					((CPictureBox*)pControl)->DrawCircle(
						nUpperLeftX,
						nUpperLeftY,
						nUpperLeftX + nLowerRightX + 1,
						nUpperLeftY + nLowerRightY + 1,
						RGB(nRed, nGreen, nBlue));
				}
				else
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawCircle);	
					acedRetInt(-1);  return 0; 
				}
				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sPictureBox_DrawCircle);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sPictureBox_DrawCircle);			
        acedRetInt(-1); 
		return 0; 
	}

		
	
	acedRetVoid();
	return 0;
	

}

int PictureBox_DrawHatchRect()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_DrawHatchRect, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}


	int nStartX;
	int nStartY;
	int nEndX;
	int nEndY;
	int nHatchPattern;

	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				
				
				acedRetVoid();
				return 0;
			}
			if (ListData->restype == RTLB) 
			{		
			
				// advance to the first list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawHatchRect);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawHatchRect);	
					acedRetInt(-1);  return 0; 
				}
				// get the first argument required
				nStartX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawHatchRect);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawHatchRect);	
					acedRetInt(-1);  return 0; 
				}
				
				// get the second argument required
				nStartY = ListData->resval.rint;

					// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawHatchRect);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawHatchRect);	
					acedRetInt(-1);  return 0; 
				}
				// get the third argument required
				nEndX = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawHatchRect);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawHatchRect);	
					acedRetInt(-1);  return 0; 
				}
				// get the fourth argument required
				nEndY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawHatchRect);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype == RTSHORT) 
				{
					// get the next argument required
					int nLineColor = ListData->resval.rint;

					ListData = ListData->rbnext;
					if (ListData == NULL)
					{
						
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawHatchRect);	
						acedRetInt(-1);  return 0; 
					}
				
					if (ListData->restype != RTSHORT) 
					{
						
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawHatchRect);	
						acedRetInt(-1);  return 0; 
					}				
					
					// get the next argument required
					nHatchPattern = ListData->resval.rint;
					
					((CPictureBox*)pControl)->DrawHatchRect(
						nStartX,
						nStartY,
						nEndX + nStartX + 1,
						nEndY + nStartY + 1,
						GetRGBColor(nLineColor),
						nHatchPattern);
				}
				else if (ListData->restype == RTLONG) 
				{
					// get the next argument required
					long lLineColor = ListData->resval.rlong;

					ListData = ListData->rbnext;
					if (ListData == NULL)
					{
						
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawHatchRect);	
						acedRetInt(-1);  return 0; 
					}
				
					if (ListData->restype != RTSHORT) 
					{
						
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawHatchRect);	
						acedRetInt(-1);  return 0; 
					}				
					
					// get the next argument required
					nHatchPattern = ListData->resval.rint;
					
					((CPictureBox*)pControl)->DrawHatchRect(
						nStartX,
						nStartY,
						nEndX + nStartX + 1,
						nEndY + nStartY + 1,
						lLineColor,
						nHatchPattern);
				}
				else if (ListData->restype == RT3DPOINT) 
				{
					int nRed = (int)ListData->resval.rpoint[X];
					int nGreen = (int)ListData->resval.rpoint[Y];
					int nBlue = (int)ListData->resval.rpoint[Z];
				
					// gets the hatch type
					ListData = ListData->rbnext;
										
					if (ListData->restype != RTSHORT) 
					{
						
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawHatchRect);	
						acedRetInt(-1);  return 0; 
					}				
					
					// get the next argument required
					nHatchPattern = ListData->resval.rint;
					
					((CPictureBox*)pControl)->DrawHatchRect(
						nStartX,
						nStartY,
						nEndX + nStartX + 1,
						nEndY + nStartY + 1,
						RGB(nRed, nGreen, nBlue),
						nHatchPattern);
				}
				else
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawHatchRect);	
					acedRetInt(-1);  return 0; 
				}				
				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sPictureBox_DrawHatchRect);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sPictureBox_DrawHatchRect);			
        acedRetInt(-1);  return 0; 
	}

	
	
	
	acedRetVoid();
	return 0;
		

	
}

int PictureBox_DrawWrappedText()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_DrawWrappedText, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}


	int nStartX;
	int nStartY;
	int nTextWidth;
	int nForeColor;
	int nBackColor;
	CString sText;
	CString sJustification;

	int nTextHeight;
	
	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				
				
				acedRetVoid();
				return 0;
			}
			if (ListData->restype == RTLB) 
			{		
			
				// advance to the first list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}
				// get the first argument required
				nStartX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}
				
				// get the second argument required
				nStartY = ListData->resval.rint;

					// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}
				// get the third argument required
				nTextWidth = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}
				// get the fourth argument required
				nForeColor = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSTR) 
				{
					if (ListData->restype != RTSHORT) 
					{
						
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawText);	
						acedRetInt(-1);  return 0; 
					}				
					
					// get the next argument required
					nBackColor = ListData->resval.rint;
				
					ListData = ListData->rbnext;
					if (ListData == NULL)
					{
						
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawText);	
						acedRetInt(-1);  return 0; 
					}
			
				}
				else
				{
					nBackColor = nUseBackColor;
				}
				
				
				if (ListData->restype != RTSTR) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}				
				
				// get the next argument required
				sText = ListData->resval.rstring;
			
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSTR) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}				
				
				// get the next argument required
				sJustification = ListData->resval.rstring;
				
				nTextHeight = ((CPictureBox*)pControl)->DrawWrappedText(
					nStartX,
					nStartY,
					nStartX + nTextWidth,
					nForeColor,
					nBackColor,
					sText,
					sJustification);
			
				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sPictureBox_DrawWrappedText);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sPictureBox_DrawWrappedText);			
        acedRetInt(-1);  return 0; 
	}

	
		
	// return nTextHeight
	acedRetInt(nTextHeight);
	return 0;

}
int PictureBox_GetTextExtent()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_GetTextExtent, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}

	CString sText;

	if (!GetStringArgument(nArg, &sText, sPictureBox_GetTextExtent) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}

	((CPictureBox*)pControl)->GetTextExtent(sText);
	
	return 0;

}
int PictureBox_DrawText()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_DrawText, &nArg);
	
	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}
	
	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}
	
	int nStartX;
	int nStartY;
	int nForeColor;
	int nBackColor;
	CString sText;
	CString sJustification;
	int nDisabled;

	struct resbuf *ListData;
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				//
				
				acedRetVoid();
				return 0;
			}
			if (ListData->restype == RTLB) 
			{		
			
				// advance to the first list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawText);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawText);	
					acedRetInt(-1);  return 0; 
				}
				// get the first argument required
				nStartX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawText);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawText);	
					acedRetInt(-1);  return 0; 
				}
				
				// get the second argument required
				nStartY = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawText);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawText);	
					acedRetInt(-1);  return 0; 
				}
				// get the fourth argument required
				nForeColor = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawText);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSTR) 
				{
					if (ListData->restype != RTSHORT) 
					{
						
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawText);	
						acedRetInt(-1);  return 0; 
					}				
					
					// get the next argument required
					nBackColor = ListData->resval.rint;
				
					ListData = ListData->rbnext;
					if (ListData == NULL)
					{
						
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawText);	
						acedRetInt(-1);  return 0; 
					}
			
				}
				else
				{
					nBackColor = nUseBackColor;
				}
				
				if (ListData->restype != RTSTR) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sPictureBox_DrawText);	
					acedRetInt(-1);  return 0; 
				}				
				
				// get the next argument required
				sText = ListData->resval.rstring;
			
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_DrawText);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSTR) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sPictureBox_DrawText);	
					acedRetInt(-1);  return 0; 
				}				
				
				// get the next argument required
				sJustification = ListData->resval.rstring;
				
				ListData = ListData->rbnext;
				if (ListData->restype == RTLE)
				{
					nDisabled = 0;
				}
				else if (ListData->restype != RTSHORT && ListData->restype != RTLONG && ListData->restype != RTT && ListData->restype != RTNIL) 
				{					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_DrawText);	
					acedRetInt(-1);  return 0; 
				}	
				else if (ListData->restype == RTSHORT) 
					// get the next argument required
					nDisabled = ListData->resval.rint;
				else if (ListData->restype == RTLONG) 
					// get the next argument required
					nDisabled = ListData->resval.rlong;
				else if (ListData->restype == RTT) 
					// get the next argument required
					nDisabled = 1;
				else if (ListData->restype == RTNIL) 
					// get the next argument required
					nDisabled = 0;

				((CPictureBox*)pControl)->DrawText(
					nStartX,
					nStartY,
					nForeColor,
					nBackColor,
					sText,
					sJustification,
					nDisabled);
				
				if (ListData->restype != RTLE) 
					// advance to the next list item
					ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sPictureBox_DrawText);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sPictureBox_DrawText);			
        acedRetInt(-1);  return 0; 
	}

		
	
	
	acedRetVoid();
	return 0;

}

int PictureBox_PaintPicture()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_PaintPicture, &nArg);
	
	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}


	int sX; 
	int sY;
	int	nPictureID;
	int	nEnabled = 1;
	int nUseMask = 1;
	
	struct resbuf *ListData;
	
	//ensure AutoLisp has passed Arguments	
	ListData = acedGetArgs();
	
	for (int i = 0; i < nArg; i++)
	{
		// first iterate forward to the next required argument
		ListData = ListData->rbnext;
	}

	if (ListData == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	

	if (ListData->restype == RTLB) 
	{		
		bool bDoLoop = true;
		while (bDoLoop)
		{
			// advance to the first list item
			ListData = ListData->rbnext;

			if (ListData->restype == RTLE ||
				ListData->restype == NULL) 
			{
				
				
				acedRetVoid();
				return 0;
			}
			if (ListData->restype == RTLB) 
			{		
			
				// advance to the first list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_PaintPicture);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_PaintPicture);	
					acedRetInt(-1);  return 0; 
				}
				// get the first argument required
				sX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_PaintPicture);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_PaintPicture);	
					acedRetInt(-1);  return 0; 
				}
				
				// get the second argument required
				sY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sPictureBox_PaintPicture);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_PaintPicture);	
					acedRetInt(-1);  return 0; 
				}
				// get the third argument required
				nPictureID = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				
				if (ListData->restype == RTLE)
				{
					nEnabled = 1;
				}				
				else if (ListData->restype != RTSHORT &&
					ListData->restype != RTLONG &&
					ListData->restype != RTT &&
					ListData->restype != RTNIL) 
				{					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_PaintPicture);	
					acedRetInt(-1);  return 0; 
				}
				else if (ListData->restype == RTSHORT) 
					// get the fourth argument required
					nEnabled = ListData->resval.rint;
				else if (ListData->restype == RTLONG) 
					// get the fourth argument required
					nEnabled = ListData->resval.rlong;
				else if (ListData->restype == RTT) 
					// get the fourth argument required
					nEnabled = 1;
				else if (ListData->restype == RTNIL) 
					// get the fourth argument required
					nEnabled = 0;

				if (ListData->restype != RTLE) 
					// advance to the next list item
					ListData = ListData->rbnext;

				if (ListData->restype == RTLE)
				{
					nUseMask = 0;
				}				
				
				else if (ListData->restype != RTSHORT &&
					ListData->restype != RTLONG &&
					ListData->restype != RTT &&
					ListData->restype != RTNIL) 
				{
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sPictureBox_PaintPicture);	
					acedRetInt(-1);  return 0; 
				}
				else if (ListData->restype == RTSHORT) 
					// get the fourth argument required
					nUseMask = ListData->resval.rint;
				else if (ListData->restype == RTLONG) 
					// get the fourth argument required
					nUseMask = ListData->resval.rlong;
				else if (ListData->restype == RTT) 
					// get the fourth argument required
					nUseMask = 1;
				else if (ListData->restype == RTNIL) 
					// get the fourth argument required
					nUseMask = 0;

				if (ListData->restype != RTLE) 
					// advance to the next list item
					ListData = ListData->rbnext;

				((CPictureBox*)pControl)->PaintPicture(
						sX, 
						sY, 
						nPictureID,
						nEnabled,
						nUseMask);

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sPictureBox_PaintPicture);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sPictureBox_PaintPicture);			
        acedRetInt(-1);  return 0; 
	}

	
	
	
	acedRetVoid();
	return 0;
		

	

}


int PictureBox_LoadPictureFile()
{
	int nArg;
	int nStretch = 0;
	
	CWnd *pControl = GetControlPointer(CtlPictureBox, sPictureBox_LoadPictureFile, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}
	if (pControl && !pControl->GetParent()->IsWindowVisible())		
	{
		
		acedRetVoid();	
		return 0;
	}


	CString sFileName;

	if (!GetStringArgument(nArg, &sFileName, sPictureBox_LoadPictureFile))
	{	
	 	acedRetInt(-1);
		return 0;
	}

	nArg++;

	if (!FindOptionalIntArgument(nArg, &nStretch, sPictureBox_LoadPictureFile))
	{
		//nAscending = nCol;
	}

	CString sPath = theWorkspace.FindFile( sFileName ); 
	if( sPath.IsEmpty() )
	{
		theWorkspace.DisplayAlert(ErrorFileNotFound);
		return 0;
	}

	((CPictureBox*)pControl)->LoadPictureFile(sPath, nStretch == 1);
	
	
	acedRetVoid();
	return 0;

}

