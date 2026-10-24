/* Target: func_80006B38  VRAM 0x80006B38  ROM 0x7738
 * Size: 0xE0  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef int s32;
typedef float f32;

extern void func_80006C18(s32 a, s32 b, f32 *x, f32 *y);
extern f32 func_80006D7C(s32 a, s32 b);
extern void func_80006C64(void *out, f32 angle, f32 x, f32 y);

void func_80006B38(void *out, s32 a, s32 b, f32 angle) {
    f32 pos[2];

    func_80006C18(a, b, &pos[0], &pos[1]);
    pos[0] += 225.0f;
    if (a == 0) {
        pos[0] += 75.0f;
    }
    angle = angle - func_80006D7C(a, b) * 0.3125;
    if (angle < 0.0f) {
        angle += 360.0f;
    }
    func_80006C64(out, angle, pos[0], pos[1]);
}
