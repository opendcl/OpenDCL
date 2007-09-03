// Methods_ListCtrl.h : header file
//

#pragma once


int Grid_SelCurRow();
int Grid_SetCurRow();
int Grid_AddString();
int Grid_InsertRow();
int Grid_InsertString();
int Grid_Cell_StartItemEdit();
int Grid_Cell_SetStyle();
int Grid_Cell_SetDropList();
int Grid_GetCheck();
int Grid_SetCheck();
int Grid_HitPointTest();
int Grid_GetColumnImage();
int Grid_SetColumnImage();
int Grid_GetCount();
int Grid_GetColumnCount();

int Grid_AddColumn();
int Grid_AddRow();
int Grid_CalcColumnWidth();
int Grid_Clear();
int Grid_DeleteColumns();
int Grid_DeleteItems();
int Grid_FillGrid();
int Grid_GetColWidth();
int Grid_GetItemData();
int Grid_GetItemImage();
int Grid_GetItemText();
int Grid_GetSelectedCell();
int Grid_InsertRow();
int Grid_ApplyNewRow(bool bLookForInsertIndex, CString sMethodName);
int Grid_SetColWidth();
int Grid_SelCurCell();
int Grid_SetItemData();
int Grid_SetItemText();
int Grid_SetItemImage();
int Grid_CancelLabelEdit();
int Grid_SortColTextItems();
int Grid_SortColNumericItems();
int Grid_GetRow();
int Grid_GetColumn();
int Grid_LoadDwg();
int Grid_Reset();
int Grid_GetFileName();
