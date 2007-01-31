#pragma once


#include "PropertyList.h"
#include "DclFormObject.h"

class CImageListObject;
class CAxContainer;
enum IOStatus;


/////////////////////////////////////////////////////////////////////////////
// CDclControlObject

class CDclControlObject : public CObject
{
public:
	typedef CList< CDclControlObject* > collection;

// Attributes
protected:
	CDclFormObject* mpOwner;
	ControlTypes mType;

public:
	CPropertyList m_PropertyList;
	int m_Id;
	int m_Index;
	CString m_Name;
	BOOL m_Delete;
	short m_ClientHeight;
	int m_PurchaseState;
	bool m_bControlCreated;
	CWnd *m_pCtrlHolder;
	CRect m_rcOldPosition;
	CImageListObject *m_pImageList;
	CAxContainer* m_pAxWnd;

	//ARX only
	bool m_bEventsAsAction;
	CWnd* m_pWnd;
	CString m_sProjectName;
	CString m_sDialogName;

	// ActiveX members
	BOOL	m_bLicenseChecked;
	CString m_sLicenseKey;
	CString m_sBaseCode;
	CLSID   m_clsid;
	ULONG	m_nTotalBytes;
	CString m_AxTypeName;
	int	m_nIsMicrosoftActiveX;
	
	CString GetActiveXTypeName() const;
	BOOL IsMicrosoftActiveXCtrl() const;

	//storage stream
	CComPtr<IStream> m_pStream;
	void SaveToStream(CAxContainer *pCtrl);
	IStream *GetLoadStream();
	void ClearStream();

// position pointers
public:
	CPropertyObject *m_pVisible;

	CPropertyObject *m_pLeft;
	CPropertyObject *m_pTop;
	CPropertyObject *m_pWidth;
	CPropertyObject *m_pHeight;

	CPropertyObject *m_pOffsetLeft;
	CPropertyObject *m_pOffsetTop;
	CPropertyObject *m_pOffsetRight;
	CPropertyObject *m_pOffsetBottom;

	CPropertyObject *m_pUseLeftOffset;
	CPropertyObject *m_pUseTopOffset;
	CPropertyObject *m_pUseRightOffset;
	CPropertyObject *m_pUseBottomOffset;

protected:
	CDclControlObject();
	CDclControlObject(const CDclControlObject& other); //declared protected to prevent copy construction
public:
	CDclControlObject(CDclFormObject* pOwner);
	CDclControlObject(ControlTypes type, CDclFormObject* pOwner, LPCTSTR pszName = NULL);

	//2007-01-30 [ORW]: save version set to 6 (no change from ObjectDCL 3)
	ULONG GetCurrentSaveVersion() const { return 6; }

// Operations
public:
	//CDclControlObject(CDclControlObject const & other);
	//virtual CDclControlObject operator=(CDclControlObject const & other);
	POSITION FindPropertyInsertPos(CString sName, bool bHidden) const;

// Implementation
public:
	bool UpdateGlobalVariable(CString sDclFormName, LPCTSTR pszProjectName = NULL);
	void ForceUpdateGlobalVariable(CString sDclFormName);
	CPropertyObject* FindProperty(CString sName);
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
	CPropertyObject* GetPropertyObject(int nID) const;
	CPropertyObject* GetActiveXPropertyObject(CString sName) const;
	void ClearProperties();
	void ClearR14Events();
	virtual ~CDclControlObject();

	//Attributes
	virtual CDclFormObject* GetOwnerForm() { return mpOwner; }
	virtual const CDclFormObject* GetOwnerForm() const { return mpOwner; }
	virtual CProject* GetOwnerProject() { return mpOwner? mpOwner->GetProject() : NULL; }
	virtual const CProject* GetOwnerProject() const { return mpOwner? mpOwner->GetProject() : NULL; }

	//File I/O
	virtual void Serialize(CArchive& ar);	
  IOStatus WriteToTextFile(FILE* pFile, const CString &fileName) const;
  IOStatus ReadFromTextFile(std::ifstream &sFile, const CString &fileName);
  IOStatus ReadFromTextFile6(std::ifstream &sFile, const CString &fileName);
	
// Name rendition
public:
	CString GetKeyName() const;
	CString GetKeyPath() const;
	ControlTypes GetType() const { return mType; }

protected:
	DECLARE_SERIAL(CDclControlObject)
};
