typedef unsigned char u8;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct Actor_8002903C {
    u8 pad_000[0x118];
    u32 flags;
    u8 pad_11C[0x28];
    s32 unk144;
    u8 pad_148[0xC];
    s16 unk154;
};

extern s32 func_8000E154(s32, s32);
extern void func_80006E30(struct Actor_8002903C *);

void func_8002903C(struct Actor_8002903C *object) {
    s32 result = -1;
    s32 i;
    s32 index = (object->unk144 + 1) % 24;

    for (i = 0; i < 12; i++) {
        switch ((s16)func_8000E154(0, index)) {
        case 1:
            result = 1;
            break;
        case 2:
            result = 2;
            break;
        case 3:
            result = 3;
            break;
        case 4:
            result = 4;
            break;
        }
        if (result != -1) {
            break;
        }
        index = (index + 1) % 24;
    }
    object->unk154 = result;
    object->flags |= 0x800;
    func_80006E30(object);
}
