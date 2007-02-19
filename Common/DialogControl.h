// DialogControl.h : header file
//

// This class defines a common interface exported and implemented by all ODCL controls. Every control 
// must have a member derived from this class that is returned by its GetWindow() member function. Controls 
// are registered with and their lifetime is managed by a CControlPane object (which may be nested).

#pragma once

#include "PropertyObject.h"

class CDclControlObject;
class CControlPane;
enum PropertyId;
enum ControlType;


struct ControlParams
{
};


/*
Architecture of ODCL Controls -- 2007-02-07 [ORW]

The original implementation of controls duplicates a lot of logic in every control. The problem is 
compounded by the fact that the various controls are derived from disparate base classes, thus making 
CWnd the highest common class. In order to restructure control logic and provide for a common interface 
shared by all controls, I've created the CDialogControl virtual base class. The goal is for every 
control to export an instance of CDialogObject, and for this class to be the control's interface to 
the rest of the code. However, since there are many controls and little time, the system is designed 
to work correctly with the original controls, thereby allowing the changeover to be completed over 
time. For example, when I need to debug a problem or make a change to a specific control, I start by 
changing the control to use the new system. The steps for changing am old control to use the new 
system are generally as follows:
1) Define a class derived from CDialogControl which provides an implementation for the pure virtual 
function CreateGlobalVariables() and overrides any virtual functions that require a custom 
implementation. Add a protected member of the new CDialogControl derived class to the control class 
and name the new member mControlX (ControlX = control interface).
3) Add public accessors named GetDialogControl for the CDialogControl interface defined by mControlX.
3) Add protected members named mpSourceControl and mpControlPane which must be initialized in all 
constructors to point to the source CDclControlobject and the managing CControlPane.
4) Add an override of the virtual CWnd::PostNcDestroy() function that calls the base class and 
immediately before returning calls 'delete this' to delete 'this' instance of the control class. 
This step is necessary because the new design gives the window control of the class' lifetime, rather 
than the other way around as under the old scheme. Omitting this step will result in memory leaks, but 
would not otherwise be noticeable.
5) Add one (or more if needed for optional parameters) Create() member function that uses the services 
provided by CDialogControl (via the mControlX member) to create the window. This must be done with 
great care to ensure that the code logic isn't changed, but merely moved to CDialogControl where 
possible. The new Create() function should call mControlX.ApplyPropertiesEnum() to apply the design 
time control properties to the new control window after it is successfully created.
6) Add a call to Create() in the constructor of the control class to create the control window at 
construction time rather than requiring a separate call to Create(). This will generally require one 
constructor overload for every overloaded version of Create().
7) Change the code for creating a new control to eliminate the extra calls and replace these with a 
single call to 'new CMyControlClass', then return the CDialogControl interface exported by the revised 
control class.

See the comments below for an explanation of how pointers to the new style controls are differentiated 
from pointers to the old style controls at control pane destruction time. This is necessary because 
the old style controls must be explicitly deleted, while the new style control will be deleted by the 
control's PostNcDestroy() function.
*/


//There are two pointer types defined to help solve the problem of determining the correct way to 
//destroy a CDialogControl instance at runtime (see above). Once all controls are ported to the new 
//style, TDialogControlPtr can be changed back to a plain pointer as shown below and the current 
//typedef for TDialogControlPtr can be removed along with the TDialogControlLockedPtr class.
//typedef TDialogControlPtr TDialogControlPtr;
typedef RefCountedPtr< class CDialogControl > TDialogControlPtr;


//Interface to a locked ref-counted pointer that does no reference counting but can nevertheless
//be cast to a ref-counted pointer, thus allowing the types to be used interchangeably in code. This 
//solves the problem of distinguishing between pointers that must be destroyed and pointers to 
//members of another class that will be destroyed by the owning class.
class TDialogControlLockedPtr : public TDialogControlPtr
{
private:
	typedef TDialogControlPtr _base;

public:
	TDialogControlLockedPtr( TDialogControlPtr pTarget ) : _base( pTarget ) { Lock(); }
	TDialogControlLockedPtr( CDialogControl& Target ) : _base( &Target ) { Lock(); }
	virtual ~TDialogControlLockedPtr(void) {}

	// copy and assignment
	TDialogControlLockedPtr(const TDialogControlLockedPtr & src) : _base( src ) {}
	TDialogControlLockedPtr(const _base & src) : _base( src ) {}
	TDialogControlLockedPtr& operator=(const TDialogControlLockedPtr & src) { _base::operator=( src ); return *this; }
	TDialogControlLockedPtr& operator=(const _base & src) { _base::operator=( src ); return *this; }
	TDialogControlPtr operator=(TDialogControlPtr src) { return _base::operator=( src ); }
};



class CDialogControl
{
	// Attributes
protected:		
	CDclControlObject* mpTemplate;
	CControlPane* mpControlPane;
	CWnd* mpControl;

public:
	CDialogControl( CDclControlObject* pTemplate, CControlPane* pPane, CWnd* pControl );
	virtual ~CDialogControl();

	//static TDialogControlPtr Create( CDclControlObject* pTemplate, CControlPane* pPane,
	//																 UINT nID, ControlParams* pParams = NULL );

	// Properties
public:
	const CDclControlObject* GetTemplate() const { return mpTemplate; }
	CDclControlObject* GetTemplate() { return mpTemplate; }
	const CControlPane* GetControlPane() const { return mpControlPane; }
	CControlPane* GetControlPane() { return mpControlPane; }
	const CWnd* GetControl() const { return mpControl; }
	CWnd* GetControl() { return mpControl; }
	ControlType GetControlType() const;
	UINT GetControlId() const { return (mpControl? mpControl->GetDlgCtrlID() : (UINT)-1); }

	// Name rendition
public:
	CString GetKeyName() const;
	CString GetKeyPath() const;

	// Implementation
public:

	// Services
public:
	virtual TDialogControlPtr FindControl( HWND hwndControl ) const { return NULL; } //find nested control
	virtual TDialogControlPtr FindControl( LPCTSTR pszControlName, ControlType type = CtlInvalid ) const { return NULL; } //find nested control


	// Control
public:
	virtual CRect GetWndRect() const; //get window position from properties
	virtual DWORD GetWndStyle() const; //get window style from properties
	virtual CString GetWndCaption() const; //get window caption from properties

	// control properties
	virtual bool ApplyPropertiesEnum(); //iterate through all template properties and apply them to the control

	// for properties without specific handlers
	virtual bool OnApplyProperty( RefCountedPtr< CPropertyObject > pProp );

	// handlers for specific properties
	virtual bool OnApplyBorderStyle( RefCountedPtr< CPropertyObject > pProp ); //nBorder
	virtual bool OnApplyEnabled( RefCountedPtr< CPropertyObject > pProp ); //nEnabled
	virtual bool OnApplyCaption( RefCountedPtr< CPropertyObject > pProp ); //nCaption
	virtual bool OnApplyCaptionFont( RefCountedPtr< CPropertyObject > pProp ); //nLabelName
};
