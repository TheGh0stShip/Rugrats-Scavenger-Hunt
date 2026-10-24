typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;
typedef struct LowPass { s16 fc, fgain; s16 coefficients[16]; void *state; s32 first; } LowPass;
typedef struct Resampler Resampler;
typedef struct Delay {
    u32 input, output;
    s16 ffcoef, fbcoef, gain;
    f32 rsinc, rsval;
    s32 rsdelta;
    f32 rsgain;
    LowPass *lp;
    Resampler *rs;
} Delay;
typedef struct Fx { char filter[0x14]; s16 *base, *input; u32 length; Delay *delay; unsigned char section_count; void *parameter_handler; } Fx;
typedef struct Synth { char pad[0x44]; s32 output_rate; } Synth;
typedef struct Globals { Synth driver; } Globals;
extern Globals *audio_globals;
extern void func_800496F0(LowPass *filter);

s32 func_8004BC84(void *filter, s32 parameter, void *value)
{
    Fx *effect = filter;
    s32 item = (parameter - 2) % 8;
    s32 section = (parameter - 2) / 8;
    s32 setting = *(s32 *)value;

    switch (item) {
    case 0: effect->delay[section].input = (u32)setting & 0xfffffff8; break;
    case 1: effect->delay[section].output = (u32)setting & 0xfffffff8; break;
    case 3: effect->delay[section].ffcoef = (s16)setting; break;
    case 2: effect->delay[section].fbcoef = (s16)setting; break;
    case 4: effect->delay[section].gain = (s16)setting; break;
    case 5:
        effect->delay[section].rsinc = (((f32)setting / 1000) * 2.0) / audio_globals->driver.output_rate;
        break;
    case 6:
        effect->delay[section].rsgain = ((f32)setting / 173123.404906676) *
            (effect->delay[section].output - effect->delay[section].input);
        break;
    case 7:
        if (effect->delay[section].lp) {
            effect->delay[section].lp->fc = (s16)setting;
            func_800496F0(effect->delay[section].lp);
        }
        break;
    }
    return 0;
}
