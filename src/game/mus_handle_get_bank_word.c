#include "../libmus/mus_channel.h"
extern int D_800C3FD4;
extern channel_t *D_800C3FDC;

int func_8003C578(int handle)
{
    channel_t *cp;
    int i;

    if (handle == 0)
        return 0;
    for (i = 0, cp = D_800C3FDC; i < D_800C3FD4; i++, cp++) {
        if (cp->handle == handle) {
            if (cp->song_bank)
                return ((int *)cp->song_bank)[2];
            return ((int *)cp->sample_bank)[2];
        }
    }
    return 0;
}
