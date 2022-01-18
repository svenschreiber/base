#include <windows.h>

static Platform_State global_platform_state;
static HDC            global_window_dc;

void platform_log(char *format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 1024, format, args);
    OutputDebugStringA(buffer);
    va_end(args);
}

static void win32_process_pending_messages() {
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

LRESULT CALLBACK win32_window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    LRESULT result = 0;

    switch (message) {
        
        case WM_SIZE: {
            platform_state->window_width  = LOWORD(lparam);
            platform_state->window_height = HIWORD(lparam);
            PostMessage(window, WM_PAINT, 0, 0);
        } break;


        case WM_QUIT:
        case WM_DESTROY:
        case WM_CLOSE: {
            platform_state->running = 0;
        } break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(window, &ps);
            EndPaint(window, &ps);
        } break;

        default: {
            result = DefWindowProc(window, message, wparam, lparam);
        } break;
            
    }

    return result;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR command_line, int show_code)
{
    
    platform_state = &global_platform_state;
    {
        platform_state->window_width  = PLATFORM_DEFAULT_WINDOW_WIDTH;
        platform_state->window_height = PLATFORM_DEFAULT_WINDOW_HEIGHT;
        platform_state->running       = 1;
    }


    WNDCLASSA window_class = {0};
    {
        window_class.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        window_class.lpfnWndProc   = win32_window_proc;
        window_class.hInstance     = instance;
        window_class.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
        window_class.hCursor       = LoadCursor(0, IDC_ARROW);
        window_class.lpszClassName = "AppWindowClass";
    }

    if (RegisterClass(&window_class) == 0) {
        return -1;
    }

    HWND window = CreateWindowA(window_class.lpszClassName,
                                "app",
                                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                platform_state->window_width, platform_state->window_height,
                                0, 0, instance, 0);

    if (window == 0) {
        return -1;
    }

    global_window_dc = GetDC(window);

    app_init();

    ShowWindow(window, show_code);
    UpdateWindow(window);

    while (platform_state->running) {
        win32_process_pending_messages();
        
        app_update();
    }

    return 0;
}
