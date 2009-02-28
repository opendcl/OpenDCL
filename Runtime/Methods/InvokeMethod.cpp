// InvokeMethod.cpp : implementation file
//

#include "stdafx.h"
#include "InvokeMethod.h"
#include "ArgumentsRetrieval.h"
#include "acutmem.h"

extern bool IsOnlyModalForm();

CString EscapeLispStringArgument( LPCTSTR pszString )
{
	CString sResult = pszString;
	sResult.Replace( _T("\\"), _T("\\\\") );
	sResult.Replace( _T("\""), _T("\\\"") );
	return sResult;
}

Acad::ErrorStatus ExecuteCommand( LPCTSTR pszCommand, bool bShowCommand = false, AcApDocument* pDoc = NULL )
{
	if( !pDoc )
		pDoc = acDocManager->curDocument();
	if( !pDoc )
		return Acad::eNoDocument;
	//CWnd* CmdBarWnd = acedGetAcadDockCmdLine();
	//CmdBarWnd->SetFocus();
	CString sCmd = pszCommand;
	if( sCmd.GetLength() > 256 )
	{
		//see if we can shorten it by replacing the first string with "..."
		int cchStart = sCmd.Find( _T('"') );
		if( cchStart > 0 )
		{
			int cchEnd = sCmd.Find( _T('"'), cchStart + 1 );
			if( cchEnd > cchStart )
				sCmd = sCmd.Left( cchStart + 1 ) + _T("<...>") + sCmd.Mid( cchEnd );
		}
		if( sCmd.GetLength() > 256 ) //still too long?
			return Acad::eStringTooLong;
	}
	Acad::ErrorStatus es = acDocManager->sendStringToExecute( pDoc, sCmd, false, true, bShowCommand );
	if( es == Acad::eNoDocument && !acDocManager->isApplicationContext() )
	{
		if( RTNORM == ads_queueexpr( sCmd.LockBuffer() ) )
			es = Acad::eOk;
	}
	return es;
}

int acedInvokeNoDocStateSafe(const struct resbuf *args, struct resbuf **result)
{
	AcApDocument* pDoc = acDocManager->curDocument();
	//assert( pDoc != NULL );
	if( !pDoc )
		return RTERROR;
	try
	{ //if the invoked function closes the current document, an exception will occur before acedInvoke returns
		int nResult = acedInvoke( args, result );
		assert( nResult == RTNORM );
	}
	catch( ... )
	{}
	return RTERROR;
}


CString FireCancel( LPCTSTR pszLispFunction )
{
	CString sLispFunction = pszLispFunction;
	
	if( sLispFunction.Left( 4 ).CompareNoCase( _T("^C^C") ) != 0 )
		return sLispFunction;

	Acad::ErrorStatus es = ExecuteCommand( _T("\x1B\x1B") );
	return sLispFunction.Mid( 4 );
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
		stat = acedInvokeNoDocStateSafe(list, &result); 
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
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString = EscapeLispStringArgument( sString );
			CString sInt;
			sInt.Format( _T("%d"), nInt );
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("("))
				+ sLispFunction
				+ _T(" \"")
				+ sString 
				+ _T("\" ") 
				+ sInt
				+ _T(") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 			    
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodStringLong(CString sLispFunction, CString sString, DWORD_PTR lLong, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString = EscapeLispStringArgument( sString );
			ads_real adsValue = lLong;
			TCHAR fmtval[26]; 
			int stat = acdbRToS(adsValue, 2,0, fmtval); 
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("("))
				+ sLispFunction
				+ _T(" \"")
				+ sString 
				+ _T("\" ") 
				+ fmtval
				+ _T(") ");
			
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    
			ads_name lVal;
			lVal[0] = lLong;
			lVal[1] = 0;

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSTR, sString,
				RTENAME, lVal,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 			    
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodLong(CString sLispFunction, DWORD_PTR lLong, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			ads_real adsValue = lLong;
			TCHAR fmtval[26]; 
			int stat = acdbRToS(adsValue, 2,0, fmtval); 
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("("))
				+ sLispFunction
				+ _T(" ")
				+ fmtval
				+ _T(") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    
			ads_name lVal;
			lVal[0] = lLong;
			lVal[1] = 0;

			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTENAME, lVal,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 			    
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodStringIntInt(CString sLispFunction, CString sString, int nInt1, int nInt2, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString = EscapeLispStringArgument( sString );
			CString sInt1;
			sInt1.Format( _T("%d"), nInt1 );
			CString sInt2;
			sInt2.Format( _T("%d"), nInt2 );
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("("))
				+ sLispFunction
				+ _T(" \"")
				+ sString 
				+ _T("\" ") 
				+ sInt1
				+ _T(" ") 
				+ sInt2
				+ _T(") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 			    
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethod(CString sLispFunction, bool UseSendString, AcApDocument* pDoc)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString || (sLispFunction.Left(1) == _T("'")))
		{
			bool bShowCommand = false;

			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("(")) + sLispFunction + _T(") ");

			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");
			else if (sLispFunction.Left(1) == _T("'"))
			{
				sCommand = sLispFunction + _T(" ");
				bShowCommand = true;
			}
			
			Acad::ErrorStatus es = ExecuteCommand(sCommand, bShowCommand, pDoc);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    

			list = acutBuildList(RTSTR, FireCancel(sLispFunction), 0);
			if (list != NULL) 
			{ 
				stat = acedInvokeNoDocStateSafe(list, &result); 
				acutRelRb(list); 
				if(result != NULL) 
					acutRelRb(result); 
			}
		}
	}
}


void InvokeMethodIntString(CString sLispFunction, int nInt, CString sString, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString = EscapeLispStringArgument( sString );
			CString sInt;
			sInt.Format( _T("%d"), nInt );
			
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("(")) + sLispFunction + _T(" ") + sInt + _T(" \"") + sString + _T("\") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodIntList(CString sLispFunction, int nInt, CStringList *pList, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			CString sInt;
			sInt.Format( _T("%d"), nInt );
			
			CString sCommand = CString(_T("(")) + FireCancel(sLispFunction) + _T(" ") + sInt + _T(" \"");
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			for (int i=0; i<pList->GetCount(); i++)
			{
				POSITION pos = pList->FindIndex(i);				
				CString sItem = EscapeLispStringArgument( pList->GetAt(pos) );
				sCommand = sCommand + sItem + _T(" ");
			}
			sCommand = sCommand + _T("\") ");
			
			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(rbpTail, &result); 
			    acutRelRb(rbpTail); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodString(CString sLispFunction, CString sString, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		//AfxMessageBox(sLispFunction);
		if (UseSendString)
		{
			sString = EscapeLispStringArgument( sString );
			CString sCommand = CString(_T("(")) + FireCancel(sLispFunction) + _T(" \"") + sString + _T("\") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}



void InvokeMethodInt(CString sLispFunction, int nInt, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			CString sInt;
			sInt.Format( _T("%d"), nInt );
			
			sLispFunction = FireCancel(sLispFunction);
			CString sCommand = CString(_T("(")) + sLispFunction + _T(" ") + sInt + _T(") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}
void InvokeMethodIntInt(CString sLispFunction, int nInt1, int nInt2, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			CString sInt1;
			sInt1.Format( _T("%d"), nInt1 );
			CString sInt2;
			sInt2.Format( _T("%d"), nInt2 );
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("(")) + sLispFunction + _T(" ") + sInt1 + _T(" ") + sInt2 +_T(") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);

			

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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 

				
				if(result != NULL)
					acutRelRb(result); 
		
			} 
		}
	}
}

void InvokeMethodIntIntInt(CString sLispFunction, int nInt1, int nInt2, int nInt3, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			CString sInt1;
			sInt1.Format( _T("%d"), nInt1 );
			CString sInt2;
			sInt2.Format( _T("%d"), nInt2 );
			CString sInt3;
			sInt3.Format( _T("%d"), nInt3 );
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("(")) + sLispFunction + _T(" ") + sInt1 + _T(" ") + sInt2 + _T(" ") + sInt3 + _T(") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list);
				if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethodIntIntIntInt(CString sLispFunction, int nInt1, int nInt2, int nInt3, int nInt4, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			CString sInt1;
			sInt1.Format( _T("%d"), nInt1 );
			CString sInt2;
			sInt2.Format( _T("%d"), nInt2 );
			CString sInt3;
			sInt3.Format( _T("%d"), nInt3 );
			CString sInt4;
			sInt4.Format( _T("%d"), nInt4 );
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("(")) + sLispFunction + _T(" ") + sInt1 + _T(" ") + sInt2 + _T(" ") + sInt3 + _T(" ") + sInt4 + _T(") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}


void InvokeMethodStringString(CString sLispFunction, CString sString1, CString sString2, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString1 = EscapeLispStringArgument( sString1 );
			sString2 = EscapeLispStringArgument( sString2 );

			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("(")) + sLispFunction + _T(" \"") + sString1 + _T("\" \"") + sString2 + _T("\") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethod3Strings(CString sLispFunction, CString sString1, CString sString2, CString sString3, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString1 = EscapeLispStringArgument( sString1 );
			sString2 = EscapeLispStringArgument( sString2 );
			sString3 = EscapeLispStringArgument( sString3 );

			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("(")) + sLispFunction + _T(" \"") + sString1 + _T("\" \"") + sString2 + _T("\" \"") +  sString3 + _T("\") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}

void InvokeMethod4Strings(CString sLispFunction, CString sString1, CString sString2, CString sString3, CString sString4, bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString1 = EscapeLispStringArgument( sString1 );
			sString2 = EscapeLispStringArgument( sString2 );
			sString3 = EscapeLispStringArgument( sString3 );
			sString4 = EscapeLispStringArgument( sString4 );

			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("(")) + sLispFunction + _T(" \"") + sString1 + _T("\" \"") + sString2 + _T("\" \"") +  sString3 + _T("\" \"") + sString4 + _T("\") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
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
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			sString1 = EscapeLispStringArgument( sString1 );
			sString2 = EscapeLispStringArgument( sString2 );
			sString3 = EscapeLispStringArgument( sString3 );
			
			sLispFunction = FireCancel(sLispFunction);
			CString sCommand;
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");
			else
				sCommand.Format( _T("(%s \"%s\" \"%s\" \"%s\" '(%d %d))"), (LPCTSTR)sLispFunction,
																																	 (LPCTSTR)sString1,
																																	 (LPCTSTR)sString2,
																																	 (LPCTSTR)sString3,
																																	 ptPoint.x,
																																	 ptPoint.y );
			ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
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
					DWORD_PTR	lValue,
					bool	UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		
		if (UseSendString)
		{
			sString1 = EscapeLispStringArgument( sString1 );
			sString2 = EscapeLispStringArgument( sString2 );
			sString3 = EscapeLispStringArgument( sString3 );

			CString sInt;
			sInt.Format( _T("%d"), lValue );
			
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("(")) 
				+ sLispFunction 
				+ _T(" \"") + sString1 
				+ _T("\" \"") + sString2 
				+ _T("\" \"") + sString3 + _T("\" ")
				+ sInt 
				+ _T(") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
		}
		else
		{
			// this code is for all other dialogs
			int stat;
			struct resbuf *result = NULL, *list;    
			ads_name lVal;
			lVal[0] = lValue;
			lVal[1] = 0;
			
			list = acutBuildList(
				RTSTR, FireCancel(sLispFunction), 
				RTSTR, sString1,
				RTSTR, sString2,
				RTSTR, sString3,
				RTENAME, lVal,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvokeNoDocStateSafe(list, &result); 
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
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if (sLispFunction.GetLength() > 0)
	{
		if (UseSendString)
		{
			CString sInt1;
			sInt1.Format( _T("%d"), ptPoint.x );
			CString sInt2;
			sInt2.Format( _T("%d"), ptPoint.y );
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand;
			sCommand.Format( _T("(%s '(%d %d)) "), (LPCTSTR)sLispFunction, ptPoint.x, ptPoint.y );
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}


void InvokeMethodPoint3DInt(
					CString sLispFunction, 
					acedDwgPoint ptPoint,
					int nViewport,
					bool UseSendString)
{
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
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

			CString sVP;
			sVP.Format( _T("%d"), nViewport );
			
			// this code if for the dockable dialog only
			sLispFunction = FireCancel(sLispFunction);			
			CString sCommand = CString(_T("(")) 
				+ sLispFunction
				+ _T(" (list ")
				+ sX + _T(" ")
				+ sY + _T(" ")
				+ sZ + _T(") ")
				+ sVP +
				_T(") ");
			
			if (sLispFunction.Left(2).CompareNoCase( _T("C:") ) != 0)
				sCommand = sLispFunction + _T(" ");

			Acad::ErrorStatus es = ExecuteCommand(sCommand);
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
				RTSHORT, nViewport,
				RTNONE);

			if (list != NULL) { 
		        stat = acedInvokeNoDocStateSafe(list, &result); 
			    acutRelRb(list); 
			    if(result != NULL) acutRelRb(result); 
			} 
		}
	}
}
