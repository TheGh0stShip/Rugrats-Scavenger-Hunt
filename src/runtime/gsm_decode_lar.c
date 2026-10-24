typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct GsmState {
    u8 pad_000[0x168];
    s16 larpp[2][8];
    s16 phase;
} GsmState;

extern void func_80045A54(GsmState *state, s16 *samples, s16 *output);

void func_80044270(GsmState *state, s16 *coded, s16 *samples, s16 *output) {
    s16 *decoded;

    decoded = state->larpp[state->phase];

    decoded[0] = ((((s32)(coded[0] - 32) << 10) * 13107 + 16384) >> 15) << 1;
    decoded[1] = ((((s32)(coded[1] - 32) << 10) * 13107 + 16384) >> 15) << 1;
    decoded[2] = ((((s32)(coded[2] - 16) << 10) * 13107 - 4096 * 13107 + 16384) >> 15) << 1;
    decoded[3] = ((((s32)(coded[3] - 16) << 10) * 13107 + 5120 * 13107 + 16384) >> 15) << 1;
    decoded[4] = ((((s32)(coded[4] - 8) << 10) * 19223 - 188 * 19223 + 16384) >> 15) << 1;
    decoded[5] = ((((s32)(coded[5] - 8) << 10) * 17476 + 3584 * 17476 + 16384) >> 15) << 1;
    decoded[6] = ((((s32)(coded[6] - 4) << 10) * 31454 + 682 * 31454 + 16384) >> 15) << 1;
    decoded[7] = ((((s32)(coded[7] - 4) << 10) * 29708 + 2288 * 29708 + 16384) >> 15) << 1;

    func_80045A54(state, samples, output);
    state->phase ^= 1;
}
