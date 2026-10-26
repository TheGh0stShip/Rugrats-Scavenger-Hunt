typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

typedef struct {
    float x;
    float y;
    float z;
} Vec3f;

struct MenuEntry {
    Vec3f current;
    Vec3f position[2];
    Vec3f velocity;
    s16 timer;
    s16 dim;
    s32 id;
    s16 angle;
    u8 pad_3A[2];
};

struct Player {
    u8 pad_000[0x118];
    u32 flags;
    u8 pad_11C[0x8A];
    s8 state_1A6;
};

extern struct MenuEntry D_8005368C[5];
extern Vec3f D_8005363C[5];
extern s16 D_8005B7F2;
extern struct Player *D_800529F0;
extern s32 D_800537D8;
extern s32 D_80052F18;

extern s32 func_8001BF3C(void);
extern s32 func_8003A840(void);
extern void func_800069C8(s32, struct Player *, s32);
extern s16 func_80026010(struct Player *);

void func_8001B5FC(void) {
    s32 i;
    struct Player *player;

    if (func_8001BF3C() == 0) {
        s32 soundId;

        player = D_800529F0;
        if (!(player->flags & 0x4000)) {
            player->flags |= 0x4000;
            soundId = 0x31;
        } else if (player->state_1A6 < 7) {
            soundId = 0x3D;
        } else {
            s32 value = func_8003A840();

            if (value / 5 * 5 != value - 1) {
                goto setup;
            }
            soundId = 0x31;
            player = D_800529F0;
        }
        func_800069C8(soundId, player, 0);
        func_800069C8(0, D_800529F0, 2);

    setup:
        D_800537D8 = 0;
        D_8005B7F2 = 1;
        for (i = 0; i < 5; i++) {
            D_8005368C[i].dim = 1;
            D_8005368C[i].current =
                D_8005363C[(i + 5 - D_8005B7F2) % 5];
        }
    }

    if (D_800529F0 != 0 && func_80026010(D_800529F0) != 0) {
        D_8005368C[2].id = 0x1B0;
    } else {
        D_8005368C[2].id = 0x1B4;
    }
    if (D_80052F18 == 0) {
        D_8005368C[0].id = 0x1AF;
    } else {
        D_8005368C[0].id = 0x1B5;
    }
}
