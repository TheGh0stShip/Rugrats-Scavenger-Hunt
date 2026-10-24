typedef short s16;
typedef int s32;

struct Obj_80057EE8 {
    s16 step;
    s16 level;
    s16 frames;
    s16 dir;
    s16 done;
};
extern struct Obj_80057EE8 D_80057EE8;
extern void func_8001F0A8(s32 left, s32 right);

s32 func_80008FB4(void) {
    struct Obj_80057EE8 *p = &D_80057EE8;
    s32 v;

    if (p->done != 0) {
        return -1;
    }
    if (p->dir == 1) {
        p->level += D_80057EE8.step;
        if (p->level >= 0xFF) {
            p->level = 0xFF;
        }
    } else {
        p->level -= D_80057EE8.step;
        if (p->level <= 0) {
            p->level = 0;
        }
    }
    v = 0x80 - D_80057EE8.level / 2;
    func_8001F0A8(v, v);
    if (D_80057EE8.level == 0xFF || D_80057EE8.level == 0) {
        if (D_80057EE8.dir == 1) {
            D_80057EE8.done = 1;
        } else if (D_80057EE8.dir == 0) {
            D_80057EE8.dir = -1;
            D_80057EE8.done = 1;
        }
        return 1;
    }
    D_80057EE8.done = 0;
    return 0;
}
