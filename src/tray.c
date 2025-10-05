//tray.c
#include <windows.h>
#include "tray.h"

void InitTrayIcon(HWND hwnd, HINSTANCE hInstance)
{
    NOTIFYICONDATA nid = {0};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = TRAY_UID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;
    HICON hIcon = (HICON)LoadImage(NULL, "assets/icon.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
    if (!hIcon) hIcon = LoadIcon(NULL, IDI_APPLICATION);
    nid.hIcon = hIcon;
    lstrcpy(nid.szTip, "1Shot");
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void RemoveTrayIcon(HWND hwnd)
{
    NOTIFYICONDATA nid = {0};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = TRAY_UID;
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void ShowTrayMenu(HWND hwnd)
{
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, TRAY_ABOUT, "About");
    AppendMenu(hMenu, MF_STRING, TRAY_EXIT, "Exit");

    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(hMenu);
}
