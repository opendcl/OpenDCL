// Methods_Month.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Month.h"
#include "ArgumentsRetrieval.h"
#include "ControlTypes.h"


static void ReturnDate( const COleDateTime& Date )
{
	resbuf rbDay = { NULL, RTSHORT };
	rbDay.resval.rint = Date.GetDay();
	resbuf rbMonth = { &rbDay, RTSHORT };
	rbMonth.resval.rint = Date.GetMonth();
	resbuf rbYear = { &rbMonth, RTSHORT };
	rbYear.resval.rint = Date.GetYear();
	acedRetList( &rbYear );
}


ADSRESULT Month::SetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlMonth))
		return RSERR; //invalid input

	COleDateTime dtDate;
	if( !GetDateArgument( pArgs, dtDate ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CMonthCalCtrl* pCtrl = (CMonthCalCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->SetCurSel( dtDate ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Month::GetCurSel()
{	
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlMonth))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CMonthCalCtrl* pCtrl = (CMonthCalCtrl*)pDlgControl->GetControlWnd();
	COleDateTime dtDate;
	if( pCtrl->GetCurSel( dtDate ) )
		ReturnDate( dtDate );
	return RSRSLT;
}

ADSRESULT Month::SetRange()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlMonth))
		return RSERR; //invalid input

	COleDateTime dtStart;
	if( !GetDateArgument( pArgs, dtStart ) )
		return RSERR; //invalid input

	COleDateTime dtEnd;
	if( !GetDateArgument( pArgs, dtEnd ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CMonthCalCtrl* pCtrl = (CMonthCalCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->SetRange( &dtStart, &dtEnd ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Month::GetRangeStart()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlMonth))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CMonthCalCtrl* pCtrl = (CMonthCalCtrl*)pDlgControl->GetControlWnd();
	COleDateTime dtStart;
	COleDateTime dtEnd;
	if( (GDTR_MIN & pCtrl->GetRange( &dtStart, &dtEnd )) != 0 )
		ReturnDate( dtStart );
	return RSRSLT;
}

ADSRESULT Month::GetRangeEnd()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlMonth))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CMonthCalCtrl* pCtrl = (CMonthCalCtrl*)pDlgControl->GetControlWnd();
	COleDateTime dtStart;
	COleDateTime dtEnd;
	if( (GDTR_MAX & pCtrl->GetRange( &dtStart, &dtEnd )) != 0 )
		ReturnDate( dtEnd );
	return RSRSLT;
}

ADSRESULT Month::GetMonthRangeStart()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlMonth))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CMonthCalCtrl* pCtrl = (CMonthCalCtrl*)pDlgControl->GetControlWnd();
	COleDateTime dtStart;
	COleDateTime dtEnd;
	pCtrl->GetMonthRange( dtStart, dtEnd, GMR_DAYSTATE );
	ReturnDate( dtStart );
	return RSRSLT;
}

ADSRESULT Month::GetMonthRangeEnd()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlMonth))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CMonthCalCtrl* pCtrl = (CMonthCalCtrl*)pDlgControl->GetControlWnd();
	COleDateTime dtStart;
	COleDateTime dtEnd;
	pCtrl->GetMonthRange( dtStart, dtEnd, GMR_DAYSTATE );
	ReturnDate( dtEnd );
	return RSRSLT;
}

ADSRESULT Month::SetSelRange()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlMonth))
		return RSERR; //invalid input

	COleDateTime dtStart;
	if( !GetDateArgument( pArgs, dtStart ) )
		return RSERR; //invalid input

	COleDateTime dtEnd;
	if( !GetDateArgument( pArgs, dtEnd ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CMonthCalCtrl* pCtrl = (CMonthCalCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->SetSelRange( dtStart, dtEnd ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Month::GetSelRangeStart()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlMonth))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CMonthCalCtrl* pCtrl = (CMonthCalCtrl*)pDlgControl->GetControlWnd();
	COleDateTime dtStart;
	COleDateTime dtEnd;
	if( pCtrl->GetSelRange( dtStart, dtEnd ) )
		ReturnDate( dtStart );
	return RSRSLT;
}

ADSRESULT Month::GetSelRangeEnd()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlMonth))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CMonthCalCtrl* pCtrl = (CMonthCalCtrl*)pDlgControl->GetControlWnd();
	COleDateTime dtStart;
	COleDateTime dtEnd;
	if( pCtrl->GetSelRange( dtStart, dtEnd ) )
		ReturnDate( dtEnd );
	return RSRSLT;
}

ADSRESULT Month::GetToday()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlMonth))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CMonthCalCtrl* pCtrl = (CMonthCalCtrl*)pDlgControl->GetControlWnd();
	COleDateTime dtToday;
	if( pCtrl->GetToday( dtToday ) )
		ReturnDate( dtToday );
	return RSRSLT;
}
