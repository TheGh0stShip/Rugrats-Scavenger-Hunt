typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

struct ZoneChoicePlayer {
    u8 pad_000[0x118];
    u32 flags;
    u8 pad_11C[0x38];
    s16 timer;
};

extern u16 D_800B9296;
extern void func_80029A40(struct ZoneChoicePlayer *player);
extern void func_80006E30(struct ZoneChoicePlayer *player);
extern void func_8001F138(s32 sound);
extern void func_8001A5C0(void);

s32 func_800299A8(struct ZoneChoicePlayer *player, u32 input) {
    s32 result = 0;

    func_80029A40(player);
    if (input & 0x10) {
        player->flags |= 0x800;
        player->timer = D_800B9296 + 1;
        result = 1;
        func_80006E30(player);
        func_8001F138(0x46);
    } else if (input & 0x40) {
        result = 1;
        func_8001A5C0();
    }
    return result;
}
