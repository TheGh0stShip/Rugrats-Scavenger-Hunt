/* Target: func_80034978  VRAM 0x80034978  ROM 0x35578
 * Size: 0xEEC  Pipeline: SN GCC 2.8 -O2 -G0 -mcpu=vr4300 -mips3
 * Confidence: medium */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct Vec3f { f32 x, y, z; } Vec3f;

typedef struct PObj_80034978 {
    u8 pad_000[0xE0];
    struct PObj_80034978 *unkE0; /* 0xE0 */
    u8 pad_0E4[0xC];
    s32 unkF0;      /* 0xF0 */
    s32 unkF4;      /* 0xF4 */
    s32 unkF8;      /* 0xF8: character index */
    u8 pad_0FC[0x4];
    s32 unk100;     /* 0x100 */
    s32 unk104;     /* 0x104 */
    u8 pad_108[0x14];
    s32 unk11C;     /* 0x11C: controller index, -1 = cpu */
    u8 pad_120[0x6C];
    u32 unk18C;     /* 0x18C */
    u8 pad_190[0x1C];
    s32 items[4];   /* 0x1AC */
} PObj_80034978;

typedef struct Player_800C3DB0 {
    PObj_80034978 *obj; /* 0x00 */
    s32 actorId;        /* 0x04: 0x1A2..0x1A4 choice, 0 = none */
    u32 input;          /* 0x08 */
    u32 held;           /* 0x0C */
    Vec3f pos;          /* 0x10 */
    Vec3f home;         /* 0x1C */
    Vec3f vel;          /* 0x28 */
    s32 **anim;         /* 0x34 */
} Player_800C3DB0;

typedef struct Pad_80058B58 {
    u8 pad_0[0x2];
    u16 held;  /* 0x2 */
    u8 pad_4[0x6];
} Pad_80058B58;

typedef struct Actor_800197C8 {
    u8 pad_00[0x2C];
    s32 *unk2C; /* 0x2C */
} Actor_800197C8;

extern Player_800C3DB0 D_800C3DB0[2];
extern s32 D_800C3E20;
extern PObj_80034978 *D_800C3E24;
extern u32 D_800C3E28;
extern s32 D_800C3E2C;
extern PObj_80034978 *D_800C3E30;
extern s32 D_800C3E38[];
extern PObj_80034978 *D_80052F04;
extern u32 D_80052EE0;
extern s32 D_80054A9C;
extern s32 D_80054AA0;
extern s32 D_80054AA4;
extern s32 D_80054AA8;
extern Vec3f D_80054A0C[3];
extern Vec3f D_80054A30[];
extern s32 D_800538D8[6];
extern char *D_800535E8[];
extern s16 D_80059DB8[];
extern Pad_80058B58 D_80058B58[];
extern s8 D_800581E9;
extern u8 *D_800581F0;

extern s32 func_8001BF3C(void);
extern s32 func_80012CE8(void);
extern void func_8001BF24(s32);
extern void func_8000B8A8(s32);
extern void func_80012D9C(s32);
extern void func_8001E718(s32);
extern void func_8001EEBC(void);
extern void func_8001ECE4(s32, s32);
extern void func_80019BD4(s32);
extern void func_8001DEF8(void);
extern int func_8001F1C4(s32);
extern void func_8000B904(void);
extern void func_80010AE8(PObj_80034978 *);
extern void *func_8001EED0(void);
extern void func_8001BED8(s32);
extern void func_8001A5C0(void);
extern void func_80033E90(void);
extern void func_8001D2F4(s32 *);
extern s32 func_8001EEAC(void);
extern void func_800344A8(void);
extern void func_80034434(void);
extern void func_800139F8(char *s, s32 x, s32 y);
extern void func_800345EC(void);
extern int func_8003A57C(char *buf, char *fmt, ...);
extern void func_8001380C(s32 x, char *s, s32 y, s32 w);
extern void func_80034350(void);
extern void func_80023360(PObj_80034978 *);
extern u32 func_800166B0(PObj_80034978 *);
extern Actor_800197C8 *func_800197C8(s32);
extern void func_8000CD50(s32 *, s32, s32);
extern void func_8000C530(s32 **, s32, s32);
extern PObj_80034978 *func_800055A4(s32);
extern PObj_80034978 *func_800112D0(s32 id, s32 a, s32 b, s32 c, s32 d, s32 e);
extern void func_8002621C(PObj_80034978 *, s32);
extern void func_8001F30C(void);
extern s32 func_8001C2C8(void);
extern void func_80012D7C(s32);
extern void func_8001E6F8(s32);

void func_80034978(void) {
    s32 i;
    s32 k;
    s32 state;
    s32 sel;
    s32 done;
    s32 base;
    s32 snd;
    s32 w;
    s32 l;
    f32 n;
    Player_800C3DB0 *p;
    PObj_80034978 *o;
    char buf[0x70];

    done = 0;
    sel = -1;
    switch ((u32)func_8001BF3C()) {
    case 0x23:
        if (func_80012CE8() == 1) {
            func_8001BF24(0x24);
            func_8000B8A8(0x20000000);
            func_80012D9C(10);
            func_8001E718(10);
            func_8001EEBC();
            func_8001ECE4(0x207, 0x38);
            D_80052F04->unkF0 = 0x15;
            func_80019BD4(*(s16 *)(D_800581F0 + D_800581E9 * 33 * 16 + 0x201C));
            func_8001DEF8();
        }
        break;
    case 0x24:
        if (func_80012CE8() == 1) {
            func_8001BF24(0);
            if (D_800C3DB0[0].obj->unk11C >= 0 && D_80054AA0 != 0) {
                func_8001F1C4(D_800C3DB0[0].obj->unkF8 + 3);
            }
        }
        break;
    case 0x25:
        if (func_80012CE8() == 1) {
            func_8001BF24(0x26);
            func_8000B904();
            func_80012D9C(10);
            func_80010AE8(D_80052F04);
            func_8001EED0();
            func_8001E718(10);
        }
        break;
    case 0x26:
        if (func_80012CE8() == 1) {
            func_8001BED8(0);
            func_8001BF24(0);
            func_8001A5C0();
            return;
        }
        break;
    }

    state = func_8001BF3C();
    if (state != 0x23 && state != 0x26) {
        func_80033E90();
        func_8001D2F4(D_800C3E38);
        switch (D_800C3E28) {
        case 0:
            if (func_8001EEAC() != 0x207) {
                func_8001ECE4(0x207, 0x38);
            }
            func_800344A8();
            func_80034434();
            if (D_80054AA0 != 0) {
                func_800139F8("Pick A Toy", D_800C3DB0[0].actorId != 0 ? 0x180 : 0x80, 0x40);
            } else {
                if (D_800C3DB0[0].actorId == 0) {
                    func_800139F8("Pick A Toy", 0x80, 0x40);
                }
                if (D_800C3DB0[1].actorId == 0) {
                    func_800139F8("Pick A Toy", 0x180, 0x40);
                }
            }
            break;
        case 1:
            func_800345EC();
            func_800139F8("You picked the same toy,", 0x100, 0x40);
            func_800139F8("it's a draw!", 0x100, 0x54);
            break;
        case 2:
        case 3:
            if (func_8001EEAC() != 0x208) {
                func_8001ECE4(0x208, 0x38);
            }
            func_800345EC();
            break;
        case 4:
        case 5:
        case 6:
            func_800139F8("Winner!", (1 - D_800C3E2C) * 140 + 0xBA, 0x15);
            for (i = 0; i < 4; i++) {
                if (!(D_800C3E24->items[i] & 0x80000000) && D_800C3E24->items[i] != D_800C3E30->items[i]) {
                    break;
                }
            }
            if (D_800C3E28 == 5) {
                func_8003A57C(buf, "%s %s", D_800535E8[D_800C3E30->unkF8], "Won 6 Cookies!");
                func_800139F8(buf, 0x100, 0xB4);
            } else if (D_800C3E28 == 4) {
                func_8001380C(0x40, "Pick a prize", 0xB4, 0xBE);
                if (D_80052F04->unk100 == D_80059DB8[D_800C3E20] * 90) {
                    func_80034350();
                }
            }
            break;
        }
    }

    if (D_80054AA8 != 0) {
        if (--D_80054AA8 != 0) {
            goto tail;
        }
    }

    p = D_800C3DB0;
    for (i = 0; i < 2; i++, p++) {
        if (p->obj->unk11C == -1) {
            func_80023360(p->obj);
        }
    }

    if (D_80054AA0 != 0) {
        if (D_800C3E28 == 4) {
            i = D_800C3E2C;
        } else {
            i = D_800C3DB0[0].actorId != 0;
        }
        p = &D_800C3DB0[i];
        p->held = D_80058B58[p->obj->unk11C].held & 0xB;
        if ((p->input = func_800166B0(p->obj)) != 0) {
            D_80054AA8 = 10;
        }
    } else {
        p = D_800C3DB0;
        for (i = 0; i < 2; i++, p++) {
            p->input = func_800166B0(p->obj);
        }
    }

    switch (D_800C3E28) {
    case 0:
        p = D_800C3DB0;
        for (i = 0; i < 2; i++, p++) {
            if (p->actorId != 0) {
                continue;
            }
            p->input &= 0x1C00;
            switch (p->input) {
            case 0x400:
                p->actorId = 0x1A2;
                p->home = D_80054A0C[0];
                p->pos = p->home;
                break;
            case 0x1000:
                p->actorId = 0x1A3;
                p->home = D_80054A0C[1];
                p->pos = p->home;
                break;
            case 0x800:
                p->actorId = 0x1A4;
                p->home = D_80054A0C[2];
                p->pos = p->home;
                break;
            }
            if (D_80054AA0 != 0 && i == 0 && D_800C3DB0[0].input != 0 &&
                D_800C3DB0[1].obj->unk11C >= 0) {
                func_8001F1C4(D_800C3DB0[1].obj->unkF8 + 3);
            }
        }
        if (D_800C3DB0[0].actorId != 0 && D_800C3DB0[1].actorId != 0) {
            if (D_800C3DB0[0].actorId == D_800C3DB0[1].actorId) {
                func_8001F1C4(8);
                D_800C3E28 = 1;
                D_80054A9C = 6;
                D_80054AA8 = 0x1E;
                p = D_800C3DB0;
                for (i = 0; i < 2; i++, p++) {
                    p->home = D_80054A30[2];
                    n = (f32)D_80054A9C;
                    p->vel.x = (f32)(s32)((p->home.x - p->pos.x + n - 1.0f) / n);
                    p->vel.y = (f32)(s32)((p->home.y - p->pos.y + n - 1.0f) / n);
                    p->vel.z = (f32)(s32)((p->home.z - p->pos.z + n - 1.0f) / n);
                }
            } else {
                D_800C3E28 = 2;
                D_80054A9C = 6;
                D_80054AA8 = 0x1E;
                base = 0;
                switch (D_800C3DB0[0].actorId + D_800C3DB0[1].actorId - 0x344) {
                case 2:
                    base = 3;
                    break;
                case 3:
                    base = 6;
                    break;
                }
                p = D_800C3DB0;
                for (i = 0; i < 2; i++, p++) {
                    p->home = D_80054A30[i + base];
                    n = (f32)D_80054A9C;
                    p->vel.x = (f32)(s32)((p->home.x - p->pos.x + n - 1.0f) / n);
                    p->vel.y = (f32)(s32)((p->home.y - p->pos.y + n - 1.0f) / n);
                    p->vel.z = (f32)(s32)((p->home.z - p->pos.z + n - 1.0f) / n);
                    p->anim = &func_800197C8(p->actorId)->unk2C;
                    func_8000CD50(*p->anim, 0, 1);
                }
            }
        }
        break;
    case 1:
        D_800C3E28 = 0;
        D_800C3DB0[1].actorId = 0;
        D_800C3DB0[0].actorId = 0;
        break;
    case 2:
        switch (D_800C3DB0[0].actorId) {
        case 0x1A2:
            D_800C3E2C = D_800C3DB0[1].actorId == 0x1A4;
            break;
        case 0x1A3:
            D_800C3E2C = D_800C3DB0[1].actorId == 0x1A2;
            break;
        case 0x1A4:
            D_800C3E2C = D_800C3DB0[1].actorId == 0x1A3;
            break;
        }
        w = D_800C3E2C;
        l = w ^ 1;
        D_800C3E20 = l;
        D_800C3E30 = D_800C3DB0[w].obj;
        D_800C3E24 = D_800C3DB0[l].obj;
        func_8000C530(D_800C3DB0[w].anim, 1, 0);
        func_8000CD50(*D_800C3DB0[D_800C3E2C].anim, 1, 1);
        func_8000C530(D_800C3DB0[D_800C3E20].anim, 2, 0);
        func_8000CD50(*D_800C3DB0[D_800C3E20].anim, 2, 1);
        D_800C3E28 = 3;
        D_80054AA8 = 0x78;
        break;
    case 3:
        D_800C3E28 = 4;
        D_80054AA4 = 1;
        D_800C3E38[D_800C3E20] = -1;
        break;
    case 4:
        for (i = 0; i < 4; i++) {
            if (!(D_800C3E24->items[i] & 0x80000000) && D_800C3E24->items[i] != D_800C3E30->items[i]) {
                break;
            }
        }
        if (D_80054AA4 != 0) {
            D_80054AA4 = 0;
            o = func_800055A4(D_80059DB8[D_800C3E2C]);
            switch (o->unkF8) {
            case 0:
                snd = 10;
                break;
            case 1:
                snd = 0;
                break;
            case 2:
                snd = 1;
                break;
            case 3:
                snd = 2;
                break;
            }
            func_8001F1C4(snd);
            D_80052EE0 &= ~2;
            o = D_80052F04;
            o->unk104 = 0xC80;
            o->unkF4 = 1;
            o->unk100 = D_80059DB8[D_800C3E20] * 90;
        }
        if (i == 4) {
            for (k = 0; k < 6; k++) {
                D_80052F04->unk100 = D_80059DB8[D_800C3E2C] * 90;
                o = func_800112D0(0x1AB, D_800538D8[k], -0x168, -0xFA0, 0, k * 5);
                o->unkE0 = func_800055A4(D_80059DB8[D_800C3E2C]);
            }
            D_80054AA8 = 0x3C;
            D_800C3E28 = 5;
        } else {
            u32 in = D_800C3DB0[D_800C3E2C].input;
            if (in & 0x1000) {
                sel = 0;
            } else if (in & 0x800) {
                sel = 1;
            } else if (in & 0x2000) {
                sel = 2;
            } else if (in & 0x400) {
                sel = 3;
            }
            if (sel >= 0 && D_800C3E24->items[sel] >= 0 && (D_800C3E30->items[sel] & 0x80000000)) {
                D_800C3E24->items[sel] |= 0x80000000;
                done = 1;
                D_800C3E30->items[sel] &= 0x7FFFFFFF;
                func_8002621C(D_800C3E24, 6);
            }
            if (done) {
                D_800C3E28 = 6;
                func_8001F30C();
            }
        }
        break;
    }

tail:
    if (func_8001BF3C() == 0 && D_800C3E28 - 5 < 2) {
        if (func_8001C2C8() != 0 || (D_800C3DB0[D_800C3E2C].obj->unk18C & 0x10)) {
            func_8001BF24(0x25);
            func_80012D7C(10);
            func_8001E6F8(10);
        }
    }
}
