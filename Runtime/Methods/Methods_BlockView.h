#if !defined(AFX_METHODS_BLOCKVIEW_H__B0E11141_557D_11D4_BB44_20020DC10000__INCLUDED_)
#define AFX_METHODS_BLOCKVIEW_H__B0E11141_557D_11D4_BB44_20020DC10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Methods_BlockView.h : header file
//
int BlockView_Clear();
int BlockView_SetHighLight();
int BlockView_RemoveHighLight();
int BlockView_Zoom();
int BlockView_ViewBlock();
int BlockView_ViewBlockToScale();
int BlockView_LoadDwg();
int	BlockView_LoadDwgToScale();
int BlockView_PreLoadDwg();
int BlockView_GetBlockList();
int BlockView_GetBlockSize();	
int BlockView_GetDwgSize();	
int BlockView_ViewPaperSpace();
int BlockView_RefreshBlock();
int	BlockView_GetViewInfo();
int BlockView_SetView();

void BlockView_LoadDwgAndDisplay(int nScaleType, CString sDefunName);

#endif // !defined(AFX_METHODS_BLOCKVIEW_H__B0E11141_557D_11D4_BB44_20020DC10000__INCLUDED_)
