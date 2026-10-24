typedef signed int s32;

struct RoundState {
    s32 mode;
    s32 first_choice;
    s32 second_choice;
    s32 value_0c;
    s32 value_10;
    s32 value_14;
    s32 value_18;
    s32 value_1c;
    s32 value_20;
    s32 value_24;
    s32 value_28;
    s32 value_2c;
    s32 value_30;
};

extern s32 D_80052E90;
extern s32 D_80052E98;
extern s32 D_80058BF0;
extern s32 D_80058BF4;
extern s32 D_80059D40;
extern struct RoundState D_80052F18;
extern struct RoundState D_80052F4C;
extern s32 func_8003A840(void);
extern void func_800196A4(void);

void func_80012324(void) {
    D_80052E90 = 1;
    D_80058BF4 = 600;
    D_80052F4C = D_80052F18;
    D_80052F18.mode = 1;
    D_80052F18.first_choice = func_8003A840() % 3;
    D_80052F18.second_choice = func_8003A840() % 2;
    D_80052F18.value_0c = 0;
    D_80052F18.value_10 = 1;
    D_80052F18.value_14 = 2;
    D_80052F18.value_18 = 3;
    D_80052F18.value_1c = 1;
    D_80052F18.value_20 = 1;
    D_80052F18.value_24 = 1;
    D_80052F18.value_28 = 1;
    func_800196A4();
    D_80059D40 = 8;
    D_80058BF0 = 10;
    D_80052E98 = 1;
}
