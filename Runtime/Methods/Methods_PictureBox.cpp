// Methods_PictureBox.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_PictureBox.h"
#include "ArgumentsRetrieval.h"
#include "ArxPictureBoxCtrl.h"
#include "ControlTypes.h"
#include "AcadColorTable.h"
#include "Workspace.h"


ADSRESULT PictureBox::DrawLine()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	if( !pCtrl->IsWindowVisible() )
		return RSRSLT;

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

ADSRESULT PictureBox::DrawPoint()
{	
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	if( !pCtrl->IsWindowVisible() )
		return RSRSLT;

	int nX;
	if( !GetIntArgument( pArgs, nX ) )
		return RSERR; //invalid input

	int nY;
	if( !GetIntArgument( pArgs, nY ) )
		return RSERR; //invalid input

	COLORREF crColor;
	if( !GetColorArgument( pArgs, crColor ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	pCtrl->DrawPoint( nX, nY, crColor );

	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::DrawEdge()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	if( !pCtrl->IsWindowVisible() )
		return RSRSLT;

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

		int nBorder;
		if( !GetIntArgument( pArgs, nBorder ) )
			return RSERR; //invalid input

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		pCtrl->DrawEdge( nStartX, nStartY, nStartX + nWidth + 1, nStartY + nHeight + 1, nBorder );
		
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::DrawFocusRect()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	if( !pCtrl->IsWindowVisible() )
		return RSRSLT;

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

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		pCtrl->DrawFocusRect( nStartX, nStartY, nStartX + nWidth + 1, nStartY + nHeight + 1 );
		
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::DrawRect()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	if( !pCtrl->IsWindowVisible() )
		return RSRSLT;

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

		COLORREF crColor;
		if( !GetColorArgument( pArgs, crColor ) )
			return RSERR; //invalid input

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		pCtrl->DrawRect( nStartX, nStartY, nStartX + nWidth + 1, nStartY + nHeight + 1, crColor );
		
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::DrawFillRect()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	if( !pCtrl->IsWindowVisible() )
		return RSRSLT;

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

ADSRESULT PictureBox::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	pCtrl->Clear();
	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::StoreImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	pCtrl->CopyDC();
	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::Refresh()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	pCtrl->Refresh();
	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::DrawArc()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	if( !pCtrl->IsWindowVisible() )
		return RSRSLT;

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
		int nCenterX;
		if( !GetIntArgument( pArgs, nCenterX ) )
			return RSERR; //invalid input

		int nCenterY;
		if( !GetIntArgument( pArgs, nCenterY ) )
			return RSERR; //invalid input

		int nRadius;
		if( !GetIntArgument( pArgs, nRadius ) )
			return RSERR; //invalid input

		int nStartAngleX;
		if( !GetIntArgument( pArgs, nStartAngleX ) )
			return RSERR; //invalid input

		int nStartAngleY;
		if( !GetIntArgument( pArgs, nStartAngleY ) )
			return RSERR; //invalid input

		int nEndAngleX;
		if( !GetIntArgument( pArgs, nEndAngleX ) )
			return RSERR; //invalid input

		int nEndAngleY;
		if( !GetIntArgument( pArgs, nEndAngleY ) )
			return RSERR; //invalid input

		COLORREF crColor;
		if( !GetColorArgument( pArgs, crColor ) )
			return RSERR; //invalid input

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		pCtrl->DrawArc( nCenterX - nRadius, nCenterY - nRadius, nCenterX + nRadius, nCenterY + nRadius,
										nStartAngleX, nStartAngleY, nEndAngleX, nEndAngleY, crColor );
		
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::DrawCircle()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	if( !pCtrl->IsWindowVisible() )
		return RSRSLT;

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

		COLORREF crColor;
		if( !GetColorArgument( pArgs, crColor ) )
			return RSERR; //invalid input

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		pCtrl->DrawCircle( nStartX, nStartY, nStartX + nWidth + 1, nStartY + nHeight + 1, crColor );
		
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::DrawHatchRect()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	if( !pCtrl->IsWindowVisible() )
		return RSRSLT;

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

		COLORREF crColor;
		if( !GetColorArgument( pArgs, crColor ) )
			return RSERR; //invalid input

		int nPattern;
		if( !GetIntArgument( pArgs, nPattern ) )
			return RSERR; //invalid input

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		pCtrl->DrawHatchRect( nStartX, nStartY, nStartX + nWidth + 1, nStartY + nHeight + 1, crColor, nPattern );
		
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::DrawWrappedText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	if( !pCtrl->IsWindowVisible() )
		return RSRSLT;

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

		COLORREF crFore;
		if( !GetColorArgument( pArgs, crFore ) )
			return RSERR; //invalid input

		COLORREF crBack = COLOR_USEBACKGROUND;
		GetColorArgument( pArgs, crBack, true );

		CString sText;
		if( !GetStringArgument( pArgs, sText ) )
			return RSERR; //invalid input

		CString sJustification;
		if( !GetStringArgument( pArgs, sJustification ) )
			return RSERR; //invalid input

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		pCtrl->DrawWrappedText( nStartX, nStartY, nStartX + nWidth + 1, crFore, crBack, sText, sJustification );
		
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::GetTextExtent()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CString sText;
	if( !GetStringArgument( pArgs, sText ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	CSize szExt;
	pCtrl->GetTextExtent( sText, szExt );
	resbuf rbHeight = { NULL, RTSHORT };
	rbHeight.resval.rint = szExt.cy;
	resbuf rbWidth = { &rbHeight, RTSHORT };
	rbWidth.resval.rint = szExt.cx;
	acedRetList( &rbWidth );
	return RSRSLT;
}

ADSRESULT PictureBox::DrawText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	if( !pCtrl->IsWindowVisible() )
		return RSRSLT;

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

		COLORREF crFore;
		if( !GetColorArgument( pArgs, crFore ) )
			return RSERR; //invalid input

		COLORREF crBack = COLOR_USEBACKGROUND;
		GetColorArgument( pArgs, crBack, true );

		CString sText;
		if( !GetStringArgument( pArgs, sText ) )
			return RSERR; //invalid input

		CString sJustification;
		if( !GetStringArgument( pArgs, sJustification ) )
			return RSERR; //invalid input

		bool bDisabled = false;
		GetBoolArgument( pArgs, bDisabled, true );

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		pCtrl->DrawText( nStartX, nStartY, crFore, crBack, sText, sJustification, (bDisabled? 1 : 0) );
		
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::PaintPicture()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	if( !pCtrl->IsWindowVisible() )
		return RSRSLT;

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

		int nPicID;
		if( !GetIntArgument( pArgs, nPicID ) )
			return RSERR; //invalid input

		bool bEnabled = false;
		GetBoolArgument( pArgs, bEnabled, true );

		bool bMasked = false;
		GetBoolArgument( pArgs, bMasked, true );

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		pCtrl->PaintPicture( nStartX, nStartY, nPicID, (bEnabled? 1 : 0), (bMasked? 1 : 0) );
		
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT PictureBox::LoadPictureFile()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlPictureBox))
		return RSERR; //invalid input

	CString sFilename;
	if( !GetStringArgument( pArgs, sFilename ) )
		return RSERR; //invalid input

	bool bStretch = false;
	GetBoolArgument( pArgs, bStretch, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;


	CArxPictureBoxCtrl* pCtrl = (CArxPictureBoxCtrl*)pDlgControl->GetControlWnd();
	pCtrl->LoadPictureFile( sFilename, bStretch );
	acedRetT();
	return RSRSLT;
}
