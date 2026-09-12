#pragma once

#include "ColorService.h"

class CArxWorkspace;

// Process-level CAD COLORTHEME syscolor tables. Light is default; dark is used
// only when COLORTHEME exists and equals 0. Callers map an index through the
// current table. The editor reactor is attached only while a form is registered.

class CArxColorService : public CWin32ColorService
{
	enum { kSysColorCount = 32 }; // COLOR_GRADIENTINACTIVECAPTION is 30

	CArxWorkspace* mpWorkspace;
	COLORREF m_light[kSysColorCount];
	COLORREF m_dark[kSysColorCount];
	const COLORREF* m_pTable;
	int m_nClients;
	bool m_bReactorAdded;
	bool m_bRefreshing;

	class CEdReactor : public AcEditorReactor
	{
		CArxColorService* mpService;
	public:
		CEdReactor( CArxColorService* pService ) : mpService( pService ) {}
#if defined(_ZRXTARGET)
		void sysVarChanged( const ZTCHAR* pszVarName, bool /*success*/ )
			{ mpService->OnColorThemeSysVar( pszVarName ); }
#elif defined(_GRXTARGET)
		void sysVarChanged( const GCHAR* pszVarName, bool /*success*/ )
			{ mpService->OnColorThemeSysVar( pszVarName ); }
#else
		void sysVarChanged( const ACHAR* pszVarName, Adesk::Boolean /*success*/ )
			{ mpService->OnColorThemeSysVar( pszVarName ); }
#endif
	} mEdReactor;

	void FillFromSysColors( COLORREF* pTable );
	bool ColorThemeExistsAndIsZero() const;
	bool SelectTable();
	void AttachReactor();
	void DetachReactor();
	void OnColorThemeSysVar( LPCTSTR pszVarName );

public:
	CArxColorService( CArxWorkspace* pWorkspace );
	~CArxColorService();

	void AddClient();
	void ReleaseClient();
	void Shutdown();
	COLORREF GetSysColor( int nIndex ) const override;
};
