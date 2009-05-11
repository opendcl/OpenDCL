// PictureBoxCtrl.h : header file
//

#pragma once

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

public:
	CPictureBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CPictureBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CAcadColorService* GetColorService() { return CPictureBox::GetColorService(); }
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual bool OnApplyCaption( TPropertyPtr pProp ) { return true; }
	virtual bool OnApplyBackgroundColor( TPropertyPtr pProp );

public:
	virtual bool IsAutoSized();
	virtual void AutoSize();
	void SetPictureID(int nPictureID);
	void Clear();
	void PaintPicture(int sX, int sY, int nPictureID, int nEnabled, int nUseMask);

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};
