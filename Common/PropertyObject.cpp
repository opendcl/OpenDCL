// PropertyObject.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyObject.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxMethodDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "AxContainer.h"
#include "Filing.h"
#include "OleFont.h"
#include "VarUtils.h"
#include "Workspace.h"
#include "Project.h"
#include "SharedRes.h"
#include "PropertyIds.h"

static const int nNotSet = -1;
static const int nDePropDescResStringOffset = 2100;


namespace PropVal
{

class CPropertyValue : public CPropertyValueBase
{
	DWORD mdwSubtype;
protected:
	friend class CPropertyObject;
	CPropertyValue() : mdwSubtype( 0 ) {}
public:
	virtual ~CPropertyValue() {}

	//attributes
	virtual PropertyType GetType() const = 0;
	virtual DWORD SetSubtype( DWORD dwFlags ) { DWORD dwOld = mdwSubtype; mdwSubtype = dwFlags; return dwOld; }
	virtual DWORD GetSubtype() const { return mdwSubtype; }

	//operations
	virtual void clear() = 0;

	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const = 0;
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion ) = 0;
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const = 0;
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion ) = 0;

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toString() const
		{
			static TCHAR buf[1024];
			_sntprintf( buf, _elements(buf), _T("[%s/%s] %s"), asString( GetType() ), asString( mdwSubtype ), toStringPropVal() );
			return buf;
		}
	virtual LPCTSTR toStringPropVal() const = 0;
#endif
};

class CNamedPropertyValue : public CPropertyValueBase
{
protected:
	CString msDisplayName; //this is the name displayed in the property grid for complex property values
protected:
	friend class CPropertyObject;
	CNamedPropertyValue() : CPropertyValueBase() {}
public:
	virtual ~CNamedPropertyValue() {}

	//operations
	virtual void clear() { msDisplayName.Empty(); }

	//simple types
	virtual bool GetValue( CString& v ) const { v = msDisplayName; return true; }
	virtual bool SetValue( const CString& v ) { msDisplayName = v; return true; }
	virtual bool SetValue( const LPCTSTR v ) { msDisplayName = v; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toString() const
		{
			static TCHAR buf[1024];
			_sntprintf( buf, _elements(buf), _T("[%s/%s/\"%s\"] %s"), asString( GetType() ), asString( GetSubtype() ), (LPCTSTR)msDisplayName, toStringPropVal() );
			return buf;
		}
	virtual LPCTSTR toStringPropVal() const = 0;
#endif
};

}; //namespace PropVal



class CPropertyValueInvalid : public PropVal::CPropertyValue
{
	friend class CPropertyObject;
protected:
	CPropertyValueInvalid() {}
public:
	virtual PropertyType GetType() const { return PropInvalid; }
	virtual void clear() {}
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const { return statOK; }
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion ) { return statOK; }
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const { fprintf(pFile, "PropInvalid"); return statOK; }
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion ) { return statOK; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return _T("<void value>"); }
#endif
};

class CPropertyValueLong : public PropVal::CPropertyValue
{
	long mValue;
	friend class CPropertyObject;
protected:
	CPropertyValueLong() : mValue( -1 ) {}
public:
	virtual PropertyType GetType() const { return PropLong; }
	virtual void clear() { mValue = -1; }

	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const { ar << mValue; return statOK; }
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion ) { ar >> mValue; return statOK; }
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropLong");
			writeLong( pFile, mValue );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			return readLong( sFile, mValue )? statOK : statInvalidFormat;
		}

	virtual bool GetValue( long& v ) const { v = mValue; return true; }
	virtual bool SetValue( const long& v ) { mValue = v; return true; }
	virtual bool GetValue( unsigned long& v ) const { v = (unsigned long)mValue; return true; }
	virtual bool SetValue( const unsigned long& v ) { mValue = (long)v; return true; }
	virtual bool GetValue( CString& v ) const { v.Format(_T("%d"), mValue); return true; }
	virtual bool SetValue( const CString& v ) { mValue = _tstol(v); return true; }
	virtual bool SetValue( const LPCTSTR v ) { mValue = _tstol(v); return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return asString( mValue ); }
#endif
};

class CPropertyValueString : public PropVal::CPropertyValue
{
	CString mValue;
	friend class CPropertyObject;
protected:
	CPropertyValueString() : mValue() {}
public:
	virtual PropertyType GetType() const { return PropString; }
	virtual void clear() { mValue.Empty(); }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const { ar << mValue; return statOK; }
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion ) { ar >> mValue; return statOK; }
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropString");
			writeString( pFile, mValue );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			return readString( sFile, mValue )? statOK : statInvalidFormat;
		}

	virtual bool GetValue( long& v ) const { v = _tstol(mValue); return true; }
	virtual bool SetValue( const long& v ) { mValue.Format(_T("%d"), v); return true; }
	virtual bool GetValue( CString& v ) const { v = mValue; return true; }
	virtual bool SetValue( const CString& v ) { mValue = v; return true; }
	virtual bool SetValue( const LPCTSTR v ) { mValue = v; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const
		{
			static TCHAR buf[1024];
			_sntprintf( buf, _elements(buf), _T("\"%s\""), (LPCTSTR)mValue );
			return buf;
		}
#endif
};

class CPropertyValueDouble : public PropVal::CPropertyValue
{
	double mValue;
	friend class CPropertyObject;
protected:
	CPropertyValueDouble() : mValue( 0 ) {}
public:
	virtual PropertyType GetType() const { return PropDouble; }
	virtual void clear() { mValue = 0; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const { ar << mValue; return statOK; }
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion ) { ar >> mValue; return statOK; }
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropDouble");
			writeDouble( pFile, mValue );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			return readDouble( sFile, mValue )? statOK : statInvalidFormat;
		}

	virtual bool GetValue( double& v ) const { v = mValue; return true; }
	virtual bool SetValue( const double& v ) { mValue = v; return true; }
	virtual bool GetValue( CString& v ) const { v.Format(_T("%f"), mValue); return true; }
	virtual bool SetValue( const CString& v ) { mValue = _tstof(v); return true; }
	virtual bool SetValue( const LPCTSTR v ) { mValue = _tstof(v); return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return asString( mValue ); }
#endif
};

class CPropertyValueBool : public PropVal::CPropertyValue
{
	bool mValue;
	friend class CPropertyObject;
protected:
	CPropertyValueBool() : mValue( false ) {}
public:
	virtual PropertyType GetType() const { return PropBool; }
	virtual void clear() { mValue = false; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const
	{
		BOOL bVal = mValue? TRUE : FALSE;
		ar << bVal;
		return statOK;
	}
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion )
	{
		BOOL bVal;
		ar >> bVal;
		mValue = (bVal != FALSE);
		return statOK;
	}
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropBool");
			writeBool( pFile, mValue );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			return readBool( sFile, mValue )? statOK : statInvalidFormat;
		}

	virtual bool GetValue( bool& v ) const { v = mValue; return true; }
	virtual bool SetValue( const bool& v ) { mValue = v; return true; }
	virtual bool GetValue( long& v ) const { v = mValue? 1 : 0; return true; }
	virtual bool GetValue( CString& v ) const { v = (mValue? _T("True") : _T("False")); return true; }
	virtual bool SetValue( const CString& v ) { mValue = (v.CompareNoCase(_T("True")) == 0); return true; }
	virtual bool SetValue( const LPCTSTR v ) { mValue = (lstrcmpi(v, _T("True")) == 0); return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return asString( mValue ); }
#endif
};

class CPropertyValueEnum : public PropVal::CPropertyValue
{
	long mValue;
	friend class CPropertyObject;
protected:
	CPropertyValueEnum() : mValue( -1 ) {}
public:
	virtual PropertyType GetType() const { return PropEnum; }
	virtual void clear() { mValue = -1; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const { ar << mValue; return statOK; }
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion ) { ar >> mValue; return statOK; }
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropEnum");
			writeLong( pFile, mValue );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			return readLong( sFile, mValue )? statOK : statInvalidFormat;
		}

	virtual bool GetValue( long& v ) const { v = mValue; return true; }
	virtual bool SetValue( const long& v ) { mValue = v; return true; }
	virtual bool GetValue( CString& v ) const { v.Format(_T("%d"), mValue); return true; }
	virtual bool SetValue( const CString& v ) { mValue = _tstol(v); return true; }
	virtual bool SetValue( const LPCTSTR v ) { mValue = _tstol(v); return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return asString( mValue ); }
#endif
};

class CPropertyValueEvent : public PropVal::CPropertyValue
{
	CString mValue;
	friend class CPropertyObject;
protected:
	CPropertyValueEvent() {}
public:
	virtual PropertyType GetType() const { return PropEvent; }
	virtual void clear() { mValue.Empty(); }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const { ar << mValue; return statOK; }
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion ) { ar >> mValue; return statOK; }
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropEvent");
			writeString( pFile, mValue );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			return readString( sFile, mValue )? statOK : statInvalidFormat;
		}

	virtual bool GetValue( CString& v ) const { v = mValue; return true; }
	virtual bool SetValue( const CString& v ) { mValue = v; return true; }
	virtual bool SetValue( const LPCTSTR v ) { mValue = v; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return (LPCTSTR)mValue; }
#endif
};

class CPropertyValuePicture : public PropVal::CPropertyValue
{
	long mValue;
	friend class CPropertyObject;
protected:
	CPropertyValuePicture() : mValue( -1 ) {}
public:
	virtual PropertyType GetType() const { return PropPicture; }
	virtual void clear() { mValue = -1; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const { ar << mValue; return statOK; }
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion ) { ar >> mValue; return statOK; }
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropPicture");
			writeLong( pFile, mValue );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			return readLong( sFile, mValue )? statOK : statInvalidFormat;
		}

	virtual bool GetValue( long& v ) const { v = mValue; return true; }
	virtual bool SetValue( const long& v ) { mValue = v; return true; }
	virtual bool GetValue( CString& v ) const { v.Format(_T("%d"), mValue); return true; }
	virtual bool SetValue( const CString& v ) { mValue = _tstol(v); return true; }
	virtual bool SetValue( const LPCTSTR v ) { mValue = _tstol(v); return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return asString( mValue ); }
#endif
};

class CPropertyValueCustom : public PropVal::CPropertyValue
{
	friend class CPropertyObject;
protected:
	CPropertyValueCustom() {}
public:
	virtual PropertyType GetType() const { return PropCustom; }
	virtual void clear() {}
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const { return statOK; }
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion ) { return statOK; }
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const { fprintf(pFile, "PropCustom"); return statOK; }
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion ) { return statOK; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return _T("<Custom>"); }
#endif
};

class CPropertyValueImageList : public PropVal::CNamedPropertyValue
{
	short mValue;
	friend class CPropertyObject;
protected:
	CPropertyValueImageList() : mValue( -1 ) {}
public:
	virtual PropertyType GetType() const { return PropImageList; }
	virtual void clear() { PropVal::CNamedPropertyValue::clear(); mValue = -1; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const { ar << mValue; return statOK; }
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion ) { ar >> mValue; return statOK; }
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropImageList");
			writeShort( pFile, mValue );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			return readShort( sFile, mValue )? statOK : statInvalidFormat;
		}

	virtual bool GetValue( short& v ) const { v = short(mValue); return true; }
	virtual bool SetValue( const short& v ) { mValue = v; return true; }
	virtual bool GetValue( long& v ) const { v = mValue; return true; }
	virtual bool SetValue( const long& v ) { mValue = short(v); return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return asString( mValue ); }
#endif
};

class CPropertyValueOLEColor : public PropVal::CPropertyValue
{
	OLE_COLOR mValue;
	friend class CPropertyObject;
protected:
	CPropertyValueOLEColor() {}
public:
	virtual PropertyType GetType() const { return PropOLEColor; }
	virtual void clear() { mValue = -1; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const { ar << mValue; return statOK; }
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion ) { ar >> mValue; return statOK; }
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropOLEColor");
			writeDWORD( pFile, mValue );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			return readDWORD( sFile, mValue )? statOK : statInvalidFormat;
		}

	virtual bool GetValue( unsigned long& v ) const { v = mValue; return true; }
	virtual bool SetValue( const unsigned long& v ) { mValue = v; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return asString( mValue ); }
#endif
};

class CPropertyValueStringArray : public PropVal::CNamedPropertyValue
{
	PropVal::TCStringArray* mpValue;
	friend class CPropertyObject;
protected:
	CPropertyValueStringArray() : mpValue( NULL ) {}
	~CPropertyValueStringArray() { delete mpValue; }
public:
	virtual PropertyType GetType() const { return PropStringArray; }
	virtual void clear() { PropVal::CNamedPropertyValue::clear(); delete mpValue; mpValue = NULL; }
	virtual size_t size() const { return mpValue? mpValue->size() : 0; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const
		{ //changing from CStringList to std::vector< CString > in version 6 [ORW]
			if (nVersion <= 5)
			{
				CStringList listStr;
				unsigned long nSize = mpValue? mpValue->size() : 0;
				for(size_t idx = 0; idx < nSize; ++idx)
					listStr.AddTail( mpValue->at(idx) );
				listStr.Serialize(ar);
			}
			else
			{
				unsigned long nSize = mpValue? mpValue->size() : 0;
				ar << unsigned long(nSize);
				for(size_t idx = 0; idx < nSize; ++idx)
					ar << CString(mpValue->at(idx));
			}
			return statOK;
		}
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion )
		{ //changing from CStringList to std::vector< CString > in version 6 [ORW]
			clear();
			if (nVersion <= 5)
			{
				CStringList listStr;
				listStr.Serialize(ar);
				POSITION pos = listStr.GetHeadPosition();
				if( !mpValue && pos )
					mpValue = new PropVal::TCStringArray;
				while(pos)
					mpValue->push_back( listStr.GetNext(pos) );
			}
			else
			{
				unsigned long nSize;
				ar >> nSize;
				if( !mpValue && nSize > 0 )
					mpValue = new PropVal::TCStringArray;
				for(size_t idx = 0; idx < nSize; ++idx)
				{
					CString sVal;
					ar >> sVal;
					mpValue->push_back(sVal);
				}
			}
			return statOK;
		}
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropStringArray");
			int nSize = (int)(mpValue? mpValue->size() : 0);
			writeInt(pFile, nSize);
			for (int idx = 0; idx < nSize; idx++)
				writeString(pFile, mpValue->at(idx));
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			clear();
      int iCount;
      if (!readInt(sFile, iCount)) return statInvalidFormat;
			if( !mpValue && iCount > 0 )
				mpValue = new PropVal::TCStringArray;
      for (int i = 0; i < iCount; i++) {
        CString str;
        if (!readString(sFile, str)) return statInvalidFormat;
        mpValue->push_back(str);
      }
			return statOK;
		}

	virtual bool GetValue( PropVal::TCStringArray* const*& v ) const { v = &mpValue; return true; }
	virtual bool GetValue( PropVal::TCStringArray**& v ) { v = &mpValue; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return mpValue? asString( *mpValue ) : _T("<null>"); }
#endif
};

class CPropertyValueIntArray : public PropVal::CNamedPropertyValue
{
	PropVal::TIntArray* mpValue;
	friend class CPropertyObject;
protected:
	CPropertyValueIntArray() : mpValue( NULL ) {}
	~CPropertyValueIntArray() { delete mpValue; }
public:
	virtual PropertyType GetType() const { return PropIntArray; }
	virtual void clear() { PropVal::CNamedPropertyValue::clear(); delete mpValue; mpValue = NULL; }
	virtual size_t size() const { return mpValue? mpValue->size() : 0; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const
		{ //changing from CArray< int, int > to std::vector< int > in version 6 [ORW]
			if (nVersion <= 5)
			{
				CArray< int, int > rInt;
				unsigned long nSize = mpValue? mpValue->size() : 0;
				for(size_t idx = 0; idx < nSize; ++idx)
					rInt.Add(mpValue->at(idx));
				rInt.Serialize(ar);
			}
			else
			{
				unsigned long nSize = mpValue? mpValue->size() : 0;
				ar << unsigned long(nSize);
				for(size_t idx = 0; idx < nSize; ++idx)
					ar << int(mpValue->at(idx));
			}
			return statOK;
		}
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion )
		{ //changing from CArray< int, int > to std::vector< int > in version 6 [ORW]
			clear();
			if (nVersion <= 5)
			{
				if (nVersion <= 4)
				{
					CStringArray rStr;
					rStr.Serialize(ar);
					if( !mpValue && rStr.GetCount() > 0 )
						mpValue = new PropVal::TIntArray;
					for(INT_PTR idx = 0; idx < rStr.GetCount(); ++idx)
						mpValue->push_back(_tstol(rStr[idx]));
				}
				else
				{
					CArray< int, int > rInt;
					rInt.Serialize(ar);
					if( !mpValue && rInt.GetCount() > 0 )
						mpValue = new PropVal::TIntArray;
					for(INT_PTR idx = 0; idx < rInt.GetCount(); ++idx)
						mpValue->push_back(rInt[idx]);
				}
			}
			else
			{
				unsigned long nSize;
				ar >> nSize;
				if( !mpValue && nSize > 0 )
					mpValue = new PropVal::TIntArray;
				for(size_t idx = 0; idx < nSize; ++idx)
				{
					int nVal;
					ar >> nVal;
					mpValue->push_back(nVal);
				}
			}
			return statOK;
		}
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropIntArray");
			int nSize = (int)(mpValue? mpValue->size() : 0);
			writeInt(pFile, nSize);
      for (int i = 0; i < nSize; i++)
        writeInt(pFile, mpValue->at(i));
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			clear();
      int iCount;
      if (!readInt(sFile, iCount)) return statInvalidFormat;
			if( !mpValue && iCount > 0 )
				mpValue = new PropVal::TIntArray;
      for (int i = 0; i < iCount; i++)
			{
        int intValue;
        if (!readInt(sFile, intValue)) return statInvalidFormat;
        mpValue->push_back(intValue);
      }
			return statOK;
		}

	virtual bool GetValue( PropVal::TIntArray* const*& v ) const { v = &mpValue; return true; }
	virtual bool GetValue( PropVal::TIntArray**& v ) { v = &mpValue; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return mpValue? asString( *mpValue ) : _T("<null>"); }
#endif
};

class CPropertyValueActiveXPropPages : public PropVal::CNamedPropertyValue
{
	AxInterfaceDescriptor* mpValue;
	friend class CPropertyObject;
protected:
	CPropertyValueActiveXPropPages() : mpValue( NULL ) {}
	~CPropertyValueActiveXPropPages(){ delete mpValue; }
public:
	virtual PropertyType GetType() const { return PropActiveXPropPages; }
	virtual void clear() { PropVal::CNamedPropertyValue::clear(); delete mpValue; mpValue = NULL; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const
		{
			if( !mpValue )
				AxInterfaceDescriptor().Serialize( ar, nVersion );
			else
				const_cast<CPropertyValueActiveXPropPages*>(this)->mpValue->Serialize( ar, nVersion );
			return statOK;
		}
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion )
		{
			clear();
			if( !mpValue )
				mpValue = new AxInterfaceDescriptor;
			mpValue->Serialize( ar, nVersion );
			return statOK;
		}
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropActiveXPropPages");
			if( !mpValue )
				AxInterfaceDescriptor().WriteToTextFile( pFile );
			else
				mpValue->WriteToTextFile( pFile );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			clear();
			if( !mpValue )
				mpValue = new AxInterfaceDescriptor;
			switch( nVersion )
			{
				case 5: return mpValue->ReadFromTextFile5( sFile );
			};
			return statInvalidFormat;
		}

	virtual bool GetValue( AxInterfaceDescriptor* const*& v ) const { v = &mpValue; return true; }
	virtual bool GetValue( AxInterfaceDescriptor**& v ) { v = &mpValue; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const
		{
			if( !mpValue )
				return _T("<null>");
			return mpValue->toString();
		}
#endif
};

class CPropertyValueActiveXProp : public PropVal::CNamedPropertyValue
{
	AxInterfaceDescriptor* mpValue;
	friend class CPropertyObject;
protected:
	CPropertyValueActiveXProp() : mpValue( NULL ) {}
	~CPropertyValueActiveXProp(){ delete mpValue; }
public:
	virtual PropertyType GetType() const { return PropActiveXProp; }
	virtual void clear() { PropVal::CNamedPropertyValue::clear(); delete mpValue; mpValue = NULL; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const
		{
			ar << msDisplayName;
			if( !mpValue )
				AxInterfaceDescriptor().Serialize( ar, nVersion );
			else
				const_cast<CPropertyValueActiveXProp*>(this)->mpValue->Serialize( ar, nVersion );
			return statOK;
		}
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion )
		{
			clear();
			ar >> msDisplayName;
			if( !mpValue )
				mpValue = new AxInterfaceDescriptor;
			mpValue->Serialize( ar, nVersion );
			return statOK;
		}
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropActiveXProp");
			writeString(pFile, msDisplayName);
			if( !mpValue )
				return AxInterfaceDescriptor().WriteToTextFile( pFile );
			mpValue->WriteToTextFile( pFile );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			clear();
			msDisplayName.Empty();
			if (!readString(sFile, msDisplayName)) return statInvalidFormat;
			if( !mpValue )
				mpValue = new AxInterfaceDescriptor;
			switch( nVersion )
			{
				case 5: return mpValue->ReadFromTextFile5( sFile );
			};
			return statInvalidFormat;
		}

	virtual bool GetValue( AxInterfaceDescriptor* const*& v ) const { v = &mpValue; return true; }
	virtual bool GetValue( AxInterfaceDescriptor**& v ) { v = &mpValue; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const
		{
			if( !mpValue )
				return _T("<null>");
			return mpValue->toString();
		}
#endif
};

class CPropertyValueActiveXEnum : public PropVal::CNamedPropertyValue
{
	AxInterfaceDescriptor* mpValue;
	friend class CPropertyObject;
protected:
	CPropertyValueActiveXEnum() : mpValue( NULL ) {}
	~CPropertyValueActiveXEnum(){ delete mpValue; }
public:
	virtual PropertyType GetType() const { return PropActiveXEnum; }
	virtual void clear() { PropVal::CNamedPropertyValue::clear(); delete mpValue; mpValue = NULL; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const
		{
			if( !mpValue )
				AxInterfaceDescriptor().Serialize( ar, nVersion );
			else
				const_cast<CPropertyValueActiveXEnum*>(this)->mpValue->Serialize( ar, nVersion );
			return statOK;
		}
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion )
		{
			clear();
			if( !mpValue )
				mpValue = new AxInterfaceDescriptor;
			mpValue->Serialize( ar, nVersion );
			return statOK;
		}
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropActiveXEnum");
			if( !mpValue )
				return AxInterfaceDescriptor().WriteToTextFile( pFile );
			mpValue->WriteToTextFile( pFile );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			clear();
			if( !mpValue )
				mpValue = new AxInterfaceDescriptor;
			switch( nVersion )
			{
				case 5: return mpValue->ReadFromTextFile5( sFile );
			};
			return statInvalidFormat;
		}

	virtual bool GetValue( AxInterfaceDescriptor* const*& v ) const { v = &mpValue; return true; }
	virtual bool GetValue( AxInterfaceDescriptor**& v ) { v = &mpValue; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const
		{
			if( !mpValue )
				return _T("<null>");
			return mpValue->toString();
		}
#endif
};

class CPropertyValueActiveXEvent : public PropVal::CNamedPropertyValue
{
	AxInterfaceDescriptor* mpValue;
	friend class CPropertyObject;
protected:
	CPropertyValueActiveXEvent() : mpValue( NULL ) {}
	~CPropertyValueActiveXEvent(){ delete mpValue; }
public:
	virtual PropertyType GetType() const { return PropActiveXEvent; }
	virtual void clear() { PropVal::CNamedPropertyValue::clear(); delete mpValue; mpValue = NULL; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const
		{
			ar << msDisplayName;
			if( !mpValue )
				AxInterfaceDescriptor().Serialize( ar, nVersion );
			else
				const_cast<CPropertyValueActiveXEvent*>(this)->mpValue->Serialize( ar, nVersion );
			return statOK;
		}
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion )
		{
			clear();
			ar >> msDisplayName;
			if( !mpValue )
				mpValue = new AxInterfaceDescriptor;
			mpValue->Serialize( ar, nVersion );
			return statOK;
		}
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropActiveXEvent");
			writeString(pFile, msDisplayName);
			if( !mpValue )
				return AxInterfaceDescriptor().WriteToTextFile( pFile );
			mpValue->WriteToTextFile( pFile );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			clear();
			msDisplayName.Empty();
			if (!readString(sFile, msDisplayName)) return statInvalidFormat;
			if( !mpValue )
				mpValue = new AxInterfaceDescriptor;
			switch( nVersion )
			{
				case 5: return mpValue->ReadFromTextFile5( sFile );
			};
			return statInvalidFormat;
		}

	virtual bool GetValue( AxInterfaceDescriptor* const*& v ) const { v = &mpValue; return true; }
	virtual bool GetValue( AxInterfaceDescriptor**& v ) { v = &mpValue; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const
		{
			if( !mpValue )
				return _T("<null>");
			return mpValue->toString();
		}
#endif
};

class CPropertyValueActiveXRunTime : public PropVal::CNamedPropertyValue
{
	AxInterfaceDescriptor* mpValue;
	friend class CPropertyObject;
protected:
	CPropertyValueActiveXRunTime() : mpValue( NULL ) {}
	~CPropertyValueActiveXRunTime(){ delete mpValue; }
public:
	virtual PropertyType GetType() const { return PropActiveXRunTime; }
	virtual void clear() { PropVal::CNamedPropertyValue::clear(); delete mpValue; mpValue = NULL; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const
		{
			if( !mpValue )
				AxInterfaceDescriptor().Serialize( ar, nVersion );
			else
				const_cast<CPropertyValueActiveXRunTime*>(this)->mpValue->Serialize( ar, nVersion );
			return statOK;
		}
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion )
		{
			clear();
			if( !mpValue )
				mpValue = new AxInterfaceDescriptor;
			mpValue->Serialize( ar, nVersion );
			return statOK;
		}
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropActiveXRunTime");
			if( !mpValue )
				return AxInterfaceDescriptor().WriteToTextFile( pFile );
			mpValue->WriteToTextFile( pFile );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			clear();
			if( !mpValue )
				mpValue = new AxInterfaceDescriptor;
			switch( nVersion )
			{
				case 5: return mpValue->ReadFromTextFile5( sFile );
			};
			return statInvalidFormat;
		}

	virtual bool GetValue( AxInterfaceDescriptor* const*& v ) const { v = &mpValue; return true; }
	virtual bool GetValue( AxInterfaceDescriptor**& v ) { v = &mpValue; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const
		{
			if( !mpValue )
				return _T("<null>");
			return mpValue->toString();
		}
#endif
};

class CPropertyValueActiveXMethods : public PropVal::CNamedPropertyValue
{
	AxInterfaceDescriptor* mpValue;
	friend class CPropertyObject;
protected:
	CPropertyValueActiveXMethods() : mpValue( NULL ) {}
	~CPropertyValueActiveXMethods(){ delete mpValue; }
public:
	virtual PropertyType GetType() const { return PropActiveXMethods; }
	virtual void clear() { PropVal::CNamedPropertyValue::clear(); delete mpValue; mpValue = NULL; }
	virtual size_t size() const { return ((mpValue && mpValue->GetMethods())? mpValue->GetMethods()->size() : 0); }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const
		{
			if( !mpValue )
				AxInterfaceDescriptor().Serialize( ar, nVersion );
			else
				const_cast<CPropertyValueActiveXMethods*>(this)->mpValue->Serialize( ar, nVersion );
			return statOK;
		}
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion )
		{
			clear();
			if( !mpValue )
				mpValue = new AxInterfaceDescriptor;
			mpValue->Serialize( ar, nVersion );
			return statOK;
		}
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropActiveXMethods");
			if( !mpValue )
				return AxInterfaceDescriptor().WriteToTextFile( pFile );
			mpValue->WriteToTextFile( pFile );
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			clear();
			if( !mpValue )
				mpValue = new AxInterfaceDescriptor;
			switch( nVersion )
			{
				case 5: return mpValue->ReadFromTextFile5( sFile );
			};
			return statInvalidFormat;
		}

	virtual bool GetValue( AxInterfaceDescriptor* const*& v ) const { v = &mpValue; return true; }
	virtual bool GetValue( AxInterfaceDescriptor**& v ) { v = &mpValue; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const
		{
			if( !mpValue )
				return _T("<null>");
			return mpValue->toString();
		}
#endif
};

class CPropertyValueStringArrayList : public PropVal::CNamedPropertyValue
{
	PropVal::TCStringArrayList* mpValue;
	friend class CPropertyObject;
protected:
	CPropertyValueStringArrayList() : mpValue( NULL ) {}
	~CPropertyValueStringArrayList() { delete mpValue; }
public:
	virtual PropertyType GetType() const { return PropStringArrayList; }
	virtual void clear() { PropVal::CNamedPropertyValue::clear(); delete mpValue; mpValue = NULL; }
	virtual size_t size() const { return mpValue? mpValue->size() : 0; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const
		{ //changing from CTypedPtrList< CObList, CStringArray* > to std::vector< std::vector< CString > > in version 6 [ORW]
			if (nVersion <= 5)
			{
				CTypedPtrList< CObList, CStringArray* > rsStringArrayList;
				if( mpValue )
				{
					PropVal::TCStringArrayList::const_iterator pos = mpValue->begin();
					while (pos != mpValue->end())
					{
						const PropVal::TCStringArray& rStr = *pos++;
						CStringArray* prsNew = new CStringArray;
						for(size_t idx = 0; idx < rStr.size(); ++idx)
							prsNew->Add(rStr[idx]);
						rsStringArrayList.AddTail(prsNew);
					}
				}
				rsStringArrayList.Serialize(ar);
				POSITION posS = rsStringArrayList.GetHeadPosition();
				while (posS)
					delete rsStringArrayList.GetNext(posS);
			}
			else
			{
				ar << unsigned long(mpValue? mpValue->size() : 0);
				if( mpValue )
				{
					PropVal::TCStringArrayList::const_iterator pos = mpValue->begin();
					while(pos != mpValue->end())
					{
						const PropVal::TCStringArray& rStr = *pos++;
						unsigned long nSize = rStr.size();
						ar << unsigned long(nSize);
						for(size_t idx = 0; idx < nSize; ++idx)
							ar << rStr[idx];
					}
				}
			}
			return statOK;
		}
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion )
		{ //changing from CTypedPtrList< CObList, CStringArray* > to std::vector< std::vector< CString > > in version 6 [ORW]
			clear();
			if (nVersion <= 5)
			{
				CTypedPtrList< CObList, CStringArray* > rsStringArrayList;
				rsStringArrayList.Serialize(ar);
				POSITION pos = rsStringArrayList.GetHeadPosition();
				if( !mpValue && pos )
					mpValue = new PropVal::TCStringArrayList;
				while (pos)
				{
					CStringArray* prStr = rsStringArrayList.GetNext(pos);
					if (!prStr) //some old files contain NULL pointers!
						continue;
					if (IsBadReadPtr(prStr, sizeof(CStringArray)))
						continue; //ignore bogus values
					mpValue->push_back( PropVal::TCStringArray() );
					PropVal::TCStringArray& rStr = mpValue->back();
					for( int idx = 0; idx < prStr->GetSize(); ++idx )
						rStr.push_back(prStr->GetAt(idx));
					delete prStr;
				}
			}
			else
			{
				unsigned long nSizeL;
				ar >> nSizeL;
				if( !mpValue && nSizeL > 0 )
					mpValue = new PropVal::TCStringArrayList;
				for( size_t idxL = 0; idxL < nSizeL; ++idxL)
				{
					unsigned long nSize;
					ar >> nSize;
					mpValue->push_back( PropVal::TCStringArray() );
					PropVal::TCStringArray& rStr = mpValue->back();
					for( size_t idx = 0; idx < nSize; ++idx)
					{
						CString Str;
						ar >> Str;
						rStr.push_back(Str);
					}
				}
			}
			return statOK;
		}
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropStringArrayList");
			writeInt(pFile, (int)(mpValue? mpValue->size() : 0));
			if( mpValue )
			{
				PropVal::TCStringArrayList::const_iterator pos = mpValue->begin();
				while (pos != mpValue->end())
				{
					const PropVal::TCStringArray& rStr = *pos++;
					writeInt(pFile, (int)rStr.size());
					for (size_t i = 0; i < rStr.size(); ++i)
						writeString(pFile, rStr[i]);
				}
			}
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			clear();
      int iArrayCount;
      if (!readInt(sFile, iArrayCount)) return statInvalidFormat;
			if( !mpValue && iArrayCount > 0 )
				mpValue = new PropVal::TCStringArrayList;
      for (int i = 0; i < iArrayCount; i++) {
				mpValue->push_back( PropVal::TCStringArray() );
				PropVal::TCStringArray& rStr = mpValue->back();
        int iStringCount;
        if (!readInt(sFile, iStringCount)) return statInvalidFormat;
        if (iStringCount != -1) {
          for (int j = 0; j < iStringCount; j++) {
            CString str;
            if (!readString(sFile, str)) return statInvalidFormat;
            rStr.push_back(str);
          }
        }
      }
			return statOK;
		}

	virtual bool GetValue( PropVal::TCStringArrayList* const*& v ) const { v = &mpValue; return true; }
	virtual bool GetValue( PropVal::TCStringArrayList**& v ) { v = &mpValue; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return mpValue? asString( *mpValue ) : _T("<null>"); }
#endif
};

class CPropertyValueIntArrayList : public PropVal::CNamedPropertyValue
{
	PropVal::TIntArrayList* mpValue;
	friend class CPropertyObject;
protected:
	CPropertyValueIntArrayList() : mpValue( NULL ) {}
	~CPropertyValueIntArrayList() { delete mpValue; }
public:
	virtual PropertyType GetType() const { return PropIntArrayList; }
	virtual void clear() { PropVal::CNamedPropertyValue::clear(); delete mpValue; mpValue = NULL; }
	virtual size_t size() const { return mpValue? mpValue->size() : 0; }
	virtual IOStatus FileOut( CArchive& ar, ULONG nVersion ) const
		{ //changing from CList< CArray< int, int >* > to std::vector< std::vector< int > > in version 6 [ORW]
			ar << unsigned long(mpValue? mpValue->size() : 0);
			if( mpValue )
			{
				PropVal::TIntArrayList::const_iterator pos = mpValue->begin();
				while (pos != mpValue->end())
				{
					const PropVal::TIntArray& rInt = *pos++;
					unsigned long nSize = rInt.size();
					ar << unsigned long(nSize);
					for (size_t idx = 0; idx < nSize; ++idx)
						ar << int(rInt[idx]);
				}
			}
			return statOK;
		}
	virtual IOStatus FileIn( CArchive& ar, ULONG nVersion )
		{ //changing from CList< CArray< int, int >* > to std::vector< std::vector< int > > in version 6 [ORW]
			clear();
			unsigned long nSize;
			ar >> nSize;
			if( !mpValue && nSize > 0 )
				mpValue = new PropVal::TIntArrayList;
			for(size_t idx = 0; idx < nSize; idx++)
			{
				unsigned long nSize;
				ar >> nSize;
				mpValue->push_back( PropVal::TIntArray() );
				PropVal::TIntArray& rIntV = mpValue->back();
				for(size_t idx = 0; idx < nSize; idx++)
				{
					int nVal;
					ar >> nVal;
					rIntV.push_back(nVal);
				}
			}
			return statOK;
		}
	virtual IOStatus FileOut( FILE* pFile, ULONG nVersion ) const
		{
			fprintf(pFile, "PropIntArrayList");
			writeInt(pFile, (int)(mpValue? mpValue->size() : 0));
			if( mpValue )
			{
				PropVal::TIntArrayList::const_iterator pos = mpValue->begin();
				while (pos != mpValue->end())
				{
					const PropVal::TIntArray& rInt = *pos++;
					writeInt(pFile, (int)rInt.size());
					for (size_t i = 0; i < rInt.size(); ++i)
						writeInt(pFile, rInt[i]);
				}
			}
			return statOK;
		}
	virtual IOStatus FileIn( std::ifstream &sFile, ULONG nVersion )
		{
			clear();
      int nCounter;
      if (!readInt(sFile, nCounter)) return statInvalidFormat;;
			if( !mpValue && nCounter > 0 )
				mpValue = new PropVal::TIntArrayList;
      for (int i=0; i<nCounter; i++)
      {
        mpValue->push_back(PropVal::TIntArray());
				PropVal::TIntArray& rInt = mpValue->back();
        int nCounter2;
        if (!readInt(sFile, nCounter2)) return statInvalidFormat;;
        for (int j=0; j<nCounter2; j++)
        {		
          int nItem;
          if (!readInt(sFile, nItem)) return statInvalidFormat;;
          rInt.push_back(nItem);
        }
      }
			return statOK;
		}

	virtual bool GetValue( PropVal::TIntArrayList* const*& v ) const { v = &mpValue; return true; }
	virtual bool GetValue( PropVal::TIntArrayList**& v ) { v = &mpValue; return true; }

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toStringPropVal() const { return mpValue? asString( *mpValue ) : _T("<null>"); }
#endif
};


/////////////////////////////////////////////////////////////////////////////
// CPropertyObject

IMPLEMENT_SERIAL(CPropertyObject, CObject, 1)

CPropertyObject::CPropertyObject()
: mbHidden( false )
, mnID( nInvalidPropertyId )
{	
	SetType( PropInvalid );
	SetSubtype( 0 );
}

CPropertyObject::CPropertyObject(PropertyType type, DWORD dwSubtype /*= 0*/, PropertyId nID /*= -1*/ )
: mbHidden( false )
, mnID( nID )
{	
	SetType( type );
	SetSubtype( dwSubtype );
}

CPropertyObject::~CPropertyObject()
{
}

void CPropertyObject::SetType( PropertyType type )
{
	if( mpValue && type == mpValue->GetType() )
		return; //no-op
	switch( type )
	{
	case PropLong:
		mpValue = new CPropertyValueLong;
		break;
	case PropString:
		mpValue = new CPropertyValueString;
		break;
	case PropDouble:
		mpValue = new CPropertyValueDouble;
		break;
	case PropBool:
		mpValue = new CPropertyValueBool;
		break;
	case PropEnum:
		mpValue = new CPropertyValueEnum;
		break;
	case PropEvent:
		mpValue = new CPropertyValueEvent;
		break;
	case PropPicture:
		mpValue = new CPropertyValuePicture;
		break;
	case PropCustom:
		mpValue = new CPropertyValueCustom;
		break;
	case PropImageList:
		mpValue = new CPropertyValueImageList;
		break;
	case PropOLEColor:
		mpValue = new CPropertyValueOLEColor;
		break;
	case PropStringArray:
		mpValue = new CPropertyValueStringArray;
		break;
	case PropIntArray:
		mpValue = new CPropertyValueIntArray;
		break;
	case PropActiveXPropPages:
		mpValue = new CPropertyValueActiveXPropPages;
		break;
	case PropActiveXProp:
		mpValue = new CPropertyValueActiveXProp;
		break;
	case PropActiveXEnum:
		mpValue = new CPropertyValueActiveXEnum;
		break;
	case PropActiveXEvent:
		mpValue = new CPropertyValueActiveXEvent;
		break;
	case PropActiveXRunTime:
		mpValue = new CPropertyValueActiveXRunTime;
		break;
	case PropActiveXMethods:
		mpValue = new CPropertyValueActiveXMethods;
		break;
	case PropStringArrayList:
		mpValue = new CPropertyValueStringArrayList;
		break;
	case PropIntArrayList:
		mpValue = new CPropertyValueIntArrayList;
		break;
	default:
		mpValue = new CPropertyValueInvalid;
		break;
	};
#ifdef _DEBUG
	mType = type; //used for inspecting type at runtime while debugging
#endif

	assert( mpValue != NULL );
}

CString CPropertyObject::GetStringValue() const
{
	CString sValue;
	bool bSuccess = mpValue->GetValue( sValue );
	assert( bSuccess == true );
	return sValue;
}

bool CPropertyObject::SetStringValue( LPCTSTR pszValue )
{
	bool bSuccess = mpValue->SetValue( pszValue );
	assert( bSuccess == true || (!pszValue || !*pszValue) ); //ignore failure on empty strings
	return bSuccess;
}

OLE_COLOR CPropertyObject::GetOLEColorValue() const
{
	OLE_COLOR dwValue = 0;
	bool bSuccess = mpValue->GetValue( dwValue );
	assert( bSuccess == true );
	return dwValue;
}

bool CPropertyObject::SetOLEColorValue( const OLE_COLOR& dwColor )
{
	bool bSuccess = mpValue->SetValue( dwColor );
	assert( bSuccess == true );
	return bSuccess;
}

long CPropertyObject::GetLongValue() const
{
	long lValue = -1;
	bool bSuccess = mpValue->GetValue( lValue );
	assert( bSuccess == true );
	return lValue;
}

bool CPropertyObject::SetLongValue( long lValue )
{
	bool bSuccess = false;
	if( GetType() == PropBool )
	{	//special case handling for control positioning properties that can be either long or boolean
		if( lValue == 0 || lValue == 1 )
			bSuccess = mpValue->SetValue( (lValue != 0) );
		else
		{
			SetType( PropLong );
			bSuccess = mpValue->SetValue( lValue );
		}
	}
	else
		bSuccess = mpValue->SetValue( lValue );
	assert( bSuccess == true );
	return bSuccess;
}

bool CPropertyObject::GetBooleanValue() const
{
	bool bValue = false;
	bool bSuccess = mpValue->GetValue( bValue );
	assert( bSuccess == true );
	return bValue;
}

bool CPropertyObject::SetBooleanValue( bool bValue )
{
	bool bSuccess = mpValue->SetValue( bValue );
	assert( bSuccess == true );
	return bSuccess;
}

double CPropertyObject::GetDoubleValue() const
{
	double dblValue = 0.0;
	bool bSuccess = mpValue->GetValue( dblValue );
	assert( bSuccess == true );
	return dblValue;
}

bool CPropertyObject::SetDoubleValue( double dblValue )
{
	bool bSuccess = mpValue->SetValue( dblValue );
	assert( bSuccess == true );
	return bSuccess;
}

short CPropertyObject::GetShortValue() const
{
	short nValue = -1;
	bool bSuccess = mpValue->GetValue( nValue );
	assert( bSuccess == true );
	return nValue;
}

bool CPropertyObject::SetShortValue( short idxValue )
{
	bool bSuccess = mpValue->SetValue( idxValue );
	assert( bSuccess == true );
	return bSuccess;
}

const PropVal::TCStringArray* CPropertyObject::GetStringArrayPtr() const
{
	PropVal::TCStringArray* const* v = NULL;
	bool bSuccess = mpValue->GetValue( v );
	assert( bSuccess == true );
	return (*v);
}

PropVal::TCStringArray* CPropertyObject::GetStringArrayPtr()
{
	PropVal::TCStringArray** v = NULL;
	bool bSuccess = mpValue->GetValue( v );
	assert( bSuccess == true );
	if( !bSuccess )
		return NULL;
	if( !(*v) )
		(*v) = new PropVal::TCStringArray;
	return (*v);
}

const PropVal::TCStringArrayList* CPropertyObject::GetStringArrayListPtr() const
{
	PropVal::TCStringArrayList* const* v = NULL;
	bool bSuccess = mpValue->GetValue( v );
	assert( bSuccess == true );
	return (*v);
}

PropVal::TCStringArrayList* CPropertyObject::GetStringArrayListPtr()
{
	PropVal::TCStringArrayList** v = NULL;
	bool bSuccess = mpValue->GetValue( v );
	assert( bSuccess == true );
	if( !bSuccess )
		return NULL;
	if( !(*v) )
		(*v) = new PropVal::TCStringArrayList;
	return (*v);
}

const PropVal::TIntArray* CPropertyObject::GetIntArrayPtr() const
{
	PropVal::TIntArray* const* v = NULL;
	bool bSuccess = mpValue->GetValue( v );
	assert( bSuccess == true );
	return (*v);
}

PropVal::TIntArray* CPropertyObject::GetIntArrayPtr()
{
	PropVal::TIntArray** v = NULL;
	bool bSuccess = mpValue->GetValue( v );
	assert( bSuccess == true );
	if( !bSuccess )
		return NULL;
	if( !(*v) )
		(*v) = new PropVal::TIntArray;
	return (*v);
}

const PropVal::TIntArrayList* CPropertyObject::GetIntArrayListPtr() const
{
	PropVal::TIntArrayList* const* v = NULL;
	bool bSuccess = mpValue->GetValue( v );
	return (*v);
}

PropVal::TIntArrayList* CPropertyObject::GetIntArrayListPtr()
{
	PropVal::TIntArrayList** v = NULL;
	bool bSuccess = mpValue->GetValue( v );
	if( !bSuccess )
		return NULL;
	if( !(*v) )
		(*v) = new PropVal::TIntArrayList;
	return (*v);
}

const AxInterfaceDescriptor* CPropertyObject::GetAxInterfaceDescriptorPtr() const
{
	AxInterfaceDescriptor* const* v = NULL;
	bool bSuccess = mpValue->GetValue( v );
	//assert( bSuccess == true );
	return (*v);
}

AxInterfaceDescriptor* CPropertyObject::GetAxInterfaceDescriptorPtr()
{
	AxInterfaceDescriptor** v = NULL;
	bool bSuccess = mpValue->GetValue( v );
	//assert( bSuccess == true );
	if( !bSuccess )
		return NULL;
	if( !(*v) )
		(*v) = new AxInterfaceDescriptor;
	return (*v);
}

void CPropertyObject::AddStringItem(CString NewString)
{
	if (GetType() == PropIntArray)
		GetIntArrayPtr()->push_back(_tstol(NewString));
	else if( GetType() == PropStringArray)
		GetStringArrayPtr()->push_back(NewString);
}

size_t CPropertyObject::size() const
{
	return mpValue->size();
}

CString CPropertyObject::GetName()
{
	switch( GetType() )
	{
	case PropActiveXMethods:
		return theWorkspace.LoadResourceString(IDS_AXMETHODS); //"(Object Browsr)"
	case PropActiveXPropPages:
		return theWorkspace.LoadResourceString(IDS_PROP_ACTIVEXPROPPAGES); //"(ActiveX Wizard)"
	case PropActiveXProp:
	case PropActiveXEnum:
	case PropActiveXEvent:
	case PropActiveXRunTime:
		return GetStringValue();
	}
	return GetPropertyName(GetID());
}

CString CPropertyObject::GetDocumentationDesc()
{
	CString sDesc;
	AxInterfaceDescriptor** v;
	if( mpValue->GetValue( v ) && *v )
		sDesc = (*v)->GetDesc();
	if( sDesc.IsEmpty() )
		sDesc = theWorkspace.LoadResourceString(mnID + nDePropDescResStringOffset);
	return sDesc;
}

CString CPropertyObject::GetStringItem(short ListIndex)
{
	// create a position variable to hold the converted ListIndex
	CString pItem;
	
	if(size_t(ListIndex) < GetStringArrayPtr()->size())
		pItem = GetStringArrayPtr()->at(ListIndex);
	else if(size_t(ListIndex) < GetIntArrayPtr()->size())
	{
		// create a new String list item and point it at the object in the list
		pItem.Format(_T("%d"), GetIntArrayPtr()->at(ListIndex));
	}
	else
	{
		//pItem = theWorkspace.LoadResourceString(IDS_ERRORONRET);
		pItem = _T("Error - on retrieval");
	}
	return pItem;
}

CString CPropertyObject::GetStdProperty()
{		
	CString RetString;
	switch (GetType())
	{
	case PropEnum:	
	case PropPicture:
	case PropLong:	
		{
		RetString.Format(_T("%d"), GetLongValue());
		break;
		}
	case PropEvent:
	case PropString:
		{
		RetString = GetStringValue();
		break;
		}
	case PropDouble:
		{
		RetString.Format(_T("%.8f"), GetDoubleValue());
		break;
		}
	case PropBool:
		{			
		if (GetBooleanValue() == TRUE)
			RetString = _T("True");
		else
			RetString = _T("False");
		
		break;
		}
	}	
	return RetString;
}


IOStatus CPropertyObject::WriteToTextFile(FILE* pFile) const
{
  fprintf(pFile, "\nCPropertyObject");
  int nThisVersion = 5;
  writeInt(pFile, nThisVersion);
  writeInt(pFile, mnID);
  writeInt(pFile, GetType());
  // store the flag that indicates this property is to be hidden
  writeBOOL(pFile, mbHidden);
	return mpValue->FileOut(pFile, nThisVersion);
}


void CPropertyObject::Serialize(CArchive& ar)
{
	ULONG nThisVersion = GetCurrentSaveVersion();
	CObject::Serialize( ar );
	if (ar.IsStoring())
	{
		ar << unsigned long(nThisVersion);
		if (nThisVersion <= 5) //changing from long to short in version 6 [ORW]
			ar << long(mnID);
		else
			ar << short(mnID);
		ar << long(GetType());
		ar << DWORD(GetSubtype()); //added in version 7
		if (nThisVersion <= 5) //changing from BOOL to bool in version 6 [ORW]
			ar << BOOL(mbHidden); // store the flag that indicates this property is to be hidden
		else
			ar << bool(mbHidden); // store the flag that indicates this property is to be hidden
		mpValue->FileOut(ar, nThisVersion);
	}
	else
	{
		ar >> unsigned long(nThisVersion);
		if (nThisVersion > GetCurrentSaveVersion())
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );

		if (nThisVersion <= 5) //changing from long to short in version 6 [ORW]
		{
			long lID;
			ar >> long(lID);
			mnID = (PropertyId)lID;
		}
		else
		{
			short nID;
			ar >> short(nID);
			mnID = (PropertyId)nID;
		}
		long lType;
		ar >> long(lType);
		SetType( (PropertyType)lType );

		if( nThisVersion >= 7 )
		{
			DWORD dwSubtype;
			ar >> dwSubtype;
			SetSubtype( dwSubtype ); //added in version 7
		}
		if (nThisVersion <= 5) //changing from BOOL to bool in version 6 [ORW]
		{
			BOOL bHidden;
			ar >> BOOL(bHidden);
			mbHidden = (bHidden != FALSE);
		}
		else
			ar >> bool(mbHidden); // get the flag that indicates this property is to be hidden
		if (GetType() == PropActiveXRunTime)
			mbHidden = true; // if this property is a run time only activeX property, set it as hidden.
		mpValue->FileIn(ar, nThisVersion);
	}	
}

IOStatus CPropertyObject::ReadFromTextFile(std::ifstream &sFile)
{
  CString sObject;
  if (readLine(sFile) != "CPropertyObject") return statInvalidFormat;
  int iVersion;
  if (!readInt(sFile, iVersion)) return statInvalidFormat;

  switch (iVersion) {
    case 5 : 
      return ReadFromTextFile5(sFile);
      break;
  }
  return statInvalidFormat;
}

IOStatus CPropertyObject::ReadFromTextFile5(std::ifstream &sFile)
{
	int nID;
  if (!readInt(sFile, nID)) return statInvalidFormat;
	mnID = (PropertyId)nID;
	int nType;
  if (!readInt(sFile, nType)) return statInvalidFormat;
	SetType( (PropertyType)nType );
  // get the flag that indicates this property is to be hidden
  if (!readBool(sFile, mbHidden)) return statInvalidFormat;

  // if this property is a run time only activeX property, set it as hidden.
  if (GetType() == PropActiveXRunTime)
    mbHidden = true;

  CString sType = readLine(sFile);
	return mpValue->FileIn(sFile, 5);
}

#ifdef _DIAGNOSTIC
LPCTSTR CPropertyObject::toString() const
{
	LPCTSTR pszHidden = mbHidden? _T(" (hidden)") : _T("");
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("Property [%20s]%s = %s"),
							asString( mnID ), pszHidden, (mpValue? mpValue->toString() : _T("<null>")) );
	return buf;
}
#endif
