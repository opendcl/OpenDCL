// GraphicButtonCtrl.h : header file
//
// Common functionality for GraphicButton control

#pragma once

#include "ButtonCtrl.h"

class CPictureObject;


/////////////////////////////////////////////////////////////////////////////
// CGraphicButtonCtrl window

class CGraphicButtonCtrl : public CButtonCtrl
{
	CPictureObject* mpPicture;
	CPictureObject* mpMouseOverPicture;

public:
	CGraphicButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CGraphicButtonCtrl();

// DialogControl Interface
public:
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );

public:
	void SetPicture( CPictureObject* pPict );
	CPictureObject* GetPicture() const { return mpPicture; }
	void SetMouseOverPicture( CPictureObject* pPict );
	CPictureObject* GetMouseOverPicture() const { return mpMouseOverPicture; }

protected:
	void UpdateButtonGraphic();

protected:
	DECLARE_MESSAGE_MAP();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
