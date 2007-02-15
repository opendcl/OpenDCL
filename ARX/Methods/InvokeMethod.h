// InvokeMethod.h : header file
//

#pragma once


CString FireCancel(CString sLispFunction);
bool InvokeCancelMethod(CString sLispFunction, bool bUserPressedEsc);
void InvokeMethod(CString sLispFunction, bool UseSendString, AcApDocument* pDoc = NULL);
void InvokeMethodIntString(CString sLispFunction, int nInt, CString sString, bool UseSendString);
void InvokeMethodIntList(CString sLispFunction, int nInt, CStringList *pList, bool UseSendString);
void InvokeMethodInt(CString sLispFunction, int nInt, bool UseSendString);
void InvokeMethodIntInt(CString sLispFunction, int nInt1, int nInt2, bool UseSendString);
void InvokeMethodIntIntInt(CString sLispFunction, int nInt1, int nInt2, int nInt3, bool UseSendString);
void InvokeMethodIntIntIntInt(CString sLispFunction, int nInt1, int nInt2, int nInt3, int nInt4, bool UseSendString);
void InvokeMethodLong(CString sLispFunction, long lLong, bool UseSendString);
void InvokeMethodStringLong(CString sLispFunction, CString sString, long lLong, bool UseSendString);
void InvokeMethodStringString(CString sLispFunction, CString sString1, CString sString2, bool UseSendString);
void InvokeMethodString(CString sLispFunction, CString sString, bool UseSendString);
void InvokeMethodStringInt(CString sLispFunction, CString sString, int nInt, bool UseSendString);
void InvokeMethodStringIntInt(CString sLispFunction, CString sString, int nInt1, int nInt2, bool UseSendString);
void InvokeMethod3StringsPoint(
					CString sLispFunction, 
					CString sString1, 
					CString sString2, 
					CString sString3, 
					CPoint	ptPoint,
					bool UseSendString);
void InvokeMethod3StringsLong(
					CString sLispFunction, 
					CString sString1, 
					CString sString2, 
					CString sString3, 
					long	lValue,
					bool UseSendString);
void InvokeMethod3Strings(CString sLispFunction, CString sString1, CString sString2, CString sString3, CString sString4, bool UseSendString);
void InvokeMethod4Strings(CString sLispFunction, CString sString1, CString sString2, CString sString3, CString sString4, bool UseSendString);

void InvokeMethodPoint(
					CString sLispFunction, 
					CPoint	ptPoint,
					bool UseSendString);
void InvokeMethodPoint3D(
					CString sLispFunction, 
					acedDwgPoint ptPoint,
					bool UseSendString);
