#include "audiostealer.h"
//
const CLSID CLSID_MMDeviceEnumerator    = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator       = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient              = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient       = __uuidof(IAudioCaptureClient);
//
static FFTransform*  _pFFTrans          = NULL;
static FFTransform*  _pFFTransStereo    = NULL;
static SampleIter*   _pSampleIter       = NULL;
//
static BOOL  _fUsedHID     = FALSE;
static BOOL  _fIsQuery     = FALSE;
static SHORT _nDevCount    = 0;
//
WAVEFORMATEX _Wfx = {0};
//
BOOL   _fIsStop        = TRUE;
BOOL   _fPolling       = TRUE;
BOOL   _fIsOtherDev    = FALSE;
PWSTR  _ChannelId      = 0;
HANDLE _NotifyAppEvent = NULL;
//HINSTANCE _hInstance   = 0;
CRITICAL_SECTION _CriticalSection = {0};
//
BOOL _DEBUG_ENABLE = FALSE;
//
VOID EnumAudioDriver(PSHORT pDevCount)
{
    HDEVINFO        hDevInfo      = 0L;
    SP_DEVINFO_DATA spDevInfoData = {0};
    SHORT           wIndex        = 0;
    SHORT           devCount      = 0;
    WCHAR           msg[256]      = {0};
//
    hDevInfo = SetupDiGetClassDevs(0L, 0L, NULL, DIGCF_PRESENT |
                                   DIGCF_ALLCLASSES | DIGCF_PROFILE);
    if (hDevInfo == (void*)-1)
    {
        return;
    };
//
    wIndex = devCount = 0;
    spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
//
    while (1)
    {
        if (SetupDiEnumDeviceInfo(hDevInfo,
                                  wIndex,
                                  &spDevInfoData))
        {
            WCHAR szBuf[2048] = {0};
            SHORT wImageIdx   = 0;
            SHORT wItem       = 0;
//
            if (!SetupDiGetDeviceRegistryPropertyW(hDevInfo,
                                                   &spDevInfoData,
                                                   SPDRP_CLASS,
                                                   0L,
                                                   (PBYTE)szBuf,
                                                   2048,
                                                   0))
            {
                wIndex++;
                continue;
            };
//
            if (_wcsnicmp(szBuf, L"MEDIA", wcslen(L"MEDIA")) == 0)
            {
                WCHAR   szName[MAX_PATH] = {0};
//
                if (SetupDiGetDeviceRegistryPropertyW(hDevInfo,
                                                      &spDevInfoData,
                                                      SPDRP_DEVICEDESC,
                                                      0L,
                                                      (PBYTE)szName,
                                                      MAX_PATH,
                                                      0))
                {
                    devCount++;
#if 0
                    wprintf(L"SPDRP_CLASS(name) = %s\t\t\tSPDRP_DEVICEDESC(Name) = %s\n", szBuf, szName);
                    StringCbPrintfW(msg, 128, L"SPDRP_CLASS(name) = %s\t\t\tSPDRP_DEVICEDESC(Name) = %s\n", szBuf, szName);
                    OutputDebugStringW(msg);
#endif
                };
            };
        }
        else
            break;
        wIndex++;
    };
//
    *pDevCount = devCount;
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return;
};
//
FLOAT FastSqrt(FLOAT x)
{
    FLOAT xhalf = 0.5f*x;
    INT   i     = *(PINT)&x;
//
    i = 0x5f375a84 - (i >> 1);
    x = *(FLOAT*)&i;
//
    x = x*(1.5f - xhalf * x * x);
    return 1.0f / x;
};
//
VOID WINAPI FreeResource(VOID)
{
    if (_pFFTrans != NULL)
    {
        delete _pFFTrans;
    };
     _pFFTrans = NULL;
//
    if (_pFFTransStereo != NULL)
    {
        delete _pFFTransStereo;
    };
    _pFFTransStereo = NULL;
//
    if (_pSampleIter != NULL)
    {
        delete _pSampleIter;
    };
    _pSampleIter = NULL;
};
//
DWORD WINAPI AudioThreadProc(_In_ LPVOID lpVoid)
{
    UNREFERENCED_PARAMETER(lpVoid);
//
    if (_fIsStop == FALSE)
        StealAudioStream();
    return 0;
};
//
DWORD WINAPI GetCurrentChannelCount(_In_ LPVOID lpVoid)
{
    HRESULT                hr;
    REFERENCE_TIME         hnsDefaultDevicePeriod;
    IMMDeviceEnumerator    *pEnumerator             = NULL;
    IMMDevice              *pDevice                 = NULL;
    IAudioClient           *pAudioClient            = NULL;
    WAVEFORMATEX           *pwfx                    = NULL;
//
    WCHAR msg[256] = {0};
    SHORT nChannel = 2;
//
    hr = CoInitialize(NULL);
//
    while (_fPolling == TRUE)
    {
        hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (PVOID*)&pEnumerator);
        EXIT_ON_ERROR_LOOP(hr)
//
        hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
        EXIT_ON_ERROR_LOOP(hr)
//
        hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (PVOID*)&pAudioClient);
        EXIT_ON_ERROR_LOOP(hr)
//
        hr = pAudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, NULL);
//
        hr = pAudioClient->GetMixFormat(&pwfx);
        EXIT_ON_ERROR_LOOP(hr)
//
        StringCbPrintfW(msg, 128, L"FormatTag      : 0x%X\n", pwfx->wFormatTag);
        OutputDebugStringW(msg);
        StringCbPrintfW(msg, 128, L"Channels       : %d\n", pwfx->nChannels);
        OutputDebugStringW(msg);
        StringCbPrintfW(msg, 128, L"SamplePerSec   : %d\n", pwfx->nSamplesPerSec);
        OutputDebugStringW(msg);
        StringCbPrintfW(msg, 128, L"AvfBytesPerSec : %d\n", pwfx->nAvgBytesPerSec);
        OutputDebugStringW(msg);
        StringCbPrintfW(msg, 128, L"nBlockAlign    : %d\n", pwfx->nBlockAlign);
        OutputDebugStringW(msg);
        StringCbPrintfW(msg, 128, L"wBitsPerSample : %d\n", pwfx->wBitsPerSample);
        OutputDebugStringW(msg);
//
        if (_fIsQuery == TRUE)
        {
            RtlCopyMemory(&_Wfx, pwfx, sizeof(WAVEFORMATEX));
//
            _fPolling = FALSE;
            _fIsQuery = FALSE;
            goto ExitPollingAudio;
        };
//
        SHORT devCount = 0;
        EnumAudioDriver(&devCount);
//
        if ((_Wfx.nChannels       != pwfx->nChannels)       ||
            (_Wfx.nSamplesPerSec  != pwfx->nSamplesPerSec)  ||
            (_Wfx.nAvgBytesPerSec != pwfx->nAvgBytesPerSec) ||
            (_Wfx.nBlockAlign     != pwfx->nBlockAlign)     ||
            (_nDevCount != devCount))
        {
            EnterCriticalSection(&_CriticalSection);
//
            _fIsStop = TRUE;
            wprintf(L"\nChannel Count Change, SetTimer, Stop = %d\n", _fIsStop);
            StringCbPrintfW(msg, 128, L"\nChannel Count Change, SetTimer, Stop = %d\n", _fIsStop);
            OutputDebugStringW(msg);
            RtlCopyMemory(&_Wfx, pwfx, sizeof(WAVEFORMATEX));
            _nDevCount = devCount;
            Sleep(1500);
//
            hr = pDevice->GetId(&_ChannelId);
            EXIT_ON_ERROR_LOOP(hr)
            SetEvent(_NotifyAppEvent);
            _fIsStop = FALSE;
//
            HANDLE hThread = ::CreateThread(NULL, 0, AudioThreadProc, NULL, 0, NULL);
            if (hThread)
            {
                CloseHandle(hThread);
            };
//
            LeaveCriticalSection(&_CriticalSection);
        };
//
ExitPollingAudio:
        CoTaskMemFree(pwfx);
        SAFE_RELEASE(pEnumerator)
        SAFE_RELEASE(pDevice)
        SAFE_RELEASE(pAudioClient)
//
        Sleep(1000);
    };
    return 0;
};
//
#if 0
BOOL
FixedWaveformTo16Bits(WAVEFORMATEX *pwfx)
{
    if(pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
        pwfx->wFormatTag = WAVE_FORMAT_PCM;
    else if(pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
        if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat))
        {
            pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
            pEx->Samples.wValidBitsPerSample = 16;
        };
    }
    else
        return FALSE;

    pwfx->wBitsPerSample  = 16;
    pwfx->nBlockAlign     = pwfx->nChannels * pwfx->wBitsPerSample / 8;
    pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
    return TRUE;
};
#endif
//
HRESULT StealAudioStream(VOID)
{
    HRESULT        hr;
    UINT           bufferFrameCount;
    UINT           numFramesAvailable;
    PBYTE          pData;
    DWORD          flags;
    REFERENCE_TIME hnsDefaultDevicePeriod;

    IMMDeviceEnumerator    *pEnumerator             = NULL;
    IMMDeviceCollection    *pCollection             = NULL;
    IMMDevice              *pDevice                 = NULL;
    IAudioClient           *pAudioClient            = NULL;
    IAudioCaptureClient    *pCaptureClient          = NULL;
    WAVEFORMATEX           *pwfx                    = NULL;
    UINT                    packetLength            = 0;
    HANDLE                  hTimerWakeUp            = NULL;
    HANDLE                  waitArray[1]            = {hTimerWakeUp};
    LARGE_INTEGER           liFirstFire;
    UINT                    cDevices;
    LONG                    lTimeBetweenFires;
    PWSTR                   strId;
    std::vector<IMMDevice *> vDevices;
//
    INT   swap1[6] = {0};
    INT   swap2[6] = {0};
    SHORT limit[7] = {0};
    WCHAR msg[256] = {0};
//
    wprintf(L"\nEnter StealAudioStream. Stop = %d\n", _fIsStop);
    StringCbPrintfW(msg, 128, L"\nEnter StealAudioStream. Stop = %d\n", _fIsStop);
    OutputDebugStringW(msg);
//
    hr = CoInitialize(NULL);
//
    hnsDefaultDevicePeriod = 0;
    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (PVOID*)&pEnumerator);
    if (_DEBUG_ENABLE == TRUE)
    {
        if (hr != S_OK)
        {
            wprintf(L"CoCreateInstance failed, Line = %d\n", __LINE__);
            StringCbPrintfW(msg, 128, L"CoCreateInstance failed, Line = %d\n", __LINE__);
            OutputDebugStringW(msg);
        };
    }
    else
        EXIT_ON_ERROR(hr)
//
    hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
    if (_DEBUG_ENABLE == TRUE)
    {
        if (hr != S_OK)
        {
            wprintf(L"EnumAudioEndpoints failed, Line = %d\n", __LINE__);
            StringCbPrintfW(msg, 128, L"EnumAudioEndpoints failed, Line = %d\n", __LINE__);
            OutputDebugStringW(msg);
        };
    }
    else
        EXIT_ON_ERROR(hr)

    hr = pCollection->GetCount(&cDevices);
    if (_DEBUG_ENABLE == TRUE)
    {
        if (hr != S_OK)
        {
            wprintf(L"GetCount failed, Line = %d\n", __LINE__);
            StringCbPrintfW(msg, 128, L"GetCount failed, Line = %d\n", __LINE__);
            OutputDebugStringW(msg);
        };
    }
    else
        EXIT_ON_ERROR(hr)
//
    if (_DEBUG_ENABLE == TRUE)
    {
        wprintf(L"Device count : %d\n", cDevices);
        StringCbPrintfW(msg, 128, L"Device count : %d, Line : %d\n", cDevices, __LINE__);
        OutputDebugStringW(msg);
    };
//
    for (UINT i = 0; i < cDevices; ++i)
    {
        hr = pCollection->Item(i, &pDevice);
        if (_DEBUG_ENABLE == TRUE)
        {
            if (hr != S_OK)
            {
                wprintf(L"pCollection->Item failed, Line = %d\n", __LINE__);
                StringCbPrintfW(msg, 128, L"pCollection->Item failed, Line = %d\n", __LINE__);
                OutputDebugStringW(msg);
            };
        }
        else
            EXIT_ON_ERROR(hr)
//
        hr = pDevice->GetId(&strId);
        if (_DEBUG_ENABLE == TRUE)
        {
            if (hr != S_OK)
            {
                wprintf(L"pDevice->GetId failed, Line = %d\n", __LINE__);
                StringCbPrintfW(msg, 128, L"pDevice->GetId failed, Line = %d\n", __LINE__);
                OutputDebugStringW(msg);
            };
        }
        else
            EXIT_ON_ERROR(hr)
//
        vDevices.push_back(pDevice);
        if (strId != nullptr)
        {
            std::wcout << L"\tDevice " << i << ": " << strId << std::endl;
            StringCbPrintfW(msg, 128, L"\tDevice %d : %s\n", i, strId);
            OutputDebugStringW(msg);
            CoTaskMemFree(strId);
            strId = nullptr;
        };
        pDevice = nullptr;
    };
    std::wcout << std::endl;
//
#if 0
    if (vDevices.empty())
    {
        hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
        EXIT_ON_ERROR(hr)
    }
    else
        std::swap(pDevice, vDevices[0]);
#else
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (_DEBUG_ENABLE == TRUE)
    {
        if (hr != S_OK)
        {
            wprintf(L"GetDefaultAudioEndpoint failed, Line = %d\n", __LINE__);
            StringCbPrintfW(msg, 128, L"GetDefaultAudioEndpoint failed, Line = %d\n", __LINE__);
            OutputDebugStringW(msg);
        };
    }
    else
        EXIT_ON_ERROR(hr)
#endif
//
    hr = pDevice->GetId(&strId);
    if (_DEBUG_ENABLE == TRUE)
    {
        if (hr != S_OK)
        {
            wprintf(L"GetId failed, Line = %d\n", __LINE__);
            StringCbPrintfW(msg, 128, L"GetId failed, Line = %d\n", __LINE__);
            OutputDebugStringW(msg);
        };
    }
    else
        EXIT_ON_ERROR(hr)

    if (strId != nullptr)
    {
        std::wcout << L"Default device: " << strId << std::endl;
        CoTaskMemFree(strId);
        strId = nullptr;
    };
//
    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (PVOID*)&pAudioClient);
    if (_DEBUG_ENABLE == TRUE)
    {
        if (hr != S_OK)
        {
            wprintf(L"Activate failed, Line = %d\n", __LINE__);
            StringCbPrintfW(msg, 128, L"Activate failed, Line = %d\n", __LINE__);
            OutputDebugStringW(msg);
        };
    }
    else
        EXIT_ON_ERROR(hr)
//
    hr = pAudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, NULL);
//
    hr = pAudioClient->GetMixFormat(&pwfx);
    if (_DEBUG_ENABLE == TRUE)
    {
        if (hr != S_OK)
        {
            wprintf(L"GetMixFormat failed, Line = %d\n", __LINE__);
            StringCbPrintfW(msg, 128, L"GetMixFormat failed, Line = %d\n", __LINE__);
            OutputDebugStringW(msg);
        };
    }
    else
        EXIT_ON_ERROR(hr)
//
//    if (FixedWaveformTo16Bits(pwfx) == FALSE)
//        return E_FAIL;
//
    hTimerWakeUp = ::CreateWaitableTimer(NULL, FALSE, NULL);

    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pwfx, NULL);
    if (_DEBUG_ENABLE == TRUE)
    {
        if (hr != S_OK)
        {
            wprintf(L"Initialize failed, Line = %d\n", __LINE__);
            StringCbPrintfW(msg, 128, L"Initialize failed, Line = %d\n", __LINE__);
            OutputDebugStringW(msg);
        };
    }
    else
        EXIT_ON_ERROR(hr)
//
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    if (_DEBUG_ENABLE == TRUE)
    {
        if (hr != S_OK)
        {
            wprintf(L"GetBufferSize failed, Line = %d\n", __LINE__);
            StringCbPrintfW(msg, 128, L"GetBufferSize failed, Line = %d\n", __LINE__);
            OutputDebugStringW(msg);
        };
    }
    else
        EXIT_ON_ERROR(hr)
//
    hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);
    if (_DEBUG_ENABLE == TRUE)
    {
        if (hr != S_OK)
        {
            wprintf(L"GetService failed, Line = %d\n", __LINE__);
            StringCbPrintfW(msg, 128, L"GetService failed, Line = %d\n", __LINE__);
            OutputDebugStringW(msg);
        };
    }
    else
        EXIT_ON_ERROR(hr)
//
    DWORD nTaskIndex = 0;
    HANDLE hTask = ::AvSetMmThreadCharacteristicsW(L"Audio", &nTaskIndex);
    if (hTask == NULL)
    {
        DWORD dwErr = ::GetLastError();
        return HRESULT_FROM_WIN32(dwErr);
    };
//
    liFirstFire.QuadPart = -hnsDefaultDevicePeriod / 2;
    lTimeBetweenFires    = (LONG)hnsDefaultDevicePeriod / 2 / (10 * 1000);
    ::SetWaitableTimer(hTimerWakeUp, &liFirstFire, lTimeBetweenFires, NULL, NULL, FALSE);
//
    {
        StringCbPrintfW(msg, 128, L"0x%X", pwfx->wFormatTag);
        wprintf(msg);
        OutputDebugStringW(msg);
        StringCbPrintfW(msg, 128, L"0x%X", pwfx->nChannels);
        wprintf(msg);
        OutputDebugStringW(msg);
        StringCbPrintfW(msg, 128, L"0x%X", pwfx->nSamplesPerSec);
        wprintf(msg);
        OutputDebugStringW(msg);
        StringCbPrintfW(msg, 128, L"0x%X", pwfx->nAvgBytesPerSec);
        wprintf(msg);
        OutputDebugStringW(msg);
        StringCbPrintfW(msg, 128, L"0x%X", pwfx->nBlockAlign);
        wprintf(msg);
        OutputDebugStringW(msg);
        StringCbPrintfW(msg, 128, L"0x%X", pwfx->wBitsPerSample);
        wprintf(msg);
        OutputDebugStringW(msg);
    };
//
    _pFFTrans    = new FFTransform(pwfx->nSamplesPerSec, AUDIO_BUFFER_SIZE / (pwfx->wBitsPerSample/8));
    _pSampleIter = new SampleIter();
//
    hr = pAudioClient->Start();
    if (_DEBUG_ENABLE == TRUE)
    {
        if (hr != S_OK)
        {
            wprintf(L"Start failed, Line = %d\n", __LINE__);
            StringCbPrintfW(msg, 128, L"Start failed, Line = %d\n", __LINE__);
            OutputDebugStringW(msg);
        };
    }
    else
        EXIT_ON_ERROR(hr)
//
    while (_fIsStop == FALSE)
    {
        WaitForMultipleObjects(sizeof(waitArray) / sizeof(waitArray[0]), waitArray, FALSE, INFINITE);
//
        hr = pCaptureClient->GetNextPacketSize(&packetLength);
        if (_DEBUG_ENABLE == TRUE)
        {
            if (hr != S_OK)
            {
                wprintf(L"GetNextPacketSize, Line = %d\n", __LINE__);
                StringCbPrintfW(msg, 128, L"GetNextPacketSize, Line = %d\n", __LINE__);
                OutputDebugStringW(msg);
            };
        }
        else
            EXIT_ON_ERROR(hr)
//
        while (packetLength != 0 && _fIsStop == FALSE)
        {
            hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
            if (_DEBUG_ENABLE == TRUE)
            {
                if (hr != S_OK)
                {
                    wprintf(L"GetBuffer, Line = %d\n", __LINE__);
                    StringCbPrintfW(msg, 128, L"GetBuffer, Line = %d\n", __LINE__);
                    OutputDebugStringW(msg);
                };
            }
            else
                EXIT_ON_ERROR(hr)
//
            if (flags != 0)
            {
                hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
                if (_DEBUG_ENABLE == TRUE)
                {
                    if (hr != S_OK)
                    {
                        wprintf(L"ReleaseBuffer, Line = %d\n", __LINE__);
                        StringCbPrintfW(msg, 128, L"ReleaseBuffer, Line = %d\n", __LINE__);
                        OutputDebugStringW(msg);
                    };
                }
                else
                    EXIT_ON_ERROR(hr)
//
                hr = pCaptureClient->GetNextPacketSize(&packetLength);
                if (_DEBUG_ENABLE == TRUE)
                {
                    if (hr != S_OK)
                    {
                        wprintf(L"GetNextPacketSize, Line = %d\n", __LINE__);
                        StringCbPrintfW(msg, 128, L"GetNextPacketSize, Line = %d\n", __LINE__);
                        OutputDebugStringW(msg);
                    };
                }
                else
                    EXIT_ON_ERROR(hr)
                continue;
            };
//
            if (_pSampleIter->SetSampleWorkSet((PCHAR)pData, numFramesAvailable, pwfx->wBitsPerSample / 8, TRUE))
            {
                static INT  channels[2][SPECSCOPEWIDTH_STEREO] = { { 0, 0, 0, 0 },
                                                                   { 0, 0, 0, 0 } };
                WorkChannel channelWork[2] = { WRKCHANNEL_LEFT, WRKCHANNEL_RIGHT};
//
                INT chan;
                INT maxFreq;
                INT freqChg;
                INT freqAccum[SPECSCOPEWIDTH_STEREO];
//
                for (chan = 0; chan < 2; chan++)
                {
                    FFTransform* pFFT = _pFFTrans;
//
                    _pSampleIter->SetPos(0);
                    _pSampleIter->SetWorkChannel(channelWork[chan]);
                    _pFFTrans->fftTransformMag(*_pSampleIter, TRUE);
                    maxFreq = (_pSampleIter->Count() / 4);
                    freqChg = maxFreq / (SPECSCOPEWIDTH_STEREO + 1);
//
                    INT    idx;
                    INT    freqIdx    = -2;
                    INT    freqCnt    = 0;
                    INT    accumlator = 0;
                    FLOAT* pMags      = _pFFTrans->fftGetMagArray();
//
                    RtlZeroMemory(freqAccum, sizeof (freqAccum));
                    for (idx = 0; idx < maxFreq; idx++)
                    {
                        if (pMags[idx] > accumlator)
                            accumlator = (INT)pMags[idx];
//
                        freqCnt++;
                        if (freqCnt > freqChg)
                        {
                            freqCnt = 0;
                            freqIdx++;
//
                            if (freqIdx >= 0)
                                freqAccum[freqIdx] = accumlator;
//
                            accumlator = 0;
                        };
                    };
//
                    if (freqIdx < (SPECSCOPEWIDTH_STEREO + 1))
                    {
                        freqIdx++;
                        freqAccum[freqIdx] = accumlator;
                    };
//
                    if (chan == 0)
                    {
                        INT  data  = 0;
//
                        if (_DEBUG_ENABLE == TRUE)
                        {
                            wprintf(L"L==> ");
                            StringCbPrintfW(msg, 128, L"L==> ");
                            OutputDebugStringW(msg);
                        };
//
                        for (idx = 0; idx < SPECSCOPEWIDTH_STEREO; idx++)
                        {
                            if (freqAccum[idx] >= channels[chan][idx])
                                channels[0][idx] = freqAccum[idx];
                            else
                            {
                                channels[0][idx] -= 128;
                                if (channels[0][idx] < 0)
                                    channels[0][idx] = 0;
                            };
//
                            if (_DEBUG_ENABLE == TRUE)
                            {
                                wprintf(L"channel[0][%d] = %d ", idx, channels[0][idx]);
                                StringCbPrintfW(msg, 128, L"channel[0][%d] = %d ", idx, channels[0][idx]);
                                OutputDebugStringW(msg);
                            };
//
                            if (channels[0][idx] == 0 || channels[0][idx] <= 64)//16)
                            {
                                swap1[idx] = 0;
                            }
                            else
                            {
                                data = channels[0][idx] / 64; //16;
                                if (data >= 256)//128)//256)
                                {
                                    data = 6;
                                }
                                else
                                {
                                    data /= 43;//21; //43;
                                };
                                swap1[idx]  = data;
                            };
                        };
//
                        if (_DEBUG_ENABLE == TRUE)
                        {
                            wprintf(L"\n");
                            StringCbPrintfW(msg, 128, L"\n");
                            OutputDebugStringW(msg);
                        };
                    }
                    else if (chan == 1)
                    {
                        INT  data  = 0;
//
                        if (_DEBUG_ENABLE == TRUE)
                        {
                            wprintf(L"R==> ");
                            StringCbPrintfW(msg, 128, L"R==> ");
                            OutputDebugStringW(msg);
                        };
//
                        for (idx = 0; idx < SPECSCOPEWIDTH_STEREO; idx++)
                        {
                            if (freqAccum[idx] >= channels[1][idx])
                                channels[1][idx] = freqAccum[idx];
                            else
                            {
                                channels[1][idx] -= 128;
                                if (channels[1][idx] < 0)
                                    channels[1][idx] = 0;
                            };
//
                            if (_DEBUG_ENABLE == TRUE)
                            {
                                wprintf(L"channel[1][%d] = %d ", idx, channels[0][idx]);
                                StringCbPrintfW(msg, 128, L"channel[0][%d] = %d ", idx, channels[1][idx]);
                                OutputDebugStringW(msg);
                            };
//
                            if (channels[1][idx] == 0 || channels[1][idx] <= 64)//16)
                            {
                                swap2[idx]   = 0;
                            }
                            else
                            {
                                data = channels[1][idx] / 64;//16;
                                if (data >= 256)//128)//256)
                                {
                                    data = 6;
                                }
                                else
                                {
                                    data /= 43;//21;//43;
                                };
                                swap2[idx] = data;
                            };
                        };
//
                        if (_DEBUG_ENABLE == TRUE)
                        {
                            wprintf(L"\n");
                            StringCbPrintfW(msg, 128, L"\n");
                            OutputDebugStringW(msg);
                        };
                    };
//
                    if (swap1[0] == 0 && swap1[1] == 0 && swap1[2] == 0)
                        swap1[0] = swap1[1] = swap1[2] = 0;
                    else
                    {
                        for (idx = 0; idx < 3; idx++)
                        {
                             limit[idx] = swap1[idx];
                        };
                    };
//
                    if (swap2[0] == 0 && swap2[1] == 0 && swap2[2] == 0)
                        swap2[0] = swap2[1] = swap2[2] = 0;
                    else
                    {
                        for (idx = 0; idx < 3; idx++)
                        {
                             limit[idx+4] = swap2[2-idx];
                        };
                    };
                };
//
                if (chan == 2)
                {
#if 0
                    printf("Send command to HID\n");
                    for (int idx = 0; idx < 6; idx++)
                        printf("Data[%d] = %d ", idx, limit[idx]);
                    printf("\n");
#endif
                    HidRgbKeyboard(limit);
                };
            };
            hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
            if (_DEBUG_ENABLE == TRUE)
            {
                if (hr != S_OK)
                {
                    wprintf(L"ReleaseBuffer, Line = %d\n", __LINE__);
                    StringCbPrintfW(msg, 128, L"ReleaseBuffer, Line = %d\n", __LINE__);
                    OutputDebugStringW(msg);
                };
            }
            else
                EXIT_ON_ERROR(hr)
//
            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            if (_DEBUG_ENABLE == TRUE)
            {
                if (hr != S_OK)
                {
                    wprintf(L"GetNextPacketSize, Line = %d\n", __LINE__);
                    StringCbPrintfW(msg, 128, L"GetNextPacketSize, Line = %d\n", __LINE__);
                    OutputDebugStringW(msg);
                };
            }
            else
                EXIT_ON_ERROR(hr)
        };
    };
//
    hr = pAudioClient->Stop();
    if (_DEBUG_ENABLE == TRUE)
    {
        if (hr != S_OK)
        {
            wprintf(L"Stop, Line = %d\n", __LINE__);
            StringCbPrintfW(msg, 128, L"Stop, Line = %d\n", __LINE__);
            OutputDebugStringW(msg);
        };
    }
    else
        EXIT_ON_ERROR(hr)
//
ExitStealAudio:
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pCollection)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pCaptureClient)
//
    CoUninitialize();
#if _DEBUG_ENABLE
    printf("\nLeave StealAudioStream.\n");
#endif
//
    return hr;
};
//
extern "C"
__declspec(dllexport)
BOOL WINAPI
StartMonitorAudio(CONST SHORT fDebugEnable)
{
    UCHAR high, low;
    SHORT devCount;
    BOOL  rtn = FALSE;
    WCHAR msg[256];
//
#if _DEBUG
    __debugbreak();
#endif
//
    EnterCriticalSection(&_CriticalSection);
//
    if (RGBKeybdInitialize(&high, &low) == TRUE)
    {
        if (fDebugEnable)
            _DEBUG_ENABLE = TRUE;
        else
            _DEBUG_ENABLE = FALSE;
//
        if (_DEBUG_ENABLE)
        {
            ::AllocConsole();
            FILE *tempOut;
            FILE *tempIn;
            ::freopen_s(&tempOut, "CONOUT$", "w+t", stdout);
            ::freopen_s(&tempIn, "CONIN$","r+t",stdin);
        };
//
        devCount = 0;
        EnumAudioDriver(&devCount);
        _nDevCount = devCount;
//
        _fIsQuery = TRUE;
        _fPolling = TRUE;
        GetCurrentChannelCount(NULL);
//
        _fPolling = TRUE;
        HANDLE pollthread = ::CreateThread(NULL, 0, GetCurrentChannelCount, NULL, 0, NULL);
        if (pollthread)
            ::CloseHandle(pollthread);
        else
            return FALSE;
//
        _fUsedHID = TRUE;
        _fIsStop  = FALSE;
//
        HANDLE thread = ::CreateThread(NULL, 0, AudioThreadProc, NULL, 0, NULL);
        if (thread)
        {
            ::CloseHandle(thread);
           wprintf(L"Exit StartMonitorAudio\n");
           StringCbPrintfW(msg, 128, L"Exit StartMonitorAudio\n");
           OutputDebugStringW(msg);
           rtn = TRUE;
        };
    };
//
    LeaveCriticalSection(&_CriticalSection);
//
    return rtn;
};
//
extern "C"
__declspec(dllexport)
VOID WINAPI StopMonitorAudio(VOID)
{
    EnterCriticalSection(&_CriticalSection);
//
    _fIsStop     = TRUE;
    _fPolling    = FALSE;
    _fIsOtherDev = FALSE;
    FreeResource();
//
    DisableLedKeybd();
    if (_hKeybdDevice != INVALID_HANDLE_VALUE)
        CloseHandle(_hKeybdDevice);
    _hKeybdDevice = INVALID_HANDLE_VALUE;
//
    LeaveCriticalSection(&_CriticalSection);
    return;
};
//
extern "C"
__declspec(dllexport)
BOOL WINAPI NotifyDeviceChnage(VOID)
{
    EnterCriticalSection(&_CriticalSection);
//
    RtlZeroMemory(&_Wfx, sizeof(WAVEFORMATEX));
//
    LeaveCriticalSection(&_CriticalSection);
//
    return TRUE;
};
//
extern "C"
__declspec(dllexport)
VOID WINAPI GetDefaultActiveDeviceString(PWCHAR DefaultString)
{
    size_t len;
//
    StringCbLengthW(_ChannelId, 256, &len);
    RtlCopyMemory(DefaultString, _ChannelId, len + sizeof(UNICODE_NULL));
    return;
};
//
extern "C"
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL,
                    _In_ DWORD     fdwReason,
                    _In_ LPVOID    lpvReserved)
{
    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            InitializeCriticalSectionAndSpinCount(&_CriticalSection, 0x00000400);
        break;
//
        case DLL_PROCESS_DETACH:
            DeleteCriticalSection(&_CriticalSection);
//            _hInstance = hinstDLL;
        break;
    };
    return TRUE;
};