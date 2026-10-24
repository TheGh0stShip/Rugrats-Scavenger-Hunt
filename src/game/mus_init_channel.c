#include "../libmus/mus_channel.h"

extern int D_800C3FE4;
extern void *D_800C3FF8;
extern void *D_800C3FFC;

void func_8003DC7C(channel_t *cp)
{
    unsigned char old;
    unsigned char *p;
    unsigned int i;
    unsigned short vol;

    old = cp->fC9;
    cp->pdata = 0;
    p = (unsigned char *)cp;
    for (i = 0; i < sizeof(channel_t); i++)
        *p++ = 0;
    cp->pan_dirty = 0xFF;
    cp->fA0 = 0xFFFF;
    cp->pan_changed = 0xFF;
    vol = 0x6000 / D_800C3FE4;
    cp->fBD = 0x40;
    cp->f9A = 1;
    cp->fD3 = 0x7F;
    cp->fBF = 1;
    cp->fA2 = 1;
    cp->fBC = 0x7F;
    cp->fC1 = 0x7F;
    cp->f28 = 99.9f;
    cp->fA4 = 1;
    cp->f6C = 1.0f / 32.0f;
    cp->stopping = -1;
    cp->fC6 = 1;
    cp->base_volume = vol;
    cp->temscale = 0x80;
    cp->fC8 = 0xF;
    cp->f58 = 1.0f;
    cp->pan_scale = 0x80;
    cp->f5C = 1.0f / 255.0f;
    cp->fC7 = 0xFF;
    cp->f60 = 1.0f / 15.0f;
    cp->volume_scale = 0x80;
    cp->volume = vol;
    cp->fC2 = 0x7F;
    cp->f7C = D_800C3FF8 ? D_800C3FF8 : D_800C3FFC;
    cp->fC9 = old;
}
