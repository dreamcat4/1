// GRGBKB_DLL.cpp : 定義 DLL 應用程式的匯出函式。
//

#include "stdafx.h"
#include "GRGBKB_Header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windowsx.h>
#include <time.h>


#ifdef __cplusplus    // If used by C++ code,
extern "C" {          // we need to export the C interface
#endif


void GRGBDLL_My_debug_log(char *fmt, ...)
	{
		char str[128] = {0x0}, fname[300];
		va_list args;
		SYSTEMTIME st;

		va_start(args, fmt);
		vsprintf(str, fmt, args);
		va_end(args);

		GetLocalTime(&st);
		//sprintf(fname, "debug_log.txt", st.wYear, st.wMonth, st.wDay);
		sprintf(fname, "debug_log.txt");//, st.wYear, st.wMonth, st.wDay);

		FILE *fp = fopen(fname, "a");
		if(fp)
		{
			fprintf(fp, "[%02d:%02d:%02d.%03d]  %s\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, str);
			//fprintf(fp, "[%02d:%02d:%02d.%03d]\n\r  R = %d, G = %d, B = %d\n", 
			//	st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GRGBDLL_g_iLRedStep, GRGBDLL_g_iLGreenStep, GRGBDLL_g_iLBlueStep);
			fclose(fp);
		}
	}


//for Delay work around==========================================
void GRGBDLL_My_Delay_Time(double a_douDelaySec)
{
	LARGE_INTEGER litmp;
	LONGLONG qt1, qt2;
	double dft = 0.0, dff, dfm;
	QueryPerformanceFrequency(&litmp);//獲得時鐘頻率
	dff = ( double )litmp.QuadPart;
	QueryPerformanceCounter(&litmp);//獲得初始值
	qt1 = litmp.QuadPart;// Sleep(1);
	while(dft < a_douDelaySec){
		QueryPerformanceCounter(&litmp);//獲得終止值
		qt2 = litmp.QuadPart;
		dfm = ( double )(qt2 - qt1);
		dft = dfm / dff;//獲得對應的時間值
	}
	qt1 = 0; // for Debug
}

BYTE GRGBDLL_checksum(BYTE *data, int len)
{
	int i, sum = 0;

	for(i = 0; i < len; i++)
		sum += data[i];

	return (0xFF - sum);
}

void GRGBDLL_Send_LED_offset(BYTE vk_key, bool is_make)
{
	int i;
	BYTE set_data[8] = { 0x19,4,2 };

	for(i = 0; i < KEYS_NUM; i++)
	{
		if(GRGBDLL_g_104_keys[i].vk_key == 0)
		{
		}
		else if(GRGBDLL_g_104_keys[i].vk_key == vk_key)
		{
			set_data[3] = i+1;
			set_data[4] = (is_make) ? 0xFF : 0x00;
			set_data[7] = GRGBDLL_checksum(set_data, 7);
			USB_Set_Report(set_data, 8);
			Sleep(10);
		}
	}
}

LRESULT CALLBACK GRGBDLL_KeyboardHookProc(int code, WPARAM wParam, LPARAM lParam)
{
	wchar_t sTestState[128] = {0x0};

	KBDLLHOOKSTRUCT *pkbhs = ( KBDLLHOOKSTRUCT * )lParam;

	if(code == HC_ACTION)
	{
		if(pkbhs->vkCode == 0xFF)
		{
			return CallNextHookEx(GRGBDLL_g_kholl, code, wParam, lParam);
		}

		//swprintf(sTestState, sizeof(sTestState), L"pkbhs->flags = %d", pkbhs->flags);
		//MessageBox(NULL, sTestState, L"G_RGB_KB_DLL", MB_OK);

		if((pkbhs->vkCode == 0x0D) && (pkbhs->flags & 1)) // Num Enter
			pkbhs->vkCode = 0xFD;
		else if((pkbhs->vkCode == 0x24) && ((pkbhs->flags & 1) == 0)) // Num Home
			pkbhs->vkCode = 0x67; // Num 7
		else if((pkbhs->vkCode == 0x26) && ((pkbhs->flags & 1) == 0)) // Num Up
			pkbhs->vkCode = 0x68; // Num 8
		else if((pkbhs->vkCode == 0x21) && ((pkbhs->flags & 1) == 0)) // Num PgUp
			pkbhs->vkCode = 0x69; // Num 9
		else if((pkbhs->vkCode == 0x25) && ((pkbhs->flags & 1) == 0)) // Num Left
			pkbhs->vkCode = 0x64; // Num 4
		else if((pkbhs->vkCode == 0x27) && ((pkbhs->flags & 1) == 0)) // Num Right
			pkbhs->vkCode = 0x66; // Num 6
		else if((pkbhs->vkCode == 0x23) && ((pkbhs->flags & 1) == 0)) // Num End
			pkbhs->vkCode = 0x61; // Num 1
		else if((pkbhs->vkCode == 0x28) && ((pkbhs->flags & 1) == 0)) // Num Down
			pkbhs->vkCode = 0x62; // Num 2
		else if((pkbhs->vkCode == 0x22) && ((pkbhs->flags & 1) == 0)) // Num PgDn
			pkbhs->vkCode = 0x63; // Num 3
		else if((pkbhs->vkCode == 0x2d) && ((pkbhs->flags & 1) == 0)) // Num Ins
			pkbhs->vkCode = 0x60; // Num 0
		else if((pkbhs->vkCode == 0x2e) && ((pkbhs->flags & 1) == 0)) // Num Del
			pkbhs->vkCode = 0x6e; // Num .
		else if(pkbhs->vkCode == 0xc) // Num Clear
			pkbhs->vkCode = 0x65; // Num 5

		if((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN))
		{
			if(GRGBDLL_g_last_key == pkbhs->vkCode)
			{
				return CallNextHookEx(GRGBDLL_g_kholl, code, wParam, lParam);
			}
			GRGBDLL_Send_LED_offset(( BYTE )pkbhs->vkCode, true);
			GRGBDLL_g_last_key = pkbhs->vkCode;
		}
		else if((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP))
		{
			GRGBDLL_Send_LED_offset(( BYTE )pkbhs->vkCode, false);
			GRGBDLL_g_last_key = 0;
		}
	}
	return CallNextHookEx(GRGBDLL_g_kholl, code, wParam, lParam);
}



BOOL GRGBDLL_USB_FindHID(int a_VendorID, int a_ProductID) 
{
	HIDD_ATTRIBUTES						Attributes;
	SP_DEVICE_INTERFACE_DATA			devInfoData;
	BOOL								bLastDevice = FALSE;
	int									iMemberIndex = 0;
	LONG								lResult = 0;
	ULONG								ulLength = 0;
	PSP_DEVICE_INTERFACE_DETAIL_DATA	detailData;
	HANDLE								hDeviceHandle;
	HANDLE								hDevInfo;
	GUID								HidGuid;
	ULONG								Required;
	BOOL								MyDeviceDetected = FALSE;

	USB_g_MyDevicePathName[0] = 0;	USB_g_MyVendorPathName[0] = 0;
	
	detailData = NULL;			hDeviceHandle = NULL;
	USB_g_feature_length = 0;			USB_g_input_length = 0;			USB_g_output_length = 0;

	HidD_GetHidGuid(&HidGuid);

	hDevInfo = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);

	devInfoData.cbSize = sizeof(devInfoData);

	//iMemberIndex =  0;
	//==Loop 100 Count===========================================
	for(iMemberIndex= 0; (iMemberIndex<= 100); iMemberIndex++){
		if(MyDeviceDetected == FALSE) {
			if(SetupDiEnumDeviceInterfaces(hDevInfo, 0, &HidGuid, iMemberIndex, &devInfoData)) {
				lResult = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &ulLength, NULL);

				detailData = ( PSP_DEVICE_INTERFACE_DETAIL_DATA )malloc(ulLength);
				detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

				//lResult = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, ulLength, &Required, NULL);
				if(SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, ulLength, &Required, NULL)){
					hDeviceHandle = CreateFile(detailData->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
											( LPSECURITY_ATTRIBUTES )NULL, OPEN_EXISTING, 0, NULL);

					//==Get HID 屬性============================================
					Attributes.Size = sizeof(Attributes);
					lResult = HidD_GetAttributes(hDeviceHandle, &Attributes);

					MyDeviceDetected = FALSE;
					if((Attributes.VendorID == a_VendorID) && (Attributes.ProductID == a_ProductID)) {
						//wcscpy(USB_g_MyDevicePathName, detailData->DevicePath);
						//wcscpy_s(USB_g_MyDevicePathName, sizeof(USB_g_MyDevicePathName), detailData->DevicePath);
						PHIDP_PREPARSED_DATA pPreData = NULL;
						if(HidD_GetPreparsedData(hDeviceHandle, &pPreData)) {
							HIDP_CAPS cap = { 0 };
							if(HidP_GetCaps(pPreData, &cap) == HIDP_STATUS_SUCCESS) {
								if (1 == cap.Usage) {
									switch (cap.UsagePage) {
										case _1ST_FME_USAGEPAGE:
										case _1ST_4ME_USAGEPAGE:
										case _2nd_FME_USAGEPAGE:
										{
											wcscpy_s(USB_g_MyDevicePathName, sizeof(USB_g_MyDevicePathName) / sizeof(wchar_t), detailData->DevicePath);

											if (cap.FeatureReportByteLength > 0)
											{
												MyDeviceDetected = TRUE;
												USB_g_input_length = cap.InputReportByteLength;
												USB_g_feature_length = cap.FeatureReportByteLength;
												USB_g_output_length = cap.OutputReportByteLength;
											}
											switch (cap.UsagePage) {
												case _1ST_FME_USAGEPAGE:
													GRGBDLL_g_bKBType = _FULL_ME_KB;
													break;
												case _1ST_4ME_USAGEPAGE:
													GRGBDLL_g_bKBType = _4ZONE_ME_KB;
													break;
												case _2nd_FME_USAGEPAGE:
													GRGBDLL_g_bKBType = _FULL_ME_KB;
													break;
											}

										}
										break;
									}
								}
								/*
								if(cap.Usage == 1 && cap.UsagePage == 0xFF02)
								{
									wcscpy_s(USB_g_MyDevicePathName, sizeof(USB_g_MyDevicePathName)/ sizeof(wchar_t), detailData->DevicePath);

									if(cap.FeatureReportByteLength > 0)
									{
										MyDeviceDetected = TRUE;
										USB_g_input_length = cap.InputReportByteLength;
										USB_g_feature_length = cap.FeatureReportByteLength;
										USB_g_output_length = cap.OutputReportByteLength;
									}
									GRGBDLL_g_bKBType = _FULL_ME_KB;
								}
								else if(cap.Usage == 1 && cap.UsagePage == 0xFF12)
								{
									wcscpy_s(USB_g_MyDevicePathName, sizeof(USB_g_MyDevicePathName)/ sizeof(wchar_t ), detailData->DevicePath);

									if(cap.FeatureReportByteLength > 0)
									{
										MyDeviceDetected = TRUE;
										USB_g_input_length = cap.InputReportByteLength;
										USB_g_feature_length = cap.FeatureReportByteLength;
										USB_g_output_length = cap.OutputReportByteLength;
									}
									GRGBDLL_g_bKBType = _4ZONE_ME_KB;
								}
								*/
							}
							HidD_FreePreparsedData(pPreData);
						}
					}
				}
				CloseHandle(hDeviceHandle);
				free(detailData);
			}
			else {
				if(GetLastError() == ERROR_NO_MORE_ITEMS){
					//MessageBox(NULL, L"USB disconnect", L"G_RGB_KB_DLL", MB_OK);
					break;
				}
			}
		}
		else break;
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return MyDeviceDetected;
}

int USB_Set_Report(BYTE *a_pbData, int a_iLen)
{
	HANDLE hDeviceHandle = NULL;
	BYTE bToHWReport[32] = {0};
	ULONG ulResult = 0;

	if(USB_g_MyDevicePathName[0] == 0) {
		//MessageBox(NULL, L"Device name does not exist ", L"G_RGB_KB_DLL", MB_OK);
		return 1;
	}
	hDeviceHandle = CreateFile(USB_g_MyDevicePathName,
							  0,
							  FILE_SHARE_READ | FILE_SHARE_WRITE,
							  ( LPSECURITY_ATTRIBUTES )NULL,
							  OPEN_EXISTING,
							  0,
							  NULL);
	if(hDeviceHandle == INVALID_HANDLE_VALUE){
		//MessageBox(NULL, L"Can not open USB Device", L"G_RGB_KB_DLL", MB_OK);
		return 2;
	}
	memcpy(bToHWReport + 1, a_pbData, a_iLen);
	Sleep(1);
	ulResult = HidD_SetFeature(hDeviceHandle, bToHWReport, USB_g_feature_length);
	CloseHandle(hDeviceHandle);

	if(ulResult == 0)
		return 3;
	return 0;
}

int USB_Get_Report(BYTE *a_pbData, int a_iLen)
{
	HANDLE hDeviceHandle = NULL;
	BYTE bGet_Data[32] = { 0 };
	ULONG ulResult = 0;

	if(USB_g_MyDevicePathName[0] == 0) {
		//MessageBox(NULL, L"Device name does not exist ", L"G_RGB_KB_DLL", MB_OK);
		return 1;
	}

	hDeviceHandle = CreateFile(USB_g_MyDevicePathName,
							  0,
							  FILE_SHARE_READ | FILE_SHARE_WRITE,
							  ( LPSECURITY_ATTRIBUTES )NULL,
							  OPEN_EXISTING,
							  0,
							  NULL);

	if(hDeviceHandle == INVALID_HANDLE_VALUE) {
		//MessageBox(NULL, L"Can not open USB Device", L"G_RGB_KB_DLL", MB_OK);
		return 2;
	}

	ulResult = HidD_GetFeature(hDeviceHandle, bGet_Data, USB_g_feature_length);
	CloseHandle(hDeviceHandle);

	if(ulResult == 0)
		return 3;

	memcpy(a_pbData, bGet_Data + 1, a_iLen);

	return 0;
}


int USB_IO_Report(BYTE *a_pbOut_data, int a_iOut_len, BYTE *a_pbIn_data, int a_iIn_len)
{
	HANDLE DeviceHandle;
	BYTE Set_Data[32] = { 0 };
	BYTE Get_Data[32] = { 0 };
	ULONG Result;
	int i;

	if(USB_g_MyDevicePathName[0] == 0)
		return 1;

	DeviceHandle = CreateFile(USB_g_MyDevicePathName,
							  0,
							  FILE_SHARE_READ | FILE_SHARE_WRITE,
							  ( LPSECURITY_ATTRIBUTES )NULL,
							  OPEN_EXISTING,
							  0,
							  NULL);

	if(DeviceHandle == INVALID_HANDLE_VALUE)		return 2;

	for(i = 0; i < 20; i++){
		memcpy(Set_Data + 1, a_pbOut_data, a_iOut_len);
		Sleep(1);
		Result = HidD_SetFeature(DeviceHandle, Set_Data, USB_g_feature_length);
		if(Result == FALSE){
			CloseHandle(DeviceHandle);
			return 3;
		}

		Sleep(1);
		Result = HidD_GetFeature(DeviceHandle, Get_Data, USB_g_feature_length);
		if(Result == FALSE){
			CloseHandle(DeviceHandle);
			return 4;
		}

		if(Get_Data[1] == a_pbOut_data[0]){
			memcpy(a_pbIn_data, Get_Data + 1, a_iIn_len);
			break;
		}
	}
	CloseHandle(DeviceHandle);

	if(i == 20)
	{
		return 5;
	}

	return 0;
}


int USB_Set_Report_And_Write_EP4(BYTE *a_pbSetReportData, int SetLen, BYTE *a_pbWriteData, int iWriteLen)
{
	HANDLE DeviceHandle;
	BYTE Set_Data[32] = { 0 };
	BYTE WriteBuffer[65] = { 0 };
	ULONG Result, wlen;

	if(USB_g_MyDevicePathName[0] == 0)
		return 1;

	DeviceHandle = CreateFile(USB_g_MyDevicePathName,
							  GENERIC_WRITE,
							  FILE_SHARE_WRITE,
							  ( LPSECURITY_ATTRIBUTES )NULL,
							  OPEN_EXISTING,
							  0,
							  NULL);

	if(DeviceHandle == INVALID_HANDLE_VALUE)
		return 2;

	memcpy(Set_Data + 1, a_pbSetReportData, SetLen);

	Result = HidD_SetFeature(DeviceHandle, Set_Data, USB_g_feature_length);

	if(Result == FALSE)
	{
		CloseHandle(DeviceHandle);
		return 3;
	}

	memcpy(WriteBuffer+2, a_pbWriteData, iWriteLen);

	Result = WriteFile(DeviceHandle, WriteBuffer, USB_g_output_length, &wlen, NULL);
	CloseHandle(DeviceHandle);

	if((Result == FALSE) || (wlen != USB_g_output_length))
		return 4;

	return 0;
}

int USB_Get_Report_And_Read_EP3(BYTE *data, int len, BYTE *color_data, int color_len)
{
	HANDLE DeviceHandle;
	BYTE Set_Data[32] = { 0 };
	BYTE Get_Data[32] = { 0 };
	BYTE ReadBuffer[65] = { 0 };
	ULONG Result, rlen;
	int i;

	if(USB_g_MyDevicePathName[0] == 0)
		return 1;

	DeviceHandle = CreateFile(USB_g_MyDevicePathName,
							  GENERIC_READ | GENERIC_WRITE,
							  FILE_SHARE_READ | FILE_SHARE_WRITE,
							  ( LPSECURITY_ATTRIBUTES )NULL,
							  OPEN_EXISTING,
							  0,
							  NULL);

	if(DeviceHandle == INVALID_HANDLE_VALUE)
		return 2;

	for(i = 0; i < 20; i++)
	{
		memcpy(Set_Data + 1, data, len);

		Result = HidD_SetFeature(DeviceHandle, Set_Data, USB_g_feature_length);
		if(Result == FALSE)
		{
			CloseHandle(DeviceHandle);
			return 3;
		}

		Sleep(20);

		memset(Get_Data, 0, sizeof(Get_Data));
		Result = HidD_GetFeature(DeviceHandle, Get_Data, 9);
		if(Result == FALSE)
		{
			CloseHandle(DeviceHandle);
			return 4;
		}

		if(Get_Data[1] == data[0])
			break;
		Sleep(20);
	}

	if(i == 20)
	{
		CloseHandle(DeviceHandle);
		return 5;
	}

	Result = ReadFile(DeviceHandle, ReadBuffer, 65, &rlen, NULL);
	memcpy(color_data, ReadBuffer + 1, color_len);
	CloseHandle(DeviceHandle);

	if((Result == FALSE) || (rlen != 65))
		return 6;

	return 0;
}

int USB_Out_Pic_EP4(BYTE *data, int len)
{
	HANDLE DeviceHandle;
	BYTE EP4_Data[100] = { 0 };
	ULONG Result;
	DWORD wlen ,block_size;
	int i;

	if(USB_g_MyDevicePathName[0] == 0)
		return 1;

	DeviceHandle = CreateFile(USB_g_MyDevicePathName,
							  GENERIC_WRITE,
							  0,
							  ( LPSECURITY_ATTRIBUTES )NULL,
							  OPEN_EXISTING,
							  0,
							  NULL);

	if(DeviceHandle == INVALID_HANDLE_VALUE)
		return 2;

	for(i = 0; i < len; i += 64)
	{
		memset(EP4_Data, 0, sizeof(EP4_Data));
		block_size = len - i;
		if(block_size > 64)
		block_size = 64;
		memcpy(EP4_Data + 1, data + i, block_size);

		wlen = 0;
		Result = WriteFile(DeviceHandle, EP4_Data, USB_g_output_length, &wlen, 0);
		if((Result == FALSE) || (wlen != USB_g_output_length))
		{
			CloseHandle(DeviceHandle);
			return 3;
		}
	}

	CloseHandle(DeviceHandle);

	return 0;
}

int USB_Out_EP4(BYTE *a_pbWriteData, int len)
{
	HANDLE DeviceHandle;
	BYTE EP4_Data[65] = { 0 };
	ULONG Result;
	DWORD wlen;//, block_size;

	if(USB_g_MyDevicePathName[0] == 0)
		return 1;

	DeviceHandle = CreateFile(USB_g_MyDevicePathName,
							  GENERIC_WRITE,
							  0,
							  ( LPSECURITY_ATTRIBUTES )NULL,
							  OPEN_EXISTING,
							  0,
							  NULL);

	if(DeviceHandle == INVALID_HANDLE_VALUE)
		return 2;

	memcpy(EP4_Data + 2, a_pbWriteData, len);

	Result = WriteFile(DeviceHandle, EP4_Data, USB_g_output_length, &wlen, NULL);
	CloseHandle(DeviceHandle);
	if((Result == FALSE) || (wlen != USB_g_output_length))
		return 4;
	/*
	for(i = 0; i < len; i += 64)
	{
		memset(EP4_Data, 0, sizeof(EP4_Data));
		block_size = len - i;
		if(block_size > 64)
			block_size = 64;
		memcpy(EP4_Data + 1, data + i, block_size);

		wlen = 0;
		Result = WriteFile(DeviceHandle, EP4_Data, USB_g_output_length, &wlen, 0);
		if((Result == FALSE) || (wlen != USB_g_output_length))
		{
			CloseHandle(DeviceHandle);
			return 3;
		}
	}

	CloseHandle(DeviceHandle);*/

	return 0;
}

int USB_In_EP3(BYTE *data, int len)
{
	HANDLE DeviceHandle;
	BYTE EP3_Data[100] = { 0 };
	ULONG Result;
	DWORD block_size, rlen;
	int i;

	if(USB_g_MyDevicePathName[0] == 0)
		return 1;

	DeviceHandle = CreateFile(USB_g_MyDevicePathName,
							  GENERIC_READ,
							  0,
							  ( LPSECURITY_ATTRIBUTES )NULL,
							  OPEN_EXISTING,
							  0,
							  NULL);

	if(DeviceHandle == INVALID_HANDLE_VALUE)
		return 2;

	for(i = 0; i < len; i += 64)
	{
		memset(EP3_Data, 0, sizeof(EP3_Data));
		block_size = len - i;
		if(block_size > 64)
			block_size = 64;

		rlen = 0;
		Result = ReadFile(DeviceHandle, EP3_Data, USB_g_input_length, &rlen, 0);
		if((Result == FALSE) || (rlen != USB_g_input_length))
		{
			CloseHandle(DeviceHandle);
			return 3;
		}

		memcpy(data + i, EP3_Data + 1, block_size);
	}

	CloseHandle(DeviceHandle);

	return 0;
}

//==Get KB Type===============================
DECLSPEC int __cdecl GRGBDLL_GetMEKBTYPE(void) {
	return GRGBDLL_g_bKBType;
}

//============================================
void GRGBDLL_DLL_THREAD_ATTACH()
{
	//GRGBDLL_My_debug_log("GRGBDLL_DLL_THREAD_ATTACH");
}

void GRGBDLL_DLL_THREAD_DETACH()
{
	//GRGBDLL_My_debug_log("GRGBDLL_DLL_THREAD_DETACH");
}

void GRGBDLL_TYPEStatic(void) {

	BYTE set_rpt[8] = { 0x16, 0x0 };
	BYTE *pbRunRowData = g_abStaticType;
	BYTE bRed = 0, bGreen = 0, bBlue = 0;
	int iRedIndex = 42, iGreenIndex = 21, iBlueIndex = 0;
	BYTE bTempID = 0, bColumnID = 0;
	BOOL bDireFlag = TRUE;
	BYTE bCount = GRGBDLL_g_bRow_Tempo;


	//GRGBDLL_g_bRow_Direction ==>//1---从左到右，2---从右到左，3---从下到上，4---从上到下
	if(DIR_UP2BOTTOM == GRGBDLL_g_bRow_Direction){
		iBlueIndex = 315;	iGreenIndex = 336;	iRedIndex = 357;
	}
	if(GRGBDLL_g_bRow_Direction == DIR_RIGHT2LEFT) {
		bColumnID = 19;

	}
	//initial Color=====================
	if(GRGBDLL_g_pbRow_Color->bKey) {
		bRed = GRGBDLL_g_pbRow_Color->bKRed;
		bGreen = GRGBDLL_g_pbRow_Color->bKGreen;
		bBlue = GRGBDLL_g_pbRow_Color->bkBlue;
	}

	do {
		if((!bCount--) && (GRGBDLL_g_bRow_Tempo != 0x0B) && bDireFlag) {
			bCount = GRGBDLL_g_bRow_Tempo;
			
			switch(GRGBDLL_g_bRow_Direction) {
			//case DIR_RIGHT2LEFT:
			case DIR_LEFT2RIGHT:
				//Index 1
				memset(g_abStaticType + 0 + bColumnID, bBlue, 2);		//Color B
				memset(g_abStaticType + 21 + bColumnID, bGreen, 2);		//Color G
				memset(g_abStaticType + 42 + bColumnID, bRed, 2);		//Color R

				//Index 2
				memset(g_abStaticType + 63 + bColumnID, bBlue, 2);		//Color B
				memset(g_abStaticType + 84 + bColumnID, bGreen, 2);		//Color G
				memset(g_abStaticType + 105 + bColumnID, bRed, 2);		//Color R

				//Index 3
				memset(g_abStaticType + 126 + bColumnID, bBlue, 2);		//Color B
				memset(g_abStaticType + 147 + bColumnID, bGreen, 2);	//Color G
				memset(g_abStaticType + 168 + bColumnID, bRed, 2);		//Color R

				//Index 4
				memset(g_abStaticType + 189 + bColumnID, bBlue, 2);		//Color B
				memset(g_abStaticType + 210 + bColumnID, bGreen, 2);	//Color G
				memset(g_abStaticType + 231 + bColumnID, bRed, 2);		//Color R


				//Index 5
				memset(g_abStaticType + 252 + bColumnID, bBlue, 2);		//Color B
				memset(g_abStaticType + 273 + bColumnID, bGreen, 2);	//Color G
				memset(g_abStaticType + 294 + bColumnID, bRed, 2);		//Color R

				//Index 6
				memset(g_abStaticType + 315 + bColumnID, bBlue, 2);		//Color B
				memset(g_abStaticType + 336 + bColumnID, bGreen, 2);	//Color G
				memset(g_abStaticType + 357 + bColumnID, bRed, 2);		//Color R
				//bCount = GRGBDLL_g_bRow_Tempo;

				if(++bColumnID == 20) {
					bColumnID = 0;
					bDireFlag = FALSE;
					/*== for Auto change Color========================
					if(bTempID++ == 2) bTempID = 0;
					if(!bTempID){
						bRed = (GRGBDLL_g_pbRow_Color + 2)->bKRed;
						bGreen = (GRGBDLL_g_pbRow_Color + 3)->bKGreen;
						bBlue = (GRGBDLL_g_pbRow_Color + 3)->bkBlue;a
					}
					//else bBlue = 0;

					if(bTempID == 1) {
						bRed = GRGBDLL_g_pbRow_Color->bKRed ;
						bGreen	= GRGBDLL_g_pbRow_Color->bKGreen;
						bBlue = GRGBDLL_g_pbRow_Color->bkBlue;
					}
					//else bGreen = 0;

					if(bTempID == 2) {
						bRed = (GRGBDLL_g_pbRow_Color+1)->bKRed;
						bGreen = (GRGBDLL_g_pbRow_Color + 1)->bKGreen;
						bBlue = (GRGBDLL_g_pbRow_Color + 1)->bkBlue;
					}
					//else bRed = 0;
					*/
				}
			break;

			case DIR_RIGHT2LEFT:
				//Index 1
				memset(g_abStaticType + 0 + bColumnID, bBlue, 2);		//Color B
				memset(g_abStaticType + 21 + bColumnID, bGreen, 2);		//Color G
				memset(g_abStaticType + 42 + bColumnID, bRed, 2);		//Color R

				//Index 2
				memset(g_abStaticType + 63 + bColumnID, bBlue, 2);		//Color B
				memset(g_abStaticType + 84 + bColumnID, bGreen, 2);		//Color G
				memset(g_abStaticType + 105 + bColumnID, bRed, 2);		//Color R

				//Index 3
				memset(g_abStaticType + 126 + bColumnID, bBlue, 2);		//Color B
				memset(g_abStaticType + 147 + bColumnID, bGreen, 2);	//Color G
				memset(g_abStaticType + 168 + bColumnID, bRed, 2);		//Color R

				//Index 4
				memset(g_abStaticType + 189 + bColumnID, bBlue, 2);		//Color B
				memset(g_abStaticType + 210 + bColumnID, bGreen, 2);	//Color G
				memset(g_abStaticType + 231 + bColumnID, bRed, 2);		//Color R

				//Index 5
				memset(g_abStaticType + 252 + bColumnID, bBlue, 2);		//Color B
				memset(g_abStaticType + 273 + bColumnID, bGreen, 2);	//Color G
				memset(g_abStaticType + 294 + bColumnID, bRed, 2);		//Color R

				//Index 6
				memset(g_abStaticType + 315 + bColumnID, bBlue, 2);		//Color B
				memset(g_abStaticType + 336 + bColumnID, bGreen, 2);	//Color G
				memset(g_abStaticType + 357 + bColumnID, bRed, 2);		//Color R
				//bCount = GRGBDLL_g_bRow_Tempo;

				if(bColumnID-- == 0) {
					bColumnID = 19;
					bDireFlag = FALSE;
					/*== for Auto change Color========================
					if(bTempID++ == 2) bTempID = 0;
					if(!bTempID) bBlue = 0xff;
					else bBlue = 0;

					if(bTempID == 1) bGreen = 0xff;
					else bGreen = 0;

					if(bTempID == 2) bRed = 0xff;
					else bRed = 0;
					*/
				}
			break;
			case DIR_UP2BOTTOM:
				memset(g_abStaticType + iBlueIndex, bBlue, 21);		//Color B
				memset(g_abStaticType + iGreenIndex, bGreen, 21);	//Color G
				memset(g_abStaticType + iRedIndex, bRed, 21);		//Color R
				//=========================================
				if(iBlueIndex >= 63) iBlueIndex -= 63;
				else {
					iBlueIndex = 315;
					bDireFlag = FALSE;
					/*== for Auto change Color========================
					if(bTempID++ == 2) bTempID = 0;
					if(!bTempID) {
						bBlue = 0xFF;
					}
					else bBlue = 0;
					*/
				}
				//=========================================
				if(iGreenIndex >= 84) iGreenIndex -= 63;
				else {
					iGreenIndex = 336;
					bDireFlag = FALSE;
					/*== for Auto change Color========================
					if(bTempID == 1) {
						bGreen = 0xFF;
					}
					else bGreen = 0;
					*/
				}
				//=========================================
				if(iRedIndex >= 105) iRedIndex -= 63;
				else {
					iRedIndex = 357;
					bDireFlag = FALSE;
					/*== for Auto change Color========================
					if(bTempID == 2) {
						bRed = 0xFF;
					}
					else bRed = 0;
					*/
				}
			break;
			//case DIR_UP2BOTTOM:
			case DIR_BOTTOM2UP:
				memset(g_abStaticType + iBlueIndex, bBlue, 21);		//Color B
				memset(g_abStaticType + iGreenIndex, bGreen, 21);	//Color G
				memset(g_abStaticType + iRedIndex, bRed, 21);		//Color R
				//=========================================
				if(iBlueIndex <= 252) iBlueIndex += 63;
				else {
					iBlueIndex = 0;
					bDireFlag = FALSE;
					/*== for Auto change Color========================
					if(bTempID++ == 2) bTempID = 0;
					if(!bTempID) {
						bBlue = 0xFF;
					}
					else bBlue = 0;
					*/
				}
				//=========================================
				if(iGreenIndex <= 273) iGreenIndex += 63;
				else {
					iGreenIndex = 21;
					bDireFlag = FALSE;
					/*== for Auto change Color========================
					if(bTempID == 1) {
						bGreen = 0xFF;
					}
					else bGreen = 0;
					*/
				}
				//=========================================
				if(iRedIndex <= 294) iRedIndex += 63;
				else {
					iRedIndex = 42;
					bDireFlag = FALSE;
					/*== for Auto change Color========================
					if(bTempID == 2) {
						bRed = 0xFF;
					}
					else bRed = 0;
					*/
				}
			break;

			}
		}
		pbRunRowData = g_abStaticType;

		for(int iIndex = 0; iIndex <= GRGBDLL_g_bRow_index; iIndex++) {
			set_rpt[2] = iIndex;
			set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);
		
			//GRGBDLL_My_Delay_Time(0.00085);
			GRGBDLL_WaitLightingState();
			if(USB_Set_Report_And_Write_EP4(set_rpt, 8, pbRunRowData, 63)) {
				//MessageBox(NULL, L"Polling Row Data Fail...", L"G_RGB_KB_DLL", MB_OK);
				return;
			}
			pbRunRowData += 63;
		}
	} while(GRGBDLL_g_bGoROWLoopFlag);

	if(GRGBDLL_g_pbRow_Color) {
		delete GRGBDLL_g_pbRow_Color;
		GRGBDLL_g_pbRow_Color = NULL;
	}

}

void GRGBDLL_TYPENero(void) {

	BYTE set_rpt[8] = { 0x16, 0x0 };
	BYTE *pbRunRowData = g_abStaticType;//GRGBDLL_g_pbROWDataPoint;
	BYTE bRed = 0, bGreen = 0, bBlue = 0;
	unsigned int iRGBData = 0;
	BOOL bDireFlag = FALSE, FullRowFlag = TRUE;
	BYTE bCount = GRGBDLL_g_bRow_Tempo;
	BYTE bIndex = 0;


	//==for initial Effect=========================================
	//iRGBData = bBlue;	iRGBData <<= 8;
	//iRGBData += bGreen;	iRGBData <<= 8;
	//iRGBData += bRed;
	bRed = (GRGBDLL_g_pbRow_Color + bIndex)->bKRed;
	bGreen = (GRGBDLL_g_pbRow_Color + bIndex)->bKGreen;
	bBlue = (GRGBDLL_g_pbRow_Color + bIndex)->bkBlue;

	do {
		for(int iIndex = 0; iIndex <= GRGBDLL_g_bRow_index; iIndex++) {
			set_rpt[2] = iIndex;
			set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);
			//GRGBDLL_My_Delay_Time(0.0008);
			GRGBDLL_WaitLightingState();
			if(USB_Set_Report_And_Write_EP4(set_rpt, 8, pbRunRowData, 63)) {
				//MessageBox(NULL, L"Polling Row Data Fail...", L"G_RGB_KB_DLL", MB_OK);
				return ;
			}
		}

		if((!bCount--) && (GRGBDLL_g_bRow_Tempo != 0x0B)) {
			//Index 1
			memset(g_abStaticType + 0, bBlue, 21);		//Color B
			memset(g_abStaticType + 21, bGreen, 21);	//Color G
			memset(g_abStaticType + 42, bRed, 21);		//Color R

			bCount = GRGBDLL_g_bRow_Tempo;
			if(bIndex == 6)bIndex = 0;
			else bIndex++;
			bRed = (GRGBDLL_g_pbRow_Color + bIndex)->bKRed;
			bGreen = (GRGBDLL_g_pbRow_Color + bIndex)->bKGreen;
			bBlue = (GRGBDLL_g_pbRow_Color + bIndex)->bkBlue;
			/*
			if(!bDireFlag) {
				if(iRGBData & 0x00FFFF) {
					bRed = iRGBData & 0xff;
					bGreen = (iRGBData >> 8) & 0xFF;
					bBlue = (iRGBData >> 16) & 0xFF;
					iRGBData <<= 1;
				}
				else bDireFlag = TRUE;
			}
			else {
				if(iRGBData & 0xFFFF00) {
					bRed = iRGBData & 0xff;
					bGreen = (iRGBData >> 8) & 0xFF;
					bBlue = (iRGBData >> 16) & 0xFF;
					iRGBData >>= 1;
				}
				else bDireFlag = FALSE;
			}
			*/
		}
		pbRunRowData = g_abStaticType;
	} while(GRGBDLL_g_bGoROWLoopFlag);
	
}

void GRGBDLL_TYPEImpact(void) {

	BYTE set_rpt[8] = { 0x16, 0x0 };
	BYTE *pbRunRowData = g_abImpactType;
	BYTE bRed = 0, bGreen = 0, bBlue = 0;
	BYTE bLRed = 0, bLGreen = 0, bLBlue = 0;
	BYTE bLift_RunCount = 0, bRight_RunCount = 19, bID = 0, boutside = 4;
	BOOL bL_plusFlag = TRUE, bR_lowerFlag = TRUE;
	BYTE bCount = GRGBDLL_g_bRow_Tempo;
	
	//==for Initial Color==========================================
	a_abInsideRunRGBTable[0][0] = GRGBDLL_g_pbRow_Color->bkBlue;
	a_abInsideRunRGBTable[0][1] = GRGBDLL_g_pbRow_Color->bKGreen;
	a_abInsideRunRGBTable[0][2] = GRGBDLL_g_pbRow_Color->bKRed;
	a_abInsideRunLRGBTable[0][0] = (GRGBDLL_g_pbRow_Color + 1)->bkBlue;
	a_abInsideRunLRGBTable[0][1] = (GRGBDLL_g_pbRow_Color + 1)->bKGreen;
	a_abInsideRunLRGBTable[0][2] = (GRGBDLL_g_pbRow_Color + 1)->bKRed;
	a_aboutsideRunRGBTable[0][0] = (GRGBDLL_g_pbRow_Color + 2)->bkBlue;
	a_aboutsideRunRGBTable[0][1] = (GRGBDLL_g_pbRow_Color + 2)->bKGreen;
	a_aboutsideRunRGBTable[0][2] = (GRGBDLL_g_pbRow_Color + 2)->bKRed;

	//==for initial Effect=========================================
	//設置左值
	memset(g_abImpactType + 0 + bLift_RunCount, bBlue, 2);				//Color B
	memset(g_abImpactType + 21 + bLift_RunCount, bGreen, 2);			//Color G
	memset(g_abImpactType + 42 + bLift_RunCount, bRed, 2);				//Color R

	//設置右值
	memset(g_abImpactType + 0 + bRight_RunCount, bBlue, 2);				//Color B
	memset(g_abImpactType + 21 + bRight_RunCount, bGreen, 2);			//Color G
	memset(g_abImpactType + 42 + bRight_RunCount, bRed, 2);				//Color R

	pbRunRowData = g_abImpactType;

	do {
		for(int iIndex = 0; iIndex <= GRGBDLL_g_bRow_index; iIndex++) {
			set_rpt[2] = iIndex;
			set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);
			//GRGBDLL_My_Delay_Time(0.0008);
			GRGBDLL_WaitLightingState();
			if(USB_Set_Report_And_Write_EP4(set_rpt, 8, pbRunRowData, 63)) {
				//MessageBox(NULL, L"Polling Row Data Fail...", L"G_RGB_KB_DLL", MB_OK);
				return ;
			}
			//pbRunRowData += 63;
		}

		if((!bCount--) && (GRGBDLL_g_bRow_Tempo != 0x0B)) {
			bCount = GRGBDLL_g_bRow_Tempo;
			if(bL_plusFlag) {
				bBlue = a_abInsideRunRGBTable[0][0];
				bGreen = a_abInsideRunRGBTable[0][1];
				bRed = a_abInsideRunRGBTable[0][2];

				bLBlue = a_abInsideRunLRGBTable[0][0];
				bLGreen = a_abInsideRunLRGBTable[0][1];
				bLRed = a_abInsideRunLRGBTable[0][2];

				//if(++boutside == 3) boutside = 0;
				//先歸左值為0
				memset(g_abImpactType + 0 + bLift_RunCount, 0x0, 1);				//Color B
				memset(g_abImpactType + 21 + bLift_RunCount, 0x0, 1);				//Color G
				memset(g_abImpactType + 42 + bLift_RunCount, 0x0, 1);				//Color R

				//先歸右值為0
				memset(g_abImpactType + 0 + bRight_RunCount + 1, 0x0, 1);				//Color B
				memset(g_abImpactType + 21 + bRight_RunCount + 1, 0x0, 1);				//Color G
				memset(g_abImpactType + 42 + bRight_RunCount + 1, 0x0, 1);				//Color R
			}
			else {

				//if(++bID == 3)bID = 0;
				//bBlue = 0x0;	bGreen = 0x0;		bRed = 0xFF;
				bLBlue = bBlue = a_aboutsideRunRGBTable[0][0];
				bLGreen = bGreen = a_aboutsideRunRGBTable[0][1];
				bLRed = bRed = a_aboutsideRunRGBTable[0][2];

				memset(g_abImpactType + 0 + bLift_RunCount + 1, 0x0, 1);				//Color B
				memset(g_abImpactType + 21 + bLift_RunCount + 1, 0x0, 1);				//Color G
				memset(g_abImpactType + 42 + bLift_RunCount + 1, 0x0, 1);				//Color R

				//先歸右值為0
				memset(g_abImpactType + 0 + bRight_RunCount, 0x0, 1);				//Color B
				memset(g_abImpactType + 21 + bRight_RunCount, 0x0, 1);				//Color G
				memset(g_abImpactType + 42 + bRight_RunCount, 0x0, 1);				//Color R
			}

			//進位,再設置
			if(bL_plusFlag) {
				if(++bLift_RunCount == 9)	bL_plusFlag = FALSE;
			}
			else {
				if(--bLift_RunCount == 0)	bL_plusFlag = TRUE;
			}

			if(bR_lowerFlag) {
				if(--bRight_RunCount == 10) bR_lowerFlag = FALSE;
			}
			else {
				if(++bRight_RunCount == 19) bR_lowerFlag = TRUE;
			}

			//設置左值
			memset(g_abImpactType + 0 + bLift_RunCount, bLBlue, 2);				//Color B
			memset(g_abImpactType + 21 + bLift_RunCount, bLGreen, 2);			//Color G
			memset(g_abImpactType + 42 + bLift_RunCount, bLRed, 2);				//Color R
			//設置右值
			memset(g_abImpactType + 0 + bRight_RunCount, bBlue, 2);				//Color B
			memset(g_abImpactType + 21 + bRight_RunCount, bGreen, 2);			//Color G
			memset(g_abImpactType + 42 + bRight_RunCount, bRed, 2);				//Color R
		}
		pbRunRowData = g_abImpactType;
	} while(GRGBDLL_g_bGoROWLoopFlag);

}

void GRGBDLL_TYPEArray(void) {
	BYTE set_rpt[8] = { 0x16, 0x0 };
	BYTE *pbRunRowData = g_abArrayType;
	BYTE bRed = 0, bGreen = 0, bBlue = 0;
	BYTE bLRed = 0, bLGreen = 0, bLBlue = 0;
	BYTE bID = 0;
	BYTE bArrayStepCount = 0;
	BOOL bDireFlag = FALSE, FullRowFlag = TRUE;
	BYTE bCount = GRGBDLL_g_bRow_Tempo;

	//==for Initial Color==========================================
	a_aArrayRunRGBTable[0][0] = GRGBDLL_g_pbRow_Color->bkBlue;
	a_aArrayRunRGBTable[0][1] = GRGBDLL_g_pbRow_Color->bKGreen;
	a_aArrayRunRGBTable[0][2] = GRGBDLL_g_pbRow_Color->bKRed;
	a_aArrayRunRGBTable[1][0] = (GRGBDLL_g_pbRow_Color + 1)->bkBlue;
	a_aArrayRunRGBTable[1][1] = (GRGBDLL_g_pbRow_Color + 1)->bKGreen;
	a_aArrayRunRGBTable[1][2] = (GRGBDLL_g_pbRow_Color + 1)->bKRed;
	a_aArrayRunRGBTable[2][0] = (GRGBDLL_g_pbRow_Color + 2)->bkBlue;
	a_aArrayRunRGBTable[2][1] = (GRGBDLL_g_pbRow_Color + 2)->bKGreen;
	a_aArrayRunRGBTable[2][2] = (GRGBDLL_g_pbRow_Color + 2)->bKRed;


	//==for initial Effect=========================================
	
	do {
		for(int iIndex = 0; iIndex <= GRGBDLL_g_bRow_index; iIndex++) {
			set_rpt[2] = iIndex;
			set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);
			//GRGBDLL_My_Delay_Time(0.0008);
			GRGBDLL_WaitLightingState();
			if(USB_Set_Report_And_Write_EP4(set_rpt, 8, pbRunRowData, 63)) {
				//MessageBox(NULL, L"Polling Row Data Fail...", L"G_RGB_KB_DLL", MB_OK);
				return ;
			}
			pbRunRowData += 63;
		}

		if((!bCount--) && (GRGBDLL_g_bRow_Tempo != 0x0B)) {
			bCount = GRGBDLL_g_bRow_Tempo;
			if(bArrayStepCount && (bArrayStepCount != 14)) {
				//if(bArrayStepCount == 14) break;
				for(int i = 0; i <= (g_bArrayStep[0][bArrayStepCount] - 1); i++) {
					memset(g_abArrayType + ((g_iKeyBGRID[g_bArrayStep[bArrayStepCount][i] - 1][0])), 0x0, 1);				//Color B
					memset(g_abArrayType + ((g_iKeyBGRID[g_bArrayStep[bArrayStepCount][i] - 1][1])), 0x0, 1);				//Color G
					memset(g_abArrayType + ((g_iKeyBGRID[g_bArrayStep[bArrayStepCount][i] - 1][2])), 0x0, 1);				//Color R
				}
			}
			//讀取Step Count數
			if(++bArrayStepCount > g_bArrayStep[0][0]) bArrayStepCount = 1;
			//利用Step Count數，來取得每一個Step Size
			for(int i = 0; i <= (g_bArrayStep[0][bArrayStepCount] - 1); i++) {
				memset(g_abArrayType + ((g_iKeyBGRID[g_bArrayStep[bArrayStepCount][i] - 1][0])), bBlue, 1);					//Color B
				memset(g_abArrayType + ((g_iKeyBGRID[g_bArrayStep[bArrayStepCount][i] - 1][1])), bGreen, 1);				//Color G
				memset(g_abArrayType + ((g_iKeyBGRID[g_bArrayStep[bArrayStepCount][i] - 1][2])), bRed, 1);					//Color R
			}

			//改變顏色
			if(bArrayStepCount == 14) {
				bBlue = a_aArrayRunRGBTable[bID][0];
				bGreen = a_aArrayRunRGBTable[bID][1];
				bRed = a_aArrayRunRGBTable[bID][2];
				if(++bID == 3)bID = 0;
			}
		}
		pbRunRowData = g_abArrayType;
	} while(GRGBDLL_g_bGoROWLoopFlag);

	
}

void GRGBDLL_TYPERotary(void) {
	BYTE set_rpt[8] = { 0x16, 0x0 };
	BYTE *pbRunRowData = g_abRotaryType;
	BYTE bCount = GRGBDLL_g_bRow_Tempo;
	//==for Rotary mode===================
	BYTE *pbRotaryBuffer = NULL;
	BYTE *pbRotaryBufferTemp = NULL;
	//以126key board為主，分為三圈。
	//每繞半圈時(以外圈來算跑25Key)需以最外圈需先shift 4中圈則再shift 4，內圈再同步移動。
	//到跑25次後需再重置"abShiftBitSet"
	BYTE abShiftBitSet[25] = { 6,6,6,6,4,4,4,4,0 };
	BYTE bTriggerBit = 0;
	BYTE bSLC = 0; //shift loop count
	BYTE bRotaryMaxSLC = 25;


	//==for initial Effect=========================================
	pbRotaryBufferTemp = new BYTE[378];
	memcpy(pbRotaryBufferTemp, g_abRotaryType, 378);

	do {
		//MessageBox(NULL, L"444...", L"G_RGB_KB_DLL", MB_OK);
		for(int iIndex = 0; iIndex <= GRGBDLL_g_bRow_index; iIndex++) {
			set_rpt[2] = iIndex;
			set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);

			//GRGBDLL_My_Delay_Time(0.0008);
			GRGBDLL_WaitLightingState();
			if(USB_Set_Report_And_Write_EP4(set_rpt, 8, pbRunRowData, 63)) {
				//MessageBox(NULL, L"Polling Row Data Fail...", L"G_RGB_KB_DLL", MB_OK);
				return;
			}
			pbRunRowData += 63;
		}

		if((!bCount--) && (GRGBDLL_g_bRow_Tempo != 0x0B)) {
			bCount = GRGBDLL_g_bRow_Tempo;
			if(pbRotaryBuffer) delete(pbRotaryBuffer);
			pbRotaryBuffer = NULL;
			pbRotaryBuffer = new BYTE[378];

			//==由上而下====
			bTriggerBit = abShiftBitSet[bSLC];
			for(int ix = 20, iy = 104; ix <= 60; ix += 20, iy -= 22) {
				for(int x = ix; x <= iy; x += 21) {
					pbRotaryBuffer[g_iKeyBGRID[x + 21][0]] = pbRotaryBufferTemp[g_iKeyBGRID[x + (bTriggerBit & 0x1) * 21][0]];
					pbRotaryBuffer[g_iKeyBGRID[x + 21][1]] = pbRotaryBufferTemp[g_iKeyBGRID[x + (bTriggerBit & 0x1) * 21][1]];
					pbRotaryBuffer[g_iKeyBGRID[x + 21][2]] = pbRotaryBufferTemp[g_iKeyBGRID[x + (bTriggerBit & 0x1) * 21][2]];
				}
				bTriggerBit >>= 1;
			}
			//==由下而上====
			bTriggerBit = abShiftBitSet[bSLC];
			for(int ix = 105, iy = 21; ix >= 65; ix -= 20, iy += 22) {
				for(int x = ix; x >= iy; x -= 21) {
					pbRotaryBuffer[g_iKeyBGRID[x - 21][0]] = pbRotaryBufferTemp[g_iKeyBGRID[x - (bTriggerBit & 0x1) * 21][0]];
					pbRotaryBuffer[g_iKeyBGRID[x - 21][1]] = pbRotaryBufferTemp[g_iKeyBGRID[x - (bTriggerBit & 0x1) * 21][1]];
					pbRotaryBuffer[g_iKeyBGRID[x - 21][2]] = pbRotaryBufferTemp[g_iKeyBGRID[x - (bTriggerBit & 0x1) * 21][2]];
				}
				bTriggerBit >>= 1;
			}

			//==由右而左====
			bTriggerBit = abShiftBitSet[bSLC];
			for(int ix = 0, iy = 19; ix <= 59; ix += 22, iy += 20) {
				for(int x = ix; x <= iy; x += 1) {
					pbRotaryBuffer[g_iKeyBGRID[x + 1][0]] = pbRotaryBufferTemp[g_iKeyBGRID[x + (bTriggerBit & 0x1) * 1][0]];
					pbRotaryBuffer[g_iKeyBGRID[x + 1][1]] = pbRotaryBufferTemp[g_iKeyBGRID[x + (bTriggerBit & 0x1) * 1][1]];
					pbRotaryBuffer[g_iKeyBGRID[x + 1][2]] = pbRotaryBufferTemp[g_iKeyBGRID[x + (bTriggerBit & 0x1) * 1][2]];
				}
				bTriggerBit >>= 1;
			}
			//==由左而右====
			bTriggerBit = abShiftBitSet[bSLC];
			for(int ix = 125, iy = 106; ix >= 66; ix -= 22, iy -= 20) {
				for(int x = ix; x >= iy; x -= 1) {
					pbRotaryBuffer[g_iKeyBGRID[x - 1][0]] = pbRotaryBufferTemp[g_iKeyBGRID[x - (bTriggerBit & 0x1) * 1][0]];
					pbRotaryBuffer[g_iKeyBGRID[x - 1][1]] = pbRotaryBufferTemp[g_iKeyBGRID[x - (bTriggerBit & 0x1) * 1][1]];
					pbRotaryBuffer[g_iKeyBGRID[x - 1][2]] = pbRotaryBufferTemp[g_iKeyBGRID[x - (bTriggerBit & 0x1) * 1][2]];
				}
				bTriggerBit >>= 1;
			}
			memcpy(pbRotaryBufferTemp, pbRotaryBuffer, 378);
			if(++bSLC >= bRotaryMaxSLC) bSLC = 0;
		}
		pbRunRowData = pbRotaryBufferTemp;
	} while(GRGBDLL_g_bGoROWLoopFlag);
}

/*
DWORD WINAPI GRGBDLL_OEMSVC_Thread(PVOID pParam)
{
	//--for test --------------------
	char *cStr[11] = {
		{ "OemServiceWinApp.exe" },
		{ "ledkb"},
		{ "/setdata" },
		{ "08" },
		{ "03" },
		{ "02" },
		{ "05" },
		{ "02" },
		{ "02" },
		{ "01" },
		{ "00" }
	};
	int iTemp[256] = {0x0};
	TCHAR *cp = NULL;
	cp = ( TCHAR* )LocalAlloc(LPTR,MAX_PATH * sizeof(TCHAR));
	//-------------------------------
	GRGBDLL_g_hOEMSVCInstLib = LoadLibrary(L"OemServiceWinApp.dll");
	if(GRGBDLL_g_hOEMSVCInstLib) {
		OemSvcHook OemSvc_Hook = ( OemSvcHook )GetProcAddress(GRGBDLL_g_hOEMSVCInstLib, "OemSvcHook");
		if(OemSvc_Hook) {
			
			OemSvc_Hook(11, *cStr, iTemp, 256);

			OemSvc_Hook = NULL;
			GRGBDLL_CloseOEMSVCThreadHandle();
		}
		else {
			MessageBox(NULL, L"Attach OemSvcHook fail !", L"G_RGB_KB_DLL", MB_OK);
			return GRGBDLL_NG_OEMSVC_THREAD_FALL;
		}
	 }
	else {
		MessageBox(NULL, L"Load OemServiceWinApp.dll fail !", L"G_RGB_KB_DLL ", MB_OK);
		return GRGBDLL_NG_OEMSVC_THREAD_FALL;
	 }
	return GRGBDLL_SUCCESS;
}
*/

DWORD WINAPI GRGBDLL_Polling_RowThread(PVOID pParam)
{
	switch(GRGBDLL_g_bRow_Effect) {
	case AP_EFF_STATIC_TYPE:
		GRGBDLL_TYPEStatic();
	break;
	case AP_EFF_RAINBOW_TYPE:
		//GRGBDLL_TYPERainbow();
	break;
	case AP_EFF_IMPACT:
		GRGBDLL_TYPEImpact();
	break;
	case AP_EFF_ARRAY_TYPE:
		GRGBDLL_TYPEArray();
	break;
	case AP_EFF_AUDIO_TYPE:
		//GRGBDLL_TYPEAudio();
		//GRGBDLL_TYPEAudio3();
	break;
	case AP_EFF_USER_TYPE:
		GRGBDLL_TYPERotary();
	break;
	case AP_EFF_NERO_TYPE:
		GRGBDLL_TYPENero();
		break;
	case AP_EFF_AUDIO_TYPE3:
		//GRGBDLL_TYPEAudio3();
		break;
	default:
	return GRGBDLL_INITIAL_STATE_FAIL;
	}

	return GRGBDLL_SUCCESS;
}


void GRGBDLL_RowCloseHandle() {
	if(GRGBDLL_g_hRowThread) {
		GRGBDLL_g_bGoROWLoopFlag = FALSE;
		WaitForSingleObject(GRGBDLL_g_hRowThread, INFINITE);
		CloseHandle(GRGBDLL_g_hRowThread);
		GRGBDLL_g_hRowThread = NULL;
		//GRGBDLL_g_pbROWDataPoint = NULL;
	}
}
/*
void GRGBDLL_CloseOEMSVCThreadHandle() {
	//GRGBDLL_g_hOEMSVCThread
	if(GRGBDLL_g_hOEMSVCThread) {
		GRGBDLL_g_bGoROWLoopFlag = FALSE;
		WaitForSingleObject(GRGBDLL_g_hOEMSVCThread, INFINITE);
		CloseHandle(GRGBDLL_g_hOEMSVCThread);
		GRGBDLL_g_hOEMSVCThread = NULL;
		//GRGBDLL_g_pbROWDataPoint = NULL;
	}
}
*/
//=====================================================================================
//=====================================================================================
DECLSPEC int __cdecl GRGBDLL_InitialDLL(void)
{
	//INTPUT_USER_KEY_SET sttaUserKeySet_Tmp[126] = {0};
	//GRGBDLL_g_sttCMDBuffer = new(INPUT_BUFFER_CMD);
	//==for Global varible============================
	GRGBDLL_g_bInitialStatus = FALSE;
	GRGBDLL_g_iHID_VendorID = 0;
	GRGBDLL_g_iHID_ProductID = 0;
	//--for Row Type----------
	GRGBDLL_g_hRowThread = NULL;
	//GRGBDLL_g_pbROWDataPoint = NULL;
	GRGBDLL_g_bGoROWLoopFlag = FALSE;
	GRGBDLL_g_bRow_index = 0;
	//--for key command-------
	GRGBDLL_g_psttCMDBuffer = NULL;
	//--for OEMSvc Thread---------------------------
	//GRGBDLL_g_hOEMSVCThread = NULL;
	//GRGBDLL_g_hOEMSVCInstLib = NULL;
	//== for Other variable=========================
	USB_g_feature_length = 0;
	USB_g_output_length = 0;
	USB_g_input_length = 0;

	GRGBDLL_g_iHID_VendorID = _VID_;
	GRGBDLL_g_iHID_ProductID = _PID_;

	//==========
	GRGBDLL_g_pbAudio3Data = abTemp;
	//==by Test for 14H Color
	if(GRGBDLL_g__STpColorTemp){
		delete GRGBDLL_g__STpColorTemp;
		GRGBDLL_g__STpColorTemp = NULL;
	}
	GRGBDLL_g__STpColorTemp = new INTPUT_USER_KEY_SET[7];

	//hinstLib = LoadLibrary(L"GRGBKB_DLLTest.dll");
	//if(hinstLib)
	//{
	//}
	//else
	//{
		//MessageBox(NULL, L"Load GRGBKB_DLL.dll fail !", L"Demo GRGB AP", MB_OK);
	//	MessageBox(NULL, L"Load GRGBKB_DLL.dll fail !", L"Demo GRGB AP", MB_OK);
	//}
	if(!GRGBDLL_USB_FindHID(GRGBDLL_g_iHID_VendorID, GRGBDLL_g_iHID_ProductID)) {
		return GRGBDLL_NG_NO_DEVICE;
	}
	//GRGBDLL_My_debug_log("GRGBDLL_InitialDLL");
	return GRGBDLL_g_bInitialStatus = TRUE;
}

DECLSPEC void __cdecl GRGBDLL_DestroyDLL(void)
{
	//==for HID Device Name=========================
	GRGBDLL_RowCloseHandle();
	if(GRGBDLL_g_pbRow_Color) {
		delete GRGBDLL_g_pbRow_Color;
		GRGBDLL_g_pbRow_Color = NULL;
	}
	if(GRGBDLL_g_psttCMDBuffer) GRGBDLL_g_psttCMDBuffer = NULL;
	//GRGBDLL_My_debug_log("GRGBDLL_DestroyDLL");
}

//DECLSPEC int __cdecl GRGBDLL_SetWelcomemode()

void GRGBDLL_WaitLightingState()
{
	for(int icount = 0; GRGBDLL_g_bGetLightingFlag == TRUE; icount++) {
		Sleep(10);
		if(icount >= 5) {
			GRGBDLL_g_bGetLightingFlag = TRUE;
			break;
		}
	}

}

DECLSPEC int __cdecl GRGBDLL_GetBacklight(void)
{
	int ret;
	BYTE set_rpt[8] = { 0x88,0 };
	BYTE get_rpt[8] = { 0 };
	set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);
	GRGBDLL_g_bGetLightingFlag = TRUE;
	USB_Set_Report(set_rpt, 8);
	ret = USB_Get_Report(get_rpt, 8);
	GRGBDLL_g_bGetLightingFlag = FALSE;
	if(ret != 0)
	{
		return GRGBDLL_INITIAL_STATE_FAIL;
	}

	return get_rpt[4];
}
DECLSPEC int __cdecl GRGBDLL_SetBacklight(BYTE a_bLigthValue)
{
	int ret;
	BYTE set_rpt[8] = { 0x88,0 };
	BYTE get_rpt[8] = { 0 };
	set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);

	//ret = USB_IO_Report(set_rpt, 8, get_rpt, 8);
	USB_Set_Report(set_rpt, 8);
	ret= USB_Get_Report(get_rpt, 8);

	if(ret != 0)
	{
		return GRGBDLL_INITIAL_STATE_FAIL;
	}
	set_rpt[0] = 0x08;
	set_rpt[1] = LED_CTRL_RUNTIME_INDEX;
	set_rpt[2] = get_rpt[2];
	set_rpt[3] = get_rpt[3];
	set_rpt[4] = a_bLigthValue;
	set_rpt[5] = get_rpt[5];
	set_rpt[6] = get_rpt[6];
	set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);

	USB_Set_Report(set_rpt, 8);
	if(ret != 0)
	{
		return GRGBDLL_INITIAL_STATE_FAIL;
	}

	return GRGBDLL_SUCCESS;
}

DECLSPEC int __cdecl GRGBDLL_iSetWelColor_14H(PINTPUT_USER_KEY_SET a_pTSColorValue)
{
	DWORD suspend_retval = static_cast< DWORD >(-1);
	//==
	//if((GRGBDLL_g_bRedTemp == a_pTSColorValue->bKRed) && (GRGBDLL_g_bRedTemp == a_pTSColorValue->bKGreen) && (GRGBDLL_g_bRedTemp == a_pTSColorValue->bkBlue)  )
	//return GRGBDLL_SUCCESS;
	for(int iTemp = 1; iTemp <= 7; iTemp++){
		if((GRGBDLL_g__STpColorTemp + iTemp)->bKRed != (a_pTSColorValue + iTemp)->bKRed)
			break;
		else if ((GRGBDLL_g__STpColorTemp + iTemp)->bKGreen != (a_pTSColorValue + iTemp)->bKGreen)
			break;
		else if((GRGBDLL_g__STpColorTemp + iTemp)->bkBlue != (a_pTSColorValue + iTemp)->bkBlue)
			break;

		if(iTemp == 7) return GRGBDLL_SUCCESS;
	}
	//==suspend============================
	if(GRGBDLL_g_hRowThread){
		for(int i = 0; (i < 10)&&(suspend_retval == static_cast< DWORD >(-1)); i++) {
			suspend_retval = SuspendThread(GRGBDLL_g_hRowThread);
		}
		//if(suspend_retval != static_cast< DWORD >(-1)) {
		//	DWORD dwId;
		//	GRGBDLL_CloseOEMSVCThreadHandle();
		//	GRGBDLL_g_hOEMSVCThread = CreateThread(NULL, 0, GRGBDLL_Polling_RowThread, 0, 0, &dwId);
		//}
		//suspend_retval = SuspendThread(GRGBDLL_g_hRowThread);
		//if(suspend_retval == static_cast<DWORD>(-1))
		//{
			//MessageBox(NULL, L"Suspend_RowType-Error", L"Set14H", MB_OK);
		//}
	}
	for(int iIndex = 1; iIndex <= 7; iIndex++) {
		//if(iIndex == 8) continue;
		if((a_pTSColorValue + iIndex)->bKey){
			if(GRGBDLL_g_bKBType == _4ZONE_ME_KB){
				//if (((a_pTSColorValue + iIndex)->bKRed) > 0xc6)		((a_pTSColorValue + iIndex)->bKRed) = 0xc6;
				if (((a_pTSColorValue + iIndex)->bKGreen) > 0xc6)	((a_pTSColorValue + iIndex)->bKGreen) = 0xc6;
				if (((a_pTSColorValue + iIndex)->bkBlue) > 0xc6)	((a_pTSColorValue + iIndex)->bkBlue) = 0xc6;
			}
			GRGBDLL_Set_Color(iIndex + 8, (a_pTSColorValue + iIndex)->bKRed, (a_pTSColorValue + iIndex)->bKGreen, (a_pTSColorValue + iIndex)->bkBlue, TRUE);
		}
	}
	//for Save Color Setting
	GRGBDLL_SET_LED_Type(LED_CTRL_WELCOME_INDEX, 0x33, 0x01,
						 GRGBDLL_GetBacklight(), 0x01, 0x01, 0x0);
	//==Delay
	//GRGBDLL_My_Delay_Time(0.5);
	//==
	if(GRGBDLL_g_hRowThread && (suspend_retval != static_cast<DWORD>(-1))) {
		DWORD resume_retval = ResumeThread(GRGBDLL_g_hRowThread);
		if(resume_retval == static_cast<DWORD>(-1))
		{
			//MessageBox(NULL, L"Resume_RowType-Error", L"Set14H", MB_OK);
		}
	}

	for(int iTemp = 1; iTemp <= 7; iTemp++) {
		(GRGBDLL_g__STpColorTemp + iTemp)->bKRed = (a_pTSColorValue + iTemp)->bKRed;
		(GRGBDLL_g__STpColorTemp + iTemp)->bKGreen = (a_pTSColorValue + iTemp)->bKGreen;
		(GRGBDLL_g__STpColorTemp + iTemp)->bkBlue = (a_pTSColorValue + iTemp)->bkBlue;
	}

	return GRGBDLL_SUCCESS;
}


DECLSPEC int __cdecl GRGBDLL_Capture4ZONEBuffer(PINPUT_4ZONE_BUFFER_CMD a_pCMDBuffer)
{
	wchar_t  Type[256] = { 0x0 };
	char cType[128] = { 0x0 };
	int iRet = 0;
	BYTE bLEDTypeColorIndex = 0;
	BYTE b08HCtrlBYTE = LED_CTRL_RUNTIME_INDEX;

	if(!GRGBDLL_g_bInitialStatus) {
		return GRGBDLL_INITIAL_STATE_FAIL;
	}
	if(a_pCMDBuffer == NULL) {
		return GRGBDLL_CMDBUFFER_FAIL;
	}
	//==close Row Type============================
	GRGBDLL_RowCloseHandle();
	//==判斷mode以決定Color index=============
	//Runtime mode  = 0, Welcome mode = 1
	if(a_pCMDBuffer->bMode) {
		//==設定Color mode (14H)=============
		//set color index0~6;
		for(int iIndex = 9; iIndex <= 15; iIndex++) {
			if((a_pCMDBuffer->_TSaRGB + iIndex)->bKey)
				GRGBDLL_Set_Color(iIndex, (a_pCMDBuffer->_TSaRGB + iIndex)->bKRed, (a_pCMDBuffer->_TSaRGB + iIndex)->bKGreen, (a_pCMDBuffer->_TSaRGB + iIndex)->bkBlue, TRUE);
		}
	}
	else {
		//==設定Color mode (14H)=============
		//set color index0~6;
		for(int iIndex = 1; iIndex <= 7; iIndex++) {
			if((a_pCMDBuffer->_TSaRGB + iIndex)->bKey)
				//if (((a_pCMDBuffer->_TSaRGB + iIndex)->bKRed) > 0xc6)		((a_pCMDBuffer->_TSaRGB + iIndex)->bKRed) = 0xc6;
				if (((a_pCMDBuffer->_TSaRGB + iIndex)->bKGreen) > 0xc6)	((a_pCMDBuffer->_TSaRGB + iIndex)->bKGreen) = 0xc6;
				if (((a_pCMDBuffer->_TSaRGB + iIndex)->bkBlue) > 0xc6)	((a_pCMDBuffer->_TSaRGB + iIndex)->bkBlue) = 0xc6;
				GRGBDLL_Set_Color(iIndex, (a_pCMDBuffer->_TSaRGB + iIndex)->bKRed, (a_pCMDBuffer->_TSaRGB + iIndex)->bKGreen, (a_pCMDBuffer->_TSaRGB + iIndex)->bkBlue, FALSE);
		}
	}
	if(a_pCMDBuffer->_TSLedType_08H.bEffectType) {
		if(a_pCMDBuffer->_TSLedType_08H.bdirection > 4)a_pCMDBuffer->_TSLedType_08H.bdirection = 1;
		if(a_pCMDBuffer->_TSLedType_08H.bSpeed > 0x0B)a_pCMDBuffer->_TSLedType_08H.bSpeed = 0x0B;
		if(a_pCMDBuffer->_TSLedType_08H.blight > 0x32)a_pCMDBuffer->_TSLedType_08H.blight = 0x32;
		//if(!a_pCMDBuffer->_TSLedType_08H.bColor) bLEDTypeColorIndex = LED_COLOR_IN_TURN_INDEX;
		//else bLEDTypeColorIndex = LED_COLOR_RUNTIME_INDEX;

		iRet = GRGBDLL_SET_LED_Type(b08HCtrlBYTE,a_pCMDBuffer->_TSLedType_08H.bEffectType, a_pCMDBuffer->_TSLedType_08H.bSpeed,
									a_pCMDBuffer->_TSLedType_08H.blight, a_pCMDBuffer->_TSLedType_08H.bColor, a_pCMDBuffer->_TSLedType_08H.bdirection, a_pCMDBuffer->_TSLedType_08H.bsavingMode);
	}
	return GRGBDLL_SUCCESS;
}


DECLSPEC int __cdecl GRGBDLL_CaptureCMDBuffer(PINPUT_BUFFER_CMD a_pCMDBuffer)
{
	wchar_t  Type[256] = { 0x0 };
	char cType[128] = { 0x0 };
	int iRet = 0;
	BYTE bLEDTypeColorIndex = 0;
	BYTE b08HCtrlBYTE = LED_CTRL_RUNTIME_INDEX;

	if(!GRGBDLL_g_bInitialStatus) {
		//MessageBox(NULL, L"The initial state of the DLL is not established.\n First, the GRGBDLL_Initial DLL function is executed",
		//		   L"G_RGB_KB_DLL", MB_OK);
		return GRGBDLL_INITIAL_STATE_FAIL;
	}
	if(a_pCMDBuffer == NULL){
		return GRGBDLL_CMDBUFFER_FAIL;
	}
	//==close Row Type============================
	GRGBDLL_RowCloseHandle();

	GRGBDLL_g_psttCMDBuffer = a_pCMDBuffer;

	//==for Sleep Mode============================
	//if(GRGBDLL_g_psttCMDBuffer->bSleepMode) {
	//	b08HCtrlBYTE = LED_CTRL_LIGHTOFF_INDEX;
	//}


	//==判斷mode以決定Color index=============
	//Runtime mode  = 0, Welcome mode = 1
	
	//if(GRGBDLL_g_psttCMDBuffer->bMode){
		//==設定Color mode (14H)=============
		//set color index0~6;
	//	for(int iIndex = 1; iIndex <= 15; iIndex++) {
	//		if(iIndex == 8) continue;
	//		if((GRGBDLL_g_psttCMDBuffer->sttaRGB + iIndex)->bKey)
	//			GRGBDLL_Set_Color(iIndex, (GRGBDLL_g_psttCMDBuffer->sttaRGB + iIndex)->bKRed, (GRGBDLL_g_psttCMDBuffer->sttaRGB + iIndex)->bKGreen, (GRGBDLL_g_psttCMDBuffer->sttaRGB + iIndex)->bkBlue,TRUE);
	//	}
		//return GRGBDLL_SUCCESS;
	//}
	//else{
		//MessageBox(NULL, L"GRGBDLL_g_psttCMDBuffer->abRGB[0] == 1",  L"G_RGB_KB_DLL", MB_OK);
		//swprintf(Type, 256,L"bKey = %02d, bKRed = %02d, bKGreen = %02d, bkBlue = %02d\n", 
		//		 GRGBDLL_g_psttCMDBuffer->sttaRGB->bKey, GRGBDLL_g_psttCMDBuffer->sttaRGB->bKRed, GRGBDLL_g_psttCMDBuffer->sttaRGB->bKGreen, GRGBDLL_g_psttCMDBuffer->sttaRGB->bkBlue);

		//MessageBox(NULL, Type, L"G_RGB_KB_DLL", MB_OK);
		//==設定Color mode (14H)=============
		//set color index0~6;
		for(int iIndex= 1; iIndex <= 15; iIndex++){
			if(iIndex == 8) continue;
			if((GRGBDLL_g_psttCMDBuffer->sttaRGB + iIndex)->bKey)
				GRGBDLL_Set_Color(iIndex, (GRGBDLL_g_psttCMDBuffer->sttaRGB + iIndex)->bKRed, (GRGBDLL_g_psttCMDBuffer->sttaRGB + iIndex)->bKGreen, (GRGBDLL_g_psttCMDBuffer->sttaRGB + iIndex)->bkBlue,FALSE);
		}
	//}
	
	//==判斷Effect Type,以決定是MCU(08H)或AP(16H)==============
	//若是AP Effect,則需先由08H下light(目前AP Effect未支援color/Directoin)
	if(GRGBDLL_g_psttCMDBuffer->bType >= 0x20) {
		//swprintf(Type, 256, L"Type = %02d\n", GRGBDLL_g_psttCMDBuffer->bType);
		//sprintf_s(cType, 128, "   cType:    %d\n", GRGBDLL_g_psttCMDBuffer->bType);
		//MessageBox(NULL, Type, L"G_RGB_KB_DLL", MB_OK);

		//
		if (!GRGBDLL_g_psttCMDBuffer->bMode)
		int ret = GRGBDLL_SET_LED_Type(b08HCtrlBYTE, LED_EFFECT_USER_DEFINE, GRGBDLL_g_psttCMDBuffer->bTempo,
							 GRGBDLL_g_psttCMDBuffer->blight, LED_COLOR_RUNTIME_INDEX, GRGBDLL_g_psttCMDBuffer->bdirection, GRGBDLL_g_psttCMDBuffer->bsavingMode);
		//if(GRGBDLL_g_psttCMDBuffer->bType == AP_EFF_AUDIO_TYPE) return GRGBDLL_AUDIO_STATUS;
		//sprintf_s(cType, 128, "   Ret:    %d\n", ret);
		//GRGBDLL_My_debug_log(cType);
		if(GRGBDLL_g_psttCMDBuffer->bType == 0x25) {
			//執行AP Effect (12H)====================
			//add by hans for Welcome mode Setting 12H Keep Status ,
			//at 20170619
			if (GRGBDLL_g_psttCMDBuffer->bMode) GRGBDLL_g_psttCMDBuffer->bsavingMode = 1;
			if (GRGBDLL_g_psttCMDBuffer->bMode) b08HCtrlBYTE = LED_CTRL_WELCOME_INDEX;
			GRGBDLL_Set_User_Picture(0, GRGBDLL_g_psttCMDBuffer->bsavingMode, GRGBDLL_g_psttCMDBuffer->bKeySize, b08HCtrlBYTE, GRGBDLL_g_psttCMDBuffer->sttaUserKeySet);
		}
		else{
			//執行AP Effect (16H)====================
			GRGBDLL_Set_Row_LED(TRUE, GRGBDLL_g_psttCMDBuffer->bType, GRGBDLL_g_psttCMDBuffer->bTempo, GRGBDLL_g_psttCMDBuffer->sttaRGB+1, GRGBDLL_g_psttCMDBuffer->bdirection);
		}
	}
	else{			
		if(GRGBDLL_g_psttCMDBuffer->bType) {
			//MessageBox(NULL, L"GRGBDLL_g_psttCMDBuffer->bType <= 0x20", L"G_RGB_KB_DLL", MB_OK);
			//swprintf(Type, 256,L"Type = %02d, bTempo = %02d, blight = %02d, bdirection = %02d\n", 
			//		GRGBDLL_g_psttCMDBuffer->bType, GRGBDLL_g_psttCMDBuffer->bTempo, GRGBDLL_g_psttCMDBuffer->blight, GRGBDLL_g_psttCMDBuffer->bdirection);

			//MessageBox(NULL, Type, L"G_RGB_KB_DLL", MB_OK);

			if(GRGBDLL_g_psttCMDBuffer->bdirection > 5)GRGBDLL_g_psttCMDBuffer->bdirection = 1;
			if(GRGBDLL_g_psttCMDBuffer->bTempo > 0x0B)GRGBDLL_g_psttCMDBuffer->bTempo = 0x0B;
			if(GRGBDLL_g_psttCMDBuffer->blight > 0x32)GRGBDLL_g_psttCMDBuffer->blight = 0x32;
			if(!GRGBDLL_g_psttCMDBuffer->bColorInTurn) bLEDTypeColorIndex = LED_COLOR_IN_TURN_INDEX;
			else bLEDTypeColorIndex = LED_COLOR_RUNTIME_INDEX;

			//20170125 for on Key press function
			if((GRGBDLL_g_psttCMDBuffer->bType == 0x04) || (GRGBDLL_g_psttCMDBuffer->bType == 0x06) ||
				(GRGBDLL_g_psttCMDBuffer->bType == 0x0E) || (GRGBDLL_g_psttCMDBuffer->bType == 0x11)) {
				if(GRGBDLL_g_psttCMDBuffer->bdirection) {
					HWND hWnd = ::FindWindow(NULL, SERVICE_NAME);
					GRGBDLL_g_hInst = ( HINSTANCE )GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
					if(GRGBDLL_g_kholl == NULL)
					{
						GRGBDLL_g_kholl = SetWindowsHookEx(WH_KEYBOARD_LL, ( HOOKPROC )GRGBDLL_KeyboardHookProc, GRGBDLL_g_hInst, 0);
					}
				}
			}

			iRet = GRGBDLL_SET_LED_Type(b08HCtrlBYTE, GRGBDLL_g_psttCMDBuffer->bType, GRGBDLL_g_psttCMDBuffer->bTempo,
								GRGBDLL_g_psttCMDBuffer->blight, bLEDTypeColorIndex, GRGBDLL_g_psttCMDBuffer->bdirection, GRGBDLL_g_psttCMDBuffer->bsavingMode);

			//swprintf(Type, 256, L"GRGBDLL_SET_LED_Type = %02d\n", iRet);
			//MessageBox(NULL, Type, L"G_RGB_KB_DLL", MB_OK);
			
		}
	}
	return GRGBDLL_SUCCESS;
}

//for 4 Zone Kb Function
DECLSPEC int __cdecl GRGBDLL_SwitchLight_4Z(BOOL bFlag, PINPUT_4ZONE_BUFFER_CMD a_pCMDBuffer)
{
	int iRet = 0;
	if(bFlag) {
		if(a_pCMDBuffer == NULL)
			return GRGBDLL_INITIAL_STATE_FAIL;
		GRGBDLL_Capture4ZONEBuffer(a_pCMDBuffer);
	}
	else {
		//add by hans for S3下要關閉Row Type
		GRGBDLL_RowCloseHandle();
		iRet = GRGBDLL_SET_LED_Type(LED_CTRL_LIGHTOFF_INDEX, 1, 1, 0, 1, 1, a_pCMDBuffer->_TSLedType_08H.bsavingMode);
	}
	return GRGBDLL_SUCCESS;
}


DECLSPEC int __cdecl GRGBDLL_SwitchLight(BOOL bFlag, PINPUT_BUFFER_CMD a_pCMDBuffer)
{
	int iRet = 0;
	if(bFlag) {
		if(a_pCMDBuffer == NULL)
			return GRGBDLL_INITIAL_STATE_FAIL;
		GRGBDLL_CaptureCMDBuffer(a_pCMDBuffer);
	}
	else {
		//add by hans for S3下要關閉Row Type
		GRGBDLL_RowCloseHandle();
		iRet = GRGBDLL_SET_LED_Type(LED_CTRL_LIGHTOFF_INDEX, 1, 1, 0, 1, 1, GRGBDLL_g_psttCMDBuffer->bsavingMode);
	}
	return GRGBDLL_SUCCESS;
} 

DECLSPEC int __cdecl GRGBDLL_GET_Firmware_Version(BYTE *a_pbHVer, BYTE *a_pbLVer)
{
	int ret;
	BYTE set_rpt[8] = { 0x80,0 };
	BYTE get_rpt[8] = { 0 };
	BYTE bHVer = 0;
	//CString sDbug = _T("");
	if(!GRGBDLL_g_bInitialStatus){
		//MessageBox(NULL, L"The initial state of the DLL is not established.\n First, the GRGBDLL_Initial DLL function is executed",
		//		  L"G_RGB_KB_DLL", MB_OK);
		return GRGBDLL_INITIAL_STATE_FAIL;
	}
	//if(!GRGBDLL_USB_FindHID(GRGBDLL_g_iHID_VendorID, GRGBDLL_g_iHID_ProductID)) {
	//	return GRGBDLL_NG_NO_DEVICE;
	//}

	set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);
	ret = USB_IO_Report(set_rpt, 8, get_rpt, 8);

	if(ret != 0) {
		//MessageBox(NULL, L"Get IO Report Fail", L"G_RGB_KB_DLL", MB_OK);
		return 2;
	}
	*a_pbHVer = get_rpt[1];// >> 4; //Get High Byte Version
	*a_pbLVer = get_rpt[2];// >> 4; //Get Low Byte Version

	return GRGBDLL_SUCCESS;
}

DECLSPEC int __cdecl GRGBDLL_SET_LED_Type(BYTE a_bCtrl, BYTE a_bEffect, BYTE a_bSpeed, BYTE a_bLight, BYTE a_bColor, BYTE a_bDirection, BOOL a_bSaveMode)
{
	int ret;
	BYTE set_rpt[8] = { 0x08};//,a_bCtrl, a_bEffect, a_bSpeed, a_bLight, a_bColor,a_bDirection };
	//CString sDbug = _T("");
	if(!GRGBDLL_g_bInitialStatus) {
		//MessageBox(NULL, L"The initial state of the DLL is not established.\n First, the GRGBDLL_Initial DLL function is executed",
		//		   L"G_RGB_KB_DLL", MB_OK);
		return GRGBDLL_INITIAL_STATE_FAIL;
	}

	//if(!GRGBDLL_USB_FindHID(GRGBDLL_g_iHID_VendorID, GRGBDLL_g_iHID_ProductID)) {
	//	return GRGBDLL_NG_NO_DEVICE;
	//}
	if((1 > a_bCtrl) || (3 < a_bCtrl))	return LED_TYPE_CMD_BYTE1_FAIL;
	else set_rpt[1] = a_bCtrl;

	if((1 > a_bEffect) || (0x34 < a_bEffect))	return LED_TYPE_CMD_BYTE2_FAIL;
	else set_rpt[2] = a_bEffect;

	if((0 > a_bSpeed) || (0x0B < a_bSpeed))	return LED_TYPE_CMD_BYTE3_FAIL;
	else set_rpt[3] = a_bSpeed;

	if((0 > a_bLight) || (0x32 < a_bLight))	return LED_TYPE_CMD_BYTE4_FAIL;
	else set_rpt[4] = a_bLight;

	if((1 > a_bColor) || (0x08 < a_bColor))	return LED_TYPE_CMD_BYTE5_FAIL;
	else set_rpt[5] = a_bColor;

	if((0 > a_bDirection) || (4 < a_bDirection)) return LED_TYPE_CMD_BYTE6_FAIL;
	else set_rpt[6] = a_bDirection;

	set_rpt[7] = a_bSaveMode;//GRGBDLL_checksum(set_rpt, 7);
	ret = USB_Set_Report(set_rpt, 8);

	if(ret != 0) {
		return 2;
	}

	return GRGBDLL_SUCCESS;
}

DECLSPEC int __cdecl GRGBDLL_GET_LED_Type(BYTE *a_pbCtrl, BYTE *a_pbEffect, BYTE *a_pbSpeed, BYTE *a_pbLight, BYTE *a_pbColor, BYTE *a_pbDirection)
{
	BYTE set_rpt[8] = { 0x08, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	BYTE get_rpt[8] = { 0 };
	//int ret;
	if(!GRGBDLL_g_bInitialStatus) {
		//MessageBox(NULL, L"The initial state of the DLL is not established.\n First, the GRGBDLL_Initial DLL function is executed",
		//		   L"G_RGB_KB_DLL", MB_OK);
		return GRGBDLL_INITIAL_STATE_FAIL;
	}

	set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);

	if(USB_IO_Report(set_rpt, 8, get_rpt, 8))
	{
		//MessageBox(NULL, L"Get IO Report Fail", L"G_RGB_KB_DLL", MB_OK);
		return 2;
	}
	*a_pbCtrl		= get_rpt[1];
	*a_pbEffect		= get_rpt[2];
	*a_pbSpeed		= get_rpt[3];
	*a_pbLight		= get_rpt[4];
	*a_pbColor		= get_rpt[5];
	*a_pbDirection	= get_rpt[6];

	return GRGBDLL_SUCCESS;
}

DECLSPEC int __cdecl GRGBDLL_Set_User_Picture(BYTE a_bIndex, BYTE a_bOption, BYTE a_KeySize, BYTE a_bCtrl, PINTPUT_USER_KEY_SET a_pbUserKeySet)
{
	BYTE set_rpt[8] = { 0x12, 0x0, 0x0, 0x08, 0x0, 0x0, 0x0, 0x0 };
	BYTE set_LEDOff_rpt[8] = {0x08, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0};
	BYTE set_LEDOn_rpt[8] = { 0x08, 0x02, 0x33, 0x06, 0x31, 0x07, 0x01};
	BYTE get_rpt[8] = { 0 };
	BYTE abPictureData[512] = {0x0};

	memset(abPictureData, 0x0, 512);
	//int ret;
	if(!GRGBDLL_g_bInitialStatus) {
		//MessageBox(NULL, L"The initial state of the DLL is not established.\n First, the GRGBDLL_Initial DLL function is executed",
		//		   L"G_RGB_KB_DLL", MB_OK);
		return GRGBDLL_INITIAL_STATE_FAIL;
	}
	GRGBDLL_RowCloseHandle();

	//if(!GRGBDLL_USB_FindHID(GRGBDLL_g_iHID_VendorID, GRGBDLL_g_iHID_ProductID)) {
	//	return GRGBDLL_NG_NO_DEVICE;
	//}
	if ((0 > a_bCtrl) || (4 < a_bCtrl))	return USER_PICTURE_CMD_BYTE2_FAIL;
	else set_rpt[1] = a_bCtrl;

	if((0 > a_bIndex) || (4 < a_bIndex))	return USER_PICTURE_CMD_BYTE2_FAIL;
	else set_rpt[2] = a_bIndex;

	if((0 > a_bOption) || (1 < a_bOption))	return USER_PICTURE_CMD_BYTE4_FAIL;
	else set_rpt[4] = a_bOption;

	//==set LED OFF=====================================
	set_LEDOff_rpt[7] = GRGBDLL_checksum(set_LEDOff_rpt, 7);


	//==DF說要先OFF再開,但經測試後發現會有USB插拔聲音，所以先關閉，===
	//==執行後發現沒影響功能，所以暫且關閉=========================
	//if(USB_Set_Report(set_LEDOff_rpt, 8)) {
	//	MessageBox(NULL, L"USB_Set_Report Fail", L"G_RGB_KB_DLL", MB_OK);
	//	return 2;
	//}

	//==Set Picture======================================
	set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);

	if(USB_Set_Report(set_rpt, 8)){
		//MessageBox(NULL, L"USB_Set_Report Fail", L"G_RGB_KB_DLL", MB_OK);
		return 2;
	}

	for(int i = 0; i <= a_KeySize-1; i++){
		if((a_pbUserKeySet + i)->bKey == 0) continue;
		memset(abPictureData + (((a_pbUserKeySet + i)->bKey * 4 ) -1), (a_pbUserKeySet + i)->bkBlue, sizeof(a_pbUserKeySet->bkBlue));
		memset(abPictureData + (((a_pbUserKeySet + i)->bKey * 4) - 2), (a_pbUserKeySet + i)->bKGreen, sizeof(a_pbUserKeySet->bKGreen));
		memset(abPictureData + (((a_pbUserKeySet + i)->bKey * 4) - 3), (a_pbUserKeySet + i)->bKRed, sizeof(a_pbUserKeySet->bKRed));
	}

	if(USB_Out_Pic_EP4(abPictureData, 512)){
		//MessageBox(NULL, L"USB_Out_EP4 Fail", L"G_RGB_KB_DLL", MB_OK);
		return 3;
	}

	//==set LED ON======================================
	//set_LEDOn_rpt[7] = GRGBDLL_checksum(set_LEDOn_rpt, 7);

	//if(USB_Set_Report(set_LEDOn_rpt, 8)) {
		//MessageBox(NULL, L"USB_Set_Report Fail", L"G_RGB_KB_DLL", MB_OK);
	//	return 2;
	//}
	
	return GRGBDLL_SUCCESS;
}


DECLSPEC int __cdecl GRGBDLL_Get_User_Picture(BYTE a_bIndex, BYTE *a_pbPic_data)
{
	BYTE set_rpt[8] = { 0x92, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	BYTE get_rpt[8] = { 0 };
//	int ret;
	if(!GRGBDLL_g_bInitialStatus) {
		//MessageBox(NULL, L"The initial state of the DLL is not established.\n First, the GRGBDLL_Initial DLL function is executed",
		//		   L"G_RGB_KB_DLL", MB_OK);
		return GRGBDLL_INITIAL_STATE_FAIL;
	}

	//if(!GRGBDLL_USB_FindHID(GRGBDLL_g_iHID_VendorID, GRGBDLL_g_iHID_ProductID)) {
	//	return GRGBDLL_NG_NO_DEVICE;
	//}
	if((0 > a_bIndex) || (4 < a_bIndex))	return USER_PICTURE_CMD_BYTE2_FAIL;
	else set_rpt[2] = a_bIndex;

	set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);

	if(USB_Set_Report(set_rpt, 8)){
		//MessageBox(NULL, L"USB_Set_Report Fail", L"G_RGB_KB_DLL", MB_OK);
		return 2;
	}

	if(USB_Get_Report(get_rpt, 8)){
		//MessageBox(NULL, L"USB_Get_Report Fail", L"G_RGB_KB_DLL", MB_OK);
		return 3;
	}

	if((get_rpt[0] != 0x92) || (get_rpt[2] != a_bIndex) || (get_rpt[3] != 8)){
		//MessageBox(NULL, L"Error reading data", L"G_RGB_KB_DLL", MB_OK);
		return 4;
	}

	if(USB_In_EP3(a_pbPic_data, 192)) {
		//MessageBox(NULL, L"USB_In_EP3 Fail", L"G_RGB_KB_DLL", MB_OK);
		return 5;
	}

	return GRGBDLL_SUCCESS;
}


DECLSPEC int __cdecl GRGBDLL_Recovery_Data()
{
	BYTE set_rpt[8] = { 0x13, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	BYTE get_rpt[8] = { 0 };
//	int ret;
	if(!GRGBDLL_g_bInitialStatus) {
		//MessageBox(NULL, L"The initial state of the DLL is not established.\n First, the GRGBDLL_Initial DLL function is executed",
		//		   L"G_RGB_KB_DLL", MB_OK);
		return GRGBDLL_INITIAL_STATE_FAIL;
	}

	GRGBDLL_RowCloseHandle();

	//if(!GRGBDLL_USB_FindHID(GRGBDLL_g_iHID_VendorID, GRGBDLL_g_iHID_ProductID)) {
	//	return GRGBDLL_NG_NO_DEVICE;
	//}
	set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);

	if(USB_Set_Report(set_rpt, 8)) {
		//MessageBox(NULL, L"USB_Set_Report Fail", L"G_RGB_KB_DLL", MB_OK);
		return 2;
	}
	return GRGBDLL_SUCCESS;
}


DECLSPEC int __cdecl GRGBDLL_Set_Color(BYTE a_bColor_index, BYTE a_bRed, BYTE a_bGreen, BYTE a_bBlue, BYTE a_bSave)
{
	BYTE set_rpt[8] = { 0x14, 0x0};
	BYTE get_rpt[8] = { 0 };
//	int ret;
	if(!GRGBDLL_g_bInitialStatus) {
		//MessageBox(NULL, L"The initial state of the DLL is not established.\n First, the GRGBDLL_Initial DLL function is executed",
		//		   L"G_RGB_KB_DLL", MB_OK);
		return GRGBDLL_INITIAL_STATE_FAIL;
	}

	//if(!GRGBDLL_USB_FindHID(GRGBDLL_g_iHID_VendorID, GRGBDLL_g_iHID_ProductID)) {
	//	return GRGBDLL_NG_NO_DEVICE;
	//}
	if((0 > a_bColor_index) || (0x0F < a_bColor_index))	return COLOR_CMD_BYTE2_FAIL;
	set_rpt[2] = a_bColor_index;
	set_rpt[3] = a_bRed;
	set_rpt[4] = a_bGreen;
	set_rpt[5] = a_bBlue;
	set_rpt[7] = a_bSave;

	if(USB_Set_Report(set_rpt, 8)) {
		//MessageBox(NULL, L"USB_Set_Report Fail", L"G_RGB_KB_DLL", MB_OK);
		return 2;
	}

	return GRGBDLL_SUCCESS;
}


DECLSPEC int __cdecl GRGBDLL_Get_Color(BYTE a_bColor_index, BYTE *a_pbRed, BYTE *a_pbGreen, BYTE *a_pbBlue)
{
	BYTE set_rpt[8] = { 0x94, 0x0};
	BYTE get_rpt[8] = { 0 };
//	int ret;
	if(!GRGBDLL_g_bInitialStatus) {
		//MessageBox(NULL, L"The initial state of the DLL is not established.\n First, the GRGBDLL_Initial DLL function is executed",
		//		   L"G_RGB_KB_DLL", MB_OK);
		return GRGBDLL_INITIAL_STATE_FAIL;
	}

	if((0 > a_bColor_index) || (6 < a_bColor_index))	return COLOR_CMD_BYTE2_FAIL;
	if((a_pbRed == NULL) || (a_pbGreen == NULL) || (a_pbBlue == NULL)) {
		return COLOR_CMD_RGBBUFF_FAIL;
	}
	//if(!GRGBDLL_USB_FindHID(GRGBDLL_g_iHID_VendorID, GRGBDLL_g_iHID_ProductID)) {
	//	return GRGBDLL_NG_NO_DEVICE;
	//}
	set_rpt[2] = a_bColor_index;
	set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);

	if(USB_Set_Report(set_rpt, 8))
		return GRGBDLL_NG_SET_REPORT;

	if(USB_Get_Report(get_rpt, 8))
		return GRGBDLL_NG_GET_REPORT;

	if((get_rpt[0] != 0x94) || (get_rpt[2] != a_bColor_index))
		return GRGBDLL_NG_GET_DATA;

	*a_pbRed	= get_rpt[3];
	*a_pbGreen	= get_rpt[4];
	*a_pbBlue	= get_rpt[5];


	return GRGBDLL_SUCCESS;
}
DECLSPEC int __cdecl GRGBDLL_Set_Row_LED(BYTE a_bSwitchFlag, BYTE a_bRowEffect, BYTE a_bRowTempo, PINTPUT_USER_KEY_SET a_bRowColor, BYTE a_bRowDirection)
{
	GRGBDLL_g_bGoROWLoopFlag = a_bSwitchFlag;
	GRGBDLL_g_bRow_index = 5; //0~5 = 6
	//GRGBDLL_g_bRow_Tempo = a_bRowTempo;
	GRGBDLL_g_bRow_Effect = a_bRowEffect;
	//GRGBDLL_g_TestSleep = a_iSleepTime;

	if((0 > a_bRowTempo) || (0x0B < a_bRowTempo))	return LED_TYPE_CMD_BYTE3_FAIL;
	else GRGBDLL_g_bRow_Tempo = a_bRowTempo;

	if(a_bRowColor)//	return LED_TYPE_CMD_BYTE5_FAIL;
	{
		if(GRGBDLL_g_pbRow_Color){
			delete GRGBDLL_g_pbRow_Color;
			GRGBDLL_g_pbRow_Color = NULL;
		}
		GRGBDLL_g_pbRow_Color = new INTPUT_USER_KEY_SET[7];
		//GRGBDLL_g_pbRow_Color = a_bRowColor;
		memcpy(GRGBDLL_g_pbRow_Color, a_bRowColor, sizeof(INTPUT_USER_KEY_SET[7]));
	}
		

	if((0 > a_bRowDirection) || (4 < a_bRowDirection)) return LED_TYPE_CMD_BYTE6_FAIL;
	else GRGBDLL_g_bRow_Direction = a_bRowDirection;

	if(!GRGBDLL_g_bInitialStatus) {
		//MessageBox(NULL, L"The initial state of the DLL is not established.\n First, the GRGBDLL_Initial DLL function is executed",
		//		   L"G_RGB_KB_DLL", MB_OK);
		return GRGBDLL_INITIAL_STATE_FAIL;
	}

	if(a_bSwitchFlag) {
		GRGBDLL_RowCloseHandle();

		//if(!GRGBDLL_USB_FindHID(GRGBDLL_g_iHID_VendorID, GRGBDLL_g_iHID_ProductID)) {
		//	return GRGBDLL_NG_NO_DEVICE;
		//}
		DWORD dwId;
		GRGBDLL_g_hRowThread = CreateThread(NULL, 0, GRGBDLL_Polling_RowThread, 0, 0, &dwId);

	}
	else{
		GRGBDLL_RowCloseHandle();
		if(GRGBDLL_g_pbRow_Color) {
			delete GRGBDLL_g_pbRow_Color;
			GRGBDLL_g_pbRow_Color = NULL;
		}
	}

	return GRGBDLL_SUCCESS;
}

DECLSPEC int __cdecl GRGBDLL_Set_key_Color(BYTE a_bkey_offset, BYTE a_bmake_break)
{
	BYTE set_rpt[8] = { 0x19, 0x04, 0x02};//, 0x0, 0x0, 0x0, 0x0, 0x0 };
	BYTE get_rpt[8] = { 0 };
//	int ret;
	if(!GRGBDLL_g_bInitialStatus) {
		//MessageBox(NULL, L"The initial state of the DLL is not established.\n First, the GRGBDLL_Initial DLL function is executed",
		//		   L"G_RGB_KB_DLL", MB_OK);
		return GRGBDLL_INITIAL_STATE_FAIL;
	}

	if((a_bkey_offset < 0) || (126 < a_bkey_offset) || ((a_bmake_break != 0) && (a_bmake_break != 0xFF)))
		return GRGBDLL_NG_PARAMETER;

	//if(!GRGBDLL_USB_FindHID(GRGBDLL_g_iHID_VendorID, GRGBDLL_g_iHID_ProductID)) {
	//	return GRGBDLL_NG_NO_DEVICE;
	//}

	set_rpt[3] = a_bkey_offset;
	set_rpt[4] = a_bmake_break;
	set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);

	if(USB_Set_Report(set_rpt, 8))
		return GRGBDLL_NG_SET_REPORT;

	return GRGBDLL_SUCCESS;
}

DECLSPEC int __cdecl GRGBDLL_Welcome_LED_setup(BYTE a_bWelOnOFF, BYTE a_bWelTime)
{
	BYTE set_rpt[8] = { 0x1A, 0x05};//, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	BYTE get_rpt[8] = { 0 };
//	int ret;
	if(!GRGBDLL_g_bInitialStatus) {
		//MessageBox(NULL, L"The initial state of the DLL is not established.\n First, the GRGBDLL_Initial DLL function is executed",
		//		   L"G_RGB_KB_DLL", MB_OK);
		return GRGBDLL_INITIAL_STATE_FAIL;
	}
	if((a_bWelOnOFF < 0) || (1 < a_bWelOnOFF))
		return GRGBDLL_NG_PARAMETER;

	if((a_bWelTime < 0) || (255 < a_bWelTime))
		return GRGBDLL_NG_PARAMETER;

	//if(!GRGBDLL_USB_FindHID(GRGBDLL_g_iHID_VendorID, GRGBDLL_g_iHID_ProductID)) {
	//	return GRGBDLL_NG_NO_DEVICE;
	//}

	set_rpt[2] = a_bWelOnOFF;
	set_rpt[3] = a_bWelTime;
	set_rpt[7] = GRGBDLL_checksum(set_rpt, 7);

	if(USB_Set_Report(set_rpt, 8))
		return GRGBDLL_NG_SET_REPORT;

	return GRGBDLL_SUCCESS;
}


#ifdef __cplusplus
}
#endif
