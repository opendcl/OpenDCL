// CAcUiLineTypeComboBox implementation

#include "StdAfx.h"
#include "MFC/CAcUi/CAcUiLineTypeComboBox.h"



BEGIN_MESSAGE_MAP(CAcUiLineTypeComboBox, CAcUiMRUComboBox)
END_MESSAGE_MAP()

CAcUiLineTypeComboBox::CAcUiLineTypeComboBox()
: m_bDbReload( false )
, m_pLTLocalMapDB( NULL )
{
}

CAcUiLineTypeComboBox::~CAcUiLineTypeComboBox()
{
}

//AcDbObjectId CAcUiLineTypeComboBox::getOIDCurSel()
//{
//	return AcDbObjectId();
//}
//
//AcDbObjectId CAcUiLineTypeComboBox::getOIDSel(int)
//{
//	return AcDbObjectId();
//}
//
//bool CAcUiLineTypeComboBox::getDbReload()
//{
//	return m_bDbReload;
//}
//
//bool CAcUiLineTypeComboBox::isOtherSelected()
//{
//	return false;
//}
//
//int CAcUiLineTypeComboBox::getLastSelection()
//{
//	return -1;
//}

void CAcUiLineTypeComboBox::DrawItem(LPDRAWITEMSTRUCT)
{
}

void CAcUiLineTypeComboBox::OnComboBoxInit()
{
}

//void CAcUiLineTypeComboBox::AddItems()
//{
//}
//
//void CAcUiLineTypeComboBox::emptyLTypeLocalList()
//{
//}
//
//void CAcUiLineTypeComboBox::forceSelectOther(BOOL)
//{
//}
//
//void CAcUiLineTypeComboBox::setCurSelByOID(const AcDbObjectId&)
//{
//}
//
//void CAcUiLineTypeComboBox::setDbReload(bool)
//{
//}
//
//AcDbDatabase* CAcUiLineTypeComboBox::getLTLocalMapDB()
//{
//	return m_pLTLocalMapDB;
//}
//
//CAcUiLTypeRecord* CAcUiLineTypeComboBox::CreateLTRecord(CString&,AcDbObjectId&)
//{
//	return NULL;
//}
//
//int CAcUiLineTypeComboBox::AddLTypeToControl(CAcUiLTypeRecord*)
//{
//	return -1;
//}
//
//int CAcUiLineTypeComboBox::FindItemByCargo(AcDbObjectId)
//{
//	return -1;
//}
//
//int CAcUiLineTypeComboBox::GetItemCargo(int)
//{
//	return -1;
//}

BOOL CAcUiLineTypeComboBox::OnSelectOther(BOOL,int,int&)
{
	return FALSE;
}

int CAcUiLineTypeComboBox::ImageWidth()
{
	return 0;
}

void CAcUiLineTypeComboBox::DrawItemImage(CDC&,CRect&,int)
{
}

void CAcUiLineTypeComboBox::OnAddItems()
{
}

void CAcUiLineTypeComboBox::LoadContentsFromDatabase()
{
}

//void CAcUiLineTypeComboBox::setLTLocalMapDB(AcDbDatabase*)
//{
//}
//
//void CAcUiLineTypeComboBox::ShowLTypes(bool /*= false*/)
//{
//}
