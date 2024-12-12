#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void restartExplorer() {
    system("taskkill /f /im explorer.exe >nul 2>&1");
    system("start explorer.exe");
    MessageBox(NULL, "Successfully restarted explorer.exe.", "Success", MB_OK | MB_ICONINFORMATION);
}

int isElevated() {
    BOOL isAdmin = FALSE;
    HANDLE token = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation;
        DWORD size;
        if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
            isAdmin = elevation.TokenIsElevated;
        }
        CloseHandle(token);
    }
    return isAdmin;
}

void relaunchAsAdmin() {
    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    ShellExecute(NULL, "runas", path, NULL, NULL, SW_SHOWNORMAL);
    exit(0);
}

void centerWindow(HWND hwnd) {
    RECT rect;
    GetWindowRect(GetDesktopWindow(), &rect);
    int screenWidth = rect.right - rect.left;
    int screenHeight = rect.bottom - rect.top;
    
    RECT windowRect;
    GetWindowRect(hwnd, &windowRect);
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    
    SetWindowPos(hwnd, NULL,
        (screenWidth - windowWidth) / 2,
        (screenHeight - windowHeight) / 2,
        0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void createControls(HWND hwnd) {
    HFONT hFont = CreateFont(
        16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI"
    );

    HWND hTitle = CreateWindow(
        "STATIC",
        "RestartMyExplorer",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        20, 20, 360, 30,
        hwnd, NULL, NULL, NULL
    );
    SendMessage(hTitle, WM_SETFONT, (WPARAM)CreateFont(
        24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI"
    ), TRUE);

    HWND hDesc = CreateWindow(
        "STATIC",
        "An open-source tool to restart Windows Explorer, built with C.",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        20, 60, 360, 20,
        hwnd, NULL, NULL, NULL
    );
    SendMessage(hDesc, WM_SETFONT, (WPARAM)hFont, TRUE);

    HWND hAdminNote = CreateWindow(
        "STATIC",
        "github.com/MeLegend5555/RestartMyExplorer",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        20, 90, 360, 20,
        hwnd, NULL, NULL, NULL
    );
    SendMessage(hAdminNote, WM_SETFONT, (WPARAM)hFont, TRUE);

    HWND hRestartBtn = CreateWindow(
        "BUTTON",
        "Restart Explorer",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        50, 130, 300, 40,
        hwnd, (HMENU)1, NULL, NULL
    );
    SendMessage(hRestartBtn, WM_SETFONT, (WPARAM)hFont, TRUE);

    HWND hExitBtn = CreateWindow(
        "BUTTON",
        "Exit",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        50, 180, 300, 40,
        hwnd, (HMENU)2, NULL, NULL
    );
    SendMessage(hExitBtn, WM_SETFONT, (WPARAM)hFont, TRUE);

    HWND hGithub = CreateWindow(
        "STATIC",
        "RestartMyExplorer - @MeLegend5555",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        20, 240, 360, 20,
        hwnd, NULL, NULL, NULL
    );
    SendMessage(hGithub, WM_SETFONT, (WPARAM)hFont, TRUE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (!isElevated()) {
        if (MessageBox(NULL, 
            "This program requires administrator privileges. Relaunch as administrator?",
            "Administrator Required",
            MB_YESNO | MB_ICONWARNING) == IDYES) {
            relaunchAsAdmin();
        }
        return 0;
    }

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "RestartMyExplorerClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "RestartMyExplorerClass",
        "RestartMyExplorer",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 320,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    centerWindow(hwnd);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            createControls(hwnd);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) restartExplorer();
            if (LOWORD(wParam) == 2) PostQuitMessage(0);
            break;

        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(0, 0, 0));
            SetBkColor(hdcStatic, GetSysColor(COLOR_WINDOW));
            return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
