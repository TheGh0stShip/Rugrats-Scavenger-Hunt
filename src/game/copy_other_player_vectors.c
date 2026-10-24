typedef signed int s32;

struct Vec3w {
    s32 x;
    s32 y;
    s32 z;
};

struct PlayerVectors {
    struct Vec3w current;
    struct Vec3w first;
    struct Vec3w second;
    unsigned char pad_24[0x18];
};

extern struct PlayerVectors D_800544E0[2];

void func_8002B6E0(s32 argument) {
    register s32 selected __asm__("$9") = argument;
    register s32 i __asm__("$8");
    register struct PlayerVectors *player __asm__("$6");
    register struct Vec3w *source __asm__("$7");

    player = D_800544E0;
    for (i = 0; i < 2; i++, player++) {
        if (i == selected) {
            source = &player->first;
        } else {
            source = &player->second;
        }
        player->current = *source;
    }
}
