#pragma once

#define _ACADTARGET 17

#define _ACRX_T(x) x


//custom macro for use with command names that include special characters (e.g. hyphens)
#define ACED_ARXCOMMAND_ENTRY_KEY_AUTO(T_CLASS,T_KEY,T_GROUPNAME,T_GLOBALNAME,T_LOCALNAME,T_FLAGS,T_CONTEXT) \
__declspec(selectany) _ArxRegisteredCommand __arxRegisteredCommand_##T_KEY##T_GROUPNAME(T_CLASS::##T_GROUPNAME ##T_KEY,_T(#T_LOCALNAME),_T(#T_GLOBALNAME),_T(#T_GROUPNAME),T_FLAGS,T_CONTEXT); \
const int __arxRegisteredFlags_##T_KEY##T_GROUPNAME = __arxRegisteredCommand_##T_KEY##T_GROUPNAME.getFlags();

//overwrite ZRX version of macros that prefix "zds_" to command handler names instead of "ads_"
#define ACED_ADSSYMBOL_ENTRY_AUTO(classname, name, regFunc) \
		__declspec(selectany) _ZDSSYMBOL_ENTRY __ZdsSymbolMap_##name = { _RXST(#name), classname::ads_ ##name, regFunc, -1 } ; \
		extern "C" __declspec(allocate("ZDSSYMBOL$__m")) __declspec(selectany) _ZDSSYMBOL_ENTRY* const __pZdsSymbolMap_##name = &__ZdsSymbolMap_##name ; \
		ZCED_ZDSSYMBOL_ENTRY_PRAGMA(name)
#define ACED_ADSCOMMAND_ENTRY_AUTO(classname, name, regFunc) \
		__declspec(selectany) _ZDSSYMBOL_ENTRY __ZdsSymbolMap_##name = { _RXST("c:") _RXST(#name), classname::ads_ ##name, regFunc, -1 } ; \
		extern "C" __declspec(allocate("ZDSSYMBOL$__m")) __declspec(selectany) _ZDSSYMBOL_ENTRY* const __pZdsSymbolMap_##name = &__ZdsSymbolMap_##name ; \
		ZCED_ZDSSYMBOL_ENTRY_PRAGMA(name)
#define ACED_ADSSYMBOL_ENTRYBYID_AUTO(classname, name, nameId, regFunc) \
		__declspec(selectany) _ZDSSYMBOL_ENTRY __ZdsSymbolMap_##name = { NULL, classname::ads_ ##name, regFunc, nameId } ; \
		extern "C" __declspec(allocate("ADSSYMBOL$__m")) __declspec(selectany) _ZDSSYMBOL_ENTRY* const __pZdsSymbolMap_##name = &__ZdsSymbolMap_##name ; \
		ZCED_ZDSSYMBOL_ENTRY_PRAGMA(name)

#define _ZRX_CUSTOM_DRAG_N_DROP_


extern "C" int zds_queueexpr( char* pszCommand );
#define ads_queueexpr zds_queueexpr
#define acedEvaluateLisp zcedEvaluateLisp

#define subSetAttributes setAttributes
#define subWorldDraw worldDraw
#define subViewportDraw viewportDraw
