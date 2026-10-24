typedef signed int s32;
typedef unsigned int u32;
typedef union Acmd { struct { u32 w0; u32 w1; } words; long long align; } Acmd;
typedef Acmd *(*N_ALCmdHandler)(s32, Acmd *);
typedef struct ALFilter { void *source; void *handler; } ALFilter;
typedef struct N_ALMainBus { ALFilter filter; } N_ALMainBus;
typedef struct N_ALSynth { unsigned char pad[0x30]; N_ALMainBus *mainBus; } N_ALSynth;
extern N_ALSynth *D_80055AF4;

#define CLEAR_BUFFER(packet, destination, count) { Acmd *_a = (packet); _a->words.w0 = 0x02000000 | (destination); _a->words.w1 = (count); }
#define MIX(packet, input, output) { Acmd *_a = (packet); _a->words.w0 = 0x0C007FFF; _a->words.w1 = ((input) << 16) | (output); }

Acmd *func_80041D30(s32 sampleOffset, Acmd *commands)
{
    Acmd *ptr = commands;

    CLEAR_BUFFER(ptr++, 0x4E0, 0x2E0);
    ptr = ((N_ALCmdHandler)D_80055AF4->mainBus->filter.handler)(sampleOffset, ptr);
    MIX(ptr++, 0x7C0, 0x4E0);
    MIX(ptr++, 0x930, 0x650);
    return ptr;
}
