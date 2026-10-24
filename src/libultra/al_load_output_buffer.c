typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;
typedef struct Acmd { u32 word0; u32 word1; } Acmd;
typedef struct ALResampler { char padding_00[0x14]; void *state; f32 ratio; s32 unity; f32 delta; s32 first; } ALResampler;
typedef struct ALDelay {
    s32 input; s32 output; s16 feed_forward; s16 feed_back; s16 gain; char pad_0e[2];
    f32 rate_increment; f32 rate_value; s32 rate_delta; f32 rate_gain;
    void *low_pass; ALResampler *resampler;
} ALDelay;
typedef struct ALFx { char padding_00[0x14]; s16 *base; s16 *input; u32 length; } ALFx;
extern f32 func_8004C808(ALDelay *delay, s32 count);
extern Acmd *func_8004C48C(ALFx *effect, s16 *current, s32 buffer, s32 count, Acmd *commands);
extern u32 func_800495D0(void *address);
#define SET(packet,input,output,count) { Acmd *_a=(Acmd *)(packet); _a->word0=0x08000000|((input)&0xFFFF); _a->word1=(((output)&0xFFFF)<<16)|((count)&0xFFFF); }
#define RESAMPLE(packet,flags,pitch,address) { Acmd *_a=(Acmd *)(packet); _a->word0=0x05000000|(((flags)&0xFF)<<16)|((pitch)&0xFFFF); _a->word1=(address); }

Acmd *func_8004C274(ALFx *effect, ALDelay *delay, s32 buffer,
                    s32 input_count, Acmd *commands)
{
    Acmd *ptr = commands;
    s32 ratio;
    s32 count;
    s32 resample_buffer = 640;
    s16 *output;
    f32 final_count;
    f32 fractional_ratio;
    f32 delta;
    s32 ram_alignment = 0;
    s32 length;

    if (delay->resampler) {
        length = delay->output - delay->input;
        delta = func_8004C808(delay, input_count);
        delta /= length;
        delta = (s32)(delta * 0x8000);
        delta = delta / 0x8000;
        fractional_ratio = 1.0 - delta;
        final_count = delay->resampler->delta + fractional_ratio * (f32)input_count;
        count = (s32)final_count;
        delay->resampler->delta = final_count - (f32)count;
        output = &effect->input[-(delay->output - delay->rate_delta)];
        ram_alignment = ((s32)output & 7) >> 1;
        ptr = func_8004C48C(effect, output - ram_alignment, resample_buffer,
                            count + ram_alignment, ptr);
        ratio = (s32)(fractional_ratio * 0x8000);
        SET(ptr++, resample_buffer + (ram_alignment << 1), buffer, input_count << 1);
        RESAMPLE(ptr++, delay->resampler->first, ratio,
                 func_800495D0(delay->resampler->state));
        delay->resampler->first = 0;
        delay->rate_delta += count - input_count;
    } else {
        output = &effect->input[-delay->output];
        ptr = func_8004C48C(effect, output, buffer, input_count, ptr);
    }
    return ptr;
}
