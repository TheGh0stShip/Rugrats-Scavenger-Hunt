#include "../libmus/mus_channel.h"

void func_8003D7A8(channel_t *cp)
{
    unsigned char c;
    int n;

    do {
        cp->f18 += 0x100;
        if (--cp->fA4 == 0) {
            c = *cp->f34++;
            if (c >= 0x80) {
                cp->f70 = (float)(c & 0x7F) - 64.0;
                cp->f24 = cp->f70 * cp->f6C;
                c = *cp->f34++;
                if (c >= 0x80) {
                    n = (c & 0x7F) << 8;
                    cp->fA4 = n;
                    cp->fA4 = *cp->f34++ + (n + 2);
                } else {
                    cp->fA4 = c + 2;
                }
            } else {
                cp->f70 = (float)c - 64.0;
                cp->f24 = cp->f70 * cp->f6C;
                cp->fA4 = 1;
            }
        }
    } while (cp->f18 - cp->f0C < 0);
}
