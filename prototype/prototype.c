#include <windows.h>

HBITMAP CaptureScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);
    SelectObject(hDC, hBitmap);
    BitBlt(hDC, 0, 0, width, height, hScreen, 0, 0, SRCCOPY);

    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    return hBitmap;
}

void CopyToClipboard(HBITMAP hBitmap)
{
    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        SetClipboardData(CF_BITMAP, hBitmap);
        CloseClipboard();
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
    // Register global hotkey Alt+X
    if (!RegisterHotKey(NULL, 1, MOD_ALT, 'X')) {
        MessageBox(NULL, "Failed to register hotkey", "Error", MB_OK);
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            HBITMAP shot = CaptureScreen();
            CopyToClipboard(shot);
        }
    }
    return 0;
}
