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

typedef struct { unsigned char pad[0x34]; struct { unsigned char pad[0x24]; ALFx *fx; } *auxBus; unsigned char pad38[8]; s32 outputRate; } synth_t;
typedef struct { void *a; void *b; } prealloc_t;

extern synth_t *D_800C4040;
extern void *D_800C4050[6];
extern void func_800496F0(ALLowPass *);

void func_80042270(s32 *param)
{
    u16 i, j;
    ALDelay *d;
    ALFx *r;

    r = D_800C4040->auxBus->fx;
    j = 2;
    r->section_count = param[0];
    r->length = param[1];
    r->delay = (ALDelay *)D_800C4050[0];
    r->base = D_800C4050[1];
    r->input = r->base;
    for (i = 0; i < r->length; i++) r->base[i] = 0;
    for (i = 0; i < r->section_count; i++) {
        d = &r->delay[i];
        d->input = param[j++]; d->output = param[j++]; d->fbcoef = param[j++]; d->ffcoef = param[j++]; d->gain = param[j++];
        if (param[j]) {
            d->rsinc = ((((f32)param[j++]) / 1000) * 2.0) / D_800C4040->outputRate;
            d->rsgain = (((f32)param[j++]) / 173123.404906676) * (d->output - d->input);
            d->rsval = 1.0; d->rsdelta = 0.0;
            d->rs = D_800C4050[2];
            d->rs->state = D_800C4050[3];
            d->rs->delta = 0.0; d->rs->first = 1;
        } else { d->rs = 0; j++; j++; }
        if (param[j]) {
            d->lp = D_800C4050[4];
            d->lp->fstate = D_800C4050[5];
            d->lp->fc = param[j++];
            func_800496F0(d->lp);
        } else { d->lp = 0; j++; }
    }
}
