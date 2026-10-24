#include "mus_channel.h"
typedef struct { unsigned char pad[0x1C]; } voice_t;
typedef struct { int a, b; } mus_message_t;
extern int D_800C3FD4;
extern voice_t *D_800C3FD8;
extern channel_t *D_800C3FDC;
extern int D_800C3FE8;
extern int D_800C4014;
extern int D_800C4018;
extern int D_800C401C;
extern mus_message_t *D_800C4020;
extern void func_8003C6C4(mus_message_t *msg);
extern void func_8003CACC(channel_t *cp, int x);
extern void func_8003D014(channel_t *cp, int x);
extern void func_8003D090(channel_t *cp, int x);
extern void func_8003D1E4(channel_t *cp, int x, float offset);
extern void func_8003D360(channel_t *cp);
extern void func_8003D570(channel_t *cp);
extern float func_8003D628(channel_t *cp);
extern float func_8003D680(channel_t *cp);
extern void func_8003D6E4(channel_t *cp);
extern void func_8003D7A8(channel_t *cp);
extern void func_80040750(voice_t *voice);

int func_8003C7DC(void)
{
    int x;
    channel_t *cp;
    float offset;

    while (D_800C4014 != D_800C4018) {
        func_8003C6C4(&D_800C4020[D_800C4014]);
        if (++D_800C4014 == D_800C401C)
            D_800C4014 = 0;
    }
    for (x = -4, cp = D_800C3FDC; x < D_800C3FD4 - 4; x++, cp++) {
        if (cp->pdata == 0)
            continue;
        if (cp->flags & 1)
            continue;
        if (cp->f08)
            func_8003D014(cp, x);
        cp->f0C += cp->volume;
        if (cp->f9A != 0x7FFF) {
            while (cp->f3C - cp->f0C < 0) {
                if (cp->pdata == 0)
                    goto next;
                func_8003CACC(cp, x);
            }
            if (cp->pdata == 0)
                continue;
        }
        if (cp->f38 && cp->f14 - cp->f0C < 0)
            func_8003D6E4(cp);
        if (cp->f34 && cp->f18 - cp->f0C < 0)
            func_8003D7A8(cp);
        if (cp->stopping != -1 && --cp->stopping == -1) {
            cp->f38 = 0;
            cp->f34 = 0;
            cp->song_bank = 0;
            cp->sample_bank = 0;
            cp->handle = 0;
            cp->f08 = 0;
            cp->pdata = 0;
            if (cp->fC9) {
                cp->fC9 = 0;
                func_80040750(&D_800C3FD8[x]);
            }
        }
        if (cp->fC9) {
            if (cp->fC3)
                func_8003D360(cp);
            if (cp->fD4 && cp->f94 - cp->f0C < 0)
                func_8003D570(cp);
            offset = cp->freqoffset;
            if (cp->fD5)
                offset += func_8003D680(cp);
            if (cp->fCE)
                offset += func_8003D628(cp);
            if (cp->f08 == 0) {
                func_8003D1E4(cp, x, offset);
                func_8003D090(cp, x);
            }
        }
        cp->fAA = (unsigned int)(cp->f0C - cp->f40) >> 8;
    next:;
    }
    return D_800C3FE8;
}
