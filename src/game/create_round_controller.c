typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

struct RoundController {
    u8 pad_000[8];
    s16 type;
    s16 state;
    s32 mode;
    void (*update)(void *);
    u8 pad_014[4];
    void (*draw)(void *);
    u8 pad_01C[0xC4];
    s32 resource_a;
    s32 resource_b;
    u8 pad_0E8[0x10];
    s32 value_f8;
    s32 value_fc;
    u8 pad_100[0x24];
    s32 position;
    s32 previous_position;
    s32 target_position;
    u8 pad_130[0x22];
    s16 player;
};

extern s32 D_80052F18;
extern void *D_80059DA8;
extern struct RoundController *D_80058C00;
extern void *func_8001A2B0(s32 size);
extern void func_80038E90(void *destination, s32 value, s32 size);
extern void *func_800197C8(s32 resource);
extern s32 func_8001EEAC(void);
extern void func_8001EEBC(void);
extern void func_8001ECE4(s32 resource, s32 size);
extern void func_8001260C(void *object);
extern void func_8001271C(void *object);
extern void func_80005528(void *object);
extern void func_80012778(void *object, s32 player);
extern void func_80007EFC(void *object, void *resource, s32 count, s32 mode);

void func_800124B0(s32 player) {
    struct RoundController *object = func_8001A2B0(0x200);
    s32 shared_resource = 0x20E;
    s32 selected_resource;

    func_80038E90(object, 0, 0x200);
    selected_resource = 0x83;
    if (D_80052F18 == 1) {
        selected_resource = 0x85;
    }
    object->resource_b = 0x82;
    object->resource_a = selected_resource;
    func_800197C8(object->resource_a);
    func_800197C8(object->resource_b);
    if (func_8001EEAC() != shared_resource) {
        func_8001EEBC();
        func_8001ECE4(shared_resource, 0x38);
    }
    object->update = func_8001260C;
    object->draw = func_8001271C;
    object->type = 3;
    object->mode = 2;
    object->state = 0;
    object->value_f8 = 7;
    func_80005528(object);
    object->player = player;
    func_80012778(object, player);
    object->value_fc = 5;
    object->previous_position = object->position;
    object->target_position = object->position;
    D_80058C00 = object;
    func_80007EFC(object, D_80059DA8, 4, 0);
}
