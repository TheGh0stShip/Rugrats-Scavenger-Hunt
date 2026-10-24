#include "mus_channel.h"
extern int D_800C3FD4;
extern channel_t *D_800C3FDC;
extern int func_8003E068(void *bank, int number, int volume, int pan, int priority);

unsigned char *func_8003ECD8(channel_t *cp, unsigned char *p)
{
    int n, handle, i, pr;
    channel_t *c;
    n = *p++;
    if (n >= 0x80) n = ((n & 0x7F) << 8) + *p++;
    pr = ++cp->f48;
    handle = func_8003E068(cp->sample_bank, n, cp->temscale, cp->pan_scale, pr);
    cp->f48--;
    if (handle) {
        for (i = 0, c = D_800C3FDC; i < D_800C3FD4; i++, c++) {
            if (c->handle == handle) { c->handle = cp->handle; c->f7C = cp->f7C; }
        }
    }
    return p;
}
