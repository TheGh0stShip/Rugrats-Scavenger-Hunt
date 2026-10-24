#include "mus_channel.h"

extern int D_800C3FD4;
extern channel_t *D_800C3FDC;

int func_8003BF84(int handle, int speed)
{
    int i;
    int effective_speed;
    int count;
    channel_t *channel;

    if (!handle) {
        return 0;
    }

    effective_speed = speed ? speed : 1;
    for (i = 0, channel = D_800C3FDC, count = 0;
         i < D_800C3FD4; i++, channel++) {
        if (channel->handle == handle && channel->stopping == -1) {
            if (channel->flags & 1) {
                channel->stopping_speed = 1;
                channel->stopping = 0;
                channel->flags &= ~1;
            } else {
                channel->stopping_speed = effective_speed;
                channel->stopping = speed;
            }
            count++;
        }
    }
    return count;
}
