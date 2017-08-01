#pragma once

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")
#pragma warning(disable:4996) //全部關掉
extern "C"
{
#include "hid_inc\hidsdi.h"
#include "hid_inc\SETUPAPI.H"
}


//==for Device ID===============================
//for DF
//#define _VID_	0x0D62//0x04D9//
//#define _PID_	0x50B7//0x8008//

//For ITE
#define _VID_	0x048D
#define _PID_	0xCE00

#define KEYS_NUM  (21*6)
#define   SERVICE_NAME		L"MainWindow"
#define _EXPORTING

#ifdef _EXPORTING
#define DECLSPEC    __declspec(dllexport)
#else
#define DECLSPEC    __declspec(dllimport)
#endif	
//#define CEIL(x,y,z){(x % y) >= z ? x = (x /y)+1 : x = x /y;}

//==for USB Protocol============================
struct _GRGB_USB_PROTOCOL_
{
	int iCMD[8];
	int iData[8];
	BYTE bEP4Buffer521b[512];	//by Set User Picture (12H)
	BYTE bROWBuffer64b[64];	//by Set Row LED (16H)
}GRGBDLL_USB_PROTOCOL, *pGRGBDLL_USB_PROTOCOL;


enum _KB_TYPE_ID_
{
	_ME_KB_FAIL = 0,
	_FULL_ME_KB = 1,
	_4ZONE_ME_KB = 2,
	_2nd_FME_KB = 3
};

enum _USB_HID_USAGEPAGE_
{
	_USAGEPAGE_FAIL = 0,
	_1ST_FME_USAGEPAGE = 0xFF02,
	_1ST_4ME_USAGEPAGE = 0xFF12,
	_2nd_FME_USAGEPAGE = 0xFF03
};


//==for define Status ID=================================
enum _GRGBDLL_ID_
{
	GRGBDLL_USE_RT_MODE = 0,	//Run time mode
	GRGBDLL_USE_WC_MODE = 1,	//welcom modes
	Reserve2 = 2,
	Reserve3 = 3,
	Reserve4 = 4,
	Reserve5 = 5,
	Reserve6 = 6,
	Reserve7 = 7,
	Reserve8 = 8,
	GRGBDLL_CMDBUFFER_FAIL = 9,
	GRGBDLL_AUDIO_STATUS = 239,
	GRGBDLL_SUCCESS = 240,
	GRGBDLL_INITIAL_STATE_FAIL	= -1,
	GRGBDLL_NG_NO_DEVICE		= -2,
	GRGBDLL_NG_PARAMETER		= -3,
	GRGBDLL_NG_SET_REPORT		= -4,
	GRGBDLL_NG_GET_REPORT		= -5,
	GRGBDLL_NG_GET_DATA			= -6,
	GRGBDLL_NG_OUT_EP4			= -7,
	GRGBDLL_NG_IN_EP3			= -8,
	GRGBDLL_NG_ROW_THREAD_FALL  = -9,
	GRGBDLL_NG_OEMSVC_THREAD_FALL = -10
};

enum _GRGBDLL_AP_EFFECT_ID_ {
	AP_EFF_STATIC_TYPE		= 0X20,
	AP_EFF_RAINBOW_TYPE		= 0X21,
	AP_EFF_IMPACT			= 0X22,
	AP_EFF_ARRAY_TYPE		= 0X23,
	AP_EFF_AUDIO_TYPE		= 0X24,
	AP_EFF_USER_TYPE		= 0X25,
	AP_EFF_NERO_TYPE		= 0X26, 
	AP_EFF_AUDIO_TYPE3		= 0X27
};

enum _LED_TYPE_ID_
{
	//==define Index===================
	LED_COLOR_RUNTIME_INDEX = 1, //for color index 1
	LED_COLOR_WELCOME_INDEX = 1, //for color index 9
	LED_COLOR_IN_TURN_INDEX = 8,
	LED_CTRL_LIGHTOFF_INDEX = 1,
	LED_CTRL_RUNTIME_INDEX = 2,
	LED_CTRL_WELCOME_INDEX = 3,
	LED_EFFECT_USER_DEFINE = 0x33,
	//==Fail status====================
	LED_TYPE_CMD_BYTE1_FAIL = 10,
	LED_TYPE_CMD_BYTE2_FAIL = 11,
	LED_TYPE_CMD_BYTE3_FAIL = 12,
	LED_TYPE_CMD_BYTE4_FAIL = 13,
	LED_TYPE_CMD_BYTE5_FAIL = 14,
	LED_TYPE_CMD_BYTE6_FAIL = 15,
	LED_TYPE_CMD_BYTE7_FAIL = 16
};

enum _USER_PICTURE_FAIL_ID_
{
	USER_PICTURE_CMD_BYTE2_FAIL = 20,
	USER_PICTURE_CMD_BYTE4_FAIL = 21,

};

enum _USER_COLOR_FAIL_ID_
{
	COLOR_CMD_BYTE2_FAIL = 30,
	COLOR_CMD_BYTE4_FAIL = 31,
	COLOR_CMD_RGBBUFF_FAIL = 32

};

enum _SET_ROW_LED_ID_
{
	ROW_CMD_BYTE2_FAIL = 40,
	ROW_CMD_BYTE4_FAIL = 41,
	ROW_CMD_DATABUFFER_FAIL = 49,
	ROW_CMD_RGBBUFF_FAIL = 32

};

enum _SET_DIRECTION_ID_
{
	DIR_LEFT2RIGHT	= 0x01,
	DIR_RIGHT2LEFT	= 0x02,
	DIR_BOTTOM2UP	= 0x03,
	DIR_UP2BOTTOM	= 0x04
};

typedef struct _INTPUT_USER_KEY_SET
{
	BYTE bKey;
	BYTE bKRed;
	BYTE bKGreen;
	BYTE bkBlue;

}INTPUT_USER_KEY_SET, *PINTPUT_USER_KEY_SET;

typedef struct _LEDTYPE_08H_SET
{
	BYTE bEffectType;	//effect
	BYTE bSpeed;		//tempolevel
	BYTE blight;		//lightlevel
	BYTE bColor;		//Color
	BYTE bdirection;	//direction
	BOOL bsavingMode;	//for Keep Status

}LEDTYPE_08H_SET, *PLEDTYPE_08H_SET;

typedef struct _INPUT_BUFFER_CMD
{
	/*========================================
	//for Run time mode/ WelCome mode
	//Runtime mode  = 0, Welcome mode = 1
	//for MCU Effect Index = 01~1F
	//for AP Effect Index = 20H~3F
	//========================================*/
	//HWND hWnd;
	BYTE bMode;			
	BYTE bType;			//effect
	BYTE blight;		//lightlevel
	BYTE bTempo;		//tempolevel
	BYTE bdirection;	//direction
	BYTE abRGB[4];		//0 = T/F, 1 = R, 2 = G, 3 = B


	unsigned int NeonMode_sec;	//user_sec
	//==sttaRGB=======================
	//for Set Color 14H & Row Type 16H
	//TRUE = 七顏色/FALSE = 單色
	BOOL bColorInTurn;
	//->bKey代表enable/disable
	INTPUT_USER_KEY_SET sttaRGB[16];
	//for User mode
	//==sttaUserKeySet=======================
	//for User mode 12H
	//->bKey代表每個Key ID
	BYTE bKeySize;
	INTPUT_USER_KEY_SET sttaUserKeySet[126];
	//reserved
	BOOL bSleepMode;
	//for Keep Status
	BOOL bsavingMode;

}INPUT_BUFFER_CMD, *PINPUT_BUFFER_CMD;

typedef struct _INPUT_4ZONE_BUFFER_CMD
{
	/*========================================
	//for Run time mode/ WelCome mode
	//Runtime mode  = 0, Welcome mode = 1
	//for MCU Effect Index = 01~1F
	//for AP Effect Index = 20H~3F
	//========================================*/
	//HWND hWnd;
	BYTE bMode;
	LEDTYPE_08H_SET _TSLedType_08H;

	//=========================================
	//for Set Color 14H & Row Type 16H
	//_TS:代表typedef struct
	//->bKey代表enable/disable
	INTPUT_USER_KEY_SET _TSaRGB[16];

}INPUT_4ZONE_BUFFER_CMD, *PINPUT_4ZONE_BUFFER_CMD;

typedef struct _AUDIO_BUFFER
{
	BYTE bBuffer[21];
}AUDIO_BUFFER, *PAUDIO_BUFFER;

//==for Static LED Type===========================
BYTE g_abStaticType[378] =
{
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
	0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF
};

BYTE g_abImpactType[63] =
{
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00
};


BYTE a_abInsideRunRGBTable[10][3] =
{
	{0xFF, 0x0, 0x0},
	{0x0FF, 0xFF, 0x0},
	{0x0FF, 0xFF, 0xFF},
	{0x0, 0xFF, 0x0},
	{0x0, 0xFF, 0xFF},
	{0xFF, 0xFF, 0x0},
	{0x0, 0x0, 0xFF},
	{0x0, 0xFF, 0xFF},
	{0xFF, 0x0, 0xFF},
	{0xFF, 0xFF, 0XFF}

};

BYTE a_abInsideRunLRGBTable[10][3] =
{
	{ 0xFF, 0x0, 0xFF },
	{ 0x0, 0xFF, 0xFF },
	{ 0x0, 0x80, 0xFF },
	{ 0x0, 0x80, 0xFF },
	{ 0x80, 0xFF, 0x0 },
	{ 0x40, 0x0, 0x80 },
	{ 0x00, 0x80, 0X80 },
	{ 0xFF, 0xFF, 0x0 },
	{ 0x40, 0xFF, 0x0 },
	{ 0xC0, 0x80, 0x0 },

};


BYTE a_aboutsideRunRGBTable[10][3] =
{
	{ 0xFF, 0x0, 0x0 },
	{ 0xFF, 0xFF, 0x0 },
	{ 0x40, 0xFF, 0x0 },
	{ 0xC0, 0x80, 0x0 },
	{ 0xFF, 0x0, 0xFF },
	{ 0x0, 0xFF, 0xFF },
	{ 0x0, 0x80, 0xFF },
	{ 0x80, 0xFF, 0x0 },
	{ 0x40, 0x0, 0x80 },
	{ 0x00, 0x80, 0X80 }

};

BYTE g_abArrayType[378] =
{
		//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

int g_iKeyBGRID[126][3]=
{
/*Index1*/{0,21,42},	  {1,22,43},	  {2,23,44},	  {3,24,45},	  {4,25,46},   	  {5,26,47},      {6,27,48},	  {7,28,49},	  {8,29,50},	  {9,30,51},	  {10,31,52},	  {11,32,53},	  {12,33,54},		{13,34,55},		{14,35,56},		{15,36,57},		{16,37,58},		{17,38,59},		{18,39,60},		{19,40,61},		{20,41,62},
/*Index2*/{ 63,84,105 },  { 64,85,106 },  { 65,86,107 },  { 66,87,108 },  { 67,88,109 },  { 68,89,110 },  { 69,90,111 },  { 70,91,112 },  { 71,92,113 },  { 72,93,114 },  { 73,94,115 },  { 74,95,116 },  { 75,96,117 },	{ 76,97,118 },	{ 77,98,119 },	{ 78,99,120 },	{ 79,100,121 },	{ 80,101,122 },	{ 81,102,123 },	{ 82,103,124 },	{ 83,104,125 },
/*Index3*/{ 126,147,168 },{ 127,148,169 },{ 128,149,170 },{ 129,150,171 },{ 130,151,172 },{ 131,152,173 },{ 132,153,174 },{ 133,154,175 },{ 134,155,176 },{ 135,156,177 },{ 136,157,178 },{ 137,158,179 },{ 138,159,180 },{ 139,160,181 },{ 140,161,182 },{ 141,162,183 },{ 142,163,184 },{ 143,164,185 },{ 144,165,186 },{ 145,166,187 },{ 146,167,188 },
/*Index4*/{ 189,210,231 },{ 190,211,232 },{ 191,212,233 },{ 192,213,234 },{ 193,214,235 },{ 194,215,236 },{ 195,216,237 },{ 196,217,238 },{ 197,218,239 },{ 198,219,240 },{ 199,220,241 },{ 200,221,242 },{ 201,222,243 },{ 202,223,244 },{ 203,224,245 },{ 204,225,246 },{ 205,226,247 },{ 206,227,248 },{ 207,228,249 },{ 208,229,250 },{ 209,230,251 },
/*Index5*/{ 252,273,294 },{ 253,274,295 },{ 254,275,296 },{ 255,276,297 },{ 256,277,298 },{ 257,278,299 },{ 258,279,300 },{ 259,280,301 },{ 260,281,302 },{ 261,282,303 },{ 262,283,304 },{ 263,284,305 },{ 264,285,306 },{ 265,286,307 },{ 266,287,308 },{ 267,288,309 },{ 268,289,310 },{ 269,290,311 },{ 270,291,312 },{ 271,292,313 },{ 272,293,314 },
/*Index6*/{ 315,336,357 },{ 316,337,358 },{ 317,338,359 },{ 318,339,360 },{ 319,340,361 },{ 320,341,362 },{ 321,342,363 },{ 322,343,364 },{ 323,344,365 },{ 324,345,366 },{ 325,346,367 },{ 326,347,368 },{ 327,348,369 },{ 328,349,370 },{ 329,350,371 },{ 330,351,372 },{ 331,352,373 },{ 332,353,374 },{ 333,354,375 },{ 334,355,376 },{ 335,356,377 }
};


BYTE g_bArrayStep[15][20] =
{
	/*Step0*/ { 14,  4,   4,  14,  18,  20,  20,  20,  20,  20,  20,  17,  11,  6,   1 },
	/*Step1*/ {  1, 21, 106, 126, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
	/*Step2*/ {  2, 20, 107, 125, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
	/*Step3*/ {  3, 19,  22,  23,  41,  42,  43,  63,  85,  86, 104, 105, 108, 124 },
	/*Step4*/ {  4, 18,  24,  25,  39,  40,  44,  45,  61,  62,  64,  84,  86,  87, 103, 104,  109,  123},
	/*Step5*/ {  5, 17,  25,  26,  38,  39,  45,  46,  60,  61,  64,  65,  83,  84,  87,  88,  102,  103,  110,  122 },
	/*Step6*/ {  6, 16,  26,  27,  37,  38,  46,  47,  59,  60,  65,  66,  82,  83,  88,  89,  101,  102,  111,  121 },
	/*Step7*/ {  7, 15,  27,  28,  36,  37,  47,  48,  58,  59,  66,  67,  81,  82,  89,  90,  100,  101,  112,  120 },
	/*Step8*/ {  8, 14,  28,  29,  35,  36,  48,  49,  57,  58,  67,  68,  80,  81,  90,  91,   99,  100,  113,  119 },
	/*Step9*/ {  9, 13,  29,  30,  34,  35,  49,  50,  56,  57,  68,  69,  79,  80,  91,  92,   98,   99,  114,  118 },
	/*Step10*/{ 10, 12,  30,  31,  33,  34,  50,  51,  55,  56,  69,  70,  78,  79,  92,  93,   97,   98,  115,  117 },
	/*Step11*/{ 11, 31,  32,  33,  51,  52,  54,  55,  70,  71,  77,  78,  93,  94,  96,  97,   116 },
	/*Step12*/{ 32, 52,  53,  54,  71,  72,  76,  77,  94,  95,  96 },
	/*Step13*/{ 53, 72,  73,  75,  76,  95 },
	/*Step14*/{ 74}
};

BYTE a_aArrayRunRGBTable[10][3] =
{
	{ 0xFF, 0x0, 0x0 },
	{ 0xFF, 0xFF, 0x0 },
	{ 0x40, 0xFF, 0x0 },
	{ 0xC0, 0x80, 0x0 },
	{ 0xFF, 0x0, 0xFF },
	{ 0x0, 0xFF, 0xFF },
	{ 0x0, 0x80, 0xFF },
	{ 0x80, 0xFF, 0x0 },
	{ 0x40, 0x0, 0x80 },
	{ 0x00, 0x80, 0X80 }

};

BYTE g_abRotaryType[378] =
{
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x80, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x00, 0x00,
	/*G*/0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xFF, 0xFF, 0xFF,
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00,
	/*G*/0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0xFF, 0xFF, 0xFF,
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x40, 0x40, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFF, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40,
	/*G*/0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80,
	/*R*/0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFF, 0x80, 0x80, 0x80, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x40, 0x40, 0x40, 0x40, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	/*G*/0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	/*R*/0x80, 0x80, 0x80, 0x80, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x40, 0x40, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x40, 0x40, 0x40,
	/*G*/0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x80, 0x80,
	/*R*/0x80, 0x80, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
	};


BYTE a_abAudioStep[3][100] =
{
	/*Step0*/ { 2, 60, 66 },
	/*Step1*/{ 1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
			   22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
			   43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
			   64, 65, 66, 67, 68 ,69, 70, 71, 72, 73,
			   85, 86, 87, 88, 89, 90, 91, 92, 93, 94,
			   106, 107, 108, 109, 110, 111, 112, 113, 114, 115},
	/*Step2*/{ 11, 12, 13, 14, 15, 16, 17, 18, 19 ,20, 21,
			   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
			   53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
			   74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
			   95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105,
			   116, 117, 118, 119 ,120, 121, 122, 123, 124, 125, 126}

};

BYTE a_abAudioStep2[13][100] =
{
	/*Step0*/{ 2, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11 },

	//Left  ======================================================
	/*Step1*/{ 
				106, 107, 108, 109, 110, 111, 112, 113, 114, 115 
			},

	/*Step1-1*/{
					85, 86, 87, 88, 89, 90, 91, 92, 93, 94
				},
	/*Step2*/{
					64, 65, 66, 67, 68 ,69, 70, 71, 72, 73
				},
	/*Step2-1*/{
					43, 44, 45, 46, 47, 48, 49, 50, 51, 52
				},
	/*Step3*/{
					22, 23, 24, 25, 26, 27, 28, 29, 30, 31
				},
	/*Step3-1*/{
					1,  2,  3,  4,  5,  6,  7,  8,  9,  10
				},


	//Right======================================================
	/*Step4*/{
			126,125,124,123,122,121,120,119,118,117,116
			},
	/*Step4-1*/{
			105,104,103,102,101,100,99,98,97,96,95
			},
	/*Step5*/{
			84,83,82,81,80,79,78,77,76,75,74
			},
	/*Step5-1*/{
			63,62,61,60,59,58,57,56,55,54,53
			},
	/*Step6*/{
			42,41,40,39,38,37,36,35,34,33,32
			},
	/*Step6-1*/{
			21,20,19,18,17,16,15,14,13,12,11
			}
};

BYTE a_abRGBBUF[21][3] = {
	//---R---G---B---//
	{ 255, 47, 32 },
	{ 255, 47, 32 },
	{ 191, 122, 32 },
	{ 191, 122, 32 },
	{ 192, 192, 31 },
	{ 192, 192, 31 },
	{ 100, 195, 30 },
	{ 100, 195, 30 },
	{ 30, 195, 60 },
	{ 30, 195, 60 },
	{ 30, 195, 150 },
	{ 30, 195, 150 },
	{ 30, 150, 195 },
	{ 30, 150, 195 },
	{ 30, 45, 195 },
	{ 30, 45, 195 },
	{ 129, 27, 197 },
	{ 129, 27, 197 },
	{ 198, 26, 164 },
	{ 198, 26, 164 },
	{ 198, 26, 164 }
};
BYTE a_abAudioStep3[25][25] =
{
	/*Step0*/{ 21, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 },

	//Left  ======================================================
	/*Step1*/{
		1,22,43,64,85,106
	},
	/*Step2*/{
		2, 23, 44, 65,86,107
	},
	/*Step3*/{
		3,24,45,66,87,108
	},
	/*Step4*/{
		4,25,46,67,88,109
	},
	/*Step5*/{
		5,26,47,68,89,110
	},
	/*Step6*/{
		6,27,48,69,90,111
	},
	/*Step7*/{
		7, 28, 49, 70, 91, 112
	},
	/*Step8*/{
		8, 29, 50, 71, 92, 113
	},
	/*Step9*/{
		9, 30, 51, 72, 93, 114
	},
	/*Step10*/{
		10, 31, 52, 73, 94, 115
	},
	/*Step11*/{
		11, 32, 53, 74, 95, 116
	},
	/*Step12*/{
		12, 33, 54, 75, 96, 117
	},
	/*Step13*/{
		13, 34, 55, 76, 97, 118
	},
	/*Step14*/{
		14, 35, 56, 77, 98, 119
	},
	/*Step15*/{
		15, 36, 57, 78, 99, 120
	},
	/*Step16*/{
		16, 37, 58, 79, 100, 121
	},
	/*Step17*/{
		17, 38, 59, 80, 101, 122
	},
	/*Step18*/{
		18, 39, 60, 81, 102, 123
	},
	/*Step19*/{
		19, 40, 61, 82, 103, 124
	},
	/*Step20*/{
		20, 41, 62, 83, 104, 125
	},
	/*Step21*/{
		21, 42, 63, 84, 105, 126
	}
};


BYTE g_abAudioType[378] =
{
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//-1----------------------5-----------------------------10----------------------------15----------------------------------21--//
	/*B*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*G*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*R*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//==20170125 for on Key press function============
const struct {
	wchar_t *key_name;
	BYTE vk_key;
} GRGBDLL_g_104_keys[KEYS_NUM] = {
	// C0  /////////////////////////////////////////////////////////////////////////
	{ L"Left Ctrl",		VK_LCONTROL },//0
	{ L"Left Shift",	VK_LSHIFT },
	{ L"Caps Lock",		VK_CAPITAL },
	{ L"Tab",			VK_TAB },
	{ L"`",				VK_OEM_3 },
	{ L"ESC",			VK_ESCAPE }, //5
	// C1  /////////////////////////////////////////////////////////////////////////
	{ L"FN",			0 },//6
	{ L"Left Shift",	VK_LSHIFT },
	{ NULL,				0 },
	{ NULL,				0 },
	{ L"1",				'1' },
	{ L"F1",			VK_F1		},//11
	// C2  /////////////////////////////////////////////////////////////////////////
	{ L"Left Win",		VK_LWIN },//12
	{ L"Z",				'Z' },
	{ L"A",				'A' },
	{ L"Q",				'Q' },
	{ L"2",				'2' },
	{ L"F2",			VK_F2		},//17
	// C3  /////////////////////////////////////////////////////////////////////////
	{ L"Left Alt",		VK_LMENU },//18
	{ L"X",				'X' },
	{ L"S",				'S' },
	{ L"W",				'W' },
	{ L"3",				'3' },
	{ L"F3",			VK_F3},//23
	// C4  /////////////////////////////////////////////////////////////////////////
	{ NULL,				0 },//24
	{ L"C",				'C' },
	{ L"D",				'D' },
	{ L"E",				'E' },
	{ L"4",				'4' },
	{ L"F4",			VK_F4 },//29
	// C5  /////////////////////////////////////////////////////////////////////////
	{ L"Space",			VK_SPACE },//30
	{ L"V",				'V' },
	{ L"F",				'F' },
	{ L"R",				'R' },
	{ L"5",				'5' },
	{ L"F5",			VK_F5	},//35
	// C6  /////////////////////////////////////////////////////////////////////////
	{ L"Space",			VK_SPACE },//36
	{ L"B",				'B' },
	{ L"G",				'G' },
	{ L"T",				'T' },
	{ L"6",				'6' },
	{ L"F6",			VK_F6 },//41	
	// C7  /////////////////////////////////////////////////////////////////////////
	{ L"Space",			VK_SPACE },//42
	{ L"N",				'N' },
	{ L"H",				'H' },
	{ L"Y",				'Y' },
	{ L"7",				'7' },
	{ L"F7",			VK_F7 },//47
	// C8  /////////////////////////////////////////////////////////////////////////
	{ NULL,				0 },//48
	{ L"M",				'M' },
	{ L"J",				'J' },
	{ L"U",				'U' },
	{ L"8",				'8' },
	{ L"F8",			VK_F8 },//53	
	// C9  /////////////////////////////////////////////////////////////////////////
	{ L"Right Alt",		VK_RMENU },//54
	{ L",",				VK_OEM_COMMA },
	{ L"K",				'K' },
	{ L"I",				'I' },
	{ L"9",				'9' },	
	{ L"F9",			VK_F9		},//59	
	// C10 /////////////////////////////////////////////////////////////////////////
	{ L"App",			VK_APPS },//60
	{ L".",				VK_OEM_PERIOD },
	{ L"L",				'L' },
	{ L"O",				'O' },
	{ L"0",				'0' },
	{ L"F10",			VK_F10 },//65
	// C11 /////////////////////////////////////////////////////////////////////////
	{ NULL,				0 },//66
	{ L"/",				VK_OEM_2 },
	{ L";",				VK_OEM_1 },
	{ L"P",				'P' },
	{ L"-",				VK_OEM_MINUS },
	{ L"F11",			VK_F11 },//71
	// C12 /////////////////////////////////////////////////////////////////////////
	{ L"Right Ctrl",	VK_RCONTROL },//72
	{ L"Right Shift",	VK_RSHIFT },
	{ L"'",				VK_OEM_7 },
	{ L"[",				VK_OEM_4 },
	{ L"=",				VK_OEM_PLUS },
	{ L"F12",			VK_F12		},//77
	// C13 /////////////////////////////////////////////////////////////////////////
	{ NULL,				0 },//78
	{ NULL,				0 },
	{ NULL,				0 },
	{ L"]",				VK_OEM_6 },
	{ NULL,				0 },
	{ L"Print Screen",	VK_SNAPSHOT },//83
	// C14 /////////////////////////////////////////////////////////////////////////
	{ L"Left",			VK_LEFT },//84
	{ NULL,				0 },
	{ L"Enter",			VK_RETURN },
	{ NULL,				0 },
	{ L"Backspace",		VK_BACK },
	{ L"Insert",		VK_INSERT	},//89	
	// C15 /////////////////////////////////////////////////////////////////////////
	{ L"Down",			VK_DOWN },//90
	{ L"Up",			VK_UP },
	{ L"Enter",			VK_RETURN },
	{ L"\\",			VK_OEM_5 },
	{ L"Backspace",		VK_BACK },
	{ L"Delete",		VK_DELETE	},//95	
	// C16 /////////////////////////////////////////////////////////////////////////
	{ L"Right",			VK_RIGHT },//96
	{ L"NumPad 1",		VK_NUMPAD1 },
	{ L"NumPad 4",		VK_NUMPAD4 },
	{ L"NumPad 7",		VK_NUMPAD7 },
	{ L"Num Lock",		VK_NUMLOCK },
	{ L"Home",			VK_HOME		},//101
	// C17 /////////////////////////////////////////////////////////////////////////
	{ L"NumPad 0",		VK_NUMPAD0 },//102
	{ L"NumPad 2",		VK_NUMPAD2 },
	{ L"NumPad 5",		VK_NUMPAD5 },
	{ L"NumPad 8",		VK_NUMPAD8 },
	{ L"NumPad /",		VK_DIVIDE },
	{ L"End",			VK_END		},//107
	// C18 /////////////////////////////////////////////////////////////////////////
	{ L"NumPad .",		VK_DECIMAL },//108
	{ L"NumPad 3",		VK_NUMPAD3 },
	{ L"NumPad 6",		VK_NUMPAD6 },
	{ L"NumPad 9",		VK_NUMPAD9 },
	{ L"NumPad *",		VK_MULTIPLY },
	{ L"Page Up",		VK_PRIOR	},//113
	// C19 /////////////////////////////////////////////////////////////////////////
	{ L"NumPad Enter",	0xFD },//114
	{ L"NumPad Enter",	0xFD },
	{ L"NumPad +",		VK_ADD },
	{ L"NumPad +",		VK_ADD },
	{ L"NumPad -",		VK_SUBTRACT },	
	{ L"Page Down",		VK_NEXT		},//119
	// C20 /////////////////////////////////////////////////////////////////////////
	{ NULL,				0			},//120
	{ NULL,				0			},
	{ NULL,				0			},
	{ NULL,				0			},
	{ NULL,				0			},
	{ NULL,				0			}
};


extern "C" {          // we need to export the C interface
	DECLSPEC int __cdecl GRGBDLL_SET_LED_Type(BYTE a_bCtrl, BYTE a_bEffect, BYTE a_bSpeed, BYTE a_bLight, BYTE a_bColor, BYTE a_bDirection, BOOL a_bSaveMode);
	DECLSPEC int __cdecl GRGBDLL_Set_Color(BYTE a_bColor_index, BYTE a_bRed, BYTE a_bGreen, BYTE a_bBlue, BYTE a_bSave);
	DECLSPEC int __cdecl GRGBDLL_Set_Row_LED(BYTE a_bSwitchFlag, BYTE a_bRowEffect, BYTE a_bRowTempo, PINTPUT_USER_KEY_SET a_bRowColor, BYTE a_bRowDirection);
	DECLSPEC int __cdecl GRGBDLL_Set_User_Picture(BYTE a_bIndex, BYTE a_bOption, BYTE a_KeySize, BYTE a_bCtrl, PINTPUT_USER_KEY_SET a_pbUserKeySet);

	//DECLSPEC int __cdecl GRGBDLL_InitialDLL(void);
	//DECLSPEC void __cdecl GRGBDLL_DestroyDLL(void);

	int USB_Set_Report(BYTE *a_pbData, int a_iLen);
	void GRGBDLL_RowCloseHandle();
	void GRGBDLL_WaitLightingState();
	void GRGBDLL_CloseOEMSVCThreadHandle();
}

typedef BOOL(__cdecl *OemSvcHook)(int argc, char* argv, int* key, int keysize);	// OemSvcHook


//==for KB Type===================================
BYTE GRGBDLL_g_bKBType = _ME_KB_FAIL;


//==20170125 for on Key press function============
HINSTANCE GRGBDLL_g_hInst;					// current instance
HHOOK GRGBDLL_g_kholl = NULL;
DWORD GRGBDLL_g_last_key = 0;

//==for Debug=============
int g_iDebugCount_L = 0;
int g_iDebugCount_R = 0;

//==for Global varible============================
BOOL GRGBDLL_g_bInitialStatus = FALSE;
int GRGBDLL_g_iHID_VendorID = 0;
int GRGBDLL_g_iHID_ProductID = 0;
BOOL GRGBDLL_g_bGetLightingFlag = FALSE;
//int GRGBDLL_g_TestSleep = 0;
//--for Row Type----------
HANDLE GRGBDLL_g_hRowThread = NULL;
//BYTE *GRGBDLL_g_pbROWDataPoint = NULL;
BYTE GRGBDLL_g_bGoROWLoopFlag = FALSE;
BYTE GRGBDLL_g_bRow_index = 0;
BYTE GRGBDLL_g_bRow_Tempo = 0;
PINTPUT_USER_KEY_SET GRGBDLL_g_pbRow_Color;
BYTE GRGBDLL_g_bRow_Direction = 0;
BYTE GRGBDLL_g_bRow_Effect = 0;

//--for Audio Type----------
BYTE GRGBDLL_g_bLBlue = 0xff;
BYTE GRGBDLL_g_bLGreen = 0xff;
BYTE GRGBDLL_g_bLRed = 0xff;
BYTE GRGBDLL_g_bRBlue = 0xff;
BYTE GRGBDLL_g_bRGreen = 0xff;
BYTE GRGBDLL_g_bRRed = 0xff;

//--for Audio2 Type----------
unsigned int GRGBDLL_g_iLBlueStep = 0;
unsigned int GRGBDLL_g_iLGreenStep = 0;
unsigned int GRGBDLL_g_iLRedStep = 0;
unsigned int GRGBDLL_g_iRBlueStep = 0;
unsigned int GRGBDLL_g_iRGreenStep = 0;
unsigned int GRGBDLL_g_iRRedStep = 0;
//==by test==
unsigned int GRGBDLL_g_iLOldBStep = 0;
unsigned int GRGBDLL_g_iLOldGStep = 0;
unsigned int GRGBDLL_g_iLOldRStep = 0;
unsigned int GRGBDLL_g_iROldBStep = 0;
unsigned int GRGBDLL_g_iROldGStep = 0;
unsigned int GRGBDLL_g_iROldRStep = 0;
unsigned int GRGBDLL_g_iLOldBStep2 = 0;
unsigned int GRGBDLL_g_iLOldGStep2 = 0;
unsigned int GRGBDLL_g_iLOldRStep2 = 0;
unsigned int GRGBDLL_g_iROldBStep2 = 0;
unsigned int GRGBDLL_g_iROldGStep2 = 0;
unsigned int GRGBDLL_g_iROldRStep2 = 0;
//==by Test for 14H Color
PINTPUT_USER_KEY_SET GRGBDLL_g__STpColorTemp = NULL;

//--for Audio3 Type----------
BYTE abTemp[25] = { 0x0 };
BYTE *GRGBDLL_g_pbAudio3Data = NULL;

//--for Key Command-------
PINPUT_BUFFER_CMD GRGBDLL_g_psttCMDBuffer;

//==for HID Device Name=========================
wchar_t USB_g_MyDevicePathName[MAX_PATH] = { 0 };
wchar_t USB_g_MyVendorPathName[MAX_PATH] = { 0 };

//== for Other variable=========================
int USB_g_feature_length = 0;
int USB_g_output_length = 0;
int USB_g_input_length = 0;
//==for OEMSvc Thread===========================
//HANDLE GRGBDLL_g_hOEMSVCThread = NULL;
//HINSTANCE GRGBDLL_g_hOEMSVCInstLib = NULL;
//HINSTANCE hinstLib;
