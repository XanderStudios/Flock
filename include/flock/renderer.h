//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-11-27 16:24:21
//

#ifndef RENDERER_H_
#define RENDERER_H_

#include "window.h"

typedef struct fl_renderer {
    fl_window *Surface;
    void *Internal;
} fl_renderer;

b8 FlockRendererInit(fl_renderer *Renderer, fl_window *Window);
void FlockRendererPresent(fl_renderer *Renderer);
void FlockRendererFree(fl_renderer *Renderer);

#endif
