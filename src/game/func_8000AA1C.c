/* Target: func_8000AA1C  VRAM 0x8000AA1C  ROM 0xB61C  Size: 0x158
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

struct Obj_8000AA1C {
    u8 pad_00[0xC];
    s32 flags;      /* 0x0C */
    u8 pad_10[0xD0];
    s32 unkE0;
    u8 pad_E4[4];
    s32 phase;      /* 0xE8 */
    u8 pad_EC[0x24];
    s32 count;      /* 0x110 */
    s32 delay;      /* 0x114 */
};

struct Actor_8000AA1C {
    u8 pad_00[0xE0];
    s32 unkE0;
};

extern void *D_80059DA8;
extern s32 func_8003A840(void);
extern void func_8000A750(s32 arg);
extern struct Actor_8000AA1C *func_800206D0(void);
extern u8 *func_800197C8(s32 arg);
extern void func_8000C530(void *ctrl, s32 idx, s32 mode);
extern int func_8001F138(s32 sfx);
extern void * func_80007FF8(void *, unsigned short, unsigned short);

void func_8000AA1C(struct Obj_8000AA1C *obj) {
    switch (obj->phase) {
        case 0:
            if (obj->delay == 0) {
                obj->delay = func_8003A840() % 3 + 1;
                func_8000A750(func_8003A840() % 5);
                if (obj->count == 0x14) {
                    func_8000C530(func_800197C8(func_800206D0()->unkE0) + 0x2C, 2, 1);
                    func_8000C530(func_800197C8(func_800206D0()->unkE0) + 0x2C, 0, 2);
                    func_8001F138(0x4E);
                }
                if (--obj->count == 0) {
                    obj->phase = 1;
                }
            } else {
                obj->delay--;
            }
            break;
        case 1:
            if (func_80007FF8(D_80059DA8, 8, 1) == 0) {
                obj->flags |= 1;
            }
            break;
    }
}
