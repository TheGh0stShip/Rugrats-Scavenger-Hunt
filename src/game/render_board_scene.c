typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;
typedef s32 M2C_UNK;
typedef u32 M2C_UNK32;
#define M2C_FIELD(base, type, offset) (*(type)((u8 *)(base) + (offset)))
#define NULL ((void *)0)

M2C_UNK func_80005544();                            /* extern */
M2C_UNK func_80006AE0(f32 *, M2C_UNK, M2C_UNK, f32); /* extern */
f32 func_80006D00(M2C_UNK, M2C_UNK, s32);           /* extern */
s32 func_8000B8C4();                                /* extern */
s32 func_8000B8D8();                                /* extern */
s32 func_8000B8EC();                                /* extern */
M2C_UNK func_8000B910();                            /* extern */
M2C_UNK func_8000C034();                            /* extern */
M2C_UNK func_8000C3AC(s32, f32 *);                  /* extern */
M2C_UNK func_8000C530(u16 *, M2C_UNK, M2C_UNK);     /* extern */
M2C_UNK func_8000C674(u16 *);                       /* extern */
M2C_UNK func_8000CD50(s32, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK func_8000CF10(s32, f32, f32, f32, f32, f32, M2C_UNK32, f32); /* extern */
M2C_UNK func_8000DCC8(u8, u8, s32 *, s32 *);        /* extern */
void *func_8000DD88(u8);                            /* extern */
s32 func_8000E078(u8, u8);                          /* extern */
M2C_UNK func_80010B0C();                            /* extern */
s32 func_80010BD8(s32, s32, s32 *);                 /* extern */
M2C_UNK func_80011D58(s32);                         /* extern */
void *func_800197C8(s16);                           /* extern */
M2C_UNK func_80019BD4(s32);                         /* extern */
s32 func_8001E630();                                /* extern */
M2C_UNK func_8001F138(M2C_UNK);                     /* extern */
s32 func_80025FC4(void *, M2C_UNK);                 /* extern */
M2C_UNK func_80027880();                            /* extern */
s32 func_80027978(s16);                             /* extern */
M2C_UNK func_8002A154();                            /* extern */
s32 func_8003A840(u16 *);                           /* extern */
extern void *D_800529F0;
extern s32 D_80052EE4;
extern s32 D_80052F04;
extern s32 D_80052F18;
extern M2C_UNK D_80054208;
extern f32 D_80054330;
extern s32 D_80054334;
extern s16 D_80057DA0;
extern M2C_UNK D_800581C0;
extern s8 D_800581E8;
extern u8 D_800581E9;
extern s32 D_800581F0;
extern M2C_UNK D_80059CF0;
extern M2C_UNK D_80059D48;
extern M2C_UNK D_8006FBE0;
extern u16 D_8006FCE8;
extern f32 D_FLT_800020E0;
extern M2C_UNK32 D_FLT_800020E4;
extern f32 D_FLT_800020E8;
extern M2C_UNK32 D_FLT_800020EC;
extern f32 D_FLT_800020F0;
extern f32 D_FLT_800020F4;
extern f32 D_FLT_800020F8;
extern f32 D_FLT_800020FC;
extern f32 D_FLT_80002100;
extern f32 D_FLT_80002104;
extern f32 D_FLT_80002108;
extern f32 D_FLT_8000210C;
extern f32 D_FLT_80002110;
extern f32 D_FLT_80002114;
extern f32 D_FLT_80002118;
extern f32 D_FLT_8000211C;
extern f32 D_FLT_80002120;
extern f32 D_FLT_80002124;
extern f32 D_FLT_80002128;
extern f32 D_FLT_80054324;
extern s32 D_FLT_80054328;
extern f32 D_FLT_800B92A8;

void func_80026824(void) {
    s32 sp54;
    s32 sp50;
    s32 sp4C;
    s32 sp48;
    s32 sp44;
    s32 sp40;
    void *sp3C;
    s32 sp38;
    s32 sp34;
    s32 sp30;
    f32 sp28;
    M2C_UNK32 sp24;
    f32 sp20;
    M2C_UNK var_a1;
    M2C_UNK var_a1_2;
    M2C_UNK32 temp_f0;
    M2C_UNK32 temp_f0_2;
    f32 *var_a2_2;
    f32 temp_f0_3;
    f32 temp_f0_4;
    f32 temp_f0_5;
    f32 temp_f22;
    f32 temp_f2;
    f32 temp_f4;
    f32 var_f0;
    s16 temp_a0_2;
    s16 temp_a0_4;
    s16 temp_s0;
    s16 temp_t1;
    s16 temp_v0_8;
    s16 temp_v1;
    s16 temp_v1_2;
    s16 temp_v1_3;
    s16 temp_v1_4;
    s16 var_a0;
    s16 var_fp;
    s16 var_s2_2;
    s16 var_s7;
    s32 temp_a0;
    s32 temp_a0_3;
    s32 temp_ret;
    s32 temp_s3;
    s32 temp_s3_2;
    s32 temp_s4;
    s32 temp_s4_2;
    s32 temp_s6;
    s32 temp_v0_10;
    s32 temp_v0_2;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v0_6;
    s32 temp_v0_7;
    s32 temp_v1_5;
    s32 temp_v1_6;
    s32 var_a2;
    s32 var_s3;
    s32 var_s4;
    s32 var_s4_2;
    s32 var_s4_3;
    s32 var_s4_4;
    s32 var_s5_2;
    s32 var_s5_3;
    s32 var_v0;
    s32 var_v0_3;
    s32 var_v0_4;
    s32 var_v1;
    u16 *temp_s2;
    u16 *temp_s2_2;
    u16 *temp_s2_3;
    u16 *var_a0_2;
    u16 *var_a0_3;
    u16 *var_a0_4;
    u16 *var_s2;
    u16 *var_s5;
    u16 *var_s6;
    u32 var_s0;
    u32 var_s1;
    u32 var_v0_2;
    void *temp_v0;
    void *temp_v0_3;
    void *temp_v0_9;
    void *var_s1_2;

    var_s5 = NULL;
    var_s6 = NULL;
    sp3C = NULL;
    sp4C = 0;
    if (func_8000B8C4() == 0) {
        if (D_80054334 == 0) {
            D_80054334 = 0x56;
        }
        if (M2C_FIELD(&D_80052F18, s32 *, 0) != 2) {
            sp3C = func_8000DD88(D_800581E9);
        }
        if (func_8000B8D8() != 0) {
            if (M2C_FIELD(&D_80052F18, s32 *, 0) == 0) {
                var_a0 = 0x54;
                goto block_10;
            }
            var_s2 = NULL;
            if (M2C_FIELD(&D_80052F18, s32 *, 0) == 1) {
                var_a0 = 0x55;
block_10:
                var_s4 = 0x20000000;
                var_s2 = func_800197C8(var_a0) + 0x2C;
            } else {
                var_s4 = 0x20000000;
            }
            var_fp = 0;
            sp44 = 0;
            sp48 = 0;
            sp40 = 0;
        } else {
            func_80019BD4(0x54);
            func_80019BD4(0x55);
            temp_s3 = M2C_FIELD(&D_80052F18, s32 *, 4) + ((M2C_FIELD(&D_80052F18, s32 *, 0) * 3) + 0xE8);
            if (func_8000B8C4() == 0) {
                var_s4 = 0;
                if (M2C_FIELD(&D_80052F18, s32 *, 0) == 2) {
                    var_fp = 0;
                    sp44 = 0;
                    sp48 = 0;
                    sp40 = 0;
                    var_s7 = (s8) D_800581E9 + 0x56;
                    var_s2 = func_800197C8(var_s7) + 0x2C;
                    if (D_800581E8 >= 0) {
                        var_s7 = D_800581E8 + 0x5B;
                        temp_a0 = *((D_800581E8 * 4) + &D_800581C0);
                        var_s5 = func_800197C8(var_s7) + 0x2C;
                        if (temp_a0 < 2) {
                            var_s7 = temp_a0 + 0x5F;
                            var_s6 = func_800197C8(var_s7) + 0x2C;
                        }
                    }
                } else {
                    var_s7 = M2C_FIELD((D_800581F0 + ((s8) D_800581E9 * 0x210)), s16 *, 0x201C);
                    temp_v0 = func_800197C8(var_s7);
                    temp_t1 = M2C_FIELD(sp3C, s16 *, 0);
                    sp44 = (s32) temp_t1;
                    var_s2 = temp_v0 + 0x2C;
                    var_fp = M2C_FIELD(sp3C, s16 *, 2);
                    sp48 = temp_t1 + M2C_FIELD(sp3C, s16 *, 4);
                    sp40 = var_fp + M2C_FIELD(sp3C, s16 *, 6);
                }
                func_80019BD4(temp_s3);
                if (var_s7 != D_80054334) {
                    func_80019BD4(D_80054334);
                    D_80054334 = (s32) var_s7;
                }
            } else {
                var_s2 = func_800197C8((s16) temp_s3) + 0x2C;
                var_s4 = 0x10000000;
                var_fp = 0;
                sp44 = 0;
                sp48 = 0x40;
                sp40 = 0x40;
            }
        }
        sp20 = 0.0f;
        sp24 = 0;
        sp28 = 0.0f;
        var_s3 = func_8001E630();
        temp_f22 = D_FLT_800020E0;
        func_8000CF10(var_s3, 0.0f, 0.0f, 0.0f, temp_f22, 0.0f, 0, 0.0f);
        if (var_s2 != NULL) {
            if (func_8000B8EC(&D_80052F18) == 0) {
                var_s4_2 = var_s4 | 0x08000000;
                var_a0_2 = var_s2;
                goto block_29;
            }
            temp_s4 = var_s4 | 0x08000000;
            func_8000C034(var_s2, M2C_FIELD(&D_80059D48, f32 *, 0), M2C_FIELD(&D_80059D48, M2C_UNK32 *, 4), M2C_FIELD(&D_80059D48, f32 *, 8), &D_80059CF0, var_s3, temp_s4);
            if (var_s5 != NULL) {
                func_8000B910(var_s5, &sp20, D_80052F04 + 0x28, &D_80059CF0, var_s3, temp_s4);
            }
            if (var_s6 != NULL) {
                temp_f0 = D_FLT_800020E4;
                sp20 = 0.0f;
                sp28 = 0.0f;
                sp24 = temp_f0;
                var_s4_2 = temp_s4 | 0x08000000;
                var_s3 = func_8001E630(&D_80052F18);
                func_8000CF10(var_s3, 0.0f, 0.0f, 0.0f, temp_f22, sp20, temp_f0, sp28);
                var_a0_2 = var_s6;
block_29:
                func_8000B910(var_a0_2, &sp20, D_80052F04 + 0x28, &D_80059CF0, var_s3, var_s4_2);
            }
        }
        var_s4_3 = 0;
        if (M2C_FIELD(&D_80052F18, s32 *, 0) == 2) {
            if ((s8) D_800581E9 == 0) {
                D_80054330 += (f32) D_FLT_80054328;
                func_80006AE0(&sp20, 0, 0, func_80006D00(0, 0, M2C_FIELD(&D_800581C0, s32 *, 0x10) & 0x7FFF) + D_FLT_800020E8);
                temp_f0_2 = D_FLT_800020EC;
                sp24 = temp_f0_2;
                temp_v0_2 = func_8001E630();
                func_8000CF10(temp_v0_2, (f32) ((s32) (D_80054330 + D_FLT_800020F0) % 360), D_80054330, (f32) ((s32) (D_80054330 + D_FLT_800020F8) % 360), D_FLT_800020F4, sp20, temp_f0_2, sp28);
                temp_v0_3 = func_800197C8(0x1F);
                func_8000B910(temp_v0_3 + 0x2C, &sp20, D_80052F04 + 0x28, &D_80059CF0, temp_v0_2, 0);
                if (M2C_FIELD(&D_800581C0, s32 *, 0x10) & 0x8000) {
                    func_8000CD50(M2C_FIELD(temp_v0_3, s32 *, 0x2C), 0, 1);
                }
                sp28 = 0.0f;
                sp24 = 0;
                sp20 = 0.0f;
                func_8000C3AC(func_800197C8(0x1E4) + 0x2C, &sp20);
                temp_v0_4 = func_8001E630();
                func_8000CF10(temp_v0_4, 0.0f, 0.0f, 0.0f, D_FLT_80054324, sp20, sp24, sp28);
                temp_s2 = func_800197C8(0x1E4) + 0x2C;
                func_8000B910(temp_s2, &sp20, D_80052F04 + 0x28, &D_80059CF0, temp_v0_4, 0);
                func_8000C674(temp_s2);
                temp_s2_2 = func_800197C8(0x1E5) + 0x2C;
                temp_v0_5 = func_8001E630();
                func_8000CF10(temp_v0_5, 0.0f, 0.0f, 0.0f, D_FLT_80054324, M2C_FIELD(&D_FLT_800B92A8, f32 *, 0), M2C_FIELD(&D_FLT_800B92A8, M2C_UNK32 *, 4), M2C_FIELD(&D_FLT_800B92A8, f32 *, 8));
                func_8000B910(temp_s2_2, &D_FLT_800B92A8, D_80052F04 + 0x28, &D_80059CF0, temp_v0_5, 0);
                func_8002A154();
            }
            func_80027880();
            return;
        }
        var_s5_2 = sp44;
        if (var_s5_2 < sp48) {
            do {
                var_s2_2 = var_fp;
                if (var_s2_2 < sp40) {
                    temp_s6 = var_s5_2 - sp44;
                    temp_v0_6 = temp_s6 * 0x258;
                    sp50 = temp_v0_6;
                    sp54 = temp_v0_6 + 0x12C;
                    do {
                        temp_a0_2 = M2C_FIELD(((var_s5_2 << 7) + D_800581F0 + (var_s2_2 * 2)), s16 *, 0x12);
                        if (temp_a0_2 != 0) {
                            var_s0 = temp_a0_2 & 0x7FFF;
                            var_s1 = var_s0 - 0xF9;
                            if ((u32) (var_s0 - 0x14) < 5U) {
                                var_v0 = var_s1 < 4U;
                                if (M2C_FIELD(&D_80052F18, s32 *, 0) == 0) {
                                    var_v0 = var_s1 < 4U;
                                    if (M2C_FIELD(&D_80052F18, s32 *, 4) == 1) {
                                        var_s0 = *((var_s0 * 4) + &D_80054208);
                                        var_s1 = var_s0 - 0xF9;
                                        goto block_44;
                                    }
                                }
                            } else {
block_44:
                                var_v0 = var_s1 < 4U;
                            }
                            if (var_v0 != 0) {
                                if (D_800529F0 != NULL) {
                                    if (func_80025FC4(D_800529F0, 0xF) != 0) {
                                        sp4C = 1;
                                        var_s0 = var_s1 + 0xF;
                                        goto block_53;
                                    }
                                    goto block_50;
                                }
block_50:
                                var_v0_2 = var_s0 - 0xF5;
                                if (M2C_FIELD(&D_80052F18, s32 *, 0) == 1) {
                                    var_s0 = 0xFE;
                                    if (var_s7 == 0x52) {
                                        var_s0 = 0xFF;
                                    }
                                    goto block_53;
                                }
                            } else {
block_53:
                                var_v0_2 = var_s0 - 0xF5;
                            }
                            if (var_v0_2 < 4U) {
                                var_s0 = 0xE;
                            }
                            if ((temp_a0_2 & 0x8000) && (var_s0 != 0x13) && (M2C_FIELD(&D_80052F18, s32 *, 8) != 0)) {
                                var_s0 = 0xC;
                            }
                            temp_a0_3 = var_s2_2 - var_fp;
                            sp38 = temp_a0_3;
                            sp34 = temp_s6;
                            if (var_s0 >= 0x27U) {
                                sp20 = (f32) (temp_a0_3 * 0x258);
                                sp28 = (f32) sp50;
                            } else {
                                sp20 = (f32) ((temp_a0_3 * 0x258) + 0x12C);
                                sp28 = (f32) sp54;
                            }
                            sp24 = 0;
                            func_8000CF10(func_8001E630((s32 *) temp_a0_3), 0.0f, 0.0f, 0.0f, D_FLT_800020FC, sp20, sp24, sp28);
                            if (var_s0 >= 0x27U) {
                                var_s4_3 |= 0x40000000;
                                var_a0_3 = func_800197C8((s16) var_s0) + 0x2C;
                            } else {
                                temp_v1 = M2C_FIELD((D_800581F0 + ((s8) D_800581E9 * 0x210)), s16 *, 0x201C);
                                if (temp_v1 < 0x53) {
                                    if (temp_v1 >= 0x51) {
                                        if (var_s0 < 0xFU) {
                                            var_s0 += 0x18;
                                        }
                                    }
                                }
                                var_s4_3 |= 0x40000000;
                                var_a0_3 = (var_s0 * 0x18) + &D_8006FBE0;
                            }
                            func_8000C034(var_a0_3, sp20, sp24, sp28, &D_80059CF0);
                        }
                        var_s2_2 += 1;
                    } while (var_s2_2 < sp40);
                }
                var_s5_2 += 1;
            } while (var_s5_2 < sp48);
        }
        if ((M2C_FIELD((D_800581F0 + ((s8) D_800581E9 * 0x210)), s16 *, 0x201C) == 0x4B) && (D_800529F0 != NULL) && (func_80025FC4(D_800529F0, 0xF) != 0)) {
            temp_f0_3 = D_FLT_80002100;
            sp20 = temp_f0_3;
            sp28 = D_FLT_80002104;
            temp_v0_7 = func_8001E630();
            func_8000CF10(temp_v0_7, 0.0f, 0.0f, 0.0f, D_FLT_80002108, temp_f0_3, sp24, sp28);
            func_8000C034(&D_8006FCE8, sp20, sp24, sp28, &D_80059CF0, temp_v0_7, var_s4_3 | 0x40000000);
        }
        if (D_80052EE4 == 0) {
            if (func_8000B8EC() != 0) {
                var_s5_3 = 0;
                func_80005544();
                func_80010B0C();
                if (M2C_FIELD(sp3C, s16 *, 0xA) > 0) {
                    var_s1_2 = sp3C;
                    do {
                        temp_s0 = M2C_FIELD(var_s1_2, s16 *, 0x14);
                        if ((u32) (temp_s0 - 0x106) < 4U) {
                            if (sp4C != 0) {
                                goto block_87;
                            }
                        } else if (((u32) (temp_s0 - 0x10A) >= 0xCU) || (sp4C == 0)) {
block_87:
                            temp_a0_4 = M2C_FIELD(var_s1_2, s16 *, 0x10);
                            temp_v0_8 = M2C_FIELD(var_s1_2, s16 *, 0x12);
                            sp24 = 0;
                            sp38 = (s32) temp_v0_8;
                            temp_f2 = (f32) ((temp_v0_8 * 0x258) + 0x12C);
                            temp_f4 = (f32) ((temp_a0_4 * 0x258) + 0x12C);
                            sp34 = (s32) temp_a0_4;
                            sp20 = temp_f2;
                            sp28 = temp_f4;
                            if (((u32) temp_s0 < 0x100U) && ((u32) temp_s0 >= 0xFEU)) {
                                sp20 = temp_f2 - D_FLT_8000210C;
                                sp28 = temp_f4 - D_FLT_8000210C;
                            }
                            temp_s3_2 = func_8001E630((s32 *) temp_a0_4);
                            temp_v1_2 = M2C_FIELD(var_s1_2, s16 *, 0x16);
                            if (temp_v1_2 == 0x5A) {
                                D_80054330 = D_FLT_80002110;
                            } else if (temp_v1_2 == 0x10E) {
                                D_80054330 = D_FLT_80002114;
                            } else {
                                D_80054330 = (f32) temp_v1_2;
                            }
                            func_8000CF10(temp_s3_2, 0.0f, D_80054330, 0.0f, D_FLT_80002118, sp20, sp24, sp28);
                            temp_v0_9 = func_800197C8(temp_s0);
                            var_a0_4 = M2C_FIELD(temp_v0_9, u16 **, 0x2C);
                            temp_s2_3 = temp_v0_9 + 0x2C;
                            temp_s4_2 = ((M2C_FIELD(var_a0_4, u16 *, 0) & 0x2000) == 0) << 0x1B;
                            if (temp_s0 == 0x144) {
                                temp_v1_3 = M2C_FIELD(temp_s2_3, s16 *, 8);
                                if (temp_v1_3 == 1) {
                                    var_a0_4 = temp_s2_3;
                                    if (M2C_FIELD(M2C_FIELD(var_a0_4, void **, 0x18), s32 *, 8) & 1) {
                                        var_a1 = 2;
                                        goto block_103;
                                    }
                                    goto block_100;
                                }
block_100:
                                if ((temp_v1_3 == 3) && (var_a0_4 = temp_s2_3, ((M2C_FIELD(M2C_FIELD(M2C_FIELD(temp_v0_9, u16 **, 0x2C), void **, 0x18), s32 *, 8) & 1) != 0))) {
                                    var_a1 = 0;
block_103:
                                    func_8000C530(var_a0_4, var_a1, 0);
                                    func_8001F138(0x36);
                                } else {
                                    temp_ret = func_8003A840(var_a0_4);
                                    if (((temp_ret / 750) * 0x2EE) == (temp_ret - 1)) {
                                        temp_v1_4 = M2C_FIELD(temp_s2_3, s16 *, 8);
                                        if (temp_v1_4 == 0) {
                                            var_a1_2 = 1;
                                            goto block_109;
                                        }
                                        if (temp_v1_4 == 2) {
                                            var_a1_2 = 3;
block_109:
                                            func_8000C530(temp_s2_3, var_a1_2, 0);
                                        }
                                    }
                                }
                            }
                            if ((func_80010BD8((s32) sp20, (s32) sp28, &sp30) != 0) && (func_80027978(temp_s0) == 0)) {
                                var_a2 = sp30;
                                var_s4_4 = temp_s4_2 | 0x04000000;
                                if (!((f32) var_a2 >= 0.0f)) {
                                    var_a2 = -var_a2;
                                }
                                if ((u32) (var_a2 - 0x10) >= 0x1DU) {
                                    var_v1 = var_a2 - 0xF;
                                    if (var_a2 < 0x10) {
                                        var_v0_3 = var_v1 * 0x10;
                                        if (!((f32) var_v1 >= 0.0f)) {
                                            var_v0_4 = 0xF;
                                            goto block_120;
                                        }
                                    } else {
                                        var_v1 = var_a2 - 0x2D;
                                        var_v0_3 = var_v1 * 0x10;
                                        if (!((f32) var_v1 >= 0.0f)) {
                                            var_v0_4 = 0x2D;
block_120:
                                            var_v0_3 = var_v0_4 - var_a2;
                                            var_v1 = var_v0_3 * 0x10;
                                        }
                                    }
                                    M2C_FIELD(temp_s2_3, s8 *, 0x14) = 0xFF;
                                    M2C_FIELD(temp_s2_3, s8 *, 0x16) = 0xFF;
                                    M2C_FIELD(temp_s2_3, s8 *, 0x15) = 0xFF;
                                    M2C_FIELD(temp_s2_3, s8 *, 0x17) = (s8) (var_v1 + var_v0_3);
                                    goto block_125;
                                }
                            } else {
                                if (temp_s0 == 0x137) {
                                    var_s4_4 = temp_s4_2 & 0xFBFFFFFF;
                                    var_a2_2 = &sp20;
                                } else {
                                    var_s4_4 = temp_s4_2 & 0xFBFFFFFF;
block_125:
                                    var_a2_2 = D_80052F04 + 0x28;
                                }
                                func_8000B910(temp_s2_3, &sp20, var_a2_2, &D_80059CF0);
                                func_8000C674(temp_s2_3);
                            }
                            if (temp_s0 == 0xEE) {
                                func_80011D58(var_s4_4);
                            }
                        }
                        var_s5_3 += 1;
                        var_s1_2 += 0x10;
                    } while (var_s5_3 < M2C_FIELD(sp3C, s16 *, 0xA));
                }
            }
        }
        if ((M2C_FIELD(D_800529F0, u8 *, 0x13E) != 0) && (D_80057DA0 == 0) && (func_80025FC4(D_800529F0, 0xE) == 0) && (func_8000E078(M2C_FIELD(D_800529F0, u8 *, 0x13F), M2C_FIELD(D_800529F0, u8 *, 0x13E)) == (s8) D_800581E9)) {
            func_8000DCC8(M2C_FIELD(D_800529F0, u8 *, 0x13F), M2C_FIELD(D_800529F0, u8 *, 0x13E), &sp34, &sp38);
            temp_v1_5 = M2C_FIELD(D_800529F0, u8 *, 0x13E) - M2C_FIELD(D_800529F0, u8 *, 0x13C);
            if (temp_v1_5 != 0) {
                var_f0 = D_FLT_8000211C;
                if (temp_v1_5 >= 0) {
                    D_80054330 = D_FLT_80002120;
                } else {
                    goto block_142;
                }
            } else {
                temp_v1_6 = M2C_FIELD(D_800529F0, u8 *, 0x13F) - M2C_FIELD(D_800529F0, u8 *, 0x13D);
                if (temp_v1_6 != 0) {
                    if (temp_v1_6 < 0) {
                        D_80054330 = 0.0f;
                    } else {
                        var_f0 = D_FLT_80002124;
block_142:
                        D_80054330 = var_f0;
                    }
                }
            }
            sp24 = 0;
            temp_f0_4 = (f32) ((sp38 * 0x258) + 0x12C);
            sp20 = temp_f0_4;
            temp_f0_5 = (f32) ((sp34 * 0x258) + 0x12C);
            sp28 = temp_f0_5;
            temp_v0_10 = func_8001E630((s32 *) sp34);
            func_8000CF10(temp_v0_10, 0.0f, D_80054330, 0.0f, D_FLT_80002128, temp_f0_4, sp24, temp_f0_5);
            func_8000B910(func_800197C8(0x1E2) + 0x2C, &sp20, D_80052F04 + 0x28, &D_80059CF0, temp_v0_10, 0);
        }
    }
}
