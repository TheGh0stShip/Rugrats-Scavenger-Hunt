typedef signed short s16;
typedef signed int s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef union Acmd { struct { u32 w0; u32 w1; } words; long long align; } Acmd;
typedef struct ALLowPass { s16 fc; s16 fgain; u8 pad[4]; s16 coefficients[16]; void *state; s32 first; } ALLowPass;
extern u32 func_800495D0(void *address);

Acmd *func_80041C40(ALLowPass *filter, s32 buffer, Acmd *commands)
{
    Acmd *ptr = commands;
    s32 temporary = buffer >> 8;
    {
        Acmd *_a = ptr++;
        _a->words.w0 = 0x0B000020;
        _a->words.w1 = func_800495D0(filter->coefficients);
    }
    {
        Acmd *_a = ptr++;
        _a->words.w0 = 0x0E000000 | ((filter->first & 0xFF) << 16) | ((u16)filter->fgain & 0xFFFF);
        _a->words.w1 = ((temporary & 0xFF) << 24) | (func_800495D0(filter->state) & 0x00FFFFFF);
    }
    filter->first = 0;
    return ptr;
}
