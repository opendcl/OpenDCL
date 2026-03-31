// ArxDwgPreviewCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"
#include "AcadBlockInsertDropTarget.h"


/////////////////////////////////////////////////////////////////////////////
// CArxDwgPreviewCtrl window

class CArxDwgPreviewCtrl : public CButton, public CDialogControl
{
	CAcadColorService mColorService;
	CArxControlServices	mArxServices;
	CAcadBlockInsertDropTarget mBlockInsertDropTarget;
	CArxDragDropService mDragDropService;
	CString msDwgFilename;

	HBITMAP mhbmLast;
	HBITMAP mhbmSaved;
	COLORREF mclrHighlight;

	//bool mbDrawSelected;
	//COLORREF mclrHighlight;
	//CRect mrcFocus;

public:
	CArxDwgPreviewCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxDwgPreviewCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	CDragDropService* GetDragDropService() override { return &mDragDropService; }
	COleDropTarget* GetDropOnAcadTarget() override { return &mBlockInsertDropTarget; }
	DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData ) override; //called to get drag data from this control
	CAcadColorService* GetColorService() override { return &mColorService; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	void OnNeedRepaint( bool bRepaintBackground = false, bool bUpdateNow = false ) const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;

	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted

protected:
	void Reset();

public:
	LPCTSTR GetFilename() const { return (msDwgFilename.IsEmpty()? NULL : (LPCTSTR)msDwgFilename); }
	void SetHighlight( const COLORREF& clrHighlight );
	void RemoveHighlight();
	bool LoadDwg( LPCTSTR pszFilename );
	void Clear();
	void Snapshot();
private:
	void SaveDC();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	void PostNcDestroy() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();	
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnSetFocus(CWnd* pOldWnd);	
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
