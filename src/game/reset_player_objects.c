typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct Obj_80034808 {
    u8 pad_000[0xF4];
    s32 unkF4;   /* 0xF4 */
    u8 pad_0F8[0x24];
    s32 unk11C;  /* 0x11C */
} Obj_80034808;
typedef struct Player_800C3DB0 {
    Obj_80034808 *obj; /* 0x00 */
    s32 unk04;         /* 0x04 */
    s32 unk08;         /* 0x08 */
    u8 pad_0C[0x2C];
} Player_800C3DB0;
typedef struct Actor_800197C8 {
    u8 pad_00[0x2C];
    void *unk2C; /* 0x2C */
} Actor_800197C8;
extern Obj_80034808 *D_80052F04;
extern Player_800C3DB0 D_800C3DB0[2];
extern s32 D_800C3E38[2];
extern s16 D_80059DB8[2];
extern s32 D_80054AA0;
extern s32 D_800C3E28;
extern u32 D_80052EE0;
extern void func_8000E630(Obj_80034808 *o);
extern Obj_80034808 *func_800055A4(s32 id);
extern Actor_800197C8 *func_800197C8(s32 id);
extern void func_8000C530(void *obj, s32 a, s32 b);
extern void func_8000CD8C(void *obj, s32 a, s32 b);

void func_80034808(void) {
    s32 i;
    Player_800C3DB0 *p;
    s16 *ids;
    s32 *out;

    func_8000E630(D_80052F04);
    for (i = 0; i < 2; i++) {
        D_800C3DB0[i].obj = func_800055A4(D_80059DB8[i]);
        D_800C3DB0[i].unk04 = 0;
        D_800C3DB0[i].unk08 = 0;
        D_800C3E38[i] = D_80059DB8[i];
    }
    if (D_800C3DB0[0].obj->unk11C == D_800C3DB0[1].obj->unk11C) {
        D_80054AA0 = 1;
    }
    D_800C3E28 = 0;
    D_80052F04->unkF4 = 0;
    func_8000C530(&func_800197C8(0x1A2)->unk2C, 0, 0);
    func_8000C530(&func_800197C8(0x1A3)->unk2C, 0, 0);
    func_8000C530(&func_800197C8(0x1A4)->unk2C, 0, 0);
    func_8000CD8C(func_800197C8(0x1A2)->unk2C, 0, 1);
    func_8000CD8C(func_800197C8(0x1A3)->unk2C, 0, 1);
    func_8000CD8C(func_800197C8(0x1A4)->unk2C, 0, 1);
    D_80052EE0 |= 2;
}
