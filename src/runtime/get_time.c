typedef unsigned int u32;
typedef unsigned long long u64;

extern u32 func_80048900(void);
extern void func_80048920(u32 mask);
extern u32 func_80048AA0(void);
extern u32 D_800C4550;
extern u64 D_800C4558;

u64 func_80048A10(void)
{
    u32 current_count;
    u32 elapsed_count;
    u64 current_time;
    register u32 saved_mask;

    saved_mask = func_80048900();
    current_count = func_80048AA0();
    elapsed_count = current_count - D_800C4550;
    current_time = D_800C4558;
    func_80048920(saved_mask);

    return current_time + elapsed_count;
}
