typedef signed int s32;
typedef unsigned int u32;
typedef union Acmd {
    struct { u32 w0; u32 w1; } words;
    long long align;
} Acmd;
typedef struct N_ALSynth { unsigned char pad[0x48]; u32 sv_dramout; } N_ALSynth;

extern N_ALSynth *D_80055AF4;
extern Acmd *func_80041D30(s32 sampleOffset, Acmd *commands);

#define INTERLEAVE(packet) { Acmd *_a = (packet); _a->words.w0 = 0x0D000000; }
#define SAVE_BUFFER(packet, source) { Acmd *_a = (packet); _a->words.w0 = 0x062E0000; _a->words.w1 = (source); }

Acmd *func_80041CE0(s32 sampleOffset, Acmd *commands)
{
    Acmd *ptr = commands;

    ptr = func_80041D30(sampleOffset, ptr);
    INTERLEAVE(ptr++);
    SAVE_BUFFER(ptr++, D_80055AF4->sv_dramout);
    return ptr;
}
