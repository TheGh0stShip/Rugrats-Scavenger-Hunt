#include "mus_channel.h"
extern int D_800C3FE4;
extern int D_800C3FD4;
extern channel_t *D_800C3FDC;

unsigned char *func_8003E674(channel_t *cp, unsigned char *p)
{
    int tempo;
    int scaled;
    int i;
    channel_t *c;
    tempo = (*p++ * 0x6000 / 120) / D_800C3FE4;
    scaled = (tempo * cp->volume_scale) >> 7;
    if (cp->sample_bank) cp->volume = tempo;
    else {
        for (i = 0, c = D_800C3FDC; i < D_800C3FD4; i++, c++) {
            if (c->song_bank == cp->song_bank) { c->base_volume = tempo; c->volume = scaled; }
        }
    }
    return p;
}
