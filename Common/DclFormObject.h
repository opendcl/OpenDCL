#pragma once

#include "FormTypes.h"
#include "ImageListObject.h"
#include "Project.h"
#include "DclControlObject.h"
#include <list>

class CFontCollection;
class CDialogObject;
enum ControlType;

typedef std::list< TDclControlPtr > TDclControlList;

/////////////////////////////////////////////////////////////////////////////
// CDclFormObject document

class CDclFormObject
{
	friend class CControlPane;

protected:
	TProjectLockedPtr mpProject;
	TDclControlList mDclControls;
	FormType mType;
	CDialogObject* mpDlgObject; //informational pointer to the one and only instance of this form (or NULL)

	//for managing tab hierarchy and order
	TDclFormPtr mpParentForm;
	CString msUniqueName;
	CString msUUID;
	short mnTabIndex;

// Attributes
public:
	//Used in editor only
	HTREEITEM m_htiTreeItem;

protected:
	CDclFormObject();
public:
	CDclFormObject( CProject* pProject, FormType type = _FrmInvalid );
	virtual ~CDclFormObject();

	//2007-01-30 [ORW]: save version set to 4 (no change from OpenDCL 3)
	//2007-02-19 [ORW]: save version set to 5 (image list collection removed from form object)
	//2007-09-17 [ORW]: save version set to 6 (changed mbUsesClientRect from BOOL to bool)
	//2008-02-16 [ORW]: save version set to 7 (removed mbUsesClientRect)
	//2008-11-02 [ORW]: save version set to 8 (changed version from ULONG to BYTE)
	//2009-03-03 [ORW]: save version set to 9 (removed msName)
	BYTE GetCurrentSaveVersion() const { return 9; }

// Operations
public:
	virtual void AddControl( TDclControlPtr pDclControl, bool bAssignNewID = false );
	virtual TDclControlPtr AddControl( ControlType type, LPCTSTR pszKeyName, const CRect& rcControl );
	virtual void DeleteControl( TDclControlPtr& pDclControl );
	virtual bool ReorderControl( TDclControlPtr pDclControl, bool bToFront );
	virtual bool ReorderControl( TDclControlPtr pDclControl, size_t idxNew );

	//void SaveSS(int n, int nType, class CStgFile &FileStg, CDocument *pDoc);
	//static TDclFormPtr ReadSS(int n, int nType, CStgFile &FileStg, CDocument *pDoc);

protected:
	virtual void OnModified();
	TDclControlPtr CreateControlProperties();

	//Services
public:
	TDclControlPtr GetRefCountedPtr( CDclControlObject* pDclControl ) const;
	void ClearControls();
	TDclControlPtr FindControl( LPCTSTR pszControlName ) const;
	TDclControlPtr FindControl( LPCTSTR pszControlName, ControlType eType ) const;
	TDclControlPtr FindFirstControlOfType( ControlType eType ) const;
	bool FindControls( ControlType eType, TDclControlList& Results ) const;
	TDclControlPtr FindControlWithVarName( LPCTSTR pszVarName ) const;
	bool GetControlFonts( CFontCollection& Fonts ) const;
	void SetGlobalVariableName( LPCTSTR pszRootName = NULL, bool bUpdateChildren = true );
	void ClearGlobalVariableName( bool bUpdateChildren = true );
	void ResetEventNames( bool bUpdateChildren = true );
	TDclFormPtr AddChildForm( FormType type );

	//Attributes
public:
	const TProjectPtr GetProject() const { return mpProject; }
	TProjectPtr GetProject() { return mpProject; }
	FormType GetType() const { return mType; }
	CUndoManager* GetUndoManager() const { return (mpProject? mpProject->GetUndoManager() : NULL); }
	bool IsModeless() const;
	CWnd* GetFormWindow() const;
	const CDialogObject* GetFormInstance() const { return mpDlgObject; }
	CDialogObject* GetFormInstance() { return mpDlgObject; }
	void SetFormInstance( CDialogObject* pDlgObject );
	size_t GetControlCount() const { return mDclControls.size(); }
	const TDclControlList& GetControlList() const { return mDclControls; }
	const TDclControlPtr GetControlProperties() const;
	TDclControlPtr GetControlProperties();
	CSize GetFormSize() const;
	UUID GetUUID() const;
	const CString& GetUUIDAsString() const { return msUUID; }
	const CString& GetUniqueName() const { return msUniqueName; }
	void SetUniqueName( LPCTSTR pszName ) { msUniqueName = pszName; OnModified(); }
	CString GetParentName() const { return mpParentForm? mpParentForm->GetUniqueName() : CString(); }
	const TDclFormPtr GetParentForm() const { return mpParentForm; }
	TDclFormPtr GetParentForm() { return mpParentForm; }
	void SetParentForm( TDclFormPtr pParentForm );
	void SetParentForm( LPCTSTR pszParentUniqueName );
	short GetTabIndex() const { return mnTabIndex; }
	void SetTabIndex( short nIndex );
	LPCTSTR GetTitleText() const;
	UINT_PTR GetTitleBarIcon();
	UINT_PTR GetUniqueControlId();
public:
	static size_t GetCurrentControlIndex( TDclControlPtr pDclControl );

	//File I/O
public:
	virtual void Serialize(CArchive& ar);
  //IOStatus WriteToTextFile(FILE* pFile, const CString &fileName) const;
  IOStatus ReadFromTextFile(std::ifstream &sFile, const CString &fileName);
  IOStatus ReadFromTextFile4(std::ifstream &sFile, const CString &fileName);
	
	// Name rendition
public:
	CString GetKeyName() const;
	CString GetKeyPath() const;
	CString GetVarName() const;

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
