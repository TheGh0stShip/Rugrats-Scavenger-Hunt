#include "mus_channel.h"
typedef struct song_s { int pad00; int count; int pad08; unsigned char **fC; unsigned char **f10; unsigned char **f14; int pad18[3]; unsigned char *f24; int f28; } song_t;
extern channel_t *D_800C3FDC;
extern int D_800C3FF0;
extern int D_800C3FF8;
extern int func_8003DDB8(song_t *song, int index);
extern void func_8003DC7C(channel_t *cp);

static __inline__ void relocate(void *address, void *offset, int count)
{
    unsigned long *p = address;
    unsigned long base = (unsigned long)offset;
    int i;
    for (i = 0; i < count; i++) if (p[i]) p[i] += base;
}

int func_8003E210(void *address)
{
    song_t *song = address;
    int i;
    int count = song->count;
    channel_t *cp;
    unsigned long handle;
    if (!song->f28 & 1) {
        song->f28 |= 1;
        relocate(&song->fC, address, 7);
        relocate(song->fC, address, count);
        relocate(song->f10, address, count);
        relocate(song->f14, address, count);
    }
    handle = D_800C3FF0++;
    cp = &D_800C3FDC[func_8003DDB8(song, -1)];
    func_8003DC7C(cp);
    cp->fD2 = 1; cp->flags |= 3; cp->song_bank = song; cp->pdata = cp->f80 = song->f24; cp->handle = handle;
    for (i = 0; i < count; i++) {
        if (song->fC[i]) {
            cp = &D_800C3FDC[func_8003DDB8(song, i)];
            func_8003DC7C(cp);
            cp->fD2 = 1; cp->flags |= 1; cp->song_bank = song;
            cp->f38 = cp->f8C = song->f10[i]; cp->f34 = cp->f88 = song->f14[i]; cp->pdata = cp->f80 = song->fC[i]; cp->handle = handle;
        }
    }
    D_800C3FF8 = 0;
    return handle;
}
