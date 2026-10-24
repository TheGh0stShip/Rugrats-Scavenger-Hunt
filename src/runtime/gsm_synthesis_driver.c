typedef short s16;
typedef int s32;

typedef struct GsmState GsmState;

extern void func_800456E0(s16 xmax, s16 mc, s16 *xmc, s16 *erp);
extern void func_800457D4(GsmState *state, s16 nc, s16 bc, s16 *erp);
extern void func_80044270(GsmState *state, s16 *lar, GsmState *samples,
                         s16 *output);

void func_80044468(GsmState *state, s16 *lar, s16 *nc, s16 *bc, s16 *mc,
                   s16 *xmax, s16 *xmc, s16 *output) {
    s32 j;
    s16 erp[40];

    for (j = 0; j <= 3; j++, xmax++, bc++, nc++, mc++, xmc += 13) {
        func_800456E0(*xmax, *mc, xmc, erp);
        func_800457D4(state, *nc, *bc, erp);
    }
    func_80044270(state, lar, state, output);
}
