/* func_8000D970  VRAM 0x8000D970  ROM 0xE570  Size: 0x214
 * Seed the scene randomiser or load the scene grid and compute its extent.
 * SN GCC 2.8 -O2 with reviewed scheduling normalization. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Ent_800581F0 {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    u8 pad_08[0x210 - 0x8];
} Ent_800581F0;

typedef struct Obj_800581F0 {
    s16 rows;
    s16 cols;
    s16 count;
    u8 pad_06[0x2014 - 0x6];
    Ent_800581F0 ents[1];
} Obj_800581F0;

typedef struct Blk_800581C0 {
    u8 pad_00[0x10];
    s32 unk10;
} Blk_800581C0;

typedef struct Mode_80052F18 {
    s32 mode;
    s32 base;
} Mode_80052F18;

extern Mode_80052F18 D_80052F18;
extern Blk_800581C0 D_800581C0;
extern Obj_800581F0 *D_800581F0;
extern void *func_80038E90(void *p, s32 val, s32 size);
extern s32 func_8003A840(void);
extern void func_8000DDAC(s32 idx);
extern void *func_80013238(s32 id);

void func_8000D970(void) {
    Obj_800581F0 *obj;
    s32 i;
    s32 id;

    if (D_80052F18.mode == 2) {
        func_80038E90(&D_800581C0, 0, 0x10);
        D_800581C0.unk10 = func_8003A840() % 24;
        D_800581C0.unk10 = func_8003A840() % 24;
        D_800581C0.unk10 = func_8003A840() % 24;
        D_800581C0.unk10 = func_8003A840() % 24;
        D_800581C0.unk10 = func_8003A840() % 24;
        D_800581C0.unk10 = func_8003A840() % 24;
        func_8000DDAC(0);
        return;
    }
    id = D_80052F18.mode * 3 + 0xE2;
    obj = func_80013238(D_80052F18.base + id);
    D_800581F0 = obj;
    i = 0;
    if (obj->count > 0) {
        Obj_800581F0 *o = obj;
        do {
        Ent_800581F0 *e = &o->ents[i];
        if (o->cols < e->y + e->h) {
            o->cols = e->y + e->h;
        }
        if (o->rows < e->x + e->w) {
            o->rows = e->x + e->w;
        }
            i++;
        } while (i < o->count);
    }
}
