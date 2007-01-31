// DialogControl.h : header file
//

#pragma once

class CDclControlObject;


class CDialogControl
{
	// Attributes
protected:		
	CDclControlObject* mpControl;
	CWnd* mpWnd;

public:
	CDialogControl( CDclControlObject* pControl, CWnd* pWnd );
	virtual ~CDialogControl();

	// Properties
public:
	CWnd* GetWindow() const { return mpWnd; }
	CDclControlObject* GetDclControl() const { return mpControl; }
	long GetDclControlType() const;
	
	// Name rendition
public:
	CString GetKeyName() const;
	CString GetKeyPath() const;
};
