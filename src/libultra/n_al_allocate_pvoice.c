typedef signed short s16;
typedef signed int s32;

typedef struct ALLink { struct ALLink *next; struct ALLink *prev; } ALLink;
typedef struct { unsigned char pad[0x16]; s16 priority; } N_ALVoice;
typedef struct N_PVoice { ALLink node; N_ALVoice *vvoice; unsigned char pad[0x88 - 0xC]; s32 offset; } N_PVoice;
typedef struct N_ALSynth { s32 pad0; ALLink pFreeList; ALLink pAllocList; ALLink pLameList; } N_ALSynth;

extern N_ALSynth *D_80055AF4;
extern void func_8004CA4C(ALLink *ln);
extern void func_8004CA2C(ALLink *ln, ALLink *to);

s32 func_8003F0A8(N_PVoice **pvoice, s16 priority)
{
    ALLink *dl;
    N_PVoice *pv;
    s32 stolen = 0;

    if ((dl = D_80055AF4->pLameList.next) != 0 || (dl = D_80055AF4->pFreeList.next) != 0) {
        *pvoice = (N_PVoice *)dl;
        func_8004CA4C(dl);
        func_8004CA2C(dl, &D_80055AF4->pAllocList);
    } else {
        for (dl = D_80055AF4->pAllocList.next; dl != 0; dl = dl->next) {
            pv = (N_PVoice *)dl;
            if (pv->vvoice->priority <= priority && pv->offset == 0) {
                *pvoice = pv;
                priority = pv->vvoice->priority;
                stolen = 1;
            }
        }
    }
    return stolen;
}
