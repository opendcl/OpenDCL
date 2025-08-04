// AxInterfaceDescriptor.cpp : implementation file
//

#include "stdafx.h"
#include "AxInterfaceDescriptor.h"
#include "Filing.h"
#include "Project.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxMethodDescriptor.h"
#include "AxTypeUtils.h"
#include "AxContainerCtrl.h"
#include "OleFont.h"


void AxInterfaceDescriptor::clear()
{
	delete mpProp;
	delete mpPropGet;
	delete mpPropPut;
	delete mpPropPutRef;
	delete mpEvent;
	delete mpMethods;
	mpProp = NULL;
	mpPropGet = NULL;
	mpPropPut = NULL;
	mpPropPutRef = NULL;
	mpEvent = NULL;
	mpMethods = NULL;
}


IOStatus AxInterfaceDescriptor::ReadFromTextFile5(std::ifstream &sFile) 
{
  BOOL bProp;
  BOOL bPropGet;
  BOOL bPropPut;
  BOOL bPropPutRef;
  BOOL bPropEvent;
  BOOL bPropMethod;

  if (!readBOOL(sFile, bProp)) return statInvalidFormat;
  if (bProp)
  {
		delete mpProp;
    mpProp = new AxPropertyDescriptor;
		IOStatus stat = mpProp->ReadFromTextFile(sFile, 5);
    if (stat != statOK) return stat;
  }

  if (!readBOOL(sFile, bPropGet)) return statInvalidFormat;
  if (bPropGet)
  {
		delete mpPropGet;
    mpPropGet = new AxPropertyDescriptor;
		IOStatus stat = mpPropGet->ReadFromTextFile(sFile, 5);
    if (stat != statOK) return stat;
  }

  if (!readBOOL(sFile, bPropPut)) return statInvalidFormat;
  if (bPropPut)
  {
		delete mpPropPut;
    mpPropPut = new AxPropertyDescriptor;
		IOStatus stat = mpPropPut->ReadFromTextFile(sFile, 5);
    if (stat != statOK) return stat;
  }

  if (!readBOOL(sFile, bPropPutRef)) return statInvalidFormat;
  if (bPropPutRef)
  {
		delete mpPropPutRef;
    mpPropPutRef = new AxPropertyDescriptor;
		IOStatus stat = mpPropPutRef->ReadFromTextFile(sFile, 5);
    if (stat != statOK) return stat;
  }

  if (!readBOOL(sFile, bPropEvent)) return statInvalidFormat;
  if (bPropEvent)
  {
		delete mpEvent;
		mpEvent = new AxEventDescriptor;
		IOStatus stat = mpEvent->ReadFromTextFile(sFile, 5);
    if (stat != statOK) return stat;
  }
  if (!readBOOL(sFile, bPropMethod)) return statInvalidFormat;
  if (bPropMethod)
  {
		delete mpMethods;
		mpMethods = new std::vector< RefCountedPtr< AxMethodDescriptor > >;
    int nCount;
    if (!readInt(sFile, nCount)) return statInvalidFormat;
    while (nCount-- > 0)
    {
			std::auto_ptr< AxMethodDescriptor > pMethod( new AxMethodDescriptor );
			IOStatus stat = pMethod->ReadFromTextFile(sFile);
			if (stat != statOK) return stat;
			mpMethods->push_back(pMethod.release());		
    }
  }
  return statOK;
}


//IOStatus AxInterfaceDescriptor::WriteToTextFile(FILE* pFile) const
//{
//  BOOL bProp;
//  BOOL bPropGet;
//  BOOL bPropPut;
//  BOOL bPropPutRef;
//  BOOL bPropEvent;
//  BOOL bPropMethod;
//
//  bProp = (mpProp != NULL);
//  writeBOOL(pFile, bProp);
//  if (bProp)
//    mpProp->WriteToTextFile(pFile);
//
//  bPropGet = (mpPropGet != NULL);
//  writeBOOL(pFile, bPropGet);
//  if (bPropGet)
//    mpPropGet->WriteToTextFile(pFile);
//
//  bPropPut = (mpPropPut != NULL);
//  writeBOOL(pFile, bPropPut);
//  if (bPropPut)
//    mpPropPut->WriteToTextFile(pFile);
//
//  bPropPutRef = (mpPropPutRef != NULL);
//  writeBOOL(pFile, bPropPutRef);
//  if (bPropPutRef)
//    mpPropPutRef->WriteToTextFile(pFile);
//
//  bPropEvent = (mpEvent != NULL);
//  writeBOOL(pFile, bPropEvent);
//  if (bPropEvent)
//    mpEvent->WriteToTextFile(pFile);
//
//	bPropMethod = (mpMethods != NULL);
//  writeBOOL(pFile, bPropMethod);
//  if (mpMethods)
//  {
//    int nCount = (int)mpMethods->size(); 
//    writeInt(pFile, nCount);
//		for(size_t idx = 0; idx < (size_t)nCount; ++idx)
//      mpMethods->at(idx)->WriteToTextFile(pFile);
//  }
//	return statOK;
//}


void AxInterfaceDescriptor::Serialize(CArchive& ar, BYTE nPropertyVersion)
{
	BYTE nThisVersion = GetCurrentSaveVersion();
	
	if (ar.IsStoring())
	{
		ar << nThisVersion;
		ar << bool(mpProp != NULL);
		if( mpProp )
			mpProp->Serialize( ar, nPropertyVersion );
		ar << bool(mpPropGet != NULL);
		if( mpPropGet )
			mpPropGet->Serialize( ar, nPropertyVersion );
		ar << bool(mpPropPut != NULL);
		if( mpPropPut )
			mpPropPut->Serialize( ar, nPropertyVersion );
		ar << bool(mpPropPutRef != NULL);
		if( mpPropPutRef )
			mpPropPutRef->Serialize( ar, nPropertyVersion );
		ar << bool(mpEvent != NULL);
		if( mpEvent )
			mpEvent->Serialize( ar, nPropertyVersion );
		ar << bool(mpMethods != NULL);
		if( mpMethods )
		{
			unsigned short nSize = mpMethods->size();
			ar << nSize;
			for( int idx = 0; idx < nSize; ++idx )
				mpMethods->at( idx )->Serialize( ar, nPropertyVersion );
		}
	}
	else
	{
		if( nPropertyVersion <= 7 )
			nThisVersion = 1;
		else
			ar >> nThisVersion;
		bool bProp;
		BOOL bTemp;
		if( nThisVersion <= 1 )
		{
			ar >> bTemp;
			bProp = (bTemp != FALSE);
		}
		else
			ar >> bProp;
		if( bProp )
		{
			delete mpProp;
			mpProp = new AxPropertyDescriptor;
			mpProp->Serialize(ar, nPropertyVersion);
		}

		if( nThisVersion <= 1 )
		{
			ar >> bTemp;
			bProp = (bTemp != FALSE);
		}
		else
			ar >> bProp;
		if( bProp )
		{
			delete mpPropGet;
			mpPropGet = new AxPropertyDescriptor;
			mpPropGet->Serialize(ar, nPropertyVersion);
		}

		if( nThisVersion <= 1 )
		{
			ar >> bTemp;
			bProp = (bTemp != FALSE);
		}
		else
			ar >> bProp;
		if( bProp )
		{
			delete mpPropPut;
			mpPropPut = new AxPropertyDescriptor;
			mpPropPut->Serialize(ar, nPropertyVersion);
		}

		if( nThisVersion <= 1 )
		{
			ar >> bTemp;
			bProp = (bTemp != FALSE);
		}
		else
			ar >> bProp;
		if( bProp )
		{
			delete mpPropPutRef;
			mpPropPutRef = new AxPropertyDescriptor;
			mpPropPutRef->Serialize(ar, nPropertyVersion);
		}

		if( nThisVersion <= 1 )
		{
			ar >> bTemp;
			bProp = (bTemp != FALSE);
		}
		else
			ar >> bProp;
		if( bProp )
		{
			delete mpEvent;
			mpEvent = new AxEventDescriptor;
			mpEvent->Serialize(ar, nPropertyVersion);
		}
		if( nThisVersion <= 1 )
		{
			ar >> bTemp;
			bProp = (bTemp != FALSE);
		}
		else
			ar >> bProp;
		if( bProp )
		{
			delete mpMethods;
			mpMethods = new std::vector< RefCountedPtr< AxMethodDescriptor > >;
			unsigned short nCount;
			ar >> nCount;		
			while( nCount-- > 0 )
			{
				AxMethodDescriptor* pMethod = new AxMethodDescriptor;
				pMethod->Serialize( ar, nPropertyVersion );
				mpMethods->push_back( pMethod );		
			}
		}
	}
}

AxPropertyDescriptor* AxInterfaceDescriptor::GetEnumDescriptor() const
{
	if (mpPropGet && !mpPropGet->GetEnum().empty())
		return mpPropGet;
	if (mpProp && !mpProp->GetEnum().empty())
		return mpProp;			
	if (mpPropPut && !mpPropPut->GetEnum().empty())
		return mpPropPut;
	if (mpPropPutRef && !mpPropPutRef->GetEnum().empty())
		return mpPropPutRef;
	return NULL;
}

AxPropertyDescriptor* AxInterfaceDescriptor::GetArgDescriptor() const
{
	if (mpPropGet && !mpPropGet->GetArgs().empty())
		return mpPropGet;
	if (mpProp && !mpProp->GetArgs().empty())
		return mpProp;			
	if (mpPropPut && !mpPropPut->GetArgs().empty())
		return mpPropPut;
	if (mpPropPutRef && !mpPropPutRef->GetArgs().empty())
		return mpPropPutRef;
	return NULL;
}

size_t AxInterfaceDescriptor::GetParamQty() const
{
	AxPropertyDescriptor* pPropDesc = GetArgDescriptor();
	if (pPropDesc == NULL)
		return 0;
	return pPropDesc->GetArgs().size();
}

void AxInterfaceDescriptor::DoActiveXFontPropDlg(CAxContainerCtrl *axContainer)
{	
	try
	{
		LPDISPATCH pDispatch;
		axContainer->InvokeHelper( GetGetDispId(), DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&pDispatch, NULL );
		COleFont font( pDispatch );
		CString		sFontName = font.GetName();
		CY			cyFontSize = font.GetSize();
		BOOL		bFontUnderLine = font.GetUnderline();
		BOOL		bFontItalic = font.GetItalic();
		BOOL		bFontStrikethrough = font.GetStrikethrough();
		short		nFontWeight = font.GetWeight();
				
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		HDC hdc = ::GetDC(NULL);
		lf.lfHeight = ::MulDiv( ::MulDiv(cyFontSize.Lo, GetDeviceCaps(hdc, LOGPIXELSY), 72), -1, 10000 );
		::ReleaseDC(NULL, hdc);
		lf.lfWidth = 0;
		lstrcpyn(lf.lfFaceName, sFontName, _elements(lf.lfFaceName));
		lf.lfItalic = bFontItalic;
		lf.lfStrikeOut = bFontStrikethrough;
		lf.lfUnderline = bFontUnderLine;
		lf.lfWeight = nFontWeight;
		
		CFontDialog dlg(&lf);
		INT_PTR nResult = dlg.DoModal();

		if( nResult != IDOK )
			return;

		font.SetName(dlg.m_cf.lpLogFont->lfFaceName );
		font.SetSize( COleCurrency( dlg.m_cf.iPointSize / 10, 1000 * (dlg.m_cf.iPointSize % 10) ));
		font.SetWeight(short( dlg.m_cf.lpLogFont->lfWeight ));	
		font.SetUnderline(dlg.m_cf.lpLogFont->lfUnderline);
		font.SetItalic(dlg.m_cf.lpLogFont->lfItalic);
		font.SetStrikethrough(dlg.m_cf.lpLogFont->lfStrikeOut);		

		static BYTE parms[] = VTS_DISPATCH;
		axContainer->InvokeHelper(GetPutDispId(), DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, (LPDISPATCH)font);
		axContainer->SaveToStream();
	}
	catch(...)
	{
	}
}

CString AxInterfaceDescriptor::GetDesc() const
{
	if (mpPropGet != NULL)
		return mpPropGet->GetDesc();
	else if (mpPropPut != NULL)
		return mpPropPut->GetDesc();
	else if (mpPropPutRef != NULL)
		return mpPropPutRef->GetDesc();
	else if (mpProp != NULL)
		return mpProp->GetDesc();
	return CString();
}

CString AxInterfaceDescriptor::GetAxMethodDesc( size_t nIndex ) const
{
	if (mpMethods && nIndex < mpMethods->size())
		return mpMethods->at(nIndex)->GetDesc();
	return CString();
}

size_t AxInterfaceDescriptor::CountAxMethodParams( size_t nIndex ) const
{
	if (mpMethods && nIndex < mpMethods->size())
		return mpMethods->at(nIndex)->GetArgs().size();
	return 0;
}

const AxMethodDescriptor* AxInterfaceDescriptor::GetAxMethod( size_t nIndex ) const
{
	if (mpMethods && nIndex < mpMethods->size())
		return mpMethods->at(nIndex);
	return NULL;
}
	
VARTYPE AxInterfaceDescriptor::GetType() const
{
	VARTYPE vReturn = 0;
	if (mpPropGet != NULL)
		vReturn = mpPropGet->GetType();
	else if (mpProp != NULL)
		vReturn = mpProp->GetType();
	else if (mpPropPut != NULL)
		vReturn = (VARTYPE) mpPropPut->GetType();
	else if (mpPropPutRef != NULL)
		vReturn = (VARTYPE) mpPropPutRef->GetType();
	return vReturn;
}

GUID AxInterfaceDescriptor::GetGuid() const
{
	if (mpPropGet != NULL)
		return mpPropGet->GetGuid();
	else if (mpProp != NULL)
		return mpProp->GetGuid();
	else if (mpPropPut != NULL)
		return mpPropPut->GetGuid();
	else if (mpPropPutRef != NULL)
		return mpPropPutRef->GetGuid();
	GUID guid;
	::memset(&guid, 0, sizeof(GUID));	
	return guid;
}

DISPID AxInterfaceDescriptor::GetGetDispId() const
{
	if (mpPropGet != NULL)
		return mpPropGet->GetDispId();
	if (mpProp != NULL)
		return mpProp->GetDispId();
	return 0;
}

DISPID AxInterfaceDescriptor::GetPutDispId() const
{
	if (mpPropPut != NULL)
		return mpPropPut->GetDispId();
	else if (mpPropPutRef != NULL)
		return mpPropPutRef->GetDispId();
	else if (mpProp != NULL)
		return mpProp->GetDispId();
	return 0;
}

CString AxInterfaceDescriptor::GetName() const
{
	if (mpPropGet != NULL)
		return mpPropGet->GetName();
	if (mpProp != NULL)
		return mpProp->GetName();
	if (mpPropPut != NULL)
		return mpPropPut->GetName();
	if (mpPropPutRef != NULL)
		return mpPropPutRef->GetName();
	if (mpEvent != NULL)
		return mpEvent->GetName();
	return CString();
}

AxPropertyDescriptor* AxInterfaceDescriptor::GetGetDescriptor() const
{
	if( mpPropGet )
		return mpPropGet;
	return mpProp;
}

AxPropertyDescriptor* AxInterfaceDescriptor::GetPutDescriptor() const
{
	if( mpPropPutRef )
		return mpPropPutRef;
	if( mpPropPut )
		return mpPropPut;
	return mpProp;
}

#ifdef _DIAGNOSTIC
LPCTSTR AxInterfaceDescriptor::toString() const
{
	CString sProp;
	sProp.Format( _T("Prop: %s"), mpProp? mpProp->toString() : _T("<null>") );
	CString sPropGet;
	sPropGet.Format( _T("PropGet: %s"), mpPropGet? mpPropGet->toString() : _T("<null>") );
	CString sPropPut;
	sPropPut.Format( _T("PropPut: %s"), mpPropPut? mpPropPut->toString() : _T("<null>") );
	CString sPropPutRef;
	sPropPutRef.Format( _T("PropPutRef: %s"), mpPropPutRef? mpPropPutRef->toString() : _T("<null>") );
	CString sEvent;
	sEvent.Format( _T("Event: %s"), mpEvent? mpEvent->toString() : _T("<null>") );
	CString sMethods = _T("Methods: {");
	if( mpMethods )
	{
		if( mpMethods->size() == 0 )
			sMethods += _T("<empty>");
		else
		{
			sMethods += _T("{");
			sMethods += mpMethods->at(0)->toString();
			for( size_t idx = 1; idx < mpMethods->size(); ++idx )
			{
				sMethods += _T("},{");
				sMethods += mpMethods->at(idx)->toString();
			}
			sMethods += _T("}");
		}
	}
	else
		sMethods += _T("<null>");
	sMethods += _T("}");
	static CString sVal;
	sVal.Format( _T("{%s, %s, %s, %s, %s, %s}"),
							(LPCTSTR)sProp, (LPCTSTR)sPropGet, (LPCTSTR)sPropPut, (LPCTSTR)sPropPutRef,
							(LPCTSTR)sEvent, (LPCTSTR)sMethods );
	return sVal;
}
#endif
