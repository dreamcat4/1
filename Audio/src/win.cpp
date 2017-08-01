#include "audiostealer.h"
//
#define WND_CLASS_NAME                      L"DeviceChangeWindowClass"
//
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE, 0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, \
             0xC0, 0x4F, 0xB9, 0x51, 0xED);
//
static BOOL          _fTimer        = FALSE;
static SHORT         _nDevCount     = 0;
static HDEVNOTIFY    _hDeviceNotify = 0;
HWND                 _hwndDlg   = 0;
//
BOOL InitWindowClass()
{
    WNDCLASSEX wndClass;
//
    wndClass.cbSize        = sizeof(WNDCLASSEX);
    wndClass.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wndClass.hInstance     = _hInstance; //reinterpret_cast<HINSTANCE>(GetModuleHandle(0));
    wndClass.lpfnWndProc   = reinterpret_cast<WNDPROC>(WinProcCallback);
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hIcon         = LoadIcon(0, IDI_APPLICATION);
    wndClass.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    wndClass.hCursor       = LoadCursor(0, IDC_ARROW);
    wndClass.lpszClassName = WND_CLASS_NAME;
    wndClass.lpszMenuName  = NULL;
    wndClass.hIconSm       = wndClass.hIcon;
    if (!RegisterClassEx(&wndClass))
    {
        return FALSE;
    };
    return TRUE;
};
//
BOOL
DoRegisterDeviceInterfaceToHwnd(VOID)
{
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    RtlZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
//
    NotificationFilter.dbcc_size = sizeof(NotificationFilter);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_reserved   = 0;
    NotificationFilter.dbcc_classguid  = GUID_DEVINTERFACE_USB_DEVICE;
    _hDeviceNotify = ::RegisterDeviceNotification(_hwndDlg, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (_hDeviceNotify == NULL) 
        return FALSE;
    return TRUE;
};
//
BOOL WINAPI MakeNewWindowMessageLoop(VOID)
{
    if (InitWindowClass() == FALSE)
        return FALSE;
//
    HWND hWnd = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_APPWINDOW,
                               WND_CLASS_NAME,
                               L"DeviceChangeWindow",
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT, 
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               NULL,
                               NULL, 
                               _hInstance, 
                               NULL);
    if (hWnd == NULL)
    {
        return FALSE;
    };
//
    ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);
//
    MSG msg; 
    int retVal;
//
    while ((retVal = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (retVal == -1)
        {
            break;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        };
    };
    return 1;
};
//
VOID CALLBACK DevChangeTimerProc(_In_ HWND     hwnd,
                                 _In_ UINT     uMsg,
                                 _In_ UINT_PTR idEvent,
                                 _In_ DWORD    dwTime)
{
    _fIsStop = TRUE;
    FreeResource();
    _nDevCount++;
    printf("Process Timer Event(DevChange).\n");
    if (_nDevCount >= 3)
    {
        SHORT limit[6] = {0};
//
        _fIsStop = FALSE;
        _fTimer  = FALSE;
        KillTimer(hwnd, 1001);
        HidRgbKeyboard(limit);
        HANDLE hThread = ::CreateThread(NULL, 0, AudioThreadProc, NULL, 0, NULL);
        if (hThread)
        {
//           ::SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
//           ::ResumeThread(hThread);
            CloseHandle(hThread);
        };
    };
};
//

INT_PTR WINAPI
WinProcCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRet = 1;
//
    switch (message)
    {
        case WM_CREATE:
            _hwndDlg = hWnd;
            DoRegisterDeviceInterfaceToHwnd();
        break;
//
        case WM_RESET_AUDIO_STEALER_MESSAGE:
            if (_fTimer == FALSE)
            {
                _nDevCount = 0;
                SetTimer(hWnd, 1001, 300, DevChangeTimerProc);
                _fTimer = TRUE;
            };
        break;
//
        case WM_DEVICECHANGE:
        {
            PDEV_BROADCAST_DEVICEINTERFACE b = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;
//
            switch (wParam)
            {
                case DBT_DEVICEARRIVAL:
                case DBT_DEVICEREMOVECOMPLETE:
                case DBT_DEVNODES_CHANGED:
                {
                    if (_fTimer == FALSE)
                    {
                        _nDevCount = 0;
                        SetTimer(hWnd, 1001, 300, DevChangeTimerProc);
                        _fTimer = TRUE;
                    };
                };
            };
        };
        break;
//
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
//
        default:
            lRet = DefWindowProc(hWnd, message, wParam, lParam);
        break;
    };
//
    return lRet;
};