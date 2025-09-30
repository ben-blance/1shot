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
    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        SetClipboardData(CF_BITMAP, hBitmap);
        CloseClipboard();
    }
}