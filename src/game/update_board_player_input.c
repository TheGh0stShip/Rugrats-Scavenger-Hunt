/* Target: func_800247C8  VRAM 0x800247C8  ROM 0x253C8  Size: 0x414. */
typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

struct InputCamera {
    u8 pad_00[0x100];
    s32 angle;
};

struct InputPlayer {
    u8 pad_000[0x104];
    s32 action;
    u8 pad_108[0x10];
    u32 flags;
    u8 pad_11C[8];
    s32 facing;
    s32 rotation;
    u8 pad_12C[0x10];
    u8 row;
    u8 column;
    u8 pad_13E[6];
    s32 tile_column;
    f32 height;
    f32 delta;
    s16 tile_row;
};

extern s32 D_8005435C;
extern s32 D_80052F18;
extern struct InputCamera *D_80052F04;
extern s32 D_80052F80[4];
extern int func_80006E88(void *player);
extern void func_80005500(s32 state, struct InputPlayer *player);
extern s16 func_8000E154(s16 row, s32 column);
extern f32 func_80006D00(s16 row, s32 unused, s32 column);
extern f32 func_80006D7C(s16 row, s32 unused);
extern void func_80006E30(struct InputPlayer *player);
extern s16 func_8000DF14(u8 column, u8 row);
extern s16 func_8000DF40(s16 tile);
extern s32 func_80005824(struct InputPlayer *player, u8 column, u8 row);
extern void func_80024DF8(struct InputPlayer *player);
extern void func_8001BED8(s32 value);

void func_800247C8(struct InputPlayer *player, s32 input) {
    s32 angle;
    register s32 tile __asm__("$18");
    s32 rotation;
    s32 state;
    s32 quotient;

    if (D_8005435C != 1 || func_80006E88(player) != 0) {
        return;
    }
    if (input & 0xF) {
        if (D_80052F18 == 2) {
            input = 4;
            angle = 0;
        } else {
            angle = D_80052F04->angle - 0x20D;
            angle = (player->facing - angle) % 360;
        }
        switch (angle) {
            case 0:
                break;
            case 270:
                if (input & 4) input = 1;
                else if (input & 1) input = 8;
                else if (input & 2) input = 4;
                else input = 2;
                break;
            case 180:
                if (input & 4) input = 8;
                else if (input & 1) input = 2;
                else if (input & 2) input = 1;
                else input = 4;
                break;
            case 90:
                if (input & 4) input = 2;
                else if (input & 1) input = 4;
                else if (input & 2) input = 8;
                else input = 1;
                break;
        }
    }
    if (D_80052F18 == 2) {
        input &= ~0xB;
    }
    if (input & 2) {
        rotation = player->rotation + 270;
        quotient = rotation / 360;
        state = 2;
        goto rotate;
    } else if (input & 1) {
        __asm__ volatile ("" ::: "$3");
        rotation = player->rotation + 90;
        quotient = rotation / 360;
        state = 3;
rotate:
        player->rotation = rotation - quotient * 360;
        func_80005500(state, player);
        player->action = 12;
    } else if (input & 8) {
        player->rotation = (player->rotation + 180) % 360;
        func_80005500(2, player);
        player->action = 24;
    } else if (input & 4) {
        func_80005500(1, player);
        if (D_80052F18 == 2) {
            if (player->tile_row != 0 && !(player->flags & 0x400) &&
                func_8000E154(player->tile_row, player->tile_column) == 13) {
                func_80006E30(player);
            } else {
                player->height = func_80006D00(player->tile_row, 0,
                                               player->tile_column);
                player->delta = player->height - func_80006D7C(player->tile_row, 0);
            }
        } else {
            tile = (s16)func_8000DF40(func_8000DF14(player->column, player->row));
            if ((u32)(tile - 0xF5) < 4 &&
                tile == D_80052F80[(player->rotation / 90) & 3]) {
                func_80006E30(player);
            }
            if ((u32)(tile - 0xF9) < 4 &&
                func_80005824(player, player->column, player->row)) {
                func_80006E30(player);
            }
        }
    }
    if ((input & 0x10) || (player->flags & 0x10)) {
        func_80024DF8(player);
    } else if (D_80052F18 != 2 && (input & 0x80)) {
        func_8001BED8(13);
        func_80005500(0, player);
    }
}
