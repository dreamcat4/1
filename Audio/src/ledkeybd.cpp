#include "audiostealer.h"
#include <dbt.h>
//
HANDLE _hKeybdDevice = INVALID_HANDLE_VALUE;
//
static USHORT _nFeatureLength = 0;
static USHORT _nOutputLength  = 0;
static USHORT _nInputLength   = 0;
//
static UCHAR     _AudioTypeArryr[378] = {0};
static RGB_TABLE _rgbTable[6]         = {0};
CONST  RGB_VALUE _rgbValue[7]         = { {0x00, {0x00, 0x00, 0xFF}}, {0x03, {0x00, 0x7D, 0xFF}}, {0x06, {0x00, 0xFF, 0xFF}}, {0x00, {0x00, 0x00, 0x00}},
                                          {0x0C, {0xFF, 0x00, 0x00}}, {0x0F, {0xFF, 0xFF, 0x00}}, {0x12, {0xFF, 0x00, 0xFF}} };
//
static
BOOLEAN
UsbSetReport(PBYTE pData, CONST INT Length)
{
    UNREFERENCED_PARAMETER(pData);
    UNREFERENCED_PARAMETER(Length);
//
    assert(_hKeybdDevice != INVALID_HANDLE_VALUE || _hKeybdDevice != NULL);
//
    UCHAR   reportData[32] = {0};
    BOOLEAN rtn;
//
    RtlCopyMemory(reportData + 1, pData, Length);
    Sleep(0);
    rtn = HidD_SetFeature(_hKeybdDevice, reportData, _nFeatureLength);
    return rtn;
};
//
static
BOOLEAN SetLedType(VOID)
{
    BOOLEAN rtn;
    UCHAR   setreport[8] = { 0x08, 0x02, 0x33, 0x0A, 0x32, 0x01, 0x00, 0x00 };
//
    rtn = UsbSetReport(setreport, 8);
    return rtn;
};
//
static
BOOLEAN
UsbIoReport(PBYTE pbOutData, INT OutLength, PBYTE pbInData, INT InLength)
{
    BYTE    SetData[32] = {0};
    BYTE    GetData[32] = {0};
    BOOLEAN rtn;
    INT   i;
//
    for (i = 0; i < 20; i++)
    {
        RtlCopyMemory(SetData + 1, pbOutData, OutLength);
        Sleep(0);
        rtn = HidD_SetFeature(_hKeybdDevice, SetData, _nFeatureLength);
        if (rtn == FALSE)
        {
            return FALSE;
        };
//
        Sleep(0);
        rtn = HidD_GetFeature(_hKeybdDevice, GetData, _nFeatureLength);
        if (rtn == FALSE)
        {
            return FALSE;
        };
//
        if (GetData[1] == pbOutData[0])
        {
            RtlCopyMemory(pbInData, GetData + 1, InLength);
            break;
        };
    };
//
    if (i >= 20)
        return FALSE;
    return TRUE;
};
//
static
BOOLEAN
UsbSetReportAndWriteEP4(CONST PBYTE pbSetReportData, CONST INT nReportLength, CONST PBYTE pbWriteData, CONST INT WriteLength)
{
    UCHAR SetData[32]     = { 0 };
    UCHAR WriteBuffer[65] = { 0 };
//
    BOOLEAN rtn;
    ULONG   len = 0;
//
    __try
    {
        RtlCopyMemory(SetData + 1, pbSetReportData, nReportLength);
        rtn = HidD_SetFeature(_hKeybdDevice, SetData, _nFeatureLength);
        EXIT_ON_HID_ERROR(rtn);
//
        RtlCopyMemory(WriteBuffer + 2, pbWriteData, WriteLength);
        rtn = (BOOLEAN)WriteFile(_hKeybdDevice, WriteBuffer, _nOutputLength, &len, NULL);
        EXIT_ON_HID_ERROR(rtn);
//
        if (len != _nOutputLength)
            return FALSE;
        return TRUE;
    }
    __except(1)
    {
        return FALSE;
    };
};
//
static
UCHAR GetChecksum(PBYTE pData, CONST INT Length)
{
    INT i, sum = 0;
//
    for (i = 0; i < Length; i++)
        sum += pData[i];
    return (0xFF - sum);
};
//
static
BOOLEAN FindHIDDevice(VOID)
{
    HIDD_ATTRIBUTES                    Attributes;
    SP_DEVICE_INTERFACE_DATA           devInfoData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA   detailData;
//
    INT      index;
    ULONG    dwRequiredSize;
    HANDLE   hDevInfo;
    GUID     HidGuid;
    BOOLEAN  rtn;
    BOOLEAN  fRtn;
//
    HidD_GetHidGuid(&HidGuid);
//
    hDevInfo = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    devInfoData.cbSize = sizeof(devInfoData);
//
    index = 0;
    fRtn  = FALSE;
    while (1)
    {
        if (SetupDiEnumDeviceInterfaces(hDevInfo, 0, &HidGuid, index, &devInfoData))
        {
            SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &dwRequiredSize, NULL);
            detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(dwRequiredSize);
            detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            if (SetupDiGetDeviceInterfaceDetailW(hDevInfo, &devInfoData, detailData, dwRequiredSize, &dwRequiredSize, NULL))
            {
                _hKeybdDevice = CreateFileW(detailData->DevicePath,
                                            GENERIC_READ | GENERIC_WRITE,
                                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                                            NULL,
                                            OPEN_EXISTING,
                                            0,
                                            NULL);
                if (_hKeybdDevice == INVALID_HANDLE_VALUE)
                {
                    index++;
                    free(detailData);
                    continue;
                };
//
                Attributes.Size = sizeof(Attributes);
                rtn = HidD_GetAttributes(_hKeybdDevice, &Attributes);
                if (rtn == FALSE)
                {
                    index++;
                    free(detailData);
                    CloseHandle(_hKeybdDevice);
                    _hKeybdDevice = INVALID_HANDLE_VALUE;
                    continue;
                };
//
                if ((Attributes.VendorID == LED_KEYBD_VID) && (Attributes.ProductID == LED_KEYBD_PID))
                {
                    PHIDP_PREPARSED_DATA pPreData = 0;
                    if (HidD_GetPreparsedData(_hKeybdDevice, &pPreData))
                    {
                        HIDP_CAPS cap = {0};
                        if (HidP_GetCaps(pPreData, &cap) == HIDP_STATUS_SUCCESS)
                        {
							if (1 == cap.Usage) {
								switch (cap.UsagePage) {
									case _1ST_FME_USAGEPAGE:
									case _1ST_4ME_USAGEPAGE:
									case _2nd_FME_USAGEPAGE:
									{
										if (cap.FeatureReportByteLength > 0)
										{
											fRtn = TRUE;
											_nInputLength = cap.InputReportByteLength;
											_nFeatureLength = cap.FeatureReportByteLength;
											_nOutputLength = cap.OutputReportByteLength;
										}
									}
									break;
								}
							}
							/*
                            if (cap.Usage == 1 && cap.UsagePage == 0xFF02)
                            {
                                if (cap.FeatureReportByteLength > 0)
                                {
                                    _nInputLength   = cap.InputReportByteLength;
                                    _nFeatureLength = cap.FeatureReportByteLength;
                                    _nOutputLength  = cap.OutputReportByteLength;
                                    fRtn = TRUE;
                                };
                            }
                            else if (cap.Usage == 1 && cap.UsagePage == 0xFF12)
                            {
                                if (cap.FeatureReportByteLength > 0)
                                {
                                    _nInputLength   = cap.InputReportByteLength;
                                    _nFeatureLength = cap.FeatureReportByteLength;
                                    _nOutputLength  = cap.OutputReportByteLength;
                                    fRtn = TRUE;
                                };
                            };
							*/

                            HidD_FreePreparsedData(pPreData);
                        };
                    };
                };
                free(detailData);
                if (fRtn == FALSE)
                {
                    CloseHandle(_hKeybdDevice);
                    _hKeybdDevice = INVALID_HANDLE_VALUE;
                }
                else
                    break;
            }
            else
            {
                free(detailData);
            };
        }
        else
        {
            break;
        };
        index++;
    };
//
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return fRtn;
};
//
static
BOOLEAN GetHIDFirmwareVersion(PUCHAR pHigh, PUCHAR pLow)
{
    UCHAR SetReport[8] = {0x80, 0};
    UCHAR GetReport[8] = {0};
    UCHAR bHVer = 0;
//
    SetReport[7] = GetChecksum(SetReport, 7);
    if (UsbIoReport(SetReport, 8, GetReport, 8) == FALSE)
        return FALSE;
    *pHigh = GetReport[1]; // >> 4; //Get High Byte Version
    *pLow  = GetReport[2]; // >> 4; //Get Low Byte Version   
    return TRUE;
};
//
static
VOID SendCmdToHID(VOID)
{
    UCHAR  report[8] = { 0x16, 0x0 };
    PUCHAR ptr       = 0;
//
    ptr = _AudioTypeArryr;
    for (INT index = 0; index <= 5; index++)
    {
        report[2] = index;
        report[7] = GetChecksum(report, 7);
        if (UsbSetReportAndWriteEP4(report, 8, ptr, 63) == FALSE)
            break;
        ptr += 63;
    };
};
//
BOOLEAN WINAPI
HidRgbKeyboard(CONST PSHORT nLimit)
{
    INT i, idx, j;
//
    UCHAR  report[8]       = {0x16, 0x0};
    UCHAR  SetData[32]     = {0};
    UCHAR  WriteBuffer[65] = {0};
//
    BOOLEAN rtn;
    ULONG   len = 0;
    SHORT   Idx = 0;
    PUCHAR  ptr = 0;
//
    CONST SHORT nFeatureLength = _nFeatureLength;
    CONST ULONG lOutputLength  = _nOutputLength;
//
    RGB_TABLE rgbTable[6] = {0};
//
    if (_hKeybdDevice == INVALID_HANDLE_VALUE || _hKeybdDevice == NULL)
        return FALSE;
//
    RtlZeroMemory(rgbTable, sizeof(RGB_TABLE)*6);
    for (i = 0; i < 7; i++)
    {
        if (i == 3)
        {
            for (j = 0, Idx = 0; j < 7; j++, Idx++)
            {
                if (j == 3)
                {
                    --Idx;
                    continue;
                };
//
                for (idx = 0; idx < 2; idx++)
                {
                     rgbTable[Idx].Blue[idx+9]  = 0;
                     rgbTable[Idx].Green[idx+9] = 0xFF;
                     rgbTable[Idx].Red[idx+9]   = 0;
                };
            };
            continue;
        };
//
        if (nLimit[i] <= 0)
        {
            for (j = 0, Idx = 0; j < 7; j++, Idx++)
            {
                if (j == 3)
                {
                    --Idx;
                    continue;
                };
//
                for (idx = 0; idx < 2; idx++)
                {
                    rgbTable[Idx].Blue[idx + _rgbValue[i].Offset]  = 0;
                    rgbTable[Idx].Green[idx + _rgbValue[i].Offset] = 0;
                    rgbTable[Idx].Red[idx + _rgbValue[i].Offset]   = 0;
                };
            };
        }
        else
        {
            for (j = 0, Idx = 0; j < 7 && j < nLimit[i]; j++, Idx++)
            {
                if (j == 3)
                {
                    --Idx;
                    continue;
                };
//
                for (idx = 0; idx < 2; idx++)
                {
                    rgbTable[Idx].Blue[idx + _rgbValue[i].Offset]  = _rgbValue[i].Value[0];
                    rgbTable[Idx].Green[idx + _rgbValue[i].Offset] = _rgbValue[i].Value[1];
                    rgbTable[Idx].Red[idx + _rgbValue[i].Offset]   = _rgbValue[i].Value[2];
                };
            };
        };
    };
//
    RtlCopyMemory(_AudioTypeArryr, rgbTable, sizeof(RGB_TABLE)*6);
    ptr = _AudioTypeArryr;
    for (INT index = 0; index <= 5; index++)
    {
        report[2] = index;
        report[7] = GetChecksum(report, 7);
        __try
        {
            RtlCopyMemory(SetData + 1, report, 8);
            rtn = HidD_SetFeature(_hKeybdDevice, SetData, nFeatureLength);
            EXIT_ON_HID_ERROR(rtn);
//
            RtlCopyMemory(WriteBuffer + 2, ptr, 63);
            rtn = (BOOLEAN)WriteFile(_hKeybdDevice, WriteBuffer, lOutputLength, &len, NULL);
            EXIT_ON_HID_ERROR(rtn);
//
            if (len != _nOutputLength)
                return FALSE;
        }
        __except(1)
        {
            return FALSE;
        };
        ptr += 63;
    };
    return TRUE;
};
//
BOOLEAN WINAPI
RGBKeybdInitialize(PUCHAR pbHigh, PUCHAR pbLow)
{
    BOOLEAN rtn = FALSE;
    
    if (FindHIDDevice())
    {
        rtn = GetHIDFirmwareVersion(pbHigh, pbLow);
    };
    EXIT_ON_HID_ERROR(rtn);
//
    if (SetLedType() == FALSE)
        EXIT_ON_HID_ERROR(rtn);
    return rtn;
};
//
VOID WINAPI
DisableLedKeybd(VOID)
{
    if (_hKeybdDevice == INVALID_HANDLE_VALUE || _hKeybdDevice == NULL)
        return;
    RtlZeroMemory(_AudioTypeArryr, sizeof(UCHAR)*378);
//    SendCmdToHID();
};