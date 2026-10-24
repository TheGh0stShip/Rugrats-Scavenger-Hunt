#include "mus_channel.h"

unsigned char *func_8003E9E0(channel_t *cp, unsigned char *p)
{
    int idx;
    idx = cp->fDB - 1;
    if (cp->f120[idx] != 0xFF) {
        if (--cp->f120[idx] == 0) {
            cp->fDB = idx;
            idx = -1;
        }
    }
    if (idx >= 0) {
        p = cp->fE0[idx]; cp->f38 = cp->fF0[idx]; cp->f34 = cp->f100[idx]; cp->fBC = cp->f124[idx]; cp->f70 = cp->f128[idx]; cp->fA2 = cp->f110[idx]; cp->fA4 = cp->f118[idx]; cp->f24 = cp->f70 * cp->f6C;
    }
    return p;
}
