typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;
typedef struct Acmd { u32 word0; u32 word1; } Acmd;
typedef struct ALFilter ALFilter;
typedef Acmd *(*Handler)(void *, s16 *, s32, s32, Acmd *);
struct ALFilter { ALFilter *source; Handler handler; void *set_parameter; char padding_0c[8]; };
typedef struct ALResampler {
    ALFilter filter;
    void *state;
    f32 ratio;
    s32 unity_pitch;
    f32 delta;
    s32 first;
} ALResampler;
extern u32 func_800495D0(void *address);
#define MOVE(packet,input,output,count) { Acmd *_a=(Acmd *)(packet); _a->word0=0x0A000000|((input)&0xFFFFFF); _a->word1=(((output)&0xFFFF)<<16)|((count)&0xFFFF); }
#define SET(packet,input,output,count) { Acmd *_a=(Acmd *)(packet); _a->word0=0x08000000|((input)&0xFFFF); _a->word1=(((output)&0xFFFF)<<16)|((count)&0xFFFF); }
#define RESAMPLE(packet,flags,pitch,address) { Acmd *_a=(Acmd *)(packet); _a->word0=0x05000000|(((flags)&0xFF)<<16)|((pitch)&0xFFFF); _a->word1=(address); }

Acmd *func_8004BA10(void *filter, s16 *output, s32 output_count,
                    s32 sample_offset, Acmd *commands)
{
    ALResampler *resampler = filter;
    Acmd *ptr = commands;
    s16 input;
    s32 input_count;
    ALFilter *source = resampler->filter.source;
    s32 increment;
    f32 final_count;

    input = 320;
    if (!output_count) return ptr;
    if (resampler->unity_pitch) {
        ptr = source->handler(source, &input, output_count, sample_offset, commands);
        MOVE(ptr++, input, *output, output_count << 1);
    } else {
        if (resampler->ratio > 1.99996) resampler->ratio = 1.99996;
        resampler->ratio = (s32)(resampler->ratio * 0x8000);
        resampler->ratio = resampler->ratio / 0x8000;
        final_count = resampler->delta + resampler->ratio * (f32)output_count;
        input_count = (s32)final_count;
        resampler->delta = final_count - (f32)input_count;
        ptr = source->handler(source, &input, input_count, sample_offset, commands);
        increment = (s32)(resampler->ratio * 0x8000);
        SET(ptr++, input, *output, output_count << 1);
        RESAMPLE(ptr++, resampler->first, increment, func_800495D0(resampler->state));
        resampler->first = 0;
    }
    return ptr;
}
