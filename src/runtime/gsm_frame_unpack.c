typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef struct GsmState GsmState;

extern void func_80044468(GsmState *, s16 *, s16 *, s16 *, s16 *, s16 *, s16 *, s16 *);

s32 func_80044550(GsmState *state, u8 *c, s16 *samples) {
    s16 lar[8], nc[4], mc[4], bc[4], xmax[4], xmc[52];
    s32 i;
    u8 *p = c;
    s16 *ncp;

    lar[0] = ((c[0] & 0xF) << 2) | ((c[1] >> 6) & 3);
    lar[1] = c[1] & 0x3F;
    lar[2] = (c[2] >> 3) & 0x1F;
    lar[3] = ((c[2] & 7) << 2) | ((c[3] >> 6) & 3);
    lar[4] = (c[3] >> 2) & 0xF;
    lar[5] = ((c[3] & 3) << 2) | ((c[4] >> 6) & 3);
    lar[6] = (c[4] >> 3) & 7;
    lar[7] = c[4] & 7;

    for (i = 0, ncp = nc; i < 4; ncp++, p += 7, i++) {
        *ncp = (p[5] >> 1) & 0x7F;
        bc[i] = ((p[5] & 1) << 1) | ((p[6] >> 7) & 1);
        mc[i] = (p[6] >> 5) & 3;
        xmax[i] = ((p[6] & 0x1F) << 1) | ((p[7] >> 7) & 1);
        xmc[i * 13 + 0] = (p[7] >> 4) & 7;
        xmc[i * 13 + 1] = (p[7] >> 1) & 7;
        xmc[i * 13 + 2] = ((p[7] & 1) << 2) | ((p[8] >> 6) & 3);
        xmc[i * 13 + 3] = (p[8] >> 3) & 7;
        xmc[i * 13 + 4] = p[8] & 7;
        xmc[i * 13 + 5] = (p[9] >> 5) & 7;
        xmc[i * 13 + 6] = (p[9] >> 2) & 7;
        xmc[i * 13 + 7] = ((p[9] & 3) << 1) | ((p[10] >> 7) & 1);
        xmc[i * 13 + 8] = (p[10] >> 4) & 7;
        xmc[i * 13 + 9] = (p[10] >> 1) & 7;
        xmc[i * 13 + 10] = ((p[10] & 1) << 2) | ((p[11] >> 6) & 3);
        xmc[i * 13 + 11] = (p[11] >> 3) & 7;
        xmc[i * 13 + 12] = p[11] & 7;
    }

    func_80044468(state, lar, nc, bc, mc, xmax, xmc, samples);
    return 0;
}
