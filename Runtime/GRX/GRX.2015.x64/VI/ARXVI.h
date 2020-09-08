#pragma once

#define _ACADTARGET 19

#define _ACRX_T(x) L ## x

#pragma comment(linker, "/EXPORT:odrxCreateModuleObject")
#pragma comment(linker, "/EXPORT:odrxGetAPIVersion")


//custom macro for use with command names that include special characters (e.g. hyphens)
#define ACED_ARXCOMMAND_ENTRY_KEY_AUTO(T_CLASS,T_KEY,T_GROUPNAME,T_GLOBALNAME,T_LOCALNAME,T_FLAGS,T_CONTEXT) \
  _GRXCOMMAND_ENTRY __GrxCmdMap_##T_GROUPNAME##T_KEY = { L#T_GROUPNAME, L#T_KEY, L#T_LOCALNAME, T_FLAGS, classname::##T_GROUPNAME ##T_GLOBALNAME, T_CONTEXT, (UINT)-1 , NULL} ; \
  _CGcRxAddCommandEntryAuto __GrxAddCommandAuto_##T_GROUPNAME##T_KEY(& __GrxCmdMap_##T_GROUPNAME##T_KEY) ;

//overwrite GRX version of macros that prefix "gds_" to command handler names instead of "ads_"
#define ACED_ADSSYMBOL_ENTRY_AUTO(classname, name, regFunc) \
  _GDSSYMBOL_ENTRY __GdsSymbolMap_##name = { L#name, classname::ads_ ##name, regFunc, (UINT)-1 , NULL } ; \
  _CGcRxAddSymbolEntryAuto __GrxAddSymbolAuto_##name(& __GdsSymbolMap_##name) ;

#define ACED_ADSCOMMAND_ENTRY_AUTO(classname, name, regFunc) \
  _GDSSYMBOL_ENTRY __GdsSymbolMap_##name = { L"C:" L#name, classname::ads_ ##name, regFunc, (UINT)-1 , NULL } ; \
  _CGcRxAddSymbolEntryAuto __GrxAddSymbolAuto_##name(& __GdsSymbolMap_##name) ;

#define ACED_ADSSYMBOL_ENTRYBYID_AUTO(classname, name, nameId, regFunc) \
  _GDSSYMBOL_ENTRY __GdsSymbolMap_##name = { NULL, classname::ads_ ##name, regFunc, nameId , NULL } ; \
  _CGcRxAddSymbolEntryAuto __GrxAddSymbolAuto_##name(& __GdsSymbolMap_##name) ;

#define kOnDWGdirectStartup kOnTeighaStartup
