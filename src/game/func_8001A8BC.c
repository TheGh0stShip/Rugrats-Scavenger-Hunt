/* Target: func_8001A8BC  VRAM 0x8001A8BC  ROM 0x1B4BC
 * Size: 0x1C4  Compiler: SN GCC 2.8 (-O2), reviewed scheduling normalization */
typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

typedef struct { float x, y, z; } Vec3f;

struct Entry_8005368C {
    Vec3f cur;     /* 0x00 */
    Vec3f pos[2];  /* 0x0C */
    Vec3f vel;     /* 0x24 */
    s16 timer;     /* 0x30 */
    s16 dim;       /* 0x32 */
    s32 id;        /* 0x34 */
    s16 angle;     /* 0x38 */
    u8 pad_3A[2];
};

extern struct Entry_8005368C D_8005368C[5];
extern Vec3f D_8005363C[5];
extern s16 D_8005B7F2;
extern void func_8001BF24(s32);
extern void func_8001DEF8(void);

void func_8001A8BC(s32 next) {
    s32 i;
    s32 j;
    s32 dim;

    func_8001BF24(next);
    for (i = 0; i < 5; i++) {
        D_8005368C[i].timer = 6;
        j = (i + 5 - D_8005B7F2) % 5;
        dim = (i != D_8005B7F2);
        if (D_8005B7F2 == 0) {
            func_8001DEF8();
        }
        if (next == 8) {
            D_8005368C[i].vel.x = (D_8005368C[i].pos[dim].x - D_8005363C[j].x) / 6.0f;
            D_8005368C[i].vel.y = (D_8005368C[i].pos[dim].y - D_8005363C[j].y) / 6.0f;
            D_8005368C[i].vel.z = (D_8005368C[i].pos[dim].z - D_8005363C[j].z) / 6.0f;
        } else {
            D_8005368C[i].vel.x = -(D_8005368C[i].pos[dim].x - D_8005363C[j].x) / 6.0f;
            D_8005368C[i].vel.y = -(D_8005368C[i].pos[dim].y - D_8005363C[j].y) / 6.0f;
            D_8005368C[i].vel.z = -(D_8005368C[i].pos[dim].z - D_8005363C[j].z) / 6.0f;
        }
        D_8005368C[i].dim = dim;
    }
}
