#ifndef TRAY_H
#define TRAY_H
#include <windows.h>

#define TRAY_UID 1001
#define TRAY_EXIT 2001
#define TRAY_ABOUT 2002

void InitTrayIcon(HWND hwnd, HINSTANCE hInstance);
void RemoveTrayIcon(HWND hwnd);
void ShowTrayMenu(HWND hwnd);

#endif
