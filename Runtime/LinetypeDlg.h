// LineWeightDlg.h : header file
//

#pragma once


//display the AutoCAD linetype dialog and return the selected linetype
//idLinetype should be set to the desired default when calling the function
//if the function succeeds, the selected linetype is return in idLinetype and its name in sLinetype
//if pParent is not null, it is disabled while the linetype dialog is open
//returns true if successful, false on failure
bool SelectLinetypeUI( /*in-out*/ AcDbObjectId& idLinetype,
											 /*out*/CString& sLinetype,
											 /*in*/ bool bIncludeMetaTypes = true,
											 /*in*/ CWnd* pParent = NULL );