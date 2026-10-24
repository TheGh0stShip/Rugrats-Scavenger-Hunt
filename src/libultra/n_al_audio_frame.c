#include "n_audio_internal.h"

typedef struct ALPlayer {
    struct ALPlayer *next;
    void *clientData;
    s32 (*handler)(void *);
    s32 callTime;
    s32 samplesLeft;
} ALPlayer;

extern N_ALSynth *n_syn;
extern s32 _n_timeToSamplesNoRound(s32 micros);
extern Acmd *n_alSavePull(s32 sampleOffset, Acmd *p);
extern void _n_collectPVoices(void);

Acmd *n_alAudioFrame(Acmd *cmdList, s32 *cmdLen, s16 *outBuf, s32 outLen)
{
    ALPlayer *client;
    Acmd *cmdlEnd = cmdList;
    s32 nOut;
    s16 *lOutBuf = outBuf;

    if (n_syn->head == 0) {
        *cmdLen = 0;
        return cmdList;
    }

    client = n_syn->head;
    while (client->samplesLeft - n_syn->curSamples < outLen) {
        n_syn->paramSamples = client->samplesLeft & ~15;
        client->samplesLeft += _n_timeToSamplesNoRound((*client->handler)(client));
    }
    n_syn->paramSamples &= ~15;

    while (outLen > 0) {
        nOut = n_syn->maxOutSamples;
        if (outLen < nOut)
            nOut = outLen;
        n_syn->sv_dramout = (s32)lOutBuf;
        cmdlEnd = n_alSavePull(n_syn->curSamples, cmdlEnd);
        outLen -= nOut;
        lOutBuf += nOut << 1;
        n_syn->curSamples += nOut;
    }

    *cmdLen = (s32)(cmdlEnd - cmdList);
    _n_collectPVoices();
    return cmdlEnd;
}
