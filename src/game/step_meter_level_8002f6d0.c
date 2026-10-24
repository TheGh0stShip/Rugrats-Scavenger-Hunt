typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct MeterOwner {
    u8 pad_000[0x1E8];
    s16 level[4];
} MeterOwner;

extern s32 D_800B9310;
extern s32 D_800B9304;
extern s32 D_80057D40;
extern void func_8002FED0(void);
extern s32 func_8002FEEC(void);
extern MeterOwner *func_800055A4(s32);
extern void func_8001F138(s32);

s32 func_8002F6D0(void) {
    s32 idx;
    s32 cur;

    if (--D_800B9310 > 0) {
        return 0;
    }
    D_800B9310 = 5;
    func_8002FED0();
    idx = func_8002FEEC();
    idx = (idx & 0xFF) * 2 + (s32)func_800055A4(D_80057D40);
    cur = ((MeterOwner *)idx)->level[0];
    if (D_800B9304 < cur) {
        ((MeterOwner *)idx)->level[0]--;
    } else if (cur < D_800B9304) {
        ((MeterOwner *)idx)->level[0]++;
    }
    if (cur == D_800B9304) {
        return 1;
    }
    func_8001F138(4);
    return 0;
}
