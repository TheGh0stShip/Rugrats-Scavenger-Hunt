/* func_80031EDC: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Pad_80058B58 {
    u8 pad_00[0x4];
    u16 button;
    s8 stick_x;
    s8 stick_y;
    u8 pad_08[0x2];
};
struct Obj_80031EDC {
    u8 pad_00[0xF8];
    s32 unkF8;
    u8 pad_FC[0x20];
    s32 unk11C;
    u8 pad_120[0x74];
    s16 unk194;
};
struct Entry_80031EDC {
    u8 pad_00[0x8];
    s16 unk8;
    u8 pad_0A[0x6];
};
struct Room_80031EDC {
    u8 pad_00[0x10];
    struct Entry_80031EDC entries[32];
};
struct Map_80031EDC {
    u8 pad_00[0x2014];
    struct Room_80031EDC rooms[1];
};
struct State_800C33C0 {
    u8 pad_00[0x8];
    s32 unk8;
};
extern struct Obj_80031EDC *D_800B9360;
extern s32 D_8005818C;
extern struct State_800C33C0 D_800C33C0;
struct Big_800B9378 {
    u8 pad_0000[0xA044];
    s32 cur;
};
extern struct Big_800B9378 D_800B9378;
extern struct Pad_80058B58 D_80058B58[];
extern s8 D_800581E9;
extern struct Map_80031EDC *D_800581F0;
extern u8 D_800B9370;
extern u8 D_800B9371;
extern char D_80002B6C[];
extern char D_80002B84[];
extern s32 func_8001BF3C(void);
extern void func_80033118(void);
extern void func_80013724(void);
extern void func_800137E8(s32);
extern void func_8001380C(s32, char *, s32, s32);
extern s32 func_80009C8C(s32);
extern s32 func_80023110(struct Obj_80031EDC *, s32);
extern void func_80031804(void);
extern void func_80031780(void);
extern void func_800319B8(int);
extern s32 func_8001C340(void);
extern void func_800322B8(struct Obj_80031EDC *);
extern void func_80031D80(void);
extern void func_800327A0(struct Obj_80031EDC *);

void func_80031EDC(void) {
    struct Obj_80031EDC *obj;
    s32 res;
    s32 active;
    s32 mode;
    struct Pad_80058B58 *pad;
    f32 sx;
    s32 sy;
    u16 btn;

    obj = D_800B9360;
    res = 0;
    active = 1;
    if (func_8001BF3C() == 0x13 && obj->unk11C >= 0 && D_8005818C == 0) {
        func_80033118();
        func_80013724();
        func_800137E8(4);
        func_8001380C(0x42, D_80002B6C, 0x64, 0xB4);
        func_8001380C(0x37, D_80002B84, 0x64, 0xC8);
    }
    if (D_800C33C0.unk8 != 0) {
        D_800C33C0.unk8--;
    } else {
        mode = func_8001BF3C();
        if (mode == 0x13) {
            if (obj->unkF8 == 4) {
                res = func_80009C8C(D_800B9378.cur);
            } else if (D_8005818C != 0) {
                if (func_8001BF3C() == mode) {
                    res = 0x10;
                    if (obj->unk11C == -1) {
                        obj->unk194 = 0x3C;
                    }
                }
            } else if (obj->unk11C == -1) {
                res = func_80023110(obj, D_800B9378.cur);
                if (res == 0) {
                    active = 0;
                }
            } else {
                pad = D_80058B58;
                pad += obj->unk11C;
                sx = pad->stick_x;
                sy = pad->stick_y;
                if (sx < -50.0f && sy == 0) {
                    res = 1;
                } else {
                    btn = pad->button;
                    if (btn & 0x200) {
                        res = 1;
                    } else if (50.0f < sx && sy == 0) {
                        res = 2;
                    } else if (btn & 0x100) {
                        res = 2;
                    }
                }
            }
            if (res == 1) {
                func_80031804();
                active = 0;
            } else if (res == 2) {
                func_80031780();
                active = 0;
            }
        }
    }
    func_800319B8(D_800581E9);
    if (active == 0) {
        return;
    }
    if (obj->unkF8 == 4) {
        if (res == 0) {
            res = func_80009C8C(D_800B9378.cur);
        }
    } else if (obj->unk11C == -1) {
        if (res == 0) {
            res = func_80023110(obj, D_800B9378.cur);
        }
    } else if (func_8001BF3C() != 0x14 || D_800B9371 == 0) {
        if (func_8001C340() != 0) {
            res = 0x10;
        }
    }
    if (func_8001BF3C() == 0x13) {
        if (res == 0x10) {
            func_800322B8(obj);
        }
        return;
    }
    mode = func_8001BF3C();
    if (mode != 0x14) {
        return;
    }
    func_80031D80();
    if (res == 0x10 && D_800B9370 != 2) {
        if (D_800581F0->rooms[D_800581E9].entries[D_800B9378.cur].unk8 != -1) {
            D_800B9371 = mode;
            D_800B9370 = 2;
            return;
        }
    } else if (D_800B9370 != 2 || D_800B9371 != 0) {
        return;
    }
    func_800327A0(obj);
}
