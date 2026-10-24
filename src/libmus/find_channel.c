#include "mus_channel.h"
typedef struct song_s { int pad00; int count; int pad08; unsigned char **fC; } song_t;
extern int D_800C3FD4;
extern channel_t *D_800C3FDC;
extern channel_t *D_800C3FE0;

int func_8003DDB8(song_t *song, int index)
{
    int i, best, lowest;
    channel_t *cp;

    if (index < 0) {
        for (i = 0, cp = D_800C3FDC; i < 4; i++, cp++) {
            if (cp->pdata == 0)
                return i;
        }
    }
    for (i = 4, cp = D_800C3FE0; i < D_800C3FD4; i++, cp++) {
        if (cp->pdata == 0)
            return i;
    }
    i = 4;
    cp = D_800C3FE0;
    lowest = 0x7FFFFFFF;
    best = 3;
    for (; i < D_800C3FD4; i++, cp++) {
        if (cp->sample_bank != 0 && cp->f48 <= lowest) {
            lowest = cp->f48;
            best = i;
        }
    }
    if (best >= 4)
        return best;
    for (i = 4, cp = D_800C3FE0; i < D_800C3FD4; i++) {
        if (cp->sample_bank == 0 && cp->song_bank != song)
            return i;
    }
    for (i = 4, cp = D_800C3FE0; i < D_800C3FD4; i++, cp++) {
        if (cp->song_bank == song && song->fC[index] == cp->f80)
            return i;
    }
    return index % (D_800C3FD4 - 4) + 4;
}
