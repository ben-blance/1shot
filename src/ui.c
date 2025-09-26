//ui.c - Simple Win32 UI (no Nuklear to avoid font issues)
#include "ui.h"
#include <windows.h>

static int auto_start = 0; // Using int instead of nk_bool
static HWND hwnd_ui = NULL;
static HWND hwnd_checkbox = NULL;
static int ui_width = 320, ui_height = 150;

#define ID_CHECKBOX_AUTOSTART 1001

static LRESULT CALLBACK UIWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg) {
        case WM_CREATE: {
            // Create checkbox for auto-start
            hwnd_checkbox = CreateWindow(
                "BUTTON", "Start with Windows",
                WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
                20, 30, 200, 25,
                hwnd, (HMENU)ID_CHECKBOX_AUTOSTART,
                GetModuleHandle(NULL), NULL
            );
            
            // Set checkbox state
            SendMessage(hwnd_checkbox, BM_SETCHECK, auto_start ? BST_CHECKED : BST_UNCHECKED, 0);
            
            // Create hotkey label
            CreateWindow(
                "STATIC", "Hotkey: Alt+X",
                WS_VISIBLE | WS_CHILD,
                20, 60, 200, 20,
                hwnd, NULL, GetModuleHandle(NULL), NULL
            );
            
            // Create info label
            CreateWindow(
                "STATIC", "Right-click tray icon to open this window",
                WS_VISIBLE | WS_CHILD,
                20, 85, 280, 20,
                hwnd, NULL, GetModuleHandle(NULL), NULL
            );
            break;
        }
        
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_CHECKBOX_AUTOSTART) {
                auto_start = (SendMessage(hwnd_checkbox, BM_GETCHECK, 0, 0) == BST_CHECKED);
                // Here you could add registry code to actually implement auto-start
            }
            break;
        }
        
        case WM_CLOSE: {
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
        
        case WM_DESTROY: {
            hwnd_ui = NULL;
            break;
        }
        
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void InitUI(HWND hwnd_parent)
{
    if(hwnd_ui) return;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = UIWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "OneShotUIClass";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    hwnd_ui = CreateWindow("OneShotUIClass", "1Shot Settings",
                           WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
                           CW_USEDEFAULT, CW_USEDEFAULT, ui_width, ui_height,
                           hwnd_parent, NULL, GetModuleHandle(NULL), NULL);
}

void ShowUI()
{
    if(hwnd_ui) {
        ShowWindow(hwnd_ui, SW_SHOW);
        SetForegroundWindow(hwnd_ui);
    }
}

void RenderUI(HWND hwnd)
{
    // No rendering needed for native Win32 controls
    if(hwnd_ui && IsWindowVisible(hwnd_ui)) {
        UpdateWindow(hwnd_ui);
    }
}

int GetAutoStartToggle() { return auto_start; } // Changed return type
void SetAutoStartToggle(int value) { 
    auto_start = value; 
    if (hwnd_checkbox) {
        SendMessage(hwnd_checkbox, BM_SETCHECK, value ? BST_CHECKED : BST_UNCHECKED, 0);
    }
}