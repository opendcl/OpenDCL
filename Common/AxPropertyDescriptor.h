#pragma once


struct AxPropertyEnum
{
	CString Name;
	VARIANT Var;
};

struct AxPropertyArg
{
	VARTYPE vt;
	CString name;
	CLSID clsid;
	AxPropertyArg() : vt( VT_EMPTY ) {}
};


// Struct for holding information about ActiveX properties so we do not
// have to keep goint back to typeinfo
class AxPropertyDescriptor : public CObject
{
public:
	DISPID Id;
	CString Name;
	CString DocumentationDesc;
	VARTYPE Type;
	BOOL IsArray;
	BOOL CanSet;
	GUID Guid;
	INVOKEKIND invKind;
	std::vector< AxPropertyEnum > rEnum;
	std::vector< AxPropertyArg > rArgs;

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
