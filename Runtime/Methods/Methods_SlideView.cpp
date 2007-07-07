// Methods_SlideView.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_SlideView.h"
#include "SlideHolder.h"
#include "ArgumentsRetrieval.h"
#include "ErrorLexicon.h"
#include "MethodLexicon.h"
#include "ControlTypes.h"
#include "Workspace.h"

const TCHAR sIsPathed[] = _T(":\\");
const TCHAR sNextLine[] = _T("\n");


/////////////////////////////////////////////////////////////////////////////
// Methods_SlideView
//*****************************************************************************
// 
// Method: SlideView_SetFileName()
// 
// Purpose: [load and redisplay the control with the requested slide]
// 
// Parameters: none
// 
// Returns:	int
// 
//*****************************************************************************
int SlideView_SetFileName()
{
	bool bFoundFile;
	CString sFileName;
	CString sSlideName;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlSlideView, sSlideView_SetFileName, &nArg);

	if (!GetStringArgument(nArg, &sFileName, sSlideView_SetFileName) || pControl == NULL)
	{		
		acedRetNil();
		return 0;
	}
	if (sFileName.Find(sIsPathed) == -1)
	{
		CString sPath = theWorkspace.FindFile( sFileName );
		if( sPath.IsEmpty() )
		{
			theWorkspace.DisplayAlert(CString(ErrorFileNotFound) + sNextLine + sFileName);
			acedRetNil();
			return 0;
		}
		nArg++;
		if (!FindOptionalStringArgument(nArg, &sSlideName, sSlideView_SetFileName) || pControl == NULL)
			bFoundFile = ((CSlideHolder*)pControl)->SetFileName(sPath, false, CString());
		else
			bFoundFile = ((CSlideHolder*)pControl)->SetFileName(sPath, true, sSlideName);
	}
	else
	{
		nArg++;
		if (!FindOptionalStringArgument(nArg, &sSlideName, sSlideView_SetFileName) || pControl == NULL)
			bFoundFile = ((CSlideHolder*)pControl)->SetFileName(sFileName, false, CString());
		else
			bFoundFile = ((CSlideHolder*)pControl)->SetFileName(sFileName, true, sSlideName);
	}
	
	if (bFoundFile)
		acedRetT();
	else
		acedRetNil();

	return 0;
}

int SlideView_Clear()
{
	CWnd *pControl = GetControlPointer(CtlSlideView, sSlideView_Clear);

	if (pControl == NULL)		
	{
		
		acedRetInt(-1);
		return 0;
	}
	// call the method to clear the control
	((CSlideHolder*)pControl)->Clear();
	
	acedRetVoid();
	return 0;
}

int SlideView_SetHighLight()
{
	int nArg;
	CWnd *pControl = GetControlPointer(
		CtlSlideView,
		sSlideView_SetHighLight,
		&nArg);

	int nColor;

	if (!GetIntArgument(nArg, &nColor, sSlideView_SetHighLight) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}

	
	((CSlideHolder*)pControl)->SetHighLight(nColor);
	
	
	acedRetVoid();
	return 0;
}

int SlideView_RemoveHighLight()
{
	CWnd *pControl = GetControlPointer(
		CtlSlideView,
		sSlideView_RemoveHighLight);

	if (pControl != NULL)
		((CSlideHolder*)pControl)->RemoveHighLight();
	
	
	acedRetVoid();
	return 0;
}

int SlideView_VectorImage()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlSlideView, sSlideView_VectorImage, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1);
		return 0;		
	}
	
	
	struct resbuf *ListData;
	int nStartX;
	int nStartY;
	int nEndX;
	int nEndY;
	int nLineColor;

	
	//ensure AutoLISP has passed Arguments	
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
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_VectorImage);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_VectorImage);	
					acedRetInt(-1);  return 0; 
				}

				// get the first argument required
				nStartX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_VectorImage);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_VectorImage);	
					acedRetInt(-1);  return 0; 
				}
				// get the second argument required
				nStartY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_VectorImage);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_VectorImage);	
					acedRetInt(-1);  return 0; 
				}
				// get the third argument required
				nEndX = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_VectorImage);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_VectorImage);	
					acedRetInt(-1);  return 0; 
				}
				// get the fourth argument required
				nEndY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_VectorImage);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_VectorImage);	
					acedRetInt(-1);  return 0; 
				}
				// get the next argument required
				nLineColor = ListData->resval.rint;

				((CSlideHolder*)pControl)->DrawLine(
					nStartX,
					nStartY,
					nEndX,
					nEndY,
					nLineColor);

				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sSlideView_VectorImage);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sSlideView_VectorImage);			
        acedRetInt(-1);  return 0; 
	}

	
	
	
	acedRetVoid();
	return 0;
}

int SlideView_FillImage()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlSlideView, sSlideView_FillImage, &nArg);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	


	int nStartX;
	int nStartY;
	int nEndX;
	int nEndY;
	int nLineColor;

	struct resbuf *ListData;
	//ensure AutoLISP has passed Arguments	
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
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_FillImage);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_FillImage);	
					acedRetInt(-1);  return 0; 
				}
				// get the first argument required
				nStartX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_FillImage);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_FillImage);	
					acedRetInt(-1);  return 0; 
				}
				
				// get the second argument required
				nStartY = ListData->resval.rint;

					// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_FillImage);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_FillImage);	
					acedRetInt(-1);  return 0; 
				}
				// get the third argument required
				nEndX = ListData->resval.rint;
				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_FillImage);	
					acedRetInt(-1);  return 0; 
				}
				
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_FillImage);	
					acedRetInt(-1);  return 0; 
				}
				// get the fourth argument required
				nEndY = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_FillImage);	
					acedRetInt(-1);  return 0; 
				}
			
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_FillImage);	
					acedRetInt(-1);  return 0; 
				}				
				
				// get the next argument required
				nLineColor = ListData->resval.rint;

				((CSlideHolder*)pControl)->DrawFillRect(
					nStartX,
					nStartY,
					nEndX + nStartX,
					nEndY + nStartY,
					nLineColor);
	
				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sSlideView_FillImage);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sSlideView_FillImage);			
        acedRetInt(-1);  return 0; 
	}
	
	
	
	acedRetVoid();
	return 0;
}

int SlideView_EndImage()
{
	CWnd *pControl = GetControlPointer(CtlPictureBox, sSlideView_EndImage);

	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	
	
	
	((CSlideHolder*)pControl)->CopyDC();
	

	
	acedRetVoid();
	return 0;

}


int SlideView_SlideImage()
{
	int nArg;
	CWnd *pControl = GetControlPointer(CtlPictureBox, sSlideView_SlideImage, &nArg);
	
	if (pControl == NULL)
	{
		acedRetInt(-1); 
		return 0;
	}

	if (pControl->m_hWnd == NULL)
	{
		
		acedRetVoid();	
		return 0;
	}


	int sX; 
	int sY;
	
	struct resbuf *ListData;
	
	//ensure AutoLISP has passed Arguments	
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
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_SlideImage);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_SlideImage);	
					acedRetInt(-1);  return 0; 
				}
				// get the first argument required
				sX = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_SlideImage);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_SlideImage);	
					acedRetInt(-1);  return 0; 
				}
				
				// get the second argument required
				sY = ListData->resval.rint;

				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_SlideImage);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_SlideImage);	
					acedRetInt(-1);  return 0; 
				}
				
				// get the second argument required
				int nDrawWidth = ListData->resval.rint;

				
				// advance to the next list item
				ListData = ListData->rbnext;
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_SlideImage);	
					acedRetInt(-1);  return 0; 
				}
				if (ListData->restype != RTSHORT) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotInt) + sSlideView_SlideImage);	
					acedRetInt(-1);  return 0; 
				}
				
				// get the second argument required
				int nDrawHeight = ListData->resval.rint;

				// advance to the next list item
				ListData = ListData->rbnext;
				
				if (ListData == NULL)
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_SlideImage);	
					acedRetInt(-1);  return 0; 
				}				
				
				if (ListData->restype != RTSTR) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sSlideView_SlideImage);	
					acedRetInt(-1);  return 0; 
				}
				// get the fourth argument required
				CString sFileName = theWorkspace.FindFile( ListData->resval.rstring );
				if( sFileName.IsEmpty() )
				{
					theWorkspace.DisplayAlert(CString(ErrorFileNotFound) + sNextLine + ListData->resval.rstring);
					acedRetInt(-1);
					return 0;
				}

				// advance to the next list item
				ListData = ListData->rbnext;

				if (ListData->restype == RTLE) 
				{
					((CSlideHolder*)pControl)->DrawASlide(
							sX, 
							sY, 
							nDrawWidth,
							nDrawHeight,
							sFileName);
				}
				else
				{
					if (ListData == NULL)
					{
						
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListNotSet) + sSlideView_SlideImage);	
						acedRetInt(-1);  return 0; 
					}				
					
					if (ListData->restype != RTSTR) 
					{
						
						// inform the programmer that he did not make the correct call
						theWorkspace.DisplayAlert(CString(ErrorListArgNotStr) + sSlideView_SlideImage);	
						acedRetInt(-1);  return 0; 
					}
					// get the fourth argument required
					CString sLibSldName = ListData->resval.rstring;

					// advance to the next list item
					ListData = ListData->rbnext;



					((CSlideHolder*)pControl)->DrawASlide(
							sX, 
							sY, 
							nDrawWidth,
							nDrawHeight,
							sFileName,
							sLibSldName);
				}

				if (ListData->restype != RTLE) 
				{
					
					// inform the programmer that he did not make the correct call
					theWorkspace.DisplayAlert(CString(ErrorToManyItemsInList) + sSlideView_SlideImage);	
					acedRetInt(-1);  return 0; 
				}			
			}
		}
	}
	else
	{	
		
		// inform the programer that he did not make the correct call
		theWorkspace.DisplayAlert(CString(ErrorListWasExpected) + sSlideView_SlideImage);			
        acedRetInt(-1);  return 0; 
	}

	
	
	
	acedRetVoid();
	return 0;
		

	

}



