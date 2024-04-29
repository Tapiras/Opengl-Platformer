#include "game_engine_lib.h"
// --------------------------------------------------------------------------------------------------
//                                  Platform Globals
// --------------------------------------------------------------------------------------------------
static bool running = true;

// --------------------------------------------------------------------------------------------------
//                                  Platform Functions
// --------------------------------------------------------------------------------------------------
bool platform_create_window(int width, int height, const char* title);
void platform_update_window();

// --------------------------------------------------------------------------------------------------
//                                  Windows Platform
// --------------------------------------------------------------------------------------------------
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

// --------------------------------------------------------------------------------------------------
//                                  Windows Globals
// --------------------------------------------------------------------------------------------------
static HWND window;

// --------------------------------------------------------------------------------------------------
//                                  Windows Implemantation
// --------------------------------------------------------------------------------------------------
LRESULT CALLBACK windows_window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (uMsg)
    {
        case WM_CLOSE:
        {
            running = false;
            break;
        }
        default:
        {
            result = DefWindowProcA(hwnd, uMsg, wParam, lParam);
        }
    }

    return result;
}

bool platform_create_window(int width, int height, const char* title)
{
    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASSA wc = {};
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); //cursor type
    wc.lpszClassName = title;
    wc.lpfnWndProc = windows_window_callback;

    if (!RegisterClassA(&wc))
    {
        return false;
    }

    int dwStyle = WS_OVERLAPPEDWINDOW;

    window = CreateWindowExA(0, title, title,dwStyle, 100, 100, width, height, NULL, NULL, instance, NULL);

    if (window == NULL)
    {
        return false;
    }

    ShowWindow(window, SW_SHOW);
    return true;   
}

void platform_update_window()
{
    MSG msg;

    while (PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

#endif

int main()
{
    platform_create_window(1200, 720, "Platformer");

    while (running)
    {
        // update game
        platform_update_window();

        SM_TRACE("test");
        SM_WARN("test");
        SM_ERROR("test");
        SM_ASSERT(false, "Assertion not hit")
    }

    return 0;
}