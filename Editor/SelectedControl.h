// SelectedControl.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// SelectedControl window

class CSelectedControl : public CObject
{
public:
	CSelectedControl()
	{
		Clear();
		Reset();
	};
	class CDclControlObject *m_pArxObject;
	CWnd *m_pControl;
	int m_nIndex;
	CRect m_rcLastDrawn;
	void Reset(){
		m_rcLastDrawn.left = 0;
		m_rcLastDrawn.top = 0;
		m_rcLastDrawn.bottom = 0;
		m_rcLastDrawn.right = 0;
	};
	void Clear(){	
		m_pArxObject = NULL;
		m_pControl = NULL;
		m_nIndex = -1;
	}
};
