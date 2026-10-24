typedef signed short s16;
typedef signed int s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef float f32;
typedef struct ALHeap ALHeap;
typedef struct ALFilter { void *source; void *handler; void *setParam; s16 inp; s16 outp; s32 type; } ALFilter;
typedef struct ALLowPass { s16 fc; s16 fgain; unsigned char pad[4]; s16 fccoef[16]; void *fstate; s32 first; } ALLowPass;
typedef struct ALResampler { ALFilter filter; void *state; f32 ratio; s32 upitch; f32 delta; s32 first; void *ctrlList; void *ctrlTail; s32 motion; } ALResampler;
typedef struct ALDelay { u32 input; u32 output; s16 ffcoef; s16 fbcoef; s16 gain; s16 pad; f32 rsinc; f32 rsval; s32 rsdelta; f32 rsgain; ALLowPass *lp; ALResampler *rs; } ALDelay;
typedef struct ALFx { ALFilter filter; s16 *base; s16 *input; u32 length; ALDelay *delay; u8 section_count; u8 pad[3]; void *paramHdl; } ALFx;
typedef struct ALSynConfig { s32 maxVVoices; s32 maxPVoices; s32 maxUpdates; s32 maxFXbusses; void *dmaproc; ALHeap *heap; s32 outputRate; u8 fxType; u8 pad[3]; s32 *params; } ALSynConfig;

static s32 SMALLROOM_PARAMS[26] = {3,4000,0,2160,9830,-9830,0,0,0,0,760,1520,3276,-3276,0x3fff,0,0,0,0,2400,5000,0,0,0,0,0x5000};
static s32 BIGROOM_PARAMS[34] = {4,4000,0,2640,9830,-9830,0,0,0,0,880,2160,3276,-3276,0x3fff,0,0,0,2640,3640,3276,-3276,0x3fff,0,0,0,0,3760,8000,0,0,0,0,0x5000};
static s32 ECHO_PARAMS[10] = {1,8000,0,7160,12000,0,0x7fff,0,0,0};
static s32 CHORUS_PARAMS[10] = {1,800,0,200,0x4000,0,0x7fff,7600,700,0};
static s32 FLANGE_PARAMS[10] = {1,800,0,200,0,0x5fff,0x7fff,380,500,0};
static s32 NULL_PARAMS[10] = {0};

extern void *func_8004CAC0(void *, s32, ALHeap *, s32, s32);
extern void func_800496F0(ALLowPass *);
#define alHeapAlloc(h,n,s) func_8004CAC0(0,0,(h),(n),(s))

void func_80040DD0(ALFx **fx_ar, ALSynConfig *c, ALHeap *hp)
{
    u16 i, j, k;
    s32 *param = 0;
    ALDelay *d;
    ALFx *r;
    *fx_ar = r = (ALFx *)alHeapAlloc(hp, 1, sizeof(ALFx));
    switch (c->fxType) {
    case 1: param = SMALLROOM_PARAMS; break;
    case 2: param = BIGROOM_PARAMS; break;
    case 5: param = ECHO_PARAMS; break;
    case 3: param = CHORUS_PARAMS; break;
    case 4: param = FLANGE_PARAMS; break;
    case 6: param = c->params; break;
    default: param = NULL_PARAMS; break;
    }
    j = 0;
    r->section_count = param[j++];
    r->length = param[j++];
    r->delay = alHeapAlloc(hp, r->section_count, sizeof(ALDelay));
    r->base = alHeapAlloc(hp, r->length, sizeof(s16));
    r->input = r->base;
    for (k = 0; k < r->length; k++) r->base[k] = 0;
    for (i = 0; i < r->section_count; i++) {
        d = &r->delay[i];
        d->input = param[j++]; d->output = param[j++]; d->fbcoef = param[j++]; d->ffcoef = param[j++]; d->gain = param[j++];
        if (param[j]) {
            d->rsinc = ((((f32)param[j++]) / 1000) * 2.0) / c->outputRate;
            d->rsgain = (((f32)param[j++]) / 173123.404906676) * (d->output - d->input);
            d->rsval = 1.0; d->rsdelta = 0.0;
            d->rs = alHeapAlloc(hp, 1, sizeof(ALResampler));
            d->rs->state = alHeapAlloc(hp, 1, 0x20);
            d->rs->delta = 0.0; d->rs->first = 1;
        } else { d->rs = 0; j++; j++; }
        if (param[j]) {
            d->lp = alHeapAlloc(hp, 1, sizeof(ALLowPass));
            d->lp->fstate = alHeapAlloc(hp, 1, 8);
            d->lp->fc = param[j++];
            func_800496F0(d->lp);
        } else { d->lp = 0; j++; }
    }
}
