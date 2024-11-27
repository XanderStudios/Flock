//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-11-27 17:28:00
//

#define MAX_AUDIO_INSTANCES 256

#define _USE_MATH_DEFINES
#define SOKOL_AUDIO_IMPL
#define DR_WAV_IMPLEMENTATION
#include <Windows.h>
#include <stdio.h>
#include <math.h>

#include "flock/audio.h"
#include "flock/ext/sokol_audio.h"
#include "flock/ext/dr_wav.h"

typedef struct fl_audio_instance {
    fl_audio_buffer *Buffer;
    u32 Cursor;
    b8 Loop;
    b8 Valid;
    f32 Volume;
    u32 Index;
} fl_audio_instance;

fl_audio_instance* INSTANCES;
u32 CURRENT_MAX = 0;

void StreamCallback(f32* buffer, i32 num_frames, i32 num_channels)
{
    assert(num_channels == 2);

    u32 sample_rate = saudio_sample_rate();
    for (i32 i = 0; i < num_frames; i++) {
        f32 Left = 0.0f;
        f32 Right = 0.0f;
        
        for (i32 Instance = 0; Instance < MAX_AUDIO_INSTANCES; Instance++) {
            fl_audio_instance* Inst = &INSTANCES[Instance];

            if (Inst->Valid) {
                if (Inst->Cursor > Inst->Buffer->BufferSize) {
                    if (Inst->Loop) {
                        Inst->Cursor = 0;
                    } else {
                        Inst->Valid = false;
                    }
                }

                Left += Inst->Buffer->Samples[Inst->Cursor] * Inst->Volume;
                Right += Inst->Buffer->Samples[Inst->Cursor + 1] * Inst->Volume;
                Inst->Cursor += 2;
            }
        }

        buffer[2 * i + 0] = (Left / (float)INT16_MAX);
        buffer[2 * i + 1] = (Right / (float)INT16_MAX);
    }
}

b8 FlockAudioContextInit()
{
    INSTANCES = malloc(sizeof(fl_audio_instance) * MAX_AUDIO_INSTANCES);
    for (i32 Instance = 0; Instance < MAX_AUDIO_INSTANCES; Instance++) {
        INSTANCES[Instance].Valid = false;
        INSTANCES[Instance].Buffer = NULL;   
    }

    saudio_desc Desc;
    memset(&Desc, 0, sizeof(Desc));
    Desc.num_channels = 2;
    Desc.sample_rate = 48'000;
    Desc.buffer_frames = 2048;
    Desc.stream_cb = StreamCallback;

    saudio_setup(&Desc);
    return saudio_isvalid();
}

void FlockAudioContextFree()
{
    saudio_shutdown();
    free(INSTANCES);
}

b8 FlockAudioLoad(fl_audio_buffer *Buffer, const char *Path)
{
    drwav File;
    if (!drwav_init_file(&File, Path, NULL)) {
        return false;
    }
    Buffer->Samples = malloc(File.totalPCMFrameCount * File.channels * sizeof(i16));
    Buffer->BufferSize = File.totalPCMFrameCount * 2;
    drwav_read_pcm_frames_s16(&File, File.totalPCMFrameCount, Buffer->Samples);
    drwav_uninit(&File);
    
    return true;
}

void FlockAudioPlay(fl_audio_buffer *Buffer, b8 Loop, f32 Volume)
{
    for (i32 Instance = 0; Instance < MAX_AUDIO_INSTANCES; Instance++) {
        if (INSTANCES[Instance].Valid == false) {
            INSTANCES[Instance].Valid = true;
            INSTANCES[Instance].Cursor = 0;
            INSTANCES[Instance].Buffer = Buffer;
            INSTANCES[Instance].Loop = Loop;
            INSTANCES[Instance].Index = Instance;
            INSTANCES[Instance].Volume = Volume;
            break;
        }
    }
}

void FlockAudioFree(fl_audio_buffer *Buffer)
{
    free(Buffer->Samples);
}
