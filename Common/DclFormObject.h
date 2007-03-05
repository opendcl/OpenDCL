#pragma once

#include "DclFormTypes.h"
#include "ImageListObject.h"

class CProject;
class CDclControlObject;
class CFontCollection;
class CDialogObject;
enum ControlType;


/////////////////////////////////////////////////////////////////////////////
// CDclFormObject document

class CDclFormObject : public CObject
{
	friend class CControlPane;
public:
	typedef CList< CDclFormObject* > collection;

protected:
	CProject* mpProject;
	CList< CDclControlObject* > mDclControls;
	DclFormType mType;
	CString msName;
	CDialogObject* mpDlgObject; //informational pointer to the one and only instance of this form (or NULL)
	std::vector< CImageListObject > mImageLists;

	//for managing tab hierarchy and order
	CDclFormObject* mpParentForm;
	CString msUniqueName;
	CString msUUID;
	short mnTabIndex;

// Attributes
public:
	bool m_bDeleted;
	BOOL m_bUsesClientRect;
	bool m_bLoaded;
	CString m_sSubFileName;
	CString m_sFileName;
	CString m_sVarName;
	//CRect m_rcPos;

	//Used in editor only
	HTREEITEM m_htiTreeItem;
	CView *m_pChildWnd;
	CMDIChildWnd *m_pMdiChildWnd;

protected:
	CDclFormObject();
public:
	CDclFormObject( CProject* pProject, DclFormType type = VdclInvalid );
	virtual ~CDclFormObject();

	//2007-01-30 [ORW]: save version set to 4 (no change from ObjectDCL 3)
	//2007-02-19 [ORW]: save version set to 5 (image list collection removed from form object)
	ULONG GetCurrentSaveVersion() const { return 5; }

// Operations
public:
	void AddControl( CDclControlObject* pDclControl );
	CDclControlObject* AddControl( ControlType type, LPCTSTR pszKeyName );
	void DeleteControl( CDclControlObject*& pDclControl );
	void PurgeDeletedControls();
	void PurgeDeletedImageLists();
	bool ReorderControl( CDclControlObject* pDclControl, bool bToFront, bool bDeferReindexing = false );
	bool ReorderControl( CDclControlObject* pDclControl, size_t idxNew, bool bDeferReindexing = false );
	void ReindexControls();

	void ClearR14Events();
	void IncrementPictureId(int nIdIncrement);
	int CountDeletedControls() const;
	bool CanWeDeleteForm() const;
	size_t CountDeletedImageLists() const;
	CString GetDclFormTitle() const;
	long GetDclFormTitleBarIcon();
	void EnsureIsLoaded();

	//void SaveSS(int n, int nType, class CStgFile &FileStg, CDocument *pDoc);
	//static CDclFormObject* ReadSS(int n, int nType, CStgFile &FileStg, CDocument *pDoc);

protected:
	CDclControlObject* CreateControlProperties();

	//Services
public:
	void ClearControls();
	CDclControlObject* FindControl( LPCTSTR pszControlName ) const;
	CDclControlObject* FindControl( LPCTSTR pszControlName, ControlType eType ) const;
	CDclControlObject* FindFirstControlOfType( ControlType eType ) const;
	bool FindControls( ControlType eType, CList< CDclControlObject* >& Results ) const;
	CDclControlObject* FindControlWithVarName( LPCTSTR pszVarName ) const;
	bool GetControlFonts( CFontCollection& Fonts ) const;
	void ZOrderFrontAddTabControls();
	void SetGlobalVariableName( LPCTSTR pszRootName = NULL, bool bUpdateChildren = true );
	void ClearGlobalVariableName( bool bUpdateChildren = true );

	//Attributes
public:
	const CProject* GetProject() const { return mpProject; }
	CProject* GetProject() { return mpProject; }
	DclFormType GetType() const { return mType; }
	const CDialogObject* GetFormInstance() const { return mpDlgObject; }
	CDialogObject* GetFormInstance() { return mpDlgObject; }
	void SetFormInstance( CDialogObject* pDlgObject );
	INT_PTR GetControlCount() const;
	const CList< CDclControlObject* >& GetControlList() const { return mDclControls; }
	//CList< CDclControlObject* >& GetControlList() { return mDclControls; }
	const CDclControlObject* GetControlProperties() const;
	CDclControlObject* GetControlProperties();
	CSize GetFormSize() const;
	UUID GetUUID() const;
	const CString& GetUUIDAsString() const { return msUUID; }
	const CString& GetUniqueName() const { return msUniqueName; }
	void SetUniqueName( LPCTSTR pszName ) { msUniqueName = pszName; }
	CString GetParentName() const { return mpParentForm? mpParentForm->GetUniqueName() : CString(); }
	const CDclFormObject* GetParentForm() const { return mpParentForm; }
	CDclFormObject* GetParentForm() { return mpParentForm; }
	void SetParentForm( CDclFormObject* pParentForm );
	void SetParentForm( LPCTSTR pszParentUniqueName );
	short GetTabIndex() const { return mnTabIndex; }
	void SetTabIndex( short nIndex ) { mnTabIndex = nIndex; }

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
