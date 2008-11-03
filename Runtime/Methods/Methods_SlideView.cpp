// Methods_SlideView.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_SlideView.h"
#include "ArgumentsRetrieval.h"
#include "ArxAcadSlideCtrl.h"
#include "ControlTypes.h"
#include "Workspace.h"

const TCHAR sIsPathed[] = _T(":\\");
const TCHAR sNextLine[] = _T("\n");


ADSRESULT SlideView::Load()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlSlideView))
		return RSERR; //invalid input

	CString sFilename;
	if( !GetStringArgument( pArgs, sFilename ) )
		return RSERR; //invalid input

	CString sSlideName;
	GetStringArgument( pArgs, sSlideName, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CString sSlidePath = theWorkspace.FindFile( sFilename );
	if( sSlidePath.IsEmpty() )
		return RSRSLT;

	CArxAcadSlideCtrl* pCtrl = (CArxAcadSlideCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SetFileName( sSlidePath, sSlideName.IsEmpty()? NULL : (LPCTSTR)sSlideName ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT SlideView::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlSlideView))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxAcadSlideCtrl* pCtrl = (CArxAcadSlideCtrl*)pDlgControl->GetControlWnd();

	pCtrl->Clear();
	acedRetT();
	return RSRSLT;
}

ADSRESULT SlideView::SetHighLight()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlSlideView))
		return RSERR; //invalid input

	int nColor;
	if( !GetIntArgument( pArgs, nColor ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxAcadSlideCtrl* pCtrl = (CArxAcadSlideCtrl*)pDlgControl->GetControlWnd();

	pCtrl->SetHighLight( nColor );
	acedRetT();
	return RSRSLT;
}

ADSRESULT SlideView::RemoveHighLight()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlSlideView))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxAcadSlideCtrl* pCtrl = (CArxAcadSlideCtrl*)pDlgControl->GetControlWnd();

	pCtrl->RemoveHighLight();
	acedRetT();
	return RSRSLT;
}

ADSRESULT SlideView::VectorImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlSlideView))
		return RSERR; //invalid input

	CArxAcadSlideCtrl* pCtrl = (CArxAcadSlideCtrl*)pDlgControl->GetControlWnd();

	if( GetNilArgument( pArgs, true ) )
	{
		acedRetT();
		return RSRSLT;
	}

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	if( GetListEndArgument( pArgs, true ) )
		return RSRSLT;

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	do
	{
		int nStartX;
		if( !GetIntArgument( pArgs, nStartX ) )
			return RSERR; //invalid input

		int nStartY;
		if( !GetIntArgument( pArgs, nStartY ) )
			return RSERR; //invalid input

		int nEndX;
		if( !GetIntArgument( pArgs, nEndX ) )
			return RSERR; //invalid input

		int nEndY;
		if( !GetIntArgument( pArgs, nEndY ) )
			return RSERR; //invalid input

		COLORREF crColor;
		if( !GetColorArgument( pArgs, crColor ) )
			return RSERR; //invalid input

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		pCtrl->DrawLine( nStartX, nStartY, nEndX, nEndY, crColor );
		
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT SlideView::FillImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlSlideView))
		return RSERR; //invalid input

	CArxAcadSlideCtrl* pCtrl = (CArxAcadSlideCtrl*)pDlgControl->GetControlWnd();

	if( GetNilArgument( pArgs, true ) )
	{
		acedRetT();
		return RSRSLT;
	}

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	if( GetListEndArgument( pArgs, true ) )
	{
		acedRetT();
		return RSRSLT;
	}

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	do
	{
		int nStartX;
		if( !GetIntArgument( pArgs, nStartX ) )
			return RSERR; //invalid input

		int nStartY;
		if( !GetIntArgument( pArgs, nStartY ) )
			return RSERR; //invalid input

		int nWidth;
		if( !GetIntArgument( pArgs, nWidth ) )
			return RSERR; //invalid input

		int nHeight;
		if( !GetIntArgument( pArgs, nHeight ) )
			return RSERR; //invalid input

		COLORREF crColor;
		if( !GetColorArgument( pArgs, crColor ) )
			return RSERR; //invalid input

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		pCtrl->DrawFillRect( nStartX, nStartY, nStartX + nWidth, nStartY + nHeight, crColor );
		
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT SlideView::EndImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlSlideView))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxAcadSlideCtrl* pCtrl = (CArxAcadSlideCtrl*)pDlgControl->GetControlWnd();

	pCtrl->CopyDC();
	acedRetT();
	return RSRSLT;
}


ADSRESULT SlideView::SlideImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlSlideView))
		return RSERR; //invalid input

	CArxAcadSlideCtrl* pCtrl = (CArxAcadSlideCtrl*)pDlgControl->GetControlWnd();

	if( GetNilArgument( pArgs, true ) )
	{
		acedRetT();
		return RSRSLT;
	}

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	if( GetListEndArgument( pArgs, true ) )
		return RSRSLT;

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	do
	{
		int nStartX;
		if( !GetIntArgument( pArgs, nStartX ) )
			return RSERR; //invalid input

		int nStartY;
		if( !GetIntArgument( pArgs, nStartY ) )
			return RSERR; //invalid input

		int nWidth;
		if( !GetIntArgument( pArgs, nWidth ) )
			return RSERR; //invalid input

		int nHeight;
		if( !GetIntArgument( pArgs, nHeight ) )
			return RSERR; //invalid input

		CString sFilename;
		if( !GetStringArgument( pArgs, sFilename ) )
			return RSERR; //invalid input

		CString sSlideName;
		GetStringArgument( pArgs, sSlideName, true );

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		pCtrl->DrawASlide( nStartX, nStartY, nStartX + nWidth, nStartY + nHeight, sFilename, (sSlideName.IsEmpty()? NULL : (LPCTSTR)sSlideName) );
		
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}
