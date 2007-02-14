#pragma once


struct AxPropertyEnum
{
	CString Name;
	VARIANT Var;
};


// Struct for holding information about ActiveX properties so we do not
// have to keep goint back to typeinfo
class AxPropertyDescriptor : public CObject
{
	static const int MAX_CALLING_ARGUMENTS = 16;
public:
	DISPID Id;
	CString Name;
	CString DocumentationDesc;
	VARTYPE Type;
	BOOL IsArray;
	BOOL CanSet;
	GUID Guid;
	int NumEnum;
	int NumParams;
	INVOKEKIND invKind;
	AxPropertyEnum* ArrEnum;
	
	VARTYPE CallingArgs[MAX_CALLING_ARGUMENTS];
	CString CallingArgNames[MAX_CALLING_ARGUMENTS+1];
	CLSID   CallingArgClsids[MAX_CALLING_ARGUMENTS];

public:
	AxPropertyDescriptor(void);
	AxPropertyDescriptor(const AxPropertyDescriptor *other);
	virtual ~AxPropertyDescriptor(void);

protected:
	DECLARE_SERIAL(AxPropertyDescriptor)

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toString() const;
#endif
};
