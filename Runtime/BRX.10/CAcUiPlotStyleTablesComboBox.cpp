// CAcUiPlotStyleTablesComboBox implementation

#include "StdAfx.h"
#include "MFC/CAcUi/CAcUiPlotStyleTablesComboBox.h"


BEGIN_MESSAGE_MAP(CAcUiPlotStyleTablesComboBox, CAcUiMRUComboBox)
	//ON_CONTROL_REFLECT(CBN_DROPDOWN, &CAcUiPlotStyleTablesComboBox::OnDropDown)
END_MESSAGE_MAP()

CAcUiPlotStyleTablesComboBox::CAcUiPlotStyleTablesComboBox()
{
}

CAcUiPlotStyleTablesComboBox::CAcUiPlotStyleTablesComboBox(BOOL b)
{
}

CAcUiPlotStyleTablesComboBox::~CAcUiPlotStyleTablesComboBox()
{
}

//bool CAcUiPlotStyleTablesComboBox::IsMissing(int)
//{
//	return true;
//}
//
//int CAcUiPlotStyleTablesComboBox::AddItemToList(LPCTSTR,int)
//{
//	return -1;
//}
//
//int CAcUiPlotStyleTablesComboBox::AddMissingStyleTable(LPCTSTR)
//{
//	return -1;
//}

int CAcUiPlotStyleTablesComboBox::ImageWidth()
{
	return 0;
}

void CAcUiPlotStyleTablesComboBox::GetLBText(int,CString&) const
{
}

//void CAcUiPlotStyleTablesComboBox::SetFileType(bool,bool)
//{
//}

afx_msg BOOL CAcUiPlotStyleTablesComboBox::OnDropDown()
{
	return TRUE;
}

int CAcUiPlotStyleTablesComboBox::CalcItemHeight()
{
	return 16;
}

void CAcUiPlotStyleTablesComboBox::DrawItemImage(CDC&,CRect&,int)
{
}

void CAcUiPlotStyleTablesComboBox::OnAddItems()
{
}

void CAcUiPlotStyleTablesComboBox::OnComboBoxInit()
{
}

