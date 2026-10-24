typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;
typedef long long s64;

typedef struct Acmd {
    u32 word0;
    u32 word1;
} Acmd;

typedef struct ALLowPass {
    s16 frequency;
    s16 gain;
    char padding_04[4];
    union {
        s16 coefficients[16];
        s64 force_alignment;
    } vector;
    void *state;
    s32 first;
} ALLowPass;

extern u32 func_800495D0(void *address);

#define SET_BUFFER(packet, input, output, count) { \
    Acmd *_a = (Acmd *)(packet); \
    _a->word0 = 0x08000000 | ((input) & 0xFFFF); \
    _a->word1 = (((output) & 0xFFFF) << 16) | ((count) & 0xFFFF); \
}
#define LOAD_ADPCM(packet, count, address) { \
    Acmd *_a = (Acmd *)(packet); \
    _a->word0 = 0x0B000000 | (count); \
    _a->word1 = (address); \
}
#define POLE_FILTER(packet, flags, gain, address) { \
    Acmd *_a = (Acmd *)(packet); \
    _a->word0 = 0x0E000000 | (((flags) & 0xFF) << 16) | ((gain) & 0xFFFF); \
    _a->word1 = (address); \
}

Acmd *func_8004C76C(ALLowPass *filter, s32 buffer, s32 count, Acmd *commands)
{
    Acmd *ptr = commands;

    SET_BUFFER(ptr++, buffer, buffer, count << 1);
    LOAD_ADPCM(ptr++, 32, func_800495D0(filter->vector.coefficients));
    POLE_FILTER(ptr++, filter->first, filter->gain,
                func_800495D0(filter->state));
    filter->first = 0;
    return ptr;
}
