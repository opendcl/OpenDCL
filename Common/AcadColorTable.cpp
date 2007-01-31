#include "StdAfx.h"
#include "AcadColorTable.h"


COLORREF GetRGBColor(long nColorIndex)
{
	if (nColorIndex == -22 )
	{
		return RGB(0,0,0);
	}
	if (nColorIndex == -23)
	{
		return RGB(255,255,255);
	}
	else if (nColorIndex > 256)
	{
		return (COLORREF)nColorIndex;
	}	
	else if (nColorIndex >= 0 && nColorIndex <= 255)
	{
		return RGB(	acadcol[nColorIndex][0],
					acadcol[nColorIndex][1],
					acadcol[nColorIndex][2]);
	}
	else if (nColorIndex > 255)
	{
		return nColorIndex;
	}
	
	int nSystemColor = (nColorIndex * -1) - 1; 
	return GetSysColor(nSystemColor);
	
}
