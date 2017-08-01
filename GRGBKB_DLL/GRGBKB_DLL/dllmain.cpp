// dllmain.cpp : 定義 DLL 應用程式的進入點。
#include "stdafx.h"
//#include "GRGBKB_Header.h"
extern "C" {
	__declspec(dllexport) int __cdecl GRGBDLL_InitialDLL(void);
	__declspec(dllexport) void __cdecl GRGBDLL_DestroyDLL(void);
	void GRGBDLL_DLL_THREAD_ATTACH();
	void GRGBDLL_DLL_THREAD_DETACH();
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GRGBDLL_InitialDLL();
		DisableThreadLibraryCalls(hModule);
		break;
	case DLL_THREAD_ATTACH:
		GRGBDLL_DLL_THREAD_ATTACH();
		break;
	case DLL_THREAD_DETACH:
		GRGBDLL_DLL_THREAD_DETACH();
		break;
	case DLL_PROCESS_DETACH:
		GRGBDLL_DestroyDLL();
		break;
	}
	return TRUE;
}

