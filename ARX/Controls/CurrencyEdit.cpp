//--------------------------------------------------------------------------------------/
//	Implementation File

//	Class:			CCurrencyEdit			
//	Base Class:		CNumEdit

//	File:			CurrencyEdit.cpp		
//	Revision:		A						
//	Date:			17 August 2000
//  Designed:		Ian J Hart	
//--------------------------------------------------------------------------------------/

#include "stdafx.h"
#include "CurrencyEdit.h"
#include "SpreadSheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C/

CCurrencyEdit::CCurrencyEdit()
	          : CNumEdit()		 	
{
	m_bEdit_Has_Focus=false;
	m_chCurrency_Symbol = _T('$');
}
//D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D/

CCurrencyEdit::~CCurrencyEdit()
{
	
}
//M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M/

BEGIN_MESSAGE_MAP(CCurrencyEdit, CNumEdit)
	ON_WM_LBUTTONUP()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)	
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()
//--------------------------------------------------------------------------------------/

void CCurrencyEdit::Init(double	fNumber,	
						 int	dPrecision,
						 bool	bNeg_Values,
						 TCHAR	chCurrency_Symbol)
{
	CEdit::OnKillFocus(AfxGetMainWnd());

// 1 - Initialise the controls data

	Set_Data(fNumber,dPrecision,bNeg_Values,chCurrency_Symbol);

// 2 - Call SetFocus to force the control to update
	SetFocus();
}
//--------------------------------------------------------------------------------------/

void CCurrencyEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CSpreadSheet *pListCtrl = (CSpreadSheet*)GetParent();
		
	if (nChar == VK_UP)
		pListCtrl->MoveUp();		
	
	if (nChar == VK_DOWN)
		pListCtrl->MoveDown();				

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CCurrencyEdit::Set_Data(double	fNumber,	
							 int	dPrecision,
							 bool	bNegValues,
							 TCHAR	chCurrency_Symbol)
{
	m_fNumber			= fNumber;
	m_dPrecision		= dPrecision;
	m_bNegValues		= bNegValues;	
	m_chCurrency_Symbol = chCurrency_Symbol;

	m_strText			= CNumEdit::Number_To_Text(m_fNumber);
}
//--------------------------------------------------------------------------------------/

void CCurrencyEdit::OnSetfocus() 
{	
// 1- If the edit style is read only ensure currency format
//	  maintained otherwise show the unformatted text
	CEdit::SetFocus();	

	if(GetStyle() & ES_READONLY)
	{
		Update();
	}
	else
	{
		CNumEdit::Set_EditCtrl_Text(m_strText);	
	}

}
//--------------------------------------------------------------------------------------/

void CCurrencyEdit::OnKillfocus() 
{
// 1- If the edit style is read only return otherwise show the
//	  formatted text
	
	if(GetStyle() & ES_READONLY)
		return;

	CNumEdit::Get_EditCtrl_Text();
	Update();
	m_bEdit_Has_Focus=false;
	CEdit::OnKillFocus(AfxGetMainWnd());

}
//--------------------------------------------------------------------------------------/

void CCurrencyEdit::Update()
{
	m_strText		=	CNumEdit::Number_To_Text(m_fNumber);
	m_strCurrency	=	Number_To_Currency(m_fNumber);
	CNumEdit::Set_EditCtrl_Text(m_strCurrency);
}
//--------------------------------------------------------------------------------------/

CString CCurrencyEdit::Number_To_Currency(double fNumber)
{
// 1 - Variables

	CString     strCurrency =   "";			//* Temporary Currency string
	int			nEndPos		=	0;			//* Position of last character prior to decimal 										
	int			nCommaPos	=	0;			//* Position of comma symbol 
	bool		bIsNegative=	false;		//* Sign of number	

// 2 - Temporary string to manipulate into the Currency string,

	strCurrency = CNumEdit::Number_To_Text(fNumber);

// 3 - Determine the sign +/-
	if(strCurrency.Find('-') != -1)
	{
		bIsNegative = true;
	}

// 4 - Determine the position of the decimal place
//	   e.g a) 1234.0	nEndPos=4
//		   b) 1234      nEndPos=4							
	if(strCurrency.Find('.') != -1)
	{
		nEndPos  = strCurrency.Find('.');  
	}
	else
	{
		nEndPos  = strCurrency.GetLength();
	}

// 5 - If the number has greater then or equal to 4 characters 
//     format with commas
//	   e.g a) unformatted 12345  formatted 1,234

	if(nEndPos   >= (4 + bIsNegative))
	{
		nCommaPos = nEndPos; 
		do
		{
			nCommaPos = nCommaPos - 3;
			strCurrency.Insert(nCommaPos,',');
		}
		while(nCommaPos >= (4+bIsNegative));
	}

// 6 - Insert the currency symbol after any negative sign 
		strCurrency.Insert(bIsNegative,m_chCurrency_Symbol);
	
	return strCurrency;
}
//--------------------------------------------------------------------------------------/

void CCurrencyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE)
		return;

// 1 - Check validity of character typed
	if(Validate_Char(nChar)!=true)
	{
		MessageBeep(0);
		return;
	}
	
// 2 - Standard OnChar processeing 
	CEdit::OnChar(nChar,nRepCnt,nFlags);
}
//--------------------------------------------------------------------------------------/
BOOL CCurrencyEdit::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message== WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
	    {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			CSpreadSheet *pListCtrl = (CSpreadSheet*)GetParent();		
			pListCtrl->MoveDown();		
			return TRUE;				// DO NOT process further
		}
		if (pMsg->wParam == VK_ESCAPE)
	    {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			SetWindowText(m_strOldValue);
			
			// Send Notification to parent of ListView ctrl
			CSpreadSheet *pListCtrl = (CSpreadSheet*)GetParent();

			pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, m_strOldValue);
			
			pListCtrl->HideEditControls();
			return TRUE;				// DO NOT process further
		}
		
	}

	return CEdit::PreTranslateMessage(pMsg);
}

void CCurrencyEdit::OnChange() 
{
		
	CString sText;	
	GetWindowText(sText);


	// Send Notification to parent of ListView ctrl
	CSpreadSheet *pListCtrl = (CSpreadSheet*)GetParent();

	//pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, GetCurrentItemColorIndex());
	pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sText);

}

CString CCurrencyEdit::Get_Currency_Text()
{
	return m_strCurrency;
}
// Note the class only accepts numbers as input so in order to set 
// a currency values call Set_Number() first
//--------------------------------------------------------------------------------------/

double CCurrencyEdit::Get_Number()
{
	return CNumEdit::Get_Number();
}
//--------------------------------------------------------------------------------------/

void CCurrencyEdit::Set_Number(double fNumber)
{
	m_fNumber   =	fNumber;
	Update();
}

//--------------------------------------------------------------------------------------/

TCHAR CCurrencyEdit::Get_Currency_Symbol()
{
// 1 - Get the currency symbol
	return m_chCurrency_Symbol;
}
//--------------------------------------------------------------------------------------/

void CCurrencyEdit::Set_Currency_Symbol(TCHAR chCurrency_Symbol)
{
// 1 - Set the currency symbol
	m_chCurrency_Symbol=chCurrency_Symbol;
	Update();
}
//--------------------------------------------------------------------------------------/
void CCurrencyEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
// 1 - One click to get focus and one click  to select
//	   (Simplistic - room for improvement)
//	   Used to get around the problem of the cusor selection
//     moving around when the edit text changes between the formatted
//	   and unformatted text. eg 
//
//		$110,233,*888.00   to      110233888*.00
//
//     where * indicates cursor selection

	if(GetStyle() & ES_READONLY)
	{
		CEdit::OnLButtonUp(nFlags, point);
	}
	else
	{
		if(m_bEdit_Has_Focus!=true)
			CEdit::SetSel(0,0);

		m_bEdit_Has_Focus=true;
		CEdit::OnLButtonUp(nFlags, point);
	}	
}

