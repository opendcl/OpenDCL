// OdclDirStylesCombo.cpp : implementation file
//

#include "stdafx.h"
#include "OdclDirStylesCombo.h"
#include "ArxGridCtrl.h"
#include "CxAcadSlide.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OdclDirStylesCombo

OdclDirStylesCombo::OdclDirStylesCombo()
{
	m_bTextStyle = false;
}

OdclDirStylesCombo::~OdclDirStylesCombo()
{
}


BEGIN_MESSAGE_MAP(OdclDirStylesCombo, CComboBox)
	//{{AFX_MSG_MAP(OdclDirStylesCombo)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OdclDirStylesCombo message handlers
BOOL OdclDirStylesCombo::Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID) 
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

	return RetVal;
}

void OdclDirStylesCombo::OnSelchange() 
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

void OdclDirStylesCombo::OnDropdown() 
{
	ResetContent();

	if (m_bTextStyle == false)
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
	
	if (m_bTextStyle == true)
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
