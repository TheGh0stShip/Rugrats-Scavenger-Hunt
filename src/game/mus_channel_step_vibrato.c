#include "../libmus/mus_channel.h"

void func_8003D570(channel_t *cp)
{
    unsigned int speed;
    unsigned int v;

    speed = cp->fD4;
    do {
        cp->f94 += 0x100;
        v = cp->fD9 + speed;
        if (v < 0x40) {
            cp->fD9 = v;
        } else {
            cp->fD9 = v & 0x3F;
            v >>= 6;
            if (cp->fDA == 0) {
                cp->fBD += v;
                if (cp->fBD >= 0x80) {
                    cp->fBD = 0x7F;
                    cp->fDA = 1;
                }
            } else {
                cp->fBD -= v;
                if (cp->fBD >= 0x80 || cp->fBD == 0) {
                    cp->fBD = 0;
                    cp->fDA = 0;
                }
            }
        }
    } while (cp->f94 - cp->f0C < 0);
}
