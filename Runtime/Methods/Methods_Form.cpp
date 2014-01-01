// Methods_Form.cpp : implementation file
//

#include "stdafx.h"
#include "ArgumentsRetrieval.h"
#include "Methods_Form.h"
#include "ArxWorkspace.h"
#include "ArxDialogControl.h"
#include "CustomFileDlg.h"



ADSRESULT Form::Center()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogObject* pDlgObject = NULL;
	if( !GetDialogArgument( pArgs, pDlgObject ) )
		return RSERR; //arguments expected

	//optional arguments
	int nX = -1;
	int nY = -1;
	if( GetIntArgument( pArgs, nX, true ) )
	{
		if( !GetIntArgument( pArgs, nY ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDlgObject )
		return RSRSLT; //dialog not found

	if( nX <= 0 || nY <= 0 )
	{
		if( pDlgObject->CenterDialog() )
			acedRetT();
	}
	else
	{
		if( pDlgObject->CenterAndResizeDialog( nX, nY ) )
			acedRetT();
	}

	return (RSRSLT) ;
}

ADSRESULT Form::Close()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogObject* pDlgObject = NULL;
	if( !GetDialogArgument( pArgs, pDlgObject ) )
		return RSERR; //arguments expected

	//optional arguments
	int nStatus = 0;
	GetIntArgument( pArgs, nStatus, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDlgObject )
		return RSRSLT; //dialog not found

	CPoint ptXlate( 0, 0 );
	pDlgObject->GetControlWnd()->ClientToScreen( &ptXlate );
	resbuf rbPosition = { NULL, RTPOINT };
	rbPosition.resval.rpoint[X] = ptXlate.x;
	rbPosition.resval.rpoint[Y] = ptXlate.y;
	acedRetVal( &rbPosition );

	pDlgObject->CloseDialog( nStatus );

	return (RSRSLT) ;
}

ADSRESULT Form::CloseAll()
{
	struct resbuf *pArgs =acedGetArgs () ;

	//optional arguments
	UINT fMask = -1;
	GetUIntArgument( pArgs, fMask, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	theArxWorkspace.CloseAllDialogs( fMask );

	return (RSRSLT) ;
}

ADSRESULT Form::Enable()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogObject* pDlgObject = NULL;
	if( !GetDialogArgument( pArgs, pDlgObject ) )
		return RSERR; //arguments expected

	bool bEnable = true;
	GetBoolArgument( pArgs, bEnable, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDlgObject )
		return RSRSLT; //dialog not found

	if( pDlgObject->GetTopLevelWnd()->EnableWindow( bEnable? TRUE : FALSE ) )
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Form::GetControlArea()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclFormPtr pDclForm;
	if( !GetFormArgument( pArgs, pDclForm ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDclForm )
		return RSRSLT; //dialog not found

	TDclControlPtr pPropObj = pDclForm->GetControlProperties();
	assert( pPropObj != NULL );

	resbuf rbHeight = {NULL, RTSHORT};
	rbHeight.resval.rint = pPropObj->GetLongProperty( Prop::Height );
	resbuf rbWidth = {&rbHeight, RTSHORT};
	rbWidth.resval.rint = pPropObj->GetLongProperty( Prop::Width );
	acedRetList(&rbWidth);		

	return (RSRSLT) ;
}

ADSRESULT Form::GetControls()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclFormPtr pDclForm;
	if( !GetFormArgument( pArgs, pDclForm ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDclForm )
		return RSRSLT; //form not found

	TDclFormList Forms;
	pDclForm->GetProject()->FindChildForms( pDclForm, Forms );
	Forms.push_front( pDclForm );
	resbuf* prbControls = NULL;
	resbuf* prbTail = NULL;
	for( TDclFormList::const_iterator iterForm = Forms.begin(); iterForm != Forms.end(); ++iterForm )
	{
		const TDclControlList& Controls = (*iterForm)->GetControlList();
		for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
		{
			if( (*iter)->GetType() == _CtlForm )
				continue;
			resbuf* prbControl = acutNewRb( RTENAME );
			prbControl->resval.rlname[0] = (LONG_PTR)(const CDclControlObject*)(*iter);
			prbControl->resval.rlname[1] = odcl::ptrDclControl;
			if( prbTail )
				prbTail->rbnext = prbControl;
			else
				prbControls = prbControl;
			prbTail = prbControl;
		}
	}
	acedRetList( prbControls );
	acutRelRb( prbControls );

	return (RSRSLT) ;
}

ADSRESULT Form::GetUUID()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclFormPtr pDclForm;
	if( !GetFormArgument( pArgs, pDclForm ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDclForm )
		return RSRSLT; //form not found

	acedRetStr( pDclForm->GetUUIDAsString() );

	return (RSRSLT) ;
}

ADSRESULT Form::Hide()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogObject* pDlgObject = NULL;
	if( !GetDialogArgument( pArgs, pDlgObject ) )
		return RSERR; //arguments expected

	//optional arguments
	bool bHide = true;
	GetBoolArgument( pArgs, bHide, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDlgObject )
		return RSRSLT; //dialog not found

	if( pDlgObject->Show( !bHide ) )
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Form::IsActive()
{
	struct resbuf *pArgs =acedGetArgs () ;

	if( GetNilArgument( pArgs, true ) )
		return RSRSLT; //accept a NIL argument and return quietly

	TDclFormPtr pDclForm;
	if( !GetFormArgument( pArgs, pDclForm ) )
		return RSERR; //arguments expected

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDclForm )
		return RSRSLT; //dialog not found

	if( pDclForm->GetFormInstance() )
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Form::IsApplyEnabled()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogObject* pDlgObject = NULL;
	if( !GetDialogArgument( pArgs, pDlgObject ) )
		return RSERR; //arguments expected

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDlgObject )
		return RSRSLT; //dialog not found

	if( pDlgObject->IsDirty() )
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Form::IsEnabled()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogObject* pDlgObject = NULL;
	if( !GetDialogArgument( pArgs, pDlgObject ) )
		return RSERR; //arguments expected

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDlgObject )
		return RSRSLT; //dialog not found

	if( pDlgObject->GetTopLevelWnd()->IsWindowEnabled() )
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Form::IsFloating()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogObject* pDlgObject = NULL;
	if( !GetDialogArgument( pArgs, pDlgObject ) )
		return RSERR; //arguments expected

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDlgObject )
		return RSRSLT; //dialog not found

	if( pDlgObject->IsFloating() )
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Form::IsVisible()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogObject* pDlgObject = NULL;
	if( !GetDialogArgument( pArgs, pDlgObject ) )
		return RSERR; //arguments expected

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDlgObject )
		return RSRSLT; //dialog not found

	if( pDlgObject->GetTopLevelWnd()->IsWindowVisible() )
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Form::RecalcLayout()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogObject* pDlgObject = NULL;
	if( !GetDialogArgument( pArgs, pDlgObject ) )
		return RSERR; //arguments expected

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDlgObject )
		return RSRSLT; //dialog not found

	pDlgObject->GetControlPane()->RecalcLayout();
	acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Form::Resize()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogObject* pDlgObject = NULL;
	if( !GetDialogArgument( pArgs, pDlgObject ) )
		return RSERR; //arguments expected

	int nNewWidth = -1;
	if( !GetIntArgument( pArgs, nNewWidth ) )
		return RSERR; //invalid input

	int nNewHeight = -1;
	if( !GetIntArgument( pArgs, nNewHeight ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDlgObject )
		return RSRSLT; //dialog not found

	if( pDlgObject->ResizeDialog( nNewWidth, nNewHeight ) )
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Form::SetApplyEnabled()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogObject* pDlgObject = NULL;
	if( !GetDialogArgument( pArgs, pDlgObject ) )
		return RSERR; //arguments expected

	bool bDirty = true;
	GetBoolArgument( pArgs, bDirty, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDlgObject )
		return RSRSLT; //dialog not found

	if( pDlgObject->SetDirty( bDirty ) )
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Form::SetMinMaxSizes()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl;
	if( !GetControlArgument( pArgs, pControl ) )
		return RSERR; //invalid input

	int nMinWidth = 0;
	if( !GetIntArgument( pArgs, nMinWidth ) )
		return RSERR; //invalid input

	int nMinHeight = 0;
	if( !GetIntArgument( pArgs, nMinHeight ) )
		return RSERR; //invalid input

	int nMaxWidth = 0;
	if( !GetIntArgument( pArgs, nMaxWidth ) )
		return RSERR; //invalid input

	int nMaxHeight = 0;
	if( !GetIntArgument( pArgs, nMaxHeight ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	bool bFailed = false;
	if( !pControl->SetLongProperty( Prop::MinDialogWidth, nMinWidth ) )
		bFailed = true;
	if( !pControl->SetLongProperty( Prop::MinDialogHeight, nMinHeight ) )
		bFailed = true;
	if( !pControl->SetLongProperty( Prop::MaxDialogWidth, nMaxWidth ) )
		bFailed = true;
	if( !pControl->SetLongProperty( Prop::MaxDialogHeight, nMaxHeight ) )
		bFailed = true;

	CDialogControl* pDlgControl = pControl->GetControlInstance();
	if( pDlgControl )
		pDlgControl->ApplyProperty( pControl->GetPropertyObject( Prop::MaxDialogHeight ) );

	if( !bFailed )
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Form::StartTimer()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogObject* pDlgObject = NULL;
	if( !GetDialogArgument( pArgs, pDlgObject ) )
		return RSERR; //arguments expected

	long lMilliseconds = -1;
	GetLongArgument( pArgs, lMilliseconds, true );
	if( lMilliseconds < 0 )
		lMilliseconds = -1;

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( pDlgObject->StartTimer( (UINT)lMilliseconds ) )
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Form::Show()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclFormPtr pDclForm;
	if( !GetFormArgument( pArgs, pDclForm ) )
		return RSERR; //invalid input

	//optional arguments
	int nX = -1;
	int nY = -1;
	CString sDefaultDirectory;
	CString sDefaultFileName;
	CString sDefaultExtension;
	bool bHasFileParams = (pDclForm->GetType() == FrmFileDlg);
	if( bHasFileParams )
	{
		if( GetStringArgument( pArgs, sDefaultDirectory, true ) )
		{
			if( GetStringArgument( pArgs, sDefaultFileName, true ) )
				GetStringArgument( pArgs, sDefaultExtension, true );
		}
	}
	else if( GetIntArgument( pArgs, nX, true ) )
	{
		if( !GetIntArgument( pArgs, nY ) )
			return RSERR; //invalid input
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDclForm )
		return RSRSLT; //form not found

	// call method to display the requested form
	FileDialogParams fdp( TRUE, NULL, NULL, 0, NULL );
	if( bHasFileParams )
	{
		CString sFilename = sDefaultDirectory;
		if( !sFilename.IsEmpty() && !sFilename.Right(1).SpanExcluding( _T("\\/") ).IsEmpty() )
			sFilename += _T('\\');
		sFilename += sDefaultFileName;
		fdp.sFilename = sFilename;
		fdp.sDefaultExtension = sDefaultExtension;
	}
	DialogParams params( CPoint( nX, nY ), CRect(0,0,0,0), bHasFileParams? (LPARAM)&fdp : NULL );
	int nResult = theArxWorkspace.ActivateDclForm(pDclForm, &params);

	if (nResult >= 0)
	{
		if (nResult == IDOK && pDclForm->GetType() == FrmFileDlg)
		{
			if( (fdp.dwFlags & OFN_ALLOWMULTISELECT) )
			{
				// Convert the array to a list that can be returned
				struct resbuf* prbRetList = NULL;
				struct resbuf* prbTail = NULL;

				INT_PTR nCount = fdp.rsFilenames.GetSize();
				for (INT_PTR i = 0; i < nCount; i++)
				{
					// get the text name of the selected line number
					CString sFileName = fdp.rsFilenames.GetAt(i);
					if( prbTail )
					{
						prbTail->rbnext = acutNewRb(RTSTR);
						prbTail = prbTail->rbnext;
					}
					else
					{
						prbRetList = acutNewRb(RTSTR);
						prbTail = prbRetList;
					}
					acutNewString(sFileName, prbTail->resval.rstring);
				}

				acedRetList(prbRetList);
				acutRelRb(prbRetList);
			}
			else
				acedRetStr( fdp.sFilename );
		}
		else
			acedRetInt(nResult);
	}

	return (RSRSLT) ;
}
