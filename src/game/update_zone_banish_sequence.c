/* func_8002A640  VRAM 0x8002A640  ROM 0x2B240  size 0x364
 * SN GCC 2.8 -O2 -G0 -mips3; assembled after the reviewed SN scheduling normalization. */
typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef unsigned char u8;

struct Actor_8002A640 {
    u8 pad_000[0xF8];
    s32 unkF8;            /* 0xF8 */
    u8 pad_0FC[0x150 - 0xFC];
    s16 unk150;           /* 0x150 */
    u8 pad_152[0x158 - 0x152];
    u32 unk158;           /* 0x158 */
    u8 pad_15C[0x1E8 - 0x15C];
    s16 unk1E8[8];        /* 0x1E8 */
    s16 unk1F8;           /* 0x1F8 */
};

struct State_8006FB80 {
    struct Actor_8002A640 *actor;  /* 0x00 */
    u8 pad_04[4];
    s32 index;                     /* 0x08 */
};

extern struct State_8006FB80 D_8006FB80;
extern u32 D_80099B30;
extern s32 D_800B92E8;
extern s32 D_80057D5C;
extern s32 D_800581C0[];
extern char *D_80054930[];
extern struct Actor_8002A640 *func_800206D0(void);
extern u32 func_8002A604(void);
extern void func_8002A5EC(s32 value);
extern void func_8002A5B0(s32 value);
extern void func_8002E6D0(struct Actor_8002A640 *a, s32 b, s32 c);
extern s32 func_8003A840(void);
extern int func_8001F588(s32 a);
extern s32 func_8003A57C(char *buf, const char *fmt, ...);
extern void func_800139F8(char *buf, s32 x, s32 y);
extern struct Actor_8002A640 *func_800055A4(s32 i);
extern void func_80006534(struct Actor_8002A640 *a, s32 b, s32 c);
extern s32 func_80005568(struct Actor_8002A640 *a);
extern void func_80005500(s32 a, struct Actor_8002A640 *b);

void func_8002A640(void) {
    struct Actor_8002A640 *actor = D_8006FB80.actor;
    struct Actor_8002A640 *other;
    struct Actor_8002A640 *o;
    s32 i;
    s32 id;
    char buf[100];

    other = func_800206D0();
    switch (func_8002A604()) {
        case 0:
            if (D_80099B30 < actor->unk158) {
                break;
            }
            if (D_8006FB80.index == -1) {
                if (other->unk1F8 != 0) {
                    func_8002E6D0(actor, 8, other->unk1F8);
                }
                other->unk1F8 = 0;
            } else {
                if (D_8006FB80.index[other->unk1E8] != 0) {
                    func_8002E6D0(actor, D_8006FB80.index, D_8006FB80.index[other->unk1E8]);
                }
                D_8006FB80.index[other->unk1E8] = 0;
            }
            if (++D_8006FB80.index >= 4) {
                func_8002A5EC(0x36);
            }
            actor->unk158 = D_80099B30 + 10;
            break;
        case 0x36:
            if (D_800B92E8 < 0) {
                actor->unk158 += 0x3C;
                D_800B92E8 = func_8003A840() % 3;
            }
            func_8001F588(0);
            switch (D_800B92E8) {
                case 0:
                    func_8003A57C(buf, "All babies banished from\n%s Zone!", D_80054930[actor->unkF8]);
                    func_800139F8(buf, 0x100, 0xB4);
                    break;
                case 1:
                    func_8003A57C(buf, "%s Zone\nvalues set to minus!", D_80054930[actor->unkF8]);
                    func_800139F8(buf, 0x100, 0xB4);
                    break;
                case 2:
                    func_8003A57C(buf, "%s Zone\nvalues set to zero!", D_80054930[actor->unkF8]);
                    func_800139F8(buf, 0x100, 0xB4);
                    break;
            }
            if (D_80099B30 >= actor->unk158) {
                func_8002A5EC(0x37);
            }
            break;
        case 0x37:
            id = actor->unkF8;
            D_800581C0[id] = D_800B92E8;
            if (D_800B92E8 == 0) {
                for (i = 0; i < D_80057D5C; i++) {
                    o = func_800055A4(i);
                    if (o->unk150 == id + 1) {
                        func_80006534(o, 0, func_8003A840() % 24);
                    }
                }
            }
            if (func_80005568(other) != 0) {
                func_80005500(0xC, other);
                func_80005500(0, actor);
            } else {
                func_80005500(0xC, actor);
                func_80005500(0, other);
            }
            func_8002A5B0(0);
            break;
    }
}
