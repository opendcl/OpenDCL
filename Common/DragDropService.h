// DragDropService.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CDragDropService class

class CDragDropService
{
public:
	CDragDropService();
	virtual ~CDragDropService();

public:
	static UINT GetDclControlClipboardFormat();

public:
	virtual bool RegisterControlAsDropTarget() = 0;
	virtual bool RevokeControlAsDropTarget() = 0;
	virtual COleDropTarget* GetCustomAcadDropTarget() const { return NULL; }
	virtual DROPEFFECT BeginDragDrop( const CPoint& point ) = 0;
};
