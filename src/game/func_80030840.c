typedef signed int s32;

extern s32 D_800B9344;
extern void func_800309C4(void);
extern void func_80030A34(void);
extern void func_80030AB4(void);
extern void func_80030AF8(void);
extern void func_80030B30(void);
extern void func_80030B84(void);
extern void func_80030BE8(void);
extern void func_80030CC8(void);
extern void func_80030DC8(void);

void func_80030840(void) {
    switch (D_800B9344) {
        case 0:
            func_800309C4();
            break;
        case 1:
            func_80030A34();
            break;
        case 2:
            func_80030AB4();
            break;
        case 3:
            func_80030AF8();
            D_800B9344 = 4;
            break;
        case 4:
            func_80030B30();
            break;
        case 5:
            func_80030B84();
            break;
        case 6:
            func_80030BE8();
            break;
        case 7:
            func_80030CC8();
            break;
        case 8:
            func_80030DC8();
            break;
    }
}
