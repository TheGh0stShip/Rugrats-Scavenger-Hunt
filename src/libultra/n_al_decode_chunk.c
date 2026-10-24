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
Acmd *_decodeChunk(Acmd *ptr, N_PVoice *f, s32 tsam, s32 nbytes, s16 outp, s16 inp, u32 flags)
{
    s32 dramAlign;
    s32 dramLoc;

    if (nbytes > 0) {
        dramLoc = ((ALDMAproc)f->dc_dma)(f->dc_memin, nbytes, f->dc_dmaState);
        dramAlign = dramLoc & 0x7;
        nbytes += dramAlign;
        {
            Acmd *_a = ptr++;
            _a->words.w0 = _SHIFTL(A_LOADBUFF, 24, 8) | _SHIFTL(nbytes + 8 - (nbytes & 0x7), 12, 12) | _SHIFTL(inp, 0, 12);
            _a->words.w1 = (unsigned int)(dramLoc - dramAlign);
        }
    } else {
        dramAlign = 0;
    }

    if (flags & A_LOOP) {
        Acmd *_a = ptr++;
        _a->words.w0 = _SHIFTL(A_SETLOOP, 24, 8);
        _a->words.w1 = K0_TO_PHYS(f->dc_lstate);
    }

    {
        Acmd *_a = ptr++;
        _a->words.w0 = _SHIFTL(A_ADPCM, 24, 8) | _SHIFTL(f->dc_state, 0, 24);
        _a->words.w1 = _SHIFTL(flags, 28, 4) | _SHIFTL(tsam << 1, 16, 12) | _SHIFTL(dramAlign, 12, 4) | _SHIFTL(outp, 0, 12);
    }

    f->dc_first = 0;
    return ptr;
}
