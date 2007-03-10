//--------------------------------------------------------------------------------------/
//	Header File

//	Class:			CCurrencyEdit			
//	Base Classe:	CNumEdit

//	File:			CurrencyEdit.h		
//	Revision:		A						
//	Date:			17 August 2000
//  Designed:		Ian J Hart	
//--------------------------------------------------------------------------------------/

#pragma once

#include "NumEdit.h"


class CCurrencyEdit : public CNumEdit  
{
public:
// 1 - Construction
	CCurrencyEdit();

	CString m_strOldValue;
	
// 2 - Destruction 
	virtual ~CCurrencyEdit();

protected:
// 3 - Message Map Functions
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	//{{AFX_MSG(CNumEdit)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetfocus();
	afx_msg void OnKillfocus();	
	afx_msg void OnChange();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

//	4 - Class Member Variables
private:
	bool	m_bEdit_Has_Focus;
	CString m_strCurrency;
	TCHAR	m_chCurrency_Symbol;


//	5 - Class Member Functions
private:

	void Update();
	CString Number_To_Currency(double fNumber);


//	6 - Class Creation/Initialisation

public:

	void Init	(double	fNumber			  = 0.00,
				 int	dPrecision		  = 2,
				 bool	bNeg_Values		  = true,
				 TCHAR	chCurrency_Symbol ='$');

	void Set_Data(double	fNumber,	
				  int		dPrecision,
				  bool		bNeg_Values,
				  TCHAR		chCurrency_Symbol);
	

// 7 - Interface

public:
	
	CString Get_Currency_Text();

	double	Get_Number();
	void	Set_Number(double fNumber);

	TCHAR	Get_Currency_Symbol();
	void	Set_Currency_Symbol(TCHAR chCurrency_Symbol);
};
