#pragma once

#include "RefCountedPtr.h"

class AxPropertyDescriptor;
class AxEventDescriptor;
class AxMethodDescriptor;
class CAxContainer;
enum IOStatus;

enum PropertyTypes
{
	PropInvalid = -1,
	PropLong = 1,
	PropString = 2,
	PropDouble = 3,
	PropBool = 4,
	PropEnum = 5,
	PropEvent = 6,
	PropPicture = 7,
	PropCustom = 8,
	PropImageList = 9,
	PropOLEColor = 10,
	PropStringArray = 11,
	PropIntArray = 12,
	PropActiveXPropPages = 13,
	PropActiveXProp = 14,
	PropActiveXEnum = 15,
	PropActiveXEvent = 16,
	PropActiveXRunTime = 17,
	PropActiveXMethods = 18,
	PropStringArrayList = 19,
	PropIntArrayList = 20,
};


/////////////////////////////////////////////////////////////////////////////
// CPropertyObject

class CPropertyObject : public CObject
{
protected:
	PropertyTypes mType;
	bool mbHidden;
	short mnID;
	CString msStringValue;
	OLE_COLOR mOLEColor;
	long mlLong;
	bool mbBoolean;
	double mdblDouble;

public:
	CStringList		 m_stringList;
	CList<CStringArray*> m_stringArrayList;
	CList<CArray<int, int>*>	 m_intArrayList;
	CArray<int, int> m_intList;
	short			 m_ImageListIndex;

	//ActiveX Attributes
public:
	AxPropertyDescriptor *m_pAxProp;
	AxPropertyDescriptor *m_pAxPropGet;
	AxPropertyDescriptor *m_pAxPropPut;
	AxPropertyDescriptor *m_pAxPropPutRef;
	AxEventDescriptor    *m_pAxEvent;
	CList<AxMethodDescriptor*> *m_pMethods;

	//Construction
public:
	CPropertyObject();
	CPropertyObject(PropertyTypes type);
	virtual ~CPropertyObject();

	//Attributes
public:
	PropertyTypes GetType() const { return mType; }
	void SetType( PropertyTypes type ) { mType = type; }
	bool IsHidden() const { return mbHidden; }
	void SetHidden( bool bHidden = true ) { mbHidden = bHidden; }
	short GetID() const { return mnID; }
	void SetID( short nID ) { mnID = nID; }
	const CString& GetStringValue() const;
	void SetStringValue( LPCTSTR pszValue );
	const OLE_COLOR& GetOLEColorValue() const;
	void SetOLEColorValue( const OLE_COLOR& dwColor );
	const long& GetLongValue() const;
	void SetLongValue( long lValue );
	const long& GetEnumValue() const { return GetLongValue(); }
	void SetEnumValue( int nValue ) { SetLongValue( (long)nValue ); }
	bool GetBooleanValue() const;
	void SetBooleanValue( bool bValue );
	double GetDoubleValue() const;
	void SetDoubleValue( double dblValue );

	//2007-01-30 [ORW]: save version set to 5 (no change from ObjectDCL 3)
	ULONG GetCurrentSaveVersion() const { return 5; }

	//Operations
public:

	
	//Implementation
public:
	GUID GetActiveXProperyGuid();
	DISPID GetActiveXGetProperyId();
	DISPID GetActiveXSetProperyId();
	int GetActiveXParamQty();
	void SetActiveXPropery(CAxContainer *axContainer, CString sNewValue);
	void SetActiveXProperyName(CString sName);	
	CString GetActiveXPropery(CAxContainer *axContainer);
	VARTYPE GetActiveXProperyType();
	bool GetActiveXEnum(/*CListBoxDlg *pListBox = NULL*/);
	CString GetActiveXEnumDesc(CString sValue);
	CString GetActiveXEnumValue(int nEnumIndex);
	void DoActiveXFontPropDlg(CAxContainer *axContainer);
	CString GetAxMethodDesc(int nIndex);
	int GetAxMethodParams(int nIndex);
	GUID GetAxMethodParamGUID(int nIndex, int nParam);
	CString GetAxMethodParamName(int nIndex, int nParam);
	CString GetAxMethodParamVarType(int nIndex, int nParam);
	VARTYPE GetAxMethodReturnType(int nIndex);
	AxMethodDescriptor *GetAxMethod(int nIndex);
	
	CString GetName();
	CString GetDocumentationDesc();
	void Copy(CPropertyObject *other);
	void AddProperty(PropertyTypes ValueType, CString strValue);
	CString GetStdProperty();
	
	void AddStringItem(CString NewString);
	void ClearList();
	INT_PTR CountList();
	CString GetStringItem(short ListIndex);

protected:

public:
	void SetProperty(CString strValue);

	//File I/O
	virtual void Serialize(CArchive& ar);
	virtual void SerializeActiveXInfo(CArchive& ar, int nPropertyVersion);
	virtual void SerializeActiveXProp(CArchive& ar, AxPropertyDescriptor *axProp, int nPropertyVersion);	
  IOStatus WriteToTextFile(FILE* pFile) const;
  IOStatus WriteActiveXInfoToTextFile(FILE* pFile) const;
  IOStatus WriteActiveXPropToTextFile(FILE* pFile, AxPropertyDescriptor *axProp) const;
  IOStatus ReadFromTextFile(std::ifstream &sFile);
  IOStatus ReadFromTextFile5(std::ifstream &sFile);
  IOStatus ReadActiveXInfoFromTextFile5(std::ifstream &sFile);
  IOStatus ReadActiveXPropFromTextFile5(std::ifstream &sFile, AxPropertyDescriptor *axProp);

protected:
	DECLARE_SERIAL(CPropertyObject)
};
