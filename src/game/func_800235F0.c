typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Obj_800235F0 {
    u8 pad_000[0xFC];
    u32 unkFC;
    u8 pad_100[0x30];
    s32 unk130;
    u8 pad_134[0x74];
    u8 unk1A8;
};

extern s32 D_80052F18;
extern s16 D_80057DA0;
extern s32 D_800529F8;
extern s32 D_800535E0;
extern s32 func_80012CE8(struct Obj_800235F0 *);
extern s32 func_8001BF3C(void);
extern void func_80011DAC(void);
extern void func_800237C0(struct Obj_800235F0 *);
extern void func_800239E8(struct Obj_800235F0 *);
extern void func_80023860(struct Obj_800235F0 *);
extern void func_80005500(s32, struct Obj_800235F0 *);
extern void func_8000550C(s32, struct Obj_800235F0 *);
extern void func_80023C40(struct Obj_800235F0 *);
extern void func_80029314(struct Obj_800235F0 *);
extern int func_80006E88(void *);
extern void func_80009D80(struct Obj_800235F0 *);
extern void func_80023B84(struct Obj_800235F0 *);
extern void func_80005AFC(struct Obj_800235F0 *);
extern void func_80024C30(struct Obj_800235F0 *);
extern void func_800120D0(struct Obj_800235F0 *);
extern void func_8000711C(void *);

void func_800235F0(struct Obj_800235F0 *obj) {
    if (func_80012CE8(obj) == 0) {
        if (func_8001BF3C() == 0x12) {
            func_80011DAC();
        }
        return;
    }
    switch (obj->unkFC) {
        case 0:
            return;
        case 1:
        case 13:
            break;
        case 4:
            func_800237C0(obj);
            break;
        case 5:
            if (D_80052F18 == 2) {
                func_800239E8(obj);
            } else {
                func_80023860(obj);
            }
            break;
        case 11:
            D_80057DA0 = 0;
            if (obj->unk1A8 == 0) {
                if (D_80052F18 == 2) {
                    D_800529F8 = 6;
                } else {
                    D_800529F8 = 5;
                }
            } else {
                D_800529F8 = obj->unk1A8;
            }
            obj->unk130 = D_800529F8;
            func_80005500(6, obj);
            func_8000550C(3, obj);
            break;
        case 6:
            func_80023C40(obj);
            break;
        case 7:
            func_80029314(obj);
            break;
        case 2:
        case 3:
            func_80006E88(obj);
            break;
        case 8:
            func_80009D80(obj);
            break;
        case 12:
            func_80023B84(obj);
            break;
        case 10:
            func_80005AFC(obj);
            break;
        case 9:
            func_80024C30(obj);
            break;
        case 14:
            func_800120D0(obj);
            break;
    }
    if (obj->unkFC != 8 && D_800535E0 == 0) {
        func_8000711C(obj);
    }
}

