//ui.c
#include "ui.h"
#include <windows.h>
#include <commctrl.h>

static int auto_start = 0;
static HWND hwnd_ui = NULL;
static HWND hwnd_toggle = NULL;
static HWND hwnd_label1 = NULL;
static HWND hwnd_label2 = NULL;
static HWND hwnd_label3 = NULL;
static HWND hwnd_github = NULL;
static int ui_width = 380, ui_height = 200;
static BOOL toggle_state = FALSE;
static BOOL is_hovering = FALSE;
static BOOL github_hovering = FALSE;

#define ID_TOGGLE_AUTOSTART 1001
#define ID_GITHUB_LINK 1002
#define REGISTRY_KEY "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
#define APP_NAME "1Shot"

static BOOL SetAutoStartRegistry(BOOL enable);
static BOOL GetAutoStartRegistry(void);
static void DrawToggleSwitch(HDC hdc, RECT rect, BOOL state, BOOL hover);
static void DrawGitHubButton(HDC hdc, RECT rect, BOOL hover);

static HFONT CreateModernFont(int size, BOOL bold)
{
    return CreateFont(
        size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, 
        FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        "Segoe UI"
    );
}

static BOOL SetAutoStartRegistry(BOOL enable)
{
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, REGISTRY_KEY, 0, KEY_SET_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        return FALSE;
    }
    
    if (enable) {
        char exePath[MAX_PATH];
        GetModuleFileName(NULL, exePath, MAX_PATH);
        result = RegSetValueEx(hKey, APP_NAME, 0, REG_SZ, (BYTE*)exePath, strlen(exePath) + 1);
    } else {
        result = RegDeleteValue(hKey, APP_NAME);
        if (result == ERROR_FILE_NOT_FOUND) {
            result = ERROR_SUCCESS;
        }
    }
    
    RegCloseKey(hKey);
    return (result == ERROR_SUCCESS);
}

static BOOL GetAutoStartRegistry(void)
{
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, REGISTRY_KEY, 0, KEY_QUERY_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        return FALSE;
    }
    
    DWORD dataSize = 0;
    result = RegQueryValueEx(hKey, APP_NAME, NULL, NULL, NULL, &dataSize);
    RegCloseKey(hKey);
    
    return (result == ERROR_SUCCESS && dataSize > 0);
}

static void DrawToggleSwitch(HDC hdc, RECT rect, BOOL state, BOOL hover)
{
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    COLORREF bgColor;
    if (state) {
        bgColor = hover ? RGB(100, 200, 120) : RGB(76, 175, 80);
    } else {
        bgColor = hover ? RGB(100, 100, 100) : RGB(60, 60, 60);
    }
    
    HBRUSH bgBrush = CreateSolidBrush(bgColor);
    HBRUSH knobBrush = CreateSolidBrush(RGB(255, 255, 255));
    HPEN noPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
    
    SelectObject(hdc, noPen);
    SelectObject(hdc, bgBrush);
    
    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, height, height);
    
    SelectObject(hdc, knobBrush);
    
    int knobSize = height - 4;
    int knobX = state ? (rect.right - knobSize - 2) : (rect.left + 2);
    int knobY = rect.top + 2;
    
    Ellipse(hdc, knobX, knobY, knobX + knobSize, knobY + knobSize);
    
    DeleteObject(bgBrush);
    DeleteObject(knobBrush);
    DeleteObject(noPen);
}

static void DrawGitHubButton(HDC hdc, RECT rect, BOOL hover)
{
    COLORREF bgColor = hover ? RGB(40, 50, 50) : RGB(30, 42, 42);
    HBRUSH bgBrush = CreateSolidBrush(bgColor);
    HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
    
    SelectObject(hdc, borderPen);
    SelectObject(hdc, bgBrush);
    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 8, 8);
    
    HICON githubIcon = (HICON)LoadImage(NULL, "assets/github.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
    if (!githubIcon) {
        githubIcon = LoadIcon(NULL, IDI_INFORMATION);
    }
    
    DrawIconEx(hdc, rect.left + 10, rect.top + 7, githubIcon, 16, 16, 0, NULL, DI_NORMAL);
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, hover ? RGB(120, 220, 150) : RGB(100, 200, 120));
    
    HFONT font = CreateModernFont(-12, FALSE);
    HFONT oldFont = SelectObject(hdc, font);
    
    RECT textRect = {rect.left + 35, rect.top, rect.right - 10, rect.bottom};
    DrawText(hdc, "Check out the source code on GitHub", -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, oldFont);
    DeleteObject(font);
    DeleteObject(bgBrush);
    DeleteObject(borderPen);
    if (githubIcon) DestroyIcon(githubIcon);
}

static LRESULT CALLBACK ToggleWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            HBRUSH bgBrush = CreateSolidBrush(RGB(24, 37, 37));
            FillRect(hdc, &rect, bgBrush);
            DeleteObject(bgBrush);
            
            DrawToggleSwitch(hdc, rect, toggle_state, is_hovering);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            if (!is_hovering) {
                is_hovering = TRUE;
                InvalidateRect(hwnd, NULL, TRUE);
                
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hwnd;
                TrackMouseEvent(&tme);
            }
            return 0;
        }
        
        case WM_MOUSELEAVE: {
            is_hovering = FALSE;
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            toggle_state = !toggle_state;
            auto_start = toggle_state;
            
            if (SetAutoStartRegistry(toggle_state)) {
                InvalidateRect(hwnd, NULL, TRUE);
            } else {
                toggle_state = !toggle_state;
                auto_start = toggle_state;
                MessageBox(GetParent(hwnd), "Failed to update auto-start setting", "1Shot", MB_OK | MB_ICONERROR);
            }
            return 0;
        }
        
        case WM_SETCURSOR: {
            SetCursor(LoadCursor(NULL, IDC_HAND));
            return TRUE;
        }
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK GitHubWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            HBRUSH bgBrush = CreateSolidBrush(RGB(24, 37, 37));
            FillRect(hdc, &rect, bgBrush);
            DeleteObject(bgBrush);
            
            DrawGitHubButton(hdc, rect, github_hovering);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            if (!github_hovering) {
                github_hovering = TRUE;
                InvalidateRect(hwnd, NULL, TRUE);
                
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hwnd;
                TrackMouseEvent(&tme);
            }
            return 0;
        }
        
        case WM_MOUSELEAVE: {
            github_hovering = FALSE;
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            ShellExecute(NULL, "open", "https://github.com/ben-blance/1shot", NULL, NULL, SW_SHOWNORMAL);
            return 0;
        }
        
        case WM_SETCURSOR: {
            SetCursor(LoadCursor(NULL, IDC_HAND));
            return TRUE;
        }
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK LabelWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            HBRUSH bgBrush = CreateSolidBrush(RGB(24, 37, 37));
            FillRect(hdc, &rect, bgBrush);
            DeleteObject(bgBrush);
            
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255));
            
            char text[256];
            GetWindowText(hwnd, text, sizeof(text));
            
            HFONT font;
            if (hwnd == hwnd_label1) {
                font = CreateModernFont(-16, TRUE);
            } else if (hwnd == hwnd_label2) {
                font = CreateModernFont(-14, FALSE);
                SetTextColor(hdc, RGB(100, 200, 120));
            } else {
                font = CreateModernFont(-12, FALSE);
                SetTextColor(hdc, RGB(180, 180, 180));
            }
            
            HFONT oldFont = SelectObject(hdc, font);
            DrawText(hdc, text, -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, oldFont);
            DeleteObject(font);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_ERASEBKGND: {
            return 1;
        }
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK UIWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg) {
        case WM_CREATE: {
            WNDCLASS labelClass = {0};
            labelClass.lpfnWndProc = LabelWndProc;
            labelClass.hInstance = GetModuleHandle(NULL);
            labelClass.lpszClassName = "ModernLabel";
            labelClass.hbrBackground = CreateSolidBrush(RGB(24, 37, 37));
            RegisterClass(&labelClass);
            
            hwnd_label1 = CreateWindow(
                "ModernLabel", "Start with Windows",
                WS_VISIBLE | WS_CHILD,
                30, 40, 200, 25,
                hwnd, NULL, GetModuleHandle(NULL), NULL
            );
            
            WNDCLASS toggleClass = {0};
            toggleClass.lpfnWndProc = ToggleWndProc;
            toggleClass.hInstance = GetModuleHandle(NULL);
            toggleClass.lpszClassName = "ModernToggle";
            toggleClass.hbrBackground = CreateSolidBrush(RGB(24, 37, 37));
            RegisterClass(&toggleClass);
            
            hwnd_toggle = CreateWindow(
                "ModernToggle", "",
                WS_VISIBLE | WS_CHILD,
                280, 42, 70, 30,
                hwnd, (HMENU)ID_TOGGLE_AUTOSTART,
                GetModuleHandle(NULL), NULL
            );
            
            hwnd_label2 = CreateWindow(
                "ModernLabel", "Hotkey: Alt+X",
                WS_VISIBLE | WS_CHILD,
                30, 90, 200, 20,
                hwnd, NULL, GetModuleHandle(NULL), NULL
            );
            
            WNDCLASS githubClass = {0};
            githubClass.lpfnWndProc = GitHubWndProc;
            githubClass.hInstance = GetModuleHandle(NULL);
            githubClass.lpszClassName = "GitHubButton";
            githubClass.hbrBackground = CreateSolidBrush(RGB(24, 37, 37));
            RegisterClass(&githubClass);
            
            hwnd_github = CreateWindow(
                "GitHubButton", "",
                WS_VISIBLE | WS_CHILD,
                30, 130, 320, 30,
                hwnd, (HMENU)ID_GITHUB_LINK,
                GetModuleHandle(NULL), NULL
            );
            
            break;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            HBRUSH bgBrush = CreateSolidBrush(RGB(24, 37, 37));
            FillRect(hdc, &rect, bgBrush);
            DeleteObject(bgBrush);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_ERASEBKGND: {
            return 1;
        }
        
        case WM_CLOSE: {
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
        
        case WM_SYSCOMMAND: {
            if (wParam == SC_MINIMIZE) {
                ShowWindow(hwnd, SW_HIDE);
                return 0;
            }
            return DefWindowProc(hwnd, msg, wParam, lParam);
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

    toggle_state = GetAutoStartRegistry();
    auto_start = toggle_state;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = UIWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "ModernUIClass";
    wc.hbrBackground = CreateSolidBrush(RGB(24, 37, 37));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    hwnd_ui = CreateWindow("ModernUIClass", "1Shot",
                           WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
                           CW_USEDEFAULT, CW_USEDEFAULT, ui_width, ui_height,
                           hwnd_parent, NULL, GetModuleHandle(NULL), NULL);
}

void ShowUI()
{
    if(hwnd_ui) {
        if (IsIconic(hwnd_ui)) {
            ShowWindow(hwnd_ui, SW_RESTORE);
        } else {
            ShowWindow(hwnd_ui, SW_SHOW);
        }
        SetWindowPos(hwnd_ui, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        SetForegroundWindow(hwnd_ui);
        SetActiveWindow(hwnd_ui);
        BringWindowToTop(hwnd_ui);
    }
}

void RenderUI(HWND hwnd)
{
    if(hwnd_ui && IsWindowVisible(hwnd_ui)) {
        UpdateWindow(hwnd_ui);
    }
}

int GetAutoStartToggle() { return auto_start; }
void SetAutoStartToggle(int value) { 
    auto_start = value;
    toggle_state = value;
    if (hwnd_toggle) {
        InvalidateRect(hwnd_toggle, NULL, TRUE);
    }
}