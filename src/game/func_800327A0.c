/* Target: func_800327A0  VRAM 0x800327A0  ROM 0x333A0
 * Size: 0xCC  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef signed char s8;
typedef signed int s32;

typedef struct Slot_800327A0 { s32 val; u8 pad_04[0xC]; } Slot_800327A0;
typedef struct Table_800327A0 {
    u8 pad_0000[0x2030];
    Slot_800327A0 slots[1]; /* 0x2030 */
} Table_800327A0;
typedef struct Obj_800327A0 {
    u8 pad_000[0x118];
    s32 flags;   /* 0x118 */
    s32 unk11C;  /* 0x11C */
    u8 pad_120[0x78];
    s32 unk198;  /* 0x198 */
} Obj_800327A0;

extern s8 D_800581E9;
extern s32 D_800C33CC;
extern Table_800327A0 *D_800581F0;
extern s32 D_8005480C;
extern void func_80031C2C(s32 idx);
extern void func_80019BD4(s32);
extern s32 func_80025E48(void);
extern void func_8001BED8(s32);
extern void func_8001A5C0(void);

void func_800327A0(Obj_800327A0 *obj) {
    s32 idx = D_800581E9;

    D_800581F0->slots[idx * 33 + D_800C33CC].val = 0x80000;
    func_80031C2C(idx);
    func_80019BD4(D_8005480C);
    D_8005480C = -1;
    if (obj->unk11C != -1) {
        obj->unk198 = obj->unk11C;
    }
    if (func_80025E48() == 0 && (obj->flags & 1)) {
        func_8001BED8(0x19);
        obj->flags &= ~1;
    } else {
        func_8001A5C0();
    }
}
