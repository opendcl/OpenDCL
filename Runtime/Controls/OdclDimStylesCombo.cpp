// OdclDimStylesCombo.cpp : implementation file
//

#include "stdafx.h"
#include "OdclDimStylesCombo.h"
#include "ArxGridCtrl.h"
#include "CxAcadSlide.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OdclDimStylesCombo

OdclDimStylesCombo::OdclDimStylesCombo()
{
	m_bTextStyle = false;
}

OdclDimStylesCombo::~OdclDimStylesCombo()
{
}


void OdclDimStylesCombo::Populate() 
{
	ResetContent();

	if (!m_bTextStyle)
	{
		AcDbDimStyleTable *pDimStyleTable;
		acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pDimStyleTable, AcDb::kForRead);

		AcDbDimStyleTableIterator* pIterator = NULL;
	    
		pDimStyleTable->newIterator(pIterator, true, true);

		// loop thru the layers
		for (; !pIterator->done(); pIterator->step())
		{
			AcDbDimStyleTableRecord *pDimStyleTableRecord = NULL;

			// open the entity for read
			if (pIterator->getRecord(pDimStyleTableRecord, AcDb::kForRead) != Acad::eOk)
				continue;

			// get the layer name
			const TCHAR* pName;
			pDimStyleTableRecord->getName(pName);
			int nItem = AddString(pName);

			// of course we must close it
			pDimStyleTableRecord->close();
		}
		
		delete pIterator;

		pDimStyleTable->close();
	}
	else
	{
		AcDbTextStyleTable *pTextStyleTable;
		acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pTextStyleTable, AcDb::kForRead);

		AcDbTextStyleTableIterator* pIterator = NULL;
	    
		pTextStyleTable->newIterator(pIterator, true, true);

		// loop thru the layers
		for (; !pIterator->done(); pIterator->step())
		{
			AcDbTextStyleTableRecord *pTextStyleTableRecord = NULL;

			// open the entity for read
			if (pIterator->getRecord(pTextStyleTableRecord, AcDb::kForRead) != Acad::eOk)
				continue;

			// get the layer name
			const TCHAR* pName;
			pTextStyleTableRecord->getName(pName);
			int nItem = AddString(pName);

			// of course we must close it
			pTextStyleTableRecord->close();
		}
		
		delete pIterator;

		pTextStyleTable->close();
	}
}


BEGIN_MESSAGE_MAP(OdclDimStylesCombo, CComboBox)
	//{{AFX_MSG_MAP(OdclDimStylesCombo)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OdclDimStylesCombo message handlers
BOOL OdclDimStylesCombo::Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal;
	DWORD dwStyle;

	if (rc.Height() < 100)
		rc.bottom = rc.top + 100;

	// set the arx control pointer
	dwStyle = WS_CHILD | WS_VISIBLE 
			  | CBS_SORT 
			  | CBS_DROPDOWNLIST | WS_GROUP |WS_TABSTOP;
	
	RetVal = CComboBox::Create( dwStyle, rc, pParentWnd, nID );
	Populate();

	return RetVal;
}

void OdclDimStylesCombo::OnSelchange() 
{
	CString sString;
	int nSel = GetCurSel();
	int nTextLength = GetLBTextLen(nSel);      
	GetLBText(nSel, sString.GetBuffer(nTextLength));
	sString.ReleaseBuffer();

	// Send Notification to parent of ListView ctrl
	CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();
	pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sString);		
	
	// fire the on Grid edit cell event.
	pListCtrl->EndEditControls(pListCtrl);
			
	
}

void OdclDimStylesCombo::OnDropdown() 
{
	CString sCurSel;
	int nCurSel = GetCurSel();
	if( nCurSel >= 0 )
		GetLBText(nCurSel, sCurSel);
	Populate();
	if(!sCurSel.IsEmpty())
		SetCurSel(FindStringExact(0, sCurSel));
}
