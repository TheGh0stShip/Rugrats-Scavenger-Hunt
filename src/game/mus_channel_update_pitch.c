#include "../libmus/mus_channel.h"

typedef struct {
    int pad[7];
} voice_t;

extern voice_t *D_800C3FD8;
extern float func_8003D8BC(float x);
extern void func_80040580(voice_t *v, float pitch);

void func_8003D1E4(channel_t *cp, int voice, float offset)
{
    float freq;

    freq = cp->f2C;
    if (cp->fB8) {
        if (cp->fB8 >= cp->fAA)
            freq = cp->f4C + (freq - cp->f4C) / cp->fB8 * cp->fAA;
        cp->f50 = freq;
    }
    freq += offset + cp->f24;
    if (freq != cp->f28) {
        cp->f28 = freq;
        freq = func_8003D8BC(freq * (1.0 / 12.0));
        if (freq > 2.0) {
            freq = 2.0;
            cp->fBB = 0;
        }
        func_80040580(&D_800C3FD8[voice], freq);
    }
}
