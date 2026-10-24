typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;
typedef double f64;
typedef s32 M2C_UNK;
typedef u32 M2C_UNK32;
#define M2C_FIELD(base, type, offset) (*(type)((u8 *)(base) + (offset)))
#define NULL ((void *)0)

M2C_UNK func_80005500(M2C_UNK, void *);             /* extern */
M2C_UNK func_8000550C(M2C_UNK, void *);             /* extern */
void *func_80005544();                              /* extern */
s32 func_80005598();                                /* extern */
s32 func_80008B80(f32, s32, f32, f32, s32, f32);    /* extern */
M2C_UNK func_8000F34C(void *);                      /* extern */
M2C_UNK func_8000F4E4(void *);                      /* extern */
M2C_UNK func_8000F604(void *, void *);              /* extern */
M2C_UNK func_8000F800(void *);                      /* extern */
M2C_UNK func_8000FB08();                            /* extern */
M2C_UNK func_8000FD38(void *);                      /* extern */
M2C_UNK func_8000FD9C(void *);                      /* extern */
M2C_UNK func_80010358(void *);                      /* extern */
M2C_UNK func_80010760(void *);                      /* extern */
M2C_UNK func_80010850(void *);                      /* extern */
M2C_UNK func_800109EC(void *, s32);                 /* extern */
M2C_UNK func_8001BED8(M2C_UNK);                     /* extern */
extern s32 D_80052BE8;
extern s32 D_80052EE4;
extern s32 D_80052F18;
extern s32 D_8005435C;
extern s32 D_800547C0;
extern s32 D_800547C4;
extern s32 D_80057D40;
extern s32 D_80058670;
extern s32 D_80058674;
extern M2C_UNK D_800588F8;
extern M2C_UNK D_80058B58;
extern M2C_UNK D_800C33D8;
extern s32 D_800C33F4;
extern f64 D_DBL_80000800;
extern f64 D_DBL_80000808;
extern f64 D_DBL_80000810;
extern f64 D_DBL_80000818;
extern f64 D_DBL_80000820;
extern f64 D_DBL_80000828;
extern f32 D_FLT_800007E4;
extern f32 D_FLT_800007E8;
extern f32 D_FLT_800007EC;
extern f32 D_FLT_800007F0;
extern f32 D_FLT_800007F4;
extern f32 D_FLT_800007F8;
extern f32 D_FLT_80000830;
extern s32 D_FLT_800547C8;
extern s32 D_FLT_800587F0;

void func_8000E7C8(void *arg0) {
    M2C_UNK var_a1;
    M2C_UNK var_a2;
    f32 temp_f2;
    f32 temp_f4;
    f32 var_f0;
    f64 var_f2;
    f64 var_f2_2;
    f64 var_f2_3;
    f64 var_f4;
    f64 var_f4_2;
    f64 var_f4_3;
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_a0_3;
    s32 temp_a0_4;
    s32 temp_a0_5;
    s32 temp_a2;
    s32 temp_ret;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 var_lo;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;
    u32 temp_v1;
    void *temp_s2;
    void *temp_v0_6;

    if (D_8005435C != 0) {
        temp_s2 = func_80005544();
        if (M2C_FIELD(&D_800C33D8, s32 *, 0x2C) != 0) {
            M2C_FIELD(&D_800C33D8, s32 *, 0x2C) = 0;
            M2C_FIELD(arg0, u32 *, 0xF0) = 0xEU;
        }
        temp_v1 = M2C_FIELD(arg0, u32 *, 0xF0);
        switch (temp_v1) {
        case 0:
            func_8000F604(arg0, temp_s2);
            func_8000FB08(arg0, M2C_FIELD(arg0, f32 *, 0x58), M2C_FIELD(arg0, f32 *, 0x5C), M2C_FIELD(arg0, f32 *, 0x60), M2C_FIELD(temp_s2, f32 *, 0x2C), M2C_FIELD(arg0, f32 *, 0x5C), M2C_FIELD(temp_s2, f32 *, 0x34));
            if (D_80052EE4 != 0) {
                M2C_FIELD(arg0, u32 *, 0xF0) = 0xDU;
            }
            break;
        case 1:
            if (func_80005598(temp_s2) == 0) {
                var_v0 = 1;
            } else {
                var_v0 = 0x12;
                if (M2C_FIELD(temp_s2, s32 *, 0x11C) == -1) {
                    var_v0 = 0xC;
                }
            }
            M2C_FIELD(arg0, s32 *, 0xEC) = var_v0;
            M2C_FIELD(&D_800588F8, f32 *, 0x18) = (f32) M2C_FIELD(arg0, s32 *, 0xFC);
            temp_f2 = (f32) M2C_FIELD(arg0, s32 *, 0x100);
            M2C_FIELD(&D_800588F8, f32 *, 0x1C) = temp_f2;
            M2C_FIELD(&D_800588F8, f32 *, 0x20) = (f32) M2C_FIELD(arg0, s32 *, 0x104);
            M2C_FIELD(&D_800588F8, f32 *, 0x24) = (f32) M2C_FIELD(arg0, f32 *, 0x58);
            M2C_FIELD(&D_800588F8, f32 *, 0x28) = (f32) M2C_FIELD(arg0, f32 *, 0x5C);
            M2C_FIELD(&D_800588F8, f32 *, 0x2C) = (f32) M2C_FIELD(arg0, f32 *, 0x60);
            var_v0_2 = -0x18;
            if (D_80052F18 == 2) {
                var_f0 = temp_f2 + D_FLT_800007E4;
                goto block_15;
            }
            var_v0_2 = -0x12;
            if (M2C_FIELD(temp_s2, s32 *, 0x1FC) == 0) {
                var_f0 = temp_f2 + D_FLT_800007E8;
block_15:
                M2C_FIELD(&D_800588F8, s32 *, 0x30) = var_v0_2;
                M2C_FIELD(&D_800588F8, s32 *, 0x34) = (s32) var_f0;
            } else {
                M2C_FIELD(&D_800588F8, s32 *, 0x30) = -0x12;
                M2C_FIELD(&D_800588F8, s32 *, 0x34) = 0x159;
            }
            temp_a0 = M2C_FIELD(temp_s2, s32 *, 0x11C);
            var_a2 = 0xB60B0000;
            if (temp_a0 == -1) {
                var_a1 = (s32)&D_800588F8;
                M2C_FIELD(&D_800588F8, s32 *, 0x34) = (s32) M2C_FIELD(temp_s2, s32 *, 0x124);
                if (M2C_FIELD(temp_s2, s32 *, 0x11C) == temp_a0) {
                    M2C_FIELD(&D_800588F8, s32 *, 0x38) = 0x960;
                } else {
                    goto block_21;
                }
            } else {
block_21:
                var_a2 = 0xB60B60B7;
                temp_v1_2 = M2C_FIELD(&D_800588F8, s32 *, 0x34) % 360;
                var_a1 = 0xB4 - temp_v1_2;
                if ((f32) var_a1 >= 0.0f) {
                    var_lo = D_80052BE8 * (s32) var_a1;
                } else {
                    var_lo = D_80052BE8 * (temp_v1_2 - 0xB4);
                }
                M2C_FIELD(&D_800588F8, s32 *, 0x38) = (s32) ((var_lo / 180) + 0x960);
            }
            M2C_FIELD(&D_800588F8, f32 *, 0x3C) = (f32) M2C_FIELD(temp_s2, f32 *, 0x2C);
            M2C_FIELD(&D_800588F8, f32 *, 0x40) = (f32) M2C_FIELD(temp_s2, f32 *, 0x30);
            M2C_FIELD(&D_800588F8, f32 *, 0x44) = (f32) M2C_FIELD(temp_s2, f32 *, 0x34);
            if (D_80052F18 == 2) {
                if (func_80005598(temp_s2, (s32) var_a1, var_a2) != 0) {
                    M2C_FIELD(&D_800588F8, f32 *, 0x40) = (f32) D_FLT_800007EC;
                } else {
                    M2C_FIELD(&D_800588F8, f32 *, 0x40) = (f32) D_FLT_800007F0;
                }
            } else if (M2C_FIELD(temp_s2, s32 *, 0xF8) == 5) {
                M2C_FIELD(&D_800588F8, f32 *, 0x40) = (f32) D_FLT_800007F4;
            } else {
                M2C_FIELD(&D_800588F8, f32 *, 0x40) = (f32) D_FLT_800007F8;
            }
            M2C_FIELD(arg0, f32 *, 0x58) = (f32) M2C_FIELD(&D_800588F8, f32 *, 0x24);
            M2C_FIELD(arg0, f32 *, 0x5C) = (f32) M2C_FIELD(&D_800588F8, f32 *, 0x28);
            M2C_FIELD(arg0, f32 *, 0x60) = (f32) M2C_FIELD(&D_800588F8, f32 *, 0x2C);
            func_8000FB08(arg0, M2C_FIELD(arg0, f32 *, 0x58), M2C_FIELD(arg0, f32 *, 0x5C), M2C_FIELD(arg0, f32 *, 0x60));
            temp_a0_2 = M2C_FIELD(arg0, s32 *, 0xEC);
            var_f2 = (f64) temp_a0_2;
            if (temp_a0_2 < 0) {
                var_f2 += D_DBL_80000800;
            }
            M2C_FIELD(&D_800588F8, f32 *, 0) = (f32) ((f32) (M2C_FIELD(&D_800588F8, s32 *, 0x30) - M2C_FIELD(arg0, s32 *, 0xFC)) / (f32) var_f2);
            temp_a0_3 = M2C_FIELD(arg0, s32 *, 0xEC);
            var_f2_2 = (f64) temp_a0_3;
            if (temp_a0_3 < 0) {
                var_f2_2 += D_DBL_80000808;
            }
            M2C_FIELD(&D_800588F8, f32 *, 4) = (f32) ((f32) (M2C_FIELD(&D_800588F8, s32 *, 0x34) - M2C_FIELD(arg0, s32 *, 0x100)) / (f32) var_f2_2);
            temp_a0_4 = M2C_FIELD(arg0, s32 *, 0xEC);
            var_f2_3 = (f64) temp_a0_4;
            if (temp_a0_4 < 0) {
                var_f2_3 += D_DBL_80000810;
            }
            M2C_FIELD(&D_800588F8, f32 *, 8) = (f32) ((f32) (M2C_FIELD(&D_800588F8, s32 *, 0x38) - M2C_FIELD(arg0, s32 *, 0x104)) / (f32) var_f2_3);
            temp_v0 = M2C_FIELD(arg0, s32 *, 0xEC);
            var_f4 = (f64) temp_v0;
            if (temp_v0 < 0) {
                var_f4 += D_DBL_80000818;
            }
            M2C_FIELD(&D_800588F8, f32 *, 0xC) = (f32) ((M2C_FIELD(&D_800588F8, f32 *, 0x3C) - M2C_FIELD(arg0, f32 *, 0x58)) / (f32) var_f4);
            temp_v0_2 = M2C_FIELD(arg0, s32 *, 0xEC);
            var_f4_2 = (f64) temp_v0_2;
            if (temp_v0_2 < 0) {
                var_f4_2 += D_DBL_80000820;
            }
            M2C_FIELD(&D_800588F8, f32 *, 0x10) = (f32) ((M2C_FIELD(&D_800588F8, f32 *, 0x40) - M2C_FIELD(arg0, f32 *, 0x5C)) / (f32) var_f4_2);
            temp_v0_3 = M2C_FIELD(arg0, s32 *, 0xEC);
            var_f4_3 = (f64) temp_v0_3;
            if (temp_v0_3 < 0) {
                var_f4_3 += D_DBL_80000828;
            }
            M2C_FIELD(&D_800588F8, f32 *, 0x14) = (f32) ((M2C_FIELD(&D_800588F8, f32 *, 0x44) - M2C_FIELD(arg0, f32 *, 0x60)) / (f32) var_f4_3);
            M2C_FIELD(arg0, u32 *, 0xF0) = 2U;
            break;
        case 2:
            if (M2C_FIELD((((M2C_FIELD(&(&D_80052F18)[D_80057D40], s32 *, 0x1C) - 2) * 0xA) + &D_80058B58), u16 *, 4) & 0x8000) {
                M2C_FIELD(arg0, s32 *, 0xEC) = 1;
            }
            temp_v0_4 = M2C_FIELD(arg0, s32 *, 0xEC);
            temp_v0_5 = temp_v0_4 - 1;
            if (temp_v0_4 != 0) {
                M2C_FIELD(arg0, s32 *, 0xEC) = temp_v0_5;
                if (temp_v0_5 != 0) {
                    M2C_FIELD(&D_800588F8, f32 *, 0x18) = (f32) (M2C_FIELD(&D_800588F8, f32 *, 0x18) + M2C_FIELD(&D_800588F8, f32 *, 0));
                    M2C_FIELD(&D_800588F8, f32 *, 0x1C) = (f32) (M2C_FIELD(&D_800588F8, f32 *, 0x1C) + M2C_FIELD(&D_800588F8, f32 *, 4));
                    M2C_FIELD(&D_800588F8, f32 *, 0x20) = (f32) (M2C_FIELD(&D_800588F8, f32 *, 0x20) + M2C_FIELD(&D_800588F8, f32 *, 8));
                    M2C_FIELD(&D_800588F8, f32 *, 0x24) = (f32) (M2C_FIELD(&D_800588F8, f32 *, 0x24) + M2C_FIELD(&D_800588F8, f32 *, 0xC));
                    M2C_FIELD(&D_800588F8, f32 *, 0x28) = (f32) (M2C_FIELD(&D_800588F8, f32 *, 0x28) + M2C_FIELD(&D_800588F8, f32 *, 0x10));
                    M2C_FIELD(&D_800588F8, f32 *, 0x2C) = (f32) (M2C_FIELD(&D_800588F8, f32 *, 0x2C) + M2C_FIELD(&D_800588F8, f32 *, 0x14));
                } else {
                    M2C_FIELD(&D_800588F8, f32 *, 0x18) = (f32) M2C_FIELD(&D_800588F8, s32 *, 0x30);
                    M2C_FIELD(&D_800588F8, f32 *, 0x1C) = (f32) M2C_FIELD(&D_800588F8, s32 *, 0x34);
                    M2C_FIELD(&D_800588F8, f32 *, 0x20) = (f32) M2C_FIELD(&D_800588F8, s32 *, 0x38);
                    M2C_FIELD(&D_800588F8, f32 *, 0x24) = (f32) M2C_FIELD(&D_800588F8, f32 *, 0x3C);
                    M2C_FIELD(&D_800588F8, f32 *, 0x28) = (f32) M2C_FIELD(&D_800588F8, f32 *, 0x40);
                    M2C_FIELD(&D_800588F8, f32 *, 0x2C) = (f32) M2C_FIELD(&D_800588F8, f32 *, 0x44);
                }
                M2C_FIELD(arg0, s32 *, 0xFC) = (s32) M2C_FIELD(&D_800588F8, f32 *, 0x18);
                M2C_FIELD(arg0, s32 *, 0x100) = (s32) M2C_FIELD(&D_800588F8, f32 *, 0x1C);
                M2C_FIELD(arg0, s32 *, 0x104) = (s32) M2C_FIELD(&D_800588F8, f32 *, 0x20);
                M2C_FIELD(arg0, f32 *, 0x58) = (f32) M2C_FIELD(&D_800588F8, f32 *, 0x24);
                M2C_FIELD(arg0, f32 *, 0x5C) = (f32) M2C_FIELD(&D_800588F8, f32 *, 0x28);
                M2C_FIELD(arg0, f32 *, 0x60) = (f32) M2C_FIELD(&D_800588F8, f32 *, 0x2C);
                func_8000FB08(arg0, M2C_FIELD(arg0, f32 *, 0x58), M2C_FIELD(arg0, f32 *, 0x5C), M2C_FIELD(arg0, f32 *, 0x60));
                if (M2C_FIELD(arg0, s32 *, 0xEC) == 0) {
                    goto block_52;
                }
            } else {
block_52:
                M2C_FIELD(arg0, u32 *, 0xF0) = 0U;
                if (func_80005598(temp_s2) != 0) {
                    func_80005500(6, temp_s2);
                    func_8000550C(3, temp_s2);
                    func_8001BED8(0);
                } else {
                    func_80005500(5, temp_s2);
                }
            }
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            func_8000FB08(arg0, M2C_FIELD(arg0, f32 *, 0x58), M2C_FIELD(arg0, f32 *, 0x5C), M2C_FIELD(arg0, f32 *, 0x60));
            break;
        case 11:
            temp_v0_6 = M2C_FIELD(arg0, void **, 0xF8);
            M2C_FIELD(arg0, f32 *, 0x58) = (f32) M2C_FIELD(temp_v0_6, f32 *, 0x2C);
            M2C_FIELD(arg0, f32 *, 0x5C) = (f32) (M2C_FIELD(temp_v0_6, f32 *, 0x30) + D_FLT_80000830);
            M2C_FIELD(arg0, f32 *, 0x60) = (f32) M2C_FIELD(temp_v0_6, f32 *, 0x34);
            break;
        case 3:
            func_8000FD38(arg0);
            break;
        case 13:
            D_800C33F4 = M2C_FIELD(arg0, s32 *, 0x104);
            /* fallthrough */
        case 14:
            temp_ret = func_80008B80(M2C_FIELD(&D_800C33D8, f32 *, 0xC), M2C_FIELD(&D_800C33D8, s32 *, 0x10), M2C_FIELD(&D_800C33D8, f32 *, 0x14), M2C_FIELD(&D_800C33D8, f32 *, 0), M2C_FIELD(&D_800C33D8, s32 *, 4), M2C_FIELD(&D_800C33D8, f32 *, 8));
            M2C_FIELD(&D_800C33D8, s32 *, 0x20) = temp_ret;
            var_v0_3 = temp_ret / 200;
            if (!((f32) var_v0_3 >= 0.0f)) {
                var_v0_3 = -var_v0_3;
            }
            M2C_FIELD(&D_800C33D8, s32 *, 0x28) = var_v0_3;
            temp_a0_5 = M2C_FIELD(arg0, s32 *, 0xFC);
            M2C_FIELD(&D_800C33D8, s32 *, 0x18) = temp_a0_5;
            temp_a2 = M2C_FIELD(arg0, s32 *, 0x104);
            M2C_FIELD(&D_800C33D8, s32 *, 0x1C) = temp_a2;
            if (var_v0_3 < 5) {
                M2C_FIELD(&D_800C33D8, s32 *, 0x28) = 5;
            }
            temp_f4 = (f32) M2C_FIELD(&D_800C33D8, s32 *, 0x28);
            M2C_FIELD(&D_800C33D8, f32 *, 0x30) = (f32) ((M2C_FIELD(&D_800C33D8, f32 *, 0) - M2C_FIELD(arg0, f32 *, 0x58)) / temp_f4);
            M2C_FIELD(&D_800C33D8, f32 *, 0x38) = (f32) ((M2C_FIELD(&D_800C33D8, f32 *, 8) - M2C_FIELD(arg0, f32 *, 0x60)) / temp_f4);
            M2C_FIELD(&D_800C33D8, f32 *, 0xC) = (f32) M2C_FIELD(arg0, f32 *, 0x58);
            M2C_FIELD(&D_800C33D8, f32 *, 0x14) = (f32) M2C_FIELD(arg0, f32 *, 0x60);
            M2C_FIELD(arg0, u32 *, 0xF0) = 0xFU;
            D_FLT_800587F0 = (s32) (((f32) D_FLT_800547C8 - M2C_FIELD(arg0, f32 *, 0x5C)) / temp_f4);
            D_80058674 = (s32) (D_800547C0 - temp_a0_5) / (s32) M2C_FIELD(&D_800C33D8, s32 *, 0x28);
            D_80058670 = (s32) (D_800547C4 - temp_a2) / (s32) M2C_FIELD(&D_800C33D8, s32 *, 0x28);
            /* fallthrough */
        case 15:
            M2C_FIELD(&D_800C33D8, s32 *, 0x20) = func_80008B80(M2C_FIELD(arg0, f32 *, 0x28), M2C_FIELD(arg0, s32 *, 0x2C), M2C_FIELD(arg0, f32 *, 0x30), M2C_FIELD(&D_800C33D8, f32 *, 0), M2C_FIELD(&D_800C33D8, s32 *, 4), M2C_FIELD(&D_800C33D8, f32 *, 8));
            temp_v1_3 = M2C_FIELD(&D_800C33D8, s32 *, 0x28) - 1;
            M2C_FIELD(&D_800C33D8, s32 *, 0x28) = temp_v1_3;
            if (temp_v1_3 <= 0) {
                M2C_FIELD(&D_800C33D8, s32 *, 0x10) = 0;
                M2C_FIELD(&D_800C33D8, f32 *, 0xC) = (f32) M2C_FIELD(&D_800C33D8, f32 *, 0);
                M2C_FIELD(&D_800C33D8, f32 *, 0x14) = (f32) M2C_FIELD(&D_800C33D8, f32 *, 8);
                M2C_FIELD(arg0, u32 *, 0xF0) = 0x10U;
            } else {
                M2C_FIELD(&D_800C33D8, f32 *, 0xC) = (f32) (M2C_FIELD(&D_800C33D8, f32 *, 0xC) + M2C_FIELD(&D_800C33D8, f32 *, 0x30));
                M2C_FIELD(&D_800C33D8, f32 *, 0x14) = (f32) (M2C_FIELD(&D_800C33D8, f32 *, 0x14) + M2C_FIELD(&D_800C33D8, f32 *, 0x38));
            }
            func_8000F34C(arg0);
            break;
        case 16:
            if (M2C_FIELD(&D_800C33D8, s32 *, 0x2C) != 0) {
                M2C_FIELD(&D_800C33D8, s32 *, 0x2C) = 0;
                M2C_FIELD(arg0, u32 *, 0xF0) = 0xEU;
            } else {
                func_8000F4E4(arg0);
            }
            break;
        case 4:
            func_8000FD9C(arg0);
            if (D_80052EE4 != 0) {
                M2C_FIELD(arg0, u32 *, 0xF0) = 0xEU;
            }
            break;
        case 17:
            func_80010760(arg0);
            break;
        case 18:
            func_80010358(arg0);
            break;
        case 19:
        case 21:
            func_800109EC(arg0, D_80057D40);
            break;
        case 20:
        case 22:
            func_80010850(arg0);
            break;
        }
    }
    func_8000F800(arg0);
}
