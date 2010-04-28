// ArxLinetypeComboBoxCtrl.h : header file
//

#pragma once

#if (_ACADTARGET < 17)
//The new AcUiLinetypeComboBox was not introduced until AutoCAD 2006, so we're just
//using a plain text combo with a linetype handler in older versions of AutoCAD
#include "ArxR16LinetypeComboBoxCtrl.h"
#else //(_ACADTARGET < 17)

#include "DialogControl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxLinetypeComboBoxCtrl window

class CArxLinetypeComboBoxCtrl : public CAcUiLineTypeComboBox, public CDialogControl
{
	CArxControlServices	mArxServices;

public:
	CArxLinetypeComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxLinetypeComboBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CRect GetWndRect() const;
	virtual DWORD GetWndStyle() const;
	virtual bool ApplyProperty( TPropertyPtr pProp );

public:

protected:
	DECLARE_MESSAGE_MAP();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
};

#endif //(_ACADTARGET < 17)
