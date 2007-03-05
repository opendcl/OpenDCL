// InvokeMethod.cpp : implementation file
//

#include "stdafx.h"
#include "InvokeMethod.h"
#include "ArgumentsRetrieval.h"
#include "acutmem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool IsOnlyModalForm();
const TCHAR sQuote[] = _T("\"");
const TCHAR sProperQuote[] = _T("\\\"");

const int nCancelCheckLength = 6;
const int nCancelRemoveLength = 4;
const TCHAR sCancelIndicator[] = _T("^C^CC:");
const TCHAR sCColan[] = _T("C:");
const TCHAR sBracket[] = _T("(");
const TCHAR sQuoteSpace[] = _T("\" ");
const TCHAR sSpaceQuote[] = _T(" \"");
const TCHAR sQuoteSpaceQuote[] = _T("\" \"");
const TCHAR sAddSpace[] = _T(" ");
const TCHAR sComma[] = _T(",");
const TCHAR sSingleQuote[] = _T("'");
const TCHAR sQuoteEndBracket[] = _T("\") ");
const TCHAR sEndBracket[] = _T(") ");
const TCHAR sDoubleEndBracket[] = _T(")) ");
const TCHAR sCancelStr[] = _T("\x1B\x1B");
const TCHAR sNewLine[] = _T("\n");
const TCHAR sList[] = _T(" (list ");


Acad::ErrorStatus ExecuteCommandInCurrentDoc( LPCTSTR pszCommand, bool bShowCommand = false )
{
	AcApDocument* pDoc = acDocManager->curDocument();
	assert( pDoc != NULL );
	if( !pDoc )
		return Acad::eNoDocument;
	//CWnd* CmdBarWnd = acedGetAcadDockCmdLine();
	//CmdBarWnd->SetFocus();		
	return acDocManager->sendStringToExecute(pDoc, pszCommand, false, true, bShowCommand);
}


CString FireCancel(CString sLispFunction)
{
	sLispFunction.MakeUpper();
	
	//int nTest = _tcsicmp(sLispFunction.Left(2), sCColan);
	//!???
	if (_tcsicmp(sLispFunction.Left(2), sCColan) > -1)
		return sLispFunction;

	//nTest = _tcsicmp(sLispFunction.Left(nCancelCheckLength), sCancelIndicator);

	if (_tcsicmp(sLispFunction.Left(nCancelCheckLength), sCancelIndicator) != 0)
		return sLispFunction;

	CString CancelStr = (sCancelStr);

	Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(CancelStr);
	return sLispFunction.Mid(nCancelRemoveLength);
}

bool InvokeCancelMethod(CString sLispFunction, bool bUserPressedEsc)
{
	if (sLispFunction.GetLength() == 0)
		return false;
	
	// this code is for all other dialogs
	int stat;
	struct resbuf *result = NULL, *list;    

	list = acutBuildList(
		RTSTR, FireCancel(sLispFunction), 
		RTSHORT, bUserPressedEsc,
		RTNONE);

	if (list != NULL) { 
		stat = acedInvoke(list, &result); 
		acutRelRb(list); 
		if(result != NULL)
		{
			if (result->restype == RTSHORT)
			{
				if (result->resval.rint == 1);
				{
					acutRelRb(result); 
					return true;
				}
			}
			if (result->restype == RTT)
			{
				acutRelRb(result); 
				return true;				
			}
			acutRelRb(result); 
		}
	} 

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// InvokeMethod
void InvokeMethodStringInt(CString sLispFunction, CString sString, int nInt, bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString.Replace(sQuote, sProperQuote);
			char sInt[80];
			_ltoa(nInt, sInt, 10);
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket)
				+ sLispFunction
				+ sSpaceQuote
				+ sString 
				+ sQuoteSpace 
				+ sInt
				+ sEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSTR, sString,
				RTSHORT, nInt,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 			    
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodStringLong(CString sLispFunction, CString sString, long lLong, bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString.Replace(sQuote, sProperQuote);
			ads_real adsValue = lLong;
			TCHAR fmtval[26]; 
			int stat = acdbRToS(adsValue, 2,0, fmtval); 
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket)
				+ sLispFunction
				+ sSpaceQuote
				+ sString 
				+ sQuoteSpace 
				+ fmtval
				+ sEndBracket;
			
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSTR, sString,
				RTLONG, lLong,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 			    
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodLong(CString sLispFunction, long lLong, bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			ads_real adsValue = lLong;
			TCHAR fmtval[26]; 
			int stat = acdbRToS(adsValue, 2,0, fmtval); 
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket)
				+ sLispFunction
				+ sAddSpace
				+ fmtval
				+ sEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTLONG, lLong,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 			    
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodStringIntInt(CString sLispFunction, CString sString, int nInt1, int nInt2, bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString.Replace(sQuote, sProperQuote);
			char sInt1[80];
			_ltoa(nInt1, sInt1, 10);
			char sInt2[80];
			_ltoa(nInt2, sInt2, 10);
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket)
				+ sLispFunction
				+ sSpaceQuote
				+ sString 
				+ sQuoteSpace 
				+ sInt1
				+ sAddSpace 
				+ sInt2
				+ sEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSTR, sString,
				RTSHORT, nInt1,
				RTSHORT, nInt2,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 			    
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethod(CString sLispFunction, bool UseSendString, AcApDocument* pDoc)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString || (sLispFunction.Left(1) == sSingleQuote))
		{
			bool bShowCommand = false;

			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket) + sLispFunction + sEndBracket;

			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;
			
			if (sLispFunction.Left(1) == sSingleQuote)
			{
				sCommand = sLispFunction + sAddSpace;
				bShowCommand = true;
			}
			
			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand, bShowCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(RTSTR, FireCancel(sLispFunction), 0);
			if (list != NULL) 
			{ 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) 
				{
					acutRelRb(result); 
				}
			}
		}
	}
}


void InvokeMethodIntString(CString sLispFunction, int nInt, CString sString, bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString.Replace(sQuote, sProperQuote);
		
			// this code if for the dockable dialog only
			char sInt[80];
			_ltoa(nInt, sInt, 10);
			
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket) + sLispFunction + sAddSpace + sInt + sSpaceQuote + sString + sQuoteEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSHORT, nInt,
				RTSTR, sString,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodIntList(CString sLispFunction, int nInt, CStringList *pList, bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			// this code if for the dockable dialog only
			char sInt[80];
			_ltoa(nInt, sInt, 10);
			
			CString sCommand = CString(sBracket) + FireCancel(sLispFunction) + sAddSpace + sInt + sSpaceQuote;
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			for (int i=0; i<pList->GetCount(); i++)
			{
				POSITION pos = pList->FindIndex(i);				
				CString sItem = pList->GetAt(pos);
				sItem.Replace(sQuote, sProperQuote);
				sCommand = sCommand + sItem + sAddSpace;
			}
			sCommand = sCommand + sQuoteEndBracket;
			
			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			//struct resbuf *result = NULL, *list;    

			// Convert the array to a list that can be returned
			struct resbuf* rbpRetList = acutNewRb(RTSTR);
			struct resbuf* rbpTail = rbpRetList;

			acutNewString(FireCancel(sLispFunction), rbpTail->resval.rstring);
				
			rbpTail->rbnext = acutNewRb(RTSHORT);
			rbpTail = rbpTail->rbnext;
			
			rbpTail->resval.rint = nInt;
				
			rbpTail->rbnext = acutNewRb(RTSTR);
			rbpTail = rbpTail->rbnext;
			
			for (int i=0; i<pList->GetCount(); i++)
			{				
				POSITION pos = pList->FindIndex(i);				
				CString sItem = pList->GetAt(pos);
				
				acutNewString(sItem, rbpTail->resval.rstring);
				
				if ((i+1) < pList->GetCount())
				{
					rbpTail->rbnext = acutNewRb(RTSTR);
					rbpTail = rbpTail->rbnext;
				}
				
			}
			
			struct resbuf *result = NULL;
			if (rbpTail != NULL) { 
		        stat = acedInvoke(rbpTail, &result); 
			    acutRelRb(rbpTail); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodString(CString sLispFunction, CString sString, bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		//AfxMessageBox(sLispFunction);
		if (UseSendString)
		{
			sString.Replace(sQuote, sProperQuote);
			// this code if for the dockable dialog only
			CString sCommand = CString(sBracket) + FireCancel(sLispFunction) + sSpaceQuote + sString + sQuoteEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSTR, sString,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}



void InvokeMethodInt(CString sLispFunction, int nInt, bool UseSendString)
{
	//acedAlert("InvokeMethodInt 1");

	if (sLispFunction.GetLength() > 0)
	{
	//	acedAlert("InvokeMethodInt 2");
	//	acedAlert(sLispFunction);
		if (UseSendString)
		{
			// this code if for the dockable dialog only
			char sInt[80];
			_ltoa(nInt, sInt, 10);
			
			sLispFunction = FireCancel(sLispFunction);
			CString sCommand = CString(sBracket) + sLispFunction + sAddSpace + sInt + sEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSHORT, nInt,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}
void InvokeMethodIntInt(CString sLispFunction, int nInt1, int nInt2, bool UseSendString)
{
	//acedAlert("InvokeMethodInt 1");

	if (sLispFunction.GetLength() > 0)
	{
	//	acedAlert("InvokeMethodInt 2");
	//	acedAlert(sLispFunction);

		if (UseSendString)
		{
			char sInt1[80];
			_ltoa(nInt1, sInt1, 10);
			char sInt2[80];
			_ltoa(nInt2, sInt2, 10);
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket) + sLispFunction + sAddSpace + sInt1 + sAddSpace + sInt2 +sEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);

			

		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSHORT, nInt1,
				RTSHORT, nInt2,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 

				
				if(result != NULL)
					acutRelRb(result); 
		
			} 
		}
	}
}

void InvokeMethodIntIntInt(CString sLispFunction, int nInt1, int nInt2, int nInt3, bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			char sInt1[80];
			_ltoa(nInt1, sInt1, 10);
			char sInt2[80];
			_ltoa(nInt2, sInt2, 10);
			char sInt3[80];
			_ltoa(nInt3, sInt3, 10);
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket) + sLispFunction + sAddSpace + sInt1 + sAddSpace + sInt2 + sAddSpace + sInt3 + sEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSHORT, nInt1,
				RTSHORT, nInt2,
				RTSHORT, nInt3,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list);
				if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodIntIntIntInt(CString sLispFunction, int nInt1, int nInt2, int nInt3, int nInt4, bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			char sInt1[80];
			_ltoa(nInt1, sInt1, 10);
			char sInt2[80];
			_ltoa(nInt2, sInt2, 10);
			char sInt3[80];
			_ltoa(nInt3, sInt3, 10);
			char sInt4[80];
			_ltoa(nInt4, sInt4, 10);
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket) + sLispFunction + sAddSpace + sInt1 + sAddSpace + sInt2 + sAddSpace + sInt3 + sAddSpace + sInt4 + sEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSHORT, nInt1,
				RTSHORT, nInt2,
				RTSHORT, nInt3,
				RTSHORT, nInt4,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}


void InvokeMethodStringString(CString sLispFunction, CString sString1, CString sString2, bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString1.Replace(sQuote, sProperQuote);
			sString2.Replace(sQuote, sProperQuote);
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket) + sLispFunction + sSpaceQuote + sString1 + sQuoteSpaceQuote + sString2 + sQuoteEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSTR, sString1,
				RTSTR, sString2,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethod3Strings(CString sLispFunction, CString sString1, CString sString2, CString sString3, bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString1.Replace(sQuote, sProperQuote);
			sString2.Replace(sQuote, sProperQuote);
			sString3.Replace(sQuote, sProperQuote);
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket) + sLispFunction + sSpaceQuote + sString1 + sQuoteSpaceQuote + sString2 + sQuoteSpaceQuote +  sString3 + sQuoteEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSTR, sString1,
				RTSTR, sString2,
				RTSTR, sString3,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethod4Strings(CString sLispFunction, CString sString1, CString sString2, CString sString3, CString sString4, bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString1.Replace(sQuote, sProperQuote);
			sString2.Replace(sQuote, sProperQuote);
			sString3.Replace(sQuote, sProperQuote);
			sString4.Replace(sQuote, sProperQuote);
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket) + sLispFunction + sSpaceQuote + sString1 + sQuoteSpaceQuote + sString2 + sQuoteSpaceQuote +  sString3 + sQuoteSpaceQuote + sString4 + sQuoteEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSTR, sString1,
				RTSTR, sString2,
				RTSTR, sString3,
				RTSTR, sString4,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}


void InvokeMethod3StringsPoint(
					CString sLispFunction, 
					CString sString1, 
					CString sString2, 
					CString sString3, 
					CPoint	ptPoint,
					bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		
		if (UseSendString)
		{
			sString1.Replace(sQuote, sProperQuote);
			sString2.Replace(sQuote, sProperQuote);
			sString3.Replace(sQuote, sProperQuote);
			char sInt1[80];
			_ltoa(ptPoint.x, sInt1, 10);
			char sInt2[80];
			_ltoa(ptPoint.y, sInt2, 10);
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket) 
				+ sLispFunction
				+ sSpaceQuote + sString1 
				+ sQuoteSpaceQuote + sString2 
				+ sQuoteSpaceQuote + sString3 + sQuoteSpace
				+ sInt1 + sComma
				+ sInt2 + sEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			ads_point pt;
			pt[0] = ptPoint.x;
			pt[1] = ptPoint.y;
			pt[2] = 0.0;

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSTR, sString1,
				RTSTR, sString2,
				RTSTR, sString3,
				RTPOINT, pt,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethod3StringsLong(
					CString sLispFunction, 
					CString sString1, 
					CString sString2, 
					CString sString3, 
					long	lValue,
					bool	UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		
		if (UseSendString)
		{
			sString1.Replace(sQuote, sProperQuote);
			sString2.Replace(sQuote, sProperQuote);
			sString3.Replace(sQuote, sProperQuote);
			char sInt[80];
			_ltoa(lValue, sInt, 10);
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket) 
				+ sLispFunction 
				+ sSpaceQuote + sString1 
				+ sQuoteSpaceQuote + sString2 
				+ sQuoteSpaceQuote + sString3 + sQuoteSpace
				+ sInt 
				+ sEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			
			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSTR, sString1,
				RTSTR, sString2,
				RTSTR, sString3,
				RTLONG, lValue,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodPoint(
					CString sLispFunction, 
					CPoint	ptPoint,
					bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			char sInt1[80];
			_ltoa(ptPoint.x, sInt1, 10);
			char sInt2[80];
			_ltoa(ptPoint.y, sInt2, 10);
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket) 
				+ sLispFunction
				+ sAddSpace
				+ sInt1 + sComma
				+ sInt2 + sEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			ads_point pt;
			pt[0] = ptPoint.x;
			pt[1] = ptPoint.y;
			pt[2] = 0.0;

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTPOINT, pt,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}


void InvokeMethodPoint3D(
					CString sLispFunction, 
					acedDwgPoint ptPoint,
					bool UseSendString)
{
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			TCHAR sX[80];
			TCHAR sY[80];
			TCHAR sZ[80];
			acdbRToS(
				ptPoint[X],
				2,
				8,
				sX);
			
			acdbRToS(
				ptPoint[Y],
				2,
				8,
				sY);
			
			acdbRToS(
				ptPoint[Z],
				2,
				8,
				sZ);
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(sBracket) 
				+ sLispFunction
				+ sList
				+ sX + sAddSpace
				+ sY + sAddSpace
				+ sZ + sDoubleEndBracket;
			
			if (_tcsicmp(sLispFunction.Left(2), sCColan) != 0)
				sCommand = sLispFunction + sAddSpace;

			Acad::ErrorStatus es = ExecuteCommandInCurrentDoc(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			ads_point pt;
			pt[0] = ptPoint[0];
			pt[1] = ptPoint[1];
			pt[2] = ptPoint[2];

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RT3DPOINT, pt,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvoke(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}
CString RtoS(double dValue)
{
	CString sReturn;
	ads_real adsValue = dValue;
	TCHAR fmtval[26]; 
	int stat = acdbRToS(adsValue, 2,8, fmtval); 
	sReturn = fmtval;
	if (stat == RTNORM)
		return sReturn;
	else
		return _T("0.0");
}