//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-11-27 16:14:56
//

#include <Windows.h>

#include "flock/window.h"

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    return DefWindowProc(Window, Message, WParam, LParam);
}

b8 FlockWindowOpen(fl_window *Window, i32 Width, i32 Height, const char *Title)
{
    HINSTANCE Instance = GetModuleHandle(NULL);

    WNDCLASSA WindowClass = {0};
    WindowClass.lpszClassName = "FlockWindowClass";
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = Instance;
    RegisterClassA(&WindowClass);

    Window->Width = Width;
    Window->Height = Height;
    Window->Title = Title;

    Window->Handle = (void*)CreateWindowA(WindowClass.lpszClassName, Title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, Instance, NULL);
    if (!Window->Handle)
        return false;
    ShowWindow(Window->Handle, SW_SHOW);

    return true;
}

b8 FlockWindowIsOpen(fl_window *Window)
{
    return IsWindowVisible((HWND)Window->Handle);
}

void FlockWindowUpdate(fl_window *Window)
{
    MSG Message;
    while (PeekMessageA(&Message, (HWND)Window->Handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
}

void FlockWindowClose(fl_window *Window)
{
    CloseWindow((HWND)Window->Handle);
}

void FlockWindowDestroy(fl_window *Window)
{
    DestroyWindow((HWND)Window->Handle);
}
