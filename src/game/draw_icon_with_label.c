typedef unsigned int u32;
typedef signed int s32;
typedef signed short s16;

extern void func_80033118(void);
extern void *func_80032870(u32);
extern void func_8003295C(void *, s16, s16);
extern void func_800137E8(s32);
extern s32 func_8001F588(s32);
extern void func_800138FC(char *, s32, s32);

void func_8001380C(u32 id, char *str, s16 x, s16 y) {
    s32 old;

    func_80033118();
    if (id < 0x42) {
        if (id >= 0x40) {
            func_8003295C(func_80032870(id), x, y - 4);
            goto done;
        }
    }
    func_8003295C(func_80032870(id), x, y);
done:
    func_800137E8(4);
    old = func_8001F588(0);
    func_800138FC(str, x + 30, y);
    func_8001F588(old);
}
