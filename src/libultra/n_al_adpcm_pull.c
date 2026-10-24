#include "n_audio_internal.h"
#define A_ADPCM 1
#define A_LOADBUFF 4
#define A_DMEMMOVE 10
#define A_LOADADPCM 11
#define A_SETLOOP 15
#define A_LOOP 0x02
#define ADPCMFSIZE 16
#define ADPCMFBYTES 9
#define LFSAMPLES 4
#define ADPCMVSIZE 8
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef s32 (*ALDMAproc)(s32 addr, s32 len, void *state);

extern void alCopy(void *src, void *dest, s32 len);

Acmd *_decodeChunk(Acmd *ptr, N_PVoice *f, s32 tsam, s32 nbytes, s16 outp, s16 inp, u32 flags);
Acmd *n_alAdpcmPull(N_PVoice *filter, s16 *outp, s32 outCount, Acmd *p)
{
    Acmd *ptr = p;
    s16 inp;
    s32 tsam;
    s32 nframes;
    s32 nbytes;
    s32 overFlow;
    s32 startZero;
    s32 nOver;
    s32 nSam;
    s32 op;
    s32 nLeft;
    s32 bEnd;
    s32 decoded = 0;
    s32 looped = 0;
    N_PVoice *f = filter;

    if (outCount == 0)
        return ptr;

    inp = 0;
    {
        Acmd *_a = ptr++;
        _a->words.w0 = _SHIFTL(A_LOADADPCM, 24, 8) | _SHIFTL(f->dc_bookSize, 0, 24);
        _a->words.w1 = K0_TO_PHYS(f->dc_table->waveInfo.adpcmWave.book->book);
    }

    looped = (outCount + f->dc_sample > f->dc_loop.end) && (f->dc_loop.count != 0);
    if (looped)
        nSam = f->dc_loop.end - f->dc_sample;
    else
        nSam = outCount;

    if (f->dc_lastsam)
        nLeft = ADPCMFSIZE - f->dc_lastsam;
    else
        nLeft = 0;

    tsam = nSam - nLeft;
    if (tsam < 0)
        tsam = 0;

    nframes = (tsam + ADPCMFSIZE - 1) >> LFSAMPLES;
    nbytes = nframes * ADPCMFBYTES;

    if (looped) {
        ptr = _decodeChunk(ptr, f, tsam, nbytes, *outp, inp, f->dc_first);

        if (f->dc_lastsam)
            *outp += (f->dc_lastsam << 1);
        else
            *outp += (ADPCMFSIZE << 1);

        f->dc_lastsam = f->dc_loop.start & 0xF;
        f->dc_memin = (s32)f->dc_table->base + ADPCMFBYTES * ((s32)(f->dc_loop.start >> LFSAMPLES) + 1);
        f->dc_sample = f->dc_loop.start;

        bEnd = *outp;
        while (outCount > nSam) {
            outCount -= nSam;
            op = (bEnd + ((nframes + 1) << (LFSAMPLES + 1)) + 16) & ~0x1F;
            bEnd += (nSam << 1);

            if ((f->dc_loop.count != -1) && (f->dc_loop.count != 0))
                f->dc_loop.count--;

            nSam = MIN(outCount, f->dc_loop.end - f->dc_loop.start);
            tsam = nSam - ADPCMFSIZE + f->dc_lastsam;
            if (tsam < 0)
                tsam = 0;
            nframes = (tsam + ADPCMFSIZE - 1) >> LFSAMPLES;
            nbytes = nframes * ADPCMFBYTES;
            ptr = _decodeChunk(ptr, f, tsam, nbytes, op, inp, f->dc_first | A_LOOP);
            {
                Acmd *_a = ptr++;
                _a->words.w0 = _SHIFTL(A_DMEMMOVE, 24, 8) | _SHIFTL(op + (f->dc_lastsam << 1), 0, 24);
                _a->words.w1 = _SHIFTL(bEnd, 16, 16) | _SHIFTL(nSam << 1, 0, 16);
            }
        }

        f->dc_lastsam = (outCount + f->dc_lastsam) & 0xF;
        f->dc_sample += outCount;
        f->dc_memin += ADPCMFBYTES * nframes;
        return ptr;
    }

    nSam = nframes << LFSAMPLES;

    overFlow = f->dc_memin + nbytes - ((s32)f->dc_table->base + f->dc_table->len);
    if (overFlow < 0)
        overFlow = 0;
    nOver = (overFlow / ADPCMFBYTES) << LFSAMPLES;
    if (nOver > nSam + nLeft)
        nOver = nSam + nLeft;

    nbytes -= overFlow;

    if ((nOver - (nOver & 0xF)) < outCount) {
        decoded = 1;
        ptr = _decodeChunk(ptr, f, nSam - nOver, nbytes, *outp, inp, f->dc_first);

        if (f->dc_lastsam)
            *outp += (f->dc_lastsam << 1);
        else
            *outp += (ADPCMFSIZE << 1);

        f->dc_lastsam = (outCount + f->dc_lastsam) & 0xF;
        f->dc_sample += outCount;
        f->dc_memin += ADPCMFBYTES * nframes;
    } else {
        f->dc_lastsam = 0;
        f->dc_memin += ADPCMFBYTES * nframes;
    }

    if (nOver) {
        f->dc_lastsam = 0;
        if (decoded)
            startZero = (nLeft + nSam - nOver) << 1;
        else
            startZero = 0;
        aClearBuffer(ptr++, startZero + *outp, nOver << 1);
    }
    return ptr;
}
