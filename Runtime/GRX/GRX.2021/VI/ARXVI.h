#pragma once

#define _ACADTARGET 19

#pragma comment(linker, "/EXPORT:odrxCreateModuleObject=_odrxCreateModuleObject")
#pragma comment(linker, "/EXPORT:odrxGetAPIVersion=_odrxGetAPIVersion")


//custom macro for use with command names that include special characters (e.g. hyphens)
#define ACED_ARXCOMMAND_ENTRY_KEY_AUTO(T_CLASS,T_KEY,T_GROUPNAME,T_GLOBALNAME,T_LOCALNAME,T_FLAGS,T_CONTEXT) \
  _GRXCOMMAND_ENTRY __GrxCmdMap_##T_GROUPNAME##T_KEY = { L#T_GROUPNAME, L#T_KEY, L#T_LOCALNAME, T_FLAGS, classname::##T_GROUPNAME ##T_GLOBALNAME, T_CONTEXT, (UINT)-1 , NULL} ; \
  _CGcRxAddCommandEntryAuto __GrxAddCommandAuto_##T_GROUPNAME##T_KEY(& __GrxCmdMap_##T_GROUPNAME##T_KEY) ;

#include "acad2grx.h"
#undef kOnAutoCADStartup
