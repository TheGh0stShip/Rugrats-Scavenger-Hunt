/* RODATA_VRAM 0x80077720; DATA_VRAM 0x80126A20.
 * Equal-power table follows the SDK source retained by Dr. Mario 64. */
typedef signed short s16;
typedef signed int s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef float f32;

typedef struct Acmd { u32 word0; u32 word1; } Acmd;
typedef struct ALWaveTable ALWaveTable;
typedef struct ALParam ALParam;

struct ALParam {
    ALParam *next;
    s32 delta;
    s16 type;
    char padding_0a[2];
    union { f32 f; s32 i; } data;
    union { f32 f; s32 i; } moredata;
    union { f32 f; s32 i; } stillmoredata;
    union { f32 f; s32 i; } yetstillmoredata;
};

typedef struct N_PVoice {
    char padding_00[0x40];
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
    ALParam *em_ctrlList;
    ALParam *em_ctrlTail;
    s32 em_motion;
    s32 offset;
} N_PVoice;

#define FIXED_SAMPLE 184

extern void __freeParam(ALParam *);
extern s32 n_alEnvmixerParam(void *, s32, void *);

#define A_ENVMIXER 3
#define A_SETVOL 9
#define A_INIT 0x01
#define A_CONTINUE 0x00
#define A_LEFT 0x02
#define A_RIGHT 0x00
#define A_VOL 0x04
#define A_RATE 0x00
#define N_EQPOWER_LENGTH 128
#define SAMPLE184(delta) (((delta) + (184 / 2)) / 184) * 184

#define n_aSetVolume(pkt, f, v, t, r)                                                    \
    {                                                                                   \
        Acmd *_a = (Acmd *)pkt;                                                         \
        _a->words.w0 = (_SHIFTL(A_SETVOL, 24, 8) | _SHIFTL(f, 16, 8) | _SHIFTL(v, 0, 16)); \
        _a->words.w1 = _SHIFTL(t, 16, 16) | _SHIFTL(r, 0, 16);                          \
    }

#define n_aEnvMixer(pkt, f, t, s)                                                          \
    {                                                                                     \
        Acmd *_a = (Acmd *)pkt;                                                           \
        _a->words.w0 = (_SHIFTL(A_ENVMIXER, 24, 8) | _SHIFTL(f, 16, 8) | _SHIFTL(t, 0, 16)); \
        _a->words.w1 = (unsigned int)(s);                                                 \
    }

typedef struct {
    ALParam *next;             /* 0x00 */
    s32 delta;                 /* 0x04 */
    s16 type;                  /* 0x08 */
    s16 unity;                 /* 0x0A */
    f32 pitch;                 /* 0x0C */
    s16 volume;                /* 0x10 */
    u8 pan;                    /* 0x12 */
    u8 fxMix;                  /* 0x13 */
    s32 samples;               /* 0x14 */
    ALWaveTable *wave;         /* 0x18 */
} ALStartParamAlt;

typedef struct {
    ALParam *next;             /* 0x00 */
    s32 delta;                 /* 0x04 */
    s16 type;                  /* 0x08 */
    s16 unity;                 /* 0x0A */
    ALWaveTable *wave;         /* 0x0C */
} ALStartParam;

typedef struct {
    ALParam *next;             /* 0x00 */
    s32 delta;                 /* 0x04 */
    s16 type;                  /* 0x08 */
    N_PVoice *pvoice;          /* 0x0C */
} N_ALFreeParam;

extern s16 n_eqpower[];
extern void n_alLoadParam(N_PVoice *filter, s32 paramID, void *param);
extern Acmd *n_alResamplePull(N_PVoice *e, s16 *outp, Acmd *p);
extern void _n_freePVoice(N_PVoice *pvoice);

Acmd *n_env_text_061C(N_PVoice *filter, s16 *inp, s16 *outp, s32 outCount, Acmd *p);
s16 _getRate(double vol, double tgt, s32 count, u16 *ratel);
s16 _getVol(s16 ivol, s32 samples, s16 ratem, u16 ratel);
Acmd *n_alEnvmixerPull(N_PVoice *filter, s32 sampleOffset, Acmd *p)
{
    Acmd *ptr = p;
    N_PVoice *e = filter;
    s16 inp;
    s32 lastOffset;
    s32 thisOffset = sampleOffset;
    s32 samples;
    s16 loutp = 0;
    s32 fVol;
    ALParam *thisParam;
    s32 outCount = FIXED_SAMPLE;

    inp = 0;

    while (e->em_ctrlList != 0) {
        lastOffset = thisOffset;
        thisOffset = e->em_ctrlList->delta;
        samples = SAMPLE184(thisOffset - lastOffset);
        if (samples == 0)
            thisOffset = lastOffset;
        if (samples > outCount)
            break;

        switch (e->em_ctrlList->type) {
        case 13:
            {
                ALStartParamAlt *param = (ALStartParamAlt *)e->em_ctrlList;
                s32 tmp;

                if (param->unity)
                    e->rs_upitch = 1;
                n_alLoadParam(e, 5, param->wave);
                e->em_motion = 1;
                e->em_first = 1;
                e->em_delta = 0;
                e->em_segEnd = SAMPLE184(param->samples);
                tmp = ((s32)param->volume * (s32)param->volume) >> 15;
                e->em_volume = (s16)tmp;
                e->em_pan = param->pan;
                e->em_dryamt = n_eqpower[param->fxMix];
                e->em_wetamt = n_eqpower[N_EQPOWER_LENGTH - param->fxMix - 1];
                if (param->samples) {
                    e->em_cvolL = 1;
                    e->em_cvolR = 1;
                } else {
                    e->em_cvolL = (e->em_volume * n_eqpower[e->em_pan]) >> 15;
                    e->em_cvolR = (e->em_volume * n_eqpower[N_EQPOWER_LENGTH - e->em_pan - 1]) >> 15;
                }
                e->rs_ratio = param->pitch;
            }
            break;
        case 16:
        case 12:
        case 11:
            ptr = n_env_text_061C(e, &inp, &loutp, samples, ptr);
            if (e->em_delta >= e->em_segEnd) {
                e->em_ltgt = (e->em_volume * n_eqpower[e->em_pan]) >> 15;
                e->em_rtgt = (e->em_volume * n_eqpower[N_EQPOWER_LENGTH - e->em_pan - 1]) >> 15;
                e->em_delta = e->em_segEnd;
                e->em_cvolL = e->em_ltgt;
                e->em_cvolR = e->em_rtgt;
            } else {
                e->em_cvolL = _getVol(e->em_cvolL, e->em_delta, e->em_lratm, e->em_lratl);
                e->em_cvolR = _getVol(e->em_cvolR, e->em_delta, e->em_rratm, e->em_rratl);
            }
            if (e->em_cvolL == 0)
                e->em_cvolL = 1;
            if (e->em_cvolR == 0)
                e->em_cvolR = 1;

            if (e->em_ctrlList->type == 12)
                e->em_pan = (s16)e->em_ctrlList->data.i;

            if (e->em_ctrlList->type == 11) {
                e->em_delta = 0;
                fVol = (e->em_ctrlList->data.i);
                fVol = (fVol * fVol) >> 15;
                e->em_volume = (s16)fVol;
                e->em_segEnd = SAMPLE184(e->em_ctrlList->moredata.i);
            }

            if (e->em_ctrlList->type == 16) {
                e->em_dryamt = n_eqpower[e->em_ctrlList->data.i];
                e->em_wetamt = n_eqpower[N_EQPOWER_LENGTH - e->em_ctrlList->data.i - 1];
            }

            e->em_first = 1;
            break;
        case 14:
            {
                ALStartParam *param = (ALStartParam *)e->em_ctrlList;

                if (param->unity)
                    e->rs_upitch = 1;
                n_alLoadParam(e, 5, param->wave);
                e->em_motion = 1;
            }
            break;
        case 15:
            ptr = n_env_text_061C(e, &inp, &loutp, samples, ptr);
            n_alEnvmixerParam(e, 4, 0);
            break;
        case 0:
            {
                N_ALFreeParam *param = (N_ALFreeParam *)e->em_ctrlList;

                param->pvoice->offset = 0;
                _n_freePVoice(param->pvoice);
            }
            break;
        case 7:
            ptr = n_env_text_061C(e, &inp, &loutp, samples, ptr);
            e->rs_ratio = e->em_ctrlList->data.f;
            break;
        case 8:
            ptr = n_env_text_061C(e, &inp, &loutp, samples, ptr);
            e->rs_upitch = 1;
            break;
        case 5:
            ptr = n_env_text_061C(e, &inp, &loutp, samples, ptr);
            n_alLoadParam(e, 5, (void *)e->em_ctrlList->data.i);
            break;
        default:
            ptr = n_env_text_061C(e, &inp, &loutp, samples, ptr);
            n_alEnvmixerParam(e, e->em_ctrlList->type, (void *)e->em_ctrlList->data.i);
            break;
        }

        loutp += (samples << 1);
        outCount -= samples;

        thisParam = e->em_ctrlList;
        e->em_ctrlList = e->em_ctrlList->next;
        if (e->em_ctrlList == 0)
            e->em_ctrlTail = 0;

        __freeParam(thisParam);
    }

    ptr = n_env_text_061C(e, &inp, &loutp, outCount, ptr);

    if (e->em_delta > e->em_segEnd)
        e->em_delta = e->em_segEnd;

    return ptr;
}
