#pragma once

#include "ImageListObject.h"

class CProject;
class CDclControlObject;
class CFontCollection;
class CDialogObject;
enum ControlTypes;

enum DclFormType
{
	VdclInvalid = -1,
	VdclModal		= 0,
	VdclModeless	= 1,
	VdclDockable	= 2,
	VdclConfigTab	= 3,
	VdclTabForm	= 4,
	VdclFileDialog = 5,
};


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

	//for managing tab hierarchy and order
	CDclFormObject* mpParentForm;
	CString msUniqueName;
	CString msUUID;
	short mnTabIndex;

// Attributes
public:
	CList<CImageListObject*> m_ImageListCollection;	
	bool m_bDeleted;
	BOOL m_bUsesClientRect;
	bool m_bLoaded;
	CString m_sSubFileName;
	CString m_sFileName;
	CString m_sVarName;
	CRect m_rcPos;

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
	ULONG GetCurrentSaveVersion() const { return 4; }

// Operations
public:
	bool DeleteControl(long nIndex);

	void ClearR14Events();
	void IncrementPictureId(int nIdIncrement);
	int CountDeletedControls() const;
	bool CanWeDeleteForm() const;
	int CountDeletedImageLists() const;
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
	CDclControlObject* FindControl( LPCTSTR pszControlName, ControlTypes eType ) const;
	CDclControlObject* FindFirstControlOfType( ControlTypes eType ) const;
	bool FindControls( ControlTypes eType, CList< CDclControlObject* >& Results ) const;
	bool GetControlFonts( CFontCollection& Fonts ) const;
	void ZOrderFrontAddTabControls();
	void UpdateGlobalVariableName( LPCTSTR pszRootName = NULL );
	void ForceUpdateGlobalVariableName( LPCTSTR pszFormName );

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
	CList< CDclControlObject* >& GetControlList() { return mDclControls; }
	const CDclControlObject* GetControlProperties() const;
	CDclControlObject* GetControlProperties();
	UUID GetUUID() const;
	const CString& GetUUIDAsString() const { return msUUID; }
	const CString& GetUniqueName() const { return msUniqueName; }
	void SetUniqueName( LPCTSTR pszName ) { msUniqueName = pszName; }
	CString GetParentName() const { return mpParentForm? mpParentForm->GetUniqueName() : CString(); }
	const CDclFormObject* GetParentForm() const { return mpParentForm; }
	void SetParentForm( CDclFormObject* pParentForm );
	void SetParentForm( LPCTSTR pszParentUniqueName );
	short GetTabIndex() const { return mnTabIndex; }
	void SetTabIndex( short nIndex ) { mnTabIndex = nIndex; }

	//File I/O
public:
	virtual void Serialize(CArchive& ar);
  IOStatus WriteToTextFile(FILE* pFile, const CString &fileName) const;
  IOStatus ReadFromTextFile(std::ifstream &sFile, const CString &fileName);
  IOStatus ReadFromTextFile4(std::ifstream &sFile, const CString &fileName);
	
	// Name rendition
public:
	CString GetKeyName() const;
	CString GetKeyPath() const;

protected:
	DECLARE_SERIAL(CDclFormObject)
};
