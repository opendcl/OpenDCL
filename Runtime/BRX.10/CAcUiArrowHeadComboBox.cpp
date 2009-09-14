// CAcUiArrowHeadComboBox implementation

#include "StdAfx.h"
#include "MFC/CAcUi/CAcUiArrowHeadComboBox.h"


BEGIN_MESSAGE_MAP(CAcUiArrowHeadComboBox, CAcUiMRUComboBox)
END_MESSAGE_MAP()

CAcUiArrowHeadComboBox::CAcUiArrowHeadComboBox()
{
}

CAcUiArrowHeadComboBox::~CAcUiArrowHeadComboBox()
{
}

BOOL CAcUiArrowHeadComboBox::GetUseOrigin2()
{
	return FALSE;
}

int CAcUiArrowHeadComboBox::AddArrowHeadToMRU(LPCTSTR)
{
	return -1;
}

void CAcUiArrowHeadComboBox::SetUseOrigin2(BOOL)
{
}

BOOL CAcUiArrowHeadComboBox::GetOtherName(BOOL,CString&)
{
	return FALSE;
}

int CAcUiArrowHeadComboBox::CalcItemHeight()
{
	return 16;
}

void CAcUiArrowHeadComboBox::DrawItemImage(CDC&,CRect&,int)
{
}

void CAcUiArrowHeadComboBox::OnAddItems()
{
}

void CAcUiArrowHeadComboBox::OnComboBoxInit()
{
}
