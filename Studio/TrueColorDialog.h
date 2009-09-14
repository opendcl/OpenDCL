#pragma once


// CTrueColorDialog

class CTrueColorDialog : public CColorDialog
{
	DECLARE_DYNAMIC(CTrueColorDialog)
	bool mbMustDeleteCustomColors;

public:
	CTrueColorDialog(COLORREF clrInit = 0, DWORD dwFlags = 0, CWnd* pParentWnd = NULL);
	virtual ~CTrueColorDialog();

protected:
	DECLARE_MESSAGE_MAP()
};


