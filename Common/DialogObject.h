#pragma once

#include "ControlPane.h"

class CDclFormObject;
class CProject;
enum DclFormType;


//This struct is used to pass standard and custom initialization data to the dialog creation function
struct DialogParams
{
	DialogParams( const CPoint& p, const CRect& s, LPARAM d = NULL )
		: position( p ), size( s ), lpData( d ) {}
	CPoint position; //desired initial position
	CRect size; //desired initial size
	LPARAM lpData; //dialog-specific initialization data
};


//This class represents a single instance of an ODCL form in AutoCAD. Since the different dialog types 
//have disparate base classes, it isn't possible to reference them via a common base class, so instead 
//each specific dialog class exposes an instance of this interface via a member object and registers it 
//with the global workspace. This interface must work correctly when the dialog it represents is 
//windowless (usually before the window has been created, or after it is destroyed). The current ODCL 
//implementation supports only one instance of a form at a time, so this class adds an informational 
//pointer to the corresponding form object during construction (and removes it during destruction) in 
//order to make it easy and deterministic for a form to find its corresponding dialog object. If support 
//for multiple simultaneous form instances is added in the future, the informational pointer must either 
//be removed or replaced with a collection of pointers.*
// * 2007-02-04 [ORW]

class CDialogObject
{
	static ULONG mnNextFormId; //constantly incremented unique dialog id

	// Attributes
protected:
	UINT mnID;
	CDclFormObject* mpSourceForm;
	CProject* mpProject;
	CWnd* mpHostDlg;

public:
	CDialogObject( CDclFormObject* pDclForm, CWnd* pHostDlg );
	virtual ~CDialogObject();

	// Services
public:
	virtual DclFormType GetType() const = 0;
	virtual const CControlPane& GetControlPane() const = 0; //control pane should be a member of the derived class
	virtual CControlPane& GetControlPane() = 0; //control pane should be a member of the derived class
	virtual bool IsModeless() const = 0;
	virtual bool IsDockable() const = 0;
	virtual bool IsResizable() const = 0;
	virtual HWND GetHWnd() const = 0;
	virtual bool IsFloating() const { return true; }
	virtual bool IsDirty() const { return false; }
	virtual bool SetDirty( bool bDirty = true ) { return false; }

	// Dialog
	virtual bool CreateModeless() const { return false; }
	virtual void CloseDialog(int nStatus = -1) const = 0;
	virtual INT_PTR DoModal() { return -1; }
	virtual bool Show(bool bShow = true);
	virtual bool CenterDialog();
	virtual bool ResizeDialog( long nNewWidth, long nNewHeight );
	virtual bool CenterAndResizeDialog( long nNewWidth, long nNewHeight );
	virtual bool SetFocus();
	virtual bool GetWindowRect( CRect& rcDlg ) const;
	virtual bool GetClientRect( CRect& rcDlg ) const;
	virtual bool SetMinMaxSize( const CSize& min, const CSize& max );

	// Properties
public:
	UINT GetID() const { return mnID; }
	const CDclFormObject* GetSourceForm() const { return mpSourceForm; }
	CDclFormObject* GetSourceForm() { return mpSourceForm; }
	const CProject* GetProject() const { return mpProject; }
	CProject* GetProject() { return mpProject; }
	const CWnd* GetWindow() const { return mpHostDlg; }
	CWnd* GetWindow() { return mpHostDlg; }

// Operations
public:
	//static CDialogObject* Create( CDclFormObject* pDclForm, CWnd* pParent = NULL,
	//															DialogParams* pParams = NULL );

// Implementation
protected:
	static UINT GetNextDialogId() { return mnNextFormId++; }
};
