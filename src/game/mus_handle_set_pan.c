#include "../libmus/mus_channel.h"
extern int D_800C3FD4;
extern channel_t *D_800C3FDC;

int func_8003C0F0(int handle, int pan)
{
    channel_t *cp;
    int i, count;

    if (handle == 0)
        return 0;
    count = 0;
    for (i = 0, cp = D_800C3FDC; i < D_800C3FD4; i++, cp++) {
        if (cp->handle == handle) {
            count++;
            cp->pan_scale = pan;
            cp->pan_changed = 0xFF;
        }
    }
    return count;
}
