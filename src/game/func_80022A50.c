typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Obj_80022A50 {
    u8 pad_000[0xFC];
    u32 unkFC;
    u32 unk100;
    u8 pad_104[0x18];
    s32 unk11C;
    u8 pad_120[0x6C];
    u32 unk18C;
    s32 unk190;
    s16 unk194;
    u8 pad_196[0x2];
    s32 unk198;
};

extern s16 D_8005B7F2;
extern s8 D_800581E9;
extern void func_80022D78(struct Obj_80022A50 *);
extern void func_80022E54(struct Obj_80022A50 *);
extern void func_80022BE8(struct Obj_80022A50 *);
extern u32 func_8001BF00(void);
extern s32 func_8003A840(void);
extern s32 func_80023054(struct Obj_80022A50 *, s16);

void func_80022A50(struct Obj_80022A50 *obj) {
    s16 s;

    if (obj->unk11C != -1) {
        return;
    }
    obj->unk18C = 0;
    if (obj->unk194 > 0) {
        obj->unk194--;
    }
    switch (obj->unk100) {
        case 1:
            func_80022D78(obj);
            break;
        case 3:
            func_80022E54(obj);
            break;
        case 4:
            break;
    }
    if (obj->unkFC != 0) {
        if (obj->unkFC == 6) {
            func_80022BE8(obj);
        }
    } else {
        switch (func_8001BF00()) {
            case 4:
                s = D_8005B7F2;
                if (s != obj->unk190) {
                    if (obj->unk194 == 0) {
                        obj->unk18C = 2;
                        obj->unk194 = 30;
                    }
                } else {
                    obj->unk18C = 0x10;
                    if (s == 1) {
                        obj->unk194 = func_8003A840() % 20 + 20;
                    }
                }
                break;
            case 5:
                if (obj->unk194 == 0) {
                    obj->unk18C = 0x10;
                }
                break;
            case 0x18:
                if (obj->unk198 == -1) {
                    obj->unk198 = func_80023054(obj, D_800581E9);
                }
                break;
        }
    }
}
