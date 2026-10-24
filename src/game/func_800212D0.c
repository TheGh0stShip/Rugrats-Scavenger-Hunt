typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

struct Actor_800212D0 {
    u8 pad_00[0xF4];
    s32 index;
    s32 kind;
    u32 state;
    u8 pad_100[0x18];
    u32 flags;
    u8 pad_11C[0x14];
    s32 unk130;
};

extern s32 D_80052F18;
extern s32 D_80057D5C;
extern s32 D_80057D58;
extern s32 func_80012CE8(struct Actor_800212D0 *);
extern void func_80005EF8(struct Actor_800212D0 *);
extern s32 func_8001EEAC(void);
extern void func_8001ECE4(s32, s32);
extern void func_8001E7DC(s32);
extern void func_80021580(s32 *, s32);
extern void func_80005500(s32, struct Actor_800212D0 *);
extern void func_80009D80(struct Actor_800212D0 *);
extern void func_80012D04(s32, s32);
extern void func_8001E7BC(s32);
extern void func_8001E678(s32, s32);
extern void func_80005AFC(struct Actor_800212D0 *);
extern void func_80021464(struct Actor_800212D0 *);
extern void func_8000711C(void *);

void func_800212D0(struct Actor_800212D0 *a) {
    s32 cur, id, n;

    if (func_80012CE8(a) == 0) {
        return;
    }
    switch (a->state) {
    case 0:
        return;
    case 4:
        func_80005EF8(a);
        if (D_80052F18 == 2) {
            cur = func_8001EEAC();
            id = 0x20B;
        } else if (D_80052F18 == 0) {
            cur = func_8001EEAC();
            id = 0x202;
        } else {
            cur = func_8001EEAC();
            id = 0x206;
        }
        if (cur != id) {
            func_8001ECE4(id, 0x38);
        }
        func_8001E7DC(0xA);
        break;
    case 5:
        n = 4;
        if (a->kind == 4 && D_80052F18 == 0) {
            n = 6;
        }
        func_80021580(&a->unk130, n);
        func_80005500(0xD, a);
        a->flags &= ~0x80;
        break;
    case 8:
        func_80009D80(a);
        return;
    case 12:
        func_80012D04(0xA, 1);
        func_80005500(0xA, a);
        func_8001E7BC(0xA);
        if (a->index == D_80057D5C + D_80057D58 - 1) {
            func_8001E678(0xA, 1);
        }
        break;
    case 10:
        func_80005AFC(a);
        break;
    default:
        func_80021464(a);
        break;
    }
    func_8000711C(a);
}
