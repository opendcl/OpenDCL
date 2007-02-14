#pragma once


// Struct for holding information about ActiveX events so we do not
// have to keep going back to type info
class AxEventDescriptor : public CObject
{
public:
	static const int MAX_CALLING_ARGUMENTS = 16;
  DISPID Id;
  CString Name;
  CString DocumentationDesc;	
  VARTYPE CallingArgs[MAX_CALLING_ARGUMENTS];
  CStringArray CallingArgNames;
  CLSID   CallingArgClsids[MAX_CALLING_ARGUMENTS];
  int nArgs;
  CString Params;

  // constructor
  AxEventDescriptor(void);
  virtual ~AxEventDescriptor(void);

protected:
	DECLARE_SERIAL(AxEventDescriptor)

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toString() const;
#endif
};
