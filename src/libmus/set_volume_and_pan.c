#include "mus_channel.h"

typedef unsigned int u32;

typedef struct {
    unsigned char pad[0x1C];
} Voice;

extern Voice *D_800C3FD8;
extern unsigned short D_800C3FEC;
extern unsigned short D_800C3FEE;
extern int D_800C3FE8;
extern void func_80040610(Voice *, short, int);
extern void func_800404F0(Voice *, int);

void func_8003D090(channel_t *channel, int index)
{
    u32 volume;

    volume = ((u32)channel->fBC * (u32)channel->fC4 *
              (u32)channel->fBB * (u32)(short)channel->temscale) >> 13;
    if (volume > 32767) {
        volume = 32767;
    }

    if (!channel->sample_bank) {
        volume *= D_800C3FEE;
    } else {
        volume *= D_800C3FEC;
    }
    volume >>= 15;
    if (channel->stopping != -1) {
        volume = (volume * channel->stopping) / channel->stopping_speed;
    }

    if (volume != channel->fA0) {
        channel->fA0 = volume;
        func_80040610(&D_800C3FD8[index], volume, D_800C3FE8);
    }

    volume = ((channel->fBD * (short)channel->pan_scale) >> 7) & 0x7F;
    if (volume != channel->pan_changed) {
        channel->pan_changed = volume;
        func_800404F0(&D_800C3FD8[index], volume);
    }
}
