//main.c
#include <windows.h>
#include "ui.h"
#include "capture.h"
#include "tray.h"

#define HOTKEY_ID 1

HWND hwndMain = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_HOTKEY:
            if(wParam == HOTKEY_ID) {
                HBITMAP shot = CaptureScreen();
                CopyToClipboard(shot);
                // Optional: Show a brief notification
                // MessageBox(NULL, "Screenshot copied to clipboard!", "1Shot", MB_OK | MB_ICONINFORMATION);
            }
            break;

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

        case WM_USER + 1: // Tray callback
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
    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "OneShotTrayClass";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    // Create main window (hidden)
    hwndMain = CreateWindow("OneShotTrayClass", "1Shot", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
                            NULL, NULL, hInstance, NULL);

    if (!hwndMain) {
        MessageBox(NULL, "Failed to create main window", "Error", MB_OK);
        return 1;
    }

    // Initialize tray icon
    InitTrayIcon(hwndMain, hInstance);

    // Register global hotkey Alt+X
    if(!RegisterHotKey(hwndMain, HOTKEY_ID, MOD_ALT, 'X')) {
        MessageBox(NULL, "Failed to register hotkey Alt+X", "Error", MB_OK);
        return 1;
    }

    // Message loop
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        // Update UI if visible
        RenderUI(NULL);
    }

    // Cleanup
    UnregisterHotKey(hwndMain, HOTKEY_ID);
    return (int)msg.wParam;
}