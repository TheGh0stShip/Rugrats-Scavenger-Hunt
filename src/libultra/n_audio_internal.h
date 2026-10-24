#ifndef N_AUDIO_INTERNAL_H
#define N_AUDIO_INTERNAL_H

typedef signed short s16;
typedef signed int s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef float f32;

typedef struct ALLink {
    struct ALLink *next;
    struct ALLink *prev;
} ALLink;

typedef struct {
    void *base;
    void *current;
    s32 length;
    s32 count;
} ALHeap;

typedef struct ALParam {
    struct ALParam *next;
    s32 delta;
    s16 type;
    union { f32 f; s32 i; } data;
    union { f32 f; s32 i; } moredata;
    union { f32 f; s32 i; } stillmoredata;
    union { f32 f; s32 i; } yetstillmoredata;
} ALParam;

typedef struct ALFilter {
    struct ALFilter *source;
    void *handler;
    void *set_parameter;
    s16 input;
    s16 output;
    s32 type;
} ALFilter;

typedef union {
    struct {
        u32 w0;
        u32 w1;
    } words;
    long long align;
} Acmd;

typedef struct {
    u32 start;
    u32 end;
    u32 count;
} ALRawLoop;

typedef struct {
    u32 start;
    u32 end;
    u32 count;
    s16 state[16];
} ALADPCMloop;

typedef struct {
    s32 order;
    s32 npredictors;
    s16 book[1];
} ALADPCMBook;

typedef struct ALWaveTable {
    u8 *base;
    s32 len;
    u8 type;
    u8 flags;
    union {
        struct {
            ALADPCMloop *loop;
            ALADPCMBook *book;
        } adpcmWave;
        struct {
            ALRawLoop *loop;
        } rawWave;
    } waveInfo;
} ALWaveTable;

typedef struct N_PVoice {
    ALLink node;
    void *vvoice;
    void *dc_state;
    void *dc_lstate;
    ALRawLoop dc_loop;
    ALWaveTable *dc_table;
    s32 dc_bookSize;
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
    s32 offset;
} N_PVoice;

typedef struct {
    ALFilter filter;
} N_ALMainBus;

typedef struct {
    ALFilter filter;
    s32 sourceCount;
    s32 maxSources;
    N_PVoice **sources;
    void *fx;
    void *fx_array[8];
} N_ALAuxBus;

typedef struct {
    void *head;
    ALLink pFreeList;
    ALLink pAllocList;
    ALLink pLameList;
    s32 paramSamples;
    s32 curSamples;
    void *dma;
    ALHeap *heap;
    ALParam *paramList;
    N_ALMainBus *mainBus;
    N_ALAuxBus *auxBus;
    s32 numPVoices;
    s32 maxAuxBusses;
    s32 outputRate;
    s32 maxOutSamples;
    s32 sv_dramout;
    s32 sv_first;
} N_ALSynth;

typedef struct {
    s32 maxVVoices;
    s32 maxPVoices;
    s32 maxUpdates;
    s32 maxFXbusses;
    void *dmaproc;
    ALHeap *heap;
    s32 outputRate;
    u8 fxType;
    s32 *params;
} ALSynConfig;

#define _SHIFTL(v, s, w) ((u32)(((u32)(v) & ((0x01 << (w)) - 1)) << (s)))
#define K0_TO_PHYS(x) ((u32)(x) & 0x1FFFFFFF)
#define FIXED_SAMPLE 184

#define A_ADPCM 1
#define A_CLEARBUFF 2
#define A_LOADBUFF 4
#define A_RESAMPLE 5
#define A_DMEMMOVE 10
#define A_LOADADPCM 11
#define A_SETLOOP 15

#define aClearBuffer(pkt, d, c)                                         \
    {                                                                   \
        Acmd *_a = (Acmd *)(pkt);                                       \
        _a->words.w0 = _SHIFTL(A_CLEARBUFF, 24, 8) | _SHIFTL(d, 0, 24); \
        _a->words.w1 = (u32)(c);                                        \
    }

#endif
