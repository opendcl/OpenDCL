// OpenDCL.h : main header file for the OpenDCL application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "FontSettings.h"

class CMDIChildWnd;


// COpenDCLApp:
// See OpenDCL.cpp for the implementation of this class
//

class COpenDCLApp : public CWinApp
{

public:
	COpenDCLApp();

// Attributes
	FontSettings GetDefaultFontSettings() const;
	void SetDefaultFontSettings( const FontSettings& FS, UINT flags = fontAll );
	UINT GetGridSpacing() const;
	void SetGridSpacing( UINT nGridSpacing );

protected:

// Overrides
protected:
	virtual BOOL InitInstance();

// Implementation
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppAbout();
	afx_msg BOOL OnOpenRecentFile(UINT nID);
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileClose();
	afx_msg void OnHelp();
	afx_msg void OnHelpFinder();
	afx_msg void OnToolsDefaultfont();
	afx_msg void OnToolsGridspacing();
	afx_msg void OnToolsEventscopytoclipboard();
	afx_msg void OnUpdateToolsEventscopytoclipboard(CCmdUI* pCmdUI);
	afx_msg void OnToolsEventswritetolispfile();
	afx_msg void OnUpdateToolsEventswritetolispfile(CCmdUI* pCmdUI);
};

extern COpenDCLApp theApp;