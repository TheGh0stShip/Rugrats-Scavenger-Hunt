/* Target: func_8002BC6C  VRAM 0x8002BC6C  ROM 0x2C86C
 * Size: 0xD8  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high (round 2 audit, unverified)
 * Unresolved: struct field types guessed; callback prototypes guessed */
typedef short s16;
typedef int s32;
typedef unsigned char u8;

struct Ctl_80053634 {
    u8 pad_00[8];
    s16 unk08;            /* 0x08 */
    s16 unk0A;            /* 0x0A */
    s32 unk0C;            /* 0x0C */
    void (*unk10)(void);  /* 0x10 */
    u8 pad_14[4];
    void (*unk18)(void);  /* 0x18 */
    s32 unk1C;            /* 0x1C */
    s32 unk20;            /* 0x20 */
    s32 unk24;            /* 0x24 */
    s32 unk28;            /* 0x28 */
    u8 pad_2C[0x14];
};

extern struct Ctl_80053634 *D_80053634;
extern s32 D_80059D40;
extern s32 D_80059DA8;
extern void *func_8001A2B0(s32 size);
extern void *func_80038E90(void *dst, int value, unsigned int size);
extern void func_8001DEF8(void);
extern void func_80007EFC(void *, void *, int, void *);
extern void func_8002BC50(void);
extern void func_8002D51C(void);
extern void func_8002D3DC(void);

void func_8002BC6C(void) {
    D_80053634 = func_8001A2B0(0x40);
    func_80038E90(D_80053634, 0, 0x40);
    D_80053634->unk08 = 6;
    D_80053634->unk0A = 0;
    D_80053634->unk0C = 0;
    if (D_80059D40 == 8) {
        D_80053634->unk10 = func_8002D51C;
    } else {
        D_80053634->unk10 = 0;
    }
    D_80053634->unk18 = func_8002D3DC;
    D_80053634->unk1C = 0;
    D_80053634->unk20 = 0;
    D_80053634->unk24 = 0;
    D_80053634->unk28 = 0x30;
    func_8001DEF8();
    func_80007EFC(D_80053634, D_80059DA8, 4, 0);
    if (D_80059D40 == 8) {
        func_8002BC50();
    }
}
