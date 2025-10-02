//main.c
#include <windows.h>
#include "ui.h"
#include "capture.h"
#include "tray.h"
#include "selection.h"

#define HOTKEY_ID 1
#define WM_SHORTSHOT (WM_USER + 2)
#define WM_START_SELECTION (WM_USER + 3)

HWND hwndMain = NULL;
static volatile BOOL g_monitorActive = FALSE;
static volatile BOOL g_keyReleased = FALSE;

DWORD WINAPI HoldMonitorThread(LPVOID lpParam)
{
    HWND hwnd = (HWND)lpParam;
    const int timeout_ms = 1000;
    const int poll = 10;
    int elapsed = 0;

    g_monitorActive = TRUE;
    g_keyReleased = FALSE;

    while (elapsed < timeout_ms && g_monitorActive) {
        SHORT s = GetAsyncKeyState('X');
        if (!(s & 0x8000)) {
            g_keyReleased = TRUE;
            g_monitorActive = FALSE;
            PostMessage(hwnd, WM_SHORTSHOT, 0, 0);
            return 0;
        }
        Sleep(poll);
        elapsed += poll;
    }

    if (!g_keyReleased) {
        g_monitorActive = FALSE;
        PostMessage(hwnd, WM_START_SELECTION, 0, 0);
    }
    
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_HOTKEY:
            if(wParam == HOTKEY_ID) {
                if (!g_monitorActive) {
                    CreateThread(NULL, 0, HoldMonitorThread, hwnd, 0, NULL);
                }
            }
            break;

        case WM_SHORTSHOT: {
            if (!g_monitorActive) {
                HBITMAP shot = CaptureScreen();
                CopyToClipboard(shot);
            }
            break;
        }

        case WM_START_SELECTION: {
            g_monitorActive = FALSE;
            UnregisterHotKey(hwnd, HOTKEY_ID);
            StartSelectionMode(hwnd);
            break;
        }

        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case TRAY_EXIT:
                    RemoveTrayIcon(hwnd);
                    PostQuitMessage(0);
                    break;
                case TRAY_ABOUT:
                    InitUI(hwnd);
                    ShowUI();
                    break;
            }
            break;

        case WM_USER + 1: 
            if(lParam == WM_RBUTTONUP) {
                ShowTrayMenu(hwnd);
            }
            break;

        case WM_DESTROY:
            RemoveTrayIcon(hwnd);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "OneShotTrayClass";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    hwndMain = CreateWindow("OneShotTrayClass", "1Shot", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
                            NULL, NULL, hInstance, NULL);

    if (!hwndMain) {
        MessageBox(NULL, "Failed to create main window", "Error", MB_OK);
        return 1;
    }

    InitTrayIcon(hwndMain, hInstance);

    if(!RegisterHotKey(hwndMain, HOTKEY_ID, MOD_ALT, 'X')) {
        MessageBox(NULL, "Failed to register hotkey Alt+X", "Error", MB_OK);
        return 1;
    }

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterHotKey(hwndMain, HOTKEY_ID);
    return (int)msg.wParam;
}