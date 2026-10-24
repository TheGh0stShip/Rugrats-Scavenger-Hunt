/* Target: func_8001F9F4  VRAM 0x8001F9F4  ROM 0x205F4  Size: 48
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */

typedef unsigned char u8;
typedef int s32;

typedef struct {
    u8 unk0;
    u8 pad_1[7];
} Slot_8001F9F4;

typedef struct {
    u8 pad_00[0x50];
    Slot_8001F9F4 slots[1];
} Ent_8001F9F4;

extern Ent_8001F9F4 *D_8006D688;

s32 func_8001F9F4(u8 arg0, u8 arg1) {
    if (D_8006D688 == 0) {
        return 0;
    }
    D_8006D688->slots[arg0].unk0 = arg1;
    return 1;
}
