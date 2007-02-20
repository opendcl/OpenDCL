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
	friend class CDclFormObject;

// Attributes
protected:
	CDclFormObject* mpOwner;
	ControlType mType;
	CDialogControl* mpDlgControl; //informational pointer to the one and only instance of this control (or NULL)
	CString msAxTypeName; //this should be moved to AxContainer -- unfortunately it's filed from here [ORW]
	CPropertyList mProperties;
	RefCountedPtr< CImageListObject > mpImageList;
	int mnID;
	int midxZOrder;

public:

	//runtime state
	BOOL m_Delete;
	short m_ClientHeight;
	int m_PurchaseState;
	CWnd *m_pCtrlHolder;
	CRect m_rcOldPosition;
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
	CDclControlObject(const CDclControlObject&); //declared protected to prevent copy construction
	CDclControlObject& operator=( const CDclControlObject& ); //declared protected to prevent copy construction
public:
	CDclControlObject(CDclFormObject* pOwner);
	CDclControlObject(ControlType type, CDclFormObject* pOwner, LPCTSTR pszName = NULL);
	virtual ~CDclControlObject();

	//2007-01-30 [ORW]: save version set to 6 (no change from ObjectDCL 3)
	//2007-02-08 [ORW]: save version set to 7 (eliminated serialized MFC classes)
	ULONG GetCurrentSaveVersion() const { return 7; }

	//Attributes
public:
	CWnd* GetWindow() const;
	const CDialogControl* GetControlInstance() const { return mpDlgControl; }
	CDialogControl* GetControlInstance() { return mpDlgControl; }
	void SetControlInstance( CDialogControl* pDlgControl );
	virtual CDclFormObject* GetOwnerForm() { return mpOwner; }
	virtual const CDclFormObject* GetOwnerForm() const { return mpOwner; }
	void SetOwnerForm( CDclFormObject* pNewOwner ) { assert( pNewOwner != NULL ); mpOwner = pNewOwner; }
	virtual CProject* GetOwnerProject() { return mpOwner? mpOwner->GetProject() : NULL; }
	virtual const CProject* GetOwnerProject() const { return mpOwner? mpOwner->GetProject() : NULL; }
	const CPropertyList& GetPropertyList() const { return mProperties; }
	CPropertyList& GetPropertyList() { return mProperties; }
	const CString& GetAxTypeName() const { return msAxTypeName; }
	void SetAxTypeName( LPCTSTR pszAxTypeName ) { msAxTypeName = pszAxTypeName; }
	int GetID() const { return mnID; }
	int GetZOrder() const { return midxZOrder; }

protected: //for use by parent form only
	void SetID( int nID ) { mnID = nID; }
	void SetZOrder( int idxZOrder ) { midxZOrder = idxZOrder; }

// Operations
public:
	void SetGlobalVariableName( LPCTSTR pszParentName = NULL );
	void ClearGlobalVariableName();
	RefCountedPtr< CPropertyObject > GetPropertyObject( PropertyId nID ) const;
	RefCountedPtr< CPropertyObject > FindPropertyObject( LPCTSTR pszName ) const;
	RefCountedPtr< CPropertyObject > GetMethods() const;
	void RemoveProperty(PropertyId nId);
	void ResetProperty(PropertyId nId);
	size_t CountPropertyListItems(PropertyId nID);
	CString GetPropertyListItem(PropertyId nID, size_t nIndex);
	RefCountedPtr< CImageListObject > GetImageList() const { return mpImageList; }
	void SetImageList( RefCountedPtr< CImageListObject > pImageList ) { mpImageList = pImageList; }
	short FindPropertyIndex(PropertyId nID) const;
	POSITION FindPropertyInsertPos( LPCTSTR pszName, bool bHidden ) const;
	POSITION FindPropertyInsertPos( PropertyId nID, bool bHidden ) const;
	bool InsertNamedProperty( RefCountedPtr< CPropertyObject > pProp );

	bool SetStringProperty( PropertyId nID, LPCTSTR pszValue );
	RefCountedPtr< CPropertyObject > AddStringProperty( PropertyId nID,
																											PropertyType type = PropString,
																											LPCTSTR pszValue = NULL,
																											bool bResetExisting = false );
	bool SetBooleanProperty( PropertyId nID, bool bValue = true );
	RefCountedPtr< CPropertyObject > AddBooleanProperty( PropertyId nID,
																											 PropertyType type = PropBool,
																											 bool bValue = true,
																											 bool bResetExisting = false );
	bool SetLongProperty( PropertyId nID, long lValue = -1 );
	RefCountedPtr< CPropertyObject > AddLongProperty( PropertyId nID,
																										PropertyType type = PropLong,
																										long lValue = -1,
																										bool bResetExisting = false );

	CString GetStrProperty(PropertyId nID) const;
	long GetLngProperty(PropertyId nID) const;
	bool GetBoolProperty(PropertyId nID) const;
	void SetColorProperty(PropertyId nID, COLORREF color);
	COLORREF GetColorProperty(PropertyId nID) const;
	void ClearProperties();
	void ClearR14Events();

	//Implementation
	CString GetActiveXTypeName() const;
	bool IsMicrosoftActiveXCtrl() const;

	//File I/O
	virtual void Serialize(CArchive& ar);	
  IOStatus WriteToTextFile(FILE* pFile, const CString &fileName) const;
  IOStatus ReadFromTextFile(std::ifstream &sFile, const CString &fileName);
  IOStatus ReadFromTextFile6(std::ifstream &sFile, const CString &fileName);
	
	//Name rendition
public:
	CString GetKeyName() const;
	CString GetKeyPath() const;
	ControlType GetType() const { return mType; }

protected:
	DECLARE_SERIAL(CDclControlObject)

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
