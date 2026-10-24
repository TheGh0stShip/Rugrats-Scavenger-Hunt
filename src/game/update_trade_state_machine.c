/* func_8002EBF0  near match (SN GCC 2.8 -O2 -G0, normalized verifier)
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

struct Obj_8002E0E0 {
    u8 pad_000[0xF4];
    s32 fF4;
    s32 fF8;
    u8 pad_0FC[0x11C - 0xFC];
    s32 f11C;
    u8 pad_120[0x194 - 0x120];
    s16 timer;
    u8 pad_196[0x1E8 - 0x196];
    s16 f1E8[4];
    s16 f1F0[4];
    s16 f1F8;
};

extern struct Obj_8002E0E0 *func_800055A4(s32);
extern u32 func_800166B0(struct Obj_8002E0E0 *);
extern s32 func_8002FEB4(void);
extern void func_8002FEC4(s32);
extern void func_8002FEE0(s32);
extern void func_8002FEA8(s32);
extern s32 func_8002FED0(void);
extern u8 func_8002FEEC(void);
extern void func_8002FF48(s32);
extern s32 *func_8002A628(s32);
extern int func_8001F1C4(s32);
extern int func_8001F138(s32);
extern void func_8002DBD8(struct Obj_8002E0E0 *);
extern void func_8002DE5C(struct Obj_8002E0E0 *);
extern void func_8002E930(void);
extern void func_8002E0E0(struct Obj_8002E0E0 *);
extern void func_8002E17C(struct Obj_8002E0E0 *);
extern void func_8002E544(void *, int);
extern void func_8002E5B8(struct Obj_8002E0E0 *);
extern void func_8002E654(struct Obj_8002E0E0 *);
extern void func_8002F784(void);
extern void func_8002FAF8(void);
extern s32 func_8002F504(void);
extern s32 func_8002F428(s32);
extern s32 func_8002F6D0(void);
extern s32 func_800301C8(struct Obj_8002E0E0 *, s32);
extern void func_80010E24(s32, s32);

extern s32 D_80057D40;
extern s32 D_800B9300;
extern s32 D_800B9304;
extern s32 D_800B9308;
extern s32 D_800B930C;
extern s32 D_800B9310;
extern s32 D_800B9314;

void func_8002EBF0(void) {
    struct Obj_8002E0E0 *self;
    struct Obj_8002E0E0 *other;
    u8 k;
    s32 mine;
    s32 theirs;
    s32 room;
    s32 m;

    self = func_800055A4(D_80057D40);
    func_800166B0(self);
    switch (func_8002FEB4()) {
        case 1:
            func_8002FEC4(0);
            func_8002FEE0(0);
            func_8002FEA8(2);
            D_800B9314 = 0;
            self = func_800055A4(D_80057D40);
            func_8001F1C4(self->fF8 + 0x130);
            if (self->f11C == -1) {
                func_8002DBD8(self);
            }
            break;
        case 2:
            func_8002E930();
            self = func_800055A4(D_80057D40);
            if (self->f11C == -1) {
                func_8002E0E0(self);
            }
            func_8002F784();
            func_8002FF48(*func_8002A628(0));
            if (func_800166B0(self) & 0x10) {
                other = (struct Obj_8002E0E0 *)(u32)func_8002FED0();
                k = func_8002FEEC();
                other = func_800055A4((u8)(u32)other);
                if (self->fF4 == other->fF4) {
                    goto refuse;
                }
                if (k != 4) {
                    if (func_800301C8(self, k) != 0 || func_800301C8(other, k) != 0) {
                        goto refuse;
                    }
                    D_800B9304 = *(other->f1E8 + k);
                    D_800B9300 = *(self->f1E8 + k);
                } else {
                    D_800B9304 = other->f1F8;
                    D_800B9300 = self->f1F8;
                }
                D_800B9310 = 0;
                func_8002FEA8(3);
            }
            break;
        case 3:
            func_8002FF48(*func_8002A628(0));
            func_8002E930();
            if (func_8002F504() != 0) {
                func_8002FEA8(4);
                if (self->f11C == -1) {
                    self->timer = 40;
                }
            }
            break;
        case 5:
            func_8002FF48(*func_8002A628(0));
            func_8002FEC4(0);
            func_8002FEE0(0);
            func_8002FEA8(6);
            D_800B9314 = 0;
            if (self->f11C == -1) {
                func_8002DE5C(self);
            }
            break;
        case 6:
            func_8002FF48(*func_8002A628(0));
            func_8002E930();
            if (self->f11C == -1) {
                func_8002E0E0(self);
            }
            func_8002F784();
            if (func_800166B0(self) & 0x10) {
                other = (struct Obj_8002E0E0 *)(u32)func_8002FED0();
                k = func_8002FEEC();
                self = func_800055A4(D_80057D40);
                other = func_800055A4((u8)(u32)other);
                if (self->fF4 == other->fF4) {
                    goto refuse;
                }
                if (k != 4) {
                    if (func_800301C8(self, k) != 0 || func_800301C8(other, k) != 0) {
                        goto refuse;
                    }
                    room = *(self->f1F0 + k);
                    mine = *(self->f1E8 + k);
                    theirs = *(other->f1E8 + k);
                    room -= mine;
                    if (theirs >= room) {
                        D_800B9304 = mine + room;
                        D_800B9300 = *(other->f1E8 + k) - room;
                    } else {
                        D_800B9304 = mine + theirs;
                        D_800B9300 = 0;
                    }
                } else {
                    mine = self->f1F8;
                    room = 99 - mine;
                    theirs = other->f1F8;
                    if (theirs >= room) {
                        D_800B9304 = mine + room;
                        D_800B9300 = theirs - room;
                    } else {
                        D_800B9304 = mine + theirs;
                        D_800B9300 = 0;
                    }
                }
                if (other->f11C != -1 && other->f11C != self->f11C) {
                    func_80010E24(other->f11C, 15);
                }
                func_8002FEA8(7);
                D_800B9310 = 0;
            }
            break;
        case 7:
            func_8002FF48(*func_8002A628(0));
            func_8002E930();
            if (func_8002F504() != 0) {
                func_8002FEA8(8);
                if (self->f11C == -1) {
                    self->timer = 40;
                }
                func_8001F1C4(self->fF8 + 0x134);
            }
            break;
        case 4:
        case 8:
            func_8002FF48(*func_8002A628(0));
            func_8002E930();
            if (self->f11C == -1) {
                func_8002E0E0(self);
            }
            if (func_800166B0(self) & 0x10) {
                func_8002FEA8(0x17);
            }
            break;
        case 9:
            func_8002FEC4(0);
            func_8002FEE0(0);
            func_8002FEA8(10);
            break;
        case 10:
            func_8002FF48(*func_8002A628(0));
            func_8002E930();
            if (func_8002F428(10) != 0) {
                func_8002FEA8(11);
                if (self->f11C == -1) {
                    self->timer = 40;
                }
                func_8001F1C4(self->fF8 + 0x138);
            }
            break;
        case 12:
            func_8002FEC4(0);
            func_8002FEE0(0);
            func_8002FEA8(13);
            break;
        case 13:
            func_8002FF48(*func_8002A628(0));
            func_8002E930();
            if (func_8002F428(20) != 0) {
                func_8002FEA8(14);
                if (self->f11C == -1) {
                    self->timer = 40;
                }
                func_8001F1C4(self->fF8 + 0x138);
            }
            break;
        case 15:
            func_8002FF48(*func_8002A628(0));
            func_8002FEC4(0);
            func_8002FEE0(0);
            func_8002FEA8(16);
            D_800B9314 = 0;
            if (self->f11C == -1) {
                self->timer = 40;
                func_8002E5B8(self);
            }
            break;
        case 16:
            func_8002FF48(*func_8002A628(0));
            func_8002E930();
            if (self->f11C == -1) {
                func_8002E654(self);
            }
            func_8002F784();
            if (func_800166B0(self) & 0x10) {
                other = (struct Obj_8002E0E0 *)(u32)func_8002FED0();
                k = func_8002FEEC();
                self = func_800055A4(D_80057D40);
                other = func_800055A4((u8)(u32)other);
                if (self->fF4 == other->fF4) {
                    m = *(self->f1F0 + k);
                    if (m != -1 && k != 4) {
                        goto accept_steal;
                    }
                }
            refuse:
                func_8001F138(0x22);
                break;
            accept_steal:
                D_800B9304 = m;
                func_8002FEA8(0x11);
                D_800B9310 = 0;
            }
            break;
        case 17:
            func_8002FF48(*func_8002A628(0));
            func_8002E930();
            if (func_8002F6D0() != 0) {
                func_8002FEA8(0x12);
                if (self->f11C == -1) {
                    self->timer = 40;
                }
            }
            break;
        case 19:
            func_8002FF48(*func_8002A628(0));
            self = func_800055A4(D_80057D40);
            func_8002FEC4(((u8 *)self)[0xF7]);
            func_8002FEE0(((u8 *)func_8002A628(1))[3]);
            D_800B9308 = self->f1E8[func_8002FEEC()];
            D_800B930C = self->f1F8;
            func_8002FEA8(0x14);
            D_800B9314 = 0;
            self->timer = 40;
            break;
        case 20:
            func_8002FF48(*func_8002A628(0));
            func_8002E930();
            if (self->f11C == -1) {
                func_8002E544(self, func_8002FEEC());
            }
            func_8002FAF8();
            if (func_800166B0(self) & 0x10) {
                func_8002FEA8(0x16);
                D_800B9310 = 0;
            }
            break;
        case 11:
        case 14:
        case 18:
        case 22:
            func_8002FF48(*func_8002A628(0));
            func_8002E930();
            if (self->f11C == -1) {
                func_8002E17C(self);
            }
            if (func_800166B0(self) & 0x10) {
                func_8002FEA8(0x17);
            }
            break;
        case 23:
            func_8002FF48(*func_8002A628(0));
            func_8002E930();
            break;
    }
}
