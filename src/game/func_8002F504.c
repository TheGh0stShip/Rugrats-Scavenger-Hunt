/* Target: func_8002F504  VRAM 0x8002F504  ROM 0x30104
 * Size: 0x1CC  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: medium
 * Unresolved: u8 locals for callee results; index recomputation for second object may differ */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
struct Obj_8002E0E0 {
    u8 pad_000[0xF4];
    s32 fF4;            /* 0x0F4 */
    s32 fF8;            /* 0x0F8 */
    u8 pad_0FC[0x1E8 - 0xFC];
    s16 f1E8[4];        /* 0x1E8 current values */
    s16 f1F0[4];        /* 0x1F0 maximum values */
    s16 f1F8;           /* 0x1F8 */
};

extern u8 func_8002FED0(void);
extern u8 func_8002FEEC(void);
extern struct Obj_8002E0E0 *func_800055A4(s32);
extern int func_8001F138(s32);
extern s32 D_800B9310;
extern s32 D_800B9300;
extern s32 D_800B9304;
extern s32 D_80057D40;

s32 func_8002F504(void) {
    u8 who;
    u8 k;
    struct Obj_8002E0E0 *me;
    struct Obj_8002E0E0 *other;
    s32 a;
    s32 b;

    if (--D_800B9310 > 0) {
        return 0;
    }
    D_800B9310 = 5;
    who = func_8002FED0();
    k = func_8002FEEC();
    me = func_800055A4(D_80057D40);
    other = func_800055A4(who);
    if (k != 4) {
        a = other->f1E8[k];
        b = me->f1E8[k];
        if (other->f1F0[k] < D_800B9300) {
            D_800B9300 = other->f1F0[k];
        }
        if (me->f1F0[k] < D_800B9304) {
            D_800B9304 = me->f1F0[k];
        }
        if (D_800B9300 < a) {
            other->f1E8[k]--;
        } else if (a < D_800B9300) {
            other->f1E8[k]++;
        }
        if (D_800B9304 < b) {
            me->f1E8[k]--;
        } else if (b < D_800B9304) {
            me->f1E8[k]++;
        }
    } else {
        a = other->f1F8;
        b = me->f1F8;
        if (D_800B9300 < a) {
            other->f1F8--;
        } else if (a < D_800B9300) {
            other->f1F8++;
        }
        if (D_800B9304 < b) {
            me->f1F8--;
        } else if (b < D_800B9304) {
            me->f1F8++;
        }
    }
    if (b == D_800B9304 && a == D_800B9300) {
        return 1;
    }
    func_8001F138(4);
    return 0;
}
