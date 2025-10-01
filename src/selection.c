#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <stdint.h>
#include "selection.h"
#include "capture.h"

static HWND g_hwndOverlay = NULL;
static POINT g_ptStart = {0,0};
static POINT g_ptEnd = {0,0};
static BOOL g_dragging = FALSE;

static int virtX = 0, virtY = 0, virtW = 0, virtH = 0;

static void EnsureDPIAwareness()
{
    HMODULE hUser = LoadLibraryA("user32.dll");
    if (hUser) {
        typedef BOOL (WINAPI *SetProcessDPIAware_t)(void);
        SetProcessDPIAware_t p = (SetProcessDPIAware_t)GetProcAddress(hUser, "SetProcessDPIAware");
        if (p) p();
        FreeLibrary(hUser);
    }
}

static void UpdateLayeredOverlay(RECT *selRect)
{
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = virtW;
    bmi.bmiHeader.biHeight = -virtH; 
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void *bits = NULL;
    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP hDib = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    HBITMAP hOld = (HBITMAP)SelectObject(memDC, hDib);

    uint32_t *px = (uint32_t *)bits;
    uint32_t fill = (160U << 24) | (0x00 << 16) | (0x00 << 8) | 0x00; 
    int total = virtW * virtH;
    for (int i = 0; i < total; ++i) px[i] = fill;

    if (selRect) {
        int left = selRect->left - virtX;
        int top  = selRect->top  - virtY;
        int right = selRect->right - virtX;
        int bottom = selRect->bottom - virtY;
        if (left < 0) left = 0;
        if (top < 0) top = 0;
        if (right > virtW) right = virtW;
        if (bottom > virtH) bottom = virtH;

        for (int y = top; y < bottom; ++y) {
            uint32_t *row = px + y * virtW;
            for (int x = left; x < right; ++x) row[x] = 0x00000000;
        }

        uint32_t border = (255U << 24) | (255U << 16) | (255U << 8) | 255U;
        int thickness = 2;
        for (int t = 0; t < thickness; ++t) {
            int ytop = top + t;
            int ybot = bottom - 1 - t;
            if (ytop >= 0 && ytop < virtH) {
                uint32_t *row = px + ytop * virtW;
                for (int x = left; x < right; ++x) row[x] = border;
            }
            if (ybot >= 0 && ybot < virtH) {
                uint32_t *row = px + ybot * virtW;
                for (int x = left; x < right; ++x) row[x] = border;
            }
        }
        for (int t = 0; t < thickness; ++t) {
            int xleft = left + t;
            int xright = right - 1 - t;
            if (xleft >= 0 && xleft < virtW) {
                for (int y = top; y < bottom; ++y) px[y * virtW + xleft] = border;
            }
            if (xright >= 0 && xright < virtW) {
                for (int y = top; y < bottom; ++y) px[y * virtW + xright] = border;
            }
        }
    }

    SIZE sz = { virtW, virtH };
    POINT ptSrc = { 0, 0 };
    POINT ptWinPos = { virtX, virtY };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    UpdateLayeredWindow(g_hwndOverlay, screenDC, &ptWinPos, &sz, memDC, &ptSrc, 0, &bf, ULW_ALPHA);

    SelectObject(memDC, hOld);
    DeleteObject(hDib);
    DeleteDC(memDC);
    ReleaseDC(NULL, screenDC);
}

LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_CREATE:
            return 0;

        case WM_LBUTTONDOWN: {
            POINT p; GetCursorPos(&p);
            g_ptStart = p;
            g_ptEnd = p;
            g_dragging = TRUE;
            SetCapture(hwnd);
            RECT sel = { g_ptStart.x, g_ptStart.y, g_ptEnd.x, g_ptEnd.y };
            UpdateLayeredOverlay(&sel);
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (g_dragging) {
                POINT p; GetCursorPos(&p);
                g_ptEnd = p;
                RECT sel = {
                    min(g_ptStart.x, g_ptEnd.x),
                    min(g_ptStart.y, g_ptEnd.y),
                    max(g_ptStart.x, g_ptEnd.x),
                    max(g_ptStart.y, g_ptEnd.y)
                };
                UpdateLayeredOverlay(&sel);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_dragging) {
                g_dragging = FALSE;
                ReleaseCapture();
                POINT p; GetCursorPos(&p);
                g_ptEnd = p;
                RECT selRect = {
                    min(g_ptStart.x, g_ptEnd.x),
                    min(g_ptStart.y, g_ptEnd.y),
                    max(g_ptStart.x, g_ptEnd.x),
                    max(g_ptStart.y, g_ptEnd.y)
                };

                // Hide & destroy overlay first
                ShowWindow(hwnd, SW_HIDE);
                DestroyWindow(hwnd);
                g_hwndOverlay = NULL;

                // small pause to ensure the overlay window is fully removed
                Sleep(30);

                if ((selRect.right - selRect.left) > 1 && (selRect.bottom - selRect.top) > 1) {
                    HBITMAP bmp = CaptureScreenRegion(selRect);
                    CopyToClipboard(bmp);
                }
            }
            return 0;
        }

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                ShowWindow(hwnd, SW_HIDE);
                DestroyWindow(hwnd);
                g_hwndOverlay = NULL;
            }
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void StartSelectionMode(HWND hwndParent)
{
    if (g_hwndOverlay) return;

    EnsureDPIAwareness();

    virtX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    virtY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    virtW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    virtH = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASS wc = {0};
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "OneShotSelectionOverlay";
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    RegisterClass(&wc);

    g_hwndOverlay = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        "OneShotSelectionOverlay", NULL,
        WS_POPUP,
        virtX, virtY, virtW, virtH,
        NULL, NULL, hInst, NULL
    );

    LONG ex = GetWindowLong(g_hwndOverlay, GWL_EXSTYLE);
    ex &= ~WS_EX_TRANSPARENT;
    SetWindowLong(g_hwndOverlay, GWL_EXSTYLE, ex);

    ShowWindow(g_hwndOverlay, SW_SHOW);
    SetForegroundWindow(g_hwndOverlay);

    UpdateLayeredOverlay(NULL);
}
