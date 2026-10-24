typedef int s32;

extern s32 D_800B9344;
extern void *func_8000D050(s32 type);
extern void func_8000D0AC(void *object);
extern s32 func_80012CE8(void);
extern s32 func_800191EC(s32 mode);

void func_80030B30(void) {
    func_8000D0AC(func_8000D050(8));
    if (func_80012CE8() == 1 && func_800191EC(0) != 0) {
        D_800B9344 = 5;
    }
}
