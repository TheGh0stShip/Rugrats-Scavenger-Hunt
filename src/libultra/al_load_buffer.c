typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;
typedef struct Acmd { u32 word0; u32 word1; } Acmd;
typedef struct ALFx {
    char padding_00[0x14];
    s16 *base;
    s16 *input;
    u32 length;
} ALFx;
extern u32 func_800495D0(void *address);
#define SET_BUFFER(packet, input, output, count) { Acmd *_a=(Acmd *)(packet); _a->word0=0x08000000|((input)&0xFFFF); _a->word1=(((output)&0xFFFF)<<16)|((count)&0xFFFF); }
#define LOAD_BUFFER(packet, address) { Acmd *_a=(Acmd *)(packet); _a->word0=0x04000000; _a->word1=(address); }

Acmd *func_8004C48C(ALFx *effect, s16 *current, s32 buffer, s32 count,
                    Acmd *commands)
{
    Acmd *ptr = commands;
    s32 after_end;
    s32 before_end;
    s16 *updated;
    s16 *delay_end;

    delay_end = &effect->base[effect->length];
    if (current < effect->base) {
        current += effect->length;
    }
    updated = current + count;
    if (updated > delay_end) {
        after_end = updated - delay_end;
        before_end = delay_end - current;
        SET_BUFFER(ptr++, buffer, 0, before_end << 1);
        LOAD_BUFFER(ptr++, func_800495D0(current));
        SET_BUFFER(ptr++, buffer + (before_end << 1), 0, after_end << 1);
        LOAD_BUFFER(ptr++, func_800495D0(effect->base));
    } else {
        SET_BUFFER(ptr++, buffer, 0, count << 1);
        LOAD_BUFFER(ptr++, func_800495D0(current));
    }
    SET_BUFFER(ptr++, 0, 0, count << 1);
    return ptr;
}
