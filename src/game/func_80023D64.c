/* Target: func_80023D64  VRAM 0x80023D64  ROM 0x24964
 * Size: 0x168  Compiler: SN Systems GCC 2.8 (-O2)
 * Confidence: verified */
typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Obj_80023D64 {
    u8 pad_000[0xFC];
    s32 unkFC;
    u8 pad_100[0x1C];
    s32 unk11C;
    u8 pad_120[0x24];
    s32 unk144;
    u8 pad_148[0x8];
    s16 unk150;
    u8 pad_152[0x42];
    s16 unk194;
};

extern s32 D_800535E4;
extern s32 D_800581C0[];
extern u32 D_800B9280;
extern void *func_80023ECC(void);
extern void **func_8002A628(s32);
extern void func_8002A5B0(s32);
extern void func_8002A5EC(s32);
extern void func_80005500(s32, struct Obj_80023D64 *);
extern s32 func_8003A840(void);
extern void func_800255AC(struct Obj_80023D64 *);

s32 func_80023D64(struct Obj_80023D64 *obj) {
    s32 ret = 0;
    void *other;

    if (obj->unk150 != 0 || obj->unkFC != 6) {
        return 0;
    }
    if (obj->unk144 == 2) {
        other = func_80023ECC();
        if (other != 0) {
            *func_8002A628(0) = other;
            func_8002A5B0(0x2E);
            func_80005500(0, obj);
            ret = 1;
        }
    }
    if (ret || D_800535E4 != 0) {
        return ret;
    }
    if (obj->unk144 == D_800581C0[4]) {
        D_800581C0[4] = obj->unk144 | 0x8000;
        D_800B9280 = func_8003A840() % 5;
        func_80005500(0, obj);
        if (D_800B9280 < 2) {
            func_8002A5B0(0x2C);
            func_8002A5EC(0);
        } else {
            func_8002A5B0(0x2B);
            func_8002A5EC(1);
        }
        if (obj->unk11C == -1) {
            obj->unk194 = 30;
        }
        func_800255AC(obj);
        ret = 1;
    }
    return ret;
}
