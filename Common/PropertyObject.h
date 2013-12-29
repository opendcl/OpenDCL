#pragma once

class CPropertyObject;
#include <vector>
#include "PropertyIds.h"
#include "PtrTypes.h"

class CDclControlObject;
class CUndoManager;
class AxPropertyDescriptor;
class AxEventDescriptor;
class AxMethodDescriptor;
class AxInterfaceDescriptor;
enum IOStatus;

enum PropertyType
{
	PropInvalid = -1,
	PropLong = 1,
	PropString = 2,
	PropDouble = 3,
	PropBool = 4,
	PropEnum = 5,
	PropEvent = 6,
	PropPicture = 7,
	PropCustom = 8,
	PropImageList = 9,
	PropOLEColor = 10,
	PropStringArray = 11,
	PropIntArray = 12,
	PropActiveXPropPages = 13,
	PropActiveXProp = 14,
	PropActiveXEnum = 15,
	PropActiveXEvent = 16,
	PropActiveXRunTime = 17,
	PropActiveXMethods = 18,
	PropStringArrayList = 19,
	PropIntArrayList = 20,
};


namespace PropVal
{
	typedef std::vector< CString > TCStringArray;
	typedef std::vector< TCStringArray > TCStringArrayList;
	typedef std::vector< int > TIntArray;
	typedef std::vector< TIntArray > TIntArrayList;


	class ConvertedCStringArray : public CStringArray
	{
	public:
		ConvertedCStringArray( const TCStringArray& Src )
		{
			for( size_t idx = 0; idx < Src.size(); idx++ )
				Add( Src[idx] );
		}
	};


	class CPropertyValueBase
	{
	protected:
		CPropertyObject* mpProperty;
	private:
		CPropertyValueBase();
		CPropertyValueBase( const CPropertyValueBase& );
	protected:
		friend class CPropertyObject;
		CPropertyValueBase( CPropertyObject* pProperty ) : mpProperty( pProperty ) {}
	public:
		virtual ~CPropertyValueBase() {}

		//attributes
		virtual PropertyType GetType() const = 0;
		virtual LPCTSTR GetName() const { return NULL; }
		virtual void SetName( LPCTSTR pszName ) { return; }
		virtual DWORD GetFlags() const { return 0; }
		virtual DWORD SetFlags(DWORD dwFlags) { return 0; }
		virtual bool IsReadOnly() const { return false; }

		//simple types
		virtual bool GetValue( short& v ) const { return false; }
		virtual bool SetValue( const short& v ) { return false; }
		virtual bool GetValue( unsigned short& v ) const { return false; }
		virtual bool SetValue( const unsigned short& v ) { return false; }
		virtual bool GetValue( long& v ) const { return false; }
		virtual bool SetValue( const long& v ) { return false; }
		virtual bool GetValue( unsigned long& v ) const { return false; }
		virtual bool SetValue( const unsigned long& v ) { return false; }
		virtual bool GetValue( bool& v ) const { return false; }
		virtual bool SetValue( const bool& v ) { return false; }
		virtual bool GetValue( double& v ) const { return false; }
		virtual bool SetValue( const double& v ) { return false; }
		virtual bool GetValue( CString& v ) const { return false; }
		virtual bool SetValue( const CString& v ) { return false; }
		virtual bool SetValue( const LPCTSTR v ) { return false; }
		//virtual bool GetValue( OLE_COLOR& v ) const { return false; }
		//virtual bool SetValue( const OLE_COLOR& v ) { return false; }

		//complex types
		virtual bool GetValue( TCStringArray* const*& v ) const { return false; }
		virtual bool GetValue( TCStringArray**& v ) { return false; }
		virtual bool GetValue( TCStringArrayList* const*& v ) const { return false; }
		virtual bool GetValue( TCStringArrayList**& v ) { return false; }
		virtual bool GetValue( TIntArray* const*& v ) const { return false; }
		virtual bool GetValue( TIntArray**& v ) { return false; }
		virtual bool GetValue( TIntArrayList* const*& v ) const { return false; }
		virtual bool GetValue( TIntArrayList**& v ) { return false; }
		virtual bool GetValue( AxInterfaceDescriptor* const*& v ) const { return false; }
		virtual bool GetValue( AxInterfaceDescriptor**& v ) { return false; }

		//operations
		virtual void clear() = 0;
		virtual size_t size() const { return 0; }

		//filing
		virtual IOStatus FileOut( CArchive& ar, BYTE nVersion ) const = 0;
		virtual IOStatus FileIn( CArchive& ar, BYTE nVersion ) = 0;
		//virtual IOStatus FileOut( FILE* pFile, BYTE nVersion ) const = 0;
		virtual IOStatus FileIn( std::ifstream &sFile, BYTE nVersion ) = 0;

	#ifdef _DIAGNOSTIC
	public:
		virtual LPCTSTR toString() const = 0;
	#endif
	};
};


/////////////////////////////////////////////////////////////////////////////
// CPropertyObject

class CPropertyObject
{
	RefCountedPtr< PropVal::CPropertyValueBase > mpValue;
	CDclControlObject* mpOwnerControl;

protected:
	bool mbHidden;
	Prop::Id mnID;
#ifdef _DEBUG
private:
	PropertyType mType; //used for inspecting type at runtime while debugging
#endif

	//Construction
protected:
	friend class RefCounter< CPropertyObject* >; //so that objects of this class are always reference counted
	CPropertyObject();
	virtual ~CPropertyObject();
public:
	CPropertyObject( CDclControlObject* pOwnerControl, PropertyType type, DWORD dwFlags = 0, Prop::Id nID = (Prop::Id)-1);

	//2007-01-30 [ORW]: save version set to 5 (no change from OpenDCL 3)
	//2007-02-08 [ORW]: save version set to 6 (eliminate MFC serialized classes)
	//2007-02-08 [ORW]: save version set to 7 (added subtype to filed data)
	//2008-10-16 [ORW]: save version set to 8 (fixed size_t filing bug -- now filing short)
	BYTE GetCurrentSaveVersion() const { return 8; }

	//Attributes
protected:
	RefCountedPtr< PropVal::CPropertyValueBase > GetValue() const { return mpValue; }
	void OnChanging() const;
public:
	CUndoManager* GetUndoManager() const;
	PropertyType GetType() const { return (mpValue? mpValue->GetType() : PropInvalid); }
	void SetType( PropertyType type );
	DWORD GetFlags() const { return (mpValue? mpValue->GetFlags() : 0); }
	void SetFlags( DWORD flags ) { if( mpValue ) { OnChanging(); mpValue->SetFlags( flags ); } }
	bool IsHidden() const { return mbHidden; }
	void SetHidden( bool bHidden = true ) { OnChanging(); mbHidden = bHidden; }
	Prop::Id GetID() const { return mnID; }
	void SetID( Prop::Id nID ) { OnChanging(); mnID = nID; }
	bool IsReadOnly() const { return (mpValue? mpValue->IsReadOnly() : false); }
	TDclControlPtr GetOwnerControl() const;

	//simple value access
	CString GetStringValue() const;
	bool SetStringValue( LPCTSTR pszValue );
	OLE_COLOR GetOLEColorValue() const;
	bool SetOLEColorValue( const OLE_COLOR& dwColor );
	long GetLongValue() const;
	bool SetLongValue( long lValue );
	long GetEnumValue() const { return GetLongValue(); }
	bool SetEnumValue( int nValue ) { return SetLongValue( (long)nValue ); }
	bool GetBooleanValue() const;
	bool SetBooleanValue( bool bValue );
	double GetDoubleValue() const;
	bool SetDoubleValue( double dblValue );
	short GetShortValue() const;
	bool SetShortValue( short idxValue );

	//complex value access
	const PropVal::TCStringArray* GetConstStringArrayPtr() const;
	PropVal::TCStringArray* GetStringArrayPtr();
	const PropVal::TCStringArrayList* GetConstStringArrayListPtr() const;
	PropVal::TCStringArrayList* GetStringArrayListPtr();
	const PropVal::TIntArray* GetConstIntArrayPtr() const;
	PropVal::TIntArray* GetIntArrayPtr();
	const PropVal::TIntArrayList* GetConstIntArrayListPtr() const;
	PropVal::TIntArrayList* GetIntArrayListPtr();
	const AxInterfaceDescriptor* GetConstAxInterfaceDescriptorPtr() const;
	AxInterfaceDescriptor* GetAxInterfaceDescriptorPtr();

	//Operations
public:
	void clear() { if( mpValue ) { OnChanging(); mpValue->clear(); } }
	size_t size() const;
	
	//Implementation
public:
	virtual CString GetApiName() const;
	virtual CString GetName() const;
	virtual void SetName( LPCTSTR pszName );
	CString GetDocumentationDesc() const;
	
	void AddStringItem( LPCTSTR pszItem );
	CString GetStringItem( size_t idxItem );

	//File I/O
public:
	virtual void Serialize(CArchive& ar);
	IOStatus ReadFromTextFile(std::ifstream &sFile);
	IOStatus ReadFromTextFile5(std::ifstream &sFile);
	//IOStatus WriteToTextFile(FILE* pFile) const;

#ifdef _DIAGNOSTIC
public:
	virtual LPCTSTR toString() const;
#endif
};
