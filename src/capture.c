//capture.c
#include <windows.h>
#include "capture.h"

HBITMAP CaptureScreen(void)
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
