// Methods_Edit.cpp : implementation file
//


#include "stdafx.h"
#include "Methods_Edit.h"
#include "DclControlObject.h"
#include "ArgumentsRetrieval.h"
#include "MethodLexicon.h"
#include "ErrorLexicon.h"
#include "EditCtrl.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "Workspace.h"


int TextBox_SetFilter()
{
	theWorkspace.DisplayAlert(_T("OpenDCL function (dcl_TextBox_SetFilter) is not yet implemented."));
/*
	int nArg=0;
	CDclControlObject *pArxObject = GetControlArxObject(sTextBox_SetFilter, &nArg);
	
	if (pArxObject == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	if (pArxObject->GetLongProperty(Prop::FilterStyle) > 0 &&
		pArxObject->GetLongProperty(Prop::FilterStyle) < 8)
	{
		theWorkspace.DisplayAlert(ErrorWrongFilterStyle);
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CWnd *pControl = pArxObject->GetWindow();

	CString sFilter;
	if (!GetStringArgument(nArg, &sFilter, sTextBox_SetFilter) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	((CEditCtrl*)pControl)->m_sFilter = sFilter;

	acedRetVoid();
	return 0;
*/
	acedRetNil();
	return 0;
}

int TextBox_GetFilter()
{
	theWorkspace.DisplayAlert(_T("OpenDCL function (dcl_TextBox_GetFilter) is not yet implemented."));
/*
	CDclControlObject *pArxObject = GetControlArxObject(sTextBox_SetFilter);
	if (pArxObject->GetLongProperty(Prop::FilterStyle) > 0)
	{
		theWorkspace.DisplayAlert(ErrorWrongFilterStyle);
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	CWnd *pControl = GetControlPointer(CtlTextBox, sTextBox_GetFilter);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	acedRetStr(((CEditCtrl*)pControl)->m_sFilter);
	return 0;
*/
	acedRetNil();
	return 0;
}

int TextBox_GetLineCount()
{
	CWnd *pControl = GetControlPointer(CtlTextBox, sTextBox_GetLineCount);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	
	acedRetInt(((CEdit*)pControl)->GetLineCount());
	return 0;

}
int TextBox_GetModify()
{
	CWnd *pControl = GetControlPointer(CtlTextBox, sTextBox_GetModify);

	if (pControl == NULL)
	{
		// return nil
		acedRetNil();
		return 0;
	}
	
	if (((CEdit*)pControl)->GetModify())
		acedRetT();
	else
		acedRetNil();

	return 0;

}
int TextBox_GetSel()
{
	CWnd *pControl = GetControlPointer(CtlTextBox, sTextBox_GetSel);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	int nStart;
	int nEnd;
	((CEdit*)pControl)->GetSel(nStart, nEnd);

	// this code is for all other dialogs
	int stat;
	struct resbuf *list;    

	list = acutBuildList(
		RTSHORT, nStart,
		RTSHORT, nEnd,
		RTNONE);

	if (list != NULL) { 	    
		stat = acedRetList(list);		
		acutRelRb(list); 
	} 

	return 0;
}
int TextBox_GetLine()
{
	int nLine;
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlTextBox, sTextBox_GetLine, &nArg);

	if (!GetIntArgument(nArg, &nLine, sTextBox_GetLine) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	CString sText;			
	int cb;         
	TCHAR *pch; 
	pch = sText.GetBuffer(_MAX_PATH); 
	
	cb = ((CEdit*)pControl)->GetLine(nLine, pch, _MAX_PATH); 
	pch[cb] = _T('\0'); 
	sText.ReleaseBuffer(); 
	

	acedRetStr(sText);
	return 0;

}
int TextBox_GetFirstVisibleLine()
{
	CWnd *pControl = GetControlPointer(CtlTextBox, sTextBox_GetFirstVisibleLine);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	
	acedRetInt(((CEdit*)pControl)->GetFirstVisibleLine());
	return 0;

}
int TextBox_GetLineLength()
{
	int nLine;
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlTextBox, sTextBox_GetLineLength, &nArg);

	if (!GetIntArgument(nArg, &nLine, sTextBox_GetLineLength) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
		
	
	acedRetInt(((CEdit*)pControl)->LineLength(nLine));
	return 0;
}
int TextBox_LineScroll()
{
	int nLine;
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlTextBox, sTextBox_LineScroll, &nArg);

	if (!GetIntArgument(nArg, &nLine, sTextBox_LineScroll) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
		
	((CEdit*)pControl)->LineScroll(nLine, 0);
	acedRetVoid();
	return 0;
}
int TextBox_ReplaceSel()
{
	CString sReplacementText;
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlTextBox, sTextBox_ReplaceSel, &nArg);

	if (!GetStringArgument(nArg, &sReplacementText, sTextBox_ReplaceSel) || pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	
	((CEdit*)pControl)->ReplaceSel(sReplacementText, TRUE);
	
	// return nil
	acedRetVoid();
	return 0;
}
int TextBox_SetSel()
{
	int nStart;
	int nEnd;

	CWnd *pControl = GetArgsControlIntInt(CtlTextBox, sTextBox_SetSel, nStart, nEnd);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}
	
	((CEdit*)pControl)->SetSel(nStart,nEnd, TRUE);
	((CEdit*)pControl)->SetFocus();
	
	// return nil
	acedRetVoid();
	return 0;
}
int TextBox_SetTabStops()
{
	int nUnits;
	int nArg=0;
	CWnd *pControl = GetControlPointer(CtlTextBox, sTextBox_SetTabStops, &nArg);

	if (!GetIntArgument(nArg, &nUnits, sTextBox_SetTabStops) || pControl == NULL)
	{
		acedRetInt(-1);
		return 0;
	}
	
	CDC *pdc = pControl->GetDC();
	TEXTMETRIC tm;
	pdc->GetTextMetrics(&tm);
	pControl->ReleaseDC(pdc);

	((CEdit*)pControl)->SetTabStops(nUnits * tm.tmAveCharWidth);

	// return nil
	acedRetVoid();
	return 0;
}
int TextBox_Undo()
{
	CWnd *pControl = GetControlPointer(CtlTextBox, sTextBox_Undo);

	if (pControl == NULL)
	{
		// return nil
		acedRetInt(-1);
		return 0;
	}

	if (((CEdit*)pControl)->CanUndo())
		((CEdit*)pControl)->Undo();

	// return nil
	acedRetVoid();
	return 0;
}
