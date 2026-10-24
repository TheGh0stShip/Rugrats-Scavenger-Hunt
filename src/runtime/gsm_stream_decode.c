typedef unsigned char u8;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

typedef struct GsmState {
    u8 pad_0000[0x18A];
    s16 frame_count;
    s16 silence_count;
    u8 pad_018E[0x1A0 - 0x18E];
    u32 words[0x400];
    u8 pad_11A0[0x11C8 - 0x11A0];
    s32 bit_position;
} GsmState;

extern void func_80051050(void *dst, s32 size);
extern void func_80044468(GsmState *state, s16 *lar, s16 *nc, s16 *bc,
                         s16 *mc, s16 *xmax, s16 *xmc, s16 *samples);

#define READ_BITS(dst, width)                                                \
    do {                                                                     \
        if (bits_left > (width)) {                                           \
            (dst) = current_word & ((1 << (width)) - 1);                     \
            current_word >>= (width);                                        \
            bits_left -= (width);                                            \
        } else {                                                             \
            (dst) = current_word & ((1 << bits_left) - 1);                   \
            bit_position += 32;                                              \
            current_word = state->words[(bit_position >> 5) & 0x3FF];        \
            if (bits_left != (width)) {                                      \
                (dst) |= (current_word &                                     \
                          ((1 << ((width) - bits_left)) - 1)) << bits_left;   \
                current_word >>= (width) - bits_left;                        \
            }                                                                \
            bits_left += 32 - (width);                                       \
        }                                                                    \
    } while (0)

s32 func_800447E0(GsmState *state, s16 *samples) {
    s16 lar[8];
    s16 nc[4];
    s16 mc[4];
    s16 bc[4];
    s16 xmax[4];
    s16 xmc[52];
    s32 bit_position;
    s32 bits_left;
    u32 current_word;
    s32 header;
    s32 i;
    s16 *x;

    bit_position = state->bit_position;
    current_word = state->words[(bit_position >> 5) & 0x3FF] >>
                   (bit_position & 31);
    bits_left = 32 - (bit_position & 31);

    if (state->frame_count == 0 && state->silence_count == 0) {
        READ_BITS(header, 1);
        if (header != 0) {
            READ_BITS(state->silence_count, 4);
            state->silence_count++;
        } else {
            READ_BITS(state->frame_count, 7);
            state->frame_count++;
        }
    }

    if (state->silence_count != 0) {
        func_80051050(samples, 160 * sizeof(*samples));
        state->silence_count--;
    } else {
        READ_BITS(lar[0], 6);
        READ_BITS(lar[1], 6);
        READ_BITS(lar[2], 5);
        READ_BITS(lar[3], 5);
        READ_BITS(lar[4], 4);
        READ_BITS(lar[5], 4);
        READ_BITS(lar[6], 3);
        READ_BITS(lar[7], 3);

        x = xmc;
        for (i = 0; i < 4; i++) {
            READ_BITS(nc[i], 7);
            READ_BITS(bc[i], 2);
            READ_BITS(mc[i], 2);
            READ_BITS(xmax[i], 6);
            READ_BITS(x[0], 3);
            READ_BITS(x[1], 3);
            READ_BITS(x[2], 3);
            READ_BITS(x[3], 3);
            READ_BITS(x[4], 3);
            READ_BITS(x[5], 3);
            READ_BITS(x[6], 3);
            READ_BITS(x[7], 3);
            READ_BITS(x[8], 3);
            READ_BITS(x[9], 3);
            READ_BITS(x[10], 3);
            READ_BITS(x[11], 3);
            READ_BITS(x[12], 3);
            x += 13;
        }

        state->frame_count--;
        func_80044468(state, lar, nc, bc, mc, xmax, xmc, samples);
    }

    state->bit_position = (bit_position & -32) - bits_left + 32;
    return 0;
}
