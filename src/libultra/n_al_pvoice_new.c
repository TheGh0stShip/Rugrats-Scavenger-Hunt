typedef signed short s16;
typedef signed int s32;
typedef unsigned short u16;
typedef float f32;

typedef struct ALHeap ALHeap;
typedef void *(*ALDMANew)(void *state);

typedef struct N_PVoice {
    unsigned char pad_00[0x0C];
    void *dc_state;
    void *dc_lstate;
    unsigned char pad_14[0x14];
    void *dc_dma;
    void *dc_dmaState;
    s32 dc_sample;
    s32 dc_lastsam;
    s32 dc_first;
    s32 dc_memin;
    void *rs_state;
    f32 rs_ratio;
    s32 rs_upitch;
    f32 rs_delta;
    s32 rs_first;
    void *em_state;
    s16 em_pan;
    s16 em_volume;
    s16 em_cvolL;
    s16 em_cvolR;
    s16 em_dryamt;
    s16 em_wetamt;
    u16 em_lratl;
    s16 em_lratm;
    s16 em_ltgt;
    u16 em_rratl;
    s16 em_rratm;
    s16 em_rtgt;
    s32 em_delta;
    s32 em_segEnd;
    s32 em_first;
    void *em_ctrlList;
    void *em_ctrlTail;
    s32 em_motion;
} N_PVoice;

extern void *func_8004CAC0(void *file, s32 line, ALHeap *heap, s32 count, s32 size);
#define alHeapAlloc(heap, count, size) func_8004CAC0(0, 0, (heap), (count), (size))

void func_800411BC(N_PVoice *mv, ALDMANew dmaNew, ALHeap *hp)
{
    mv->dc_state = alHeapAlloc(hp, 1, 0x20);
    mv->dc_lstate = alHeapAlloc(hp, 1, 0x20);
    mv->dc_dma = dmaNew(&mv->dc_dmaState);
    mv->dc_lastsam = 0;
    mv->dc_first = 1;
    mv->dc_memin = 0;

    mv->rs_state = alHeapAlloc(hp, 1, 0x20);
    mv->rs_delta = 0.0;
    mv->rs_first = 1;
    mv->rs_ratio = 1.0;
    mv->rs_upitch = 0;

    mv->em_state = alHeapAlloc(hp, 1, 0x50);
    mv->em_first = 1;
    mv->em_motion = 0;
    mv->em_volume = 1;
    mv->em_ltgt = 1;
    mv->em_rtgt = 1;
    mv->em_cvolL = 1;
    mv->em_cvolR = 1;
    mv->em_dryamt = 0;
    mv->em_wetamt = 0;
    mv->em_lratm = 1;
    mv->em_lratl = 0;
    mv->em_lratm = 1;
    mv->em_lratl = 0;
    mv->em_delta = 0;
    mv->em_segEnd = 0;
    mv->em_pan = 0;
    mv->em_ctrlList = 0;
    mv->em_ctrlTail = 0;
}
