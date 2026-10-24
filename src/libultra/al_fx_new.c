typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef float f32;
typedef double f64;

typedef struct Acmd { u32 word0; u32 word1; } Acmd;
typedef struct ALFilter ALFilter;
typedef struct ALHeap { u8 *base; u8 *current; s32 length; s32 count; } ALHeap;
typedef s16 RESAMPLE_STATE[16];
typedef s16 POLEF_STATE[4];
typedef Acmd *(*ALCmdHandler)(void *, s16 *, s32, s32, Acmd *);
typedef s32 (*ALSetParam)(void *, s32, void *);
typedef s32 (*ALSetFXParam)(void *, s32, void *);

struct ALFilter {
    ALFilter *source;
    ALCmdHandler handler;
    ALSetParam setParam;
    s16 input;
    s16 output;
    s32 type;
};

typedef struct ALResampler {
    ALFilter filter;
    RESAMPLE_STATE *state;
    f32 ratio;
    s32 unity;
    f32 delta;
    s32 first;
    void *control_list;
    void *control_tail;
    s32 motion;
} ALResampler;

typedef struct ALLowPass {
    s16 fc;
    s16 fgain;
    union { s16 fccoef[16]; s64 force_alignment; } fcvec;
    POLEF_STATE *fstate;
    s32 first;
} ALLowPass;

typedef struct ALDelay {
    u32 input;
    u32 output;
    s16 ffcoef;
    s16 fbcoef;
    s16 gain;
    char padding_0e[2];
    f32 rsinc;
    f32 rsval;
    s32 rsdelta;
    f32 rsgain;
    ALLowPass *lp;
    ALResampler *rs;
} ALDelay;

typedef struct ALFx {
    ALFilter filter;
    s16 *base;
    s16 *input;
    u32 length;
    ALDelay *delay;
    u8 section_count;
    char padding_25[3];
    ALSetFXParam paramHdl;
} ALFx;

typedef struct ALSynConfig {
    s32 max_virtual_voices;
    s32 max_physical_voices;
    s32 max_updates;
    s32 max_effect_buses;
    void *dma_proc;
    ALHeap *heap;
    s32 outputRate;
    u8 fxType;
    char padding_1d[3];
    s32 *params;
} ALSynConfig;

#define AL_FX_SMALLROOM 1
#define AL_FX_BIGROOM 2
#define AL_FX_CHORUS 3
#define AL_FX_FLANGE 4
#define AL_FX_ECHO 5
#define AL_FX_CUSTOM 6
#define AL_FX 5

extern void alFilterNew(ALFilter *, ALCmdHandler, ALSetParam, s32);
extern s32 alFxParam(void *, s32, void *);
extern Acmd *alFxPull(void *, s16 *, s32, s32, Acmd *);
extern s32 alFxParamHdl(void *, s32, void *);
extern void *alHeapDBAlloc(u8 *, s32, ALHeap *, s32, s32);
#define alHeapAlloc(heap, count, size) \
    alHeapDBAlloc(0, 0, (heap), (count), (size))

extern s32 SMALLROOM_PARAMS[];
extern s32 BIGROOM_PARAMS[];
extern s32 ECHO_PARAMS[];
extern s32 CHORUS_PARAMS[];
extern s32 FLANGE_PARAMS[];
extern s32 NULL_PARAMS[];

#undef _init_lpfilter
#define SCALE 16384
static __inline__ void _init_lpfilter(ALLowPass *lp)
{
    s32 i, temp;
    s16 fc;
    f64 ffc, fcoef;

    temp = lp->fc * SCALE;
    fc = temp >> 15;
    lp->fgain = SCALE - fc;
    lp->first = 1;
    for (i = 0; i < 8; i++)
        lp->fcvec.fccoef[i] = 0;
    lp->fcvec.fccoef[i++] = fc;
    fcoef = ffc = (f64)fc / SCALE;
    for (; i < 16; i++) {
        fcoef *= ffc;
        lp->fcvec.fccoef[i] = (s16)(fcoef * SCALE);
    }
}

void func_80049794(ALFx *r, ALSynConfig *c, ALHeap *hp)
{
    u16 i, j, k;
    s32 *param = 0;
    ALFilter *f = (ALFilter *)r;
    ALDelay *d;

    alFilterNew(f, 0, alFxParam, AL_FX);
    f->handler = alFxPull;
    r->paramHdl = (ALSetFXParam)alFxParamHdl;

    switch (c->fxType) {
    case AL_FX_SMALLROOM: param = SMALLROOM_PARAMS; break;
    case AL_FX_BIGROOM: param = BIGROOM_PARAMS; break;
    case AL_FX_ECHO: param = ECHO_PARAMS; break;
    case AL_FX_CHORUS: param = CHORUS_PARAMS; break;
    case AL_FX_FLANGE: param = FLANGE_PARAMS; break;
    case AL_FX_CUSTOM: param = c->params; break;
    default: param = NULL_PARAMS; break;
    }

    j = 0;
    r->section_count = param[j++];
    r->length = param[j++];
    r->delay = alHeapAlloc(hp, r->section_count, sizeof(ALDelay));
    r->base = alHeapAlloc(hp, r->length, sizeof(s16));
    r->input = r->base;

    for (k = 0; k < r->length; k++)
        r->base[k] = 0;

    for (i = 0; i < r->section_count; i++) {
        d = &r->delay[i];
        d->input = param[j++];
        d->output = param[j++];
        d->fbcoef = param[j++];
        d->ffcoef = param[j++];
        d->gain = param[j++];

        if (param[j]) {
            d->rsinc = ((((f32)param[j++]) / 1000) * 2.0) / c->outputRate;
            d->rsgain = (((f32)param[j++]) / 173123.404906676) *
                        (d->output - d->input);
            d->rsval = 1.0;
            d->rsdelta = 0.0;
            d->rs = alHeapAlloc(hp, 1, sizeof(ALResampler));
            d->rs->state = alHeapAlloc(hp, 1, sizeof(RESAMPLE_STATE));
            d->rs->delta = 0.0;
            d->rs->first = 1;
        } else {
            d->rs = 0;
            j++;
            j++;
        }

        if (param[j]) {
            d->lp = alHeapAlloc(hp, 1, sizeof(ALLowPass));
            d->lp->fstate = alHeapAlloc(hp, 1, sizeof(POLEF_STATE));
            d->lp->fc = param[j++];
            _init_lpfilter(d->lp);
        } else {
            d->lp = 0;
            j++;
        }
    }
}
