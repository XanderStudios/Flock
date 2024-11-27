//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-11-27 17:26:24
//

#ifndef AUDIO_H_
#define AUDIO_H_

#include "types.h"

typedef struct fl_audio_buffer {
    u32 BufferSize;
    i16* Samples;
} fl_audio_buffer;

b8 FlockAudioContextInit();
void FlockAudioContextFree();

b8 FlockAudioLoad(fl_audio_buffer *Buffer, const char *Path);
void FlockAudioPlay(fl_audio_buffer *Buffer, b8 Loop, f32 Volume);
void FlockAudioFree(fl_audio_buffer *Buffer);

#endif
