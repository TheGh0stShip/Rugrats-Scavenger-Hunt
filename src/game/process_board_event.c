/* Target: func_80025730  VRAM 0x80025730  ROM 0x26330  Size: 0x4E4. */
typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

struct BoardEventPlayer {
    u8 pad_00[0xF8];
    s32 item;
    u8 pad_FC[0x118 - 0xFC];
    s32 flags;
    u8 pad_11C[0x133 - 0x11C];
    u8 state;
    u8 pad_134[0x144 - 0x134];
    s32 column;
    u8 pad_148[0x150 - 0x148];
    s16 row;
    u8 pad_152[0x1A8 - 0x152];
    u8 last_state;
};

extern s32 D_800529F8;
extern u8 D_800529FB;
extern s32 D_800535E4;
extern s32 D_800581E0;
extern s32 D_800581C0[4];
extern void *D_80052F04;
extern s32 func_8000E154(s32 row, s32 column);
extern void func_8002A5B0(s32 value);
extern void func_8002A5EC(s32 value);
extern s32 *func_8002A628(s32 index);
extern void func_80005500(s32 state, struct BoardEventPlayer *player);
extern void func_800069C8(s32 index, struct BoardEventPlayer *player, s32 value);
extern void func_80006E30(struct BoardEventPlayer *player);
extern void func_800255AC(struct BoardEventPlayer *player);
extern s32 func_80024660(struct BoardEventPlayer *player);
extern void func_8000DFD8(void);
extern int func_8001F138(s32 id);
extern void func_80024D14(struct BoardEventPlayer *player, s32 state, s32 id);
extern s32 func_800301C8(struct BoardEventPlayer *player, s32 index);
extern void func_80025498(struct BoardEventPlayer *player, s32 type);
extern void func_8000E630(void *camera);

void func_80025730(struct BoardEventPlayer *player) {
    s32 process = 0;
    s32 notify = 1;
    s32 reset = notify;
    s32 forced = process;
    s32 type;
    s32 id;

    type = (s16)func_8000E154(player->row, player->column);
    if (player->flags & 0x10) {
        forced = notify;
        player->flags &= ~0x10;
    }
    if (!forced && (D_800529F8 == 0 || D_800535E4 != 0) &&
        !(player->flags & 0x400) && ((u32)(type - 1) < 4 || type == 13)) {
        if (player->row == 0) {
            func_8002A5B0(0x2F);
            func_8002A5EC(1);
            *func_8002A628(0) = type;
            func_80005500(0, player);
            func_800069C8(0, player, 0);
        } else {
            func_80006E30(player);
        }
    } else if (D_800535E4 != 0 || D_800529F8 == 0 || forced) {
        process = 1;
    }
    if (!process) {
        return;
    }

    func_800255AC(player);
    if (!forced && func_80024660(player)) {
        notify = 0;
        goto finish;
    }
    if (D_800581E0 == 4) {
        func_8000DFD8();
    }
    switch (type) {
        case 10:
            func_8002A5B0(0x24);
            func_8001F138(0x3F);
            break;
        case 14:
            if ((u32)(D_800581C0[0] - 1) < 2) goto deny;
            func_80024D14(player, 12, 0x25);
            *func_8002A628(0) = 0;
            reset = 0;
            notify = 0;
            break;
        case 15:
            if ((u32)(D_800581C0[1] - 1) < 2) goto deny;
            func_80024D14(player, 12, 0x25);
            *func_8002A628(0) = 1;
            reset = 0;
            notify = 0;
            break;
        case 16:
            if ((u32)(D_800581C0[2] - 1) < 2) goto deny;
            func_80024D14(player, 12, 0x25);
            *func_8002A628(0) = 2;
            reset = 0;
            notify = 0;
            break;
        case 17:
            if ((u32)(D_800581C0[3] - 1) < 2) goto deny;
            func_80024D14(player, 12, 0x25);
            *func_8002A628(0) = 3;
            reset = 0;
            notify = 0;
            break;
        case 18:
            if (player->item == 0) {
                func_80024D14(player, 0x1E, 0x26);
                *func_8002A628(1) = 0;
            } else {
                func_80005500(12, player);
            }
            notify = 0;
            reset = 0;
            break;
        case 19:
            id = player->item;
            if (id == 1) {
                func_80024D14(player, 0x1E, 0x26);
                *func_8002A628(1) = id;
            } else {
                func_80005500(12, player);
            }
            notify = 0;
            reset = 0;
            break;
        case 20:
            id = player->item;
            if (id == 2) {
                func_80024D14(player, 0x1E, 0x26);
                *func_8002A628(1) = id;
            } else {
                func_80005500(12, player);
            }
            notify = 0;
            reset = 0;
            break;
        case 21:
            id = player->item;
            if (id == 3) {
                func_80024D14(player, 0x1E, 0x26);
                *func_8002A628(1) = id;
            } else {
                func_80005500(12, player);
            }
            notify = 0;
            reset = 0;
            break;
        case 22:
            if (func_800301C8(player, 0) != 0) goto deny;
            func_8002A5B0(0x27);
            *func_8002A628(1) = 0;
            break;
        case 23:
            if (func_800301C8(player, 1) != 0) goto deny;
            func_8002A5B0(0x27);
            *func_8002A628(1) = 1;
            break;
        case 24:
            if (func_800301C8(player, 2) != 0) goto deny;
            func_8002A5B0(0x27);
            *func_8002A628(1) = 2;
            break;
        case 25:
            if (func_800301C8(player, 3) != 0) goto deny;
            func_8002A5B0(0x27);
            *func_8002A628(1) = 3;
            break;
        case 6: case 7: case 8:
        case 26: case 27: case 28: case 29:
        case 30: case 31: case 32: case 33:
        case 34: case 35: case 36: case 37:
        case 38: case 39: case 40: case 41:
            func_80025498(player, type);
            break;
        case 11:
            func_80024D14(player, 0x18, 0x35);
            notify = 0;
            reset = 0;
            func_8001F138(0x45);
            break;
        case 9:
            func_80024D14(player, 13, 0x36);
            notify = 0;
            reset = 0;
            func_8000E630(D_80052F04);
            break;
        default:
        deny:
            func_80005500(12, player);
            notify = 0;
            break;
    }
finish:
    if (notify) {
        func_80005500(0, player);
    }
    if (reset) {
        func_800069C8(0, player, 0);
    }
    player->last_state = player->state - D_800529FB;
}
