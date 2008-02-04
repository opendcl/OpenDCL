#pragma once


#include "Project.h"
#include "FontSettings.h"
#include "ImageListObject.h"
#include <list>

class CAxContainerCtrl;
class CDclFormObject;
class CDialogControl;
enum IOStatus;
enum Prop::Id;

typedef std::list< TPropertyPtr > TPropertyList;


/////////////////////////////////////////////////////////////////////////////
// CDclControlObject

class CDclControlObject : public CObject
{
protected:
	CDclFormObject* mpOwner;
	CDialogControl* mpDlgControl; //informational pointer to the one and only instance of this control (or NULL)
	ControlType mType;

private:
	CString msAxTypeName; //this should be moved to AxContainer -- unfortunately it's filed from here [ORW]
	TPropertyList mProperties;
	RefCountedPtr< CImageListObject > mpImageList;
	int mnID;

public:
	// ActiveX members
	CString m_sLicenseKey;
	CLSID   m_clsid;

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
	//2008-01-12 [ORW]: save version set to 10 (removed m_bLicenseChecked, m_sBaseCode members)
	ULONG GetCurrentSaveVersion() const { return 10; }

// Attributes
public:
	CUndoManager* GetUndoManager() const;
	CWnd* GetWindow() const;
	const CDialogControl* GetControlInstance() const { return mpDlgControl; }
	CDialogControl* GetControlInstance() { return mpDlgControl; }
	virtual TDclFormPtr GetOwnerForm();
	virtual const TDclFormPtr GetOwnerForm() const;
	void SetOwnerForm( CDclFormObject* pNewOwner ) { assert( pNewOwner != NULL ); mpOwner = pNewOwner; }
	virtual TProjectPtr GetOwnerProject();
	virtual const TProjectPtr GetOwnerProject() const;
	const TPropertyList& GetPropertyList() const { return mProperties; }
	TPropertyList& GetPropertyList() { return mProperties; }
	RefCountedPtr< CImageListObject > GetImageList() const { return mpImageList; }
	void SetImageList( RefCountedPtr< CImageListObject > pImageList ) { mpImageList = pImageList; }
	const CString& GetAxTypeName() const { return msAxTypeName; }
	void SetAxTypeName( LPCTSTR pszAxTypeName ) { msAxTypeName = pszAxTypeName; }
	int GetID() const { return mnID; }
	virtual bool IsZOrderAllowed() const;

protected: //for use by parent form only
	friend class CDclFormObject;
	void SetID( int nID ) { mnID = nID; }

protected: //for use by control instance only
	friend class CDialogControl;
	void SetControlInstance( CDialogControl* pDlgControl );

// Operations
public:
	void SetGlobalVariableName( LPCTSTR pszParentName = NULL );
	void ClearGlobalVariableName();

// Property access
public:
	TPropertyPtr GetRefCountedPtr( const CPropertyObject* pProperty ) const;
	const TPropertyPtr GetPropertyObject( Prop::Id nID ) const;
	TPropertyPtr GetPropertyObject( Prop::Id nID );
	TPropertyPtr FindPropertyObject( LPCTSTR pszName ) const;
	bool InsertNamedProperty( TPropertyPtr pProp );
	void RemoveProperty( Prop::Id nId );
	void ResetProperty( Prop::Id nId );
	TPropertyPtr GetMethods() const;
	size_t CountPropertyListItems( Prop::Id nID );
	CString GetPropertyListItem( Prop::Id nID, size_t nIndex );
	CRect GetWndRect() const;
	void SetFontProperties( const FontSettings& FS );

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

// Implementation
public:
	CString GetActiveXTypeName() const;
	bool IsMicrosoftActiveXCtrl() const;

protected:
	TPropertyList::iterator FindPropertyInsertPos( LPCTSTR pszName, bool bHidden );
	TPropertyList::iterator FindPropertyInsertPos( Prop::Id nID, bool bHidden );

// File I/O
public:
	virtual void Serialize(CArchive& ar);	
  IOStatus ReadFromTextFile(std::ifstream &sFile, const CString &fileName);
  IOStatus ReadFromTextFile6(std::ifstream &sFile, const CString &fileName);
  //IOStatus WriteToTextFile(FILE* pFile, const CString &fileName) const;
	
// Name rendition
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
