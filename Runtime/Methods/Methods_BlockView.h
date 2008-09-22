// Methods_BlockView.h : header file
//

#pragma once


namespace BlockView
{
ADSRESULT Clear();
ADSRESULT SetHighlight();
ADSRESULT RemoveHighlight();
ADSRESULT Zoom();
ADSRESULT DisplayBlock();
ADSRESULT DisplayBlockToScale();
ADSRESULT DisplayDwg();
ADSRESULT	DisplayDwgToScale();
ADSRESULT PreLoadDwg();
ADSRESULT GetBlockList();
ADSRESULT GetBlockSize();	
ADSRESULT GetDwgSize();	
ADSRESULT DisplayPaperSpace();
ADSRESULT RefreshBlock();
//ADSRESULT	GetViewInfo();
//ADSRESULT SetView();
};
