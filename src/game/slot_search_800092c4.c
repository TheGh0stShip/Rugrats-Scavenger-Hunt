/* func_800092C4: SN GCC 2.8 -O2 -G0, reviewed scheduling normalization
 * (normalize_d1_func_800092C4). */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

struct Entry_80057F70 {
    s32 unk0;
    s16 slots[30];
};
extern struct Entry_80057F70 D_80057F70[];
extern u8 *D_800581F0;
extern s16 func_8000E02C(s32 id, s32 index);

s32 func_800092C4(s32 id) {
    s32 i;
    s16 *slot;
    struct Entry_80057F70 *e;
    s16 v;
    u16 u;

    if (*(s16 *)(D_800581F0 + id * 0x210 + 0x201E) > 0) {
        i = 0;
        e = &D_80057F70[id];
        slot = e->slots;
        do {
            v = *slot;
            u = *slot;
            if (v != 0 && u - 0x17CU < 4 && func_8000E02C(id, i) != 0) {
                return 1;
            }
            i++;
            slot++;
        } while (i < *(s16 *)(D_800581F0 + id * 0x210 + 0x201E));
    }
    return 0;
}
