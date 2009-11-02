// GraphicButtonCtrl.h : header file
//
// Common functionality for GraphicButton control

#pragma once

#include "ButtonCtrl.h"
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// CGraphicButtonCtrl window

class CGraphicButtonCtrl : public CButtonCtrl
{
	TPicturePtr mpPicture;
	TPicturePtr mpMouseOverPicture;

public:
	CGraphicButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CGraphicButtonCtrl();

// DialogControl Interface
public:
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual void OnValidateBkgnd( CWnd* pBkgnd );

public:
	void SetPicture( TPicturePtr pPict );
	TPicturePtr GetPicture() const { return mpPicture; }
	void SetMouseOverPicture( TPicturePtr pPict );
	TPicturePtr GetMouseOverPicture() const { return mpMouseOverPicture; }

protected:
	virtual void UpdateButtonGraphic();

protected:
	DECLARE_MESSAGE_MAP();
};
