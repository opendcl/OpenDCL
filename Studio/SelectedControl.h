// SelectedControl.h : header file
//

#pragma once

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// SelectedControl window

class CSelectedControl : public CObject
{
public:
	CDclControlObject* m_pArxObject;
	CWnd* m_pControl;
	int m_nIndex;
	CRect m_rcLastDrawn;

public:
	CSelectedControl( CDclControlObject* pDclControl = NULL, CWnd* pControlWnd = NULL, int idxControl = -1 )
	: m_pArxObject( pDclControl )
	, m_pControl( pControlWnd )
	, m_nIndex( idxControl )
	, m_rcLastDrawn( 0, 0, 0, 0 )
	{
	}
	void Reset() { m_rcLastDrawn.SetRect( 0, 0, 0, 0 ); }
	void Clear()
	{	
		m_pArxObject = NULL;
		m_pControl = NULL;
		m_nIndex = -1;
	}
};
