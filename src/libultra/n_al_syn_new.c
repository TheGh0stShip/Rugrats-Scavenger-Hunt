#include "n_audio_internal.h"

extern N_ALSynth *n_syn;
extern void *alHeapDBAlloc(u8 *file, s32 line, ALHeap *heap, s32 count, s32 size);
extern void alLink(ALLink *link, ALLink *destination);
extern void alN_PVoiceNew(N_PVoice *voice, void *dma, ALHeap *heap);
extern void *n_alFxCreate(s16 bus, ALSynConfig *config, ALHeap *heap);
extern void n_alFxPull(void);
extern void n_alMainBusPull(void);

#define alHeapAlloc(heap, count, size) alHeapDBAlloc(0, 0, heap, count, size)

void n_alSynNew(ALSynConfig *config)
{
    s32 i;
    N_PVoice *physical_voices;
    ALHeap *heap = config->heap;
    ALParam *parameters;
    ALParam *parameter;

    n_syn->head = 0;
    n_syn->numPVoices = config->maxPVoices;
    n_syn->curSamples = 0;
    n_syn->paramSamples = 0;
    n_syn->outputRate = config->outputRate;
    n_syn->maxOutSamples = 184;
    n_syn->dma = config->dmaproc;
    n_syn->sv_dramout = 0;
    n_syn->sv_first = 1;

    n_syn->auxBus = alHeapAlloc(heap, 1, sizeof(N_ALAuxBus));
    n_syn->auxBus->sourceCount = 0;
    n_syn->auxBus->maxSources = config->maxPVoices;
    n_syn->auxBus->sources = alHeapAlloc(heap, config->maxPVoices, sizeof(N_PVoice *));

    n_syn->mainBus = alHeapAlloc(heap, 1, 0x14);

    if (config->fxType) {
        n_syn->auxBus->fx = n_alFxCreate(0, config, heap);
        n_syn->mainBus->filter.handler = n_alFxPull;
    } else {
        n_syn->mainBus->filter.handler = n_alMainBusPull;
    }

    n_syn->pFreeList.next = 0;
    n_syn->pFreeList.prev = 0;
    n_syn->pLameList.next = 0;
    n_syn->pLameList.prev = 0;
    n_syn->pAllocList.next = 0;
    n_syn->pAllocList.prev = 0;

    physical_voices = alHeapAlloc(heap, config->maxPVoices, sizeof(N_PVoice));
    for (i = 0; i < config->maxPVoices; i++) {
        N_PVoice *voice = &physical_voices[i];
        alLink((ALLink *)voice, &n_syn->pFreeList);
        voice->vvoice = 0;
        alN_PVoiceNew(voice, n_syn->dma, heap);
        n_syn->auxBus->sources[n_syn->auxBus->sourceCount++] = voice;
    }

    parameters = alHeapAlloc(heap, config->maxUpdates, sizeof(ALParam));
    n_syn->paramList = 0;
    for (i = 0; i < config->maxUpdates; i++) {
        parameter = &parameters[i];
        parameter->next = n_syn->paramList;
        n_syn->paramList = parameter;
    }
    n_syn->heap = heap;
}
