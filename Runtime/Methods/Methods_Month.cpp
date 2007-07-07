// Methods_Month.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Month.h"
#include "MethodLexicon.h"
#include "ArgumentsRetrieval.h"
#include "ControlTypes.h"


void ReturnDate(COleDateTime *pDate)
{
	
	// this code is for all other dialogs
	int stat;
	struct resbuf *list;    

	list = acutBuildList(
		RTSHORT, pDate->GetYear(),
		RTSHORT, pDate->GetMonth(),
		RTSHORT, pDate->GetDay(),
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 

}

int Month_SetCurSel()
{
	COleDateTime dtDate;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlMonth, sMonth_SetCurSel, &nArg);

	if (!GetDateArgument(nArg, &dtDate, sMonth_SetCurSel) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	
	((CMonthCalCtrl*)pControl)->SetCurSel(dtDate);
	
	return 0;
}
int Month_GetCurSel()
{	
	CWnd *pControl = GetControlPointer(CtlMonth, sMonth_GetCurSel);

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}

	SYSTEMTIME sysTime;

	BOOL bRet = ((CMonthCalCtrl*)pControl)->GetCurSel(&sysTime);

	sysTime.wHour = sysTime.wMinute = sysTime.wSecond =
	sysTime.wMilliseconds = 0;

	COleDateTime dtDate(sysTime);

	if (bRet)
		ReturnDate(&dtDate);
	else
		acedRetNil();

	return 0;
}
int Month_SetRange()
{
	COleDateTime dtStartDate;
	COleDateTime dtEndDate;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlMonth, sMonth_SetRange, &nArg);

	if (!GetDateArgument(nArg, &dtStartDate, sMonth_SetRange) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	nArg++;
	if (!GetDateArgument(nArg, &dtEndDate, sMonth_SetRange) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	((CMonthCalCtrl*)pControl)->SetRange(&dtStartDate, &dtEndDate);
	
	acedRetNil();
	return 0;
}
int Month_GetRangeStart()
{
	COleDateTime dtStartDate;
	COleDateTime dtEndDate;
	
	CWnd *pControl = GetControlPointer(CtlMonth, sMonth_GetRangeStart);

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	
	((CMonthCalCtrl*)pControl)->GetRange(&dtStartDate, &dtEndDate);
	
	ReturnDate(&dtStartDate);
	return 0;

}

int Month_GetRangeEnd()
{
	COleDateTime dtStartDate;
	COleDateTime dtEndDate;
	
	CWnd *pControl = GetControlPointer(CtlMonth, sMonth_GetRangeEnd);

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}	
	
	((CMonthCalCtrl*)pControl)->GetRange(&dtStartDate, &dtEndDate);
	
	ReturnDate(&dtEndDate);
	return 0;

}
int Month_GetMonthRangeStart()
{
	COleDateTime dtStartDate;
	COleDateTime dtEndDate;
	
	CWnd *pControl = GetControlPointer(CtlMonth, sMonth_GetMonthRangeStart);

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}	
	
	((CMonthCalCtrl*)pControl)->GetMonthRange(dtStartDate, dtEndDate, GMR_DAYSTATE);
	
	ReturnDate(&dtStartDate);
	return 0;

}
int Month_GetMonthRangeEnd()
{
	COleDateTime dtStartDate;
	COleDateTime dtEndDate;
	
	CWnd *pControl = GetControlPointer(CtlMonth, sMonth_GetMonthRangeEnd);

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}	
	
	((CMonthCalCtrl*)pControl)->GetMonthRange(dtStartDate, dtEndDate, GMR_DAYSTATE);
	
	ReturnDate(&dtEndDate);
	return 0;

}
int Month_SetSelRange()
{
	COleDateTime dtStartDate;
	COleDateTime dtEndDate;
	int nArg;
	CWnd *pControl = GetControlPointer(CtlMonth, sMonth_SetSelRange, &nArg);

	if (!GetDateArgument(nArg, &dtStartDate, sMonth_SetSelRange) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	nArg++;
	if (!GetDateArgument(nArg, &dtEndDate, sMonth_SetSelRange) || pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	((CMonthCalCtrl*)pControl)->SetSelRange(dtStartDate, dtEndDate);
	
	acedRetNil();
	return 0;
}
int Month_GetSelRangeStart()
{
	COleDateTime dtStartDate;
	COleDateTime dtEndDate;
	
	CWnd *pControl = GetControlPointer(CtlMonth, sMonth_GetSelRangeStart);

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	
	((CMonthCalCtrl*)pControl)->SetSelRange(dtStartDate, dtEndDate);
	
	ReturnDate(&dtStartDate);
	return 0;
}
int Month_GetSelRangeEnd()
{
	COleDateTime dtStartDate;
	COleDateTime dtEndDate;
	
	CWnd *pControl = GetControlPointer(CtlMonth, sMonth_GetSelRangeEnd);

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	
	
	((CMonthCalCtrl*)pControl)->SetSelRange(dtStartDate, dtEndDate);
	
	ReturnDate(&dtEndDate);
	return 0;
}

int Month_GetToday()
{
	CWnd *pControl = GetControlPointer(CtlMonth, sMonth_GetToday);

	if (pControl == NULL)
	{
		
		acedRetInt(-1);
		return 0;
	}
	COleDateTime dtDate;
	
	((CMonthCalCtrl*)pControl)->GetToday(dtDate);
	
	ReturnDate(&dtDate);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// Methods_Month
