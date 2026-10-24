typedef short word;
typedef unsigned int u32;

struct mort_state {
    unsigned char pad000[0x18A];
    word speech_run;
    word silence_run;
    unsigned char pad18E[0x12];
    u32 data[0x400];
    unsigned char pad11A0[0x28];
    int bit_position;
};

extern void func_80051050(void *address, int length);
extern void func_80044468(struct mort_state *S, word *LARcr, word *Ncr, word *bcr, word *Mcr,
                          word *xmaxcr, word *xMcr, word *s);

#define GETBITS(dst, n)                                             \
    if (avail > (n)) {                                              \
        dst = bits & ((1 << (n)) - 1);                              \
        bits >>= (n);                                               \
        avail -= (n);                                               \
    } else {                                                        \
        dst = bits & ((1 << avail) - 1);                            \
        pos += 32;                                                  \
        bits = S->data[(pos >> 5) & 0x3FF];                         \
        if (avail != (n)) {                                         \
            dst |= (bits & ((1 << ((n) - avail)) - 1)) << avail;    \
            bits >>= (n) - avail;                                   \
        }                                                           \
        avail += 32 - (n);                                          \
    }

int func_800447E0(struct mort_state *S, word *target)
{
    word LARc[8], Nc[4], Mc[4], bc[4], xmaxc[4], xmc[13 * 4];
    u32 bits;
    int avail;
    int pos;
    int flag;
    int i;

    pos = S->bit_position;
    bits = S->data[(pos >> 5) & 0x3FF] >> (pos & 0x1F);
    avail = 32 - (pos & 0x1F);
    if (S->speech_run == 0 && S->silence_run == 0) {
        GETBITS(flag, 1);
        if (flag) {
            GETBITS(S->silence_run, 4);
            S->silence_run++;
        } else {
            GETBITS(S->speech_run, 7);
            S->speech_run++;
        }
    }
    if (S->silence_run) {
        func_80051050(target, 0x140);
        S->silence_run--;
    } else {
        GETBITS(LARc[0], 6);
        GETBITS(LARc[1], 6);
        GETBITS(LARc[2], 5);
        GETBITS(LARc[3], 5);
        GETBITS(LARc[4], 4);
        GETBITS(LARc[5], 4);
        GETBITS(LARc[6], 3);
        GETBITS(LARc[7], 3);
        for (i = 0; i < 4; i++) {
            GETBITS(Nc[i], 7);
            GETBITS(bc[i], 2);
            GETBITS(Mc[i], 2);
            GETBITS(xmaxc[i], 6);
            GETBITS(xmc[i * 13 + 0], 3);
            GETBITS(xmc[i * 13 + 1], 3);
            GETBITS(xmc[i * 13 + 2], 3);
            GETBITS(xmc[i * 13 + 3], 3);
            GETBITS(xmc[i * 13 + 4], 3);
            GETBITS(xmc[i * 13 + 5], 3);
            GETBITS(xmc[i * 13 + 6], 3);
            GETBITS(xmc[i * 13 + 7], 3);
            GETBITS(xmc[i * 13 + 8], 3);
            GETBITS(xmc[i * 13 + 9], 3);
            GETBITS(xmc[i * 13 + 10], 3);
            GETBITS(xmc[i * 13 + 11], 3);
            GETBITS(xmc[i * 13 + 12], 3);
        }
        S->speech_run--;
        func_80044468(S, LARc, Nc, bc, Mc, xmaxc, xmc, target);
    }
    S->bit_position = (pos & ~0x1F) - avail + 32;
    return 0;
}
