#include "mus_channel.h"
typedef unsigned char *(*mus_command_t)(channel_t *, unsigned char *);
extern mus_command_t D_80055A30[];
extern int D_800C3FD4;
extern channel_t *D_800C3FDC;
extern int func_8003E210(void *song);
extern int func_8003C3F8(int handle);
extern void func_8003D6E4(channel_t *cp);
extern void func_8003D7A8(channel_t *cp);

int func_8003BA50(void *song, int marker)
{
    channel_t *cp;
    int i;
    int handle = func_8003E210(song);
    for (i = 0, cp = D_800C3FDC; i < D_800C3FD4; i++, cp++) {
        unsigned char *p;
        unsigned char c;
        if (cp->handle != handle || cp->song_bank != song || cp->pdata == 0) continue;
        while (cp->pdata != 0) {
            p = cp->pdata; c = *p;
            if (c >= 0x80) {
                if (c == 0xAB && p[1] == marker) break;
                cp->pdata = (*(mus_command_t *)((unsigned char *)D_80055A30 + ((c & 0x7F) << 2)))(cp, p + 1);
                continue;
            }
            cp->pdata = p + 1;
            if (cp->fD2) {
                cp->pdata = p + 2; cp->fBB = p[1];
                if (cp->fBB >= 0x80) { cp->fBB &= 0x7F; cp->fD2 = 0; cp->fD3 = cp->fBB; }
            } else cp->fBB = cp->fD3;
            if (cp->fAC == 0 || cp->fB7 != 0) {
                unsigned char *q = cp->pdata;
                cp->fB7 = 0; cp->pdata = q + 1; c = *q;
                if (c < 0x80) cp->f9A = c;
                else { cp->pdata = q + 2; cp->f9A = ((c & 0x7F) << 8) + q[1]; }
            } else cp->f9A = cp->fAC;
            cp->f0C += cp->f9A << 8;
        }
        cp->f3C = cp->f0C;
        {
            int duration; unsigned char low; unsigned char *q = cp->pdata;
            if (q != 0) {
                duration = q[2]; p = q + 3;
                if (duration >= 0x80) { duration &= 0x7F; low = q[3]; duration <<= 8; p++; duration |= low; }
                cp->fAA = 0; cp->f9A = duration; cp->pdata = p; cp->f0C -= duration << 8;
            }
        }
        cp->f40 = cp->f0C;
        if (cp->f38 != 0) func_8003D6E4(cp);
        if (cp->f34 != 0) func_8003D7A8(cp);
    }
    func_8003C3F8(handle);
    return handle;
}
