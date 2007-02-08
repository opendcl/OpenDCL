#pragma once


#include "PropertyList.h"
#include "DclFormObject.h"

class CImageListObject;
class CAxContainer;
class CDialogControl;
enum IOStatus;
enum PropertyId;


/////////////////////////////////////////////////////////////////////////////
// CDclControlObject

class CDclControlObject : public CObject
{
// Attributes
protected:
	CDclFormObject* mpOwner;
	ControlType mType;
	CDialogControl* mpDlgControl; //informational pointer to the one and only instance of this control (or NULL)
	CString msAxTypeName; //this should be moved to AxContainer -- unfortunately it's filed from here [ORW]
	CPropertyList mProperties;

public:
	int m_Index;
	int m_Id;

	//runtime state
	BOOL m_Delete;
	short m_ClientHeight;
	int m_PurchaseState;
	CWnd *m_pCtrlHolder;
	CRect m_rcOldPosition;
	CImageListObject *m_pImageList;
	CAxContainer* m_pAxWnd;

	//ARX only
	bool m_bEventsAsAction;

	// ActiveX members
	BOOL	m_bLicenseChecked;
	CString m_sLicenseKey;
	CString m_sBaseCode;
	CLSID   m_clsid;
	ULONG	m_nTotalBytes;

	//storage stream
	CComPtr<IStream> m_pStream;
	void SaveToStream(CAxContainer *pCtrl);
	IStream *GetLoadStream();
	void ClearStream();

// position pointers
public:
	RefCountedPtr< CPropertyObject > m_pVisible;

	RefCountedPtr< CPropertyObject > m_pLeft;
	RefCountedPtr< CPropertyObject > m_pTop;
	RefCountedPtr< CPropertyObject > m_pWidth;
	RefCountedPtr< CPropertyObject > m_pHeight;

	RefCountedPtr< CPropertyObject > m_pOffsetLeft;
	RefCountedPtr< CPropertyObject > m_pOffsetTop;
	RefCountedPtr< CPropertyObject > m_pOffsetRight;
	RefCountedPtr< CPropertyObject > m_pOffsetBottom;

	RefCountedPtr< CPropertyObject > m_pUseLeftOffset;
	RefCountedPtr< CPropertyObject > m_pUseTopOffset;
	RefCountedPtr< CPropertyObject > m_pUseRightOffset;
	RefCountedPtr< CPropertyObject > m_pUseBottomOffset;

protected:
	CDclControlObject();
	CDclControlObject(const CDclControlObject& other); //declared protected to prevent copy construction
public:
	CDclControlObject(CDclFormObject* pOwner);
	CDclControlObject(ControlType type, CDclFormObject* pOwner, LPCTSTR pszName = NULL);
	virtual ~CDclControlObject();

	//2007-01-30 [ORW]: save version set to 6 (no change from ObjectDCL 3)
	ULONG GetCurrentSaveVersion() const { return 6; }

// Operations
public:
	CWnd* GetWindow() const;
	const CDialogControl* GetControlInstance() const { return mpDlgControl; }
	CDialogControl* GetControlInstance() { return mpDlgControl; }
	void SetControlInstance( CDialogControl* pDlgControl );
	const CDclFormObject* GetOwner() const { return mpOwner; }
	CDclFormObject* GetOwner() { return mpOwner; }
	void SetOwner( CDclFormObject* pNewOwner ) { assert( pNewOwner != NULL ); mpOwner = pNewOwner; }
	const CPropertyList& GetPropertyList() const { return mProperties; }
	CPropertyList& GetPropertyList() { return mProperties; }
	//CDclControlObject(CDclControlObject const & other);
	//virtual CDclControlObject operator=(CDclControlObject const & other);
	POSITION FindPropertyInsertPos(CString sName, bool bHidden) const;
	const CString& GetAxTypeName() const { return msAxTypeName; }
	void SetAxTypeName( LPCTSTR pszAxTypeName ) { msAxTypeName = pszAxTypeName; }

// Implementation
public:
	bool UpdateGlobalVariable(CString sDclFormName, LPCTSTR pszProjectName = NULL);
	void ForceUpdateGlobalVariable(CString sDclFormName);
	RefCountedPtr< CPropertyObject > FindProperty(CString sName);
	void RemoveProperty(int nId);
	void ResetProperty(int nId);
	INT_PTR CountPropertyListItems(int nID);
	CString GetPropertyListItem(int nID, int nIndex);
	long GetImageListIndex();
	void SetImageListIndex(int nIndex);
	//void Copy(CDclControlObject *other);
	short FindPropertyIndex(int nID) const;
	POSITION FindPropertyInsertPos(int nID, bool bHidden) const;
	CString GetStrProperty(int nID) const;
	void SetStrProperty(int nID, CString sValue);
	long GetLngProperty(int nID) const;
	void SetLngProperty(int nID, long lValue);	
	void SetBoolProperty(int nID, bool bValue);
	bool GetBoolProperty(int nID) const;
	void SetColorProperty(int nID, COLORREF color);
	COLORREF GetColorProperty(int nID) const;
	RefCountedPtr< CPropertyObject > GetPropertyObject(PropertyId nID) const;
	RefCountedPtr< CPropertyObject > GetActiveXPropertyObject(CString sName) const;
	void ClearProperties();
	void ClearR14Events();

	//Attributes
	virtual CDclFormObject* GetOwnerForm() { return mpOwner; }
	virtual const CDclFormObject* GetOwnerForm() const { return mpOwner; }
	virtual CProject* GetOwnerProject() { return mpOwner? mpOwner->GetProject() : NULL; }
	virtual const CProject* GetOwnerProject() const { return mpOwner? mpOwner->GetProject() : NULL; }
	CString GetActiveXTypeName() const;
	bool IsMicrosoftActiveXCtrl() const;

	//File I/O
	virtual void Serialize(CArchive& ar);	
  IOStatus WriteToTextFile(FILE* pFile, const CString &fileName) const;
  IOStatus ReadFromTextFile(std::ifstream &sFile, const CString &fileName);
  IOStatus ReadFromTextFile6(std::ifstream &sFile, const CString &fileName);
	
// Name rendition
public:
	CString GetKeyName() const;
	CString GetKeyPath() const;
	ControlType GetType() const { return mType; }

protected:
	DECLARE_SERIAL(CDclControlObject)
};
