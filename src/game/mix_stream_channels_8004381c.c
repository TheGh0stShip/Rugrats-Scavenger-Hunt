typedef struct {
    unsigned int w0;
    unsigned int w1;
} Acmd;

typedef struct {
    unsigned char pad0000[0x11D4];
    unsigned int base;
    unsigned char pad11D8[5];
    unsigned char state;
    unsigned char pad11DE[2];
    unsigned int buffer;
    void *resample_state;
    unsigned short id;
    unsigned char first;
    unsigned char pad11EB;
    float fraction;
    float pitch;
    unsigned int consumed;
    unsigned short left;
    unsigned short right;
    unsigned char pad11FC[4];
} StreamChannel;

extern StreamChannel *D_800C4300;
extern unsigned short D_800C4304;
extern void func_800441CC(StreamChannel *channel, unsigned int amount);
extern unsigned int func_800495D0(void *address);

#define WRITE_COMMAND(p, first, second) { \
    Acmd *command = (p); \
    command->w0 = (first); \
    command->w1 = (second); \
}

#define WRITE_FIRST_WORD(p, first) { \
    Acmd *command = (p); \
    command->w0 = (first); \
}

Acmd *func_8004381C(Acmd *cmdList, int *cmdLen, short *outBuf, int outLen)
{
    Acmd *ptr;
    int i;
    int active;
    int n;
    int samples;
    unsigned int pos;
    int start;
    int end;
    unsigned int addr;
    int align;
    int len;
    unsigned int pitch;
    float fraction;

    ptr = cmdList;
    active = 0;
    for (i = 0; i < D_800C4304; i++) {
        if (D_800C4300[i].id != 0xFFFF && D_800C4300[i].state == 3) {
            active++;
            if (D_800C4300[i].consumed != 0) {
                func_800441CC(&D_800C4300[i], D_800C4300[i].consumed);
                D_800C4300[i].consumed = 0;
            }
        }
    }
    if (active == 0) {
        return cmdList;
    }

    while (outLen > 0) {
        n = outLen;
        if (outLen > 184) {
            n = 184;
        }
        WRITE_COMMAND(ptr++, 0x020004E0, 0x2E0);
        for (i = 0; i < D_800C4304; i++) {
            if (D_800C4300[i].id == 0xFFFF || D_800C4300[i].state != 3) {
                continue;
            }
            pos = D_800C4300[i].base + D_800C4300[i].consumed;
            fraction = D_800C4300[i].fraction + D_800C4300[i].pitch * n;
            samples = fraction;
            D_800C4300[i].fraction = fraction - samples;
            start = pos % 0xA00;
            end = start + samples;
            if (end > 0xA00) {
                addr = D_800C4300[i].buffer + start * 2 - 0x80000000;
                align = addr & 7;
                addr -= align;
                len = (0xA00 - start) * 2 + align + 7;
                WRITE_COMMAND(ptr++, ((len & 0xFF8) << 12) | 0x04000170, addr);
                WRITE_FIRST_WORD(ptr++,
                                 ((((end - 0xA00) * 2 + 0xF) & 0xFF8) << 12) |
                                     (((len & ~7) + 0x170) & 0xFF8) | 0x04000000);
                addr = D_800C4300[i].buffer - 0x80000000;
            } else {
                addr = D_800C4300[i].buffer + start * 2 - 0x80000000;
                align = addr & 7;
                addr -= align;
                WRITE_FIRST_WORD(ptr++,
                                 (((samples * 2 + align + 0xF) & 0xFF8) << 12) |
                                     0x04000170);
            }
            ptr[-1].w1 = addr;
            pitch = D_800C4300[i].pitch * 32768.0f;
            WRITE_COMMAND(ptr++,
                          (func_800495D0(D_800C4300[i].resample_state) & 0xFFFFFF) |
                              0x05000000,
                          (D_800C4300[i].first << 30) | ((pitch & 0xFFFF) << 14) |
                              ((align | 0x170) << 2));
            D_800C4300[i].first = 0;
            WRITE_COMMAND(ptr++, D_800C4300[i].left | 0x0C000000, 0x4E0);
            WRITE_COMMAND(ptr++, D_800C4300[i].right | 0x0C000000, 0x650);
            D_800C4300[i].consumed += samples;
        }
        WRITE_FIRST_WORD(ptr++, 0x0D000000);
        WRITE_COMMAND(ptr++, 0x042E04E0, (unsigned int)outBuf);
        WRITE_COMMAND(ptr++, 0x0C007FFF, 0x04E00000);
        WRITE_COMMAND(ptr++, 0x0C007FFF, 0x06500170);
        WRITE_COMMAND(ptr++, 0x062E0000, (unsigned int)outBuf);
        outBuf += n * 2;
        outLen -= n;
    }
    *cmdLen += ptr - cmdList;
    return ptr;
}
