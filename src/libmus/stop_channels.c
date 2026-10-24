#include "mus_channel.h"

typedef unsigned int u32;

extern int D_800C3FD4;
extern channel_t *D_800C3FDC;

void func_8003BE38(u32 flags, int speed)
{
    int i;
    int effective_speed;
    channel_t *channel;

    effective_speed = speed ? speed : 1;
    for (i = 0, channel = D_800C3FDC; i < D_800C3FD4; i++, channel++) {
        if ((channel->sample_bank && (flags & 1)) ||
            (!channel->sample_bank && (flags & 2))) {
            if (channel->pdata && channel->stopping == -1) {
                if (channel->flags & 1) {
                    channel->stopping_speed = 1;
                    channel->stopping = 0;
                    channel->flags &= ~1;
                } else {
                    channel->stopping_speed = effective_speed;
                    channel->stopping = speed;
                }
            }
        }
    }
}
