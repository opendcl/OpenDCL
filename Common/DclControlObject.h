#pragma once


#include "PropertyList.h"
#include "DclFormObject.h"

class CImageListObject;
class CAxContainerCtrl;
class CDialogControl;
class CControlHolder;
enum IOStatus;
enum Prop::Id;


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
	bool mbDeleted;

public:

	//runtime state
	CControlHolder *m_pCtrlHolder;
	CAxContainerCtrl* m_pAxWnd;

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
	void SaveToStream(CAxContainerCtrl *pCtrl);
	IStream *GetLoadStream();
	void ClearStream();

protected:
	CDclControlObject();
	CDclControlObject(const CDclControlObject&); //declared protected to prevent copy construction
	CDclControlObject& operator=( const CDclControlObject& ); //declared protected to prevent copy construction
public:
	CDclControlObject(CDclFormObject* pOwner);
	CDclControlObject(ControlType type, CDclFormObject* pOwner, LPCTSTR pszName = NULL);
	virtual ~CDclControlObject();

	//2007-01-30 [ORW]: save version set to 6 (no change from OpenDCL 3)
	//2007-02-08 [ORW]: save version set to 7 (eliminated serialized MFC classes)
	//2007-06-16 [ORW]: save version set to 8 (removed purchase state)
	//2007-09-17 [ORW]: save version set to 9 (removed m_ClientHeight member)
	ULONG GetCurrentSaveVersion() const { return 9; }

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
	bool IsDeleted() const { return mbDeleted; }
	void SetDeleted( bool bDelete = true ) { mbDeleted = bDelete; }

protected: //for use by parent form only
	void SetID( int nID ) { mnID = nID; }
	void SetZOrder( int idxZOrder ) { midxZOrder = idxZOrder; }

// Operations
public:
	void SetGlobalVariableName( LPCTSTR pszParentName = NULL );
	void ClearGlobalVariableName();
	TPropertyPtr GetPropertyObject( Prop::Id nID ) const;
	TPropertyPtr FindPropertyObject( LPCTSTR pszName ) const;
	TPropertyPtr GetMethods() const;
	void RemoveProperty(Prop::Id nId);
	void ResetProperty(Prop::Id nId);
	size_t CountPropertyListItems(Prop::Id nID);
	CString GetPropertyListItem(Prop::Id nID, size_t nIndex);
	RefCountedPtr< CImageListObject > GetImageList() const { return mpImageList; }
	void SetImageList( RefCountedPtr< CImageListObject > pImageList ) { mpImageList = pImageList; }
	POSITION FindPropertyInsertPos( LPCTSTR pszName, bool bHidden ) const;
	POSITION FindPropertyInsertPos( Prop::Id nID, bool bHidden ) const;
	bool InsertNamedProperty( TPropertyPtr pProp );

	bool SetStringProperty( Prop::Id nID, LPCTSTR pszValue );
	TPropertyPtr AddStringProperty( Prop::Id nID,
																											PropertyType type = PropString,
																											LPCTSTR pszValue = NULL,
																											bool bResetExisting = false );
	bool SetBooleanProperty( Prop::Id nID, bool bValue = true );
	TPropertyPtr AddBooleanProperty( Prop::Id nID,
																											 PropertyType type = PropBool,
																											 bool bValue = true,
																											 bool bResetExisting = false );
	bool SetLongProperty( Prop::Id nID, long lValue = -1 );
	TPropertyPtr AddLongProperty( Prop::Id nID,
																										PropertyType type = PropLong,
																										long lValue = -1,
																										bool bResetExisting = false );

	CString GetStringProperty(Prop::Id nID) const;
	long GetLongProperty(Prop::Id nID) const;
	bool GetBooleanProperty(Prop::Id nID) const;
	void SetColorProperty(Prop::Id nID, COLORREF color);
	COLORREF GetColorProperty(Prop::Id nID) const;
	void ClearProperties();
	void ClearR14Events();

	//Implementation
	CString GetActiveXTypeName() const;
	bool IsMicrosoftActiveXCtrl() const;

	//File I/O
	virtual void Serialize(CArchive& ar);	
  IOStatus ReadFromTextFile(std::ifstream &sFile, const CString &fileName);
  IOStatus ReadFromTextFile6(std::ifstream &sFile, const CString &fileName);
  //IOStatus WriteToTextFile(FILE* pFile, const CString &fileName) const;
	
	//Name rendition
public:
	CString GetKeyName() const;
	CString GetKeyPath() const;
	ControlType GetType() const { return mType; }
	CString GetVarName() const;

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
