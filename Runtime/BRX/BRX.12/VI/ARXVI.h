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
#define ACED_ARXCOMMAND_ENTRY_KEY_AUTO(T_CLASS,T_KEY,T_GROUPNAME,T_GLOBALNAME,T_LOCALNAME,T_FLAGS,T_CONTEXT) \
__declspec(selectany) _ArxRegisteredCommand __arxRegisteredCommand_##T_KEY##T_GROUPNAME(T_CLASS::##T_GROUPNAME ##T_KEY,_T(#T_LOCALNAME),_T(#T_GLOBALNAME),_T(#T_GROUPNAME),T_FLAGS,T_CONTEXT); \
const int __arxRegisteredFlags_##T_KEY##T_GROUPNAME = __arxRegisteredCommand_##T_KEY##T_GROUPNAME.getFlags();


extern "C" int ads_queueexpr( wchar_t* pszCommand );

#define subSetAttributes setAttributes
#define subWorldDraw worldDraw
#define subViewportDraw viewportDraw

#define ON_WM_MOUSELEAVE() \
	{ WM_MOUSELEAVE, 0, 0, 0, AfxSig_vv, \
		(AFX_PMSG)(AFX_PMSGW) \
		(static_cast< void (AFX_MSG_CALL CWnd::*)(void) > ( &ThisClass :: OnMouseLeave)) },

#include "UxThemeVI.h"
