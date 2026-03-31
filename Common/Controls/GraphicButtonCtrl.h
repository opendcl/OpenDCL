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
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;

public:
	void SetPicture( TPicturePtr pPict );
	TPicturePtr GetPicture() const { return mpPicture; }
	void SetMouseOverPicture( TPicturePtr pPict );
	TPicturePtr GetMouseOverPicture() const { return mpMouseOverPicture; }

protected:
	void UpdateButtonGraphic() override;

protected:
	DECLARE_MESSAGE_MAP();
};
