#include "n_audio_internal.h"

typedef struct ALDelay {
    u32 input;
    u32 output;
    s16 ffcoef;
    s16 fbcoef;
    s16 gain;
    f32 rsinc;
    f32 rsval;
    s32 rsdelta;
    f32 rsgain;
    void *lp;
    void *rs;
} ALDelay;

typedef struct ALFx {
    ALFilter filter;
    s16 *base;
    s16 *input;
    u32 length;
    ALDelay *delay;
    u8 section_count;
    void *paramHdl;
} ALFx;

extern N_ALSynth *n_syn;
extern Acmd *n_alAuxBusPull(s32 sampleOffset, Acmd *p);
extern Acmd *_n_loadOutputBuffer(ALFx *r, ALDelay *d, s32 buff, Acmd *p);
extern Acmd *_n_loadBuffer(ALFx *r, s16 *curr_ptr, s32 buff, s32 count, Acmd *p);
extern Acmd *_n_saveBuffer(ALFx *r, s16 *curr_ptr, s32 buff, Acmd *p);
extern Acmd *_n_filterBuffer(void *lp, s32 buff, Acmd *p);

#define aMix(pkt, f, g, i, o) { Acmd *_a = (Acmd *)(pkt); \
    _a->words.w0 = _SHIFTL(12, 24, 8) | _SHIFTL(f, 16, 8) | _SHIFTL(g, 0, 16); \
    _a->words.w1 = _SHIFTL(i, 16, 16) | _SHIFTL(o, 0, 16); }
#define aDMEMMove(pkt, i, o, c) { Acmd *_a = (Acmd *)(pkt); \
    _a->words.w0 = _SHIFTL(A_DMEMMOVE, 24, 8) | _SHIFTL(i, 0, 24); \
    _a->words.w1 = _SHIFTL(o, 16, 16) | _SHIFTL(c, 0, 16); }
#define SWAP(in, out) { s16 t = out; out = in; in = t; }

Acmd *n_alFxPull(s32 sampleOffset, Acmd *p)
{
    Acmd *ptr = p;
    ALFx *r = (ALFx *)n_syn->auxBus->fx;
    s16 i, buff1, buff2, input, output;
    s16 *in_ptr, *out_ptr, *prev_out_ptr = 0;
    ALDelay *d;

    ptr = n_alAuxBusPull(sampleOffset, p);

    input = 0x7C0;
    output = 0x930;
    buff1 = 0;
    buff2 = 0x170;

    aMix(ptr++, 0, 0xda83, 0x7C0, input);
    aMix(ptr++, 0, 0x5a82, 0x930, input);

    ptr = _n_saveBuffer(r, r->input, input, ptr);

    aClearBuffer(ptr++, output, FIXED_SAMPLE << 1);

    for (i = 0; i < r->section_count; i++) {
        d = &r->delay[i];
        in_ptr = &r->input[-d->input];
        out_ptr = &r->input[-d->output];

        if (in_ptr == prev_out_ptr) {
            SWAP(buff1, buff2);
        } else {
            ptr = _n_loadBuffer(r, in_ptr, buff1, FIXED_SAMPLE, ptr);
        }
        ptr = _n_loadOutputBuffer(r, d, buff2, ptr);

        if (d->ffcoef) {
            aMix(ptr++, 0, (u16)d->ffcoef, buff1, buff2);
            if (!d->rs && !d->lp) {
                ptr = _n_saveBuffer(r, out_ptr, buff2, ptr);
            }
        }

        if (d->fbcoef) {
            aMix(ptr++, 0, (u16)d->fbcoef, buff2, buff1);
            ptr = _n_saveBuffer(r, in_ptr, buff1, ptr);
        }

        if (d->lp)
            ptr = _n_filterBuffer(d->lp, buff2, ptr);

        if (!d->rs)
            ptr = _n_saveBuffer(r, out_ptr, buff2, ptr);

        if (d->gain)
            aMix(ptr++, 0, (u16)d->gain, buff2, output);

        prev_out_ptr = &r->input[d->output];
    }

    r->input += FIXED_SAMPLE;
    if (r->input > &r->base[r->length])
        r->input -= r->length;

    aDMEMMove(ptr++, output, 0x7C0, FIXED_SAMPLE << 1);

    return ptr;
}
