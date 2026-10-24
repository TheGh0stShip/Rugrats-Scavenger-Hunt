#include "n_audio_internal.h"

#define N_AL_DECODER_OUT 0x170
#define UNITY_PITCH 0x8000
#define MAX_RATIO 1.99996

extern Acmd *n_alAdpcmPull(N_PVoice *voice, s16 *output, s32 count, Acmd *commands);
extern u32 osVirtualToPhysical(void *address);

Acmd *n_alResamplePull(N_PVoice *voice, s16 *output, Acmd *commands)
{
    Acmd *ptr = commands;
    s16 input;
    s32 input_count;
    s32 increment;
    f32 final_count;

    input = N_AL_DECODER_OUT;

    if (voice->rs_upitch) {
        ptr = n_alAdpcmPull(voice, &input, FIXED_SAMPLE, commands);
        {
            Acmd *command = ptr++;
            command->words.w0 = _SHIFTL(A_DMEMMOVE, 24, 8) | _SHIFTL(input, 0, 24);
            command->words.w1 = _SHIFTL(*output, 16, 16) |
                                _SHIFTL(FIXED_SAMPLE << 1, 0, 16);
        }
    } else {
        if (voice->rs_ratio > MAX_RATIO)
            voice->rs_ratio = MAX_RATIO;
        voice->rs_ratio = (s32)(voice->rs_ratio * UNITY_PITCH) / (f32)UNITY_PITCH;
        final_count = voice->rs_delta + (voice->rs_ratio * (f32)FIXED_SAMPLE);
        input_count = (s32)final_count;
        voice->rs_delta = final_count - (f32)input_count;
        ptr = n_alAdpcmPull(voice, &input, input_count, commands);
        increment = (s32)(voice->rs_ratio * UNITY_PITCH);
        {
            Acmd *command = ptr++;
            command->words.w0 = _SHIFTL(A_RESAMPLE, 24, 8) |
                                _SHIFTL(osVirtualToPhysical(voice->rs_state), 0, 24);
            command->words.w1 = _SHIFTL(voice->rs_first, 30, 2) |
                                _SHIFTL(increment, 14, 16) |
                                _SHIFTL(input, 2, 12);
        }
        voice->rs_first = 0;
    }
    return ptr;
}
