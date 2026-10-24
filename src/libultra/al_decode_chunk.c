typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;
typedef struct Acmd { u32 word0; u32 word1; } Acmd;
typedef u32 (*DMAProc)(s32 address, s32 length, void *state);
typedef struct ALLoadFilter {
    char padding_00[0x14];
    void *state;
    void *loop_state;
    char padding_1c[0x14];
    DMAProc dma;
    void *dma_state;
    char padding_38[8];
    s32 first;
    s32 memory_input;
} ALLoadFilter;
#define SET_BUFFER(packet, input, output, count) { Acmd *_a=(Acmd *)(packet); _a->word0=0x08000000|((input)&0xFFFF); _a->word1=(((output)&0xFFFF)<<16)|((count)&0xFFFF); }
#define LOAD_BUFFER(packet, address) { Acmd *_a=(Acmd *)(packet); _a->word0=0x04000000; _a->word1=(address); }
#define SET_LOOP(packet, address) { Acmd *_a=(Acmd *)(packet); _a->word0=0x0F000000; _a->word1=((u32)(address)&0x1FFFFFFF); }
#define DECODE(packet, flags, address) { Acmd *_a=(Acmd *)(packet); _a->word0=0x01000000|(((flags)&0xFF)<<16); _a->word1=((u32)(address)&0x1FFFFFFF); }

Acmd *func_8004A8AC(Acmd *ptr, ALLoadFilter *filter, s32 samples,
                    s32 byte_count, s16 output, s16 input, u32 flags)
{
    s32 alignment;
    s32 address;

    if (byte_count > 0) {
        address = filter->dma(filter->memory_input, byte_count, filter->dma_state);
        alignment = address & 7;
        byte_count += alignment;
        SET_BUFFER(ptr++, input, 0, byte_count + 8 - (byte_count & 7));
        LOAD_BUFFER(ptr++, address - alignment);
    } else {
        alignment = 0;
    }
    if (flags & 2) {
        SET_LOOP(ptr++, filter->loop_state);
    }
    SET_BUFFER(ptr++, input + alignment, output, samples << 1);
    DECODE(ptr++, flags, filter->state);
    filter->first = 0;
    return ptr;
}
