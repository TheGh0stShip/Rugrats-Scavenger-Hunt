typedef signed short s16;
typedef signed int s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef float f32;

typedef struct Acmd {
    u32 word0;
    u32 word1;
} Acmd;

typedef struct ALFilter ALFilter;
typedef struct ALLowPass ALLowPass;
typedef struct ALResampler ALResampler;
typedef Acmd *(*ALCmdHandler)(void *, s16 *, s32, s32, Acmd *);

struct ALFilter {
    ALFilter *source;
    ALCmdHandler handler;
    void *set_param;
    s16 input;
    s16 output;
    s32 type;
};

typedef struct ALDelay {
    u32 input;
    u32 output;
    s16 feed_forward;
    s16 feed_back;
    s16 gain;
    char padding_0e[2];
    f32 rate_increment;
    f32 rate_value;
    s32 rate_delta;
    f32 rate_gain;
    ALLowPass *low_pass;
    ALResampler *resampler;
} ALDelay;

typedef struct ALFx {
    ALFilter filter;
    s16 *base;
    s16 *input;
    u32 length;
    ALDelay *delay;
    u8 section_count;
    char padding_25[3];
    void *parameter_handler;
} ALFx;

extern void func_8004B870(char *category, char *file, s32 line);
extern Acmd *func_8004C274(ALFx *, ALDelay *, s32, s32, Acmd *);
extern Acmd *func_8004C48C(ALFx *, s16 *, s32, s32, Acmd *);
extern Acmd *func_8004C5FC(ALFx *, s16 *, s32, s32, Acmd *);
extern Acmd *func_8004C76C(ALLowPass *, s32, s32, Acmd *);

#define SWAP(in, out) { s16 temporary = out; out = in; in = temporary; }
#define SET_BUFFER(packet, flags, input, output, count) { \
    Acmd *_a = (Acmd *)(packet); \
    _a->word0 = 0x08000000 | (((flags) & 0xFF) << 16) | ((input) & 0xFFFF); \
    _a->word1 = ((u32)((output) & 0xFFFF) << 16) | ((count) & 0xFFFF); \
}
#define MIX(packet, flags, gain, input, output) { \
    Acmd *_a = (Acmd *)(packet); \
    _a->word0 = 0x0C000000 | (((flags) & 0xFF) << 16) | ((gain) & 0xFFFF); \
    _a->word1 = (((input) & 0xFFFF) << 16) | ((output) & 0xFFFF); \
}
#define CLEAR_BUFFER(packet, destination, count) { \
    Acmd *_a = (Acmd *)(packet); \
    _a->word0 = 0x02000000 | (destination); \
    _a->word1 = (u32)(count); \
}
#define DMEM_MOVE(packet, input, output, count) { \
    Acmd *_a = (Acmd *)(packet); \
    _a->word0 = 0x0A000000 | ((u32)(input) & 0xFFFFFF); \
    _a->word1 = ((u32)((output) & 0xFFFF) << 16) | ((count) & 0xFFFF); \
}

Acmd *func_8004BCC0(void *filter, s16 *outp, s32 outCount,
                    s32 sampleOffset, Acmd *p)
{
    Acmd *ptr = p;
    ALFx *r = (ALFx *)filter;
    ALFilter *source = r->filter.source;
    s16 i, buff1, buff2, input, output;
    s16 *in_ptr, *out_ptr, gain, *prev_out_ptr = 0;
    ALDelay *d, *pd;

    if (source == 0) {
        func_8004B870("EX", "reverb.c", 75);
    }
    ptr = (*source->handler)(source, outp, outCount, sampleOffset, p);
    input = 0x6C0;
    output = 0x800;
    buff1 = 0;
    buff2 = 320;
    SET_BUFFER(ptr++, 0, 0, 0, outCount << 1);
    MIX(ptr++, 0, 0xDA83, 0x6C0, input);
    MIX(ptr++, 0, 0x5A82, 0x800, input);
    ptr = func_8004C5FC(r, r->input, input, outCount, ptr);
    CLEAR_BUFFER(ptr++, output, outCount << 1);
    for (i = 0; i < r->section_count; i++) {
        d = &r->delay[i];
        in_ptr = &r->input[-d->input];
        out_ptr = &r->input[-d->output];
        if (in_ptr == prev_out_ptr) {
            SWAP(buff1, buff2);
        } else {
            ptr = func_8004C48C(r, in_ptr, buff1, outCount, ptr);
        }
        ptr = func_8004C274(r, d, buff2, outCount, ptr);
        if (d->feed_forward) {
            MIX(ptr++, 0, (u16)d->feed_forward, buff1, buff2);
            if (!d->resampler && !d->low_pass) {
                ptr = func_8004C5FC(r, out_ptr, buff2, outCount, ptr);
            }
        }
        if (d->feed_back) {
            MIX(ptr++, 0, (u16)d->feed_back, buff2, buff1);
            ptr = func_8004C5FC(r, in_ptr, buff1, outCount, ptr);
        }
        if (d->low_pass) {
            ptr = func_8004C76C(d->low_pass, buff2, outCount, ptr);
        }
        if (!d->resampler) {
            ptr = func_8004C5FC(r, out_ptr, buff2, outCount, ptr);
        }
        if (d->gain) {
            MIX(ptr++, 0, (u16)d->gain, buff2, output);
        }
        prev_out_ptr = &r->input[d->output];
    }
    r->input += outCount;
    if (r->input > &r->base[r->length]) {
        r->input -= r->length;
    }
    DMEM_MOVE(ptr++, output, 0x6C0, outCount << 1);
    return ptr;
}
