typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct GameObject {
    u8 pad_000[0x11C];
    u32 field_11C;
    u8 pad_120[0x6C];
    u32 flags;
    u8 pad_190[4];
    s16 timer;
};

extern s32 D_800B92F4;

void func_8002E478(struct GameObject *object, s32 selection) {
    s32 current;

    if (object->timer > 0) {
        object->timer = (u16)object->timer - 1;
    } else {
        current = D_800B92F4;
        object->timer = 40;
        if (selection == current) {
            object->flags |= 0x10;
        } else {
            switch (current) {
            case 1:
                object->flags |= 2;
                break;
            case 2:
                object->flags |= 8;
                break;
            case 3:
                switch (selection) {
                case 0:
                    object->flags |= 8;
                    break;
                case 2:
                    object->flags |= 2;
                    break;
                }
                break;
            case 0:
                object->flags |= 0x10;
                break;
            }
        }
    }
}
