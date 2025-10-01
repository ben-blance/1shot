//capture.c
#include <windows.h>
#include "capture.h"

HBITMAP CaptureScreen(void)
{
    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int scaledWidth = GetDeviceCaps(hScreenDC, DESKTOPHORZRES);
    int scaledHeight = GetDeviceCaps(hScreenDC, DESKTOPVERTRES);

    if (scaledWidth > 0 && scaledHeight > 0) {
        screenWidth = scaledWidth;
        screenHeight = scaledHeight;
    }

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenWidth, screenHeight);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

    SetStretchBltMode(hMemoryDC, HALFTONE);
    StretchBlt(hMemoryDC, 0, 0, screenWidth, screenHeight,
               hScreenDC, 0, 0, screenWidth, screenHeight, SRCCOPY);

    SelectObject(hMemoryDC, hOldBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    return hBitmap;
}

void CopyToClipboard(HBITMAP hBitmap)
{
    if (!hBitmap) return;
    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        SetClipboardData(CF_BITMAP, hBitmap);
        CloseClipboard();
    } else {
        DeleteObject(hBitmap);
    }
}

HBITMAP CaptureScreenRegion(RECT r)
{
    if (r.right <= r.left || r.bottom <= r.top) return NULL;

    int w = r.right - r.left;
    int h = r.bottom - r.top;

    HDC hScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, w, h);
    HBITMAP hOld = (HBITMAP)SelectObject(hMemDC, hBitmap);

    BitBlt(hMemDC, 0, 0, w, h, hScreenDC, r.left, r.top, SRCCOPY);

    SelectObject(hMemDC, hOld);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);

    return hBitmap;
}
