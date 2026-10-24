typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;
typedef union Acmd { struct { u32 w0; u32 w1; } words; long long align; } Acmd;
typedef struct ALFx { unsigned char pad[0x14]; s16 *base; s16 *input; u32 length; } ALFx;
extern u32 func_800495D0(void *address);
#define LOAD_BUFFER(packet,count,destination,source) { Acmd *_a=(packet); _a->words.w0=0x04000000|(((count)&0xFFF)<<12)|((destination)&0xFFF); _a->words.w1=func_800495D0(source); }

Acmd *func_80041A00(ALFx *effect, s16 *current, s32 buffer, s32 count, Acmd *commands)
{
    Acmd *ptr = commands;
    s32 after_end, before_end;
    s16 *updated, *end = &effect->base[effect->length];
    if (current < effect->base) current += effect->length;
    updated = current + count;
    if (updated > end) {
        after_end = updated - end;
        before_end = end - current;
        LOAD_BUFFER(ptr++, before_end << 1, buffer, current);
        LOAD_BUFFER(ptr++, after_end << 1, buffer + (before_end << 1), effect->base);
    } else {
        LOAD_BUFFER(ptr++, count << 1, buffer, current);
    }
    return ptr;
}
