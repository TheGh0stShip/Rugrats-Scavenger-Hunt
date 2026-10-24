typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

struct Obj_80023360 {
    u8 pad_000[0x18C];
    u32 unk18C;
    u8 pad_190[4];
    s16 unk194;
    u8 pad_196[0x16];
    u32 unk1AC[4];
};

extern s32 D_800C3E28;
extern struct Obj_80023360 *D_800C3E30;
extern struct Obj_80023360 *D_800C3E24;
extern s32 func_8001F3A8(struct Obj_80023360 *);
extern s32 func_8001BF3C(void);
extern s32 func_8003A840(void);

void func_80023360(struct Obj_80023360 *obj)
{
    s32 i;
    s32 index;

    obj->unk18C = 0;
    if (obj->unk194 > 0 || func_8001F3A8(obj)) {
        obj->unk194--;
        return;
    }
    obj->unk194 = 20;
    if (func_8001BF3C() == 0 && (u32)(D_800C3E28 - 5) < 2) {
        obj->unk18C |= 0x10;
        return;
    }
    if (D_800C3E28 != 4) {
        switch (func_8003A840() % 3) {
            case 0:
                obj->unk18C |= 0x400;
                break;
            case 1:
                obj->unk18C |= 0x1000;
                break;
            case 2:
                obj->unk18C |= 0x800;
                break;
        }
        return;
    }
    if (obj != D_800C3E30) {
        return;
    }
    index = -1;
    for (i = 0; i < 4; i++) {
        if (!(D_800C3E24->unk1AC[i] & 0x80000000) &&
            obj->unk1AC[i] != D_800C3E24->unk1AC[i]) {
            index = i;
        }
    }
    if (index == -1) {
        obj->unk18C |= 0x40;
        return;
    }
    switch (index) {
        case 0:
            obj->unk18C |= 0x1000;
            break;
        case 1:
            obj->unk18C |= 0x800;
            break;
        case 2:
            obj->unk18C |= 0x2000;
            break;
        case 3:
            obj->unk18C |= 0x400;
            break;
    }
}
