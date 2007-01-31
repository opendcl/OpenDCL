// Methods_ListCtrl.h : header file
//

#pragma once


void ReturnRowCol(int nRow, int nCol);

int ListCtrl_AddString();

int ListCtrl_GetColumnImage();
int ListCtrl_SetColumnImage();
int ListCtrl_GetCount();
int ListCtrl_GetColumnCount();

int ListCtrl_AddColumn();
int ListCtrl_AddRow();
int ListCtrl_Arrange();
int ListCtrl_CalcColumnWidth();
int ListCtrl_Clear();
int ListCtrl_CountItems();
int ListCtrl_DeleteColumns();
int ListCtrl_DeleteItems();
int ListCtrl_FillGrid();
int ListCtrl_GetColWidth();
int ListCtrl_GetItemData();
int ListCtrl_GetItemImage();
int ListCtrl_GetItemText();
int ListCtrl_GetSelectedCount();
int ListCtrl_GetSelectedItems();
int ListCtrl_GetSelectedNths();
int ListCtrl_InsertItem();										
int ListCtrl_InsertRow();
int ListCtrl_ApplyNewRow(bool bLookForInsertIndex, CString sMethodName);
int ListCtrl_SetColWidth();
int ListCtrl_SetCurSel();
int ListCtrl_SetItemData();
int ListCtrl_SetItemText();
int ListCtrl_SetItemImage();
int ListCtrl_StartLabelEdit();
int ListCtrl_CancelLabelEdit();
int ListCtrl_SortColTextItems();
int ListCtrl_SortColNumericItems();
int ListCtrl_GetRow();
int ListCtrl_GetColumn();
int ListCtrl_LoadDwg();
int ListCtrl_Reset();
int ListCtrl_GetFileName();
int ListCtrl_HitPointTest();
