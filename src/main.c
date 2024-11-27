//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-11-27 16:07:02
//

#include <flock/flock.h>
#include <stdio.h>

int main(void)
{
    fl_window Window;
    fl_renderer Renderer;

    if (!FlockWindowOpen(&Window, 1280, 720, "Flock Window")) {
        printf("Failed to open window\n");
        return -1;
    }
    if (!FlockRendererInit(&Renderer, &Window)) {
        printf("Failed to create renderer\n");
        return -1;
    }

    while (FlockWindowIsOpen(&Window)) {
        FlockWindowUpdate(&Window);
        FlockRendererPresent(&Renderer);
    }
    
    FlockRendererFree(&Renderer);
    FlockWindowDestroy(&Window);
    return 0;
}
