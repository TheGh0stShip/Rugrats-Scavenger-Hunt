/* Target: func_8001A754  VRAM 0x8001A754  ROM 0x1B354
 * Size: 0x168  Compiler: SN GCC 2.8 (-O2), reviewed scheduling normalization */
typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

typedef struct { float x, y, z; } Vec3f;

struct Entry_8005368C {
    Vec3f cur;     /* 0x00 */
    Vec3f on;      /* 0x0C */
    Vec3f off;     /* 0x18 */
    Vec3f vel;     /* 0x24 */
    s16 timer;     /* 0x30 */
    u8 pad_32[2];
    s32 id;        /* 0x34 */
    s16 angle;     /* 0x38 */
    u8 pad_3A[2];
};

struct Rec_800197C8 {
    u8 pad_00[0x2C];
    u8 obj2C[1];   /* 0x2C */
};

extern struct Entry_8005368C D_8005368C[5];
extern s16 D_8005B7F2;
extern void *D_8005B33C;
extern s32 D_80059D48[3];
extern u8 D_80059CF0[];
extern void *func_8001E630(void);
extern void func_80047A38(void *, float, float, float, float, float, float, float);
extern struct Rec_800197C8 *func_800197C8(s32);
extern void *func_8000BDFC(void *, s32, s32, s32, void *, void *, int);
extern void func_8000C674(void *);

void func_8001A754(void) {
    s32 i;

    for (i = 0; i < 5; i++) {
        if (D_8005368C[i].cur.z == D_8005368C[i].vel.z) {
            continue;
        }
        if (i != D_8005B7F2) {
            D_8005368C[i].angle = 0;
        }
        D_8005B33C = func_8001E630();
        func_80047A38(D_8005B33C, 0.0f, (float)D_8005368C[i].angle, 0.0f, 1.0f, D_8005368C[i].cur.x, D_8005368C[i].cur.y,
                      D_8005368C[i].cur.z + ((i == D_8005B7F2) ? 800.0f : 0.0f));
        func_8000BDFC(func_800197C8(D_8005368C[i].id)->obj2C, D_80059D48[0], D_80059D48[1], D_80059D48[2],
                      D_80059CF0, D_8005B33C, 0);
        func_8000C674(func_800197C8(D_8005368C[i].id)->obj2C);
    }
}
