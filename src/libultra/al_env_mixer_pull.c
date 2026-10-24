typedef signed short s16;
typedef signed int s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef float f32;

typedef struct Acmd { u32 word0; u32 word1; } Acmd;
typedef struct ALFilter ALFilter;
typedef struct ALParam ALParam;
typedef struct PVoice PVoice;
typedef struct ALSynth { char opaque; } ALSynth;
typedef Acmd *(*ALCmdHandler)(void *, s16 *, s32, s32, Acmd *);
typedef s32 (*ALSetParam)(void *, s32, void *);

struct ALFilter {
    ALFilter *source;
    ALCmdHandler handler;
    ALSetParam setParam;
    s16 input;
    s16 output;
    s32 type;
};

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

typedef struct ALStartParamAlt {
    ALParam *next;
    s32 delta;
    s16 type;
    s16 unity;
    f32 pitch;
    s16 volume;
    u8 pan;
    u8 fxMix;
    s32 samples;
    void *wave;
} ALStartParamAlt;

typedef struct ALStartParam {
    ALParam *next;
    s32 delta;
    s16 type;
    s16 unity;
    void *wave;
} ALStartParam;

struct PVoice {
    char padding_00[0xD8];
    s32 offset;
};

typedef struct ALFreeParam {
    ALParam *next;
    s32 delta;
    s16 type;
    char padding_0a[2];
    PVoice *pvoice;
} ALFreeParam;

typedef struct ALEnvMixer {
    ALFilter filter;
    void *state;
    s16 pan;
    s16 volume;
    s16 cvolL;
    s16 cvolR;
    s16 dryamt;
    s16 wetamt;
    u16 lratl;
    s16 lratm;
    s16 ltgt;
    u16 rratl;
    s16 rratm;
    s16 rtgt;
    s32 delta;
    s32 segEnd;
    s32 first;
    ALParam *ctrlList;
    ALParam *ctrlTail;
    ALFilter **sources;
    s32 motion;
} ALEnvMixer;

typedef struct ALGlobals { ALSynth drvr; } ALGlobals;

enum {
    AL_FILTER_FREE_VOICE,
    AL_FILTER_SET_SOURCE,
    AL_FILTER_ADD_SOURCE,
    AL_FILTER_ADD_UPDATE,
    AL_FILTER_RESET,
    AL_FILTER_SET_WAVETABLE,
    AL_FILTER_SET_DRAM,
    AL_FILTER_SET_PITCH,
    AL_FILTER_SET_UNITY_PITCH,
    AL_FILTER_START,
    AL_FILTER_SET_STATE,
    AL_FILTER_SET_VOLUME,
    AL_FILTER_SET_PAN,
    AL_FILTER_START_VOICE_ALT,
    AL_FILTER_START_VOICE,
    AL_FILTER_STOP_VOICE,
    AL_FILTER_SET_FXAMT
};

#define AL_MAX_RSP_SAMPLES 160
#define AL_RESAMPLER_OUT 0
#define EQPOWER_LENGTH 128

extern void __assert(char *, char *, s32);
#define assert(condition) ((condition) ? (void)0 : __assert("EX", __FILE__, __LINE__))
extern s16 eqpower[];
extern ALGlobals *alGlobals;
extern Acmd *func_8004B160(void *, s16 *, s16 *, s32, s32, Acmd *);
extern f32 func_8004B7E4(f32, s32, s16, u16);
extern void __freeParam(ALParam *);
extern void _freePVoice(ALSynth *, PVoice *);

Acmd *func_8004AB40(void *filter, s16 *outp, s32 outCount, s32 sampleOffset,
                    Acmd *p)
{
    Acmd        *ptr = p;
    ALEnvMixer  *e = (ALEnvMixer *)filter;
    s16         inp;
    s32         lastOffset;
    s32         thisOffset = sampleOffset;
    s32         samples;
    s16         loutp = 0;
    s32         fVol;
    ALParam     *thisParam;

    inp = AL_RESAMPLER_OUT;

    while (e->ctrlList != 0) {
        lastOffset = thisOffset;
        thisOffset = e->ctrlList->delta;
        samples    = thisOffset - lastOffset;
        if (samples > outCount)
            break;
#line 104 "env.c"
        assert(samples >= 0);
        assert(samples <= AL_MAX_RSP_SAMPLES);

        switch (e->ctrlList->type) {
          case (AL_FILTER_START_VOICE_ALT):
              {
                  ALStartParamAlt *param = (ALStartParamAlt *)e->ctrlList;
                  ALFilter     *f = (ALFilter *) e;
                  s32 tmp;

                  if (param->unity) {
                      (*e->filter.setParam)(&e->filter,
                                            AL_FILTER_SET_UNITY_PITCH, 0);
                  }

                  (*e->filter.setParam)(&e->filter, AL_FILTER_SET_WAVETABLE,
                                        param->wave);
                  (*e->filter.setParam)(&e->filter, AL_FILTER_START, 0);

                  e->first  = 1;
                  e->delta  = 0;
                  e->segEnd = param->samples;

                  tmp = ((s32)param->volume * (s32)param->volume) >> 15;
                  e->volume = (s16) tmp;
                  e->pan    = param->pan;
                  e->dryamt = eqpower[param->fxMix];
                  e->wetamt = eqpower[EQPOWER_LENGTH - param->fxMix - 1];

                  if (param->samples) {
                      e->cvolL  = 1;
                      e->cvolR  = 1;
                  } else {
                      e->cvolL = (e->volume * eqpower[e->pan]) >> 15;
                      e->cvolR = (e->volume *
                                  eqpower[EQPOWER_LENGTH - e->pan - 1]) >> 15;
                  }

                  if (f->source) {
                      union {
                          f32 f;
                          s32 i;
                      } data;
                      data.f = param->pitch;
                      (*f->source->setParam)(f->source, AL_FILTER_SET_PITCH,
                                            (void *)data.i);
                  }
              }
              break;

          case (AL_FILTER_SET_FXAMT):
          case (AL_FILTER_SET_PAN):
          case (AL_FILTER_SET_VOLUME):
              ptr = func_8004B160(e, &inp, &loutp, samples, sampleOffset, ptr);

              if (e->delta >= e->segEnd){
                  e->ltgt = (e->volume * eqpower[e->pan]) >> 15;
                  e->rtgt = (e->volume *
                             eqpower[EQPOWER_LENGTH - e->pan - 1]) >> 15;
                  e->delta = e->segEnd;
                  e->cvolL = e->ltgt;
                  e->cvolR = e->rtgt;
              } else {
                  e->cvolL = func_8004B7E4(e->cvolL, e->delta, e->lratm,
                                           e->lratl);
                  e->cvolR = func_8004B7E4(e->cvolR, e->delta, e->rratm,
                                           e->rratl);
              }

              if (e->cvolL == 0) e->cvolL = 1;
              if (e->cvolR == 0) e->cvolR = 1;

              if (e->ctrlList->type == AL_FILTER_SET_PAN)
                  e->pan = (s16)e->ctrlList->data.i;

              if (e->ctrlList->type == AL_FILTER_SET_VOLUME){
                  e->delta = 0;
                  fVol = e->ctrlList->data.i;
                  fVol = (fVol * fVol) >> 15;
                  e->volume = (s16)fVol;
                  e->segEnd = e->ctrlList->moredata.i;
              }

              if (e->ctrlList->type == AL_FILTER_SET_FXAMT){
                  e->dryamt = eqpower[e->ctrlList->data.i];
                  e->wetamt = eqpower[EQPOWER_LENGTH - e->ctrlList->data.i - 1];
              }
              e->first = 1;
              break;

          case (AL_FILTER_START_VOICE):
              {
                  ALStartParam *start = (ALStartParam *)e->ctrlList;
                  if (start->unity) {
                      (*e->filter.setParam)(&e->filter,
                                            AL_FILTER_SET_UNITY_PITCH, 0);
                  }
                  (*e->filter.setParam)(&e->filter, AL_FILTER_SET_WAVETABLE,
                                        start->wave);
                  (*e->filter.setParam)(&e->filter, AL_FILTER_START, 0);
              }
              break;

          case (AL_FILTER_STOP_VOICE):
              ptr = func_8004B160(e, &inp, &loutp, samples, sampleOffset, ptr);
              (*e->filter.setParam)(&e->filter, AL_FILTER_RESET, 0);
              break;

          case (AL_FILTER_FREE_VOICE):
              {
                  ALSynth *drvr = &alGlobals->drvr;
                  ALFreeParam *param = (ALFreeParam *)e->ctrlList;
                  param->pvoice->offset = 0;
                  _freePVoice(drvr, (PVoice *)param->pvoice);
              }
              break;

          default:
              ptr = func_8004B160(e, &inp, &loutp, samples, sampleOffset, ptr);
              (*e->filter.setParam)(&e->filter, e->ctrlList->type,
                                    (void *)e->ctrlList->data.i);
              break;
        }
        loutp += samples << 1;
        outCount -= samples;
        thisParam = e->ctrlList;
        e->ctrlList = e->ctrlList->next;
        if (e->ctrlList == 0)
            e->ctrlTail = 0;
        __freeParam(thisParam);
    }

    ptr = func_8004B160(e, &inp, &loutp, outCount, sampleOffset, ptr);
    if (e->delta > e->segEnd)
        e->delta = e->segEnd;
    return ptr;
}
