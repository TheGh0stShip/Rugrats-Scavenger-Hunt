typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;
typedef float f32;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Vec3f;

struct SpinObject {
    Vec3f position;
    u8 pad_0C[0x28];
    s32 item;
    u8 pad_38[4];
};

struct SpinPlayer {
    u8 pad_000[0x1C8];
    s32 items[3];
};

extern s32 D_80057D40;
extern s32 D_80054AB0;
extern Vec3f D_80054AE4[];
extern struct SpinObject D_80054B08[];
extern s16 D_800C3E44;
extern s32 D_800C3E50[];

extern struct SpinPlayer *func_800055A4(s32 id);
extern s16 func_80026010(struct SpinPlayer *player);
extern void func_800197C8(s32 item);

void func_80035D0C(void) {
    s32 i;
    s32 slot;
    s32 selected;
    struct SpinPlayer *player;

    D_800C3E44 = 0;
    player = func_800055A4(D_80057D40);
    D_80054AB0 = func_80026010(player);
    for (i = 0; i < D_80054AB0; i++) {
        D_800C3E50[i] = player->items[i];
        selected = D_800C3E44 - 3;
        __asm__("" : "+r"(selected));
        slot = (i - selected) % 3;
        D_80054B08[i].position = D_80054AE4[slot];
        D_80054B08[i].item = D_800C3E50[i];
        func_800197C8(D_800C3E50[i]);
    }

    while (i < 3) {
        D_80054B08[i].item = -1;
        i++;
    }
}
