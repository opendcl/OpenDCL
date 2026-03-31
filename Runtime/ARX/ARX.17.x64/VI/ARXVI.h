#pragma once

#define _ACADTARGET 17

#include <memory>
namespace std
{
	template <class T> class unique_ptr : public auto_ptr<T>
	{
	public:
		explicit unique_ptr( T* p = 0 ) : auto_ptr<T>( p ) {}
	};
}


//custom macro for use with command names that include special characters (e.g. hyphens)
#define ACED_ARXCOMMAND_ENTRY_KEY_AUTO(classname, key, group, globCmd, locCmd, cmdFlags, UIContext) \
	__declspec(selectany) _ARXCOMMAND_ENTRY __ArxCmdMap_##group##key = { _RXST(#group), _RXST(#globCmd), _RXST(#locCmd), cmdFlags, classname::##group ##key, UIContext } ; \
	extern "C" __declspec(allocate("ARXCOMMAND$__m")) __declspec(selectany) _ARXCOMMAND_ENTRY* const __pArxCmdMap_##group##key = &__ArxCmdMap_##group##key ; \
	ACED_ARXCOMMAND_ENTRY_PRAGMA(group, key)


extern "C" int ads_queueexpr( wchar_t* pszCommand );

#define subSetAttributes setAttributes
#define subWorldDraw worldDraw
#define subViewportDraw viewportDraw

#define ON_WM_MOUSELEAVE() \
	{ WM_MOUSELEAVE, 0, 0, 0, AfxSig_vv, \
		(AFX_PMSG)(AFX_PMSGW) \
		(static_cast< void (AFX_MSG_CALL CWnd::*)(void) > ( &ThisClass :: OnMouseLeave)) },

#include "UxThemeVI.h"
