#pragma once

#include "DclFormTypes.h"
#include "ImageListObject.h"
#include "Project.h"
#include "PtrTypes.h"
#include <list>

class CFontCollection;
class CDialogObject;
enum ControlType;

typedef std::list< TDclControlPtr > TDclControlList;

/////////////////////////////////////////////////////////////////////////////
// CDclFormObject document

class CDclFormObject : public CObject
{
	friend class CControlPane;

protected:
	TProjectLockedPtr mpProject;
	TDclControlList mDclControls;
	DclFormType mType;
	CString msName;
	UINT_PTR mnNextId;
	bool mbUsesClientRect;
	bool mbDeleted;
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
	CView *m_pChildWnd;
	CMDIChildWnd *m_pMdiChildWnd;

protected:
	CDclFormObject();
public:
	CDclFormObject( CProject* pProject, DclFormType type = VdclInvalid );
	virtual ~CDclFormObject();

	//2007-01-30 [ORW]: save version set to 4 (no change from OpenDCL 3)
	//2007-02-19 [ORW]: save version set to 5 (image list collection removed from form object)
	//2007-09-17 [ORW]: save version set to 6 (changed mbUsesClientRect from BOOL to bool)
	ULONG GetCurrentSaveVersion() const { return 6; }

// Operations
public:
	void AddControl( TDclControlPtr pDclControl );
	TDclControlPtr AddControl( ControlType type, LPCTSTR pszKeyName );
	void DeleteControl( TDclControlPtr& pDclControl );
	void PurgeDeletedControls();
	bool ReorderControl( TDclControlPtr pDclControl, bool bToFront, bool bDeferReindexing = false );
	bool ReorderControl( TDclControlPtr pDclControl, size_t idxNew, bool bDeferReindexing = false );
	void ReindexControls();
	size_t CountDeletedControls() const;

	//void SaveSS(int n, int nType, class CStgFile &FileStg, CDocument *pDoc);
	//static TDclFormPtr ReadSS(int n, int nType, CStgFile &FileStg, CDocument *pDoc);

protected:
	TDclControlPtr CreateControlProperties();

	//Services
public:
	void ClearControls();
	TDclControlPtr FindControl( LPCTSTR pszControlName ) const;
	TDclControlPtr FindControl( LPCTSTR pszControlName, ControlType eType ) const;
	TDclControlPtr FindFirstControlOfType( ControlType eType ) const;
	bool FindControls( ControlType eType, TDclControlList& Results ) const;
	TDclControlPtr FindControlWithVarName( LPCTSTR pszVarName ) const;
	bool GetControlFonts( CFontCollection& Fonts ) const;
	void SetGlobalVariableName( LPCTSTR pszRootName = NULL, bool bUpdateChildren = true );
	void ClearGlobalVariableName( bool bUpdateChildren = true );
	TDclFormPtr AddChildForm( DclFormType type );

	//Attributes
public:
	const TProjectPtr GetProject() const { return mpProject; }
	TProjectPtr GetProject() { return mpProject; }
	DclFormType GetType() const { return mType; }
	bool IsModeless() const;
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
	void SetUniqueName( LPCTSTR pszName ) { msUniqueName = pszName; }
	CString GetParentName() const { return mpParentForm? mpParentForm->GetUniqueName() : CString(); }
	const TDclFormPtr GetParentForm() const { return mpParentForm; }
	TDclFormPtr GetParentForm() { return mpParentForm; }
	void SetParentForm( TDclFormPtr pParentForm );
	void SetParentForm( LPCTSTR pszParentUniqueName );
	short GetTabIndex() const { return mnTabIndex; }
	void SetTabIndex( short nIndex ) { mnTabIndex = nIndex; }
	bool UsesClientRect() const { return mbUsesClientRect; }
	void SetUsesClientRect( bool bUsesClientRect = true ) { mbUsesClientRect = bUsesClientRect; }
	bool IsDeleted() const { return mbDeleted; }
	void SetDeleted( bool bDelete = true ) { mbDeleted = bDelete; }
	LPCTSTR GetTitleText() const;
	UINT_PTR GetTitleBarIcon();
	UINT_PTR GetNextId() { if( mnNextId < mDclControls.size() ) mnNextId = mDclControls.size(); return mnNextId++; }

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

protected:
	DECLARE_SERIAL(CDclFormObject)

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
