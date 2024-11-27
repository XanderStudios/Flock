//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-11-27 16:11:35
//

#ifndef WINDOW_H_
#define WINDOW_H_

#include "types.h"

typedef struct fl_window {
    const char *Title;
    i32 Width;
    i32 Height;
    void *Handle;
} fl_window;

b8 FlockWindowOpen(fl_window *Window, i32 Width, i32 Height, const char *Title);
b8 FlockWindowIsOpen(fl_window *Window);
void FlockWindowUpdate(fl_window *Window);
void FlockWindowClose(fl_window *Window);
void FlockWindowDestroy(fl_window *Window);

#endif
