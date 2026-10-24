typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef float f32;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Vec3f;

struct SpinRandomObject {
    Vec3f position;
    u8 pad_0C[0x28];
    s32 item;
    u8 pad_38[4];
};

struct SpinRandomPlayer {
    u8 pad_000[0xF8];
    s32 base_id;
    u8 pad_0FC[0x20];
    s32 state;
    u8 pad_120[0x74];
    s16 timer;
    u8 pad_196[6];
    s32 choice;
    u8 pad_1A0[7];
    s8 cookies;
};

extern s32 D_80052F18;
extern struct SpinRandomPlayer *D_800529F0;
extern s32 D_80054AB0;
extern s32 D_80054ABC[][5];
extern Vec3f D_80054AE4[];
extern struct SpinRandomObject D_80054B08[];
extern s32 D_80057D5C;
extern s16 D_800C3E44;
extern u8 D_800C3E48[];
extern s32 D_800C3E50[];

extern void func_80038E90(void *dst, s32 value, s32 size);
extern void func_8001AB7C(s32 mode);
extern s32 func_8003A840(void);
extern void func_800197C8(s32 item);
extern s32 func_8001BF00(void);
extern s16 func_80026010(struct SpinRandomPlayer *player);
extern void func_8001F1C4(s32 id);

void func_80035E74(void) {
    s32 i;
    s32 pick;
    s32 slot;
    s32 selected;
    s32 offset;
    struct SpinRandomObject *object;

    D_800C3E44 = 0;
    func_80038E90(D_800C3E48, 0, 12);
    func_8001AB7C(2);
    D_80054AB0 = 3;

    offset = 0;
    for (i = 0; i < 3; i++) {
        do {
            pick = func_8003A840() % 5;
        } while (D_800C3E48[pick] != 0 ||
                 (D_80057D5C == 1 && D_80054ABC[D_80052F18][pick] == 0x19D));

        D_800C3E48[pick] = 1;
        D_800C3E50[i] = D_80054ABC[D_80052F18][pick];
        selected = D_800C3E44 - 3;
        __asm__("" : "+r"(selected));
        slot = (i - selected) % 3;
        object = (struct SpinRandomObject *)((u8 *)D_80054B08 + offset);
        object->position = D_80054AE4[slot];
        object->item = D_800C3E50[i];
        func_800197C8(D_800C3E50[i]);
        offset += sizeof(*object);
    }

    if (D_800529F0->state == -1) {
        D_800529F0->timer = 30;
        D_800529F0->choice = func_8003A840() % 3;
    }

    if (func_8001BF00() == 26 && func_80026010(D_800529F0) == 3) {
        return;
    }
    if (func_8001BF00() == 26 && D_800529F0->cookies < 6) {
        func_8001F1C4(D_800529F0->base_id + 0x177);
    } else {
        func_8001F1C4(D_800529F0->base_id + 0x173);
    }
}
