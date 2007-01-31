// PropertyObject.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyObject.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxMethodDescriptor.h"
#include "AxContainer.h"
#include "Filing.h"
#include "OleFont.h"
#include "VarUtils.h"
#include "Workspace.h"
#include "Project.h"

static const int nNotSet = -1;
static const int nDePropResStringOffset = 210;
static const int nDePropDescResStringOffset = 2100;
static const int MAX_CALLING_ARGUMENTS = 16;


/////////////////////////////////////////////////////////////////////////////
// CPropertyObject

IMPLEMENT_SERIAL(CPropertyObject, CObject, 1)

CPropertyObject::CPropertyObject()
: mType( PropInvalid )
, mbHidden( false )
, mnID( -1 )
, mOLEColor( 0 )
, mlLong( 0 )
, mbBoolean( false )
, mdblDouble( 0 )
{	
	m_ImageListIndex = nNotSet;
	m_pAxProp = NULL;
	m_pAxPropGet = NULL;
	m_pAxPropPut = NULL;
	m_pAxPropPutRef = NULL;
	m_pAxEvent = NULL;
	m_pMethods = NULL;
}

CPropertyObject::CPropertyObject(PropertyTypes type)
: mType( type )
, mbHidden( false )
, mnID( -1 )
, mOLEColor( 0 )
, mlLong( 0 )
, mbBoolean( false )
, mdblDouble( 0 )
{	
	m_ImageListIndex = nNotSet;
	m_pAxProp = NULL;
	m_pAxPropGet = NULL;
	m_pAxPropPut = NULL;
	m_pAxPropPutRef = NULL;
	m_pAxEvent = NULL;
	m_pMethods = NULL;
}

CPropertyObject::~CPropertyObject()
{
	if (m_pAxProp != NULL)
	{
		delete m_pAxProp;
		m_pAxProp = NULL;
	}

	if (m_pAxEvent != NULL)
		delete m_pAxEvent;

	if (m_pAxPropGet != NULL)
		delete m_pAxPropGet;

	if (m_pAxPropPut != NULL)
		delete m_pAxPropPut;

	if (m_pAxPropPutRef != NULL)
		delete m_pAxPropPutRef;

	if (m_pMethods != NULL)
	{
		POSITION pos = m_pMethods->GetHeadPosition();
		while (pos != NULL)
		{
			AxMethodDescriptor *pAxMethod = m_pMethods->GetHead();
			m_pMethods->RemoveAt(pos);
			pos = m_pMethods->GetHeadPosition();
			delete pAxMethod;
		}
		delete m_pMethods;
		m_pMethods = NULL;
	}
}

const CString& CPropertyObject::GetStringValue() const
{
	assert(mType == PropString || mType == PropEvent);
	return msStringValue;
}

void CPropertyObject::SetStringValue( LPCTSTR pszValue )
{
	assert(mType == PropString || mType == PropEvent);
	msStringValue = pszValue;
}

const OLE_COLOR& CPropertyObject::GetOLEColorValue() const
{
	assert(mType == PropOLEColor);
	return mOLEColor;
}

void CPropertyObject::SetOLEColorValue( const OLE_COLOR& dwColor )
{
	assert(mType == PropOLEColor);
	mOLEColor = dwColor;
}

const long& CPropertyObject::GetLongValue() const
{
	assert(mType == PropLong || mType == PropEnum || mType == PropPicture);
	return mlLong;
}

void CPropertyObject::SetLongValue( long lValue )
{
	assert(mType == PropLong || mType == PropEnum || mType == PropPicture);
	mlLong = lValue;
}

bool CPropertyObject::GetBooleanValue() const
{
	assert(mType == PropBool);
	return mbBoolean;
}

void CPropertyObject::SetBooleanValue( bool bValue )
{
	assert(mType == PropBool);
	mbBoolean = bValue;
}

double CPropertyObject::GetDoubleValue() const
{
	assert(mType == PropDouble);
	return mdblDouble;
}

void CPropertyObject::SetDoubleValue( double dblValue )
{
	assert(mType == PropDouble);
	mdblDouble = dblValue;
}

CString CPropertyObject::GetActiveXEnumDesc(CString sValue)
{
	AxPropertyDescriptor *pProp;
	bool bFoundEnum = false;
	int nThisValue;

	if (!sValue.IsEmpty())
	{
		nThisValue = _tstol(sValue);
	}
	
	if (m_pAxPropGet != NULL)
	{
		if (m_pAxPropGet->NumEnum > 0)
		{
			bFoundEnum = true;
			pProp = m_pAxPropGet;
		}			
	}
	else if (m_pAxProp != NULL)
	{
		if (m_pAxProp->NumEnum > 0)
		{
			bFoundEnum = true;
			pProp = m_pAxProp;			
		}
	}
	else if (m_pAxPropPut != NULL)
	{
		if (m_pAxPropPut->NumEnum > 0)
		{
			bFoundEnum = true;
			pProp = m_pAxPropPut;
		}
	}	

	if (pProp == NULL)
	{
		CString sValue;
		sValue.Format(_T("%d"), nThisValue);
		return sValue;
	}

	if (bFoundEnum)
	{
		for (int i = 0; i<pProp->NumEnum; i++)
		{
			if (VariantToString(pProp->ArrEnum[i].Var) == sValue)				
				return sValue + _T('-') + pProp->ArrEnum[i].Name;		
		}
	}
	return CString();
}

bool CPropertyObject::GetActiveXEnum(/*CListBoxDlg *pListBox*/)
{
	AxPropertyDescriptor *pProp;
	//bool bFoundEnum = false;

	//if (pListBox == NULL)
	//{
		if (m_pAxPropGet != NULL)
		{
			pProp = m_pAxPropGet;
			if (pProp->NumEnum > 0)
				return true;
		}
		else if (m_pAxProp != NULL)
		{
			pProp = m_pAxProp;
			if (pProp->NumEnum > 0)
				return true;
		}
		else if (m_pAxPropPut != NULL)
		{
			pProp = m_pAxPropPut;
			if (pProp->NumEnum > 0)
				return true;
		}
		return false;
	//}
	//else
	//{
	//	if (m_pAxPropGet != NULL)
	//	{
	//		if (m_pAxPropGet->NumEnum > 0)
	//		{
	//			bFoundEnum = true;
	//			pProp = m_pAxPropGet;
	//		}			
	//	}
	//    else if (m_pAxProp != NULL)
	//	{
	//		if (m_pAxProp->NumEnum > 0)
	//		{
	//			bFoundEnum = true;
	//			pProp = m_pAxProp;			
	//		}
	//	}
	//	else if (m_pAxPropPut != NULL)
	//	{
	//		if (m_pAxPropPut->NumEnum > 0)
	//		{
	//			bFoundEnum = true;
	//			pProp = m_pAxPropPut;
	//		}
	//	}	
	//	if (pProp == NULL)
	//		return false;
	//}
	//if (bFoundEnum)
	//{
	//	for (int i=0; i<pProp->NumEnum; i++)
	//		pListBox->AddString(VariantToString(pProp->ArrEnum[i].Var) + _T('-') + pProp->ArrEnum[i].Name);
	//}
	//return true;
}

CString CPropertyObject::GetActiveXEnumValue(int nEnumIndex)
{	
	AxPropertyDescriptor *pProp = NULL;
	if (m_pAxPropGet != NULL)
	{
		pProp = m_pAxPropGet;					
	}
	else if (m_pAxProp != NULL)
	{
		pProp = m_pAxProp;
	}
	else if (m_pAxPropPut != NULL)
	{
		pProp = m_pAxPropPut;
	}	
	if (pProp == NULL)
		return CString();

	return VariantToString(pProp->ArrEnum[nEnumIndex].Var);
}

CString CPropertyObject::GetActiveXPropery(CAxContainer *axContainer)
{
	HRESULT hr = - 1;
	HRESULT hrGet = - 1;
	HRESULT hrPut = - 1;
	CString sResult;

	if (m_pAxPropGet != NULL)
	{
		if (m_pAxPropGet->Type == VT_DISPATCH ||
			m_pAxPropGet->Type == VT_UNKNOWN)
			return CString();
		hrGet = axContainer->GetProperty(m_pAxPropGet, sResult);
		if (FAILED(hrGet))
		{
			return CString();
			//mType = PropActiveXRunTime;
		}
	
	}
	else if (m_pAxProp != NULL)
	{
		hr = axContainer->GetProperty(m_pAxProp, sResult);
		if (FAILED(hr))
		{
			//sResult = theWorkspace.LoadResourceString(IDS_RUNTIME);
			//mType = PropActiveXRunTime;
		}
	}
	else if (m_pAxPropPutRef != NULL)
	{
		hrGet = axContainer->GetProperty(m_pAxPropPutRef, sResult);
		if (FAILED(hrGet))
		{
			//sResult = theWorkspace.LoadResourceString(IDS_RUNTIME);
			//mType = PropActiveXRunTime;
		}
	
	}
	else if (m_pAxPropPut != NULL)
	{
		//sResult = theWorkspace.LoadResourceString(IDS_RUNTIME);
		//mType = PropActiveXRunTime;		
	}
	
	return sResult;
}

int CPropertyObject::GetActiveXParamQty()
{
	int nQtyGet = 0;
	int nQty = 0;
	int nQtyPut = 0;
	if (m_pAxPropGet != NULL)
	{
		nQtyGet = m_pAxPropGet->NumParams;
		if (nQtyGet > 0)
			return nQtyGet;
	}
	if (m_pAxProp != NULL)
	{
		nQty = m_pAxProp->NumParams;
		if (m_pAxProp->invKind == INVOKE_PROPERTYGET && nQty > 0)
		{
			//mType = PropActiveXRunTime;
			return nQty;
		}
	}
	if (m_pAxPropPut != NULL)
	{
		nQtyPut = m_pAxPropPut->NumParams;
	}

	if (nQtyGet > nQty && nQtyGet > nQtyPut)
		return nQtyGet;
	
	else if (nQty > nQtyGet && nQty > nQtyPut)
		return nQty;
	
	else if (nQtyPut > nQty && nQtyPut > nQtyGet)
		return nQtyPut;
	
	else
		return 0;
}

void CPropertyObject::DoActiveXFontPropDlg(CAxContainer *axContainer)
{	
	try
	{
		COleFont font;
		if (m_pAxPropGet != NULL)
			font = axContainer->GetFont(m_pAxPropGet->Id);
		else if (m_pAxProp != NULL)
			font = axContainer->GetFont(m_pAxProp->Id);
		else if (m_pAxPropPut != NULL)
			font = axContainer->GetFont(m_pAxPropPut->Id);
		else
			return;

		CString		sFontName = font.GetName();
		CY			cyFontSize = font.GetSize();
		BOOL		bFontBold = font.GetBold();
		BOOL		bFontUnderLine = font.GetUnderline();
		BOOL		bFontItalic = font.GetItalic();
		BOOL		bFontStrikethrough = font.GetStrikethrough();
		short		nFontWeight = font.GetWeight();
		short		nFontCharset = font.GetCharset();
				
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));

		COleCurrency size = COleCurrency( 10 / cyFontSize.Lo, 
		  (10 % cyFontSize.Lo) / 1000);

		int nH = size.m_cur.Lo;
		HDC hdc = ::GetDC(GetDesktopWindow());
		lf.lfHeight = -::MulDiv(cyFontSize.Lo,
							GetDeviceCaps(hdc, LOGPIXELSY), 72);

		lf.lfHeight = lf.lfHeight / 1000;
		lf.lfWidth = 0;
		lstrcpy(lf.lfFaceName, sFontName);
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
		
		if (m_pAxPropPut != NULL)
			axContainer->SetFont(m_pAxPropPut->Id, font);
		else if (m_pAxProp != NULL)
			axContainer->SetFont(m_pAxProp->Id, font);
		else if (m_pAxPropGet != NULL)
			axContainer->SetFont(m_pAxPropGet->Id, font);
		
	}
	catch(...)
	{
	}
}


void CPropertyObject::SetActiveXPropery(CAxContainer *axContainer, CString sNewValue)
{
	if (m_pAxPropPut != NULL)
		axContainer->SetProperty(m_pAxPropPut, sNewValue);
	else if (m_pAxPropPutRef != NULL)
		axContainer->SetProperty(m_pAxPropPutRef, sNewValue);
	else if (m_pAxProp != NULL)
		axContainer->SetProperty(m_pAxProp, sNewValue);
	
	
}

CString CPropertyObject::GetName()
{
	if (mType == PropActiveXMethods)
	{
		//CString sName;
		//sName = theWorkspace.LoadResourceString(IDS_AXMETHODS);
		//return sName;
		return _T("(Object Browser)");
	}
	if (mType >= PropActiveXProp)
	{		
		if (m_pAxPropGet != NULL)
			return m_pAxPropGet->Name;
		else if (m_pAxProp != NULL)
			return m_pAxProp->Name;
		else if (m_pAxPropPut != NULL)
			return m_pAxPropPut->Name;
		else if (m_pAxPropPutRef != NULL)
			return m_pAxPropPutRef->Name;
		else if (m_pAxEvent != NULL)
			return m_pAxEvent->Name;
		return CString();
	}		
	else
	{
		CString sDesc;
		sDesc = theWorkspace.LoadResourceString(mnID + nDePropResStringOffset);
		return sDesc;
	}
}
	
CString CPropertyObject::GetDocumentationDesc()
{
	if (mType >= PropActiveXProp)
	{		
		if (m_pAxPropGet != NULL)
			return m_pAxPropGet->DocumentationDesc;
		else if (m_pAxPropPut != NULL)
			return m_pAxPropPut->DocumentationDesc;
		else if (m_pAxPropPutRef != NULL)
			return m_pAxPropPutRef->DocumentationDesc;
		else if (m_pAxProp != NULL)
			return m_pAxProp->DocumentationDesc;
		
	}
	else
	{
		CString sDesc;
		sDesc = theWorkspace.LoadResourceString(mnID + nDePropDescResStringOffset);
		return sDesc;
	}
	return CString();
}

CString CPropertyObject::GetAxMethodDesc(int nIndex)
{
	if (m_pMethods != NULL)		
	{
		POSITION pos = m_pMethods->FindIndex(nIndex);
		if (pos != NULL)
		{			
			CString sDesc = m_pMethods->GetAt(pos)->Desc;
			return sDesc;
		}
	}
	return CString();
}



int CPropertyObject::GetAxMethodParams(int nIndex)
{
	if (m_pMethods != NULL)		
	{
		POSITION pos = m_pMethods->FindIndex(nIndex);
		if (pos != NULL)
		{			
			return m_pMethods->GetAt(pos)->nParamQty;
		}
	}
	return 0;
}

CString CPropertyObject::GetAxMethodParamName(int nIndex, int nParam)
{
	if (m_pMethods != NULL)		
	{
		POSITION pos = m_pMethods->FindIndex(nIndex);
		if (pos != NULL)
		{			
			return m_pMethods->GetAt(pos)->CallingArgNames[nParam];
		}
	}
	return CString();
}

VARTYPE CPropertyObject::GetAxMethodReturnType(int nIndex)
{
	if (m_pMethods != NULL)		
	{
		POSITION pos = m_pMethods->FindIndex(nIndex);
		if (pos != NULL)
		{			
			return m_pMethods->GetAt(pos)->ReturnType;
		}
	}
	return 0;
}


AxMethodDescriptor * CPropertyObject::GetAxMethod(int nIndex)
{
	if (m_pMethods != NULL)		
	{
		POSITION pos = m_pMethods->FindIndex(nIndex);
		if (pos != NULL)
		{			
			return m_pMethods->GetAt(pos);
		}
	}
	return NULL;
}

CString CPropertyObject::GetAxMethodParamVarType(int nIndex, int nParam)
{
	AxMethodDescriptor *pAxMethod = NULL;
	if (m_pMethods != NULL)		
	{
		POSITION pos = m_pMethods->FindIndex(nIndex);
		if (pos != NULL)
		{			
			pAxMethod = m_pMethods->GetAt(pos);
			return VARTYPEtoString(pAxMethod->CallingArgs[nParam]);
		}
	}
	return CString();
}

GUID CPropertyObject::GetAxMethodParamGUID(int nIndex, int nParam)
{
	GUID guid;
	::memset(&guid, 0, sizeof(GUID));	
	if (m_pMethods != NULL)		
	{
		POSITION pos = m_pMethods->FindIndex(nIndex);
		if (pos != NULL)
		{			
			return m_pMethods->GetAt(pos)->CallingArgClsids[nParam];
		}
	}
	return guid;
}

void CPropertyObject::SetActiveXProperyName(CString sName)
{
	
}
	
VARTYPE CPropertyObject::GetActiveXProperyType()
{
	VARTYPE vReturn = 0;
	if (m_pAxPropGet != NULL)
		vReturn = m_pAxPropGet->Type;
	else if (m_pAxProp != NULL)
		vReturn = m_pAxProp->Type;
	else if (m_pAxPropPut != NULL)
		vReturn = (VARTYPE) m_pAxPropPut->Type;
	else if (m_pAxPropPutRef != NULL)
		vReturn = (VARTYPE) m_pAxPropPutRef->Type;

	return vReturn;
}

GUID CPropertyObject::GetActiveXProperyGuid()
{
	if (m_pAxPropGet != NULL)
		return m_pAxPropGet->Guid;
	else if (m_pAxProp != NULL)
		return m_pAxProp->Guid;
	else if (m_pAxPropPut != NULL)
		return m_pAxPropPut->Guid;
	else if (m_pAxPropPutRef != NULL)
		return m_pAxPropPutRef->Guid;

	GUID guid;
	::memset(&guid, 0, sizeof(GUID));	
	return guid;
}

DISPID CPropertyObject::GetActiveXGetProperyId()
{
	if (m_pAxPropGet != NULL)
		return m_pAxPropGet->Id;
	else if (m_pAxProp != NULL)
		return m_pAxProp->Id;
	else if (m_pAxPropPut != NULL)
		return m_pAxPropPut->Id;
	else if (m_pAxPropPutRef != NULL)
		return m_pAxPropPutRef->Id;

	DISPID dispid;
	::memset(&dispid, 0, sizeof(DISPID));	
	return dispid;
}

DISPID CPropertyObject::GetActiveXSetProperyId()
{
	if (m_pAxPropPut != NULL)
		return m_pAxPropPut->Id;
	else if (m_pAxPropPutRef != NULL)
		return m_pAxPropPutRef->Id;
	else if (m_pAxProp != NULL)
		return m_pAxProp->Id;
	else if (m_pAxPropGet != NULL)
		return m_pAxPropGet->Id;
	
	DISPID dispid;
	::memset(&dispid, 0, sizeof(DISPID));	
	return dispid;
}

void CPropertyObject::Copy(CPropertyObject *other)
{
	mType = other->mType;
	mnID = other->mnID;
	mbHidden = other->mbHidden;

	mOLEColor = other->mOLEColor;
	msStringValue = other->msStringValue;
	mlLong = other->mlLong;
	mdblDouble = other->mdblDouble;
	mbBoolean = other->mbBoolean;
	
	m_pAxProp = other->m_pAxProp;
	m_pAxPropGet = other->m_pAxPropGet;
	m_pAxPropPut = other->m_pAxPropPut;
	m_pAxPropPutRef = other->m_pAxPropPutRef;
	m_pAxEvent = other->m_pAxEvent;
	m_pMethods = other->m_pMethods;

	
	POSITION pos;
	CString pItem;
	int nCount = 0;
	
	// do a loop to copy all the string items over to the string list of this object
	while(nCount < other->m_stringList.GetCount())
	{
		// set the position variable to be equal the index to passing to the GetAt method
		pos = other->m_stringList.FindIndex(nCount);	

		// create a new String list item and point it at the object in the list
		pItem = other->m_stringList.GetAt(pos);
		m_stringList.AddTail(pItem);
		nCount++;
	}

	pos = other->m_stringArrayList.GetHeadPosition();

	// do a loop to copy all the string items over to the string list of this object
	while (pos != NULL)
	{
		CStringArray *pStringArray = NULL;

		if (other->m_stringArrayList.GetAt(pos) != NULL)
		{
			pStringArray = new CStringArray();

			pStringArray->Copy(*other->m_stringArrayList.GetAt(pos));			
		}
		
		m_stringArrayList.AddTail(pStringArray);

		other->m_stringArrayList.GetNext(pos);		
	}
	
	pos = other->m_intArrayList.GetHeadPosition();

	// do a loop to copy all the string items over to the string list of this object
	while (pos != NULL)
	{
		CArray<int, int> *pIntArray = NULL;

		if (other->m_intArrayList.GetAt(pos) != NULL)
		{
			pIntArray = new CArray<int, int>;

			pIntArray->Copy(*other->m_intArrayList.GetAt(pos));			
		}
		
		m_intArrayList.AddTail(pIntArray);

		other->m_intArrayList.GetNext(pos);		
	}
	

	for (int i=0; i<m_intList.GetSize(); i++)
	{
		m_intList.Add(other->m_intList[i]);
	}

	m_ImageListIndex = nNotSet;
}

void CPropertyObject::AddProperty(PropertyTypes ValueType, CString strValue)
{
	mType = ValueType;
	SetProperty(strValue);
}

void CPropertyObject::AddStringItem(CString NewString)
{
	if (mType == PropIntArray)
	{
		m_intList.Add(_tstol(NewString));
	}
	else
	{		
		m_stringList.AddTail(NewString);
	}
}

void CPropertyObject::ClearList()
{
	POSITION pos = m_stringArrayList.GetHeadPosition();

	while (pos != NULL)
	{
		if (m_stringArrayList.GetAt(pos) != NULL)
		{		
			delete m_stringArrayList.GetAt(pos);
		}
		m_stringArrayList.GetNext(pos);
	}
	m_stringArrayList.RemoveAll();

	pos = m_intArrayList.GetHeadPosition();

	while (pos != NULL)
	{
		if (m_intArrayList.GetAt(pos) != NULL)
		{		
			delete m_intArrayList.GetAt(pos);
		}
		m_intArrayList.GetNext(pos);
	}
	m_intArrayList.RemoveAll();

	m_stringList.RemoveAll();
	m_intList.RemoveAll();
	

	if (m_pAxPropPutRef != NULL)
	{
		if (m_pAxPropPutRef->ArrEnum != NULL)
		{
			delete [] m_pAxPropPutRef->ArrEnum;
			m_pAxPropPutRef->ArrEnum = NULL;
		}		
		delete m_pAxPropPutRef;
		
	}
	if (m_pAxPropPut != NULL)
	{
		if (m_pAxPropPut->ArrEnum != NULL)
		{
			delete [] m_pAxPropPut->ArrEnum;
			m_pAxPropPut->ArrEnum = NULL;
		}		
		delete m_pAxPropPut;
		
	}
	if (m_pAxPropGet != NULL)
	{
		if (m_pAxPropGet->ArrEnum != NULL)
		{
			delete [] m_pAxPropGet->ArrEnum;
			m_pAxPropGet->ArrEnum = NULL;
		}		
		delete m_pAxPropGet;		
		
	}
	if (m_pAxProp != NULL)
	{
		if (m_pAxProp->ArrEnum != NULL)
		{
			delete [] m_pAxProp->ArrEnum;
			m_pAxProp->ArrEnum = NULL;
		}		
		delete m_pAxProp;
		
	}
	m_pAxPropPutRef = NULL;
	m_pAxPropPut = NULL;
	m_pAxPropGet = NULL;
	m_pAxProp = NULL;
}

INT_PTR CPropertyObject::CountList()
{
	
	if (m_intArrayList.GetCount() > 0)
		return m_intArrayList.GetCount();

	if (m_stringArrayList.GetCount() > 0)
		return m_stringArrayList.GetCount();

	if (m_intList.GetSize() > 0)
		return m_intList.GetSize();

	if (m_stringList.GetCount() > 0)
		return m_stringList.GetCount();

	return 0;
}

CString CPropertyObject::GetStringItem(short ListIndex)
{
	// create a position variable to hold the converted ListIndex
	POSITION Pos;
	CString pItem;
	
	if(ListIndex < m_stringList.GetCount())
	{
		// set the position variable to be equal the index to passing to the GetAt method
		Pos = m_stringList.FindIndex(ListIndex);	

		// create a new String list item and point it at the object in the list
		pItem = m_stringList.GetAt(Pos);
	}
	else if(ListIndex < m_intList.GetSize())
	{
		// create a new String list item and point it at the object in the list
		pItem.Format(_T("%d"), m_intList[ListIndex]);
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
	switch (mType)
	{
	case PropEnum:	
	case PropPicture:
	case PropLong:	
		{
		RetString.Format(_T("%d"), mlLong);
		break;
		}
	case PropEvent:
	case PropString:
		{
		RetString = msStringValue;
		break;
		}
	case PropDouble:
		{
		RetString.Format(_T("%.8f"), mdblDouble);
		break;
		}
	case PropBool:
		{			
		if (GetBooleanValue() == TRUE)
			RetString = _T("TRUE");
		else
			RetString = _T("FALSE");
		
		break;
		}
	}	
	return RetString;
}

void CPropertyObject::SetProperty(CString strValue)
{
	theWorkspace.SetModified(true);

	switch (mType)
	{
	case PropEnum:
	case PropLong:		
	case PropPicture:
		{
		mlLong = _tstol(strValue);
		break;
		}
	case PropEvent:
	case PropString:
		{
		msStringValue = strValue;	
		break;
		}
	case PropDouble:
		{
		mdblDouble = _tstof(strValue);
		break;
		}
	case PropBool:
		{
		mbBoolean = (strValue.CompareNoCase(_T("TRUE")) == 0);
		break;
		}		
	case PropImageList:
		m_ImageListIndex = nNotSet;
		break;
	case PropOLEColor:
		mOLEColor = strValue.IsEmpty()? RGB(0,0,255) : RGB(0,0,0);
		break;
	}			

}

IOStatus CPropertyObject::WriteActiveXPropToTextFile(FILE* pFile, AxPropertyDescriptor *axProp) const
{
  int i=0;
  LONG Id = (LONG)axProp->Id;
  writeLong(pFile, Id);
  writeString(pFile, axProp->Name);
  writeString(pFile, axProp->DocumentationDesc);
  writeVARTYPE(pFile, axProp->Type);
  writeBOOL(pFile, axProp->IsArray);
  writeBOOL(pFile, axProp->CanSet);
  writeCLSID(pFile, axProp->Guid);
  writeInt(pFile, axProp->NumEnum);
  writeInt(pFile, axProp->NumParams);
  int iKind = axProp->invKind;
  writeInt(pFile, iKind);
  if (axProp->NumEnum > 0)
  {
    for (i=0; i<axProp->NumEnum; i++)
    {
      writeString(pFile, axProp->ArrEnum[i].Name);
      COleVariant var = axProp->ArrEnum[i].Var;
      writeOleVariant(pFile, var);
    }
  }		
  for (i = 0; i<axProp->NumParams; i++)
  {
    writeVARTYPE(pFile, axProp->CallingArgs[i]);
    writeString(pFile, axProp->CallingArgNames[i]);
    writeCLSID(pFile, axProp->CallingArgClsids[i]);
  }
	return statOK;
}

void CPropertyObject::SerializeActiveXProp(CArchive& ar, AxPropertyDescriptor *axProp, int nPropertyVersion)
{
	int i=0;
	if (ar.IsStoring())
	{
		LONG Id = (LONG)axProp->Id;
		ar << Id;
		ar << axProp->Name;
		ar << axProp->DocumentationDesc;
		ar << axProp->Type;
		ar << axProp->IsArray;
		ar << axProp->CanSet;
		SerializeCLSID(ar, axProp->Guid);
		ar << axProp->NumEnum;
		ar << axProp->NumParams;
		int iKind = axProp->invKind;
		ar << iKind;
		if (axProp->NumEnum > 0)
		{
			for (i=0; i<axProp->NumEnum; i++)
			{
				ar << axProp->ArrEnum[i].Name;
				COleVariant var = axProp->ArrEnum[i].Var;
				ar << var;
			}
		}		
		for (i = 0; i<axProp->NumParams; i++)
		{
			ar << axProp->CallingArgs[i];
			ar << axProp->CallingArgNames[i];
			SerializeCLSID(ar, axProp->CallingArgClsids[i]);
		}
	}
	else
	{
		LONG Id;
		ar >> Id; axProp->Id = Id;
		ar >> axProp->Name;
		ar >> axProp->DocumentationDesc;


		ar >> axProp->Type; 
		ar >> axProp->IsArray;
		ar >> axProp->CanSet;
		
		SerializeCLSID(ar, axProp->Guid);		
		ar >> axProp->NumEnum;
		ar >> axProp->NumParams;
		int iKind;
		ar >> iKind;
		switch(iKind)
		{
			case INVOKE_FUNC:
				axProp->invKind = INVOKE_FUNC;
				break;
			case INVOKE_PROPERTYGET:
				axProp->invKind = INVOKE_PROPERTYGET;
				break;
			case INVOKE_PROPERTYPUT:
				axProp->invKind = INVOKE_PROPERTYPUT;
				break;
			case INVOKE_PROPERTYPUTREF:
				axProp->invKind = INVOKE_PROPERTYPUTREF;
				break;
			default:
				axProp->invKind = (INVOKEKIND)iKind;
				break;
		}
		if (axProp->NumEnum > 0)
		{
			axProp->ArrEnum = new AxPropertyEnum[axProp->NumEnum];
			for (i=0; i<axProp->NumEnum; i++)
			{
				ar >> axProp->ArrEnum[i].Name;
				COleVariant var;
				ar >> var; 
        axProp->ArrEnum[i].Var = var;
			}
		}
		for (i = 0; i<axProp->NumParams; i++)
		{
			ar >> axProp->CallingArgs[i];
			ar >> axProp->CallingArgNames[i];
			if (nPropertyVersion >= 4)
				SerializeCLSID(ar, axProp->CallingArgClsids[i]);
		}
	}

}

IOStatus CPropertyObject::WriteActiveXInfoToTextFile(FILE* pFile) const
{
  BOOL bProp;
  BOOL bPropGet;
  BOOL bPropPut;
  BOOL bPropPutRef;
  BOOL bPropEvent;
  BOOL bPropMethod;

  bProp = (m_pAxProp != NULL);
  writeBOOL(pFile, bProp);
  if (bProp)
    WriteActiveXPropToTextFile(pFile, m_pAxProp);

  bPropGet = (m_pAxPropGet != NULL);
  writeBOOL(pFile, bPropGet);
  if (bPropGet)
    WriteActiveXPropToTextFile(pFile, m_pAxPropGet);

  bPropPut = (m_pAxPropPut != NULL);
  writeBOOL(pFile, bPropPut);
  if (bPropPut)
    WriteActiveXPropToTextFile(pFile, m_pAxPropPut);

  bPropPutRef = (m_pAxPropPutRef != NULL);
  writeBOOL(pFile, bPropPutRef);
  if (bPropPutRef)
    WriteActiveXPropToTextFile(pFile, m_pAxPropPutRef);

  bPropEvent = (m_pAxEvent != NULL);
  writeBOOL(pFile, bPropEvent);

  if (m_pAxEvent != NULL)
  {
    writeDISPID(pFile, m_pAxEvent->Id);
    writeString(pFile, m_pAxEvent->Name);
    writeString(pFile, m_pAxEvent->DocumentationDesc);
    writeInt(pFile, m_pAxEvent->nArgs);
    for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
    {
      writeVARTYPE(pFile, m_pAxEvent->CallingArgs[i]);
      writeString(pFile, m_pAxEvent->CallingArgNames[i]);
      writeCLSID(pFile, m_pAxEvent->CallingArgClsids[i]);
  }
    writeString(pFile, m_pAxEvent->Params);
  }
  bPropMethod = (m_pMethods != NULL);
  writeBOOL(pFile, bPropMethod);
  if (m_pMethods != NULL)
  {
    int nCount = (int)m_pMethods->GetCount(); 
    writeInt(pFile, nCount);

    // set start position for navigating objects
    POSITION pos = m_pMethods->GetHeadPosition();

    // do loop to navigate objects
    while (pos != NULL)
    {
      // get current object
      AxMethodDescriptor* pMethod = m_pMethods->GetNext(pos);

      // put object into archive
      pMethod->WriteToTextFile(pFile);
    }
  }
	return statOK;
}

void CPropertyObject::SerializeActiveXInfo(CArchive& ar, int nPropertyVersion)
{
	BOOL bProp;
	BOOL bPropGet;
	BOOL bPropPut;
	BOOL bPropPutRef;
	BOOL bPropEvent;
	BOOL bPropMethod;

	if (ar.IsStoring())
	{
		bProp = (m_pAxProp != NULL);
		ar << bProp;
		if (bProp)
			SerializeActiveXProp(ar, m_pAxProp, nPropertyVersion);

		bPropGet = (m_pAxPropGet != NULL);
		ar << bPropGet;
		if (bPropGet)
			SerializeActiveXProp(ar, m_pAxPropGet, nPropertyVersion);

		bPropPut = (m_pAxPropPut != NULL);
		ar << bPropPut;
		if (bPropPut)
			SerializeActiveXProp(ar, m_pAxPropPut, nPropertyVersion);

		bPropPutRef = (m_pAxPropPutRef != NULL);
		ar << bPropPutRef;
		if (bPropPutRef)
			SerializeActiveXProp(ar, m_pAxPropPutRef, nPropertyVersion);

		bPropEvent = (m_pAxEvent != NULL);
		ar << bPropEvent;

		if (m_pAxEvent != NULL)
		{
			ar << m_pAxEvent->Id;
			ar << m_pAxEvent->Name;
			ar << m_pAxEvent->DocumentationDesc;
			ar << m_pAxEvent->nArgs;
			for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
			{
				 ar << m_pAxEvent->CallingArgs[i];
				 ar << m_pAxEvent->CallingArgNames[i];
				 SerializeCLSID(ar, m_pAxEvent->CallingArgClsids[i]);
			}				
			ar << m_pAxEvent->Params;
		}
		bPropMethod = (m_pMethods != NULL);
		ar << bPropMethod;
		if (m_pMethods != NULL)
		{
			int nCount = (int)m_pMethods->GetCount(); 
			ar << nCount;

			// set start position for navigating objects
			POSITION pos = m_pMethods->GetHeadPosition();

			// do loop to navigate objects
			while (pos != NULL)
			{
				// get current object
				AxMethodDescriptor* pMethod = m_pMethods->GetNext(pos);

				// put object into archive
				pMethod->Serialize(ar);				
			}
		}
	}
	else
	{
		ar >> bProp;
		if (bProp)
		{
			m_pAxProp = new AxPropertyDescriptor;
			SerializeActiveXProp(ar, m_pAxProp, nPropertyVersion);
		}

		ar >> bPropGet;
		if (bPropGet)
		{
			m_pAxPropGet = new AxPropertyDescriptor;
			SerializeActiveXProp(ar, m_pAxPropGet, nPropertyVersion);
		}

		ar >> bPropPut;
		if (bPropPut)
		{
			m_pAxPropPut = new AxPropertyDescriptor;
			SerializeActiveXProp(ar, m_pAxPropPut, nPropertyVersion);
		}

		ar >> bPropPutRef;
		if (bPropPutRef)
		{
			m_pAxPropPutRef = new AxPropertyDescriptor;
			SerializeActiveXProp(ar, m_pAxPropPutRef, nPropertyVersion);
		}

		ar >> bPropEvent;
		if (bPropEvent)
		{
			m_pAxEvent = new AxEventDescriptor;
			ar >> m_pAxEvent->Id;
			ar >> m_pAxEvent->Name;
			ar >> m_pAxEvent->DocumentationDesc;


			ar >> m_pAxEvent->nArgs;
			for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
			{
				 ar >> m_pAxEvent->CallingArgs[i];
				 ar >> m_pAxEvent->CallingArgNames[i];


				 if (nPropertyVersion >= 4)
					SerializeCLSID(ar, m_pAxEvent->CallingArgClsids[i]);
		}
			ar >> m_pAxEvent->Params;
		}
		ar >> bPropMethod;
		if (bPropMethod)
		{
			m_pMethods = new CList<AxMethodDescriptor*>;
			int nCount;
		
			ar >> nCount;		

			m_pMethods->RemoveAll();
			
			// do loop to navigate objects
			while (nCount-- > 0)
			{
				// get current object
				AxMethodDescriptor* pMethod = new AxMethodDescriptor;
				
				// get object from archive
				pMethod->Serialize(ar);
		
				// add that ArxControlObject to the list object
				m_pMethods->AddTail(pMethod);		
			}
		}
	}
}

IOStatus CPropertyObject::WriteToTextFile(FILE* pFile) const
{
  fprintf(pFile, "\nCPropertyObject");
  int nThisVersion = 5;
  writeInt(pFile, nThisVersion);
  writeInt(pFile, mnID);
  writeInt(pFile, mType);
  // store the flag that indicates this property is to be hidden
  writeBOOL(pFile, mbHidden);

  fprintf(pFile, "\n");
  switch (mType) {
    case PropEnum: fprintf(pFile, "PropEnum"); break;
    case PropPicture: fprintf(pFile, "PropPicture"); break;
    case PropLong: fprintf(pFile, "PropLong"); break;
    case PropEvent: fprintf(pFile, "PropEvent"); break;
    case PropString: fprintf(pFile, "PropString"); break;
    case PropDouble: fprintf(pFile, "PropDouble"); break;
    case PropBool: fprintf(pFile, "PropBool"); break;
    case PropOLEColor: fprintf(pFile, "PropOLEColor"); break;
    case PropStringArrayList: fprintf(pFile, "PropStringArrayList"); break;
    case PropIntArrayList: fprintf(pFile, "PropIntArrayList"); break;
    case PropStringArray: fprintf(pFile, "PropStringArray"); break;
    case PropIntArray: fprintf(pFile, "PropIntArray"); break;
    case PropImageList: fprintf(pFile, "PropImageList"); break;
    case PropActiveXEvent: fprintf(pFile, "PropActiveXEvent"); break;
    case PropActiveXProp: fprintf(pFile, "PropActiveXProp"); break;
    case PropActiveXPropPages: fprintf(pFile, "PropActiveXPropPages"); break;
    case PropActiveXEnum: fprintf(pFile, "PropActiveXEnum"); break;
    case PropActiveXRunTime: fprintf(pFile, "PropActiveXRunTime"); break;
    case PropActiveXMethods: fprintf(pFile, "PropActiveXMethods"); break;
  }

  switch (mType)
  {
    case PropEnum:
    case PropPicture:
    case PropLong:
      {
        writeLong(pFile, mlLong);
        break;
      }
    case PropEvent:
    case PropString:
      {
        writeString(pFile, msStringValue);
        break;
      }
    case PropDouble:
      {
        writeDouble(pFile, mdblDouble);
        break;
      }
    case PropBool:
      {
        writeBOOL(pFile, mbBoolean);
        break;
      }
    case PropOLEColor:
      {
        writeDWORD(pFile, mOLEColor);
        break;
      }
    case PropStringArrayList:
      {
        writeInt(pFile, (int)m_stringArrayList.GetCount());
        for (POSITION pos = m_stringArrayList.GetHeadPosition(); pos != NULL; m_stringArrayList.GetNext(pos)) {
          CStringArray* stringArray = m_stringArrayList.GetAt(pos);
          if (stringArray != NULL) {
            writeInt(pFile, (int)stringArray->GetCount());
            for (int i = 0; i < stringArray->GetCount(); i++) {
              writeString(pFile, stringArray->GetAt(i));
            }
          } else {
            writeInt(pFile, -1);
          }
        }
        break;
      }
    case PropIntArrayList:
      {
        writeInt(pFile, (int)m_intArrayList.GetCount());
        POSITION pos = m_intArrayList.GetHeadPosition();

        while (pos != NULL)
        {
          CArray<int, int> *pIntArray = m_intArrayList.GetNext(pos);

          if (pIntArray == NULL)
          {
            writeInt(pFile, 0);
          }
          else
          {
            writeInt(pFile, (int)pIntArray->GetSize());
            for (int j=0; j < pIntArray->GetSize(); j++)
            {			
              writeInt(pFile, pIntArray->GetAt(j));
            }
          }		
        }
        break;
      }
    case PropStringArray:
      {
        writeInt(pFile, (int)m_stringList.GetCount());
        for (POSITION pos = m_stringList.GetHeadPosition(); pos != NULL; m_stringList.GetNext(pos)) {
          writeString(pFile, m_stringList.GetAt(pos));
        }
        break;
      }
    case PropIntArray:
      {
        writeInt(pFile, (int)m_intList.GetCount());
        for (int i = 0; i < m_intList.GetCount(); i++) {
          writeInt(pFile, m_intList.GetAt(i));
        }
        break;
      }			
    case PropImageList:
      {
        writeShort(pFile, m_ImageListIndex);
        break;
      }	
    case PropActiveXEvent:
      {
        writeString(pFile, msStringValue);
        // call the method to save the activeX info
        WriteActiveXInfoToTextFile(pFile);
        break;
      }
    case PropActiveXProp:
      writeString(pFile, msStringValue);
      // call the method to save the activeX info
      WriteActiveXInfoToTextFile(pFile);
      break;
    case PropActiveXPropPages:			
    case PropActiveXEnum:
    case PropActiveXRunTime:
    case PropActiveXMethods:
      {
        // call the method to save the activeX info
        WriteActiveXInfoToTextFile(pFile);
        break;
      }	
  }
	return statOK;
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
		ar << long(mType);
		if (nThisVersion <= 5) //changing from BOOL to bool in version 6 [ORW]
			ar << BOOL(mbHidden); // store the flag that indicates this property is to be hidden
		else
			ar << bool(mbHidden); // store the flag that indicates this property is to be hidden
		
		switch (mType)
		{
		case PropEnum:
		case PropPicture:
		case PropLong:
			{
			ar << mlLong;
			break;
			}
		case PropEvent:
		case PropString:
			{
			ar << msStringValue;			
			break;
			}
		case PropDouble:
			{
			ar << mdblDouble;
			break;
			}
		case PropBool:
			{
				if (nThisVersion <= 5) //changing from BOOL to bool in version 6 [ORW]
					ar << BOOL(mbBoolean);
				else
					ar << bool(mbBoolean);
				break;
			}
		case PropOLEColor:
			{
			ar << mOLEColor;
			break;
			}
		case PropStringArrayList:
			{
			m_stringArrayList.Serialize(ar);
			break;
			}
		case PropIntArrayList:
			{
			ar << (DWORD)m_intArrayList.GetCount();
			POSITION pos = m_intArrayList.GetHeadPosition();
			while (pos != NULL)
			{
				CArray<int, int> *pIntArray = m_intArrayList.GetNext(pos);
				if (pIntArray == NULL)
					ar << (DWORD)0;
				else
				{
					ar << (DWORD)pIntArray->GetSize();
					for (int j=0; j<pIntArray->GetSize(); j++)
						ar << pIntArray->GetAt(j);
				}				
			}
			break;
			}
		case PropStringArray:
			{
			m_stringList.Serialize(ar);
			break;
			}
		case PropIntArray:
			{
			m_intList.Serialize(ar);
			break;
			}			
		case PropImageList:
			{
			ar << m_ImageListIndex;
			break;
			}	
		case PropActiveXEvent:
			{
			ar << msStringValue;
			// call the method to save the activeX info
			SerializeActiveXInfo(ar, nThisVersion);
			break;
			}
		case PropActiveXProp:
			ar << msStringValue;
			// call the method to save the activeX info
			SerializeActiveXInfo(ar, nThisVersion);
			break;
		case PropActiveXPropPages:			
		case PropActiveXEnum:
		case PropActiveXRunTime:
		case PropActiveXMethods:
			{
			// call the method to save the activeX info
			SerializeActiveXInfo(ar, nThisVersion);
			break;
			}	
		}
		
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
			mnID = (short)lID;
		}
		else
			ar >> short(mnID);
		long lType;
		ar >> long(lType);
		mType = (PropertyTypes)lType;
		if (nThisVersion <= 5) //changing from BOOL to bool in version 6 [ORW]
		{
			BOOL bHidden;
			ar >> BOOL(bHidden);
			mbHidden = (bHidden != FALSE);
		}
		else
			ar >> bool(mbHidden); // get the flag that indicates this property is to be hidden
		if (mType == PropActiveXRunTime)
			mbHidden = true; // if this property is a run time only activeX property, set it as hidden.

		switch (mType)
		{
		case PropEnum:
		case PropPicture:
		case PropLong:
			{
			ar >> mlLong;
			break;
			}
		case PropEvent:
		case PropString:
			{
			ar >> msStringValue;
			break;
			}
		case PropDouble:
			{
			ar >> mdblDouble;
			break;
			}
		case PropBool:
			{
				if (nThisVersion <= 5) //changing from BOOL to bool in version 6 [ORW]
				{
					BOOL bVal;
					ar >> BOOL(bVal);// get the flag that indicates this property is to be hidden
					mbBoolean = (bVal != FALSE);
				}
				else
					ar >> bool(mbBoolean);
				break;
			}
		case PropOLEColor:
			{
			ar >> mOLEColor;
			break;
			}
		case PropStringArrayList:
			{
			m_stringArrayList.Serialize(ar);
			break;
			}		
		case PropIntArrayList:
			{
			int nCounter;
			ar >> nCounter;
			for (int i=0; i<nCounter; i++)
			{
				CArray<int, int> *pIntArray = NULL;
				int nCounter2;
				ar >> nCounter2;
				if (nCounter2 > 0)
				{
					pIntArray = new CArray<int, int>;
					for (int j=0; j<nCounter2; j++)
					{		
						int nItem;
						ar >> nItem;
						pIntArray->Add(nItem);
					}
				}	
				m_intArrayList.AddTail(pIntArray);
			}
			break;
			}
		case PropStringArray:
			{
			m_stringList.Serialize(ar);
			break;
			}		
		case PropIntArray:
			{
				if (nThisVersion <= 4)
				{
					m_stringList.Serialize(ar);
					POSITION pos = m_stringList.GetHeadPosition();
					while (pos != NULL)
						m_intList.Add(_tstol(m_stringList.GetNext(pos)));
					m_stringList.RemoveAll();
				}
				else
					m_intList.Serialize(ar);
			break;
			}			
		case PropImageList:
			{
			ar >> m_ImageListIndex;
			break;
			}

		case PropActiveXEvent:
			{
			ar >> msStringValue;
			// call the method to save the activeX info
			SerializeActiveXInfo(ar, nThisVersion);
			break;
			}
		case PropActiveXProp:
			// if this is the new file version than load the activeX info
			if (nThisVersion >= 3)
				ar >> msStringValue;
			// if this is the new file version than load the activeX info
			if (nThisVersion >= 2)
				SerializeActiveXInfo(ar, nThisVersion);			
			break;	
		case PropActiveXPropPages:
		case PropActiveXEnum:
		case PropActiveXRunTime:
		case PropActiveXMethods:			
			{
			// if this is the new file version than load the activeX info
			if (nThisVersion >= 2)
				SerializeActiveXInfo(ar, nThisVersion);
			break;
			}			
		}		
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
  if (!readShort(sFile, mnID)) return statInvalidFormat;
	int nType;
  if (!readInt(sFile, nType)) return statInvalidFormat;
	mType = (PropertyTypes)nType;
  // get the flag that indicates this property is to be hidden
  if (!readBool(sFile, mbHidden)) return statInvalidFormat;

  // if this property is a run time only activeX property, set it as hidden.
  if (mType == PropActiveXRunTime)
    mbHidden = true;

  CString sType = readLine(sFile);

  switch (mType)
  {
  case PropEnum:
  case PropPicture:
  case PropLong:
    {
      if (!readLong(sFile, mlLong)) return statInvalidFormat;
      break;
    }
  case PropEvent:
  case PropString:
    {
      if (!readString(sFile, msStringValue)) return statInvalidFormat;
      break;
    }
  case PropDouble:
    {
      if (!readDouble(sFile, mdblDouble)) return statInvalidFormat;
      break;
    }
  case PropBool:
    {
			BOOL bValue;
      if (!readBOOL(sFile, bValue)) return statInvalidFormat;
			mbBoolean = (bValue != FALSE);
      break;
    }
  case PropOLEColor:
    {
      DWORD dwordColor;
      if (!readDWORD(sFile, dwordColor)) return statInvalidFormat;
      mOLEColor = dwordColor;
      break;
    }
  case PropStringArrayList:
    {
      int iArrayCount;
      if (!readInt(sFile, iArrayCount)) return statInvalidFormat;

      for (int i = 0; i < iArrayCount; i++) {
        CStringArray* stringArray = NULL;
        int iStringCount;
        if (!readInt(sFile, iStringCount)) return statInvalidFormat;
        if (iStringCount != -1) {
          stringArray = new CStringArray;
          for (int j = 0; j < iStringCount; j++) {
            CString str;
            if (!readString(sFile, str)) return statInvalidFormat;

            stringArray->Add(str);
          }
        } else {
          stringArray = NULL;
        }
        m_stringArrayList.AddTail(stringArray);
      }
      break;
    }		
  case PropIntArrayList:
    {
      int nCounter;
      if (!readInt(sFile, nCounter)) return statInvalidFormat;;

      for (int i=0; i<nCounter; i++)
      {
        CArray<int, int> *pIntArray = NULL;

        int nCounter2;

        if (!readInt(sFile, nCounter2)) return statInvalidFormat;;

        if (nCounter2 > 0)
        {
          pIntArray = new CArray<int, int>;

          for (int j=0; j<nCounter2; j++)
          {		
            int nItem;
            if (!readInt(sFile, nItem)) return statInvalidFormat;;
            pIntArray->Add(nItem);
          }
        }	
        m_intArrayList.AddTail(pIntArray);
      }
      break;
    }
  case PropStringArray:
    {
      int iCount;
      if (!readInt(sFile, iCount)) return statInvalidFormat;
      for (int i = 0; i < iCount; i++) {
        CString str;
        if (!readString(sFile, str)) return statInvalidFormat;
        m_stringList.AddTail(str);
      }
      break;
    }		
  case PropIntArray:
    {
      int iCount;
      if (!readInt(sFile, iCount)) return statInvalidFormat;
      for (int i = 0; i < iCount; i++) {
        int intValue;
        if (!readInt(sFile, intValue)) return statInvalidFormat;
        m_intList.Add(intValue);
      }
      break;
    }			
  case PropImageList:
    {
      if (!readShort(sFile, m_ImageListIndex)) return statInvalidFormat;;
      break;
    }
  case PropActiveXEvent:
    {
      if (!readString(sFile, msStringValue)) return statInvalidFormat;;
      // call the method to save the activeX info
      if (!ReadActiveXInfoFromTextFile5(sFile)) return statInvalidFormat;
      break;
    }
  case PropActiveXProp:
    {
      if (!readString(sFile, msStringValue)) return statInvalidFormat;
			IOStatus stat = ReadActiveXInfoFromTextFile5(sFile);
      if (stat != statOK) return stat;
      break;	
    }
  case PropActiveXPropPages:
  case PropActiveXEnum:
  case PropActiveXRunTime:
  case PropActiveXMethods:			
    {
			IOStatus stat = ReadActiveXInfoFromTextFile5(sFile);
      if (stat != statOK) return stat;
      break;
    }			
  }		
  return statOK;
}

IOStatus CPropertyObject::ReadActiveXPropFromTextFile5(std::ifstream &sFile, AxPropertyDescriptor *axProp)
{
  LONG Id;
  if (!readLong(sFile, Id)) return statInvalidFormat;
  axProp->Id = Id;
  if (!readString(sFile, axProp->Name)) return statInvalidFormat;
  if (!readString(sFile, axProp->DocumentationDesc)) return statInvalidFormat;

  if (!readVARTYPE(sFile, axProp->Type)) return statInvalidFormat;
  if (!readBOOL(sFile, axProp->IsArray)) return statInvalidFormat;
  if (!readBOOL(sFile, axProp->CanSet)) return statInvalidFormat;

  if (!readCLSID(sFile, axProp->Guid)) return statInvalidFormat;
  if (!readInt(sFile, axProp->NumEnum)) return statInvalidFormat;
  if (!readInt(sFile, axProp->NumParams)) return statInvalidFormat;
  int iKind;
  if (!readInt(sFile, iKind)) return statInvalidFormat;
  switch(iKind)
  {
  case INVOKE_FUNC:
    axProp->invKind = INVOKE_FUNC;
    break;
  case INVOKE_PROPERTYGET:
    axProp->invKind = INVOKE_PROPERTYGET;
    break;
  case INVOKE_PROPERTYPUT:
    axProp->invKind = INVOKE_PROPERTYPUT;
    break;
  case INVOKE_PROPERTYPUTREF:
    axProp->invKind = INVOKE_PROPERTYPUTREF;
    break;
  default:
    axProp->invKind = (INVOKEKIND)iKind;
    break;
  }
  if (axProp->NumEnum > 0)
  {
    axProp->ArrEnum = new AxPropertyEnum[axProp->NumEnum];
    for (int i=0; i<axProp->NumEnum; i++)
    {
      if (!readString(sFile, axProp->ArrEnum[i].Name)) return statInvalidFormat;
      COleVariant var;
      if (!readOleVariant(sFile, var)) return statInvalidFormat;
      axProp->ArrEnum[i].Var = var;
    }
  }
  for (int i = 0; i<axProp->NumParams; i++)
  {
    if (!readVARTYPE(sFile, axProp->CallingArgs[i])) return statInvalidFormat;
    if (!readString(sFile, axProp->CallingArgNames[i])) return statInvalidFormat;
    if (!readCLSID(sFile, axProp->CallingArgClsids[i])) return statInvalidFormat;
  }

  return statOK;
}

IOStatus CPropertyObject::ReadActiveXInfoFromTextFile5(std::ifstream &sFile) 
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
    m_pAxProp = new AxPropertyDescriptor;
		IOStatus stat = ReadActiveXPropFromTextFile5(sFile, m_pAxProp);
    if (stat != statOK) return stat;
  }

  if (!readBOOL(sFile, bPropGet)) return statInvalidFormat;
  if (bPropGet)
  {
    m_pAxPropGet = new AxPropertyDescriptor;
		IOStatus stat = ReadActiveXPropFromTextFile5(sFile, m_pAxPropGet);
    if (stat != statOK) return stat;
  }

  if (!readBOOL(sFile, bPropPut)) return statInvalidFormat;
  if (bPropPut)
  {
    m_pAxPropPut = new AxPropertyDescriptor;
		IOStatus stat = ReadActiveXPropFromTextFile5(sFile, m_pAxPropPut);
    if (stat != statOK) return stat;
  }

  if (!readBOOL(sFile, bPropPutRef)) return statInvalidFormat;
  if (bPropPutRef)
  {
    m_pAxPropPutRef = new AxPropertyDescriptor;
		IOStatus stat = ReadActiveXPropFromTextFile5(sFile, m_pAxPropPutRef);
    if (stat != statOK) return stat;
  }

  if (!readBOOL(sFile, bPropEvent)) return statInvalidFormat;
  if (bPropEvent)
  {
    m_pAxEvent = new AxEventDescriptor;
    if (!readDISPID(sFile, m_pAxEvent->Id)) return statInvalidFormat;
    if (!readString(sFile, m_pAxEvent->Name)) return statInvalidFormat;
    if (!readString(sFile, m_pAxEvent->DocumentationDesc)) return statInvalidFormat;

    if (!readInt(sFile, m_pAxEvent->nArgs)) return statInvalidFormat;
    for (int i=0; i<MAX_CALLING_ARGUMENTS; i++) {
      if (!readVARTYPE(sFile, m_pAxEvent->CallingArgs[i])) return statInvalidFormat;
      CString temp;
      if (!readString(sFile, temp)) return statInvalidFormat;
      m_pAxEvent->CallingArgNames[i] = temp;
      if (!readCLSID(sFile, m_pAxEvent->CallingArgClsids[i])) return statInvalidFormat;
    }
    if (!readString(sFile, m_pAxEvent->Params)) return statInvalidFormat;
  }
  if (!readBOOL(sFile, bPropMethod)) return statInvalidFormat;
  if (bPropMethod)
  {
    m_pMethods = new CList<AxMethodDescriptor*>;
    int nCount;

    if (!readInt(sFile, nCount)) return statInvalidFormat;

    m_pMethods->RemoveAll();

    // do loop to navigate objects
    while (nCount-- > 0)
    {
      // get current object
      AxMethodDescriptor* pMethod = new AxMethodDescriptor;

      // get object from archive
			IOStatus stat = pMethod->ReadFromTextFile(sFile);
      if (stat != statOK) return stat;

      // add that ArxControlObject to the list object
      m_pMethods->AddTail(pMethod);		
    }
  }

  return statOK;
}
