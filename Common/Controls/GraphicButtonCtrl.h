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
	CPictureObject* mpPressedPicture;

public:
	CGraphicButtonCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CGraphicButtonCtrl();

// DialogControl Interface
public:
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( RefCountedPtr< CPropertyObject > pProp );

public:
	void SetPicture( CPictureObject* pPict );
	CPictureObject* GetPicture() const { return mpPicture; }
	void SetPressedPicture( CPictureObject* pPict );
	CPictureObject* GetPressedPicture() const { return mpPressedPicture; }

protected:
	void UpdateButtonGraphic();

protected:
	DECLARE_MESSAGE_MAP();
};
