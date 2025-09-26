//ui.h
#ifndef UI_H
#define UI_H

#include <windows.h>

void InitUI(HWND hwnd_parent);
void ShowUI();
void RenderUI(HWND hwnd);
int GetAutoStartToggle();     // Changed from nk_bool to int
void SetAutoStartToggle(int value);  // Changed from nk_bool to int

#endif