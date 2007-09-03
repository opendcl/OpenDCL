// ArxGridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxGridCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"
#include "ButtonEditCtrl.h"
#include "ComboEditCtrl.h"
#include "ImageComboEditCtrl.h"
#include "TextBoxEditCtrl.h"
#include "ToggleEditCtrl.h"
#include "RadioEditCtrl.h"
#include "AngleFilter.h"
#include "CurrencyFilter.h"
#include "DateFilter.h"
#include "IntegerFilter.h"
#include "LowerCaseFilter.h"
#include "MultilineFilter.h"
#include "NumericFilter.h"
#include "TimeFilter.h"
#include "UpperCaseFilter.h"
//#include "VdclArrowHeadComboBox.h"
//#include "ComboBoxHolder.h"
//#include "ComboBoxFolder.h"
//#include "CurrencyEdit.h"
//#include "OdclEdit.h"
//#include "PercentageEdit.h"
//#include "VdclAngleEdit.h"
//#include "FontCombo.h"
//#include "OdclLayerCombo.h"
//#include "PrinterComboBox.h"
//#include "VdclPlotStyleNamesComboBox.h"
//#include "VdclPlotStyleTablesComboBox.h"
//#include "OdclDimStylesCombo.h"
//#include "VdclColorComboBox.h"
//#include "VdclComboBox.h"
//#include "VdclComboBoxEx.h"
//#include "VdclNumericEdit.h"
//#include "VdclSymbolEdit.h"
//#include "DateTimeHolder.h"
//#include "LineWeightDlg.h"
//#include "LinetypeDlg.h"
#include "DirDialog.h"
#include "Workspace.h"
#include "AcadColorTable.h"
#include "SharedRes.h"


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


static int GetCurrentLayerColor()
{
	AcDbObjectId idLayer = acdbHostApplicationServices()->workingDatabase()->clayer();
	AcDbLayerTableRecord* pLayer = NULL;
	if( Acad::eOk != acdbOpenObject( pLayer, idLayer, AcDb::kForRead ) )
		return 7;
	int nColor = pLayer->color().colorIndex();
	pLayer->close();
	return nColor;
}

class CTrueColorEditCtrl : public CGridCellEditCtrl
{
public:
	CTrueColorEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol )
		: CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		{
			int nCellImage = pGridCtrl->GetCellImage( nRow, nCol );
			resbuf rbCurColor = { NULL, RTSHORT };
			rbCurColor.resval.rint = GetCurrentLayerColor();
			resbuf rbT = { &rbCurColor, RTT };
			resbuf rbColor = { &rbT };
			resbuf rbFuncName = { &rbColor, RTSTR };
			rbFuncName.resval.rstring = _T("acad_truecolordlg");
			CString sText = pGridCtrl->GetCellText( nRow, nCol );
			if( nCellImage == -1 )
			{
				int idxComma = sText.Find( _T(",") );
				CString sRed = sText.Left( idxComma );
				CString sGreen = sText.Mid( idxComma + 1 );
				idxComma = sGreen.Find( _T(",") );
				CString sBlue = sGreen.Mid( idxComma + 1 );
				sGreen = sGreen.Left( idxComma );
				rbColor.restype = 420;
				rbColor.resval.rlong =
					(long)RGB(_tstol( sBlue ), _tstol( sGreen ), _tstol( sRed ) ); //note: BGR instead of RGB!
			}
			else if( !sText.IsEmpty() )
			{
				rbColor.restype = 430;
				rbColor.resval.rstring = sText.LockBuffer(); //color book color
			}
			else
			{
				rbColor.restype = RTSHORT;
				rbColor.resval.rint = (nCellImage > 0 && nCellImage < 256)? nCellImage : 7;
			}
			resbuf* prbResult = NULL;
			if( RTNORM == acedInvoke( &rbFuncName, &prbResult ) && prbResult )
			{
				resbuf* prbTrueColor = prbResult->rbnext;
				resbuf* prbColorBook = (prbTrueColor? prbTrueColor->rbnext : NULL);
				if( prbColorBook && prbColorBook->restype == 430 )
					pGridCtrl->SetCellTextImage( nRow, nCol, prbColorBook->resval.rstring, 300 );
				else if( prbTrueColor && prbTrueColor->restype == 420 )
				{
					COLORREF clr = (COLORREF)prbTrueColor->resval.rlong; //note: BGR instead of RGB!
					CString sColor;
					sColor.Format( _T("%d,%d,%d"), GetBValue( clr ), GetGValue( clr ), GetRValue( clr ) );
					pGridCtrl->SetCellTextImage( nRow, nCol, sColor, -1 );
				}
				else
				{
					CString sColor;
					sColor.Format( _T("%d"), prbResult->resval.rint );
					pGridCtrl->SetCellTextImage( nRow, nCol, sColor, prbResult->resval.rint );
				}
				acutRelRb( prbResult );
			}
		}
	virtual ~CTrueColorEditCtrl()
		{
		}
};


/////////////////////////////////////////////////////////////////////////////
// CArxGridCtrl

CArxGridCtrl::CArxGridCtrl( CDclControlObject* pTemplate,
														CControlPane* pPane,
														UINT nID,
														bool bCreate /*= true*/ )
: CGridCtrl( pTemplate, pPane, nID, false )
, mArxServices( pTemplate )
{
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

	if( GetTemplate()->GetLongProperty(Prop::EventInvoke) == 1 )
		mbInvokeWithSendString = true;
	else
		mbInvokeWithSendString = false;

	return bSuccess;
}

bool CArxGridCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	//switch( pProp->GetID() )
	//{
	//case Prop::DragnDropAllowDrop:
	//	{
	//		SetDragnDrop( pProp->GetBooleanValue() );
	//		break;
	//	}
	//}
	return !bFailed;
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

void CArxGridCtrl::OnEditCurCell()
{
	__super::OnEditCurCell();
	if( !IsEditing() )
		return;
	InvokeMethodIntInt( mpTemplate->GetStrProperty(Prop::EventBeginLabelEdit),
											mCurrentCell.row(),
											mCurrentCell.col(),
											mbInvokeWithSendString);
}

void CArxGridCtrl::OnEndEditCurCell()
{
	InvokeMethodIntInt( mpTemplate->GetStrProperty(Prop::EventEndLabelEdit),
											mCurrentCell.row(),
											mCurrentCell.col(),
											mbInvokeWithSendString);
	__super::OnEndEditCurCell();
}

void CArxGridCtrl::DrawColor( CDC& cdc, const CRect& rcIcon, int nColor, const CString& sText )
{
	CRect rc = rcIcon;
	//if (nColor == -1)
	//{
	//	VdclColorComboBox *pComboBox = NULL;
	//	CComboBoxHolder *pHolder;
	//	
	//	// get or create the control.
	//	if (m_pComboBox[4] == NULL)
	//	{
	//		pComboBox = new VdclColorComboBox;
	//		pHolder = new CComboBoxHolder;
	//		
	//		pHolder->Create(CString(), WS_CHILD, CRect(1,1,10,10), this, 124);
	//		
	//		pHolder->pComboBox = pComboBox;
	//		pComboBox->Create(4, CRect(1,1,10,10), pHolder, 410);
	//		pComboBox->SetFont(GetFont());

	//		pHolder->m_nStyle = 4;
	//		pHolder->SetFont(GetFont());				
	//		m_pComboBox[4] = pHolder;
	//		
	//	}
	//	else
	//	{
	//		pHolder = (CComboBoxHolder*)m_pComboBox[4];
	//		pComboBox = (VdclColorComboBox*)pHolder->pComboBox;
	//	}

	//	int n = -1;
	//	if (sText.GetLength() == 0)
	//	{
	//		pComboBox->GetLBText(8, sText);
	//	}

	//	n = pComboBox->FindStringExact(0, sText);
	//	if (n == -1)
	//	{
	//		n = pComboBox->FindString(0, sText);
	//		if (n != -1)
	//		{					
	//			pComboBox->GetLBText(n, sText);
	//		}
	//	}
	//	if (n == -1)
	//	{	
	//		nColor = _ttoi(sText);

	//		n = pComboBox->FindItemByColorIndex(nColor);
	//		
	//		if (n == -101)
	//			sText = LTOA(nColor);
	//		else
	//			pComboBox->GetLBText(n, sText);
	//	}
	//	else
	//	{
	//		nColor = pComboBox->GetItemColorIndex(n);
	//	}
	//}
	
	// setup the CRect for FillRect	
	CPoint point;
	
	// move the color splotch down by one.
	rc.top++;
	rc.bottom++;

	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, RGB(0,0,0));
	HGDIOBJ OldPen = SelectObject(cdc.m_hDC, pen);
		
	::Rectangle(cdc.m_hDC, rc.left, rc.top, rc.right, rc.bottom);

	SelectObject(cdc.m_hDC, OldPen);			
	DeleteObject(pen);

	rc.DeflateRect(1,1);

	// draw the solid rectangle
	COLORREF rgb;
	
	if (nColor == 256 || nColor == 0)
	{
		rgb = RGB(255,255,255);
		::SetBkColor(cdc.m_hDC, rgb);
	}
	else
	{
		rgb = GetRGBColor(nColor);
		::SetBkColor(cdc.m_hDC, rgb);
	}

	::ExtTextOut(cdc.m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	::SetBkColor(cdc.m_hDC, rgb);
}

void CArxGridCtrl::DrawFontIcons( CDC& cdc, const CRect& rcIcon, int nImage, const CString& sText )
{
	CRect rc = rcIcon;
/*
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
*/

	rc.top++;

	//if (m_pFontImages == NULL)
	//{
	//	m_pFontImages = new CImageList();
	//	m_pFontImages->Create(15,13,ILC_COLOR4 | ILC_MASK, 3, 1);
	//	
	//	HMODULE hRes = theWorkspace.GetResourceModule();

	//	HICON hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_TRUEFONT), IMAGE_ICON, 0, 0, 0);	
	//	m_pFontImages->Add(hIcon);
	//	DestroyIcon(hIcon);

	//	hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_ACADFONT), IMAGE_ICON, 0, 0, 0);
	//	m_pFontImages->Add(hIcon);
	//	DestroyIcon(hIcon);
	//}

	//if (nImage > -1)
	//	m_pFontImages->Draw(pDC, nImage, rc.TopLeft(), ILD_TRANSPARENT);
}

/*
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
		mCurrentCell.row() = nRow;
		mCurrentCell.col() = nCol;
	}
	else
	{
		mCurrentCell.row() = -1;
		mCurrentCell.col() = -1;
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
*/

/*
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
*/

/*
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
*/

void CArxGridCtrl::DoFileDlg(CellStyle nStyle) 
{
	switch (nStyle)
	{
		case Grid_DirectoryCell:
		{			
			CDirDialog dlg( theWorkspace.LoadResourceString(IDS_SELFOLDER), GetItemText(mCurrentCell.row(), mCurrentCell.col()) );
			if (dlg.DoBrowse(this) == TRUE)
				SetCellTextImage( mCurrentCell.row(), mCurrentCell.col(), dlg.GetSelectedFolder(), -1 );
			break;
		}
		case Grid_DwgFilesCell:
		{
			struct resbuf * result;
			CString sFileExt = _T("dwg;dxf");
			CString sFile = GetItemText(mCurrentCell.row(), mCurrentCell.col());
			result = acutNewRb(RTSTR);

			const _CellData* pCellData = GetCellData( mCurrentCell.row(), mCurrentCell.col() );
			if( pCellData && !pCellData->mrsComboList.empty() )
				sFileExt = pCellData->mrsComboList.front().c_str();
			else
			{
				std::vector< tstring > rsList;
				std::vector< int > ridxImage;
				if( GetCellComboListItems( mCurrentCell.row(), mCurrentCell.col(), rsList, ridxImage ) && !rsList.empty() )
					sFileExt = rsList.front().c_str();
			}
			if (acedGetFileD(sFile, sFile, sFileExt, 8, result) == RTNORM)
				SetCellTextImage( mCurrentCell.row(), mCurrentCell.col(), result->resval.rstring, -1 );
			acutRelRb(result);
			break;
		}
	}
}

CGridCellEditCtrl* CArxGridCtrl::CreateEditControl( int nRow, int nCol )
{
	switch( GetCellStyle( nRow, nCol ) )
	{
		case Grid_CheckBoxes: return new CToggleEditCtrl( this, nRow, nCol );
		case Grid_OptionButtons: return new CRadioEditCtrl( this, nRow, nCol );
		case Grid_SwitchableIcons: return new CToggleEditCtrl( this, nRow, nCol );
		case Grid_EllipsesButtons: return new CButtonEditCtrl( this, nRow, nCol, _T("..."), ID_CELLBUTTON );
		case Grid_PickButtons: return new CButtonEditCtrl( this, nRow, nCol, IDI_PICSM, ID_CELLBUTTON );
		case Grid_Strings: return new CTextBoxEditCtrl( this, nRow, nCol );
		case Grid_AngleUnits: return new CTextBoxEditCtrl( this, nRow, nCol, new CAngleFilter );
		case Grid_Integers: return new CTextBoxEditCtrl( this, nRow, nCol, new CIntegerFilter );
		case Grid_Units: return new CTextBoxEditCtrl( this, nRow, nCol );
		case Grid_UpperCase: return new CTextBoxEditCtrl( this, nRow, nCol, new CUpperCaseFilter );
		case Grid_LowerCase: return new CTextBoxEditCtrl( this, nRow, nCol, new CLowerCaseFilter );
		case Grid_Password: return new CTextBoxEditCtrl( this, nRow, nCol );
		case Grid_MultiLine: return new CTextBoxEditCtrl( this, nRow, nCol, new CMultilineFilter );
		case Grid_Currency: return new CTextBoxEditCtrl( this, nRow, nCol, new CCurrencyFilter );
		case Grid_Date: return new CTextBoxEditCtrl( this, nRow, nCol, new CDateFilter );
		case Grid_Time: return new CTextBoxEditCtrl( this, nRow, nCol, new CTimeFilter );
		case Grid_Percentage: return new CTextBoxEditCtrl( this, nRow, nCol, new CIntegerFilter );
		case Grid_DropDown:
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
			return new CComboDropdownListEditCtrl( this, nRow, nCol );
		case Grid_ImageDropList: return new CImageComboDropdownListEditCtrl( this, nRow, nCol );
		case Grid_AcadColorCell: return new CTextBoxEditCtrl( this, nRow, nCol );
		case Grid_TrueColorCell: return new CTrueColorEditCtrl( this, nRow, nCol );
		case Grid_LineWeightCell: return new CTextBoxEditCtrl( this, nRow, nCol );
		case Grid_LinetypeCell: return new CTextBoxEditCtrl( this, nRow, nCol );
		case Grid_DirectoryCell: return new CButtonEditCtrl( this, nRow, nCol, IDI_FOLDER, ID_CELLBUTTON );
		case Grid_DwgFilesCell: return new CButtonEditCtrl( this, nRow, nCol, IDI_FOLDER, ID_CELLBUTTON );
		case Grid_Strings_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol );
		case Grid_AngleUnits_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CAngleFilter );
		case Grid_Integers_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CIntegerFilter );
		case Grid_Units_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol );
		case Grid_UpperCase_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CUpperCaseFilter );
		case Grid_LowerCase_Combo: return new CComboDropdownEditCtrl( this, nRow, nCol, new CLowerCaseFilter );
		//default: return new CTextBoxEditCtrl( this, nRow, nCol );
	}
	return NULL;
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
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxGridCtrl message handlers

void CArxGridCtrl::OnCellButtonClicked(void)
{
	switch( GetCurCellStyle() )
	{
	case Grid_DirectoryCell: return DoFileDlg( Grid_DirectoryCell );
	case Grid_DwgFilesCell: return DoFileDlg( Grid_DwgFilesCell );
	}
	InvokeMethodIntInt( mpTemplate->GetStrProperty(Prop::EventBtnClicked),
											mCurrentCell.row(),
											mCurrentCell.col(),
											mbInvokeWithSendString);
}

void CArxGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt( mpTemplate->GetStrProperty(Prop::EventMouseDown),
														1,
														nFlags,
														point.x,
														point.y,
														mbInvokeWithSendString);

	__super::OnLButtonDown(nFlags, point);


	if( !mCurrentCell )
		return;

	InvokeMethodIntInt( mpTemplate->GetStrProperty(Prop::EventClicked),
											mCurrentCell.row(),
											mCurrentCell.col(),
											mbInvokeWithSendString);
}

void CArxGridCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(Prop::EventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		mbInvokeWithSendString);
	__super::OnRButtonDown(nFlags, point);
}

void CArxGridCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(Prop::EventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		mbInvokeWithSendString);
	__super::OnMButtonDown(nFlags, point);
}

void CArxGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( !mCurrentCell )
		return;

	InvokeMethodIntInt( mpTemplate->GetStrProperty(Prop::EventDblClicked),
											mCurrentCell.row(),
											mCurrentCell.col(),
											mbInvokeWithSendString);
}

void CArxGridCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (mpTemplate)		
		InvokeMethodStringIntInt(mpTemplate->GetStrProperty(Prop::EventKeyUp), CString() + (char)nChar,  (int)nRepCnt,  (int)nFlags, mbInvokeWithSendString);
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (mpTemplate)
	{
		TCHAR sChar = nChar;
		InvokeMethodStringIntInt(mpTemplate->GetStrProperty(Prop::EventKeyDown), CString( (TCHAR)sChar ),  (int)nRepCnt, (int)nFlags, mbInvokeWithSendString);
	}

	switch (nChar) 
	{
		case VK_ESCAPE: 
			break;
		case VK_RETURN: 
			OnEditCurCell();
			return;
		case VK_UP: 
		{
			MoveUp();
			return;
		}
		case VK_DOWN: 
		{
			MoveDown();
			return;
		}
		case VK_LEFT: 
		{
			MoveLeft();
			return;
		}
		case VK_RIGHT: 
		{
			MoveRight();
			return;
		}
	}
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxGridCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(mpTemplate->GetStrProperty(Prop::EventKillFocus), mbInvokeWithSendString);
	*pResult = 0;
}

void CArxGridCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(Prop::EventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		mbInvokeWithSendString);
	__super::OnLButtonUp(nFlags, point);
}

void CArxGridCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(Prop::EventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		mbInvokeWithSendString);
	__super::OnRButtonUp(nFlags, point);
}

void CArxGridCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(Prop::EventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		mbInvokeWithSendString);
	__super::OnMButtonUp(nFlags, point);
}

void CArxGridCtrl::OnContextMenu( CWnd* pTarget, CPoint point )
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(Prop::EventMouseDown),
		2,
		MK_RBUTTON,
		point.x,
		point.y,
		mbInvokeWithSendString);
	__super::OnContextMenu(pTarget, point);
}

void CArxGridCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		mpTemplate->GetStrProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		mbInvokeWithSendString);
	__super::OnMouseMove(nFlags, point);
}

/*
void CArxGridCtrl::DoEditCellNow(int nStyle) 
{
	
	switch (nStyle)
	{		
	case Grid_AngleUnits_Combo:
		{
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), nStyle, nChar);
			
			TPropertyPtr pProp = mpTemplate->GetPropertyObject(Prop::ColumnListItems);	
			
			if (GetCellData(mCurrentCell.row(),	mCurrentCell.col()) != NULL)
				ShowComboBox(mCurrentCell.row(), mCurrentCell.col(), 2, GetCellData(mCurrentCell.row(), mCurrentCell.col())->sStrings);
			else if (mCurrentCell.col() < pProp->GetStringArrayListPtr()->size())
					ShowComboBox(mCurrentCell.row(), mCurrentCell.col(), 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(mCurrentCell.col())));
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
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), nStyle, nChar);
			
			TPropertyPtr pProp = mpTemplate->GetPropertyObject(Prop::ColumnListItems);	

			if (GetCellData(mCurrentCell.row(),	mCurrentCell.col()) != NULL)
				ShowComboBox(mCurrentCell.row(), mCurrentCell.col(), 2, GetCellData(mCurrentCell.row(), mCurrentCell.col())->sStrings);
			else if (mCurrentCell.col() < pProp->GetStringArrayListPtr()->size())
				ShowComboBox(mCurrentCell.row(), mCurrentCell.col(), 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(mCurrentCell.col())));

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
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), nStyle, nChar);
			
			TPropertyPtr pProp = mpTemplate->GetPropertyObject(Prop::ColumnListItems);	

			if (GetCellData(mCurrentCell.row(),	mCurrentCell.col()) != NULL)
				ShowComboBox(mCurrentCell.row(), mCurrentCell.col(), 2, GetCellData(mCurrentCell.row(), mCurrentCell.col())->sStrings);
			else if (mCurrentCell.col() < pProp->GetStringArrayListPtr()->size())
				ShowComboBox(mCurrentCell.row(), mCurrentCell.col(), 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(mCurrentCell.col())));
			
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
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), nStyle, nChar);
			
			TPropertyPtr pProp = mpTemplate->GetPropertyObject(Prop::ColumnListItems);	

			if (GetCellData(mCurrentCell.row(),	mCurrentCell.col()) != NULL)
				ShowComboBox(mCurrentCell.row(), mCurrentCell.col(), 2, GetCellData(mCurrentCell.row(), mCurrentCell.col())->sStrings);
			else if (mCurrentCell.col() < pProp->GetStringArrayListPtr()->size())
				ShowComboBox(mCurrentCell.row(), mCurrentCell.col(), 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(mCurrentCell.col())));
			
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
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), 0, nChar);
			break;
	case Grid_AngleUnits:
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), 1, nChar);
			break;
	case Grid_Integers:
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), 2, nChar);
			break;
	case Grid_Units:
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), 3, nChar);
			break;
	case Grid_UpperCase:
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), 5, nChar);
			break;
	case Grid_LowerCase:
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), 6, nChar);
			break;
	case Grid_Password:
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), 7, nChar);
			break;
	case Grid_MultiLine:
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), 8, nChar);
			break;
	case Grid_Currency:
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), 9, nChar);
			break;
	case Grid_Date:
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), 10, nChar);
			break;
	case Grid_Time:
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), 11, nChar);
			break;
	case Grid_Percentage:
			ShowTextBox(mCurrentCell.row(),	mCurrentCell.col(), 12, nChar);
			break;			
	case Grid_DropDown:
		{
			TPropertyPtr pProp = mpTemplate->GetPropertyObject(Prop::ColumnListItems);	
			if (GetCellData(mCurrentCell.row(),	mCurrentCell.col()) != NULL)
				ShowComboBox(mCurrentCell.row(), mCurrentCell.col(), 2, GetCellData(mCurrentCell.row(), mCurrentCell.col())->sStrings);
			else if (mCurrentCell.col() < pProp->GetStringArrayListPtr()->size())
				ShowComboBox(mCurrentCell.row(), mCurrentCell.col(), 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(mCurrentCell.col())));
		break;
		}
	case Grid_ImageDropList:
		{
			if (GetCellData(mCurrentCell.row(), mCurrentCell.col()) != NULL &&
				GetCellData(mCurrentCell.row(), mCurrentCell.col())->sStrings.GetSize() > 0)
			{
				ShowImageComboBox(mCurrentCell.row(), 
						mCurrentCell.col(),
						GetCellData(mCurrentCell.row(), mCurrentCell.col())->sStrings,
						GetCellData(mCurrentCell.row(), mCurrentCell.col())->nInts);
			}
			else
			{
				TPropertyPtr pProp = mpTemplate->GetPropertyObject(Prop::ColumnListItems);	
				TPropertyPtr pPropImages = mpTemplate->GetPropertyObject(Prop::ColumnListImages);
				if (mCurrentCell.col() < pProp->GetStringArrayListPtr()->size())
				{
					PropVal::TIntArray& rInts = pPropImages->GetIntArrayListPtr()->at(mCurrentCell.col());
					CArray< int, int > rInt;
					for(int idx = 0; idx < rInts.size(); idx++)
						rInt.Add(rInts[idx]);
					ShowImageComboBox(mCurrentCell.row(), 
							mCurrentCell.col(),
							PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(mCurrentCell.col())),
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
			ShowComboBox(mCurrentCell.row(), mCurrentCell.col(), nStyle-16, sStrings);
		break;
		}	
		
	case Grid_AcadColorCell:
		{
			int nCurrentColor = GetItemImage(mCurrentCell.row(), mCurrentCell.col());
			if (acedSetColorDialog(
					nCurrentColor,
					TRUE,
					GetCurrentLayerColor()) == TRUE)
			{
				AcCmColor retColor;
				CString sDesc;

				retColor.setColorIndex(nCurrentColor);
				sDesc = retColor.colorNameForDisplay();
				SetItemText(mCurrentCell.row(), mCurrentCell.col(), sDesc);
				SetItemImage(mCurrentCell.row(), mCurrentCell.col(), nCurrentColor);
			}
			// fire the on Grid edit cell event.
			EndEditControls(this);
		break;
		}
	case Grid_TrueColorCell:
		{
				COLORREF clr = 0;
			int nImage = GetItemImage(mCurrentCell.row(), mCurrentCell.col());
		
			CString sLabel = GetItemText(mCurrentCell.row(), mCurrentCell.col());
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
			
				SetItemImage(mCurrentCell.row(), mCurrentCell.col(), -1);
				SetItemText(mCurrentCell.row(), mCurrentCell.col(), sColor);
			}
			// fire the on Grid edit cell event.
			EndEditControls(this);
		break;
		}
		
	case Grid_LineWeightCell:
		{
			AcDb::LineWeight newLW = (AcDb::LineWeight)GetItemImage(mCurrentCell.row(), mCurrentCell.col());
			SelectLineWeightUI(newLW, true, this);
			SetItemImage(mCurrentCell.row(), mCurrentCell.col(), (int)newLW);
			
			// fire the on Grid edit cell event.
			EndEditControls(this);
			break;
		}
	case Grid_LinetypeCell:
		{
			CString sLinetype;
			AcDbObjectId idLinetype;
			SelectLinetypeUI(idLinetype, sLinetype, true, this);
			SetItemText(mCurrentCell.row(), mCurrentCell.col(), sLinetype);

			// fire the on Grid edit cell event.
			EndEditControls(this);
			break;
		}
	case Grid_DirectoryCell:
		{			
			CDirDialog dlg( theWorkspace.LoadResourceString(IDS_SELFOLDER), GetItemText(mCurrentCell.row(), mCurrentCell.col()) );
			if (dlg.DoBrowse(this) == TRUE)
			{
				SetItemText(mCurrentCell.row(), mCurrentCell.col(), dlg.GetSelectedFolder());
				SetItemImage(mCurrentCell.row(), mCurrentCell.col(), -1);
			}
			// fire the on Grid edit cell event.
			EndEditControls(this);
			break;
		}
	case Grid_DwgFilesCell:
		{
			struct resbuf * result;
			CString sFile = GetItemText(mCurrentCell.row(), mCurrentCell.col());
			result = acutNewRb(RTSTR);

			if (acedGetFileD(sFile, sFile, _T("dwg"), 8, result) == RTNORM)
			{
				SetItemText(mCurrentCell.row(), mCurrentCell.col(), result->resval.rstring);
				SetItemImage(mCurrentCell.row(), mCurrentCell.col(), -1);
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
		mbShowHighlight = true;
		ShowCurSel();
	}
}
*/

void CArxGridCtrl::OnSelectionChanged() 
{
	InvokeMethodIntInt(mpTemplate->GetStrProperty(Prop::EventSelChanged), mCurrentCell.row(), mCurrentCell.col(), mbInvokeWithSendString);
}

/*
void CArxGridCtrl::ShowTextBox(int nRow, int nCol, int nStyle)
{
	int nCount = GetItemCount();
	
	if (nRow > -1 && nCol > -1)
	{
		mCurrentCell.row() = nRow;
		mCurrentCell.col() = nCol;
	}
	else
	{
		mCurrentCell.row() = -1;
		mCurrentCell.col() = -1;
	}
	
	COLORREF backGround = GetBkColor();
	
	if (!m_bOrientationVer)
	{
		if ((mCurrentCell.row() % 2) != 0)
			backGround = alertnateColor; 
	}
	else
	{
		int n = 0;
		if (m_bHasRowHeader)
			n = 1;
		if (((mCurrentCell.col()+n) % 2) != 0)
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
		int nCalc = (mpTemplate->GetLongProperty( Prop::RowHeight ) * 5);
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
		mCurrentCell.row() = nRow;
		mCurrentCell.col() = nCol;
	}
	else
	{
		mCurrentCell.row() = -1;
		mCurrentCell.col() = -1;
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
		mCurrentCell.row() = nRow;
		mCurrentCell.col() = nCol;
	}
	else
	{
		mCurrentCell.row() = -1;
		mCurrentCell.col() = -1;
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
			int nRowHeight = mpTemplate->GetLongProperty( Prop::RowHeight );
			if (nRowHeight == 19)
				rc.bottom++;
			if (nRowHeight >= 20)
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
			int nRowHeight = mpTemplate->GetLongProperty( Prop::RowHeight );
			if (nRowHeight == 19)
				rc.bottom++;
			if (nRowHeight >= 20)
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
*/

void CArxGridCtrl::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HideEditControls();
	*pResult = 0;
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
	InvokeMethodInt(mpTemplate->GetStrProperty(Prop::EventColumnClick), pNMListView->iSubItem, mbInvokeWithSendString);
	*pResult = 0;
}

void CArxGridCtrl::DrawCell( int nRow, int nCol, const CRect& rectCell, CDC& cdc )
{
	__super::DrawCell( nRow, nCol, rectCell, cdc );
/*
	if( lpDrawItemStruct->CtlType != ODT_LISTVIEW )
        return;

	COLORREF backGround = GetColorService()->GetBackgroundColor();
	COLORREF back = backGround;
	COLORREF foreGround = GetColorService()->GetForegroundColor();
	
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem); 
	
	CRect rcClipBox;
	pDC->GetClipBox(rcClipBox);

  int nItem = lpDrawItemStruct->itemID;
  CImageList* pImageList; 
    
	CRect rc;
  GetClientRect(rc);    
  HGDIOBJ pOldFont = SelectObject(pDC->m_hDC, GetFont()->m_hObject);

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

	if (mbHasRowHeader == true)
	{
		CThemeHelperST* pTheme = GetThemeHelper();
		if( pTheme )
			hTheme = pTheme->OpenThemeData(GetSafeHwnd(), L"HEADER");
		if (hTheme == NULL) // If the handle cannot be retrieved, go back to non-visual style drawing code.
		{			
			if (nItem == GetTopIndex())
			{
				rcBtn.top--;
				rcBtn.top--;
			}
			
			rcBtn.bottom++;

			pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
			pDC->SetBkColor(::GetSysColor(COLOR_BTNFACE));
			CBrush br(::GetSysColor(COLOR_BTNFACE));
			pDC->FillRect(rcBtn, &br);
			br.DeleteObject();

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

			pTheme->DrawThemeBackground(hTheme, m_hWnd, pDC->GetSafeHdc(), HP_HEADERITEM, iStateId, &rcTheme, NULL);

			pTheme->CloseThemeData(hTheme);

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
			CBrush br(::GetSysColor(COLOR_HIGHLIGHT));
			pDC->FillRect(rcHighlight, &br);
			br.DeleteObject();
		}
		else
		{
			CBrush br(backGround);
			pDC->FillRect(rcHighlight, &br); 
			br.DeleteObject();
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
	if (!mbHasRowHeader)
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

		
		if (mbShowHighlight)
		{
			switch(nCellStyle)
			{
				case Grid_DirectoryCell:
				case Grid_DwgFilesCell:		
				case Grid_EllipsesButtons:
				case Grid_PickButtons:
				case Grid_SwitchableIcons:
					mbShowHighlight = false;
					break;
			}
		}
		// if the entire column is selected.
		if (mCurrentCell.col() == -1)
			mbShowHighlight = true;
		
		if (mCurrentCell.row() == lvi.iItem &&
			mCurrentCell.col() == nColumn &&
			mbShowHighlight)
		{
			backGround = ::GetSysColor(COLOR_HIGHLIGHT);
			foreGround = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
			bHighlighted = true;
		}
		else if (mCurrentCell.row() == lvi.iItem &&
			     mCurrentCell.col() == -1 &&
				 mbShowHighlight)
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

		
		if (mbHasGridLines)
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
*/
}

void CArxGridCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( GetFocus() != this) 
		SetFocus();
	HideEditControls();
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CArxGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( GetFocus() != this) 
		SetFocus();
	HideEditControls();
	__super::OnVScroll(nSBCode, nPos, pScrollBar);
}
