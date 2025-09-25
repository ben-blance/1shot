#include <windows.h>
#include "capture.h"
#include "tray.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_HOTKEY:
        if (wParam == 1) {
            HBITMAP shot = CaptureScreen();
            CopyToClipboard(shot);
        }
        break;
    case WM_USER + 1: // tray callback
        if (lParam == WM_RBUTTONUP) {
            ShowTrayMenu(hwnd);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case TRAY_EXIT:
            RemoveTrayIcon(hwnd);
            PostQuitMessage(0);
            break;
        case TRAY_ABOUT:
            MessageBox(NULL, "1Shot v1.0\nQuick screenshot tool", "About", MB_OK | MB_ICONINFORMATION);
            break;
        }
        break;
    case WM_DESTROY:
        RemoveTrayIcon(hwnd);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "OneShotClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("OneShotClass", "1Shot", 0, 0, 0, 0, 0,
                             HWND_MESSAGE, NULL, hInstance, NULL);

    if (!RegisterHotKey(hwnd, 1, MOD_ALT, 'X')) {
        MessageBox(NULL, "Failed to register hotkey", "Error", MB_OK);
        return 1;
    }

    InitTrayIcon(hwnd, hInstance);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
