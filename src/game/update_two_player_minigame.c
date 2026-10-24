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

void *func_800055A4();                              /* extern */
M2C_UNK func_8000B8A8(M2C_UNK);                     /* extern */
M2C_UNK func_8000B904();                            /* extern */
M2C_UNK func_8000C530();                            /* extern */
M2C_UNK func_8000CD50(s32, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK func_80010AE8(void *);                      /* extern */
void *func_800112D0(M2C_UNK, s32, M2C_UNK, M2C_UNK, s32, s32); /* extern */
s32 func_80012CE8();                                /* extern */
M2C_UNK func_80012D7C(M2C_UNK);                     /* extern */
M2C_UNK func_80012D9C(M2C_UNK);                     /* extern */
M2C_UNK func_8001380C(M2C_UNK, M2C_UNK *, M2C_UNK, M2C_UNK); /* extern */
M2C_UNK func_800139F8(M2C_UNK *, s32, M2C_UNK);     /* extern */
s32 func_800166B0(void *);                          /* extern */
void *func_800197C8(s32, f32);                      /* extern */
M2C_UNK func_80019BD4(s16);                         /* extern */
M2C_UNK func_8001A5C0();                            /* extern */
M2C_UNK func_8001BED8(M2C_UNK);                     /* extern */
M2C_UNK func_8001BF24(M2C_UNK);                     /* extern */
u32 func_8001BF3C();                                /* extern */
s32 func_8001C2C8();                                /* extern */
M2C_UNK func_8001D2F4(M2C_UNK *);                   /* extern */
M2C_UNK func_8001DEF8();                            /* extern */
M2C_UNK func_8001E6F8(M2C_UNK);                     /* extern */
M2C_UNK func_8001E718(M2C_UNK);                     /* extern */
M2C_UNK func_8001ECE4(M2C_UNK, M2C_UNK);            /* extern */
s32 func_8001EEAC();                                /* extern */
M2C_UNK func_8001EEBC();                            /* extern */
M2C_UNK func_8001EED0();                            /* extern */
M2C_UNK func_8001F1C4();                            /* extern */
M2C_UNK func_8001F30C();                            /* extern */
M2C_UNK func_80023360(void *);                      /* extern */
M2C_UNK func_8002621C(void *, M2C_UNK, M2C_UNK, void *); /* extern */
M2C_UNK func_80033E90();                            /* extern */
M2C_UNK func_80034350(void *);                      /* extern */
M2C_UNK func_80034434();                            /* extern */
M2C_UNK func_800344A8();                            /* extern */
M2C_UNK func_800345EC();                            /* extern */
M2C_UNK func_8003A57C(M2C_UNK *, M2C_UNK *, s32, M2C_UNK *); /* extern */
extern M2C_UNK D_80002F38;
extern M2C_UNK D_80002F44;
extern M2C_UNK D_80002F60;
extern M2C_UNK D_80002F70;
extern M2C_UNK D_80002F78;
extern M2C_UNK D_80002F80;
extern M2C_UNK D_80002F90;
extern s32 D_80052EE0;
extern void *D_80052F04;
extern M2C_UNK D_800535E8;
extern M2C_UNK D_80053860;
extern M2C_UNK D_80054A0C;
extern M2C_UNK D_80054A30;
extern s32 D_80054A9C;
extern s32 D_80054AA0;
extern s32 D_80054AA4;
extern s32 D_80054AA8;
extern s8 D_800581E9;
extern s32 D_800581F0;
extern M2C_UNK D_80058B58;
extern M2C_UNK D_80059DB8;
extern void *D_800C3DB0;
extern s32 D_800C3DB4;
extern s32 D_800C3E20;
extern void *D_800C3E24;
extern u32 D_800C3E28;
extern s32 D_800C3E2C;
extern void *D_800C3E30;
extern M2C_UNK D_800C3E38;
extern f32 D_FLT_80002FD4;
extern f32 D_FLT_80002FD8;

void func_80034978(void) {
    M2C_UNK sp28;
    M2C_UNK *var_s1_9;
    f32 temp_a1;
    f32 temp_f20;
    f32 temp_f8;
    f32 temp_f8_2;
    s32 temp_a1_2;
    s32 temp_t0;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v1;
    s32 temp_v1_4;
    s32 temp_v1_5;
    s32 temp_v1_6;
    s32 temp_v1_7;
    s32 temp_v1_8;
    s32 var_a1;
    s32 var_s1;
    s32 var_s1_2;
    s32 var_s1_3;
    s32 var_s1_4;
    s32 var_s1_5;
    s32 var_s1_6;
    s32 var_s1_7;
    s32 var_s1_8;
    s32 var_s2;
    s32 var_s2_2;
    s32 var_s3;
    s32 var_s3_2;
    s32 var_s4;
    s32 var_s5;
    s32 var_v1;
    u32 temp_v0;
    u32 temp_v0_2;
    u32 temp_v1_2;
    u32 temp_v1_3;
    void **var_a2;
    void **var_s0;
    void **var_s0_2;
    void **var_s0_3;
    void **var_s0_4;
    void *temp_a0;
    void *temp_a3;
    void *temp_a3_2;
    void *temp_s0;
    void *temp_s0_2;
    void *temp_t1;
    void *temp_v0_5;
    void *temp_v0_6;
    void *temp_v0_7;
    void *var_a0;
    void *var_a0_2;
    void *var_a1_2;
    void *var_a1_3;

    var_s5 = 0;
    var_s3 = -1;
    temp_v0 = func_8001BF3C();
    switch (temp_v0) {                              /* switch 1; irregular */
    case 35:                                        /* switch 1 */
        if (func_80012CE8() == 1) {
            func_8001BF24(0x24);
            func_8000B8A8(0x20000000);
            func_80012D9C(0xA);
            func_8001E718(0xA);
            func_8001EEBC();
            func_8001ECE4(0x207, 0x38);
            M2C_FIELD(D_80052F04, s32 *, 0xF0) = 0x15;
            func_80019BD4(M2C_FIELD((D_800581F0 + (D_800581E9 * 0x210)), s16 *, 0x201C));
            func_8001DEF8();
        }
    default:                                        /* switch 1 */
block_17:
        temp_v0_2 = func_8001BF3C();
        if (temp_v0_2 != 0x23) {
            if (temp_v0_2 != 0x26) {
                func_80033E90();
                func_8001D2F4(&D_800C3E38);
                switch (D_800C3E28) {               /* switch 2 */
                case 0:                             /* switch 2 */
                    if (func_8001EEAC() != 0x207) {
                        func_8001ECE4(0x207, 0x38);
                    }
                    func_800344A8();
                    func_80034434();
                    var_a1 = 0x80;
                    if (D_80054AA0 != 0) {
                        if (D_800C3DB4 != 0) {
                            var_a1 = 0x180;
                        }
                        goto block_30;
                    }
                    if (M2C_FIELD(&D_800C3DB0, u32 *, 4) == 0) {
                        func_800139F8(&D_80002F38, 0x80, 0x40);
                    }
                    if (M2C_FIELD(&D_800C3DB0, s32 *, 0x3C) == 0) {
                        var_a1 = 0x180;
block_30:
                        func_800139F8(&D_80002F38, var_a1, 0x40);
                    }
                    break;
                case 1:                             /* switch 2 */
                    func_800345EC();
                    func_800139F8(&D_80002F44, 0x100, 0x40);
                    func_800139F8(&D_80002F60, 0x100, 0x54);
                    break;
                case 2:                             /* switch 2 */
                case 3:                             /* switch 2 */
                    if (func_8001EEAC() != 0x208) {
                        func_8001ECE4(0x208, 0x38);
                    }
                    func_800345EC();
                    break;
                case 4:                             /* switch 2 */
                case 5:                             /* switch 2 */
                case 6:                             /* switch 2 */
                    var_s1 = 0;
                    func_800139F8(&D_80002F70, ((1 - D_800C3E2C) * 0x8C) + 0xBA, 0x15);
                    var_a1_2 = D_800C3E30;
                    var_a0 = D_800C3E24;
loop_36:
                    temp_v1 = M2C_FIELD(var_a0, s32 *, 0x1AC);
                    if ((temp_v1 & 0x80000000) || (temp_v1 == M2C_FIELD(var_a1_2, s32 *, 0x1AC))) {
                        var_a1_2 += 4;
                        var_s1 += 1;
                        var_a0 += 4;
                        if (var_s1 >= 4) {

                        } else {
                            goto loop_36;
                        }
                    }
                    if (D_800C3E28 == 5) {
                        func_8003A57C(&sp28, &D_80002F78, *((M2C_FIELD(D_800C3E30, s32 *, 0xF8) * 4) + &D_800535E8), &D_80002F80);
                        func_800139F8(&sp28, 0x100, 0xB4);
                    } else if (D_800C3E28 == 4) {
                        func_8001380C(0x40, &D_80002F90, 0xB4, 0xBE);
                        if (M2C_FIELD(D_80052F04, s32 *, 0x100) == (*((D_800C3E20 * 2) + &D_80059DB8) * 0x5A)) {
                            func_80034350(D_80052F04);
                        }
                    }
                    break;
                }
            }
        } else {
            ;
        }
        temp_v0_3 = D_80054AA8 - 1;
        if ((D_80054AA8 == 0) || (D_80054AA8 = temp_v0_3, (temp_v0_3 == 0))) {
            var_s1_2 = 0;
            var_s0 = &D_800C3DB0;
            do {
                temp_a0 = *var_s0;
                var_s1_2 += 1;
                if (M2C_FIELD(temp_a0, s32 *, 0x11C) == -1) {
                    func_80023360(temp_a0);
                }
                var_s0 += 0x38;
            } while (var_s1_2 < 2);
            if (D_80054AA0 != 0) {
                if (D_800C3E28 == 4) {
                    var_s1_3 = D_800C3E2C;
                } else {
                    var_s1_3 = D_800C3DB4 != 0;
                }
                temp_s0 = (var_s1_3 * 0x38) + &D_800C3DB0;
                M2C_FIELD(temp_s0, s32 *, 0xC) = (s32) (M2C_FIELD(((M2C_FIELD(M2C_FIELD(temp_s0, void **, 0), s32 *, 0x11C) * 0xA) + &D_80058B58), u16 *, 2) & 0xB);
                temp_v0_4 = func_800166B0(M2C_FIELD(temp_s0, void **, 0));
                M2C_FIELD(temp_s0, s32 *, 8) = temp_v0_4;
                if (temp_v0_4 != 0) {
                    D_80054AA8 = 0xA;
                }
            } else {
                var_s1_4 = 0;
                var_s0_2 = &D_800C3DB0;
                do {
                    var_s1_4 += 1;
                    M2C_FIELD(var_s0_2, s32 *, 8) = func_800166B0(M2C_FIELD(var_s0_2, void **, 0));
                    var_s0_2 += 0x38;
                } while (var_s1_4 < 2);
            }
            switch (D_800C3E28) {                   /* switch 3 */
            case 0:                                 /* switch 3 */
                var_s1_5 = 0;
                var_s0_3 = &D_800C3DB0;
                do {
                    if (M2C_FIELD(var_s0_3, s32 *, 4) == 0) {
                        temp_v1_2 = M2C_FIELD(var_s0_3, u32 *, 8) & 0x1C00;
                        M2C_FIELD(var_s0_3, u32 *, 8) = temp_v1_2;
                        switch (temp_v1_2) {        /* switch 4; irregular */
                        case 0x400:                 /* switch 4 */
                            M2C_FIELD(var_s0_3, s32 *, 4) = 0x1A2;
                            M2C_FIELD(var_s0_3, s32 *, 0x1C) = (s32) M2C_FIELD(&D_80054A0C, s32 *, 0);
                            M2C_FIELD(var_s0_3, s32 *, 0x20) = (s32) M2C_FIELD(&D_80054A0C, s32 *, 4);
                            M2C_FIELD(var_s0_3, s32 *, 0x24) = (s32) M2C_FIELD(&D_80054A0C, s32 *, 8);
block_73:
                            M2C_FIELD(var_s0_3, s32 *, 0x10) = (s32) M2C_FIELD(var_s0_3, s32 *, 0x1C);
                            M2C_FIELD(var_s0_3, s32 *, 0x14) = (s32) M2C_FIELD(var_s0_3, s32 *, 0x20);
                            M2C_FIELD(var_s0_3, s32 *, 0x18) = (s32) M2C_FIELD(var_s0_3, s32 *, 0x24);
                            break;
                        case 0x1000:                /* switch 4 */
                            M2C_FIELD(var_s0_3, s32 *, 4) = 0x1A3;
                            M2C_FIELD(var_s0_3, s32 *, 0x1C) = (s32) M2C_FIELD(&D_80054A0C, s32 *, 0xC);
                            M2C_FIELD(var_s0_3, s32 *, 0x20) = (s32) M2C_FIELD(&D_80054A0C, s32 *, 0x10);
                            M2C_FIELD(var_s0_3, s32 *, 0x24) = (s32) M2C_FIELD(&D_80054A0C, s32 *, 0x14);
                            goto block_73;
                        case 0x800:                 /* switch 4 */
                            M2C_FIELD(var_s0_3, s32 *, 4) = 0x1A4;
                            M2C_FIELD(var_s0_3, s32 *, 0x1C) = (s32) M2C_FIELD(&D_80054A0C, s32 *, 0x18);
                            M2C_FIELD(var_s0_3, s32 *, 0x20) = (s32) M2C_FIELD(&D_80054A0C, s32 *, 0x1C);
                            M2C_FIELD(var_s0_3, s32 *, 0x24) = (s32) M2C_FIELD(&D_80054A0C, s32 *, 0x20);
                            goto block_73;
                        }
                        if ((D_80054AA0 != 0) && (var_s1_5 == 0) && (M2C_FIELD(&D_800C3DB0, s32 *, 8) != 0) && (M2C_FIELD(M2C_FIELD(&D_800C3DB0, void **, 0x38), s32 *, 0x11C) >= 0)) {
                            func_8001F1C4(M2C_FIELD(M2C_FIELD(&D_800C3DB0, void **, 0x38), s32 *, 0xF8) + 3);
                        }
                    }
                    var_s1_5 += 1;
                    var_s0_3 += 0x38;
                } while (var_s1_5 < 2);
                if ((M2C_FIELD(&D_800C3DB0, u32 *, 4) != 0) && (M2C_FIELD(&D_800C3DB0, s32 *, 0x3C) != 0)) {
                    if (M2C_FIELD(&D_800C3DB0, u32 *, 4) == M2C_FIELD(&D_800C3DB0, s32 *, 0x3C)) {
                        func_8001F1C4(8, M2C_FIELD(&D_800C3DB0, s32 *, 0x3C), M2C_FIELD(&D_800C3DB0, u32 *, 4));
                        var_s1_6 = 0;
                        var_a2 = &D_800C3DB0;
                        D_800C3E28 = 1;
                        D_80054A9C = 6;
                        D_80054AA8 = 0x1E;
                        do {
                            M2C_FIELD(var_a2, f32 *, 0x1C) = (f32) M2C_FIELD(&D_80054A30, f32 *, 0x18);
                            M2C_FIELD(var_a2, f32 *, 0x20) = (f32) M2C_FIELD(&D_80054A30, f32 *, 0x1C);
                            M2C_FIELD(var_a2, f32 *, 0x24) = (f32) M2C_FIELD(&D_80054A30, f32 *, 0x20);
                            temp_f8 = (f32) D_80054A9C;
                            var_s1_6 += 1;
                            M2C_FIELD(var_a2, f32 *, 0x28) = (f32) (s32) ((((M2C_FIELD(var_a2, f32 *, 0x1C) - M2C_FIELD(var_a2, f32 *, 0x10)) + temp_f8) - D_FLT_80002FD4) / temp_f8);
                            M2C_FIELD(var_a2, f32 *, 0x2C) = (f32) (s32) ((((M2C_FIELD(var_a2, f32 *, 0x20) - M2C_FIELD(var_a2, f32 *, 0x14)) + temp_f8) - D_FLT_80002FD4) / temp_f8);
                            M2C_FIELD(var_a2, f32 *, 0x30) = (f32) (s32) ((((M2C_FIELD(var_a2, f32 *, 0x24) - M2C_FIELD(var_a2, f32 *, 0x18)) + temp_f8) - D_FLT_80002FD4) / temp_f8);
                            var_a2 += 0x38;
                        } while (var_s1_6 < 2);
                    } else {
                        D_800C3E28 = 2;
                        D_80054A9C = 6;
                        D_80054AA8 = 0x1E;
                        temp_v1_3 = (M2C_FIELD(&D_800C3DB0, u32 *, 4) + M2C_FIELD(&D_800C3DB0, s32 *, 0x3C)) - 0x344;
                        var_s2 = 0;
                        if (temp_v1_3 != 2) {
                            var_s1_7 = 0;
                            if (temp_v1_3 >= 3U) {
                                if (temp_v1_3 == 3) {
                                    var_s2 = 6;
                                }
                            }
                        } else {
                            var_s2 = 3;
                            var_s1_7 = 0;
                        }
                        temp_f20 = D_FLT_80002FD8;
                        var_s0_4 = &D_800C3DB0;
                        var_v1 = var_s2;
                        do {
                            temp_v0_5 = (var_v1 * 0xC) + &D_80054A30;
                            temp_a1 = M2C_FIELD(temp_v0_5, f32 *, 8);
                            M2C_FIELD(var_s0_4, f32 *, 0x1C) = (f32) M2C_FIELD(temp_v0_5, f32 *, 0);
                            M2C_FIELD(var_s0_4, f32 *, 0x20) = (f32) M2C_FIELD(temp_v0_5, f32 *, 4);
                            M2C_FIELD(var_s0_4, f32 *, 0x24) = temp_a1;
                            temp_f8_2 = (f32) D_80054A9C;
                            var_s1_7 += 1;
                            M2C_FIELD(var_s0_4, f32 *, 0x28) = (f32) (s32) ((((M2C_FIELD(var_s0_4, f32 *, 0x1C) - M2C_FIELD(var_s0_4, f32 *, 0x10)) + temp_f8_2) - temp_f20) / temp_f8_2);
                            M2C_FIELD(var_s0_4, f32 *, 0x2C) = (f32) (s32) ((((M2C_FIELD(var_s0_4, f32 *, 0x20) - M2C_FIELD(var_s0_4, f32 *, 0x14)) + temp_f8_2) - temp_f20) / temp_f8_2);
                            M2C_FIELD(var_s0_4, f32 *, 0x30) = (f32) (s32) ((((M2C_FIELD(var_s0_4, f32 *, 0x24) - M2C_FIELD(var_s0_4, f32 *, 0x18)) + temp_f8_2) - temp_f20) / temp_f8_2);
                            temp_v0_6 = func_800197C8(M2C_FIELD(var_s0_4, s32 *, 4), temp_a1);
                            M2C_FIELD(var_s0_4, s32 *, 0x34) = (s32) (temp_v0_6 + 0x2C);
                            var_s0_4 += 0x38;
                            func_8000CD50(M2C_FIELD(temp_v0_6, s32 *, 0x2C), 0, 1);
                            var_v1 = var_s1_7 + var_s2;
                        } while (var_s1_7 < 2);
                    }
                }
                break;
            case 1:                                 /* switch 3 */
                D_800C3E28 = 0;
                M2C_FIELD(&D_800C3DB0, s32 *, 0x3C) = 0;
                M2C_FIELD(&D_800C3DB0, u32 *, 4) = 0U;
                break;
            case 2:                                 /* switch 3 */
                switch (M2C_FIELD(&D_800C3DB0, u32 *, 4)) { /* switch 5; irregular */
                case 0x1A2:                         /* switch 5 */
                    if (M2C_FIELD(&D_800C3DB0, s32 *, 0x3C) != 0x1A4) {
                        D_800C3E2C = 0;
                    } else {
block_109:
                        D_800C3E2C = 1;
                    }
                    break;
                case 0x1A4:                         /* switch 5 */
                    if (M2C_FIELD(&D_800C3DB0, s32 *, 0x3C) != 0x1A3) {
                        D_800C3E2C = 0;
                    } else {
                        goto block_109;
                    }
                    break;
                case 0x1A3:                         /* switch 5 */
                    if (M2C_FIELD(&D_800C3DB0, s32 *, 0x3C) == 0x1A2) {
                        goto block_109;
                    }
                    D_800C3E2C = 0;
                    break;
                }
                temp_v0_7 = (D_800C3E2C * 0x38) + &D_800C3DB0;
                temp_v1_4 = D_800C3E2C ^ 1;
                temp_a3 = M2C_FIELD(temp_v0_7, void **, 0);
                D_800C3E20 = temp_v1_4;
                D_800C3E30 = temp_a3;
                D_800C3E24 = *((temp_v1_4 * 0x38) + &D_800C3DB0);
                func_8000C530(M2C_FIELD(temp_v0_7, s32 *, 0x34), 1, 0, temp_a3);
                func_8000CD50(*M2C_FIELD(((D_800C3E2C * 0x38) + &D_800C3DB0), s32 **, 0x34), 1, 1);
                func_8000C530(M2C_FIELD(((D_800C3E20 * 0x38) + &D_800C3DB0), s32 *, 0x34), 2, 0);
                func_8000CD50(*M2C_FIELD(((D_800C3E20 * 0x38) + &D_800C3DB0), s32 **, 0x34), 2, 1);
                D_800C3E28 = 3;
                D_80054AA8 = 0x78;
                break;
            case 3:                                 /* switch 3 */
                D_800C3E28 = 4;
                D_80054AA4 = 1;
                *((D_800C3E20 * 4) + &D_800C3E38) = -1;
                break;
            case 4:                                 /* switch 3 */
                var_s1_8 = 0;
                var_a1_3 = D_800C3E30;
                var_a0_2 = D_800C3E24;
loop_116:
                temp_v1_5 = M2C_FIELD(var_a0_2, s32 *, 0x1AC);
                if (!(temp_v1_5 & 0x80000000)) {
                    if (temp_v1_5 == M2C_FIELD(var_a1_3, s32 *, 0x1AC)) {
                        goto block_120;
                    }
                } else {
block_120:
                    var_a1_3 += 4;
                    var_s1_8 += 1;
                    var_a0_2 += 4;
                    if (var_s1_8 < 4) {
                        goto loop_116;
                    }
                }
                if (D_80054AA4 != 0) {
                    D_80054AA4 = 0;
                    temp_v1_6 = M2C_FIELD(func_800055A4(*((D_800C3E2C * 2) + &D_80059DB8), &D_80054AA4, 0x80000000), s32 *, 0xF8);
                    if (temp_v1_6 == 1) {
                        var_s4 = 0;
                    } else if (temp_v1_6 != 0) {
                        if (temp_v1_6 != 2) {
                            if (temp_v1_6 == 3) {
                                var_s4 = 2;
                            }
                        } else {
                            var_s4 = 1;
                        }
                    } else {
                        var_s4 = 0xA;
                    }
                    func_8001F1C4(var_s4);
                    D_80052EE0 &= ~2;
                    M2C_FIELD(D_80052F04, s32 *, 0x104) = 0xC80;
                    M2C_FIELD(D_80052F04, s32 *, 0xF4) = 1;
                    M2C_FIELD(D_80052F04, s32 *, 0x100) = (s32) (*((D_800C3E20 * 2) + &D_80059DB8) * 0x5A);
                }
                if (var_s1_8 == 4) {
                    var_s3_2 = 0;
                    var_s2_2 = 0;
                    var_s1_9 = &D_80053860;
                    M2C_FIELD(D_80052F04, s32 *, 0x100) = (s32) (*((D_800C3E2C * 2) + &D_80059DB8) * 0x5A);
                    do {
                        var_s2_2 += 5;
                        temp_a1_2 = M2C_FIELD(var_s1_9, s32 *, 0x78);
                        var_s1_9 += 4;
                        var_s3_2 += 1;
                        temp_s0_2 = func_800112D0(0x1AB, temp_a1_2, -0x168, -0xFA0, 0, var_s2_2);
                        M2C_FIELD(temp_s0_2, void **, 0xE0) = func_800055A4(*((D_800C3E2C * 2) + &D_80059DB8));
                    } while (var_s3_2 < 6);
                    D_80054AA8 = 0x3C;
                    D_800C3E28 = 5;
                } else {
                    temp_v1_7 = M2C_FIELD(((D_800C3E2C * 0x38) + &D_800C3DB0), s32 *, 8);
                    if (temp_v1_7 & 0x1000) {
                        var_s3 = 0;
                    } else if (temp_v1_7 & 0x800) {
                        var_s3 = 1;
                    } else if (temp_v1_7 & 0x2000) {
                        var_s3 = 2;
                    } else if (temp_v1_7 & 0x400) {
                        var_s3 = 3;
                    }
                    if (var_s3 >= 0) {
                        temp_v1_8 = var_s3 * 4;
                        temp_t1 = temp_v1_8 + D_800C3E24;
                        temp_t0 = M2C_FIELD(temp_t1, s32 *, 0x1AC);
                        if (temp_t0 >= 0) {
                            temp_a3_2 = temp_v1_8 + D_800C3E30;
                            if (M2C_FIELD(temp_a3_2, s32 *, 0x1AC) & 0x80000000) {
                                M2C_FIELD(temp_t1, s32 *, 0x1AC) = (s32) (temp_t0 | 0x80000000);
                                var_s5 = 1;
                                M2C_FIELD(temp_a3_2, s32 *, 0x1AC) = (s32) (M2C_FIELD(temp_a3_2, s32 *, 0x1AC) & 0x7FFFFFFF);
                                func_8002621C(D_800C3E24, 6, 0x80000000, temp_a3_2);
                            }
                        }
                    }
                    if (var_s5 != 0) {
                        D_800C3E28 = 6;
                        func_8001F30C();
                    }
                }
                break;
            }
            if ((func_8001BF3C() == 0) && ((u32) (D_800C3E28 - 5) < 2U) && ((func_8001C2C8() != 0) || (M2C_FIELD(*((D_800C3E2C * 0x38) + &D_800C3DB0), s32 *, 0x18C) & 0x10))) {
                func_8001BF24(0x25);
                func_80012D7C(0xA);
                func_8001E6F8(0xA);
            }
        }
        return;
    case 36:                                        /* switch 1 */
        if (func_80012CE8() == 1) {
            func_8001BF24(0);
            if ((M2C_FIELD(M2C_FIELD(&D_800C3DB0, void **, 0), s32 *, 0x11C) >= 0) && (D_80054AA0 != 0)) {
                func_8001F1C4(M2C_FIELD(M2C_FIELD(&D_800C3DB0, void **, 0), s32 *, 0xF8) + 3);
            }
        }
        goto block_17;
    case 37:                                        /* switch 1 */
        if (func_80012CE8() == 1) {
            func_8001BF24(0x26);
            func_8000B904();
            func_80012D9C(0xA);
            func_80010AE8(D_80052F04);
            func_8001EED0();
            func_8001E718(0xA);
        }
        goto block_17;
    case 38:                                        /* switch 1 */
        if (func_80012CE8() == 1) {
            func_8001BED8(0);
            func_8001BF24(0);
            func_8001A5C0();
            return;
        }
        goto block_17;
    }
}
