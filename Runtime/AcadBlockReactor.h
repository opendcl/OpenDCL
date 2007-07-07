// AcadBlockReactor.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CAcadBlockReactor

class CAcadBlockReactor : public AcEditorReactor 
{
public:
	CAcadBlockReactor(CWnd *pBlockList = NULL, CWnd *pBlockView = NULL);
	~CAcadBlockReactor();

	virtual void endInsert(AcDbDatabase* pTo);
	virtual void endDeepClone(AcDbIdMapping& idMap);
	CWnd *m_pParentBlockList;
	CWnd *m_pParentBlockView;
	virtual void commandEnded(const char * cmdStr);
};
