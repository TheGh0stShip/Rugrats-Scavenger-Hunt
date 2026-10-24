/* func_80031D80: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed int s32;
typedef float f32;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Vec3f;
extern u8 D_800B9371;
extern u8 D_800B9370;
extern s32 D_80054814;
extern void *D_800B9368;
extern Vec3f D_800547CC;
extern s32 D_800547D8;
extern s32 D_8005480C;
extern Vec3f D_80059D48;
extern u8 D_80059CF0[];
extern s32 D_800549B8[];
extern void *func_8001E630(void);
extern void func_80047A38(void *, f32, f32, f32, f32, f32, f32, f32);
extern u8 *func_800197C8(s32);
extern void *func_8000BDFC(void *, Vec3f, void *, void *, s32);
extern void func_800138FC(s32, s32, s32);

void func_80031D80(void) {
    D_80054814 = D_800B9371 * 18;
    D_800B9368 = func_8001E630();
    func_80047A38(D_800B9368, 0.0f, 0.0f, (f32)D_80054814, 1.0f,
                  D_800547CC.x, D_800547CC.y, D_800547CC.z);
    func_8000BDFC(func_800197C8(D_8005480C) + 0x2C, D_80059D48, D_80059CF0, D_800B9368, 0);
    if (D_800B9371 != 0) {
        D_800B9371--;
        if (D_800B9370 == 1) {
            D_800547CC.z -= -425.0f;
        } else {
            D_800547CC.z += -425.0f;
        }
    }
    func_800138FC(*(D_800547D8 + D_800549B8), 0x100, 0xB4);
}
