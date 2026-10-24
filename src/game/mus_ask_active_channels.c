#include "../libmus/mus_channel.h"
extern int D_800C3FD4;
extern channel_t *D_800C3FDC;

int func_8003BF04(int flags)
{
    channel_t *cp;
    int i, count;

    count = 0;
    for (i = 0, cp = D_800C3FDC; i < D_800C3FD4; i++, cp++) {
        if (cp->pdata) {
            if (cp->sample_bank) {
                if (flags & 1)
                    goto inc;
            } else if (flags & 2) {
            inc:
                count++;
            }
        }
    }
    return count;
}
