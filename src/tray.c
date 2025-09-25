#include <windows.h>
#include "tray.h"

#define TRAY_UID 1001

void InitTrayIcon(HWND hwnd, HINSTANCE hInstance)
{
    NOTIFYICONDATA nid = {0};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = TRAY_UID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;
    nid.hIcon = LoadImage(hInstance, "assets/icon.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
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
