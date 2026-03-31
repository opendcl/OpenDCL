// ArxDragDropService.h : header file
//

#pragma once

#include "DragDropService.h"

class CDialogControl;


/////////////////////////////////////////////////////////////////////////////
// CArxDragDropService class

class CArxDragDropService : public CDragDropService
{
	CDialogControl* mpDlgControl;
	COleDropSource* mpDropSource;
	bool mbMustDeleteDropTarget;
	COleDropTarget* mpDropTarget;
	bool mbMustDeleteDropOnAcadTarget;
	COleDropTarget* mpDropOnAcadTarget;
	bool mbRegistered;

public:
	CArxDragDropService( CDialogControl* pDlgControl );
	virtual ~CArxDragDropService();

public:
	bool RegisterControlAsDropTarget() override;
	bool RevokeControlAsDropTarget() override;
	COleDropTarget* GetCustomAcadDropTarget() const override { return NULL; }
	DROPEFFECT BeginDragDrop( const CPoint& point ) override;
};
