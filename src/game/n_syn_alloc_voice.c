typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned char u8;

typedef struct ALParam {
    struct ALParam *next;
    s32 delta;
    s16 type;
    s32 data;
    s32 moredata;
} ALParam;

typedef struct N_PVoice N_PVoice;

typedef struct N_ALVoice {
    void *node[2];
    N_PVoice *pvoice;
    void *table;
    void *clientPrivate;
    s16 state;
    s16 priority;
    s16 fxBus;
    s16 unityPitch;
} N_ALVoice;

struct N_PVoice {
    void *node[2];
    N_ALVoice *vvoice;
    unsigned char pad[0x88 - 0xC];
    s32 offset;
};

typedef struct {
    s16 priority;
    s16 fxBus;
    u8 unityPitch;
} ALVoiceConfig;

typedef struct N_ALSynth { unsigned char pad[0x1C]; s32 paramSamples; } N_ALSynth;

extern N_ALSynth *D_80055AF4;
extern s32 func_8003F0A8(N_PVoice **pvoice, s16 priority);
extern ALParam *func_80040BA8(void);
extern s32 func_8003F6E8(void *filter, s32 paramID, void *param);

s32 func_8003EF90(N_ALVoice *voice, ALVoiceConfig *vc)
{
    N_PVoice *pvoice = 0;
    ALParam *update;
    s32 stolen;

    voice->priority = vc->priority;
    voice->unityPitch = vc->unityPitch;
    voice->table = 0;
    voice->fxBus = vc->fxBus;
    voice->state = 0;
    voice->pvoice = 0;

    stolen = func_8003F0A8(&pvoice, vc->priority);

    if (pvoice) {
        if (stolen) {
            pvoice->offset = 552;
            pvoice->vvoice->pvoice = 0;
            pvoice->vvoice = voice;
            voice->pvoice = pvoice;

            update = func_80040BA8();
            update->delta = D_80055AF4->paramSamples;
            update->type = 11;
            update->data = 0;
            update->moredata = 0x170;
            func_8003F6E8(voice->pvoice, 3, update);

            update = func_80040BA8();
            if (update) {
                update->delta = D_80055AF4->paramSamples + pvoice->offset;
                update->type = 15;
                update->next = 0;
                func_8003F6E8(voice->pvoice, 3, update);
            }
        } else {
            pvoice->offset = 0;
            pvoice->vvoice = voice;
            voice->pvoice = pvoice;
        }
    }
    return pvoice != 0;
}
