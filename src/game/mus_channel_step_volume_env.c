#include "../libmus/mus_channel.h"

void func_8003D6E4(channel_t *cp)
{
    unsigned char c;
    int n;

    do {
        cp->f14 += 0x100;
        if (--cp->fA2 == 0) {
            c = *cp->f38++;
            if (c >= 0x80) {
                cp->fBC = c & 0x7F;
                c = *cp->f38++;
                if (c >= 0x80) {
                    n = (c & 0x7F) << 8;
                    cp->fA2 = n;
                    cp->fA2 = *cp->f38++ + (n + 2);
                } else {
                    cp->fA2 = c + 2;
                }
            } else {
                cp->fBC = c;
                cp->fA2 = 1;
            }
        }
    } while (cp->f14 - cp->f0C < 0);
}
