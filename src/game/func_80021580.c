typedef int s32;

extern s32 func_8003A840(void);
extern s32 D_800529F8;

void func_80021580(s32 *val, s32 range)
{
    s32 old = *val;
    s32 r;

    do {
        r = func_8003A840() % range;
        *val = r;
    } while (old == r || r == 0);
    D_800529F8 = r;
}
