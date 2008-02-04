// Methods_BlockView.h : header file
//

#pragma once


namespace BlockView
{
ADSRESULT Clear();
ADSRESULT SetHighLight();
ADSRESULT RemoveHighLight();
ADSRESULT Zoom();
ADSRESULT DisplayBlock();
ADSRESULT DisplayBlockToScale();
ADSRESULT LoadDwg();
ADSRESULT	LoadDwgToScale();
ADSRESULT PreLoadDwg();
ADSRESULT GetBlockList();
ADSRESULT GetBlockSize();	
ADSRESULT GetDwgSize();	
ADSRESULT DisplayPaperSpace();
ADSRESULT RefreshBlock();
//ADSRESULT	GetViewInfo();
//ADSRESULT SetView();
};
