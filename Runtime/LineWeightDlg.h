// LineWeightDlg.h : header file
//

#pragma once


//display the AutoCAD line weight dialog and return the selected line weight
//lw should be set to the desired default when calling the function
//if the function succeeds, the selected line weight is return in lw
//if bIncludeMetaTypes is true, the dialog includes ByLayer/ByBlock lineweights
//if pParent is not null, it is disabled while the line weight dialog is open
//returns true if successful, false on failure
bool SelectLineWeightUI( /*in-out*/ AcDb::LineWeight& lw,
												 /*in*/ bool bIncludeMetaTypes = true,
												 /*in*/ CWnd* pParent = NULL );