#include "n_audio_internal.h"

#define A_ENVMIXER 3
#define A_SETVOL 9
#define A_INIT 0x01
#define A_CONTINUE 0x00
#define A_LEFT 0x02
#define A_RIGHT 0x00
#define A_VOL 0x04
#define A_RATE 0x00
#define N_EQPOWER_LENGTH 128

#define n_aSetVolume(pkt, f, v, t, r)                                      \
    {                                                                      \
        Acmd *_a = (Acmd *)(pkt);                                          \
        _a->words.w0 = _SHIFTL(A_SETVOL, 24, 8) | _SHIFTL(f, 16, 8) |     \
                       _SHIFTL(v, 0, 16);                                  \
        _a->words.w1 = _SHIFTL(t, 16, 16) | _SHIFTL(r, 0, 16);            \
    }

#define n_aEnvMixer(pkt, f, t, s)                                          \
    {                                                                      \
        Acmd *_a = (Acmd *)(pkt);                                          \
        _a->words.w0 = _SHIFTL(A_ENVMIXER, 24, 8) | _SHIFTL(f, 16, 8) |   \
                       _SHIFTL(t, 0, 16);                                  \
        _a->words.w1 = (u32)(s);                                           \
    }

extern s16 n_eqpower[];
extern Acmd *n_alResamplePull(N_PVoice *voice, s16 *output, Acmd *commands);
extern u32 osVirtualToPhysical(void *address);

s16 _getRate(double volume, double target, s32 count, u16 *fraction);

Acmd *n_env_text_061C(N_PVoice *filter, s16 *input, s16 *output,
                      s32 output_count, Acmd *commands)
{
    Acmd *ptr = commands;
    N_PVoice *voice = filter;

    if (voice->em_motion != 1 || !output_count)
        return ptr;

    ptr = n_alResamplePull(voice, input, commands);

    if (voice->em_first) {
        voice->em_first = 0;

        voice->em_ltgt = (voice->em_volume * n_eqpower[voice->em_pan]) >> 15;
        voice->em_lratm = _getRate((double)voice->em_cvolL, (double)voice->em_ltgt,
                                   voice->em_segEnd, &voice->em_lratl);
        voice->em_rtgt = (voice->em_volume *
                          n_eqpower[N_EQPOWER_LENGTH - voice->em_pan - 1]) >> 15;
        voice->em_rratm = _getRate((double)voice->em_cvolR, (double)voice->em_rtgt,
                                   voice->em_segEnd, &voice->em_rratl);

        n_aSetVolume(ptr++, A_RATE, voice->em_ltgt, voice->em_lratm, voice->em_lratl);
        n_aSetVolume(ptr++, A_VOL | A_LEFT, voice->em_cvolL,
                     voice->em_dryamt, voice->em_wetamt);
        n_aSetVolume(ptr++, A_VOL | A_RIGHT, voice->em_rtgt,
                     voice->em_rratm, voice->em_rratl);
        n_aEnvMixer(ptr++, A_INIT, voice->em_cvolR,
                    osVirtualToPhysical(voice->em_state));
    } else {
        n_aEnvMixer(ptr++, A_CONTINUE, 0, osVirtualToPhysical(voice->em_state));
    }

    *input += FIXED_SAMPLE << 1;
    voice->em_delta += FIXED_SAMPLE;

    return ptr;
}

s16 _getRate(double volume, double target, s32 count, u16 *fraction)
{
    s16 integer;
    s16 carry;
    double inverse_count;
    double rate;

    if (count == 0) {
        if (target >= volume) {
            *fraction = 0xFFFF;
            return 0x7FFF;
        } else {
            *fraction = 0;
            return -0x8000;
        }
    }

    inverse_count = 1.0 / count;

    if (target < 1.0)
        target = 1.0;

    if (volume <= 0)
        volume = 1.0;

    rate = (target - volume) * inverse_count * 8.0;
    integer = (s16)rate - 1;
    rate -= (s16)rate;
    rate += 1.0;
    carry = (s16)rate;
    rate -= carry;
    integer += carry;
    *fraction = (u16)(0xFFFF * rate);
    return integer;
}
