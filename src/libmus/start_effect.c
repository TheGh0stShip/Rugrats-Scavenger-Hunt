#include "mus_channel.h"
typedef struct { unsigned char *fxdata; int priority; } fx_t;
typedef struct { int pad00[4]; void *wave_table; int pad14; fx_t effects[1]; } fx_header_t;
extern int D_800C3FD4;
extern channel_t *D_800C3FE0;
extern int D_800C3FF0;
extern void func_8003DC7C(channel_t *cp);

int func_8003E068(fx_header_t *ptr, int number, int volume, int pan, int priority)
{
    channel_t *cp, *cp2;
    int i, low;

    if (priority == -1)
        priority = ptr->effects[number].priority;
    low = priority + 1;
    for (i = 4, cp = D_800C3FE0; i < D_800C3FD4; i++, cp++) {
        if (cp->pdata == 0) {
            func_8003DC7C(cp);
            cp->fx_number = number;
            cp->sample_bank = ptr;
            cp->temscale = volume;
            cp->pan_scale = pan;
            cp->f48 = priority;
            cp->handle = D_800C3FF0++;
            if (ptr->wave_table)
                cp->f7C = ptr->wave_table;
            cp->pdata = cp->f80 = ptr->effects[number].fxdata;
            return cp->handle;
        }
        if (cp->sample_bank) {
            if (cp->f48 < low) {
                low = cp->f48;
                cp2 = cp;
            }
        }
    }
    if (low >= priority)
        return 0;
    func_8003DC7C(cp2);
    cp2->fx_number = number;
    cp2->sample_bank = ptr;
    cp2->temscale = volume;
    cp2->pan_scale = pan;
    cp2->f48 = priority;
    cp2->handle = D_800C3FF0++;
    if (ptr->wave_table)
        cp2->f7C = ptr->wave_table;
    cp2->pdata = cp2->f80 = ptr->effects[number].fxdata;
    return cp2->handle;
}
