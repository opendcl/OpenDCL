// DialogControl.h : header file
//

// This class defines a common interface exported and implemented by all ODCL controls. Every control 
// must have a member derived from this class that is returned by its GetWindow() member function. Controls 
// are registered with and their lifetime is managed by a CControlPane object (which may be nested).

#pragma once

#include "PPToolTip.h"
#include "DclControlObject.h"
#include <list>
#include <vector>

class CControlPane;
class CAcadColorService;
class CThemeHelperST;
class CDragDropService;
class CArxControlServices;
class CControlManager;
enum Prop::Id;
enum ControlType;


struct ControlParams
{
};


/*
Architecture of ODCL Controls -- 2007-02-07 [ORW]
  revised to use a multiple inheritance model -- 2007-02-25 [ORW]

The original implementation of controls duplicates a lot of logic in every control. The problem is 
compounded by the fact that the various controls are derived from disparate base classes, thus making 
CWnd the highest common class. In order to provide for a common interface shared by all controls, to  
eliminate duplication of code, and to improve consistency of control appearance and behavior by 
ensuring that controls utilize the same code in the dialog editor at design time that they do in the 
ARX module at run time, I've created the CDialogControl virtual base class. The goal is for every 
control to export an instance of CDialogObject, and for this class to be the control's interface to 
the rest of the code. However, since there are many controls and little time, the system is designed 
to work correctly with the original controls, thereby allowing the changeover to be completed over 
time. For example, when I need to debug a problem or make a change to a specific control, I start by 
changing the control to use the new system. The steps for changing an old control to use the new 
system are generally as follows:
1) Derive the control class from an appropriate CDialogControl-derived class that implements all 
pure virtual functions of the base class and overrides any virtual functions that require a custom 
implementation for the particular control. Typically this means adding an override of OnApplyProperty 
and handling any properties that are specific to the control. Be sure to supermessage the base class' 
OnApplyProperty so that common properties are correctly handled.
2) Implement a Create() function that creates the control's window and performs any necessary 
initialization  of the control. If Create() is implemented in a base class, add an override of 
Create() that simply supermessages the base class. This is important because the Create() function 
should be implemented by the lowest class in the inheritance chain in order to be sure that all base 
class initialization has been completed by the time Create() executes. Ensure that no base classes 
call Create() before initialization is complete by passing 'false' for the bCreate argument to the 
class constructor. In the control class constructor, call Create() if and only if bCreate is true.
3) Add an override of the virtual CWnd::PostNcDestroy() function that first calls the base class,  
then immediately before returning calls 'delete this' to delete 'this' instance of the control class. 
This step is necessary because the new design gives the window control of the class' lifetime, rather 
than the other way around as under the old scheme. Omitting this step will result in memory leaks, but 
may not otherwise be noticeable.
4) Combine the editor and ARX module's control handling code into a single common class with derived 
classes for any editor or ARX specific functionality. Some common ARX specific or editor specific 
functionality may be added by simply including an implementation object as a member of the control 
class.
5) Change the code for creating a new control to eliminate the extra calls and replace these with a 
single call to 'new CMyControlClass', then return the new CDialogControl by dereferencing the pointer 
to the control and allowing the control's TDialogObjectPtr cast operator to provide a RefCountedPtr 
that is locked to prevent automatic destruction.

See the comments below for an explanation of how pointers to the new style controls are differentiated 
from pointers to the old style controls at control pane destruction time. This is necessary because 
the old style controls must be explicitly deleted, while the new style control will be deleted by the 
control's PostNcDestroy() function.
*/


//There are two pointer types defined to help solve the problem of determining the correct way to 
//destroy a CDialogControl instance at runtime (see above). Once all controls are ported to the new 
//style, TDialogControlPtr can be changed back to a plain pointer as shown below and the current 
//typedef for TDialogControlPtr can be removed along with the TDialogControlLockedPtr class.
//typedef class CDialogControl* TDialogControlPtr;
typedef RefCountedPtr< class CDialogControl > TDialogControlPtr;


//Interface to a locked ref-counted pointer that does no reference counting but can nevertheless
//be cast to a ref-counted pointer, thus allowing the types to be used interchangeably in code. This 
//solves the problem of distinguishing between pointers to objects that must be destroyed by the 
//reference counter and pointers to new style controls that will be destroyed when their window is 
//destroyed.
typedef LockedPtr< class CDialogControl > TDialogControlLockedPtr;



class CDialogControl
{
protected:		
	TDclControlPtr mpTemplate;
	CControlPane* mpControlPane;
	CWnd* mpControlWnd;
	CControlManager* mpControlManager;

private:		
	bool mbEnumProps;
	CPPToolTip mToolTip;

public:
	CDialogControl( TDclControlPtr pTemplate, CControlPane* pPane, CWnd* pControlWnd );
	virtual ~CDialogControl();

	//static TDialogControlPtr Create( TDclControlPtr pTemplate, CControlPane* pPane,
	//																 UINT nID, ControlParams* pParams = NULL );

	// Properties
public:
	const TDclControlPtr GetTemplate() const { return mpTemplate; }
	TDclControlPtr GetTemplate() { return mpTemplate; }
	const CControlPane* GetControlPane() const { return mpControlPane; }
	CControlPane* GetControlPane() { return mpControlPane; }
	const CPPToolTip& GetToolTipCtrl() const { const_cast< CDialogControl* >(this)->CreateTooltip(); return mToolTip; }
	CPPToolTip& GetToolTipCtrl() { CreateTooltip(); return mToolTip; }
	virtual CAcadColorService* GetColorService() { return NULL; }
	virtual CThemeHelperST* GetThemeHelper();
	virtual const CWnd* GetControlWnd() const { return mpControlWnd; }
	virtual CWnd* GetControlWnd() { return mpControlWnd; }
	virtual CWnd* GetTopLevelWnd();
	virtual ControlType GetControlType() const;
	virtual UINT GetControlId() const { return (mpControlWnd? mpControlWnd->GetDlgCtrlID() : (UINT)-1); }
	virtual bool GetChildPanes( std::list< const CControlPane* >& listChildren ) const { return false; }
	virtual HWND GetHWnd() const { return mpControlWnd? mpControlWnd->m_hWnd : NULL; }
	virtual bool Focus();
	virtual CRect GetEffectiveWindowRect() const; //returns control's window rect in parent's client coordinates
	virtual CRect GetEffectiveClientRect() const; //return control's client rect

	// ARX specific services
	virtual const CArxControlServices* GetArxServices() const { return NULL; }
	virtual bool IsAsyncEvents() const;

	// Editor specific services
	CControlManager* GetControlManager() const { return mpControlManager; }
	void SetControlManager( CControlManager* pManager ) { mpControlManager = pManager; }

	// Name rendition
public:
	virtual CString GetKeyName() const;
	virtual CString GetKeyPath() const;
	virtual CString GetVarName() const;

	// Implementation
public:

	// Services
public:
	CAxContainerCtrl* GetActiveXCtrl() const;
private:
	void CreateTooltip() { if( !mToolTip.m_hWnd ) mToolTip.Create( mpControlWnd ); }

	// Drag and Drop Support
public:
	virtual CDragDropService* GetDragDropService() { return NULL; }
	virtual COleDropSource* GetDropSource() const { return NULL; }
	virtual COleDropTarget* GetDropTarget() const { return NULL; }
	virtual DROPEFFECT BeginDragDrop( const CPoint& point ); //point in client coordinates
	virtual DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData ); //called to get drag data from this control
	virtual DROPEFFECT OnDragEnter( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState );
	virtual DROPEFFECT OnDragOver( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState );
	virtual void OnDragLeave() {}
	virtual bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT dropEffect );

	// Creation & Property Application
public:
	virtual CRect GetWndRect() const; //get window position from properties
	virtual DWORD GetWndStyle() const; //get window style from properties
	virtual CString GetWndCaption() const; //get window caption from properties
	virtual void OnFrameChanged(); //called by member functions that change the non-client size
	virtual void OnNeedRepaint( bool bRepaintBackground = true, bool bUpdateNow = false ) const; //called when a property change requires a repaint
	virtual void ApplyPosition(); //move control window to new position
	virtual bool Create( CWnd* pParentWnd, UINT nID ) = 0;

	// control properties
	virtual bool ApplyPropertiesEnum(); //iterate through all template properties and apply them to the control
	virtual void ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst, std::vector< Prop::Id >& ridLast );
	virtual bool IsEnumeratingProperties() const { return mbEnumProps; }

	// for properties without specific handlers
	virtual bool OnApplyProperty( TPropertyPtr pProp );

	// handlers for specific properties
	virtual bool OnApplyName( TPropertyPtr pProp ); //Prop::Name
	virtual bool OnApplyForegroundColor( TPropertyPtr pProp ); //Prop::ForegroundColor
	virtual bool OnApplyBackgroundColor( TPropertyPtr pProp ); //Prop::BackgroundColor
	virtual bool OnApplyBorderStyle( TPropertyPtr pProp ); //Prop::BorderStyle
	virtual bool OnApplyEnabled( TPropertyPtr pProp ); //Prop::Enabled
	virtual bool OnApplyDragDropAllowDrop( TPropertyPtr pProp ); //Prop::DragDropAllowDrop
	virtual bool OnApplyVisible( TPropertyPtr pProp ); //Prop::Visible
	virtual bool OnApplyCaption( TPropertyPtr pProp ); //Prop::Caption, Prop::TitleBarText
	virtual bool OnApplyIsTabStop( TPropertyPtr pProp ); //Prop::IsTabStop
	virtual bool OnApplyBeginGroup( TPropertyPtr pProp ); //Prop::BeginGroup
	virtual bool OnApplyVScrollBar( TPropertyPtr pProp ); //Prop::VScrollBar
	virtual bool OnApplyHScrollBar( TPropertyPtr pProp ); //Prop::HScrollBar
	virtual bool OnApplyUseVisualStyle( TPropertyPtr pProp ); //Prop::UseVisualStyle
	virtual bool OnApplyToolTip( TPropertyPtr pProp ); //Prop::ToolTipTitle
	virtual bool OnApplyFont( TPropertyPtr pProp ); //Prop::FontName
	virtual bool OnApplyLeft( TPropertyPtr pProp ); //Prop::Left
	virtual bool OnApplyTop( TPropertyPtr pProp ); //Prop::Top
	virtual bool OnApplyWidth( TPropertyPtr pProp ); //Prop::Width
	virtual bool OnApplyHeight( TPropertyPtr pProp ); //Prop::Height
	virtual bool OnApplyLeftFromRight( TPropertyPtr pProp ); //Prop::LeftFromRight
	virtual bool OnApplyRightFromRight( TPropertyPtr pProp ); //Prop::RightFromRight
	virtual bool OnApplyTopFromBottom( TPropertyPtr pProp ); //Prop::TopFromBottom
	virtual bool OnApplyBottomFromBottom( TPropertyPtr pProp ); //Prop::BottomFromBottom
	virtual bool OnApplyUseLeftFromRight( TPropertyPtr pProp ); //Prop::UseLeftFromRight
	virtual bool OnApplyUseRightFromRight( TPropertyPtr pProp ); //Prop::UseRightFromRight
	virtual bool OnApplyUseTopFromBottom( TPropertyPtr pProp ); //Prop::UseTopFromBottom
	virtual bool OnApplyUseBottomFromBottom( TPropertyPtr pProp ); //Prop::UseBottomFromBottom
};


//This class deletes the control object in its destructor (used for old style and other dynamically 
//created controls)
class CAutoDialogControl : public CDialogControl
{
public:
	CAutoDialogControl( TDclControlPtr pTemplate, CControlPane* pPane, CWnd* pControlWnd )
		: CDialogControl( pTemplate, pPane, pControlWnd ) {}
	virtual ~CAutoDialogControl() { delete mpControlWnd; }
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
};
