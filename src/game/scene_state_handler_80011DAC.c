/* func_80011DAC: scene state handler for states 0xF-0x12. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Player_80011DAC {
    u8 pad_00[0x2C];
    f32 unk_2C;
    f32 unk_30;
    f32 unk_34;
    u8 pad_38[0xF8 - 0x38];
    s32 unk_F8;
    u8 pad_FC[0x118 - 0xFC];
    u32 unk_118;
};

typedef struct { f32 x, y, z; } Vec3f;

struct Cam_80011DAC {
    u8 pad_00[0x58];
    Vec3f pos;
    u8 pad_64[0xF0 - 0x64];
    s32 unk_F0;
    u8 pad_F4[0xFC - 0xF4];
    s32 unk_FC;
    s32 unk_100;
    s32 unk_104;
};

struct Level_800581F0 {
    u8 pad_00[0xC];
    s16 unk_0C;
    s16 unk_0E;
};

struct Mdl_800197C8 {
    u8 pad_00[0x2C];
    void *unk_2C;
    u8 pad_30[0x3A - 0x30];
    s16 unk_3A;
};

extern struct Player_80011DAC *func_80005544(void);
extern u32 func_8001BF3C(void);
extern s32 func_80012CE8(void);
extern s32 func_800059EC(void);
extern void func_80019BD4(s32 id);
extern void func_80026310(s32 a);
extern void func_80027FD4(void);
extern s32 func_8000E078(s32 x, s32 z);
extern void func_8000DDAC(s32 a);
extern void func_80005610(s32 x, s32 z);
extern void func_80025308(struct Player_80011DAC *pl);
extern void func_8000FB08(struct Cam_80011DAC *cam, Vec3f pos);
extern void func_80005528(struct Player_80011DAC *pl, s32 a);
extern void func_8001BF24(s32 state);
extern void func_80012D04(s32 a, s32 b);
extern s32 func_8001EEAC(void);
extern void func_8001EEBC(void);
extern void func_8001ECE4(s32 a, s32 b);
extern void func_80011A40(s32);
extern struct Mdl_800197C8 *func_800197C8(s32 id);
extern s32 func_8000CE08(void *p, s32 a);
extern s32 func_8001C2C8(void);
extern void func_80005500(s32 a, struct Player_80011DAC *pl);
extern void func_8001A5C0(void);
extern void func_8001E678(s32 a, s32 b);

extern struct Cam_80011DAC *D_80052F04;
extern s32 D_80052F18;
extern struct Level_800581F0 *D_800581F0;
extern s8 D_800581E9;
extern s32 D_8005300C[];
extern s32 D_80058BE0;
extern s32 D_80058C24;

void func_80011DAC(void) {
    struct Player_80011DAC *pl = func_80005544();
    struct Mdl_800197C8 *mdl;
    struct Cam_80011DAC *cam;
    s32 i;
    s16 frame;
    s32 arg;

    switch (func_8001BF3C()) {
        case 0xF:
            if (func_80012CE8() == 0) {
                break;
            }
            if (func_800059EC() != 0) {
                break;
            }
            for (i = 0; i < 0x211; i++) {
                func_80019BD4(i);
            }
            if (D_80052F18 == 0) {
                func_80026310(0);
            } else {
                func_80026310(1);
            }
            func_80027FD4();
            func_8000DDAC(func_8000E078(D_800581F0->unk_0C, D_800581F0->unk_0E) & 0xFF);
            func_80005610(D_800581F0->unk_0C, D_800581F0->unk_0E - 2);
            func_80025308(pl);
            pl->unk_2C = 2474.0f;
            pl->unk_30 = 370.0f;
            pl->unk_34 = 910.0f;
            pl->unk_118 |= 0x100000;
            cam = D_80052F04;
            cam->pos.x = 1800.0f;
            cam->pos.y = 0.0f;
            cam->unk_FC = 25;
            cam->pos.y = 500.0f;
            cam->unk_100 = 0xD2;
            cam->pos.z = 1800.0f;
            cam->unk_104 = 0x578;
            func_8000FB08(cam, cam->pos);
            D_80052F04->unk_F0 = 12;
            func_80005528(pl, D_800581E9);
            func_8001BF24(0x10);
            func_80012D04(10, 0);
            if (func_8001EEAC() != 0x209) {
                func_8001EEBC();
                func_8001ECE4(0x209, 0x38);
            }
            break;
        case 0x10:
            if (func_80012CE8() == 0) {
                arg = 3;
                goto call;
            }
            func_80011A40(0);
            mdl = func_800197C8(D_8005300C[pl->unk_F8]);
            frame = mdl->unk_3A;
            if (frame < func_8000CE08(mdl->unk_2C, 0x3F) - 1) {
                break;
            }
            if (func_8001C2C8() == 0) {
                break;
            }
            func_8001BF24(0x12);
            func_80005500(12, pl);
            func_8001A5C0();
            break;
        case 0x11:
            break;
        case 0x12:
            if (func_80012CE8() == 0) {
                if (D_80058C24 != 0) {
                    goto call0;
                }
            }
            func_80019BD4(D_80058BE0);
            func_80019BD4(D_8005300C[pl->unk_F8]);
            for (i = 0; i < 0x211; i++) {
                func_80019BD4(i);
            }
            if (D_80052F18 == 0) {
                func_80026310(0);
            } else {
                func_80026310(1);
            }
            func_80027FD4();
            func_8001E678(10, 0);
            func_8001BF24(0x11);
            break;
    }
    return;
call0:
    arg = 0;
call:
    func_80011A40(arg);
}
