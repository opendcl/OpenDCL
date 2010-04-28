#pragma once

#include "DialogControl.h"
#include "ControlPane.h"

class CDclFormObject;
class CProject;
enum FormType;


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

class CDialogObject : public CDialogControl
{
	static ULONG mnNextFormId; //constantly incremented unique dialog id
	int mnNCWidth; //width of non-client window area
	int mnNCHeight; //height of non-client window area
	int mnMinWidth;
	int mnMinHeight;
	int mnMaxWidth;
	int mnMaxHeight;
	bool mbClosing;
	UINT mnID;

	// Attributes
protected:
	TDclFormPtr mpSourceForm;
	TProjectPtr mpProject;

private:
	bool mbIgnoreSizing;

public:
	CDialogObject( TDclFormPtr pSourceForm, CControlPane* pPane, CWnd* pHostDlg );
	virtual ~CDialogObject();

	// Properties
public:
	UINT GetID() const { return mnID; }
	const TDclFormPtr GetSourceForm() const { return mpSourceForm; }
	TDclFormPtr GetSourceForm() { return mpSourceForm; }
	const TProjectPtr GetProject() const { return mpProject; }
	TProjectPtr GetProject() { return mpProject; }

// Operations
public:
	//static CDialogObject* Create( TDclFormPtr pDclForm, CWnd* pParent = NULL,
	//															DialogParams* pParams = NULL );

// Implementation
protected:
	static UINT GetNextDialogId() { return mnNextFormId++; }
	bool IsIgnoreSizing() const { return mbIgnoreSizing; }
	bool IgnoreSizing( bool bIgnore = true )
		{ bool bOld = mbIgnoreSizing; mbIgnoreSizing = bIgnore; return bOld; }
	virtual void SetNCWidth( int nWidth ) { mnNCWidth = nWidth; }
	virtual void SetNCHeight( int nHeight ) { mnNCHeight = nHeight; }
	virtual int GetNCWidth() const { return mnNCWidth; }
	virtual int GetNCHeight() const { return mnNCHeight; }

	// Services
public:
	virtual FormType GetType() const = 0;
	virtual const CControlPane* GetControlPane() const = 0; //control pane should be a member of the derived class
	virtual CControlPane* GetControlPane() = 0; //control pane should be a member of the derived class
	virtual CWnd* GetTopLevelWnd() { return mpControlWnd; }
	virtual bool IsModeless() const = 0;
	virtual bool IsDockable() const = 0;
	virtual bool IsResizable() const = 0;
	virtual bool IsFloating() const { return true; }
	virtual bool IsDirty() const { return false; }
	virtual bool SetDirty( bool bDirty = true ) { return false; }
	virtual bool IsClosing() const { return mbClosing; }
	virtual void SetClosing( bool bClosing = true ) { mbClosing = bClosing; }

	// Dialog
public:
	virtual bool CreateModeless( UINT nID ) { return false; }
	virtual void CloseDialog(int nStatus = -1) = 0;
	virtual INT_PTR DoModal() { return -1; }
	virtual bool Show(bool bShow = true);
	virtual bool CenterDialog();
	virtual bool MoveDialog( long nNewLeft, long nNewTop );
	virtual bool ResizeDialog( long nNewWidth, long nNewHeight );
	virtual bool CenterAndResizeDialog( long nNewWidth, long nNewHeight );
	virtual CRect GetEffectiveWindowRect() const; //returns control's window rect in parent's client coordinates
	virtual CRect GetEffectiveClientRect() const; //return control's client rect
protected:
	virtual void GetMinMaxSize( CSize& szMin, CSize& szMax ); //returns min/max size in window coordinates

	// Windows painting helpers
protected:
	virtual BOOL HandleEraseBkgnd( CDC* pDC );

	// Creation & Property Application
public:
	virtual CRect GetWndRect() const; //get window position from properties
	virtual DWORD GetWndStyle() const; //get window style from properties
	virtual CString GetWndCaption() const; //get window caption from properties
	virtual void OnFrameChanged(); //called by member functions that change the non-client size
	virtual void ApplyPosition(); //move control window to new position

	// for properties without specific handlers
	virtual bool ApplyProperty( TPropertyPtr pProp );

	// handlers for specific properties
	virtual bool OnApplyBackgroundColor( TPropertyPtr pProp ); //Prop::BackgroundColor
	virtual bool OnApplyResizable( TPropertyPtr pProp ); //Prop::AllowResizing
	virtual bool OnApplyWidth( TPropertyPtr pProp ); //Prop::Width
	virtual bool OnApplyHeight( TPropertyPtr pProp ); //Prop::Height
	virtual bool OnApplyMinMaxSize( TPropertyPtr pProp ); //min/max width properties
	virtual bool OnApplyIcon( TPropertyPtr pProp ); //Prop::TitleBarIcon
	virtual bool OnApplyTitleBar( TPropertyPtr pProp ); //Prop::TitleBar
};
