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
    fl_audio_buffer WhiteNoiz;
    fl_audio_buffer Skibidi;

    if (!FlockWindowOpen(&Window, 1280, 720, "Flock Window")) {
        printf("Failed to open window\n");
        return -1;
    }
    if (!FlockRendererInit(&Renderer, &Window)) {
        printf("Failed to create renderer\n");
        return -1;
    }
    if (!FlockAudioContextInit()) {
        printf("Failed to create audio context\n");
        return -1;
    }

    if (!FlockAudioLoad(&WhiteNoiz, "assets/white_noiz.wav")) {
        printf("skibidi toilet\n");
        return -1;
    }
    if (!FlockAudioLoad(&Skibidi, "assets/skibidi.wav")) {
        printf("skibidi toilet\n");
        return -1;
    }
    //FlockAudioPlay(&WhiteNoiz, false, 0.2f);
    FlockAudioPlay(&Skibidi, false, 0.5f);

    while (FlockWindowIsOpen(&Window)) {
        FlockWindowUpdate(&Window);
        FlockRendererPresent(&Renderer);
    }
    
    FlockAudioFree(&WhiteNoiz);
    FlockAudioFree(&Skibidi);
    FlockAudioContextFree();
    FlockRendererFree(&Renderer);
    FlockWindowDestroy(&Window);
    return 0;
}
