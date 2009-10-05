// CAcUiMRUComboBox implementation

#include "StdAfx.h"
#include "MFC/CAcUi/CAcUiMRUComboBox.h"


class OdclCAcUiMRUComboBox
{
	public:
		void DrawItem(LPDRAWITEMSTRUCT)
		{
		}
};

#pragma warning(push)
#pragma warning(disable: 4608)
template < typename Dest, typename Src >
Dest force_cast( Src src )
{
	union _convertor { Dest d; Src s; _convertor() : d(0), s(0) {} } convertor;
	convertor.s = src;
	return convertor.d;
}
#pragma warning(pop)


class CAcUiMRUComboBox_DrawItem_fixup
{
	FARPROC mpfOriginal;
	BYTE mrbSaved[5];
	DWORD mdwOldProtect;
	static HMODULE GetBrxApiModule()
		{
		#if (_BRXTARGET == 10)
			return GetModuleHandle( _T("brx10.dll") );
		#elif (_BRXTARGET == 9)
			return GetModuleHandle( _T("brx.dll") );
		#else
			#error Unknown BRX target!
			return GetModuleHandle( NULL );
		#endif
		}
public:
	CAcUiMRUComboBox_DrawItem_fixup()
		: mpfOriginal( GetProcAddress( GetBrxApiModule(), "?DrawItem@CAcUiMRUComboBox@@UAEXPAUtagDRAWITEMSTRUCT@@@Z" ) )
		{
			if( mpfOriginal )
			{
				VirtualProtect( mpfOriginal, sizeof(mrbSaved), PAGE_EXECUTE_READWRITE, &mdwOldProtect );
				CopyMemory( mrbSaved, mpfOriginal, sizeof(mrbSaved) );
				*(LPBYTE)mpfOriginal = 0xe9; //jmp
				*(DWORD_PTR*)((LPBYTE)mpfOriginal + 1) = ((DWORD_PTR)force_cast<DWORD_PTR>(&OdclCAcUiMRUComboBox::DrawItem) - (DWORD_PTR)mpfOriginal - 5);
			}
		}
	~CAcUiMRUComboBox_DrawItem_fixup()
		{
			if( mpfOriginal )
			{
				CopyMemory( mpfOriginal, mrbSaved, sizeof(mrbSaved) );
				DWORD dwOldProtect;
				VirtualProtect( mpfOriginal, sizeof(mrbSaved), mdwOldProtect, &dwOldProtect );
			}
		}
} gCAcUiMRUComboBox_DrawItem_fixup;