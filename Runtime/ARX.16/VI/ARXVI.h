#pragma once

#define _ACADTARGET 16

//define ACHAR type
typedef char ACHAR;
#define ACRX_T(x) x

#include <TChar.h> //needed by acrxEntryPoint.cpp

typedef unsigned char __RPC_FAR *RPC_CSTR;


//custom macro for use with command names that include special characters (e.g. hyphens)
#define ACED_ARXCOMMAND_ENTRY_KEY_AUTO(classname, key, group, globCmd, locCmd, cmdFlags, UIContext) \
	__declspec(selectany) _ARXCOMMAND_ENTRY __ArxCmdMap_##group##key = { _RXST(#group), _RXST(#globCmd), _RXST(#locCmd), cmdFlags, classname::##group ##key, UIContext } ; \
	extern "C" __declspec(allocate("ARXCOMMAND$__m")) __declspec(selectany) _ARXCOMMAND_ENTRY* const __pArxCmdMap_##group##key = &__ArxCmdMap_##group##key ; \
	ACED_ARXCOMMAND_ENTRY_PRAGMA(group, key)


extern "C" int ads_queueexpr( ACHAR* pszCommand );
