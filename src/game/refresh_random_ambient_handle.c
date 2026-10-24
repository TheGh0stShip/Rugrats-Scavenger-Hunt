typedef short s16;
typedef int s32;

extern s32 D_80057EF8[][9];
extern s16 D_80052A20[][9];
extern s32 D_80057EE0;

extern s32 func_8003A840(void);
extern s32 func_8001F088(s32 handle);
extern s32 func_8001F138(s32 id);

void func_80008E20(s32 idx) {
    s32 *set;
    s16 *def;
    s32 n;
    s32 r;

    r = func_8003A840() % 200;
    set = D_80057EF8[idx];
    if (r == 1) {
        n = func_8003A840();
        def = D_80052A20[idx];
        n %= def[0];
        if (set[n + 1] == 0 || func_8001F088(set[n + 1]) == 0) {
            set[n + 1] = func_8001F138(def[n + 1]);
            D_80057EE0 = n;
        }
    }
}
