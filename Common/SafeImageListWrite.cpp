#include "StdAfx.h"
#include "SafeImageListWrite.h"

#if (_WIN32_WINNT < 0x0501)
#define ILP_NORMAL          0           // Writes or reads the stream using new sematics for this version of comctl32
#define ILP_DOWNLEVEL       1           // Write or reads the stream using downlevel sematics.
#endif


typedef HRESULT (WINAPI *F_ImageList_WriteEx)(HIMAGELIST himl, DWORD dwFlags, LPSTREAM pstm);
static F_ImageList_WriteEx GetImageList_WriteEx()
{
	static F_ImageList_WriteEx pfImageList_WriteEx =
		(F_ImageList_WriteEx)GetProcAddress( GetModuleHandle( _T("comctl32.dll") ), "ImageList_WriteEx" );
	return pfImageList_WriteEx;
}

typedef BOOL (WINAPI *F_ImageList_Write)(HIMAGELIST himl, LPSTREAM pstm);
static F_ImageList_Write GetImageList_Write()
{
	static F_ImageList_Write pfImageList_Write =
		(F_ImageList_Write)GetProcAddress( GetModuleHandle(  _T("comctl32.dll") ), "ImageList_Write" );
	return pfImageList_Write;
}

BOOL SafeImageListWrite(HIMAGELIST himl, LPSTREAM pstm)
{
	F_ImageList_WriteEx pfImageList_WriteEx = GetImageList_WriteEx();
	if( pfImageList_WriteEx )
		return (S_OK == pfImageList_WriteEx( himl, ILP_DOWNLEVEL, pstm ));
	F_ImageList_Write pfImageList_Write = GetImageList_Write();
	if( pfImageList_Write )
		return pfImageList_Write( himl, pstm );
	return FALSE;
}