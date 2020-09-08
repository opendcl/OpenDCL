// CAcUiColorComboBox implementation

#include "StdAfx.h"
#include "MFC/CAcUi/CAcUiColorComboBox.h"



//BEGIN_MESSAGE_MAP(CAcUiColorComboBox, CAcUiMRUComboBox)
//END_MESSAGE_MAP()
//
//CAcUiColorComboBox::CAcUiColorComboBox()
//: m_blockColorIndex( -1 )
//{
//}
//
//CAcUiColorComboBox::~CAcUiColorComboBox()
//{
//}
//
//int CAcUiColorComboBox::AddColorToMRU(int)
//{
//	return -1;
//}
//
//int CAcUiColorComboBox::FindItemByColorIndex(int)
//{
//	return -1;
//}
//
//int CAcUiColorComboBox::GetBlockColorIndex()
//{
//	return m_blockColorIndex;
//}
//
//int CAcUiColorComboBox::GetCurrentItemColorIndex()
//{
//	return -1;
//}
//
//int CAcUiColorComboBox::GetItemColorIndex(int)
//{
//	return -1;
//}

COLORREF CAcUiColorComboBox::GetColorFromIndex(int)
{
	return RGB(0,0,0);
}

COLORREF CAcUiColorComboBox::LookupColor(int,LOGPALETTE*)
{
	return RGB(0,0,0);
}

int CAcUiColorComboBox::GetColorIndex(COLORREF)
{
	return -1;
}
//
//int CAcUiColorComboBox::GetCurrentLayerColorIndex()
//{
//	return -1;
//}

void CAcUiColorComboBox::SetBlockColorIndex(int nIdx)
{
	m_blockColorIndex = nIdx;
}
//
//BOOL CAcUiColorComboBox::GetOtherColorIndex(int,int,BOOL,int&)
//{
//	return FALSE;
//}
//
//BOOL CAcUiColorComboBox::GetOtherName(BOOL,CString&)
//{
//	return FALSE;
//}
//
//BOOL CAcUiColorComboBox::GetWindowsColor(COLORREF&)
//{
//	return FALSE;
//}
//
//BOOL CAcUiColorComboBox::OnSelectOther(BOOL,int,int&)
//{
//	return FALSE;
//}
//
//void CAcUiColorComboBox::DrawItemImage(CDC&,CRect&,int)
//{
//}
//
//void CAcUiColorComboBox::OnAddItems()
//{
//}
//
//void CAcUiColorComboBox::RGBFill(CDC&,CRect&)
//{
//}
