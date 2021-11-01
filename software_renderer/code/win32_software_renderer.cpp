#include <stdint.h>
#include <stdio.h>

/*only need for sqrt*/
#include <math.h>

#include <windowsx.h>
#include <windows.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t b8;
typedef int16_t b16;
typedef int32_t b32;
typedef int64_t b64;
typedef float f32;
typedef double f64;
typedef f32 float32;
typedef f64 float64;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#define global static

#include "maths.h"
#include "win32_software_renderer.h"
#include "input.h"
#include "junk.h"

global_variable bool32 running = 0;
global_variable uint64 global_frq_count;
global_variable input_t global_input = {0};
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

internal LRESULT CALLBACK
win32_process_message(HWND handle, UINT msg,
                      WPARAM wparam, LPARAM lparam)
{
    if(msg == WM_CLOSE)
    {
        running = 0;
        return 0;
    } else {
        return DefWindowProcA(handle, msg, wparam, lparam);
    }
}

internal void
message_loop()
{
    MSG message;
    
    local_persist u64 prev_inputs[16];
    local_persist u64 input_count = 0;
    
    for(u32 input = 0;
        input < input_count;
        input++)
    {
        u64 index = prev_inputs[input];
        if(global_input.key_states[index] & Input_State_Pressed)
            global_input.key_states[index] &= ~Input_State_Pressed;
        else if (global_input.key_states[index] & Input_State_Released)
            global_input.key_states[index] &= ~Input_State_Released;
        else if (global_input.mouse_states[index] & Input_State_Pressed)
            global_input.mouse_states[index] &= ~Input_State_Pressed;
        else if (global_input.mouse_states[index] & Input_State_Released)
            global_input.mouse_states[index] &= ~Input_State_Released;
    }
    input_count = 0;
    
    for(;;)
    {
        // TODO(ajeej): Assert that pkr_count doesn't go above 16
        BOOL got_message = FALSE;
        DWORD skip_messages[] = { 0x738, 0xFFFFFFFF, };
        DWORD last_message = 0;
        for(u32 skip_index = 0;
            skip_index< ArrayCount(skip_messages);
            skip_index++)
        {
            DWORD skip = skip_messages[skip_index];
            got_message = PeekMessage(&message, 0, last_message, skip-1, PM_REMOVE);
            if(got_message)
                break;
            
            last_message = skip+1;
        }
        
        if(!got_message)
        {
            break;
        }
        
        u64 wparam = message.wParam;
        u64 lparam = message.lParam;
        
        u8 modifiers = 0;
        if(GetKeyState(VK_CONTROL) & 0x0080)
        {
            modifiers |= Input_State_Ctrl;
        }
        if(GetKeyState(VK_MENU) & 0x0080)
        {
            modifiers |= Input_State_Alt;
        }
        if(GetKeyState(VK_SHIFT) & 0x0080)
        {
            modifiers |= Input_State_Shift;
        }
        
        switch(message.message)
        {
            // NOTE(Ajeej): Handle Key Input
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYUP :
            case WM_KEYDOWN :
            {
                u64 key_code = wparam;
                b8 is_down = ((lparam & (1<< 31)) == 0);
                b8 was_down = ((lparam & (1<< 30)) > 0);
                u64 key = 0;
                u8 *key_state = 0;
                
                if(key_code >= 'A' && key_code <= 'Z')
                    key = Key_ID_A + (key_code - 'A');
                else if (key_code >= '0' && key_code <= '9')
                    key = Key_ID_0 + (key_code - '0');
                else if (key_code == VK_ESCAPE)
                    key = Key_ID_Esc;
                else if (key_code >= VK_F1 && key_code <= VK_F12)
                    key = Key_ID_F1 + (key_code - VK_F1);
                else if (key_code == VK_OEM_3)
                    key = Key_ID_GraveAccent;
                else if (key_code == VK_OEM_MINUS)
                    key = Key_ID_Minus;
                else if (key_code == VK_OEM_PLUS)
                    key = Key_ID_Equal;
                else if (key_code == VK_BACK)
                    key = Key_ID_Backspace;
                else if (key_code == VK_TAB)
                    key = Key_ID_Tab;
                else if (key_code == VK_SPACE)
                    key = Key_ID_Space;
                else if (key_code == VK_RETURN)
                    key = Key_ID_Enter;
                else if (key_code == VK_UP)
                    key = Key_ID_Up;
                else if (key_code == VK_LEFT)
                    key = Key_ID_Left;
                else if (key_code == VK_DOWN)
                    key = Key_ID_Down;
                else if (key_code == VK_RIGHT)
                    key = Key_ID_Right;
                else if (key_code == VK_DELETE)
                    key = Key_ID_Delete;
                else if (key_code == VK_PRIOR)
                    key = Key_ID_PageUp;
                else if (key_code == VK_NEXT)
                    key = Key_ID_PageDown;
                else if (key_code == VK_HOME)
                    key = Key_ID_Home;
                else if (key_code == VK_END)
                    key = Key_ID_End;
                else if (key_code == VK_OEM_2)
                    key = Key_ID_ForwardSlash;
                else if (key_code == VK_OEM_102)
                    key = Key_ID_BackSlash;
                else if (key_code == VK_OEM_PERIOD)
                    key = Key_ID_Period;
                else if (key_code == VK_OEM_COMMA)
                    key = Key_ID_Comma;
                else if (key_code == VK_OEM_7)
                    key = Key_ID_Quote;
                else if (key_code == VK_OEM_4)
                    key = Key_ID_LeftBracket;
                else if (key_code == VK_OEM_6)
                    key = Key_ID_RightBracket;
                else if (key_code == VK_CONTROL)
                {
                    key = Key_ID_Ctrl;
                    modifiers &= ~Input_State_Ctrl;
                }
                else if (key_code == VK_SHIFT)
                {
                    key = Key_ID_Shift;
                    modifiers &= ~Input_State_Shift;
                }
                else if (key_code == VK_MENU)
                {
                    key = Key_ID_Alt;
                    modifiers &= ~Input_State_Alt;
                }
                
                if(update_input_state(&global_input.key_states[key],
                                      is_down, was_down, modifiers))
                    prev_inputs[input_count++] = key;
            } break;
            
            // NOTE(Ajeej): Handle Mouse Input
            case WM_LBUTTONDOWN:
            {
                u8 *mouse_state = &global_input.mouse_states[Mouse_ID_Left];
                b32 was_down = *mouse_state & Input_State_Down;
                if(update_input_state(mouse_state, 1, was_down, modifiers))
                    prev_inputs[input_count++] = Mouse_ID_Left;
            } break;
            
            case WM_LBUTTONUP:
            {
                u8 *mouse_state = &global_input.mouse_states[Mouse_ID_Left];
                if(update_input_state(mouse_state, 0, 0, modifiers))
                    prev_inputs[input_count++] = Mouse_ID_Left;
            } break;
            
            case WM_RBUTTONDOWN:
            {
                u8 *mouse_state = &global_input.mouse_states[Mouse_ID_Right];
                b32 was_down = *mouse_state & Input_State_Down;
                if(update_input_state(mouse_state, 1, was_down, modifiers))
                    prev_inputs[input_count++] = Mouse_ID_Right;
            } break;
            
            case WM_RBUTTONUP:
            {
                u8 *mouse_state = &global_input.mouse_states[Mouse_ID_Right];
                if(update_input_state(mouse_state, 0, 0, modifiers))
                    prev_inputs[input_count++] = Mouse_ID_Right;
            } break;
            
            case WM_MBUTTONDOWN:
            {
                u8 *mouse_state = &global_input.mouse_states[Mouse_ID_Middle];
                b32 was_down = *mouse_state & Input_State_Down;
                if(update_input_state(mouse_state, 1, was_down, modifiers))
                    prev_inputs[input_count++] = Mouse_ID_Middle;
            } break;
            
            case WM_MBUTTONUP:
            {
                u8 *mouse_state = &global_input.mouse_states[Mouse_ID_Middle];
                if(update_input_state(mouse_state, 0, 0, modifiers))
                    prev_inputs[input_count++] = Mouse_ID_Middle;
            } break;
            
            case WM_XBUTTONDOWN:
            {
                u64 btn = 0;
                if(wparam & (1<<6))
                    btn= Mouse_ID_Btn1;
                else if(wparam & (1<<6))
                    btn= Mouse_ID_Btn2;
                
                u8 *mouse_state = &global_input.mouse_states[btn];
                b32 was_down = *mouse_state & Input_State_Down;
                if(update_input_state(mouse_state, 1, was_down, modifiers))
                    prev_inputs[input_count++] = btn;
            } break;
            
            case WM_XBUTTONUP:
            {
                u64 btn = 0;
                if(wparam & (1<<6))
                    btn= Mouse_ID_Btn1;
                else if(wparam & (1<<6))
                    btn= Mouse_ID_Btn2;
                
                u8 *mouse_state = &global_input.mouse_states[btn];
                if(update_input_state(mouse_state, 0, 0, modifiers))
                    prev_inputs[input_count++] = btn;
            } break;
            
            
            // NOTE(Ajeej): Handle Mouse Move
            case WM_MOUSEMOVE:
            {
                int32 mouse_x = GET_X_LPARAM(lparam);
                int32 mouse_y = GET_Y_LPARAM(lparam);
                mouse_y = (-mouse_y + WINDOW_HEIGHT);
                
                global_input.mouse_pos.x = mouse_x;
                global_input.mouse_pos.y = mouse_y;
            };
            
            default :
            {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            } break;
        }
    }
}

inline LARGE_INTEGER
get_wall_clock()
{    
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return(Result);
}

inline float32
get_seconds_elapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    float32 Result = ((float32)(End.QuadPart - Start.QuadPart) /
                      (float32)global_frq_count);
    return(Result);
}

inline void
hit_target_seconds_per_frame(float32 target_dt, LARGE_INTEGER last_counter,
                             bool32 sleep_is_granular)
{
    LARGE_INTEGER work_counter = get_wall_clock();
    float32 work_seconds_elapsed = get_seconds_elapsed(last_counter, work_counter);
    
    float32 seconds_elapsed_for_frame = work_seconds_elapsed;
    if(seconds_elapsed_for_frame < target_dt)
    {                        
        if(sleep_is_granular)
        {
            DWORD sleep_ms = (DWORD)(1000.0f * (target_dt -
                                                seconds_elapsed_for_frame));
            if(sleep_ms > 0)
            {
                Sleep(sleep_ms);
            }
        }
        
        float32 test_seconds_elapsed_for_frame = get_seconds_elapsed(last_counter,
                                                                     get_wall_clock());
        if(test_seconds_elapsed_for_frame < target_dt)
        {
            
        }
        
        while(seconds_elapsed_for_frame < target_dt)
        {                            
            seconds_elapsed_for_frame= get_seconds_elapsed(last_counter,
                                                           get_wall_clock());
        }
    }
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance,
                   LPSTR cmd_line, int show_cmd)
{
    // TODO(Ajeej): See if this all can be divided into functions
    
    LARGE_INTEGER frq_counter_result;
    QueryPerformanceFrequency(&frq_counter_result);
    int64 global_frq_count = frq_counter_result.QuadPart;
    
    UINT desired_scheduler_ms = 1;
    bool32 sleep_is_granular = (timeBeginPeriod(desired_scheduler_ms) == TIMERR_NOERROR);
    
    /* Register Window Class */
    ATOM class_atom;
    WNDCLASSA window_class = {};
    
    //Define additional elements of the window class
    window_class.style = CS_VREDRAW | CS_HREDRAW;
    //Pointer to message loop function
    window_class.lpfnWndProc = &win32_process_message;
    //Number of extra bytes to allocated after the window class struct
    window_class.cbClsExtra = 0;
    //Number of extra bytes to allocated after the window instance
    window_class.cbWndExtra = 0;
    //Handle to instance that contains the window
    window_class.hInstance = instance;
    //Handle to the class icon
    window_class.hIcon = LoadIconA(instance, IDI_APPLICATION);
    //Handle to the class cursor
    window_class.hCursor = LoadCursorA(instance, IDC_ARROW);
    //Handle to the class background brush
    window_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    //Resource name of the class menu
    window_class.lpszMenuName = NULL;
    //Name or class atom that defines the class
    window_class.lpszClassName = "WindowClass";
    
    class_atom = RegisterClassA(&window_class);
    
    ASSERT(class_atom);
    
    
    /* Create Window Handle */
    // TODO(Ajeej): Play around with the styles
    DWORD style = WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT window_dim;
    HWND window_handle = {};
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    
    window_dim.left = 0;
    window_dim.top = 0;
    window_dim.right = width;
    window_dim.bottom = height;
    AdjustWindowRect(&window_dim, style, 0);
    width = window_dim.right - window_dim.left;
    height = window_dim.bottom - window_dim.top;
    
    window_handle = CreateWindowExA(0, window_class.lpszClassName,
                                    "Software Renderer", style,
                                    CW_USEDEFAULT, CW_USEDEFAULT,
                                    width, height,
                                    0, 0, instance, 0);
    
    // TODO(Ajeej): Change WS_VISIBLE to the ShowWindow function
    // so to initialize a window struct before the window is shown
    
    ASSERT(window_handle != 0);
    
    /* Create Framebuffer */
    int32 color_buffer_size = WINDOW_WIDTH * WINDOW_HEIGHT * 4;
    framebuffer_t framebuffer = {};
    framebuffer.width = WINDOW_WIDTH;
    framebuffer.height = WINDOW_HEIGHT;
    framebuffer.color_buffer_size = color_buffer_size;
    framebuffer.color_buffer = (uint8 *)malloc(color_buffer_size);
    
    // TODO(Ajeej): Create a surface image buffer as the main
    // buffer to present images through
    
    /* Create Bitmap for blitting */
    BITMAPINFOHEADER bitmap_header = {};
    HBITMAP dib_bitmap; //device-indepedent bitmap
    HBITMAP old_bitmap;
    HDC window_dc; //device-dependent device context
    HDC memory_dc; // device-independent device context
    
    //Get the device context handle for the specified window
    window_dc = GetDC(window_handle);
    //Create a memory device context that is compatible with
    //the window context
    memory_dc = CreateCompatibleDC(window_dc);
    ReleaseDC(window_handle, window_dc);
    
    bitmap_header.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_header.biWidth = WINDOW_WIDTH;
    // TODO(Ajeej): Change the height back to negative after
    // adding a surface, framebuffer in the process
    bitmap_header.biHeight = WINDOW_HEIGHT; //blit top-down
    bitmap_header.biPlanes = 1;
    bitmap_header.biBitCount = 32; /*RGBA -> (8+8+8+8)*/
    bitmap_header.biCompression = BI_RGB;
    dib_bitmap = CreateDIBSection(memory_dc, (BITMAPINFO *)&bitmap_header,
                                  DIB_RGB_COLORS, (void **)&framebuffer.color_buffer,
                                  0, 0);
    ASSERT(dib_bitmap != 0);
    
    /* Allow the memory device context to use the dib_bitmap */
    old_bitmap = (HBITMAP)SelectObject(memory_dc, dib_bitmap);
    DeleteObject(old_bitmap);;
    
    v2_t points[3] = { {0, 0}, {500, 500}, {500, 50} };
    v2_t *point0 = points;
    v2_t *point1 = points+1;
    v2_t *point2 = points+2;
    f32 weight = 1;
    fvector4_t color = {0, 0, 0, 0};
    u32 radi = 50;
    v2_t *selected_point = 0;
    
    b32 debug = 1;
    
    running = 1;
    while(running)
    {
        
        message_loop();
        
        /*Draw Point*/
        // TODO(Ajeej): Fix the order of the buffer in order to
        // have it as RR GG BB AA.
        fvector4_t blue = { 0.0f, 0.0f, 1.0f, 1.0f };
        fvector4_t red = { 1.0f, 0.0f, 0.0f, 1.0f };
        fvector4_t white = { 1.0f, 1.0f, 1.0f, 1.0f };
        
        if(IsKeyPressed(global_input, Key_ID_1))
        {
            debug = 0;
        }
        if(IsKeyPressed(global_input, Key_ID_2))
        {
            debug = 1;
        }
        if(IsKeyDown(global_input, Key_ID_W))
        {
            weight += 0.01;
        }
        if(IsKeyDown(global_input, Key_ID_S))
        {
            weight -= 0.01;
        }
        if(IsKeyDown(global_input, Key_ID_R))
        {
            weight = 1;
        }
        if(weight < 0)
        {
            weight = 0;
        }
        
        clear_color(&framebuffer, blue);
        
        for(u32 i = 0;
            i < ArrayCount(points);
            i++)
        {
            if(is_within_radius(global_input.mouse_pos,
                                points[i],
                                radi))
            {
                if(IsMouseDown(global_input, Mouse_ID_Left) && 
                   (selected_point == 0 || points[i] == *selected_point))
                {
                    points[i] = global_input.mouse_pos;
                    selected_point = &points[i];
                }
                else if((selected_point == 0 || points[i] == *selected_point))
                {
                    selected_point = 0;
                }
                
                draw_circle(&framebuffer, points[i].x, points[i].y,
                            radi, red);
            }
            else
            {
                draw_circle(&framebuffer, points[i].x, points[i].y,
                            radi, white);
            }
        }
        
        if(debug)
        {
            for(u32 i = 0;
                i < 2;
                i++)
            {
                draw_line(&framebuffer,
                          points[i].x, points[i].y,
                          points[i+1].x, points[i+1].y, white);
            }
        }
        
        draw_quad_rational_bezier(&framebuffer, point0->x, point0->y,
                                  point1->x, point1->y,
                                  point2->x, point2->y, weight, color);
        
        draw_square(&framebuffer, 100, 100, 400, 400, {0.0, 1.0, 1.0, 1.0});
        
        draw_rotated_ellipse(&framebuffer, 600, 250, 100, 300, 0.78f, red);
        
        
        
        /*Blit image to screen*/
        window_dc = GetDC(window_handle);
        BitBlt(window_dc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
               memory_dc, 0, 0, SRCCOPY);
        ReleaseDC(window_handle, window_dc);
    }
    
    UnregisterClassA(window_class.lpszClassName, instance);
    
    return 1;
}