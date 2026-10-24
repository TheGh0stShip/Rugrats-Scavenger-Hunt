typedef signed int s32;
typedef unsigned int u32;
typedef union Acmd { struct { u32 w0; u32 w1; } words; long long align; } Acmd;
typedef struct N_ALAuxBus { unsigned char pad[0x14]; s32 sourceCount; s32 maxSources; void **sources; } N_ALAuxBus;
typedef struct N_ALSynth { unsigned char pad[0x34]; N_ALAuxBus *auxBus; } N_ALSynth;
extern N_ALSynth *D_80055AF4;
extern Acmd *func_8003F170(void *filter, s32 sampleOffset, Acmd *p);

#define CLEAR_BUFFER(packet, destination, count) { Acmd *_a = (packet); _a->words.w0 = 0x02000000 | (destination); _a->words.w1 = (count); }

Acmd *func_80040D30(s32 sampleOffset, Acmd *p)
{
    Acmd *ptr = p;
    N_ALAuxBus *m = D_80055AF4->auxBus;
    void **sources = m->sources;
    s32 i;

    CLEAR_BUFFER(ptr++, 0x7C0, 0x2E0);
    for (i = 0; i < m->sourceCount; i++)
        ptr = func_8003F170(sources[i], sampleOffset, ptr);
    return ptr;
}
