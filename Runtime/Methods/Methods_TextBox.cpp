// Methods_TextBox.cpp : implementation file
//


#include "stdafx.h"
#include "Methods_TextBox.h"
#include "ArgumentsRetrieval.h"
#include "TextBoxCtrl.h"
#include "CustomFilter.h"


ADSRESULT TextBox::GetFilter()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CTextBoxCtrl* pCtrl = (CTextBoxCtrl*)pDlgControl->GetControlWnd();

	CInputFilter* pFilter = pCtrl->GetInputFilter();
	CString sFilter;
	if( pFilter )
		sFilter = pFilter->GetFilter();
	acedRetStr( sFilter );
	return RSRSLT;
}

ADSRESULT TextBox::SetFilter()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	CString sFilter;
	if( !GetStringArgument( pArgs, sFilter ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CTextBoxCtrl* pCtrl = (CTextBoxCtrl*)pDlgControl->GetControlWnd();

	pCtrl->SetInputFilter( sFilter.IsEmpty()? NULL : new CCustomFilter( sFilter ) );
	acedRetT();
	return RSRSLT;
}

ADSRESULT TextBox::GetLineCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CTextBoxCtrl* pCtrl = (CTextBoxCtrl*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetLineCount() );
	return RSRSLT;
}

ADSRESULT TextBox::IsModified()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CTextBoxCtrl* pCtrl = (CTextBoxCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->GetModify() )
		acedRetT();
	return RSRSLT;
}

ADSRESULT TextBox::GetSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CEdit* pCtrl = (CEdit*)pDlgControl->GetControlWnd();

	DWORD dwSel = pCtrl->GetSel();
	if( dwSel != CB_ERR )
	{
		resbuf rbEnd = { NULL, RTSHORT };
		rbEnd.resval.rint = LOWORD(dwSel);
		resbuf rbStart = { &rbEnd, RTSHORT };
		rbStart.resval.rint = HIWORD(dwSel);
		acedRetList( &rbStart );
	}
	return RSRSLT;
}

ADSRESULT TextBox::GetLine()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	int nLine = 0;
	if( !GetIntArgument( pArgs, nLine ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CEdit* pCtrl = (CEdit*)pDlgControl->GetControlWnd();

	int cchLine = pCtrl->LineLength( pCtrl->LineIndex( nLine ) ) + 1;
	CString sText;
	sText.ReleaseBuffer( pCtrl->GetLine( nLine, sText.GetBuffer( cchLine ), cchLine ) ); 
	acedRetStr( sText );
	return RSRSLT;
}

ADSRESULT TextBox::GetFirstVisibleLine()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CEdit* pCtrl = (CEdit*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetFirstVisibleLine() );
	return RSRSLT;
}

ADSRESULT TextBox::GetLineLength()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	int nLine = 0;
	if( !GetIntArgument( pArgs, nLine ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CEdit* pCtrl = (CEdit*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->LineLength( pCtrl->LineIndex( nLine ) ) );
	return RSRSLT;
}

ADSRESULT TextBox::LineScroll()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	int nLine = 0;
	if( !GetIntArgument( pArgs, nLine ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CEdit* pCtrl = (CEdit*)pDlgControl->GetControlWnd();

	pCtrl->LineScroll( nLine, 0 );
	acedRetT();
	return RSRSLT;
}

ADSRESULT TextBox::ReplaceSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	CString sText;
	if( !GetStringArgument( pArgs, sText ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CEdit* pCtrl = (CEdit*)pDlgControl->GetControlWnd();

	pCtrl->ReplaceSel( sText, TRUE );
	acedRetT();
	return RSRSLT;
}

ADSRESULT TextBox::SetSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	int nStart = 0;
	if( !GetIntArgument( pArgs, nStart ) )
		return RSERR; //invalid input

	int nEnd = 0;
	if( !GetIntArgument( pArgs, nEnd ) )
		return RSERR; //invalid input

	bool bNoScroll = false;
	GetBoolArgument( pArgs, bNoScroll, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CEdit* pCtrl = (CEdit*)pDlgControl->GetControlWnd();

	pCtrl->SetSel( nStart, nEnd, bNoScroll? TRUE : FALSE );
	if( GetFocus() != pCtrl->m_hWnd )
		pCtrl->SetFocus();
	acedRetT();
	return RSRSLT;
}

ADSRESULT TextBox::SetTabStops()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	int nUnits = 0;
	if( !GetIntArgument( pArgs, nUnits ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CEdit* pCtrl = (CEdit*)pDlgControl->GetControlWnd();

	CDC* pdc = pCtrl->GetDC();
	TEXTMETRIC tm;
	pdc->GetTextMetrics( &tm );
	pCtrl->ReleaseDC( pdc );
	if( pCtrl->SetTabStops( nUnits * tm.tmAveCharWidth ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT TextBox::Undo()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlTextBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CEdit* pCtrl = (CEdit*)pDlgControl->GetControlWnd();

	if( pCtrl->Undo() )
		acedRetT();
	return RSRSLT;
}
