typedef unsigned int u32;
typedef unsigned long long u64;

extern u32 func_80048900(void);
extern void func_80048920(u32 mask);
extern u32 func_80048AA0(void);
extern void func_800495C0(u32 count);
extern u32 D_800C4560;

void func_80049450(u64 time)
{
    u64 new_time;
    u32 saved_mask;

    saved_mask = func_80048900();
    D_800C4560 = func_80048AA0();
    new_time = D_800C4560 + time;
    func_800495C0(new_time);
    func_80048920(saved_mask);
}
