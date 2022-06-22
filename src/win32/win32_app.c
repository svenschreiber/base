#include <windows.h>
#include "win32_opengl.c"

static Platform_State global_platform_state;
static HDC            global_window_dc;
static HGLRC          global_opengl_rc;
static b32            tracking_mouse = 0;

void platform_swap_buffers() {
    wglSwapLayerBuffers(global_window_dc, WGL_SWAP_MAIN_PLANE);
}

void * platform_reserve_memory(u64 size) {
    void *mem = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
    return mem;
}

void platform_commit_memory(void *mem, u64 size) {
    VirtualAlloc(mem, size, MEM_COMMIT, PAGE_READWRITE);
}

void platform_release_memory(void *mem) {
    VirtualFree(mem, 0, MEM_RELEASE);
}

void platform_decommit_memory(void *mem, u64 size) {
    VirtualFree(mem, size, MEM_DECOMMIT);
}


b32 platform_read_entire_file(char *file_name, Platform_File *result) {
    HANDLE file_handle = CreateFileA(file_name, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (file_handle == INVALID_HANDLE_VALUE) {
        return 0;
    }

    LARGE_INTEGER file_size;
    GetFileSizeEx(file_handle, &file_size);
    result->size = file_size.QuadPart;
    result->data = (u8 *)VirtualAlloc(0, result->size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    DWORD bytes_read = 0;
    ReadFile(file_handle, result->data, (u32)result->size, &bytes_read, 0);
    CloseHandle(file_handle);

    if (result->size == bytes_read) {
        return 1;
    } else {
        platform_release_memory(result->data);
        return 0;
    }
}

void platform_log(char *format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 1024, format, args);
    OutputDebugStringA(buffer);
    va_end(args);
}

static ivec2 win32_get_mouse_pos(HWND window)
{
    ivec2 result = {0};
    POINT mouse_pos;
    GetCursorPos(&mouse_pos);
    ScreenToClient(window, &mouse_pos);
    result.x = mouse_pos.x;
    result.y = mouse_pos.y;
    return result;
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

    Key_Modifiers key_modifiers = 0;
    if (GetKeyState(VK_CONTROL) & 0x8000) {
        key_modifiers |= KEY_MODIFIER_CTRL;
    } else if (GetKeyState(VK_SHIFT) & 0x8000) {
        key_modifiers |= KEY_MODIFIER_SHIFT;
    } else if (GetKeyState(VK_MENU) & 0x8000) {
        key_modifiers |= KEY_MODIFIER_ALT;
    }

    switch (message) {
        
        case WM_SIZE: {
            platform_state->window_width  = LOWORD(lparam);
            platform_state->window_height = HIWORD(lparam);
            glViewport(0, 0, platform_state->window_width, platform_state->window_height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0.0f, (f32)platform_state->window_width, (f32)platform_state->window_height, 0.0f, 0.0f, 1.0f);
            if (app_data) {
                app_update();
            }
            PostMessage(window, WM_PAINT, 0, 0);
        } break;

        case WM_QUIT:
        case WM_DESTROY:
        case WM_CLOSE: {
            platform_state->running = 0;
        } break;

                case WM_LBUTTONDOWN: {
            Platform_Event event = {0};
            {
                event.type          = Platform_Event_Type_Mouse_Press;
                event.key           = KEY_MOUSE_BUTTON_LEFT;
                event.key_modifiers = key_modifiers;
            }
            platform_push_event(event);
        } break;

        case WM_LBUTTONUP: {
            Platform_Event event = {0};
            {
                event.type          = Platform_Event_Type_Mouse_Release;
                event.key           = KEY_MOUSE_BUTTON_LEFT;
                event.key_modifiers = key_modifiers;
            }
            platform_push_event(event);
        } break;

        case WM_RBUTTONDOWN: {
            Platform_Event event = {0};
            {
                event.type          = Platform_Event_Type_Mouse_Press;
                event.key           = KEY_MOUSE_BUTTON_RIGHT;
                event.key_modifiers = key_modifiers;
            }
            platform_push_event(event);
        } break;

        case WM_RBUTTONUP: {
            Platform_Event event = {0};
            {
                event.type          = Platform_Event_Type_Mouse_Release;
                event.key           = KEY_MOUSE_BUTTON_RIGHT;
                event.key_modifiers = key_modifiers;
            }
            platform_push_event(event);
        } break;

        case WM_MOUSEMOVE: {
            Platform_Event event = {0};
            {
                event.type          = Platform_Event_Type_Mouse_Move;
                event.mouse_pos     = win32_get_mouse_pos(window);
            }
            platform_push_event(event);

            if (!tracking_mouse) {
                tracking_mouse = 1;
                TRACKMOUSEEVENT track_mouse_event = {0};
                {
                    track_mouse_event.cbSize = sizeof(track_mouse_event);
                    track_mouse_event.dwFlags = TME_LEAVE;
                    track_mouse_event.hwndTrack = window;
                    track_mouse_event.dwHoverTime = HOVER_DEFAULT;
                }
                TrackMouseEvent(&track_mouse_event);
                Platform_Event cursor_enter_event = {0};
                {
                    cursor_enter_event.type = Platform_Event_Type_Cursor_Enter;
                }
                platform_push_event(cursor_enter_event);
            }
        } break;

        case WM_MOUSELEAVE: {
            tracking_mouse = 0;
            Platform_Event event = {0};
            {
                event.type = Platform_Event_Type_Cursor_Leave;
            }
            platform_push_event(event);
        } break;

        case WM_MOUSEWHEEL: {
            s32 scroll_delta = (s32)GET_WHEEL_DELTA_WPARAM(wparam);
            Platform_Event event = {0};
            {
                event.type          = Platform_Event_Type_Mouse_Scroll;
                event.scroll_delta  = scroll_delta;
            }
            platform_push_event(event);
            platform_log("%i\n", scroll_delta);
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            s32 vk_code = (s32)wparam;
            b32 is_down = (lparam & (1 << 31)) == 0;

            s32 key_input = 0;
            if (vk_code >= 'A' && vk_code <= 'Z') {
                key_input = KEY_A + (vk_code - 'A');
            } else if (vk_code >= '0' && vk_code <= '9') {
                key_input = KEY_0 + (vk_code - '0');
            } else {
                if (vk_code == VK_BACK) {
                    key_input = KEY_BACKSPACE;
                } else if (vk_code == VK_TAB) {
                    key_input = KEY_TAB;
                } else if (vk_code == VK_RETURN) {
                    key_input = KEY_ENTER;
                } else if (vk_code == VK_SHIFT) {
                    key_input = KEY_SHIFT;
                } else if (vk_code == VK_CONTROL) {
                    key_input = KEY_CTRL;
                    key_modifiers &= ~KEY_MODIFIER_CTRL;
                } else if (vk_code == VK_MENU) {
                    key_input = KEY_ALT;
                    key_modifiers &= ~KEY_MODIFIER_ALT;
                } else if (vk_code == VK_PAUSE) {
                    key_input = KEY_PAUSE;
                } else if (vk_code == VK_CAPITAL) {
                    key_input = KEY_CAPS_LOCK;
                } else if (vk_code == VK_ESCAPE) {
                    key_input = KEY_ESCAPE;
                } else if (vk_code == VK_SPACE) {
                    key_input = KEY_SPACE;
                } else if (vk_code == VK_PRIOR) {
                    key_input = KEY_PAGE_UP;
                } else if (vk_code == VK_NEXT) {
                    key_input = KEY_PAGE_DOWN;
                } else if (vk_code == VK_END) {
                    key_input = KEY_END;
                } else if (vk_code == VK_HOME) {
                    key_input = KEY_HOME;
                } else if (vk_code == VK_LEFT) {
                    key_input = KEY_LEFT;
                } else if (vk_code == VK_RIGHT) {
                    key_input = KEY_RIGHT;
                } else if (vk_code == VK_UP) {
                    key_input = KEY_UP;
                } else if (vk_code == VK_DOWN) {
                    key_input = KEY_DOWN;
                } else if (vk_code == VK_SNAPSHOT) {
                    key_input = KEY_PRINT_SCREEN;
                } else if (vk_code == VK_INSERT) {
                    key_input = KEY_INSERT;
                } else if (vk_code == VK_DELETE) {
                    key_input = KEY_DELETE;
                }  else if (vk_code >= VK_F1 && vk_code <= VK_F12) {
                    key_input = KEY_F1 + (vk_code - VK_F1);
                } else if (vk_code == VK_SCROLL) {
                    key_input = KEY_SCROLL_LOCK;
                } else if (vk_code == VK_OEM_1) {
                    key_input = KEY_SEMICOLON;
                } else if (vk_code == VK_OEM_PLUS) {
                    key_input = KEY_PLUS;
                } else if (vk_code == VK_OEM_MINUS) {
                    key_input = KEY_MINUS;
                } else if (vk_code == VK_OEM_PERIOD) {
                    key_input = KEY_PERIOD;
                } else if (vk_code == VK_OEM_2) {
                    key_input = KEY_SLASH;
                } else if (vk_code == VK_OEM_3) {
                    key_input = KEY_GRAVE_ACCENT;
                } else if (vk_code == VK_OEM_4) {
                    key_input = KEY_LEFT_BRACKET;
                } else if (vk_code == VK_OEM_5) {
                    key_input = KEY_BACKSLASH;
                } else if (vk_code == VK_OEM_6) {
                    key_input = KEY_RIGHT_BRACKET;
                } else if (vk_code == VK_OEM_7) {
                    key_input = KEY_QUOTE;
                }
            }

            if (is_down) {
                Platform_Event event = {0};
                {
                    event.type          = Platform_Event_Type_Key_Press;
                    event.key           = key_input;
                    event.key_modifiers = key_modifiers;
                }
                platform_push_event(event);
            } else {
                Platform_Event event = {0};
                {
                    event.type          = Platform_Event_Type_Key_Release;
                    event.key           = key_input;
                    event.key_modifiers = key_modifiers;
                }
                platform_push_event(event);
            }
            
            result = DefWindowProc(window, message, wparam, lparam);            
        } break;

        case WM_CHAR: {
            s32 character = (s32)wparam;
            if (character >= 32) {
                Platform_Event event = {0};
                {
                    event.type          = Platform_Event_Type_Character_Input;
                    event.character     = character;
                    event.key_modifiers = key_modifiers;
                }
                platform_push_event(event);
            }
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
    LARGE_INTEGER perf_count_frequency_result;
    QueryPerformanceFrequency(&perf_count_frequency_result);
    s64 perf_count_frequency = perf_count_frequency_result.QuadPart;
    
    platform_state = &global_platform_state;
    {
        platform_state->window_width  = PLATFORM_DEFAULT_WINDOW_WIDTH;
        platform_state->window_height = PLATFORM_DEFAULT_WINDOW_HEIGHT;
        platform_state->running       = 1;
        platform_state->event_count   = 0;
        platform_state->delta         = 0;
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
    win32_create_opengl_context(&global_opengl_rc, global_window_dc);

    app_init();

    ShowWindow(window, show_code);
    UpdateWindow(window);

    LARGE_INTEGER last_counter;
    QueryPerformanceCounter(&last_counter);
    LARGE_INTEGER end_counter;
    

    while (platform_state->running) {
        win32_process_pending_messages();
        
        app_update();

        QueryPerformanceCounter(&end_counter);
        s64 counter_elapsed = end_counter.QuadPart - last_counter.QuadPart;
        platform_state->delta = (f32)counter_elapsed / (f32)perf_count_frequency;
        last_counter = end_counter;
    }

    wglMakeCurrent(global_window_dc, 0);
    wglDeleteContext(global_opengl_rc);

    return 0;
}
