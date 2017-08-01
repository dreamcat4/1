#ifndef __AUDIO_STEALER_H_
#define __AUDIO_STEALER_H_
//
#define _CRT_SECURE_NO_WARNINGS
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <avrt.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <strsafe.h>
#include <dbt.h>
#include <initguid.h>
//
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "avrt.lib")

#define PI                                  3.1415926535897931f
#define HALFPI                              1.5707963267948966f
//
#define REFTIMES_PER_SEC                    10000000
#define REFTIMES_PER_MILLISEC               10000
#define AUDIO_BUFFER_SIZE                   512
//
#define EXIT_ON_ERROR(hres)  \
    if (FAILED(hres)) { goto ExitStealAudio; }
//
#define EXIT_ON_ERROR_LOOP(hres)  \
    if (FAILED(hres)) { goto ExitPollingAudio; }
//
#define SAFE_RELEASE(punk)  \
    if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; }
//
#define BITREV_F(j, nu, fp)                                             \
{                                                                       \
    INT j2, j1 = j, k = 0;                                              \
    for (INT i = 1; i <= nu; i++)                                       \
    {                                                                   \
        j2 = j1/2; k  = ((2*k) + j1) - (2*j2); j1 = j2;                 \
    };                                                                  \
    *fp = (FLOAT)k;                                                     \
}
//
#define BITREV_I(j, nu, ip)                                             \
{                                                                       \
    INT j2, j1 = j, k = 0;                                              \
    for (INT i = 1; i <= nu; i++)                                       \
    {                                                                   \
         j2 = j1/2; k  = ((2*k) + j1) - (2*j2); j1 = j2;                \
    };                                                                  \
    *ip =  k;                                                           \
}
//
#include <hidsdi.h>
#include <setupapi.h>
#include <assert.h>
//
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")
//
#define LED_KEYBD_VID                                   0x048D
#define LED_KEYBD_PID                                   0xCE00
//
#define LED_CTRL_RUNTIME_INDEX                          0x02
#define LED_USER_DEFINE_EFFECT                          0x33
//
#define MAX_ROW_COUNT                                   5
#define SPECSCOPEWIDTH_STEREO                           4
#define SPECSCOPEWIDTH_QUAD                             5
//
#define EXIT_ON_HID_ERROR(rtnValue)  \
    if (rtnValue == FALSE) { printf("Return FALSE\n"); return FALSE; }
//
#define WM_RESET_AUDIO_STEALER_MESSAGE                  WM_USER+1
//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
//
enum _USB_HID_USAGEPAGE_
{
	_USAGEPAGE_FAIL = 0,
	_1ST_FME_USAGEPAGE = 0xFF02,
	_1ST_4ME_USAGEPAGE = 0xFF12,
	_2nd_FME_USAGEPAGE = 0xFF03
};


//
enum WorkChannel
{
    WRKCHANNEL_LEFT   = 0,
    WRKCHANNEL_RIGHT  = 1,
    WRKCHANNEL_MONO   = 2,
    WRKCHANNEL_STEREO = 3
};
//
class SampleIter
{
protected:
    WorkChannel m_curChannel;
    PCHAR       m_pSamples;
    INT         m_sampleLen;
    INT         m_byteWidth;
    BOOL        m_stereoSamples;
    INT         m_interRWIdx;

public:
    SampleIter()
    { 
        m_curChannel    = WRKCHANNEL_STEREO;
        m_pSamples      = NULL;
        m_sampleLen     = 0;
        m_byteWidth     = 0;
        m_interRWIdx    = 0;
        m_stereoSamples = FALSE;
    };
//
	VOID SetWorkChannel(WorkChannel workChhannel)
    {
        if (this == NULL)
            return;
        m_curChannel = workChhannel;
    };
//
    BOOL SetSampleWorkSet(PCHAR pSamples, INT sampleLen, INT byteWidth, BOOL stereo)
    {
        if (this == NULL)
            return FALSE;
        if (pSamples == NULL)
            return FALSE;
//
        m_pSamples = pSamples;
//
        if (byteWidth <= 0)
            return FALSE;
//
        m_sampleLen = sampleLen;
        m_byteWidth = byteWidth;
//
        if ((m_stereoSamples = stereo) == FALSE)
            m_curChannel = WRKCHANNEL_MONO;
        else
            m_curChannel = WRKCHANNEL_STEREO;        
        SetPos (0);
        return TRUE;
    };
//
    INT Count(VOID)
    {
        if (this == NULL)
            return 0;
        if (m_pSamples == NULL)
            return 0;
        
        if (m_sampleLen <= 0)
            return 0;

        INT cnt = m_sampleLen / m_byteWidth;

        if (m_stereoSamples != FALSE)
            cnt /= 2;
        return cnt;
    };
//
    VOID SetPos(INT idx)
    {
        if (this == NULL)
            return;
        __try
        {
            m_interRWIdx = (idx * m_byteWidth);
            if (m_stereoSamples != FALSE)
                m_interRWIdx *= 2;
        }
        __except(1)
        {
        };
    };
//
    INT GetPos(VOID)
    {
        if (this == NULL)
            return 0;
        INT pos = m_interRWIdx / m_byteWidth;
        if (m_stereoSamples != FALSE)
            pos /= 2;
        return pos;
    };
//
    VOID Advance(VOID)
    {   
        if (this == NULL)
            return;
        m_interRWIdx += m_byteWidth;
        if (m_stereoSamples != FALSE)
            m_interRWIdx += m_byteWidth;        
    };
//
#if 0
    VOID BartlettWindow(VOID)
    {
        if (this == NULL)
            return;
//
        INT    fullRange = Count();
        PSHORT pData     = (PSHORT)m_pSamples;
//
        INT    idx;
        FLOAT  pwr  = (FLOAT)(1.0 / (FLOAT)(fullRange / 2));
//
        switch (m_curChannel)
        {
            case WRKCHANNEL_LEFT:
                idx = 0;
            break;
//
            case WRKCHANNEL_RIGHT:
                idx = 2;
            break;
        };
//
        for (INT c = 0; c < fullRange; c++)
        {
            if (c < (fullRange/2))
            {
                pData[idx] = (INT)((FLOAT)pData[idx] * (pwr * (FLOAT)idx));
            }
            else
            {
                pData[idx] = (INT)((FLOAT)pData[idx] * (1.0 - (pwr * (FLOAT)((fullRange / 2) - idx))));
            };
            if (m_stereoSamples != FALSE)
                idx += 4;
            else
                idx+= 2;
        };
    };
#endif
//
    SHORT GetSample(VOID)
    {
        if (this == NULL)
            return -1;
//
        if (m_pSamples == NULL)
            return -1;
//
        switch (m_byteWidth)
        {
            case 0:
                return -1;
//
            case 1:
                if (m_curChannel != WRKCHANNEL_RIGHT)
                {
                    return m_pSamples[m_interRWIdx];
                };
                return m_pSamples[m_interRWIdx + 1];
//
            case 2:
                if (m_curChannel != WRKCHANNEL_RIGHT)
                {
                    return *((PSHORT)(m_pSamples + m_interRWIdx)); 
                } ;
                return *((PSHORT)(m_pSamples + m_interRWIdx + 2));                
//
            default:
                return *((PSHORT)(m_pSamples + m_interRWIdx)); 
        };
        return -1;
    };
};
//
HRESULT StealAudioStream(VOID);
BOOL    FixedWaveformTo16Bits(WAVEFORMATEX *pwfx);
FLOAT   FastSqrt(FLOAT x);
//
class FFTransform
{
private:
    INT    sampleRate;
    INT    sampleCount;
    FLOAT* pSpecMag;
    FLOAT* pXReal;
    FLOAT* pXImaginary;
    FLOAT* pSamples;
//
#define sinCosRng 2048
    FLOAT  sINTab[sinCosRng];
    FLOAT  cosTab[sinCosRng]; 
//
    INT bitrev(INT j, INT nu)
    {
        if (this == NULL)
            return 0;
//
        INT j2;
        INT j1 = j;
        INT k = 0;
        for (INT i = 1; i <= nu; i++)
        {
            j2 = j1 / 2;
            k  = ((2 * k) + j1) - (2 * j2);
            j1 = j2;
        };
        return k;
    };
//
    VOID InitFftTables(VOID)
    {
        if (this == NULL)
            return;
//
        DOUBLE step = HALFPI / (DOUBLE)sinCosRng;
        DOUBLE rad  = 0.0;
//
        for (INT idx = 0; idx < sinCosRng; idx++)
        {
            sINTab[idx] = (FLOAT)sin(rad);
            cosTab[idx] = (FLOAT)cos(rad);
            rad        += step;
        };
    };
//
public:
//
    FFTransform(INT sampleRate, INT sampleCount)
    {
        if (this == NULL)
            return;
//
        InitFftTables();
//
        this->sampleRate  = sampleRate;
        this->sampleCount = sampleCount;
        pSpecMag          = new FLOAT [sampleCount];
        pXReal            = new FLOAT [sampleCount];
        pXImaginary       = new FLOAT [sampleCount];
    };
//
    ~FFTransform()
    {
        if (pSpecMag != NULL)
            delete[] pSpecMag;
//
        if (pXReal != NULL)
            delete[] pXReal;
//
        if (pXImaginary != NULL)
            delete[] pXImaginary;
//
        pXReal = pXImaginary = pSpecMag = NULL;
    };
//
#if 0
    INT GetFrequency(INT poINT) const
    {
        if (this == NULL)
            return 0;
        return sampleRate / sampleCount;
    };
#endif
//
    INT HzToPoINT(INT freq) const
    {
        if (this == NULL)
            return 0;
        return (sampleCount * freq) / sampleRate;
    };
//
    FLOAT* fftGetMagArray(VOID)
    {
        if (this == NULL)
            return NULL;
        return pSpecMag;
    };
//
    FLOAT fftGetMagFromHertz(INT freq)
    {
        if (this == NULL)
            return 0.0;
        return pSpecMag[HzToPoINT(freq)];
    };
//
    BOOL fftTransformMag(SampleIter& samples, BOOL powerSpectrum)
    {
        if (this == NULL)
            return FALSE;
        if ((pSpecMag == NULL) || (pXImaginary == NULL) || (pXReal == NULL))
            return FALSE;
        if (samples.Count() > sampleCount)
            return FALSE;
//
        INT nu            = (INT)(log(sampleCount)/log(2));
        INT halfSampCount = sampleCount/2;
        INT nu1           = nu - 1;
//
        FLOAT tr, ti, p, arg, c, s;
//
        INT idx;
        for (idx = 0; idx < sampleCount; idx++)
        {
            pXReal[idx]  = samples.GetSample();
//
            if (powerSpectrum != FALSE)
                pSpecMag[idx] = pXReal[idx];
//
            pXImaginary[idx] = 0.0f;
            samples.Advance();
        };
//
        INT k = 0;
        INT scidx;
//
        for (INT l = 1; l <= nu; l++)
        {
            while (k < sampleCount)
            {
                for (INT i = 1; i <= halfSampCount; i++)
                {
                    BITREV_F(k >> nu1, nu, &p)
                    arg = 2 * (FLOAT)PI * p / sampleCount;
//
                    while (arg >= HALFPI)
                        arg = arg - HALFPI;
//
                    scidx = (INT)((arg / HALFPI) * (FLOAT)sinCosRng);
                    c     = cosTab [scidx];
                    s     = sINTab [scidx];
//
                    tr  = pXReal[k + halfSampCount] * c + pXImaginary[k + halfSampCount] * s;
                    ti  = pXReal[k + halfSampCount] * c - pXReal[k + halfSampCount] * s;
//
                    pXReal[k+halfSampCount]      = pXReal[k] - tr;
                    pXImaginary[k+halfSampCount] = pXImaginary[k] - ti;
                    pXReal[k]                   += tr;
                    pXImaginary[k]              += ti;
                    k++;
                };
                k += halfSampCount;
            };
//
            k = 0;
            nu1--;
            halfSampCount = halfSampCount / 2;
        };
//
        k = 0;
        INT r;        
        while (k < sampleCount)
        {
            BITREV_I(k, nu, &r)
//
            if (r > k)
            {
                tr             = pXReal[k];
                ti             = pXImaginary[k];
                pXReal[k]      = pXReal[r];
                pXImaginary[k] = pXImaginary[r];
                pXReal[r]      = tr;
                pXImaginary[r] = ti;
            };
            k++;
        };
//
        halfSampCount = sampleCount / 2;
        if (powerSpectrum != FALSE)
        {
	        for (idx = 0; idx < halfSampCount; idx++)
                pSpecMag[idx] = 2.0f * ((FLOAT)pXReal[idx] / (FLOAT)sampleCount);
        }
        else
        {
            pSpecMag[0]   = (FLOAT)(FastSqrt(pXReal[0] * pXReal[0] + pXImaginary[0] * pXImaginary[0])) / (FLOAT)sampleCount;
            for (idx = 1; idx < halfSampCount; idx++)
                  pSpecMag[idx] = 2.0f * (FLOAT)(FastSqrt((pXReal[idx] * pXReal[idx]) + (pXImaginary[idx] * pXImaginary[idx]))) / (FLOAT)sampleCount;
        };
        return TRUE;
    };
};
//
typedef struct _tagRGB_TABLE
{
    UCHAR Blue[21];
    UCHAR Green[21];
    UCHAR Red[21];
} RGB_TABLE;
//
typedef struct _tagRGB_VALUE
{
    SHORT Offset;
    UCHAR Value[3]; //Blue, Green, Red
    
} RGB_VALUE;
//
BOOLEAN WINAPI RGBKeybdInitialize(PUCHAR pbHigh, PUCHAR pbLow);
BOOLEAN WINAPI HidRgbKeyboard(CONST PSHORT nLimit);
VOID WINAPI    DisableLedKeybd(VOID);
//
VOID  WINAPI FreeResource(VOID);
//
DWORD WINAPI AudioThreadProc(_In_ LPVOID lpVoid);
DWORD WINAPI GetCurrentChannelCount(_In_ LPVOID lpVoid);
//
VOID EnumAudioDriver(PSHORT pDevCount);
//
VOID CALLBACK
DevChangeTimerProc(_In_ HWND     hwnd,
                   _In_ UINT     uMsg,
                   _In_ UINT_PTR idEvent,
                   _In_ DWORD    dwTime);
//
extern
BOOL     _fIsStop;
extern
BOOL     _fIsOtherDev;
extern
HANDLE   _hKeybdDevice;
//
#endif