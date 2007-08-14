// ArgumentsRetrieval.h : header file
//

#pragma once

#include "DialogObject.h"
#include "AxArg.h"

class CDclControlObject;
class CPropertyObject;


inline void acedRetPointer(void* ptr)
{
	struct resbuf RetVal = {NULL,RTENAME};
	RetVal.resval.rlname[0] = (LONG_PTR)ptr;
	RetVal.resval.rlname[1] = 0;
	acedRetVal(&RetVal);
}


inline void acedRetHandle(DWORD_PTR hdl)
{
	struct resbuf RetVal = {NULL,RTENAME};
	RetVal.resval.rlname[0] = (LONG_PTR)hdl;
	RetVal.resval.rlname[1] = 0;
	acedRetVal(&RetVal);
}


inline void acedRetLong(LONG lValue)
{
	struct resbuf RetVal = {NULL,RTLONG};
	RetVal.resval.rlong = lValue;
	acedRetVal(&RetVal);
}


struct resbuf *getLispTargetInput(LPCTSTR sMethod, CDclControlObject *&pArg);
void GetLispRealInput(struct resbuf *ListData, double &dArg1);
void GetLispBoolInput(struct resbuf *ListData, bool &bArg1);

CDclControlObject * GetLispInput(LPCTSTR sMethod, CPoint &Arg1);
CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, CString &sArg2);
CDclControlObject * GetLispInput(LPCTSTR sMethod, CString &sArg1, CString &sArg2);
CDclControlObject * GetLispInput(LPCTSTR sMethod, CString &sArg1);
CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2, int &nArg3);
CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2);
CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1);
CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2, CString &sArg3);
CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2, int &nArg3, int &nArg4);
CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2, int &nArg3, int &nArg4, int &nArg5);
CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2, int &nArg3, int &nArg4, int &nArg5, CString &sOptionalString);
CDclControlObject * GetLispInput(LPCTSTR sMethod, CString &sArg1, int &nArg2, int &nArg3, int &nArg4, int &nArg5);
CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, int &nArg2, CArray<int, int> *pIntArray, CStringArray *pStrings);
CDclControlObject * GetLispInput(LPCTSTR sMethod, int &nArg1, long &lArg2);
CDclControlObject * GetLispInput(LPCTSTR sMethod, int nArg1, CString &sArg2, CString &sArg3);

struct resbuf *GetProjectAndFormName(CString *pArg1, CString *pArg2, CString sMethod);
bool GetNextString(struct resbuf *ListData, CString *pArg, int nIndex, CString sMethod);
bool GetStringArgument(int nIndex, CString *pArg, CString sMethod);
bool GetIntArgument(int nIndex, int *pArg, CString sMethod);
bool GetDoubleArgument(int nIndex, double *pArg, CString sMethod);
bool GetDateArgument(int nIndex, COleDateTime *pArg, CString sMethod);
RefCountedPtr< CPropertyObject > GetPropertyArgument(int nIndex, CString sMethod);
CDclFormObject * FindDclObject(CDclControlObject *pControl);

bool FindOptionalStringArgument(int nIndex, CString *pArg, CString sMethod);
bool FindOptionalIntArgument(int nIndex, int *pArg, CString sMethod);
bool FindOptionalDoubleArgument(int nIndex, double *pArg, CString sMethod);

CWnd * GetControlPointer(int nControlType, CString sMethod, int *pnArgs = NULL);
CDclControlObject * GetControlArxObject(CString sMethod, int *pnArgs = NULL);
bool GetControlInfo(int nControlType, CString sMethod, CWnd *pParent, CWnd *pControl);
bool IsArgumentString(short restype, int index, CString sMethod);
bool IsArgumentInt(short restype, int index, CString sMethod);
bool IsArgumentDouble(short restype, int index, CString sMethod);

// activeX argument retrieval method
bool GetAxPropertyArgument(struct resbuf*& ListData, COleVariant *oleVar, const AxArg& type );
	
bool GetStringOrHandleArgument(int nIndex, CString& sArg, DWORD_PTR& hdl, LPCTSTR pszMethod);

CWnd * GetArgsControlInt(int nControlType, CString sMethod, int &nInt);
CWnd * GetArgsControlDouble(int nControlType, CString sMethod, double &dDouble);
CWnd * GetArgsControlString(int nControlType, CString sMethod, CString &sString);
CWnd * GetArgsControlIntString(int nControlType, CString sMethod, int &nInt, CString &sString);
CWnd * GetArgsControlStringString(int nControlType, CString sMethod, CString &sString, CString &sString2);
CWnd * GetArgsControlIntInt(int nControlType, CString sMethod, int &nInt, int &nInt2);
CWnd * GetArgsControlIntIntInt(int nControlType, CString sMethod, int &nInt, int &nInt2, int &nInt3);

//Functions to pull out arguments passed in from LISP.
//If returns false, the argument passed is not of that type.
//If returns true, argument is returned in parameter and args is advanced to
//the next resbuf in the list.
//This allows you to easily chain these together. If the return is false you fail,
//otherwise you keep calling it and the functions keep advancing the resbuf chain for you.
bool isPairArgument(struct resbuf* args);
void skipArgument(struct resbuf* &args);
bool getListBeginArgument(struct resbuf* &args);
bool getListEndArgument(struct resbuf* &args);
bool getDoubleArgument(struct resbuf* &args, double &d);
bool getIntArgument(struct resbuf* &args, int &i);
bool getEnameArgument(struct resbuf* &args, ads_name &en);
bool getSSArgument(struct resbuf* &args, ads_name &ss);
bool getObjectIdArgument(struct resbuf* &args, AcDbObjectId &id);
bool getStringArgument(struct resbuf* &args, CString &str);
bool getCharStrArgument(struct resbuf* &args, char* str);
bool getStringIntPairArgument(struct resbuf* &args, CString &str, int &i);
bool getBoolArgument(struct resbuf* &args, bool& b);
bool getPointArgument(struct resbuf* &args, AcGePoint3d &p);
bool get2DPointArgument(struct resbuf* &args, AcGePoint2d &p);
bool getOLEArgument(struct resbuf* &args, COleVariant &ole);