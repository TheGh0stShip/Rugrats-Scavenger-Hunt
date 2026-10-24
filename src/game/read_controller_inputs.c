/* Target: func_8001116C  VRAM 0x8001116C  ROM 0x11D6C
 * Size: 0x160  Compiler: SN GCC 2.8 (-O2)
 * Confidence: medium
 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct { u8 pad_00[0x18]; } OSMesgQueue_8005;

typedef struct {
    u16 button;
    s8 stick_x;
    s8 stick_y;
    u8 errno;
    u8 pad_5;
} ContPad_80058950;

typedef struct {
    u16 cur;
    u16 raw;
    u16 pressed;
    s8 stick_x;
    s8 stick_y;
    u16 unk8;
} Ctrl_80058B58;

extern OSMesgQueue_8005 D_80058B30;
extern ContPad_80058950 D_80058950[4];
extern Ctrl_80058B58 D_80058B58[4];
extern u8 D_80052C00;
extern void func_8004DB30(OSMesgQueue_8005 *mq);
extern void func_8004DBB8(ContPad_80058950 *pads);

void func_8001116C(void) {
    u16 i;

    func_8004DB30(&D_80058B30);
    func_8004DBB8(D_80058950);
    for (i = 0; i < 4; i++) {
        ContPad_80058950 *pad;
        Ctrl_80058B58 *c;
        s32 v;

        if (!((D_80052C00 >> i) & 1)) {
            continue;
        }
        pad = &D_80058950[i];
        if (pad->errno != 0) {
            continue;
        }
        if (pad->button == 0xFFFF) {
            continue;
        }
        c = &D_80058B58[i];
        c->raw = pad->button;
        c->pressed = pad->button ^ c->cur;
        c->pressed &= pad->button;
        c->cur = pad->button;

        v = pad->stick_x;
        if (v < 0) {
            v = -v;
        }
        if (20.0f < (f32)v) {
            c->stick_x = pad->stick_x;
        } else {
            c->stick_x = 0;
        }

        v = pad->stick_y;
        if (v < 0) {
            v = -v;
        }
        if (50.0f < (f32)v) {
            D_80058B58[i].stick_y = pad->stick_y;
        } else {
            D_80058B58[i].stick_y = 0;
        }
    }
}
