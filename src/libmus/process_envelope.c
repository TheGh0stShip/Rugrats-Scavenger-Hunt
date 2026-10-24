#include "mus_channel.h"

void func_8003D360(channel_t *cp)
{
    int v;

    if (cp->f54 - cp->f0C < 0) {
        if (cp->fC3 < 4) {
            cp->fC3 = 4;
            cp->fC5 = 1;
            cp->fCD = cp->fC4;
        }
    }
    if (--cp->fC5 != 0)
        return;
    cp->fC5 = cp->fBF;
    switch (cp->fC3) {
    case 1:
        v = ((unsigned int)(cp->f0C - cp->f40) >> 8) * cp->f64 >> 10;
        if (v < cp->fC6) {
            cp->fC4 = cp->fC0 + (int)(cp->f58 * v);
        } else {
            cp->fC3++;
            cp->fC4 = cp->fC1;
        }
        break;
    case 2:
        v = (((unsigned int)(cp->f0C - cp->f40) >> 8) - cp->fC6) * cp->f64 >> 10;
        if (v < cp->fC7) {
            cp->fC4 = cp->fC1 + (int)(cp->f5C * v);
        } else {
            cp->fC3++;
            cp->fC4 = cp->fC2;
        }
        break;
    case 3:
        break;
    case 4:
        v = ((unsigned int)(cp->f0C - cp->f54) >> 8) * cp->f64 >> 10;
        if (v < cp->fC8) {
            cp->fC4 = cp->fCD - (int)(cp->f60 * v * cp->fCD);
        } else {
            cp->fC3++;
            cp->fC4 = 0;
        }
        break;
    }
}
