// DialogControl.h : header file
//

#pragma once

class CDclControlObject;


//this class manages the relationship between a control definition and an instance of the control
class CDialogControl
{
	// Attributes
protected:		
	CDclControlObject* mpTemplate;
	RefCountedPtr< CWnd > mpControl;

public:
	CDialogControl( CDclControlObject* pTemplate, RefCountedPtr< CWnd > pControl );
	virtual ~CDialogControl();

	// Properties
public:
	RefCountedPtr< CWnd > GetWindow() const { return mpControl; }
	CDclControlObject* GetDclControl() const { return mpTemplate; }
	long GetDclControlType() const;

	// Name rendition
public:
	CString GetKeyName() const;
	CString GetKeyPath() const;

	// Implementation
public:
	virtual void CreateGlobalVariables() const = 0;
};
