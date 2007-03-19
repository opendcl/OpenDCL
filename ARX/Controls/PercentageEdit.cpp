//--------------------------------------------------------------------------------------/
//	Implementation File

//	Class:			CPercentageEdit			
//	Base Classe:	CNumEdit

//	File:			PercentageEdit.cpp		
//	Revision:		A						
//	Date:			17 August 2000
//  Designed:		Ian J Hart	
//--------------------------------------------------------------------------------------/

#include "stdafx.h"
#include "PercentageEdit.h"
#include "ArxGridCtrl.h"


//C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C_C/

CPercentageEdit::CPercentageEdit()
	          : CNumEdit()		 	
{
}
//D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D/

CPercentageEdit::~CPercentageEdit()
{

}
//M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M_M/

BEGIN_MESSAGE_MAP(CPercentageEdit, CNumEdit)
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)	
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()
//--------------------------------------------------------------------------------------/


void CPercentageEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent();
		
	if (nChar == VK_UP)
		pListCtrl->MoveUp();		
	
	if (nChar == VK_DOWN)
		pListCtrl->MoveDown();				

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPercentageEdit::Init(double	fNumber,	
						   int	dPrecision,
						   bool	bNeg_Values)
{

	CEdit::OnKillFocus(AfxGetMainWnd());
// 1 - Initialise the controls data


	Set_Data(fNumber,dPrecision,bNeg_Values);

// 3 - Call SetFocus to force the control to update

	SetFocus();
}
//--------------------------------------------------------------------------------------/

void CPercentageEdit::Set_Data(double	fNumber,	
							   int		dPrecision,
							   bool		bNegValues)
{
	m_fNumber		= fNumber;
	m_dPrecision	= dPrecision;
	m_bNegValues	= bNegValues;	

	m_strText		= CNumEdit::Number_To_Text(m_fNumber);
}
//--------------------------------------------------------------------------------------/

void CPercentageEdit::OnSetfocus() 
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

void CPercentageEdit::OnKillfocus() 
{
// 1- If the edit style is read only return otherwise show the
//	  formatted text
	
	
	if(GetStyle() & ES_READONLY)
		return;

	CNumEdit::Get_EditCtrl_Text();
	Update();
	CEdit::OnKillFocus(AfxGetMainWnd());
}
//--------------------------------------------------------------------------------------/

void CPercentageEdit::Update()
{
	m_strText		=	CNumEdit::Number_To_Text(m_fNumber);
	m_strPercentage	=	Number_To_Percentage(m_fNumber);
	CNumEdit::Set_EditCtrl_Text(m_strPercentage);
}
//--------------------------------------------------------------------------------------/

CString CPercentageEdit::Number_To_Percentage(double fNumber)
{
// 1 - Temporary string to manipulate into the Percentage string,


	CString     strPercentage;

	strPercentage = CNumEdit::Number_To_Text(fNumber);

// 2 - Insert the percentage symbol  

	int dLength = strPercentage.GetLength();
	strPercentage.Insert(dLength,_T('%'));

	
	return strPercentage;
}


void CPercentageEdit::OnChange() 
{
		
	CString sText;	
	GetWindowText(sText);


	// Send Notification to parent of ListView ctrl
	CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent();
	//pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, GetCurrentItemColorIndex());
	pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sText);

}
//--------------------------------------------------------------------------------------/

void CPercentageEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
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
BOOL CPercentageEdit::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message== WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
	    {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent();		
			pListCtrl->MoveDown();		
			return TRUE;				// DO NOT process further
		}
		if (pMsg->wParam == VK_ESCAPE)
	    {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			SetWindowText(m_strOldValue);
			
			// Send Notification to parent of ListView ctrl
			CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent();

			pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, m_strOldValue);
			
			pListCtrl->HideEditControls();
			return TRUE;				// DO NOT process further
		}
		
	}

	return CEdit::PreTranslateMessage(pMsg);
}

//--------------------------------------------------------------------------------------/

CString CPercentageEdit::Get_Percentage_Text()
{
	return m_strPercentage;
}
// Note the class only accepts numbers as input so in order to set 
// a percentage values call Set_Number()
//--------------------------------------------------------------------------------------/

double CPercentageEdit::Get_Number()
{
	return CNumEdit::Get_Number();
}
//--------------------------------------------------------------------------------------/

void CPercentageEdit::Set_Number(double fNumber)
{
	m_fNumber   =	fNumber;
	Update();
}
//--------------------------------------------------------------------------------------/

