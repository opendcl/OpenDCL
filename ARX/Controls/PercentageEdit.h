//--------------------------------------------------------------------------------------/
//	Header File

//	Class:			CPercentageEdit			
//	Base Class:		CNumEdit

//	File:			PercentageEdit.h		

//	Revision:		A						
//	Date:			17 August 2000
//  Designed;		Ian J Hart	
//--------------------------------------------------------------------------------------/

#pragma once

#include "NumEdit.h"
#include "PPToolTip.h"


//--------------------------------------------------------------------------------------/
class CPercentageEdit : public CNumEdit  
{
public:
// 1 - Construction - /////////////////////////////////////////////////

	CPercentageEdit();

// 2 - Destruction - /////////////////////////////////////////////////

	virtual ~CPercentageEdit();

	CString m_strOldValue;

protected:
// 3 - Message Map Functions - /////////////////////////////////////////
	virtual BOOL PreTranslateMessage(MSG* pMsg);
		
	//{{AFX_MSG(CNumEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetfocus();
	afx_msg void OnKillfocus();	
	afx_msg void OnChange();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

//	4 - Class Member Variables - ///////////////////////////////////////
private:

	CString m_strPercentage;
	TCHAR	m_chPercentage_Symbol;

//	5 - Class Member Functions - ///////////////////////////////////////
private:
	void Update();

	CString Number_To_Percentage(double fNumber);


//	6 - Class Creation/Initialisation - /////////////////////////////////

public:

	void Init ( double	fNumber			  = 0.00,
				int		dPrecision		  = 2,
				bool	bNeg_Values		  = true);

	void Set_Data(double fNumber,	
				  int	 dPrecision,
				  bool	 bNeg_Values);
								 
// 7 - Interface - ///////////////////////////////////////////////////////

public:

	CString Get_Percentage_Text();

	double	Get_Number();
	void	Set_Number(double fNumber);
};
