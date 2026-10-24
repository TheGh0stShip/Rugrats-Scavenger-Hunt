/* Target: func_8000AE0C  VRAM 0x8000AE0C  ROM 0xBA0C  Size: 0x200
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef unsigned char u8;
typedef unsigned int u32;
typedef signed int s32;

struct Actor_8000AE0C {
    u8 pad_00[0xE0];
    s32 unkE0;
    u8 pad_E4[0x34];
    u32 flags;      /* 0x118 */
    u8 pad_11C[0x35];
    u8 unk151;
};

extern u32 D_80057D5C;
extern u32 D_80057D58;
extern s32 D_800581A0;
extern struct Actor_8000AE0C *func_8000AD88(void);
extern s32 func_8001BF3C(void);
extern struct Actor_8000AE0C *func_800055A4(s32 idx);
extern void func_8000DDAC(s32 a);
extern void func_8000F150(struct Actor_8000AE0C *a);
extern void func_80012D9C(s32 a);
extern void func_80012D7C(s32 a);
extern void func_8001BF24(s32 a);
extern s32 func_80012CE8(void);
extern u8 *func_800197C8(s32 arg);
extern void func_8000C530(void *ctrl, s32 idx, s32 mode);
extern s32 func_8000CE08(void *model, s32 idx);

s32 func_8000AE0C(void) {
    s32 ret = 0;
    struct Actor_8000AE0C *self;
    u32 i;

    self = func_8000AD88();
    switch (func_8001BF3C()) {
        case 1:
            for (i = 0; i < D_80057D5C + D_80057D58; i++) {
                if (func_800055A4(i) != self) {
                    func_800055A4(i)->flags |= 0x100000;
                }
            }
            self->flags |= 0x400000;
            func_8000DDAC(self->unk151);
            func_8000F150(self);
            func_80012D9C(10);
            func_8001BF24(3);
            break;
        case 3:
            if (func_80012CE8() == 1) {
                func_8000C530(func_800197C8(self->unkE0) + 0x2C, 0x30, 1);
                func_8000C530(func_800197C8(self->unkE0) + 0x2C, 0, 2);
                D_800581A0 = func_8000CE08(*(void **)(func_800197C8(self->unkE0) + 0x2C), 0x30);
                func_8001BF24(4);
            }
            break;
        case 4:
            if (--D_800581A0 == 0) {
                func_80012D7C(0x14);
                func_8001BF24(5);
            }
            break;
        case 5:
            if (func_80012CE8() == 1) {
                ret = 1;
            }
            break;
    }
    return ret;
}
