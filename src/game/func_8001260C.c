/* Target: func_8001260C  VRAM 0x8001260C  ROM 0x1320C
 * Size: 0x110  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct { u8 m[0x40]; } Mtx_8001260C;

struct Anim_8001260C {
    u8 pad_00[0x2C];
    u8 unk2C[1];
};

struct Obj_8001260C {
    u8 pad_00[0x2C];
    f32 unk2C[3];
    f32 unk38[3];
    u8 pad_44[0x60 - 0x44];
    Mtx_8001260C unk60[2];
    s32 unkE0;
    s32 unkE4;
    u8 pad_E8[0x150 - 0xE8];
    s16 unk150;
};

extern s8 D_800581E9;
extern u8 *D_80052F04;
extern s32 D_80054358;

extern void func_8001298C(struct Obj_8001260C *);
extern void func_80007B00(struct Obj_8001260C *);
extern struct Anim_8001260C *func_800197C8(s32);
extern void * func_8000B910(void *, void *, void *, void *, void *, int);
extern void func_8000C674(void *);

void func_8001260C(struct Obj_8001260C *obj) {
    if (obj->unk150 == D_800581E9) {
        func_8001298C(obj);
        func_80007B00(obj);
        func_8000B910(func_800197C8(obj->unkE0)->unk2C, obj->unk2C, D_80052F04 + 0x28,
                      obj->unk38, &obj->unk60[D_80054358], 0);
        func_8000B910(func_800197C8(obj->unkE4)->unk2C, obj->unk2C, D_80052F04 + 0x28,
                      obj->unk38, &obj->unk60[D_80054358], 0);
        func_8000C674(func_800197C8(obj->unkE0)->unk2C);
        func_8000C674(func_800197C8(obj->unkE4)->unk2C);
    }
}
