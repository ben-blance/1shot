//ui.h
#ifndef UI_H
#define UI_H

#include <windows.h>

void InitUI(HWND hwnd_parent);
void ShowUI();
void RenderUI(HWND hwnd);
int GetAutoStartToggle();
void SetAutoStartToggle(int value);

#endif