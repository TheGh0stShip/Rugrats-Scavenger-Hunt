/* func_80036138: SN GCC 2.8 -O2 -G0. The empty asm hides the call
 * argument 8 from post-reload CSE so the modulo shift keeps its immediate. */
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct {
    f32 x, y, z;
} Vec3f;

struct Obj_80054B08 {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ u8 pad_0C[0x18];
    /* 0x24 */ Vec3f step;
    /* 0x30 */ s16 unk30;
    /* 0x32 */ u8 pad_32[2];
    /* 0x34 */ s32 unk34;
    /* 0x38 */ s16 unk38;
    /* 0x3A */ u8 pad_3A[2];
}; /* size 0x3C */

struct Player_80036138 {
    /* 0x000 */ u8 pad_000[0x118];
    /* 0x118 */ u32 flags;
    /* 0x11C */ s32 unk11C;
    /* 0x120 */ u8 pad_120[0x1C];
    /* 0x13C */ u8 unk13C;
    /* 0x13D */ u8 unk13D;
    /* 0x13E */ u8 pad_13E[0x12];
    /* 0x150 */ s16 unk150;
    /* 0x152 */ u8 pad_152[0x55];
    /* 0x1A7 */ s8 unk1A7;
    /* 0x1A8 */ u8 unk1A8;
};

extern s32 D_80057D40;
extern s32 D_80054AB0;
extern s32 D_80054AB4;
extern s32 D_80054AB8;
extern struct Obj_80054B08 D_80054B08[];
extern s16 D_800C3E44;
extern s32 D_800C3E50[];
extern char D_8005B218[];
extern char *D_800542DC[];
extern void *D_800529F0;

extern struct Player_80036138 *func_800055A4(s32 id);
extern s32 func_8001BF3C(void);
extern s32 func_8001BF00(void);
extern void func_8001BF24(s32 mode);
extern void func_8001BED8(s32 mode);
extern int func_8003A57C(char *buf, char *fmt, ...);
extern void func_800139F8(char *s, s32 x, s32 y);
extern void func_800138FC(char *s, s32 x, s32 y);
extern void func_800137E8(s32 a);
extern s16 func_80026010(void *p);
extern void func_80026064(struct Player_80036138 *p, s32 id);
extern void func_80026098(struct Player_80036138 *p, s32 sel);
extern void func_8001A754(void);
extern void func_800358F0(void);
extern void func_80035BD0(s32 dir);
extern s32 func_800166B0(struct Player_80036138 *p);
extern s32 func_800231B4(struct Player_80036138 *p);
extern int func_8001F138(s32 a);
extern s32 func_8001C340(void);
extern s32 func_8001C36C(void);
extern void func_8001A8BC(s32 a);
extern void func_8001A5C0(void);
extern void * func_80009140(void);
extern void func_800211C0(void *, s32 b);
extern void func_80006468(void *, int, int);
extern void func_80005500(s32 a, struct Player_80036138 *p);

void func_80036138(void) {
    s32 id;
    struct Player_80036138 *player;
    struct Obj_80054B08 *obj;
    s32 input;
    s32 i;

    player = func_800055A4(D_80057D40);

    if (func_8001BF3C() == 10 || func_8001BF3C() == 11 || func_8001BF3C() == 12 ||
        func_8001BF3C() == 13) {
        for (i = 0; i < D_80054AB0; i++) {
            obj = &D_80054B08[i];
            if (--obj->unk30 >= 0) {
                obj->pos.x += obj->step.x;
                obj->pos.y += obj->step.y;
                obj->pos.z += obj->step.z;
            } else if (func_8001BF3C() != 10 && func_8001BF3C() != 11 && func_8001BF3C() == 12) {
                func_8003A57C(D_8005B218, "Item %d Selected", D_800C3E50[D_800C3E44]);
                func_800139F8(D_8005B218, 0x100, 0xB9);
            } else {
                func_8001BF24(0);
            }
        }
    }

    if (func_8001BF3C() == 0) {
        if (D_80054B08[D_800C3E44].unk38 >= 90 &&
            D_80054B08[D_800C3E44].unk38 <= 270) {
            D_80054B08[D_800C3E44].unk38 = 270;
        }
        {
            struct Obj_80054B08 *o = &D_80054B08[D_800C3E44];
            o->unk38 = o->unk38 % 360;
        }
        { s32 x; __asm__("" : "=r"(x) : "0"(8)); func_800137E8(x); }
        if (func_8001BF00() == 0x1A && func_80026010(D_800529F0) == 3) {
            func_800138FC("Toy Chest Is Full.  You\nCannot Buy Any More Cards.", 0x100, 0xBA);
        } else if (func_8001BF00() == 0x1A && player->unk1A7 < 6) {
            func_800138FC("You Need 6 Cookies to Trade", 0x100, 0xBA);
        } else if (D_80054AB0 != 0) {
            id = *(D_800C3E50 + D_800C3E44);
            func_800138FC(D_800542DC[id], 0x100, 0xBA);
        } else {
            func_800138FC("Toy Chest Is Empty", 0x100, 0xBA);
        }
    }

    func_8001A754();
    func_800358F0();
    if (D_80054AB8 != 0) {
        D_80054AB8--;
    }
    if (func_8001BF3C() != 0) {
        return;
    }

    if (player->unk11C != -1) {
        input = func_800166B0(player);
    } else {
        input = func_800231B4(player);
    }

    if (D_80054AB0 >= 2 && D_80054AB8 == 0 && (input & 2)) {
        D_80054AB8 = 5;
        func_8001F138(D_80054AB4);
        func_80035BD0(10);
        D_80054B08[D_800C3E44].unk38 = 0;
        if (++D_800C3E44 >= D_80054AB0) {
            D_800C3E44 = 0;
        }
        return;
    }
    if (D_80054AB0 >= 2 && D_80054AB8 == 0 && (input & 1)) {
        D_80054AB8 = 5;
        func_8001F138(D_80054AB4);
        func_80035BD0(11);
        D_80054B08[D_800C3E44].unk38 = 0;
        if (--D_800C3E44 < 0) {
            D_800C3E44 = D_80054AB0 - 1;
        }
        return;
    }

    if (func_8001C340()) {
        if (D_80054AB0 == 0 && func_8001BF00() == 10) {
            goto cancel;
        }
        if (func_8001BF00() == 10) {
            func_80026098(player, D_800C3E44);
            switch (D_800C3E50[D_800C3E44]) {
                case 0x199:
                    player->unk1A8 = 0;
                    player->flags |= 0x60;
                    func_8001BED8(0);
                    func_8001BF24(14);
                    func_80005500(11, player);
                    break;
                case 0x19A:
                    player->unk1A8 = 0;
                    func_8001BED8(0);
                    func_8001BF24(14);
                    func_80005500(11, player);
                    break;
                case 0x19B:
                    func_800211C0(func_80009140(), player->unk150);
                    func_8001BED8(0);
                    func_80005500(5, player);
                    break;
                case 0x19C:
                case 0x1A1:
                    func_80006468(func_80009140(), player->unk13C, player->unk13D);
                    func_8001BED8(0);
                    player->flags |= 8;
                    func_80005500(6, player);
                    break;
                case 0x19D:
                    func_8001BED8(0x20);
                    func_8001BF24(0x31);
                    break;
                case 0x19F:
                    func_8001BED8(0x21);
                    func_8001BF24(0x31);
                    break;
                case 0x19E:
                    func_8001BED8(0x23);
                    break;
            }
        } else if (player->unk1A7 >= 6 && func_80026010(player) < 3) {
            func_80026064(player, *(D_800C3E50 + D_800C3E44));
            func_8001BED8(11);
        }
    } else if (func_8001C36C() && func_8001BF00() == 10) {
    cancel:
        func_8001BED8(4);
        func_8001A8BC(9);
    } else if (func_8001C36C()) {
        func_8001A5C0();
    }
}
