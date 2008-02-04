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
	COleDropTarget* mpDropOnAcadTarget;
	bool mbRegistered;

public:
	CArxDragDropService( CDialogControl* pDlgControl );
	virtual ~CArxDragDropService();

public:
	virtual bool RegisterControlAsDropTarget();
	virtual bool RevokeControlAsDropTarget();
	virtual COleDropTarget* GetCustomAcadDropTarget() const { return NULL; }
	virtual DROPEFFECT BeginDragDrop( const CPoint& point );
};
