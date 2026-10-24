/* Target: func_80011CC4  VRAM 0x80011CC4  ROM 0x128C4
 * Size: 0x94  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Obj_80005544 {
    u8 pad_00[0xF8];
    s32 unk_F8;
};

extern struct Obj_80005544 *func_80005544(void);
extern u8 *func_800197C8(s32 id);
extern void func_80011BE0(void *pos, void *arg, s32 flag);
extern s32 D_8005300C[];

void func_80011CC4(void *arg) {
    struct Obj_80005544 *player = func_80005544();
    u8 i;

    for (i = 0; i < 2; i++) {
        s32 id = 0xEF;
        if (i == 0) {
            id = D_8005300C[player->unk_F8];
        }
        func_80011BE0(func_800197C8(id) + 0x2C, arg, 0);
    }
}
