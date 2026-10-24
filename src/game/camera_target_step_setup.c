typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

typedef struct { float x, y, z; } Vec3f;

struct Entry_8005368C {
    Vec3f cur;     /* 0x00 */
    Vec3f on;      /* 0x0C */
    Vec3f off;     /* 0x18 */
    Vec3f vel;     /* 0x24 */
    s16 timer;     /* 0x30 */
    u8 pad_32[0xA];
};

extern struct Entry_8005368C D_8005368C[5];
extern void func_8001BF24(s32);

void func_8001AA80(s32 dir) {
    s32 i;
    s32 six;
    s32 j;
    struct Entry_8005368C *e;
    Vec3f from;
    Vec3f to;

    func_8001BF24(dir);
    for (i = 0; i < 5; i++) {
        D_8005368C[i].timer = six = 6;
        from = D_8005368C[i].cur;
        if (dir == six) {
            j = i - 1;
            if (j < 0) {
                j = 4;
            }
        } else {
            j = i + 1;
            if (j >= 5) {
                j = 0;
            }
        }
        to = D_8005368C[j].cur;
        D_8005368C[i].vel.x = (to.x - from.x) / 6.0f;
        D_8005368C[i].vel.y = (to.y - from.y) / 6.0f;
        D_8005368C[i].vel.z = (to.z - from.z) / 6.0f;
    }
}
