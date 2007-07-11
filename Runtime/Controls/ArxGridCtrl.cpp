// ArxGridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxGridCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"
#include "DynamicButtonCtrl.h"
#include "VdclArrowHeadComboBox.h"
#include "ComboBoxHolder.h"
#include "ComboBoxFolder.h"
#include "CurrencyEdit.h"
#include "OdclEdit.h"
#include "PercentageEdit.h"
#include "VdclAngleEdit.h"
#include "FontCombo.h"
#include "OdclLayerCombo.h"
#include "PrinterComboBox.h"
#include "VdclPlotStyleNamesComboBox.h"
#include "VdclPlotStyleTablesComboBox.h"
#include "OdclDimStylesCombo.h"
#include "VdclColorComboBox.h"
#include "VdclComboBox.h"
#include "VdclComboBoxEx.h"
#include "VdclNumericEdit.h"
#include "VdclSymbolEdit.h"
#include "DateTimeHolder.h"
#include "LineWeightDlg.h"
#include "LinetypeDlg.h"
#include "DirDialog.h"
#include "Workspace.h"
#include "AcadColorTable.h"
#include "SharedRes.h"

#define HP_HEADERITEM			0x00000001

#define ID_CELLBUTTON 191
#define ID_CELLBUTTON_DIR2 192
#define ID_CELLBUTTON_DIR3 193

static LPTSTR s_DateFormats[] =
{
	_T("d/M/yy"),
	_T("dd/MM/yy"),
	_T("dd/MMM/yy"),
	_T("ddd/MM/yy"),
	_T("ddd/MMM/yyyy"),
	_T("dddd/MMMM/yyyy"),
	_T("M/d/yy"),
	_T("MM/dd/yy"),
	_T("MM/ddd/yy"),
	_T("MMM/dd/yy"),
	_T("MMM/ddd/yyyy"),
	_T("MMMM/dddd/yyyy"),
	NULL
};
static LPTSTR s_TimeFormats[] =
{
	_T("h':'m':'s"),
	_T("hh':'mm':'ss"),
	_T("h':'m':'s' 'tt"),
	_T("hh':'mm':'ss' 'tt"),
	_T("H':'m':'s"),
	_T("HH':'mm':'ss"),
	NULL
};


bool getvar(LPCTSTR pszVarName)
{	
	struct resbuf rsVarVal;
	if (acedGetVar(pszVarName, &rsVarVal) != RTNORM)
		return false;
	if (rsVarVal.restype == RTT)
		return true;
	if (rsVarVal.restype == RTNIL)
		return false;
	return (rsVarVal.restype == RTSHORT && rsVarVal.resval.rint == 1);	
}


int GetCurrentLayerColor() 
{
	CString sLayerName;
	Acad::ErrorStatus es;
	AcDbEntity *pEnt;
	AcDbObjectId objId = acdbHostApplicationServices()->workingDatabase()->clayer();

	if (Acad::eOk != (es = acdbOpenAcDbEntity(pEnt, objId, AcDb::kForRead)))
		return 0;
	
	AcDbLayerTableRecord *pLayerTableRecord = (AcDbLayerTableRecord*)pEnt;
	if (pLayerTableRecord == NULL)
		return 0;
	
	AcCmColor clr = pLayerTableRecord->color();
	
	int nColor = clr.colorIndex();

	// of course we must close it
	pLayerTableRecord->close();

	return nColor;
}


CString LTOA(long lValue)
{
	CString sVal;
	sVal.Format( _T("%d"), lValue );
	return sVal;
}	


/////////////////////////////////////////////////////////////////////////////
// CArxGridCtrl

CArxGridCtrl::CArxGridCtrl( CDclControlObject* pTemplate,
														CControlPane* pPane,
														UINT nID,
														bool bCreate /*= true*/ )
: CGridCtrl( pTemplate, pPane, nID, false )
, mArxServices( pTemplate )
{
	m_bShowHighlight = true;
	m_bEditCells = true;

	// init all the pointers for cell editing controls.
	for (int i=0; i<nNumOfTextBoxes; i++)
		m_pTextBox[i] = NULL;
	for (int i=0; i<nNumOfComboBoxes; i++)
		m_pComboBox[i] = NULL;
	m_pFolderButton = NULL;
	m_pEllipsesButton = NULL;
	m_pPickButton = NULL;

	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxGridCtrl::~CArxGridCtrl()
{
}

bool CArxGridCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	if( GetTemplate()->GetLongProperty(nEventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	return bSuccess;
}

bool CArxGridCtrl::OnApplyProperty( RefCountedPtr< CPropertyObject > pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	//switch( pProp->GetID() )
	//{
	//case nDragnDropAllowDrop:
	//	{
	//		SetDragnDrop( pProp->GetBooleanValue() );
	//		break;
	//	}
	//}
	return !bFailed;
}

void CArxGridCtrl::SetCurSel(int nRow, int nCol) 
{
	m_bShowHighlight = true;
	bool bRefreshOldRect = false;
	CRect rcOld;
			
	if (m_nRowSelected != nRow || m_nColSelected != nCol)
	{
		if (m_nRowSelected != -1)
		{
			rcOld = GetCurSelRect();
			bRefreshOldRect = true;
		}
	}
	m_nRowSelected = nRow;
	m_nColSelected = nCol;

	if (bRefreshOldRect)
		InvalidateRect(rcOld, TRUE);
	
	CRect rc = GetCurSelRect();
	InvalidateRect(rc, TRUE);
	
	ShowCurSel();
	Invalidate(TRUE);
}

void CArxGridCtrl::CheckLayer(CString &sLayer, int &nImage)
{
	if (sLayer.GetLength() > 0 && nImage > -1)
		return;

	AcDbLayerTable *pLayerTable;
    acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLayerTable, AcDb::kForRead);

    AcDbLayerTableIterator* pIterator = NULL;
    
	pLayerTable->newIterator(pIterator, true, true);

	//pIterator->step();

	AcDbLayerTableRecord *pLayerTableRecord = NULL;

	// open the entity for read
	if (pIterator->getRecord(pLayerTableRecord, AcDb::kForRead) != Acad::eOk)
		return;

	// get the layer name
	const TCHAR* pName;
	pLayerTableRecord->getName(pName);
	sLayer = pName;

	// get the layer color
	AcCmColor clr = pLayerTableRecord->color();
	nImage = clr.colorIndex();

	// of course we must close it
	pLayerTableRecord->close();
	delete pIterator;
	pLayerTable->close();
}

void CArxGridCtrl::EditCellNow(UINT nChar/* = 0 */) 
{
	if( m_nColSelected < 0 )
		return;
	InvokeMethodIntInt(		
		mpTemplate->GetStrProperty(nEventBeginLabelEdit),
		m_nRowSelected, m_nColSelected,
		m_bInvokeWithSendString);

	switch (GetCurCellStyle())
	{
		case Grid_CheckBoxes:
		case Grid_OptionButtons:
		case Grid_SwitchableIcons:
			return;
			break;
		case Grid_DirectoryCell:
		{
			ShowEllipsesButton(m_nRowSelected,	m_nColSelected, 2);
			return;
			break;
		}
		case Grid_DwgFilesCell:
		{
			ShowEllipsesButton(m_nRowSelected,	m_nColSelected, 3);
			return;
			break;
		}
		case Grid_EllipsesButtons:
		{
			ShowEllipsesButton(m_nRowSelected,	m_nColSelected, 0);
			return;
			break;
		}
		case Grid_PickButtons:
		{
			ShowEllipsesButton(m_nRowSelected,	m_nColSelected, 1);
			return;
			break;
		}
	}
	RefreshCell(m_nRowSelected, m_nColSelected);

	m_bShowHighlight = false;

	DoEditCellNow(GetCurCellStyle());
}

void CArxGridCtrl::ShowEllipsesButton(int nRow, int nCol, int nAsPick)
{
	if (!m_bEditCells) 
		return;

	int nCount = GetItemCount();
	for (int i=0; i<nCount; i++)
	{
		SetItemState(i, NULL, NULL);

		CHeaderCtrl *pHdr = GetHeaderCtrl();
		int nCount = pHdr->GetItemCount();
		for (int j=0; j<nCount; j++)
		{
			SetItem(i, j, LVIF_STATE, NULL, 0, NULL, NULL, 0);				
		}
	}		
		
	if (nRow > -1 && nCol > -1)
	{
		//SetItem(nRow, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);				
		//SetItem(nRow, nCol, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);				
		m_nRowSelected = nRow;
		m_nColSelected = nCol;
	}
	else
	{
		m_nRowSelected = -1;
		m_nColSelected = -1;
	}
	

	CRect rc;
	if (nCol == 0)
		GetItemRect(nRow, rc, LVIR_LABEL);
	else
		GetSubItemRect(nRow, nCol, LVIR_LABEL, rc);

	rc.left = rc.right - 17;
	rc.left--;
	//rc.right--;
	rc.top--;
	//rc.bottom--;
	
	

	if (nAsPick > 1)
	{			
		rc.left -= 5;
	
		if (m_pFolderButton == NULL)
		{
			UINT idCmd = ID_CELLBUTTON;
			if( nAsPick == 2 )
				idCmd = ID_CELLBUTTON_DIR2;
			else if( nAsPick == 3 )
				idCmd = ID_CELLBUTTON_DIR3;
			m_pFolderButton = new CDynamicButtonCtrl( this, rc, idCmd );
			m_pFolderButton->SetIcon(IDI_FOLDER);
			m_pFolderButton->SetThemeHelper(mpControlPane->GetThemeHelper());
			m_pFolderButton->SetFlat(FALSE);
		}
		m_pFolderButton->MoveWindow( &rc, TRUE );
		m_pFolderButton->ShowWindow(TRUE);		
	}
	else if (nAsPick == 1)
	{			
		if (m_pPickButton == NULL)
		{
			m_pPickButton = new CDynamicButtonCtrl( this, rc, ID_CELLBUTTON );
			m_pPickButton->SetIcon(IDI_PICSM);
			m_pPickButton->SetThemeHelper(mpControlPane->GetThemeHelper());
			m_pPickButton->SetFlat(FALSE);
		}
		m_pPickButton->MoveWindow( &rc, TRUE );
		m_pPickButton->ShowWindow(TRUE);		
	}
	else if (nAsPick == 0)
	{
		if (m_pEllipsesButton == NULL)
		{
			m_pEllipsesButton = new CDynamicButtonCtrl( this, rc, ID_CELLBUTTON );
			m_pEllipsesButton->SetWindowText(_T("..."));
			m_pEllipsesButton->SetThemeHelper(mpControlPane->GetThemeHelper());
			m_pEllipsesButton->SetFlat(FALSE);
		}
		m_pEllipsesButton->MoveWindow( &rc, TRUE );
		m_pEllipsesButton->ShowWindow(TRUE);
	}
}

void CArxGridCtrl::DrawFontIcons(CDC* pDC, CRect rc, int &nImage, CString &sText)
{
	if (nImage == -1)
	{
		BeginWaitCursor();

		CFontCombo *pComboBox = NULL;
		CComboBoxHolder *pHolder;
		
		// get or create the control.
		if (m_pComboBox[9] == NULL)
		{
			pComboBox = new CFontCombo;
			pHolder = new CComboBoxHolder;

			pHolder->Create(CString(), WS_CHILD, CRect(1,1,10,10), this, 129);
			
			pHolder->pComboBox = pComboBox;
			pComboBox->Create(9, CRect(1,1,10,10), pHolder, 410);
			pComboBox->SetFont(GetFont());
			
			pHolder->m_nStyle = 9;
			pHolder->SetFont(GetFont());				
			m_pComboBox[9] = pHolder;			
		}
		else
		{
			pHolder = (CComboBoxHolder*)m_pComboBox[9];
			pComboBox = (CFontCombo*)pHolder->pComboBox;
		}

		int n = pComboBox->FindStringExact(0, sText);
		if (n == -1)
		{
			n = pComboBox->FindString(0, sText);
			if (n == -1)
			{	
				n = _ttoi(sText);
				pComboBox->GetLBText(n, sText);
			}
		}
		
		DWORD dwData = pComboBox->GetFontTypeId(sText);
	
		if (dwData == 4)
			nImage = 0;
		else if (dwData == 6)
			nImage = 1;

		EndWaitCursor();

		if (nImage == -1)
			return;
	}

	rc.top++;

	if (m_pFontImages == NULL)
	{
		m_pFontImages = new CImageList();
		m_pFontImages->Create(15,13,ILC_COLOR4 | ILC_MASK, 3, 1);
		
		HMODULE hRes = _hdllInstance;

		HICON hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_TRUEFONT), IMAGE_ICON, 0, 0, 0);	
		m_pFontImages->Add(hIcon);
		DestroyIcon(hIcon);

		hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_ACADFONT), IMAGE_ICON, 0, 0, 0);
		m_pFontImages->Add(hIcon);
		DestroyIcon(hIcon);
	}

	if (nImage > -1)
		m_pFontImages->Draw(pDC, nImage, rc.TopLeft(), ILD_TRANSPARENT);
}

void CArxGridCtrl::DrawLineWeights(CDC* pDC, CRect rc, AcDb::LineWeight LW)
{
	CPoint point;
	if (LW < AcDb::kLnWt030)
	{
		COLORREF rgb =RGB(0,0,0);
		HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
		HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
		
		MoveToEx(pDC->m_hDC, rc.left, rc.top + 6, &point);
		LineTo(pDC->m_hDC, rc.right, rc.top + 6);		
		
		SelectObject(pDC->m_hDC, OldPen);			
		DeleteObject(pen);
	}
	else
	{
		rc.right = rc.left + 12;

		switch (LW)
		{
			case AcDb::kLnWt030:
			case AcDb::kLnWt035:
			{
				rc.top +=5;
				rc.bottom = rc.top + 3;
				break;
			}
			case AcDb::kLnWt040:
			{
				rc.top +=5;
				rc.bottom = rc.top + 4;	
				break;
			}
			case AcDb::kLnWt050:
			case AcDb::kLnWt053:
			{
				rc.top +=4;
				rc.bottom = rc.top + 5;
				break;
			}
			case AcDb::kLnWt060:
			{
				rc.top +=4;
				rc.bottom = rc.top + 6;
				break;
			}
			case AcDb::kLnWt070:
			{
				rc.top +=3;
				rc.bottom = rc.top + 7;
				break;
			}
			case AcDb::kLnWt080:
			{
				rc.top +=3;
				rc.bottom = rc.top + 8;
				break;
			}
			case AcDb::kLnWt090:
			{
				rc.top +=3;
				rc.bottom = rc.top + 9;
				break;
			}
			case AcDb::kLnWt100:
			case AcDb::kLnWt106:
			{
				rc.top +=3;
				rc.bottom = rc.top + 10;
				break;
			}
			case AcDb::kLnWt120:
			{
				rc.top +=2;
				rc.bottom = rc.top + 12;
				break;
			}
			case AcDb::kLnWt140:
			case AcDb::kLnWt158:
			case AcDb::kLnWt200:
			case AcDb::kLnWt211:
			{
				rc.bottom = rc.top + 14;
				break;
			}
		}
		
		// draw the solid rectangle
		COLORREF rgb = RGB(0,0,0);
		rgb = ::SetBkColor(pDC->m_hDC, rgb);
		
		::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
		::SetBkColor(pDC->m_hDC, rgb);
	}
}

void CArxGridCtrl::DrawArrowHeads(CDC* pDC, CRect rc, int &nImage, CString &sText)
{
	if (nImage == -1)
	{
		VdclArrowHeadComboBox *pComboBox;
		CComboBoxHolder *pHolder;
		// get or create the control.
		if (m_pComboBox[3] == NULL)
		{
			pHolder = new CComboBoxHolder;
			pComboBox = new VdclArrowHeadComboBox;
			
			pHolder->Create(CString(), WS_CHILD, CRect(1,1,10,10), this, 123);
			
			pHolder->pComboBox = pComboBox;
			pComboBox->Create(3, CRect(1,1,10,10), pHolder, 410);
			pComboBox->SetFont(GetFont());

			pHolder->m_nStyle = 3;
			pHolder->SetFont(GetFont());				
			pHolder->ShowWindow(FALSE);
			m_pComboBox[3] = pHolder;			
		}
		else
		{
			pHolder = (CComboBoxHolder*)m_pComboBox[3];
			pComboBox = (VdclArrowHeadComboBox*)pHolder->pComboBox;				
		}
		int n = pComboBox->FindStringExact(0, sText);
		if (n == -1)
		{
			n = pComboBox->FindString(0, sText);
			if (n == -1)
			{	
				n = _ttoi(sText);
				pComboBox->GetLBText(n, sText);
			}
		}
		
		nImage = n;

		if (nImage == -1)
			return;
	}

	if (nImage == 20)
		return;

	rc.top++;
	rc.top++;

	if (m_pArrowImages == NULL)
	{
		m_pArrowImages = new CImageList();
		m_pArrowImages->Create(IDB_ARROWHEADS, 11, 1, CLR_NONE);
		m_pArrowImages->SetBkColor(RGB(255,255,255));
	}

	m_pArrowImages->Draw(pDC, nImage, rc.TopLeft(), ILD_NORMAL);
}

void CArxGridCtrl::MoveUp() 
{
	HideEditControls();
	m_bShowHighlight = true;
	
	if (m_nRowSelected > 0)
	{
		int nRow = m_nRowSelected;
		m_nRowSelected--;
		RefreshCell(nRow, m_nColSelected);
		ShowCurSel();
	}
}

void CArxGridCtrl::MoveDown() 
{
	HideEditControls();
	m_bShowHighlight = true;
	
	if (m_nRowSelected < GetItemCount()-1)
	{
		int nRow = m_nRowSelected;
		m_nRowSelected++;
		RefreshCell(nRow, m_nColSelected);
		ShowCurSel();
	}
}


BEGIN_MESSAGE_MAP(CArxGridCtrl, CGridCtrl)
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()	
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()	
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()	
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()	
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(NM_SETFOCUS, OnSetfocus)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillfocus)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)		
	ON_WM_MOUSEMOVE()
	ON_WM_KEYUP()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CELLBUTTON, OnCellButtonClicked)
	ON_COMMAND(ID_CELLBUTTON_DIR2, OnDir2CellButtonClicked)
	ON_COMMAND(ID_CELLBUTTON_DIR3, OnDir3CellButtonClicked)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxGridCtrl message handlers

void CArxGridCtrl::OnCellButtonClicked(void)
{
	InvokeMethodIntInt(mpTemplate->GetStrProperty(nEventBtnClicked), m_nRowSelected, m_nColSelected, m_bInvokeWithSendString);
}

void CArxGridCtrl::OnDir2CellButtonClicked(void)
{
	DoFileDlg(2);
}

void CArxGridCtrl::OnDir3CellButtonClicked(void)
{
	DoFileDlg(3);
}

void CArxGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	__super::OnLButtonDown(nFlags, point);

	CRect rcOld = GetCurSelRect();
	InvalidateRect( &rcOld, TRUE );
	int nRow, nCol;
	CellHitTest(point, nRow, nCol);
	if (m_nRowSelected != nRow || m_nColSelected != nCol)
		HideEditControls();

	if (nCol == 0 && m_bHasRowHeader)
		nCol = -1;

	CHeaderCtrl *pHeader = GetHeaderCtrl();

	if (pHeader != NULL && nCol > pHeader->GetItemCount())
		return;

	if (nRow == -1)
		return;

	InvokeMethodIntInt(
		mpTemplate->GetStrProperty(nEventClicked),
		nRow,
		nCol,
		m_bInvokeWithSendString);

	//m_bShowHighlight = true;

	if (nCol >= 0)
	{
		// test to see how to edit the cell each different kind of edit style
		// according to the column specification.
		switch( GetCellStyle(nRow, nCol) )
		{
		case Grid_CheckBoxes:
		case Grid_OptionButtons:
			{
				int nImage = GetItemImage(nRow, nCol);
				
				// if this column is an option button
				if (GetCellStyle(nRow, nCol) == Grid_OptionButtons)
				{
					if (nImage <= 0)
					{
						// reset all the other option buttons to unchecked.
						for (int i=0; i<GetItemCount(); i++)
						{
							if (GetCellStyle(i, nCol) == Grid_OptionButtons)
								SetItemImage(i, nCol, 0);
						}
						if (nImage != 1)
							SetItemImage(nRow, nCol, 1);
						else
							SetItemImage(nRow, nCol, 0);
					}
					else
						SetItemImage(nRow, nCol, 1);
				}
				else
				{			
					if (nImage != 1)
						SetItemImage(nRow, nCol, 1);
					else
						SetItemImage(nRow, nCol, 0);
				}
				
				// fire the on Grid edit cell event.
				EndEditControls(this);
				break;
			}
		case Grid_SwitchableIcons:
			{
				int nImage = GetItemImage(nRow, nCol);
				int nDefImage;
				int nAltImage;
				
				if (GetCellData(nRow, nCol) == NULL)
				{
					nDefImage = m_pColDefault->GetIntArrayPtr()->at(nCol);	
					nAltImage = m_pColAlternate->GetIntArrayPtr()->at(nCol);	
				}
				else
				{
					nDefImage = GetCellData(nRow, nCol)->nData1;
					nAltImage = GetCellData(nRow, nCol)->nData2;
				}

				if (nImage == nDefImage || nImage == -1)
					SetItemImage(nRow, nCol, nAltImage);
				else
					SetItemImage(nRow, nCol, nDefImage);
				// fire the on Grid edit cell event.
				EndEditControls(this);
				break;
			}
		case Grid_EllipsesButtons:
		case Grid_PickButtons:
			{
				EditCellNow();
				return;
			}
		default:
			{
				//if (m_nRowSelected == nRow && m_nColSelected == nCol)
				//	EditCellNow();
				//else
				//	break;
				int nOldRow = m_nRowSelected;
				int nOldCol = m_nColSelected;

				m_nColSelected = nCol;
				m_nRowSelected = nRow;
				
				EditCellNow();
				return;
			}
		}
	}
	
	m_nRowSelected = nRow;
	m_nColSelected = nCol;
	ShowCurSel();
	
	if (nCol > 0)
	{
		if (GetCellStyle(nRow, nCol) != Grid_CheckBoxes &&
			GetCellStyle(nRow, nCol) != Grid_OptionButtons &&
			GetCellStyle(nRow, nCol) != Grid_SwitchableIcons)
		{
			CRect rc = GetCurSelRect();
			InvalidateRect(rc, TRUE);
		}
	}
	// if not in a cell exit here
	if (nRow == -1)
		return;

	// if no style property is set for this column
	// exit here.
	//if (m_pColStyles->m_intList.GetSize() < nCol)
	//	return;
}

void CArxGridCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	__super::OnRButtonDown(nFlags, point);
}

void CArxGridCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	__super::OnMButtonDown(nFlags, point);
}

void CArxGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
//	__super::OnLButtonDblClk(nFlags, point);
	int nRow, nCol;
	CellHitTest(point, nRow, nCol);
	if (nCol < 0)
		return;

	InvokeMethodIntInt(
		mpTemplate->GetStrProperty(nEventDblClicked),
		nRow,
		nCol,
		m_bInvokeWithSendString);

	// test to see how to edit the cell each different kind of edit style
	// according to the column specification.
	if (GetCellStyle(nRow, nCol) == Grid_CheckBoxes ||
		GetCellStyle(nRow, nCol) == Grid_OptionButtons)
	{
		int nImage = GetItemImage(nRow, nCol);
		
		// if this column is an option button
		if (GetCellStyle(nRow, nCol) == Grid_OptionButtons)
		{
			if (nImage <= 0)
			{
				// reset all the other option buttons to unchecked.
				for (int i=0; i<GetItemCount(); i++)
				{
					SetItemImage(i, nCol, 0);
				}
				if (nImage != 1)
					SetItemImage(nRow, nCol, 1);
				else
					SetItemImage(nRow, nCol, 0);
			}
			else
				SetItemImage(nRow, nCol, 1);
		}
		else
		{			
			if (nImage != 1)
				SetItemImage(nRow, nCol, 1);
			else
				SetItemImage(nRow, nCol, 0);
		}
	}

	// test to see how to edit the cell each different kind of edit style
	// according to the column specification.
	if (GetCellStyle(nRow, nCol) == Grid_SwitchableIcons)
	{
		int nImage = GetItemImage(nRow, nCol);
		int nDefImage;
		int nAltImage;
		
		if (GetCellData(nRow, nCol) == NULL)
		{
			nDefImage = m_pColDefault->GetIntArrayPtr()->at(nCol);	
			nAltImage = m_pColAlternate->GetIntArrayPtr()->at(nCol);	
		}
		else
		{
			nDefImage = GetCellData(nRow, nCol)->nData1;
			nAltImage = GetCellData(nRow, nCol)->nData2;
		}

		if (nImage == nDefImage || nImage == -1)
		{
			SetItemImage(nRow, nCol, nAltImage);
		}
		else
		{
			SetItemImage(nRow, nCol, nDefImage);
		}
	}

	//
	if (m_bHasRowHeader && nCol == 0)
		return;

	ShowCurSel();
	EditCellNow();
}

void CArxGridCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (mpTemplate)		
		InvokeMethodStringIntInt(mpTemplate->GetStrProperty(nEventKeyUp), CString() + (char)nChar,  (int)nRepCnt,  (int)nFlags, m_bInvokeWithSendString);
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (mpTemplate)
	{
		char sChar = nChar;
		InvokeMethodStringIntInt(mpTemplate->GetStrProperty(nEventKeyDown), CString() + sChar,  (int)nRepCnt, (int)nFlags, m_bInvokeWithSendString);
	}

	m_bShowHighlight = true;

	switch (nChar) 
	{
		case VK_ESCAPE: 
			break;
		case VK_RETURN: 
			EditCellNow();
			return;
			break;			
		case VK_UP: 
		{
			MoveUp();
			return;
			break;
		}
		case VK_DOWN: 
		{
			MoveDown();
			return;
			break;
		}
		case VK_LEFT: 
		{
			
			if (m_nColSelected > (BOOL)m_bHasRowHeader)
			{
				int nCol = m_nColSelected;
				m_nColSelected--;
				RefreshCell(m_nRowSelected, nCol);
				ShowCurSel();
				return;
			}
			break;
		}
		case VK_RIGHT: 
		{
			CHeaderCtrl* pHdrCtrl = GetHeaderCtrl();

			if (m_nColSelected < pHdrCtrl->GetItemCount()-1)
			{
				int nCol = m_nColSelected;
				m_nColSelected++;
				RefreshCell(m_nRowSelected, nCol);
				ShowCurSel();
				return;
			}
			break;
		}
	}
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxGridCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_bShowHighlight = true;
	switch (nChar) 
	{
		case VK_ESCAPE: 
			break;
		case VK_RETURN: 
			EditCellNow();
			return;
			break;	
		case VK_BACK: 
			EditCellNow();
			return;
			break;		
		default:
			EditCellNow(nChar);
			return;
			break;		
	}
	__super::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CArxGridCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		// get the Multiline control.
		if (m_pTextBox[8] != NULL)
		{		
			OdclEdit *pTextBox = (OdclEdit*)m_pTextBox[8];
			int nS, nE;
			CString sText;

			pTextBox->GetSel(nS, nE);
			pTextBox->GetWindowText(sText);

			sText = sText.Mid(nS, nE) + _T("\r\n");
			
			pTextBox->ReplaceSel(sText, TRUE);
			return TRUE;
		}
		pMsg->wParam = NULL;
		pMsg->message = NULL;
		EditCellNow();
	}
	return __super::PreTranslateMessage(pMsg);
}

void CArxGridCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(mpTemplate->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
	*pResult = 0;
}

void CArxGridCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseUp),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	__super::OnLButtonUp(nFlags, point);
}

void CArxGridCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseUp),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	__super::OnRButtonUp(nFlags, point);
}

void CArxGridCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseUp),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	__super::OnMButtonUp(nFlags, point);
}

void CArxGridCtrl::OnContextMenu( CWnd* pTarget, CPoint point )
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseDown),
		2,
		MK_RBUTTON,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	__super::OnContextMenu(pTarget, point);
}

void CArxGridCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	__super::OnMouseMove(nFlags, point);
}

bool CArxGridCtrl::acad_truecolordlg(COLORREF color, int curColor, int nIndex = -1, CString sPantone = CString())
{
	struct resbuf *arglist, *rslt=NULL; 
	int rc; 

	if (nIndex == -1)
	{
		arglist = acutBuildList(
					RTSTR, _T("acad_truecolordlg"), 
					420, (long)color, 
					RTT, 
					RTSHORT, curColor, 
					0); 
	}	
	else if (sPantone.GetLength() > 0)
	{
		arglist = acutBuildList(
					RTSTR, _T("acad_truecolordlg"), 
					430, sPantone, 
					RTT, 
					RTSHORT, curColor, 
					0); 
	}
	else // uses index value not true value
	{
		arglist = acutBuildList(
					RTSTR, _T("acad_truecolordlg"), 
					RTSHORT, nIndex, 
					RTT, 
					RTSHORT, curColor, 
					0); 
	}

	if (arglist == NULL) 
		return false;

	rc = acedInvoke(arglist, &rslt); 

	acutRelRb(arglist);

	if (rc != RTNORM) 
	{ 
		return false;
	} 
	else if (rslt == NULL) 
	{ 
		return false;
	} 
	
	struct resbuf *index, *trueColor, *colorBook;
	/* Save the user-selected color */ 
	index = rslt;
	
	trueColor = index->rbnext;
	if (trueColor != NULL)
	{
		colorBook = trueColor->rbnext;

		if (colorBook != NULL)
		{
			if (colorBook->restype == 430)
			{
				CString sColorBook = colorBook->resval.rstring;

				SetItemImage(m_nRowSelected, m_nColSelected, 300);
				SetItemText(m_nRowSelected, m_nColSelected, sColorBook);
				return true;
			}
		}

		if (trueColor->restype == 420)
		{
			COLORREF clr = trueColor->resval.rlong;
			CString sColor = LTOA(GetBValue(clr)) + _T(",") +
							 LTOA(GetGValue(clr)) + _T(",") +
							 LTOA(GetRValue(clr));
			
			SetItemImage(m_nRowSelected, m_nColSelected, -1);
			SetItemText(m_nRowSelected, m_nColSelected, sColor);
		}
	}
	else
	{
		SetItemImage(m_nRowSelected, m_nColSelected, index->resval.rint);
		SetItemText(m_nRowSelected, m_nColSelected, LTOA(index->resval.rint));
	}

	acutRelRb(rslt);

	return true;
}

void CArxGridCtrl::DoEditCellNow(int nStyle, UINT nChar/* = 0 */) 
{
	
	switch (nStyle)
	{		
	case Grid_AngleUnits_Combo:
		{
			ShowTextBox(m_nRowSelected,	m_nColSelected, nStyle, nChar);
			
			RefCountedPtr< CPropertyObject > pProp = mpTemplate->GetPropertyObject(nColumnListItems);	
			
			if (GetCellData(m_nRowSelected,	m_nColSelected) != NULL)
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, GetCellData(m_nRowSelected, m_nColSelected)->sStrings);
			else if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
					ShowComboBox(m_nRowSelected, m_nColSelected, 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(m_nColSelected)));
			VdclAngleEdit *pText = (VdclAngleEdit *)m_pTextBox[nStyle-36];
				
			CComboBoxHolder *pHolder = (CComboBoxHolder*)m_pComboBox[2];
			VdclComboBox *pComboBox = (VdclComboBox*)pHolder->pComboBox;
			
			pText->m_pGridDropList = pComboBox;
			pComboBox->m_pGridEdit = pText;
			pText->SetFocus();			
			return;
			break;
		}
	case Grid_Units_Combo:
		{
			ShowTextBox(m_nRowSelected,	m_nColSelected, nStyle, nChar);
			
			RefCountedPtr< CPropertyObject > pProp = mpTemplate->GetPropertyObject(nColumnListItems);	

			if (GetCellData(m_nRowSelected,	m_nColSelected) != NULL)
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, GetCellData(m_nRowSelected, m_nColSelected)->sStrings);
			else if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(m_nColSelected)));

			VdclNumericEdit *pText = (VdclNumericEdit *)m_pTextBox[nStyle-36];
			
			CComboBoxHolder *pHolder = (CComboBoxHolder*)m_pComboBox[2];
			VdclComboBox *pComboBox = (VdclComboBox*)pHolder->pComboBox;
			
			pText->m_pGridDropList = pComboBox;
			pComboBox->m_pGridEdit = pText;
			pText->SetFocus();
			return;	
			break;
		}
	case Grid_Strings_Combo:
	case Grid_Integers_Combo:
		{
			ShowTextBox(m_nRowSelected,	m_nColSelected, nStyle, nChar);
			
			RefCountedPtr< CPropertyObject > pProp = mpTemplate->GetPropertyObject(nColumnListItems);	

			if (GetCellData(m_nRowSelected,	m_nColSelected) != NULL)
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, GetCellData(m_nRowSelected, m_nColSelected)->sStrings);
			else if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(m_nColSelected)));
			
			OdclEdit *pText = (OdclEdit *)m_pTextBox[nStyle-36];
			
			CComboBoxHolder *pHolder = (CComboBoxHolder*)m_pComboBox[2];
			if (pHolder != NULL)
			{
				VdclComboBox *pComboBox = (VdclComboBox*)pHolder->pComboBox;					
				pText->m_pGridDropList = pComboBox;
				pComboBox->m_pGridEdit = pText;
				pText->SetFocus();
			}			
			return;
			break;
		}
	case Grid_UpperCase_Combo:
	case Grid_LowerCase_Combo:
		{
			ShowTextBox(m_nRowSelected,	m_nColSelected, nStyle, nChar);
			
			RefCountedPtr< CPropertyObject > pProp = mpTemplate->GetPropertyObject(nColumnListItems);	

			if (GetCellData(m_nRowSelected,	m_nColSelected) != NULL)
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, GetCellData(m_nRowSelected, m_nColSelected)->sStrings);
			else if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(m_nColSelected)));
			
			OdclEdit *pText = NULL;
			switch(nStyle)
			{
			case Grid_UpperCase_Combo:
				nStyle = 5;
				break;
			case Grid_LowerCase_Combo:
				nStyle = 6;
				break;
			}
			pText = (OdclEdit *)m_pTextBox[nStyle];
			
			CComboBoxHolder *pHolder = (CComboBoxHolder*)m_pComboBox[2];
			VdclComboBox *pComboBox = (VdclComboBox*)pHolder->pComboBox;
			
			pText->m_pGridDropList = pComboBox;
			pComboBox->m_pGridEdit = pText;
			return;
			break;
		}
	case Grid_Strings:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 0, nChar);
			break;
	case Grid_AngleUnits:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 1, nChar);
			break;
	case Grid_Integers:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 2, nChar);
			break;
	case Grid_Units:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 3, nChar);
			break;
	case Grid_UpperCase:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 5, nChar);
			break;
	case Grid_LowerCase:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 6, nChar);
			break;
	case Grid_Password:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 7, nChar);
			break;
	case Grid_MultiLine:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 8, nChar);
			break;
	case Grid_Currency:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 9, nChar);
			break;
	case Grid_Date:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 10, nChar);
			break;
	case Grid_Time:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 11, nChar);
			break;
	case Grid_Percentage:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 12, nChar);
			break;			
	case Grid_DropDown:
		{
			RefCountedPtr< CPropertyObject > pProp = mpTemplate->GetPropertyObject(nColumnListItems);	
			if (GetCellData(m_nRowSelected,	m_nColSelected) != NULL)
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, GetCellData(m_nRowSelected, m_nColSelected)->sStrings);
			else if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(m_nColSelected)));
		break;
		}
	case Grid_ImageDropList:
		{
			if (GetCellData(m_nRowSelected, m_nColSelected) != NULL &&
				GetCellData(m_nRowSelected, m_nColSelected)->sStrings.GetSize() > 0)
			{
				ShowImageComboBox(m_nRowSelected, 
						m_nColSelected,
						GetCellData(m_nRowSelected, m_nColSelected)->sStrings,
						GetCellData(m_nRowSelected, m_nColSelected)->nInts);
			}
			else
			{
				RefCountedPtr< CPropertyObject > pProp = mpTemplate->GetPropertyObject(nColumnListItems);	
				RefCountedPtr< CPropertyObject > pPropImages = mpTemplate->GetPropertyObject(nColumnListImages);
				if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
				{
					PropVal::TIntArray& rInts = pPropImages->GetIntArrayListPtr()->at(m_nColSelected);
					CArray< int, int > rInt;
					for(int idx = 0; idx < rInts.size(); idx++)
						rInt.Add(rInts[idx]);
					ShowImageComboBox(m_nRowSelected, 
							m_nColSelected,
							PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(m_nColSelected)),
							rInt);
				}
			}
		break;
		}
	case Grid_ArrowHead:
	case Grid_AcadColors:
	case Grid_TextStyleList:
	case Grid_PlotStyleNames:
	case Grid_PlotStyleTables:
	case Grid_PlotterList:
	case Grid_Fonts:
	case Grid_DriveList:
	case Grid_LayerList:
	case Grid_DimStyleList:
		{
			CStringArray sStrings;
			ShowComboBox(m_nRowSelected, m_nColSelected, nStyle-16, sStrings);
		break;
		}	
		
	case Grid_AcadColorCell:
		{
			int nCurrentColor = GetItemImage(m_nRowSelected, m_nColSelected);
			if (acedSetColorDialog(
					nCurrentColor,
					TRUE,
					GetCurrentLayerColor()) == TRUE)
			{
				AcCmColor retColor;
				CString sDesc;

				retColor.setColorIndex(nCurrentColor);
				sDesc = retColor.colorNameForDisplay();
				SetItemText(m_nRowSelected, m_nColSelected, sDesc);
				SetItemImage(m_nRowSelected, m_nColSelected, nCurrentColor);
			}
			// fire the on Grid edit cell event.
			EndEditControls(this);
		break;
		}
	case Grid_TrueColorCell:
		{
				COLORREF clr = 0;
			int nImage = GetItemImage(m_nRowSelected, m_nColSelected);
		
			CString sLabel = GetItemText(m_nRowSelected, m_nColSelected);
			int nComma = sLabel.Find(_T(","));
			int nComma2 = sLabel.Find(_T(","), nComma+1);
			if (nComma > -1 && nComma2 > -1)
			{
				CString sRed = sLabel.Left(nComma);
				CString sGreen = sLabel.Mid(nComma+1, nComma2-nComma);
				CString sBlue = sLabel.Mid(nComma2+1);
				
				int nRed = _ttol(sRed);
				int nGreen = _ttol(sGreen);
				int nBlue = _ttol(sBlue);
				
				clr = RGB(nRed, nGreen, nBlue);									
			}
			else
			{
				clr = RGB(255, 255, 255);									
			}
			CColorDialog dlg(clr, CC_FULLOPEN, this);
			if (dlg.DoModal() == IDOK)
			{
			   COLORREF color = dlg.GetColor();
			   
			   CString sColor = LTOA(GetRValue(color)) + _T(",") +
							 LTOA(GetGValue(color)) + _T(",") +
							 LTOA(GetBValue(color));
			
				SetItemImage(m_nRowSelected, m_nColSelected, -1);
				SetItemText(m_nRowSelected, m_nColSelected, sColor);
			}
			// fire the on Grid edit cell event.
			EndEditControls(this);
		break;
		}
		
	case Grid_LineWeightCell:
		{
			AcDb::LineWeight newLW = (AcDb::LineWeight)GetItemImage(m_nRowSelected, m_nColSelected);
			SelectLineWeightUI(newLW, true, this);
			SetItemImage(m_nRowSelected, m_nColSelected, (int)newLW);
			
			// fire the on Grid edit cell event.
			EndEditControls(this);
			break;
		}
	case Grid_LinetypeCell:
		{
			CString sLinetype;
			AcDbObjectId idLinetype;
			SelectLinetypeUI(idLinetype, sLinetype, true, this);
			SetItemText(m_nRowSelected, m_nColSelected, sLinetype);

			// fire the on Grid edit cell event.
			EndEditControls(this);
			break;
		}
	case Grid_DirectoryCell:
		{			
			CDirDialog dlg( theWorkspace.LoadResourceString(IDS_SELFOLDER), GetItemText(m_nRowSelected, m_nColSelected) );
			if (dlg.DoBrowse(this) == TRUE)
			{
				SetItemText(m_nRowSelected, m_nColSelected, dlg.GetSelectedFolder());
				SetItemImage(m_nRowSelected, m_nColSelected, -1);
			}
			// fire the on Grid edit cell event.
			EndEditControls(this);
			break;
		}
	case Grid_DwgFilesCell:
		{
			struct resbuf * result;
			CString sFile = GetItemText(m_nRowSelected, m_nColSelected);
			result = acutNewRb(RTSTR);

			if (acedGetFileD(sFile, sFile, _T("dwg"), 8, result) == RTNORM)
			{
				SetItemText(m_nRowSelected, m_nColSelected, result->resval.rstring);
				SetItemImage(m_nRowSelected, m_nColSelected, -1);
			}
			acutRelRb(result);
			
			// fire the on Grid edit cell event.
			EndEditControls(this);
			
			return;
			break;
		}
	}	

	if (nStyle == 0)
	{
		m_bShowHighlight = true;
		ShowCurSel();
	}
}

void CArxGridCtrl::DoFileDlg(int nStyle) 
{
	switch (nStyle)
	{
		case 2:
		{			
			CDirDialog dlg( theWorkspace.LoadResourceString(IDS_SELFOLDER), GetItemText(m_nRowSelected, m_nColSelected) );
			if (dlg.DoBrowse(this) == TRUE)
			{
				SetItemText(m_nRowSelected, m_nColSelected, dlg.GetSelectedFolder());
				SetItemImage(m_nRowSelected, m_nColSelected, -1);
			}

			// fire the on Grid edit cell event.
			EndEditControls(this);

			break;
		}
		case 3:
		{
			struct resbuf * result;
			CString sFileExt = _T("dwg;dxf");
			CString sFile = GetItemText(m_nRowSelected, m_nColSelected);
			result = acutNewRb(RTSTR);
		
			if (GetCellData(m_nRowSelected, m_nColSelected) != NULL &&
				GetCellData(m_nRowSelected, m_nColSelected)->sStrings.GetSize() > 0)
			{
				sFileExt = GetCellData(m_nRowSelected, m_nColSelected)->sStrings[0];
			}
			else
			{			
				RefCountedPtr< CPropertyObject > pProp = mpTemplate->GetPropertyObject(nColumnListItems);	
				if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
					sFileExt = pProp->GetStringArrayListPtr()->at(m_nColSelected)[0];
			}
			if (acedGetFileD(sFile, sFile, sFileExt, 8, result) == RTNORM)
			{
				SetItemText(m_nRowSelected, m_nColSelected, result->resval.rstring);
				SetItemImage(m_nRowSelected, m_nColSelected, -1);
			}
			acutRelRb(result);
			
			// fire the on Grid edit cell event.
			EndEditControls(this);

			break;
		}
	}
}

void CArxGridCtrl::ShowCurSel() 
{
	InvokeMethodIntInt(mpTemplate->GetStrProperty(nEventSelChanged), m_nRowSelected, m_nColSelected, m_bInvokeWithSendString);
	__super::ShowCurSel();
}

void CArxGridCtrl::ShowTextBox(int nRow, int nCol, int nStyle, UINT nChar/* = 0 */)
{
	int nCount = GetItemCount();
	
	if (nRow > -1 && nCol > -1)
	{
		m_nRowSelected = nRow;
		m_nColSelected = nCol;
	}
	else
	{
		m_nRowSelected = -1;
		m_nColSelected = -1;
	}
	
	COLORREF backGround = GetBkColor();
	
	if (!m_bOrientationVer)
	{
		if ((m_nRowSelected % 2) != 0)
			backGround = alertnateColor; 
	}
	else
	{
		int n = 0;
		if (m_bHasRowHeader)
			n = 1;
		if (((m_nColSelected+n) % 2) != 0)
			backGround = alertnateColor;
	}

	CRect rc;
	if (nCol == 0)
		GetItemRect(nRow, rc, LVIR_LABEL);
	else
		GetSubItemRect(nRow, nCol, LVIR_LABEL, rc);
	
	rc.right = rc.left + GetColumnWidth(nCol)-(4/2)-1;
	rc.top++;

	switch (nStyle)
	{
	case 8:
		{
		CRect rcClient;
		GetClientRect(&rcClient);
		int nCalc = (m_nRowHeight * 5);
		rc.bottom = rc.top + nCalc;
		if (rc.bottom > rcClient.bottom)
			rc.bottom = rcClient.bottom-1;
		rc.left -= 2;
		rc.top -= 2;
		break;
		}
	case Grid_Strings_Combo:
	case Grid_AngleUnits_Combo:
	case Grid_Integers_Combo:
	case Grid_Units_Combo:
		rc.right = rc.left + GetColumnWidth(nCol)-(4/2)-16;
		nStyle -= 36;
		break;
	case Grid_UpperCase_Combo:
		rc.right = rc.left + GetColumnWidth(nCol)-(4/2)-16;
		nStyle = 5;
		break;
	case Grid_LowerCase_Combo:
		rc.right = rc.left + GetColumnWidth(nCol)-(4/2)-16;
		nStyle = 6;
		break;
	}
	
	if (!m_bHasRowHeader || nCol > 0)
	{
		int nImage = GetItemImage(nRow, nCol);

		if (nImage > -1)
		{
			IMAGEINFO Info;

			CImageList *pImage1 = GetImageList(LVSIL_NORMAL);
			CImageList *pImage2 = GetImageList(LVSIL_SMALL);
		
			if (pImage1 != NULL)
				pImage1->GetImageInfo(0, &Info);
			if (pImage2 != NULL)
				pImage2->GetImageInfo(0, &Info);

			rc.left += Info.rcImage.right - Info.rcImage.left + 2;
		}
	}

	rc.bottom--;
	rc.left++;
	if (GetItemImage(nRow, nCol) > 0)
		rc.left += 1;
	else if (nStyle != 10)
		rc.left += 3;
	
	if (nStyle == 10)
		rc.bottom+=1;


	//rc.left++;
	//rc.left++;

	CString sCellText;
	sCellText = GetItemText(nRow, nCol);
	
	
	switch (nStyle)
	{
	case 1:
		{
			VdclAngleEdit *pTextBox;

			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new VdclAngleEdit;
				pTextBox->Create(nStyle, rc, this, 110+nStyle);
			}
			else
			{
				pTextBox = (VdclAngleEdit*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}
			pTextBox->m_pGridDropList = NULL;
			// setup the control
			pTextBox->m_strOldValue = sCellText;
			pTextBox->SetWindowText(sCellText);		
			pTextBox->SetFont(GetFont());
			pTextBox->SetBkColor(backGround);
			if (nChar > 0)
			{
				if (pTextBox->m_sFilter.FindOneOf(CString(char(nChar))) > -1)
					pTextBox->SetWindowText(CString(char(nChar)));
				pTextBox->GetWindowText(sCellText);
			}
			//pTextBox->EnableWindow(TRUE);
			pTextBox->ShowWindow(TRUE);

			if (nChar == 0)
				pTextBox->SetSel(sCellText.GetLength(),0, TRUE);
			else
				pTextBox->SetSel(sCellText.GetLength(), sCellText.GetLength(), TRUE);

			pTextBox->SetFocus();	
			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			break;
		}
	case 3:
		{
			VdclNumericEdit *pTextBox;
			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new VdclNumericEdit;
				pTextBox->Create(nStyle, rc, this, 110+nStyle);
			}
			else
			{
				pTextBox = (VdclNumericEdit*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}

			pTextBox->m_pGridDropList = NULL;
			
			// setup the control
			pTextBox->m_strOldValue = sCellText;
			pTextBox->SetWindowText(sCellText);
			pTextBox->SetBkColor(backGround);
			if (nChar > 0)
			{
				if (pTextBox->m_sFilter.FindOneOf(CString(char(nChar))) > -1)
					pTextBox->SetWindowText(CString(char(nChar)));
				pTextBox->GetWindowText(sCellText);
			}
			pTextBox->SetFont(GetFont());
			//pTextBox->EnableWindow(TRUE);
			pTextBox->ShowWindow(TRUE);
			if (nChar == 0)
				pTextBox->SetSel(sCellText.GetLength(),0, TRUE);
			else
				pTextBox->SetSel(sCellText.GetLength(), sCellText.GetLength(), TRUE);
			pTextBox->SetFocus();			
			
			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			break;
		}
	case 4:
		{
			VdclSymbolEdit *pTextBox;
			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new VdclSymbolEdit;
				pTextBox->Create(nStyle, rc, this, 110+nStyle);
			}
			else
			{
				pTextBox = (VdclSymbolEdit*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}


			// setup the control
			pTextBox->m_strOldValue = sCellText;
			pTextBox->SetWindowText(sCellText);		
			if (nChar > 0)
			{
				pTextBox->SetWindowText(CString(char(nChar)));
				pTextBox->GetWindowText(sCellText);
			}
			pTextBox->SetFont(GetFont());
			//pTextBox->EnableWindow(TRUE);
			pTextBox->ShowWindow(TRUE);
			if (nChar == 0)
				pTextBox->SetSel(sCellText.GetLength(),0, TRUE);
			else
				pTextBox->SetSel(sCellText.GetLength(), sCellText.GetLength(), TRUE);
			pTextBox->SetFocus();			

			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			break;
		}
	case 9:
		{
			CCurrencyEdit *pTextBox;
			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new CCurrencyEdit;

				UINT nCreateStyle = WS_CHILD | WS_VISIBLE | ES_WANTRETURN | WS_CLIPSIBLINGS | ES_AUTOHSCROLL | ES_LEFT;
				pTextBox->Create(nCreateStyle, rc, this, 110+nStyle);
			}
			else
			{
				pTextBox = (CCurrencyEdit*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}

			// setup the control
			pTextBox->m_strOldValue = sCellText;
			sCellText.TrimLeft(_T("$"));
			sCellText.Replace(_T(","), CString());
			double rValue = _tstof(sCellText);
			pTextBox->Set_Number(rValue);
			
			if (nChar > 0)
			{
				pTextBox->Set_Number(_ttoi(CString(TCHAR(nChar))));
				pTextBox->GetWindowText(sCellText);
			}
			pTextBox->SetFont(GetFont());
			pTextBox->ShowWindow(TRUE);

			pTextBox->SetFocus();			

			pTextBox->SetSel(0, -1, TRUE);
						
			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			break;
		}	
	case 10:
	case 11:
		{
			CDateTimeHolder *pTextBox;
			
			rc.top -= 1;
			rc.right += 2;

			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new CDateTimeHolder;

				if (nStyle == 10)
				{
					pTextBox->CreateDate(rc, this, 310+nStyle);
					
				}
				else
				{
					pTextBox->CreateTime(rc, this, 410+nStyle);
				}
			}
			else
			{
				pTextBox = (CDateTimeHolder*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}
			
			int nDataStyle = 0;

			if (GetCellData(nRow, nCol) == NULL)
			{
				nDataStyle = m_pColImages->GetIntArrayPtr()->at(nCol);
			}
			else
			{
				nDataStyle = GetCellData(nRow, nCol)->nData1;
			}

			if (nStyle == 10)
			{
				pTextBox->m_Child.SetFormat(s_DateFormats[nDataStyle]);					
			}
			else
			{
				pTextBox->m_Child.SetFormat(s_TimeFormats[nDataStyle]);
			}

			COleDateTime date;
			date.ParseDateTime(sCellText);
			SYSTEMTIME st = { '\0' };
			date.GetAsSystemTime(st);
			pTextBox->m_Child.SetTime(date);
			
			// setup the control
			pTextBox->m_Child.m_strOldValue = sCellText;
			//pTextBox->m_Child.SetBkColor(RGB(255, 0,0));
			
			pTextBox->m_Child.SetFont(GetFont());
			pTextBox->ShowWindow(TRUE);

			pTextBox->m_Child.SetFocus();			

			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			break;
		}	
	case 12:
		{
			CPercentageEdit *pTextBox;
			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new CPercentageEdit;

				UINT nCreateStyle = WS_CHILD | WS_VISIBLE | ES_WANTRETURN | WS_CLIPSIBLINGS | ES_AUTOHSCROLL | ES_LEFT;
				pTextBox->Create(nCreateStyle, rc, this, 110+nStyle);
			}
			else
			{
				pTextBox = (CPercentageEdit*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}

			// setup the control
			pTextBox->m_strOldValue = sCellText;
			sCellText.Replace(_T(","), CString());
			double rValue = _tstof(sCellText);
			pTextBox->Set_Number(rValue);
			
			
			if (nChar > 0)
			{
				pTextBox->Set_Number(_ttoi(CString(TCHAR(nChar))));
				pTextBox->GetWindowText(sCellText);
			}
			pTextBox->SetFont(GetFont());
			pTextBox->ShowWindow(TRUE);

			pTextBox->SetFocus();			
			
			pTextBox->SetSel(0, -1, TRUE);
						
			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			break;
		}	
	default:
		{
			OdclEdit *pTextBox;

			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new OdclEdit;
				pTextBox->Create(nStyle, rc, this, 110+nStyle);
			}
			else
			{
				pTextBox = (OdclEdit*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}

			pTextBox->m_pGridDropList = NULL;
			
			// setup the control
			pTextBox->m_strOldValue = sCellText;
			pTextBox->SetWindowText(sCellText);		
			pTextBox->SetBkColor(backGround);
			if (nChar > 0)
			{
				pTextBox->SetWindowText(CString(TCHAR(nChar)));
				sCellText = TCHAR(nChar);
			}
			
			pTextBox->SetFont(GetFont());
			pTextBox->ShowWindow(TRUE);
			if (nChar == 0)
				pTextBox->SetSel(sCellText.GetLength(), 0, TRUE);
			else
				pTextBox->SetSel(sCellText.GetLength(), sCellText.GetLength(), TRUE);
			pTextBox->SetFocus();			
			
			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			
			break;
		}
	}
}

void CArxGridCtrl::ShowImageComboBox(int nRow, int nCol, CStringArray &sStrings, CArray<int, int> &nItems)
{
	if (!m_bEditCells) 
		return;

	int nCount = GetItemCount();
	for (int i=0; i<nCount; i++)
	{
		SetItemState(i, NULL, NULL);

		CHeaderCtrl *pHdr = GetHeaderCtrl();
		int nCount = pHdr->GetItemCount();
		for (int j=0; j<nCount; j++)
		{
			SetItem(i, j, LVIF_STATE, NULL, 0, NULL, NULL, 0);				
		}
	}		
		
	if (nRow > -1 && nCol > -1)
	{
		SetItem(nRow, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);				
		SetItem(nRow, nCol, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);				
		m_nRowSelected = nRow;
		m_nColSelected = nCol;
	}
	else
	{
		m_nRowSelected = -1;
		m_nColSelected = -1;
	}
	

	CRect rc;
	if (nCol == 0)
		GetItemRect(nRow, rc, LVIR_LABEL);
	else
		GetSubItemRect(nRow, nCol, LVIR_LABEL, rc);
	
	int nItemHeight = rc.bottom - rc.top - 2;

	if (!m_bHasRowHeader || nCol > 0)
	{
		int nImage = GetItemImage(nRow, nCol);

		if (nImage == -1)
			nImage = 0;

		IMAGEINFO Info;

		CImageList *pImageList = GetImageList(LVSIL_NORMAL);
		if( !pImageList )
			pImageList = GetImageList(LVSIL_SMALL);
		if( pImageList )
		{
			pImageList->GetImageInfo(0, &Info);
			nItemHeight = Info.rcImage.bottom - Info.rcImage.top;
		}
	}


	rc.left = rc.right - 15;
	rc.left--;
	rc.right--;
	

	CString sCellText;
	sCellText = GetItemText(nRow, nCol);
	
	
	CComboBoxHolder *pHolder = NULL;
	VdclComboBoxEx *pComboBox = NULL;
	int nStyle = 13;
	// get or create the control.
	if (m_pComboBox[nStyle] == NULL)
	{
		int nColWidth = GetColumnWidth(nCol);
		
		pHolder = new CComboBoxHolder;
		pComboBox = new VdclComboBoxEx;
		pHolder->Create(CString(), WS_CHILD|WS_VISIBLE, rc, this, 140);
		pHolder->pComboBox = pComboBox;
		CImageList *pImage = GetImageList(LVSIL_SMALL);
		
		nItemHeight = nItemHeight * 12;
		nItemHeight = nItemHeight + 10;
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST;

		BOOL b = pComboBox->Create(dwStyle, CRect(15-nColWidth,-2, rc.Width()+2, nItemHeight), pHolder, IDC_CB_CHILD);
		pComboBox->SetFont(GetFont());		
		pComboBox->SetDroppedWidth(nColWidth);
		pComboBox->SetImageList(pImage);
			
		pHolder->m_nStyle = nStyle;
		pHolder->SetFont(GetFont());
	}
	else
	{
		pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
		pComboBox = (VdclComboBoxEx*)pHolder->pComboBox;
		pHolder->MoveWindow(rc, TRUE);
		pComboBox->ResetContent();
	}

	if (pComboBox->GetImageList() != NULL)
		pComboBox->GetImageList()->SetBkColor(RGB(255,255,255));

	_Style *pCell = GetCellData(nRow, nCol);

	if (pCell == NULL)
	{
		// add the strings.
		for (int i=0; i<sStrings.GetSize(); i++)
		{
			COMBOBOXEXITEM cbi;

			cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY |
			CBEIF_SELECTEDIMAGE | CBEIF_TEXT;

			cbi.iItem = i;
			cbi.pszText = (LPTSTR)(LPCTSTR)sStrings[i];
			cbi.cchTextMax = sStrings[i].GetLength();
			cbi.iImage = nItems[i];
			cbi.iSelectedImage = nItems[i];
			cbi.iOverlay = 2;
			cbi.iIndent = 0;

			pComboBox->InsertItem(&cbi);
		}			
	}
	else
	{
		// add the strings.
		for (int i=0; i<pCell->sStrings.GetSize(); i++)
		{
			COMBOBOXEXITEM cbi;

			cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY |
			CBEIF_SELECTEDIMAGE | CBEIF_TEXT;

			cbi.iItem = i;
			cbi.pszText = (LPTSTR)(LPCTSTR)sStrings[i];
			cbi.cchTextMax = pCell->sStrings[i].GetLength();
			cbi.iImage = pCell->nInts[i];
			cbi.iSelectedImage = pCell->nInts[i];
			cbi.iOverlay = 2;
			cbi.iIndent = 0;

			pComboBox->InsertItem(&cbi);
		}			
	}
	// setup the control
	//
	pHolder->ShowWindow(TRUE);
	
	int nIndex = pComboBox->FindStringExact(0, sCellText);
	if (nIndex > -1)
	{
		pComboBox->SetCurSel(nIndex);
	}
	else
	{
		nIndex = pComboBox->FindString(0, sCellText);
		if (nIndex > -1)
			pComboBox->SetCurSel(nIndex);			
	}	
	pComboBox->SetFocus();			

	// set the control array.
	m_pComboBox[nStyle] = pHolder;

}

void CArxGridCtrl::ShowComboBox(int nRow, int nCol, int nStyle, CStringArray &sStrings)
{
	if (!m_bEditCells) 
		return;

	int nCount = GetItemCount();
	if (nRow > -1 && nCol > -1)
	{
		m_nRowSelected = nRow;
		m_nColSelected = nCol;
	}
	else
	{
		m_nRowSelected = -1;
		m_nColSelected = -1;
	}
	
	BeginWaitCursor();

	CRect rc;
	if (nCol == 0)
		GetItemRect(nRow, rc, LVIR_LABEL);
	else
		GetSubItemRect(nRow, nCol, LVIR_LABEL, rc);
	
	if (!m_bHasRowHeader || nCol > 0)
	{
		int nImage = GetItemImage(nRow, nCol);

		if (nImage > -1)
		{
			IMAGEINFO Info;

			CImageList *pImage1 = GetImageList(LVSIL_NORMAL);
			CImageList *pImage2 = GetImageList(LVSIL_SMALL);
		
			if (pImage1 != NULL)
				pImage1->GetImageInfo(0, &Info);
			if (pImage2 != NULL)
				pImage2->GetImageInfo(0, &Info);

			rc.left += Info.rcImage.right - Info.rcImage.left + 4;
		}
	}


	rc.left = rc.right - 15;
	rc.bottom = rc.top + 17;
	
	if (m_bHasGridLines)
	{
		rc.right--;
		rc.left--;
	}

	if (nStyle > 2)
	{
		rc.top--;
		rc.bottom = rc.top + 18;
	}


	CString sCellText;
	sCellText = GetItemText(nRow, nCol);
	
	CComboBoxHolder *pHolder = NULL;
	CComboBox *pTheComboBox = NULL;
			
	int nColWidth = GetColumnWidth(nCol);
				
	if (m_pComboBox[nStyle] == NULL)
	{
		pHolder = new CComboBoxHolder;
	}

	rc.top++;	
			
	switch (nStyle)
	{
	case 2:
		{
			VdclComboBox *pComboBox = NULL;
			rc.top--;	
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new VdclComboBox;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
			
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (VdclComboBox*)pHolder->pComboBox;
				pHolder->MoveWindow(rc, TRUE);
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pComboBox->ResetContent();
			}
			pComboBox->m_pGridEdit = NULL;
			pTheComboBox = pComboBox;
			break;
		}
	case 3:
		{
			VdclArrowHeadComboBox *pComboBox = NULL;
			rc.top--;
			rc.bottom++;
			if (m_nRowHeight == 19)
				rc.bottom++;
			if (m_nRowHeight >= 20)
				rc.bottom += 2;

			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new VdclArrowHeadComboBox;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());

				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (VdclArrowHeadComboBox*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}
	case 4:
		{
			VdclColorComboBox *pComboBox = NULL;
			rc.top--;
			rc.bottom++;
			if (m_nRowHeight == 19)
				rc.bottom++;
			if (m_nRowHeight >= 20)
				rc.bottom += 2;

			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new VdclColorComboBox;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());

				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				

			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (VdclColorComboBox*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			
			pTheComboBox = pComboBox;
			break;
		}
	case 5:
		{
			OdclDimStylesCombo *pComboBox = NULL;
			//rc.top--;	
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new OdclDimStylesCombo;
				
				pComboBox->m_bTextStyle = true;
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
			
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (OdclDimStylesCombo*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
				pComboBox->ResetContent();
			}
			pTheComboBox = pComboBox;
			break;
		}
	case 6:
		{
			VdclPlotStyleNamesComboBox *pComboBox = NULL;
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new VdclPlotStyleNamesComboBox;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
				
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (VdclPlotStyleNamesComboBox*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}	
	case 7:
		{
			VdclPlotStyleTablesComboBox *pComboBox = NULL;
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new VdclPlotStyleTablesComboBox;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
				
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (VdclPlotStyleTablesComboBox*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}	

	case 8:
		{
			CPrinterComboBox *pComboBox = NULL;
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				CRect rcWnd(15-nColWidth,-2, rc.Width()+2, 350);
				pComboBox = new CPrinterComboBox( *mpControlPane, NULL, 410, rcWnd );
				pHolder->pComboBox = pComboBox;
				pComboBox->SetFont(GetFont());
				
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (CPrinterComboBox*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}	
	case 9:
		{
			CFontCombo *pComboBox = NULL;
			rc.top--;
			rc.bottom++;
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new CFontCombo;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
				
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (CFontCombo*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}
	case 10:
		{
			CComboBoxFolder *pComboBox = NULL;
			//rc.top--;	
			//rc.bottom++;

			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new CComboBoxFolder;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
				pComboBox->Init(0, 300); 
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (CComboBoxFolder*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}
	case 11:
		{
			OdclLayerCombo *pComboBox = NULL;
			rc.top--;	
			rc.bottom++;
			
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new OdclLayerCombo;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
				
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (OdclLayerCombo*)pHolder->pComboBox;
				
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}
	case 12:
		{
			OdclDimStylesCombo *pComboBox = NULL;
			//rc.top--;	
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new OdclDimStylesCombo;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
			
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (OdclDimStylesCombo*)pHolder->pComboBox;
				pHolder->MoveWindow(rc, TRUE);
				pComboBox->ResetContent();
			}
			pTheComboBox = pComboBox;
			break;
		}
		
	}

	pTheComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));				
	pTheComboBox->ShowWindow(TRUE);

	if (nStyle == 2)
	{
		_Style *pCell = GetCellData(nRow, nCol);

		if (pCell != NULL)
		{
			// add the strings.
			for (int i=0; i<pCell->sStrings.GetSize(); i++)
			{
				pTheComboBox->AddString(pCell->sStrings[i]);
			}			
		}
		else
		{			
			// add the strings.
			for (int i=0; i<sStrings.GetSize(); i++)
			{
				pTheComboBox->AddString(sStrings[i]);
			}			
		}	
	}

	// setup the control
	//
	pHolder->ShowWindow(TRUE);
	
	int nIndex = pTheComboBox->FindStringExact(0, sCellText);
	if (nIndex > -1)
	{
		pTheComboBox->SetCurSel(nIndex);
	}
	else
	{
		nIndex = pTheComboBox->FindString(0, sCellText);
		if (nIndex > -1)
			pTheComboBox->SetCurSel(nIndex);			
		else
			pTheComboBox->SetCurSel(0);			
	}
		
	pTheComboBox->SetDroppedWidth(nColWidth);
				
	pTheComboBox->SetFocus();			

	// set the control array.
	m_pComboBox[nStyle] = pHolder;

	EndWaitCursor();
}

void CArxGridCtrl::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_bShowHighlight = true;
	HideEditControls();
	*pResult = 0;
}

void CArxGridCtrl::OnEndlabeledit(CString sText) 
{	
	if (m_nRowSelected > -1)
	{
		InvokeMethodIntInt(
			mpTemplate->GetStrProperty(nEventEndLabelEdit),
			m_nRowSelected,
			m_nColSelected,
			m_bInvokeWithSendString);

		SetItemText(m_nRowSelected, m_nColSelected, sText);

		int nCount = GetItemCount();
		for (int i=0; i<nCount; i++)
		{
			SetItemState(i, NULL, NULL);

			CHeaderCtrl *pHdr = GetHeaderCtrl();
			int nCount = pHdr->GetItemCount();
			for (int j=0; j<nCount; j++)
				SetItem(i, j, LVIF_STATE, NULL, 0, NULL, NULL, 0);				
		}	
		SetItem(m_nRowSelected, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);				
		SetItem(m_nRowSelected,	m_nColSelected, LVIF_STATE, NULL, 0, NULL, NULL, 0);				
	}
}

void CArxGridCtrl::CallBeginLabelEdit(CPoint point) 
{
	LVHITTESTINFO lvhti;

	// Clear the subitem text the user clicked on.
	lvhti.pt = point;
	SubItemHitTest(&lvhti);
	
	if (lvhti.flags & LVHT_ONITEMLABEL)
	{
		// call methods to invoke the event
		InvokeMethodIntInt(		
			mpTemplate->GetStrProperty(nEventBeginLabelEdit),
			lvhti.iItem,
			lvhti.iSubItem,
			m_bInvokeWithSendString);
	}
}

void CArxGridCtrl::EndEditControls(CWnd *pWnd)
{
	if (pWnd->IsWindowVisible() == TRUE)
	{
		InvokeMethodIntInt(		
			mpTemplate->GetStrProperty(nEventEndLabelEdit),
			m_nRowSelected, m_nColSelected,
			m_bInvokeWithSendString);
	}
}

void CArxGridCtrl::HideEditControls()
{
	CRect rc;
  int i;
	for (i=0; i<nNumOfTextBoxes; i++)
	{
		if (m_pTextBox[i] != NULL && m_pTextBox[i]->IsWindowVisible() == TRUE)
		{
			EndEditControls(m_pTextBox[i]);
			m_pTextBox[i]->ShowWindow(FALSE);
		}
	}

	for (i=0; i<nNumOfComboBoxes; i++)
	{
		if (m_pComboBox[i] != NULL && m_pComboBox[i]->IsWindowVisible() == TRUE)
		{
			//EndEditControls(m_pComboBox[i]);
			m_pComboBox[i]->ShowWindow(SW_HIDE);
		}
	}
	if( m_pPickButton )
		m_pPickButton->ShowWindow(SW_HIDE);
	if( m_pEllipsesButton )
		m_pEllipsesButton->ShowWindow(SW_HIDE);
	if( m_pFolderButton )
		m_pFolderButton->ShowWindow(SW_HIDE);
}

void CArxGridCtrl::OnDestroy() 
{
	// init all the pointers for cell editing controls.
  int i;
	for (i=0; i<nNumOfTextBoxes; i++)
	{
		if (m_pTextBox[i] != NULL)
		{
			m_pTextBox[i]->DestroyWindow();
			delete m_pTextBox[i];
			m_pTextBox[i] = NULL;
		}
	}
	for (i=0; i<nNumOfComboBoxes; i++)
	{
		if (m_pComboBox[i] != NULL)
		{
			CComboBoxHolder *pHolder = (CComboBoxHolder*)m_pComboBox[i];
			
			pHolder->pComboBox->DestroyWindow();
			delete pHolder->pComboBox;
			pHolder->pComboBox = NULL;
			
			m_pComboBox[i]->DestroyWindow();
			delete m_pComboBox[i];
			m_pComboBox[i] = NULL;
		}
	}
	
	if (m_pPickButton != NULL)
	{
		m_pPickButton->DestroyWindow();
		delete m_pPickButton;
		m_pPickButton = NULL;
	}
	if (m_pEllipsesButton != NULL)
	{
		m_pEllipsesButton->DestroyWindow();
		delete m_pEllipsesButton;
		m_pEllipsesButton = NULL;
	}
	
	if (m_pFolderButton != NULL)
	{
		m_pFolderButton->DestroyWindow();
		delete m_pFolderButton;
		m_pFolderButton = NULL;
	}
	for (i=0; i<GetItemCount(); i++)
	{
		if (GetItemData(i) > 0)
			delete (_RowData*)GetItemData(i);	
	}
	__super::OnDestroy();
}

void CArxGridCtrl::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	if (IsWindow(m_hWnd) == TRUE)
		HideEditControls();
}

void CArxGridCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	InvokeMethodInt(mpTemplate->GetStrProperty(nEventColumnClick), pNMListView->iSubItem, m_bInvokeWithSendString);
	*pResult = 0;
}

void CArxGridCtrl::DrawColor(CDC* pDC, CRect rc, int &nColor, CString &sText)
{

	if (nColor == -1)
	{
		VdclColorComboBox *pComboBox = NULL;
		CComboBoxHolder *pHolder;
		
		// get or create the control.
		if (m_pComboBox[4] == NULL)
		{
			pComboBox = new VdclColorComboBox;
			pHolder = new CComboBoxHolder;
			
			pHolder->Create(CString(), WS_CHILD, CRect(1,1,10,10), this, 124);
			
			pHolder->pComboBox = pComboBox;
			pComboBox->Create(4, CRect(1,1,10,10), pHolder, 410);
			pComboBox->SetFont(GetFont());

			pHolder->m_nStyle = 4;
			pHolder->SetFont(GetFont());				
			m_pComboBox[4] = pHolder;
			
		}
		else
		{
			pHolder = (CComboBoxHolder*)m_pComboBox[4];
			pComboBox = (VdclColorComboBox*)pHolder->pComboBox;
		}

		int n = -1;
		if (sText.GetLength() == 0)
		{
			pComboBox->GetLBText(8, sText);
		}

		n = pComboBox->FindStringExact(0, sText);
		if (n == -1)
		{
			n = pComboBox->FindString(0, sText);
			if (n != -1)
			{					
				pComboBox->GetLBText(n, sText);
			}
		}
		if (n == -1)
		{	
			nColor = _ttoi(sText);

			n = pComboBox->FindItemByColorIndex(nColor);
			
			if (n == -101)
				sText = LTOA(nColor);
			else
				pComboBox->GetLBText(n, sText);
		}
		else
		{
			nColor = pComboBox->GetItemColorIndex(n);
		}
	}
	
	// setup the CRect for FillRect	
	CPoint point;
	
	// move the color splotch down by one.
	rc.top++;
	rc.bottom++;

	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, RGB(0,0,0));
	HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
		
	::Rectangle(pDC->m_hDC, rc.left, rc.top, rc.right, rc.bottom);

	SelectObject(pDC->m_hDC, OldPen);			
	DeleteObject(pen);

	rc.DeflateRect(1,1);

	// draw the solid rectangle
	COLORREF rgb;
	
	if (nColor == 256 || nColor == 0)
	{
		rgb = RGB(255,255,255);
		::SetBkColor(pDC->m_hDC, rgb);
	}
	else
	{
		rgb = GetRGBColor(nColor);
		::SetBkColor(pDC->m_hDC, rgb);
	}

	::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	::SetBkColor(pDC->m_hDC, rgb);
}

void CArxGridCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlType != ODT_LISTVIEW)
        return;

	if(lpDrawItemStruct->itemAction != ODA_DRAWENTIRE)
		return;

	COLORREF rowColor = alertnateColor;
	COLORREF backGround = mpTemplate->GetColorProperty(nAcadColor);
	COLORREF back = backGround;
	COLORREF foreGround = ::GetSysColor(COLOR_WINDOWTEXT);
	
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem); 
	
	CRect rcClipBox;
	pDC->GetClipBox(rcClipBox);

    //CMemDC pDC(CDC::FromHandle(lpDrawItemStruct->hDC), rcItem);
    int nItem = lpDrawItemStruct->itemID;
    CImageList* pImageList; 
    // Save dc state 
    //int nSavedDC = pDC->SaveDC();
    
	//CDC dcm;    
	CRect rc;
    GetClientRect(rc);    
    //pDC->CreateCompatibleDC(pDC);
    //CBitmap bmt;
    //bmt.CreateCompatibleBitmap(pDC,rc.Width(),rc.Height());
    //CBitmap *pBitmapOld = pDC->SelectObject(&bmt);
    HGDIOBJ pOldFont = SelectObject(pDC->m_hDC, GetFont()->m_hObject);

  //  pDC->Rectangle(rc);// make the work of the OnEraseBkgnd function
  

    // Get item image and state info 
    LV_ITEM lvi;
    lvi.mask = LVIF_IMAGE | LVIF_STATE; 
    lvi.iItem = nItem; 
    lvi.iSubItem = 0;
    lvi.stateMask = 0xFFFF; 
    // get all state flags 
    GetItem(&lvi);
    
    // Should the item be highlighted
    BOOL bHighlight =
        ((lvi.state & LVIS_DROPHILITED) ||
        ((lvi.state & LVIS_SELECTED)
        && ((GetFocus()
        == this) || (GetStyle() & LVS_SHOWSELALWAYS))));

	int nCellStyle = GetCellStyle(lvi.iItem, 0);

	if (bHighlight && nCellStyle == Grid_EllipsesButtons)
		bHighlight = false;
	if (bHighlight && nCellStyle == Grid_PickButtons)
		bHighlight = false;
	if (bHighlight && nCellStyle == Grid_SwitchableIcons)
		bHighlight = false;

    // Get rectangles for drawing 
    CRect rcBounds, rcLabel, rcIcon;
    GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
    GetItemRect(nItem, rcLabel, LVIR_LABEL); 
    GetItemRect(nItem, rcIcon, LVIR_ICON);
    CRect rcCol(rcBounds);
    CString sLabel = GetItemText(nItem, 0);
  
	// Labels are offset by a certain amount 
    // This offset is related to the width of a space character
    int offset = 4;//pDC->GetTextExtent(_T(" "), 1).cx*2;
    
	bool bWordWrap = false;
	if (rcLabel.Height() >= (pDC->GetTextExtent(_T(" "), 1).cy*2) + 1)
		bWordWrap = true;
	
    
    CRect rcHighlight;
    CRect rcWnd; 
    int nExt; 
    
    switch (HighlightType) 
	{ 
    case HIGHLIGHT_NORMAL: 
        nExt = pDC->GetOutputTextExtent(sLabel).cx + offset; 
        rcHighlight = rcLabel;
        if( rcLabel.left + nExt < rcLabel.right )
            rcHighlight.right = rcLabel.left + nExt; 
        break; 
    case HIGHLIGHT_ALLCOLUMNS:
        rcHighlight = rcBounds;
        rcHighlight.left = rcLabel.left;
        break; 
    case HIGHLIGHT_ROW:
        GetClientRect(&rcWnd);
        rcHighlight = rcBounds;
        rcHighlight.left = rcLabel.left;
        rcHighlight.right = rcWnd.right;
        
        break;
    default: 
        rcHighlight = rcLabel; 
    } 
    
	CRect rcBtn = rcBounds;
	//rcBtn.left = 0;
	//rcBtn.top++;
	rcBtn.bottom--;
	rcBtn.right = rcBtn.left + __super::GetColumnWidth(0);

	HTHEME	hTheme = NULL;

	if (m_bHasRowHeader == true)
	{
		if( m_pTheme )
			hTheme = m_pTheme->OpenThemeData(m_hWnd, L"HEADER");
		if (hTheme == NULL) // If the handle cannot be retrieved, 
							// go back to non-visual style drawing
							// code.
		{			
			if (nItem == GetTopIndex())
			{
				rcBtn.top--;
				rcBtn.top--;
			}
			
			rcBtn.bottom++;

			pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
			pDC->SetBkColor(::GetSysColor(COLOR_BTNFACE));
			pDC->FillRect(rcBtn,
				&CBrush(::GetSysColor(COLOR_BTNFACE)));

			// draw the solid rectangle
			::DrawEdge(pDC->m_hDC, &rcBtn, BDR_RAISEDINNER, BF_RECT);
		}    
		else
		{
			CRect rcTheme = rcBtn;

			rcTheme.right++;
			rcTheme.right++;
			rcTheme.bottom++;
			rcTheme.bottom++;
			rcTheme.bottom++;
			int iStateId = 0;

			m_pTheme->DrawThemeBackground(hTheme, m_hWnd, pDC->GetSafeHdc(), HP_HEADERITEM, iStateId, &rcTheme, NULL);

			m_pTheme->CloseThemeData(hTheme);

			hTheme = NULL;

			CPoint point;
		
			COLORREF rgb = ::GetSysColor(COLOR_BTNHIGHLIGHT);
			HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
			HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
			
			MoveToEx(pDC->m_hDC, rcTheme.left, rcTheme.top, &point);
			LineTo(pDC->m_hDC, rcTheme.right+1, rcTheme.top);	

			SelectObject(pDC->m_hDC, OldPen);			
			DeleteObject(pen);
		
				
			if (nItem == GetItemCount()-1)
			{
				HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, backGround);
				HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
				
				pDC->MoveTo(CPoint(rcBtn.right,rcBtn.bottom+1));
				pDC->LineTo(CPoint(rcBtn.right + 3,rcBtn.bottom+1));
				pDC->MoveTo(CPoint(rcBtn.left,rcBtn.bottom+2));
				pDC->LineTo(CPoint(rcBtn.right + 3,rcBtn.bottom+2));
			
				SelectObject(pDC->m_hDC, OldPen);			
				DeleteObject(pen);
			}
		}
	}
	else
	{
		// Draw the background color
		if (bHighlight)
		{
			pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
			pDC->FillRect(rcHighlight,
			&CBrush(::GetSysColor(COLOR_HIGHLIGHT))); 
		}
		else
		{
			pDC->FillRect(rcHighlight, &CBrush(backGround)); 
			if (ForegroundColor != 0)
				pDC->SetTextColor(ForegroundColor); 
		}
	}

	// Set clip region 
	rcCol.right = rcCol.left + GetColumnWidth(0); 
	//CRgn rgn;
	//CRect rc2 = rcCol;
	//rc2.bottom++;
	//rc2.right++;
	//rgn.CreateRectRgnIndirect(&rc2); 
	//pDC->SelectClipRgn(&rgn);
	//rgn.DeleteObject(); 
    

	// Draw state icon 
	if (lvi.state & LVIS_STATEIMAGEMASK)
	{
		int nImage = ((lvi.state & LVIS_STATEIMAGEMASK)>>12) - 1;
		pImageList = GetImageList(LVSIL_STATE);
		if (pImageList)
			pImageList->Draw(pDC, nImage,CPoint(rcCol.left, rcCol.top+1), ILD_TRANSPARENT);
	}
    

	// Draw item label - Column 0    
	rcLabel.left = rcBounds.left + 4;//(offset/2)-1;
	rcLabel.right -= offset;

	rcLabel.top++;

	pDC->SetBkMode(TRANSPARENT);

	// Draw normal and overlay icon
	int nCellImage = GetItemImage(lvi.iItem, 0);
    pImageList = GetImageList(LVSIL_SMALL);
	if (lvi.iImage > -1 && pImageList != NULL)
	{
		IMAGEINFO inf;
		pImageList->GetImageInfo(nCellImage, &inf);
		rcLabel.left += inf.rcImage.right - inf.rcImage.left + 4;
	}

	rcLabel.top++;

	if (bWordWrap)
		pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT |
			DT_NOPREFIX | DT_NOCLIP |
			DT_TOP | DT_WORDBREAK); 
	else
		pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT |
			DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP |
			DT_TOP | DT_END_ELLIPSIS); 

	rcLabel.top--;

	// Draw labels for remaining columns
	LV_COLUMN lvc; 
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	if (HighlightType == 0) 
	{
		// Highlight only first column
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkColor(backGround);
	} 

	rcIcon.top += 1;

	if (m_pColDefault->GetIntArrayPtr()->size() > 0)
	{
		if (nCellStyle == Grid_SwitchableIcons &&
			lvi.iImage != m_pColDefault->GetIntArrayPtr()->at(0) &&
			lvi.iImage != m_pColAlternate->GetIntArrayPtr()->at(0))
		{
			if (lvi.iImage < m_pColDefault->GetIntArrayPtr()->at(0))
				lvi.iImage = m_pColDefault->GetIntArrayPtr()->at(0);
			if (lvi.iImage > m_pColAlternate->GetIntArrayPtr()->at(0))
				lvi.iImage = m_pColAlternate->GetIntArrayPtr()->at(0);						
		}
	}

	if (pImageList && lvi.iImage > -1) 
	{
		UINT nOvlImageMask = lvi.state & LVIS_OVERLAYMASK;
		pImageList->Draw(pDC,
				lvi.iImage, CPoint(rcIcon.left, rcIcon.top+1),
				//(bHighlight ? ILD_BLEND50 : 0) |
				ILD_TRANSPARENT | nOvlImageMask ); 
    }

	rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right : rcBounds.right;

	
	int nColumn = 1;
	if (!m_bHasRowHeader)
	{
		nColumn = 0;
		rcCol.right = rcBtn.left; 
	}

	// draw each cell item.
	for (; GetColumn(nColumn, &lvc) == TRUE; nColumn++) 
	{		
		int nCellStyle = GetCellStyle(lvi.iItem, nColumn);
		bool bHighlighted = false;
		rcCol.left = rcCol.right; 
        rcCol.right = rcCol.left + lvc.cx;

		
		if (m_bShowHighlight)
		{
			switch(nCellStyle)
			{
				case Grid_DirectoryCell:
				case Grid_DwgFilesCell:		
				case Grid_EllipsesButtons:
				case Grid_PickButtons:
				case Grid_SwitchableIcons:
					m_bShowHighlight = false;
					break;
			}
		}
		// if the entire column is selected.
		if (m_nColSelected == -1)
			m_bShowHighlight = true;
		
		if (m_nRowSelected == lvi.iItem &&
			m_nColSelected == nColumn &&
			m_bShowHighlight)
		{
			backGround = ::GetSysColor(COLOR_HIGHLIGHT);
			foreGround = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
			bHighlighted = true;
		}
		else if (m_nRowSelected == lvi.iItem &&
			     m_nColSelected == -1 &&
				 m_bShowHighlight)
		{
			backGround = ::GetSysColor(COLOR_HIGHLIGHT);
			foreGround = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
			bHighlighted = true;
		}
		else
		{
			backGround = back;
			foreGround = ::GetSysColor(COLOR_WINDOWTEXT);
		}

		if (!bHighlighted)
		{
			if (!m_bOrientationVer)
			{
				if ((lvi.iItem % 2) != 0)
					backGround = rowColor;
			}
			else
			{
				int n = 0;
				if (m_bHasRowHeader)
					n = 1;
				if (((nColumn+n) % 2) != 0)
					backGround = rowColor;
			}
		}
		pDC->SetBkColor(backGround);
		pDC->SetTextColor(foreGround);

		if (hTheme != NULL)
		{
			::SetBkColor(pDC->m_hDC, backGround);
			::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rcCol, NULL, 0, NULL);
		}
		if (hTheme == NULL)
		{
			if (nItem == GetTopIndex())
			{
				rcCol.top--;
				rcCol.top--;
			}
			
			::SetBkColor(pDC->m_hDC, backGround);
			::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rcCol, NULL, 0, NULL);
			
			if (nItem == GetTopIndex())
			{
				rcCol.top++;
				rcCol.top++;
			}
		}
		
		sLabel = GetItemText(nItem, nColumn); 

		
		if (m_bHasGridLines)
		{
			COLORREF rgb = ::GetSysColor(COLOR_BTNFACE);
			HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
			HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
			
			pDC->MoveTo(CPoint(rcCol.left, rcCol.bottom-1));
			pDC->LineTo(CPoint(rcCol.right-1, rcCol.bottom-1));
			pDC->LineTo(CPoint(rcCol.right-1, rcCol.top-1));
			
			SelectObject(pDC->m_hDC, OldPen);			
			DeleteObject(pen);
		}

		int nImageOffset = 0;
		int nCellImage = GetItemImage(lvi.iItem, nColumn);

		bool bDrawIcon = true;
			
		switch (nCellStyle)
		{
		case Grid_ImageDropList:
			{
				if (nCellImage == -1)
				{
					GetImageDropListItems(lvi.iItem, nColumn, nCellImage, sLabel);
					SetItemImage(lvi.iItem, nColumn, nCellImage);
					SetItemText(lvi.iItem, nColumn, sLabel);
				}
				break;
			}
		}
			

		// if this cell is to use switchable image icons...
		if (m_pColDefault->GetIntArrayPtr()->size() > nColumn)
		{
			// if switchable icons are to be drawn.
			if (nCellStyle == Grid_SwitchableIcons &&
				nCellImage != m_pColDefault->GetIntArrayPtr()->at(nColumn) &&
				nCellImage != m_pColAlternate->GetIntArrayPtr()->at(nColumn))
			{
				_Style *pData = GetCellData(lvi.iItem, nColumn);
				if (pData != NULL)
				{
					if (pData->nStyle == Grid_SwitchableIcons)
					{
						if (nCellImage < pData->nData1)
							nCellImage = pData->nData1;
						if (nCellImage > pData->nData2)
						{
							nCellImage = pData->nData2;
						}
					}					
					else
					{
						if (nCellImage < m_pColDefault->GetIntArrayPtr()->at(nColumn))
								nCellImage = m_pColDefault->GetIntArrayPtr()->at(nColumn);
						if (nCellImage > m_pColAlternate->GetIntArrayPtr()->at(nColumn))
							nCellImage = m_pColAlternate->GetIntArrayPtr()->at(nColumn);
					}
				}
				else
				{
					if (nCellImage < m_pColDefault->GetIntArrayPtr()->at(nColumn))
							nCellImage = m_pColDefault->GetIntArrayPtr()->at(nColumn);
					if (nCellImage > m_pColAlternate->GetIntArrayPtr()->at(nColumn))
						nCellImage = m_pColAlternate->GetIntArrayPtr()->at(nColumn);
				}
			}
		
			// if check boxes are to be drawn.
			switch (nCellStyle)
			{
				case Grid_CheckBoxes:
				case Grid_OptionButtons:
				case Grid_AcadColors:
				case Grid_ArrowHead:
				//case Grid_LineWeights:
				case Grid_LineWeightCell:
				case Grid_LayerList:
				case Grid_AcadColorCell:
				case Grid_TrueColorCell:
				case Grid_Fonts:
				{
					CRect rcBox;
					rcBox.left = rcCol.left + 2;
					rcBox.top = rcCol.top + 1;
					rcBox.right = rcBox.left + 13;
					rcBox.bottom = rcBox.top + 13;
					nImageOffset = 16;
					bDrawIcon = false;

					int nImage = GetItemImage(lvi.iItem, nColumn);					
				
					int nColStyle = nCellStyle;

					switch (nCellStyle)
					{
						case Grid_CheckBoxes:
						{
							if (GetItemText(lvi.iItem, nColumn).GetLength() == 0)
							{
								rcBox.top++;
								rcBox.left = rcCol.left + ((rcCol.Width()-13) / 2);
								rcBox.right = rcBox.left + 13;
								DrawCheckBox(pDC, rcBox, nImage == 1, bHighlighted);
							}
							else
							{
								rcBox.top++;
								rcBox.bottom++;
								DrawCheckBox(pDC, rcBox, nImage == 1, bHighlighted);
							}
							break;
						}
					
						case Grid_OptionButtons:
						{
							DrawOptionButton(pDC, rcBox, nImage == 1, backGround, bHighlighted);							
							break;
						}
					
						case Grid_AcadColors:
						case Grid_AcadColorCell:
						{
							if (nImage > 256)
								nImage = 0;

							if (nImage == -1)
							{
								DrawColor(pDC, rcBox, nImage, sLabel);
								SetItemText(lvi.iItem, nColumn, sLabel);
								SetItemImage(lvi.iItem, nColumn, nImage);
							}
							else
							{
								DrawColor(pDC, rcBox, nImage, sLabel);
							}
							break;
						}
						case Grid_TrueColorCell:
						{
							DrawTrueColor(pDC, rcBox, nImage, sLabel);
							/*
							COLORREF clr;
							if (nImage >= -1 && nImage <= 256)								
							{
								if (nImage == -1)
								{
									DrawColor(pDC, rcBox, nImage, sLabel);
									SetItemText(lvi.iItem, nColumn, sLabel);
									SetItemImage(lvi.iItem, nColumn, nImage);
								}
								else
								{
									DrawColor(pDC, rcBox, nImage, sLabel);
								}
							}
							if (nImage < 0)
							{	
								int nComma = sLabel.Find(_T(","));
								int nComma2 = sLabel.Find(_T(","), nComma+1);

								if (nComma > -1 && nComma2 > -1)
								{
									CString sRed = sLabel.Left(nComma);
									CString sGreen = sLabel.Mid(nComma+1, nComma2-nComma);
									CString sBlue = sLabel.Mid(nComma2+1);
									
									int nRed = atol(sRed);
									int nGreen = atol(sGreen);
									int nBlue = atol(sBlue);
									
									clr = RGB(nRed, nGreen, nBlue);									
									DrawTrueColor(pDC, rcBox, clr);
								}
							}
							if (nImage == 300) // if a color book color
							{
								AcCmColor clrBook;
								Acad::ErrorStatus es;

								CString sColorBook = sLabel;
								int n = sColorBook.Find(sDollarSign);
								sLabel = sColorBook.Mid(n+1);
								sColorBook = sColorBook.Left(n);

								es = accmGetColorFromColorBookName(clrBook,
									sColorBook,
									sLabel);								
								
								clr = RGB(clrBook.red(), clrBook.green(), clrBook.blue());	
								DrawTrueColor(pDC, rcBox, clr);
							}
							*/

							break;
						}
						case Grid_LayerList:
						{
							//if (nImage < 0 || nImage > 256)
							//	nImage = 0;

							CheckLayer(sLabel, nImage);
							DrawColor(pDC, rcBox, nImage, CString());
							break;
						}
					

						case Grid_ArrowHead:
						{
							if (nImage > 20)
								nImage = 20;

							if (nImage == -1)
							{
								DrawArrowHeads(pDC, rcBox, nImage, sLabel);
								SetItemText(lvi.iItem, nColumn, sLabel);
								SetItemImage(lvi.iItem, nColumn, nImage);
							}
							else
							{
								DrawArrowHeads(pDC, rcBox, nImage, sLabel);
							}
							break;
						}
					
						case Grid_Fonts:
						{
							if (nImage > 1)
								nImage = 0;

							if (nImage == -1)
							{
								DrawFontIcons(pDC, rcBox, nImage, sLabel);
								SetItemText(lvi.iItem, nColumn, sLabel);
								SetItemImage(lvi.iItem, nColumn, nImage);
							}
							else
							{
								DrawFontIcons(pDC, rcBox, nImage, sLabel);
							}
							break;
						}

						case Grid_LineWeightCell:
						{
							AcDb::LineWeight newLW = (AcDb::LineWeight)nImage;

							if (newLW == AcDb::kLnWtByLayer)
							{
								sLabel = theWorkspace.LoadResourceString(4221);
							}
							else if (newLW == AcDb::kLnWtByBlock)
							{
								sLabel = theWorkspace.LoadResourceString(4222);
							}
							else if (newLW == AcDb::kLnWtByLwDefault)
							{
								sLabel = theWorkspace.LoadResourceString(4223);
							}
							else if (getvar(_T("LWUNITS")))
							{
								double dValue = ((double) newLW) / 100;
								sLabel.Format( _T("%.2f mm"), dValue);								
							}
							else
							{
								double dValue = ((double) newLW);								
								dValue = dValue / 2540.0;
								sLabel.Format( _T("%.3f\""), dValue);
							}

							DrawLineWeights(pDC, rcBox, newLW);
						}
					}
				}
			}
			hTheme = NULL;

			lvi.iImage = nCellImage;
		}

		if (pImageList && nCellImage > -1 && bDrawIcon) 
		{
			rcIcon.left = rcCol.left + 2;
			IMAGEINFO inf;
			pImageList->GetImageInfo(nCellImage, &inf);
			nImageOffset = inf.rcImage.right - inf.rcImage.left + 2;
						
			//UINT nOvlImageMask = lvc.state & LVIS_OVERLAYMASK;
			pImageList->Draw(pDC,
				nCellImage, CPoint(rcIcon.left, rcIcon.top-1), 
				//(bHighlighted ? ILD_BLEND50 : 0) |
				ILD_TRANSPARENT);// | nOvlImageMask ); 			
		}

		if (sLabel.GetLength() > 0)
		{
			// Get the text justification 
			UINT nJustify = DT_LEFT;
			switch (lvc.fmt & LVCFMT_JUSTIFYMASK) { 
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT; 
				break;
	            
			case LVCFMT_CENTER:
				nJustify = DT_CENTER;
				break;
	            
			default:
				break;
			}
	        
			rcLabel = rcCol;
	        
			rcLabel.left += offset + nImageOffset;
			rcLabel.right -= offset;
			rcLabel.top++;
			
			if (nCellStyle == Grid_Password)
			{
				int nCount = sLabel.GetLength();
				sLabel = CString();
				for (int k=0; k<nCount; k++)
					sLabel += _T("*");
			}

			if (nCellStyle == Grid_MultiLine)
				bWordWrap = true;

			if (bWordWrap)
				pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT |
					DT_NOPREFIX | DT_EDITCONTROL | 
					DT_TOP | DT_WORDBREAK); 
			else
				pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT |
					DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | 
					DT_TOP | DT_END_ELLIPSIS); 		
		}
  }

	// restore the old font ** a must
	SelectObject(pDC->m_hDC, pOldFont);
	
//	BitBlt(pDC->m_hDC,rcItem.left,rcItem.top,rcItem.Width()+1,m_nRowHeight+1,pDC->m_hDC,rcItem.left,rcItem.top,SRCCOPY);

 //   pDC->SelectObject(pBitmapOld);
    //pDC->RestoreDC( nSavedDC );
}

void CArxGridCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	//its not meself
	if( GetFocus() != this) 
		SetFocus();
	
	HideEditControls();
	
	//Invalidate();

	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CArxGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( GetFocus() != this) 
		SetFocus();

	HideEditControls();
	__super::OnVScroll(nSBCode, nPos, pScrollBar);
}
