typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

struct Ctx_8005B214 {
    u8 pad_00[0xF4];
    s32 unkF4;      /* 0xF4 */
    u8 pad_F8[0x58];
    s16 unk150;     /* 0x150 */
    u8 pad_152[0x54];
    s8 unk1A6;      /* 0x1A6 */
};

extern struct Ctx_8005B214 *D_8005B214;
extern s32 D_80057D5C;
extern s32 D_800535E0;
extern s32 func_8000E108(void);
extern void func_8001BED8(s32);
extern void func_80005500(s32, void *);

void func_8001A5C0(void) {
    if (D_8005B214->unk1A6 == 0 && D_8005B214->unk150 != func_8000E108() &&
        D_8005B214->unkF4 < D_80057D5C && D_800535E0 == 0) {
        func_8001BED8(0x15);
    } else {
        func_8001BED8(0);
        func_80005500(0xC, D_8005B214);
    }
}

