/* func_8000D838  VRAM 0x8000D838  ROM 0xE438  Size: 0xD8
 * Step the fade-in/hold/fade-out state machine.
 * SN GCC 2.8 -O2 with reviewed scheduling normalization. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Obj_8000D790 {
    u8 pad_00[0x28];
    u32 state;    /* 0x28 */
    s32 timer;    /* 0x2C */
    u8 pad_30[0x8];
    u8 alpha;     /* 0x38 */
} Obj_8000D790;

extern u16 D_80058B5A;

void func_8000D838(Obj_8000D790 *obj) {
    switch (obj->state) {
        case 0:
            obj->alpha += 0x10;
            if (obj->alpha == 0) {
                obj->state = 1;
                obj->timer = 40;
            }
            break;
        case 3:
            obj->alpha -= 0x10;
            if (obj->alpha == 0) {
                obj->state = 4;
            }
            break;
        case 1:
            if (obj->timer != 0) {
                if (--obj->timer == 0) {
                    obj->state = 2;
                    obj->timer = 80;
                }
            }
            break;
        case 2:
            if (obj->timer != 0) {
                if (--obj->timer == 0) {
                    obj->state = 3;
                }
            } else if (D_80058B5A & 0x8000) {
                obj->state = 3;
            }
            break;
    }
}
