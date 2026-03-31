// PictureBoxCtrl.h : header file
//

#pragma once

#include "AcadColorService.h"
#include "DialogControl.h"
#include "PictureBox.h"

#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CPictureBoxCtrl window

class CPictureBoxCtrl : public CPictureBox, public CDialogControl
{
	bool mbButtonLike;
	CAcadColorService mColorService;

public:
	CPictureBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CPictureBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	CAcadColorService* GetColorService() override { return &mColorService; }
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	bool OnApplyCaption( TPropertyPtr pProp ) override { return true; }
	bool OnApplyBackgroundColor( TPropertyPtr pProp ) override;

public:
	virtual bool IsAutoSized();
	virtual void AutoSize();
	void SetPictureID(int nPictureID);
	void Clear();
	void PaintPicture(int sX, int sY, int nPictureID, int nEnabled, int nUseMask);

protected:
	DECLARE_MESSAGE_MAP()

	void PostNcDestroy() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
