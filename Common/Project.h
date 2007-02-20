#pragma once

/*
General Project Structure and Nomenclature Notes -- 2007-01-28 [ORW]

The logical hierarchy of an ObjectDCL project starts with the project itself at the top. The project contains 
one or more forms, and these in turn contain one or more controls. The original code tended to use the words  
dialog and form interchangeably, but in code that I've rewritten I use "dialog" to refer to an instantiated 
form (i.e. a dialog window on the screen), and "form" or "template" when referring to the form definition 
data.

Forms can be nested, therefore a nested form is a valid kind of control -- or said another way, forms are 
also controls. All controls (including forms) contain a collection of named properties. Some properties 
like "Name", "Width", "Height", etc. are common for all (or at least most -- there are always exceptions) 
controls, but many properties are control-specific. Every property has a name, a data type, and a value. Most 
control properties are defined at design time and never change, but some (e.g. "Width" and "Height") may 
change at runtime in an instance of the control.

The important classes that represent the different elements of a project are:

CProject           Project.h/cpp            : a single project
CDclFormObject     DclFormObject.h/cpp      : a single form
CDclControlObject  DclControlObject.h/cpp   : a single control (which may contain child controls)
CPropertyObject    PropertyObject.h/cpp     : a single property

CProjectCollection ProjectCollection.h/cpp  : collection of projects (e.g. all currently loaded projects)
CFontCollection    FontCollection.h/cpp     : collection of CFont objects that defines all needed fonts
CImageListObject   ImageListObject.h/cpp    : collection of images (either icons or bitmaps) used by a control

CWorkspace         Workspace.h/cpp          : global data (projects, fonts, and client-specific data)
CDialogObject      DialogObject.h/cpp       : instantiated ODCL form interface (analagous to CDclFormObject)
CControlPane       ControlPane.h/cpp        : manages a collection of control windows (can be nested via tab pages)
CDialogControl     DialogControl.h/cpp      : instantiated ODCL control interface (analagous to CDclControlObject)

The ObjectDCL editor defines CEditorProject and CEditorWorkspace that customize and extend the common
CProject/CWorkspace classes. Likewise, the ARX modules define CArxProject/CArxWorkspace to customize and 
extend the common classes with ARX-specific data and services.

To prevent name collisions, ObjectDCL uses a case-insensitive three-tier naming system to name a specific 
control instance:
<project-name>_<form-name>_<control-name>

Note that, since forms can be controls, <control-name> for a nested control will itself be constructed by 
concatenating the name of its parent form, so that the resulting name could look like this, for example:
MyProject_MyMainForm_MySubForm_MyDeeplyNestedForm_MyButton

To achieve a consistent and easily maintainable naming system, each element in the project hierarchy should 
be able to identify itself. This is achieved by defining functions* in each element class that will 
reliably return the element name or path when needed. The names of some elements (e.g. projects) are static 
and never change, but dialogs and most controls can change their names at runtime in some cases. For that 
reason it is important to not cache element names, but always query for them at the time they are needed.

* For now, the element name is returned by a function called GetKeyName(). The function GetKeyPath() returns 
the complete "path" for the control by concatenating the names of the owning project, the parent form, 
and the subject control. It would make sense to define a standard name rendition interface as a generic base 
class that all named objects derive from or export. This I leave as an exercise for the future.
*/

#include "Workspace.h"
#include <vector>

class CDclFormObject;
class CDclControlObject;
class COleControlObject;
class CStgFile;
class CPictureObject;
class AxPropertyDescriptor;
class AxMethodDescriptor;
class AxEventDescriptor;
class CAxContainer;

#define activeProject (theWorkspacePtr()->GetActiveProject()) //shortcut to the active project*
//* The concept of an active project is only useful in the editor, and even there, it is used only 
//because there is no reliable system in place that allows every project element to identify 
//its containing project. In the future this should be rectified in the editor in the same way it 
//has been in the ARX modules (by requiring every element to be able to identify its owner), thus 
//removing the primary limitation preventing the editor from handling multiple simultaneous 
//projects.  2007-02-07 [ORW]


//Error status returned by file I/O functions
enum IOStatus
{
	statOK = 0,
	statFail,
	statFileNotFound,
	statReadFailed,
	statInvalidFormat,
	statWriteFailed,
};



//CProject represents the contents of a single .odc file
class CProject : public CObject
{
protected:		
	CList< CPictureObject* > mPictures;
	CList< CDclFormObject* > mDclForms;
	std::vector< RefCountedPtr< COleControlObject > > mOleControls;
	CList< CDclControlObject* > mClipBoard;
	CString msKeyName;
	CString msProjectFilePath;
	CString msBaseFileName; //the project file base name (no path, no extension)
	CStringArray mrsActiveXFiles;
	CString msLispFileName;
	CString msDistFileName;

	CString msPassword;
	bool mbHasPassword;
	DWORD mnAutoCADVersion;
	DWORD mnPurchaseState;
	bool mbFreeVersion;
	
public:		
	CString m_sDefaultFontName;
	long m_nDefaultFontSize;
	BOOL m_bDefaultFontItalic;
	BOOL m_bDefaultFontUnderLine;
	BOOL m_bDefaultFontBold;
	BOOL m_bDefaultFontSizeStyle;

public:
	CProject();
	CProject( LPCTSTR pszKeyName );
	virtual ~CProject();
protected:
	void Initialize();

	//2007-01-30 [ORW]: save version set to 9 (no change from ObjectDCL 3)
	//2007-02-09 [ORW]: save version set to 10
	//2007-02-14 [ORW]: save version set to 11 (form count changed from short to unsigned long)
	ULONG GetCurrentSaveVersion() const { return 11; }

public:
	virtual LPCTSTR GetPassword() const { return _T("d32afd3aw3aq3fdaw3"); }
	virtual bool IsInUse() const;
	virtual DWORD GetPurchaseState() const { return mnPurchaseState; }

	HBITMAP GetBitmap( UINT nID, CSize& sz ) const;
	HICON GetIcon( UINT nID ) const;
	bool GetPictureSize( UINT nID, CSize& size ) const; //return true if found
	CPictureObject* FindPicture( UINT nID ) const;

	//Centralized File I/O
	virtual DWORD GetCurrentProjectFileFormatVersion() const { return 11; }
	virtual IOStatus ReadFromFile( LPCTSTR pszFilePath ); //read a project from all supported file formats
	virtual IOStatus WriteToFile( LPCTSTR pszFilePath ); //write in the current project file format
protected:
	IOStatus ReadFromTextFile( LPCTSTR pszFilePath ); //read a project from a text file
  IOStatus ReadFromTextFile(std::ifstream &sFile, const CString &fileName); //read from text file
  IOStatus ReadFromTextFile9(std::ifstream &sFile, const CString &fileName); //read version 9 text file

public:
	//Attributes
	const CList<CPictureObject*>& GetPictureList() const { return mPictures; }
	CList<CPictureObject*>& GetPictureList() { return mPictures; }
	const CList< CDclFormObject* >& GetDclFormList() const { return mDclForms; }
	//CList< CDclFormObject* >& GetDclFormList() { return mDclForms; }
	const CString& GetKeyName() const { return msKeyName; }
	void SetKeyName( LPCTSTR pszKeyName );
	const CString& GetBaseFileName() const { return msBaseFileName; }
	const CString& GetProjectFilePath() const { return msProjectFilePath; }
	const CString& GetLispFileName() const { return msLispFileName; }
	const CString& GetDistFileName() const { return msDistFileName; }

	//Services
	virtual void DeleteForm( CDclFormObject*& pDclForm );
	virtual CDclFormObject* AddForm( DclFormType nType );
	virtual CDclFormObject* AddForm( DclFormType nType, CDclFormObject* pParentForm );
	virtual void SetGlobalVariableNames( LPCTSTR pszRootName = NULL );
	virtual void ClearGlobalVariableNames();
	bool AddActiveXFile( LPCTSTR pszFileName );
	bool RemoveActiveXFile( LPCTSTR pszFileName );
	bool HasActiveXFile( LPCTSTR pszFileName ) const;
	INT_PTR AddPicture(CPictureObject* pPicture);
	bool LoadPictureFile(LPCTSTR szFile, int nID);

protected:

	//from old CProjectList
public:
	void ClearProject();
	CDclFormObject* GetDclForm(CString DclFormName);
	void ClearR14Events();
	CDclFormObject* GetDclTabChildForm(CString sDclParentUniqueName, int nTabIndex);
	CDclFormObject* GetParentDclForm(CString ParentName);
	CString QueryForLispFileName();
	CString QueryForOdsFileName();
	void SaveDistFile();
	int CountDeletedForms();

	// OLE object management
	void AddOleObject(const CLSID& clsid, CAxContainer *pAxCont);
	bool HasOleObject(const CLSID& clsid);
	CString GetOleObjectName(const AxPropertyDescriptor *pProperty);
	RefCountedPtr< COleControlObject > GetOleObject(const CLSID& clsid);
	RefCountedPtr< COleControlObject > GetOleObject(const AxPropertyDescriptor *pProperty);
	RefCountedPtr< COleControlObject > GetOleObject(const AxMethodDescriptor *pProperty);
	RefCountedPtr< COleControlObject > GetOleObject(const AxEventDescriptor *pEvent);

public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o

protected:
	DECLARE_SERIAL(CProject)

#ifdef _DIAGNOSTIC
public:
	virtual LPCTSTR toString() const;
	virtual void dump( bool bDeep = true ) const;
#endif

#ifdef _DEBUG
public:
	virtual void dumpDebugger( bool bDeep = true ) const;
#endif
};
