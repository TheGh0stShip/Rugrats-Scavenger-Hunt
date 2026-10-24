typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Entry_8006F6B8 {
    u8 pad_00[0x4];
    u8 x;
    u8 y;
    u16 id;
    u8 pad_08[0x2];
    s16 n;
    u8 pts[9][2];
};

struct Obj_80022E54 {
    u8 pad_000[0x13E];
    u8 unk13E;
    u8 unk13F;
    u8 pad_140[0x10];
    s16 unk150;
    u8 pad_152[0xA];
    s32 unk15C;
    s32 unk160;
    u8 pad_164[0x8];
    s32 unk16C;
    s16 unk170;
    s16 unk172;
    u8 unk174[1][2];
    u8 pad_176[0x32];
    u8 unk1A8;
};

extern struct Entry_8006F6B8 D_8006F6B8[];
extern s32 D_800541A0[];
extern void func_80020050(struct Obj_80022E54 *, u8, u8, u8);
extern s16 func_8000DF48(s16);
extern s32 func_8000E108(void);
extern s32 func_8000E1B0(s32);
extern s32 func_8003A840(void);

#define ENTRY_COUNT (*(s32 *)D_8006F6B8)

void func_80022E54(struct Obj_80022E54 *obj) {
    s32 best = 20;
    s32 bestIdx;
    s32 i;
    s32 j;
    s32 v;
    s32 w;

    func_80020050(obj, obj->unk13E, obj->unk13F, obj->unk1A8);
    for (i = 0; i < ENTRY_COUNT; i++) {
        if (func_8000DF48((s16)D_8006F6B8[i].id) != 0) {
            continue;
        }
        if (obj->unk150 != func_8000E108()) {
            if (best < D_800541A0[func_8000E1B0(D_8006F6B8[i].id & 0x7FFF)]) {
                continue;
            }
            bestIdx = i;
            best = D_800541A0[func_8000E1B0(D_8006F6B8[i].id & 0x7FFF)];
        } else {
            v = func_8000E1B0(D_8006F6B8[i].id & 0x7FFF);
            w = D_800541A0[v] + func_8003A840() % 5;
            if (!(best < w)) {
                best = w;
                bestIdx = i;
            }
        }
    }
    obj->unk15C = D_8006F6B8[bestIdx].x;
    obj->unk160 = D_8006F6B8[bestIdx].y;
    obj->unk16C = D_8006F6B8[bestIdx].id & 0x7FFF;
    obj->unk170 = D_8006F6B8[bestIdx].n;
    for (j = 0; j <= D_8006F6B8[bestIdx].n; j++) {
        obj->unk174[j][0] = D_8006F6B8[bestIdx].pts[j][0];
        obj->unk174[j][1] = D_8006F6B8[bestIdx].pts[j][1];
    }
    obj->unk172 = 0;
}
