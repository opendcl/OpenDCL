#pragma once

#define _ACADTARGET 23

//custom macro for use with command names that include special characters (e.g. hyphens)
#define ACED_ARXCOMMAND_ENTRY_KEY_AUTO(T_CLASS,T_KEY,T_GROUPNAME,T_GLOBALNAME,T_LOCALNAME,T_FLAGS,T_CONTEXT) \
  _GRXCOMMAND_ENTRY __GrxCmdMap_##T_GROUPNAME##T_KEY = { L#T_GROUPNAME, L#T_KEY, L#T_LOCALNAME, T_FLAGS, classname::##T_GROUPNAME ##T_GLOBALNAME, T_CONTEXT, (UINT)-1 , NULL} ; \
  _CGcRxAddCommandEntryAuto __GrxAddCommandAuto_##T_GROUPNAME##T_KEY(& __GrxCmdMap_##T_GROUPNAME##T_KEY) ;

//fix prefix: gcs_ -> ads_
#include "grxEntryPoint.h"
#undef GCED_GDSSYMBOL_ENTRY_AUTO
#define GCED_GDSSYMBOL_ENTRY_AUTO(classname, name, regFunc) \
    __declspec(selectany) _GDSSYMBOL_ENTRY __GdsSymbolMap_##name = { _RXST(#name), classname::ads_ ##name, regFunc, -1 } ; \
    extern "C" __declspec(allocate("GDSSYMBOL$__m")) __declspec(selectany) _GDSSYMBOL_ENTRY* __pGdsSymbolMap_##name = &__GdsSymbolMap_##name ; \
    GCED_GDSSYMBOL_ENTRY_PRAGMA(name)

#undef kOnAutoCADStartup
