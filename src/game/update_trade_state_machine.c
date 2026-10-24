typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef s32 M2C_UNK;
#define M2C_FIELD(base, type, offset) (*(type)((u8 *)(base) + (offset)))

void *func_800055A4(s32);                           /* extern */
M2C_UNK func_80010E24(s32, M2C_UNK);                /* extern */
s32 func_800166B0(void *);                          /* extern */
M2C_UNK func_8001F138(M2C_UNK);                     /* extern */
M2C_UNK func_8001F1C4(s32);                         /* extern */
s32 *func_8002A628(M2C_UNK);                        /* extern */
M2C_UNK func_8002DBD8(void *);                      /* extern */
M2C_UNK func_8002DE5C(void *);                      /* extern */
M2C_UNK func_8002E0E0(void *);                      /* extern */
M2C_UNK func_8002E17C(void *);                      /* extern */
M2C_UNK func_8002E544(void *, s32);                 /* extern */
M2C_UNK func_8002E5B8(void *);                      /* extern */
M2C_UNK func_8002E654(void *);                      /* extern */
M2C_UNK func_8002E930();                            /* extern */
s32 func_8002F428(M2C_UNK);                         /* extern */
s32 func_8002F504();                                /* extern */
s32 func_8002F6D0();                                /* extern */
M2C_UNK func_8002F784();                            /* extern */
M2C_UNK func_8002FAF8();                            /* extern */
M2C_UNK func_8002FEA8(M2C_UNK);                     /* extern */
s32 func_8002FEB4();                                /* extern */
M2C_UNK func_8002FEC4(u8);                          /* extern */
s32 func_8002FED0();                                /* extern */
M2C_UNK func_8002FEE0(u8);                          /* extern */
s32 func_8002FEEC();                                /* extern */
M2C_UNK func_8002FF48(s32);                         /* extern */
s32 func_800301C8(void *, s32);                     /* extern */
extern s32 D_80057D40;
extern s32 D_800B9300;
extern s32 D_800B9304;
extern s32 D_800B9308;
extern s32 D_800B930C;
extern s32 D_800B9310;
extern s32 D_800B9314;

void func_8002EBF0(void) {
    M2C_UNK var_a0_2;
    M2C_UNK var_a0_3;
    s16 temp_a1;
    s16 var_a1;
    s16 var_v0;
    s16 var_v1;
    s16 var_v1_2;
    s32 temp_a0;
    s32 temp_s1;
    s32 temp_s1_2;
    s32 temp_s1_3;
    s32 temp_s1_4;
    s32 temp_s1_5;
    s32 temp_s2;
    s32 temp_s2_2;
    s32 temp_s2_3;
    s32 temp_v0_2;
    s32 temp_v0_7;
    s32 temp_v1;
    s32 temp_v1_3;
    s32 var_a0;
    void *temp_a2;
    void *temp_s0;
    void *temp_s0_2;
    void *temp_v0;
    void *temp_v0_3;
    void *temp_v0_4;
    void *temp_v0_5;
    void *temp_v0_6;
    void *temp_v0_8;
    void *temp_v1_2;

    temp_v0 = func_800055A4(D_80057D40);
    func_800166B0(temp_v0);
    temp_v0_2 = func_8002FEB4();
    switch (temp_v0_2) {
    case 1:
        func_8002FEC4(0U);
        func_8002FEE0(0U);
        func_8002FEA8(2);
        D_800B9314 = 0;
        temp_v0_3 = func_800055A4(D_80057D40);
        func_8001F1C4(M2C_FIELD(temp_v0_3, s32 *, 0xF8) + 0x130);
        if (M2C_FIELD(temp_v0_3, s32 *, 0x11C) == -1) {
            func_8002DBD8(temp_v0_3);
            return;
        }
    default:
        return;
    case 2:
        func_8002E930();
        temp_v0_4 = func_800055A4(D_80057D40);
        if (M2C_FIELD(temp_v0_4, s32 *, 0x11C) == -1) {
            func_8002E0E0(temp_v0_4);
        }
        func_8002F784();
        func_8002FF48(*func_8002A628(0));
        if (func_800166B0(temp_v0_4) & 0x10) {
            temp_s2 = func_8002FED0();
            temp_s1 = func_8002FEEC();
            temp_v0_5 = func_800055A4(temp_s2 & 0xFF);
            temp_s1_2 = temp_s1 & 0xFF;
            if (M2C_FIELD(temp_v0_4, s32 *, 0xF4) != M2C_FIELD(temp_v0_5, s32 *, 0xF4)) {
                if (temp_s1_2 != 4) {
                    if (func_800301C8(temp_v0_4, temp_s1_2) == 0) {
                        temp_v1 = temp_s1_2 * 2;
                        if (func_800301C8(temp_v0_5, temp_s1_2) == 0) {
                            var_v1 = M2C_FIELD((temp_v1 + temp_v0_4), s16 *, 0x1E8);
                            D_800B9304 = (s32) M2C_FIELD((temp_v1 + temp_v0_5), s16 *, 0x1E8);
                            goto block_13;
                        }
                    }
                    goto block_61;
                }
                D_800B9304 = (s32) M2C_FIELD(temp_v0_5, s16 *, 0x1F8);
                var_v1 = M2C_FIELD(temp_v0_4, s16 *, 0x1F8);
block_13:
                D_800B9300 = (s32) var_v1;
                D_800B9310 = 0;
                func_8002FEA8(3);
                return;
            }
block_61:
            func_8001F138(0x22);
            return;
        }
        break;
    case 3:
        func_8002FF48(*func_8002A628(0));
        func_8002E930();
        if (func_8002F504() != 0) {
            func_8002FEA8(4);
            if (M2C_FIELD(temp_v0, s32 *, 0x11C) == -1) {
                M2C_FIELD(temp_v0, s16 *, 0x194) = 0x28;
                return;
            }
        }
        break;
    case 5:
        func_8002FF48(*func_8002A628(0));
        func_8002FEC4(0U);
        func_8002FEE0(0U);
        func_8002FEA8(6);
        D_800B9314 = 0;
        if (M2C_FIELD(temp_v0, s32 *, 0x11C) == -1) {
            func_8002DE5C(temp_v0);
            return;
        }
        break;
    case 6:
        func_8002FF48(*func_8002A628(0));
        func_8002E930();
        if (M2C_FIELD(temp_v0, s32 *, 0x11C) == -1) {
            func_8002E0E0(temp_v0);
        }
        func_8002F784();
        if (func_800166B0(temp_v0) & 0x10) {
            temp_s2_2 = func_8002FED0();
            temp_s1_3 = func_8002FEEC();
            temp_s0 = func_800055A4(D_80057D40);
            temp_v0_6 = func_800055A4(temp_s2_2 & 0xFF);
            temp_s1_4 = temp_s1_3 & 0xFF;
            if (M2C_FIELD(temp_s0, s32 *, 0xF4) != M2C_FIELD(temp_v0_6, s32 *, 0xF4)) {
                if (temp_s1_4 != 4) {
                    if (func_800301C8(temp_s0, temp_s1_4) == 0) {
                        temp_v0_7 = temp_s1_4 * 2;
                        if (func_800301C8(temp_v0_6, temp_s1_4) == 0) {
                            temp_v1_2 = temp_v0_7 + temp_s0;
                            temp_a2 = temp_v0_7 + temp_v0_6;
                            var_a1 = M2C_FIELD(temp_v1_2, s16 *, 0x1E8);
                            var_v1_2 = M2C_FIELD(temp_a2, s16 *, 0x1E8);
                            var_a0 = M2C_FIELD(temp_v1_2, s16 *, 0x1F0) - var_a1;
                            if (var_v1_2 >= var_a0) {
                                D_800B9304 = var_a1 + var_a0;
                                var_v0 = M2C_FIELD(temp_a2, s16 *, 0x1E8);
                                goto block_30;
                            }
                            goto block_31;
                            goto block_32;
                        }
                    }
                    goto block_61;
                }
                var_a1 = M2C_FIELD(temp_s0, s16 *, 0x1F8);
                var_v1_2 = M2C_FIELD(temp_v0_6, s16 *, 0x1F8);
                var_a0 = 0x63 - var_a1;
                if (var_v1_2 >= var_a0) {
                    D_800B9304 = var_a1 + var_a0;
                    var_v0 = M2C_FIELD(temp_v0_6, s16 *, 0x1F8);
block_30:
                    D_800B9300 = var_v0 - var_a0;
                } else {
block_31:
                    D_800B9304 = var_a1 + var_v1_2;
                    D_800B9300 = 0;
                }
block_32:
                temp_a0 = M2C_FIELD(temp_v0_6, s32 *, 0x11C);
                if (temp_a0 == -1) {
                    var_a0_2 = 7;
                } else if (temp_a0 == M2C_FIELD(temp_s0, s32 *, 0x11C)) {
                    var_a0_2 = 7;
                } else {
                    func_80010E24(temp_a0, 0xF);
                    var_a0_2 = 7;
                }
block_70:
                func_8002FEA8(var_a0_2);
                D_800B9310 = 0;
                return;
            }
            goto block_61;
        }
        break;
    case 7:
        func_8002FF48(*func_8002A628(0));
        func_8002E930();
        if (func_8002F504() != 0) {
            func_8002FEA8(8);
            if (M2C_FIELD(temp_v0, s32 *, 0x11C) == -1) {
                M2C_FIELD(temp_v0, s16 *, 0x194) = 0x28;
            }
            func_8001F1C4(M2C_FIELD(temp_v0, s32 *, 0xF8) + 0x134);
            return;
        }
        break;
    case 4:
    case 8:
        func_8002FF48(*func_8002A628(0));
        func_8002E930();
        if (M2C_FIELD(temp_v0, s32 *, 0x11C) == -1) {
            func_8002E0E0(temp_v0);
        }
block_73:
        if (func_800166B0(temp_v0) & 0x10) {
            func_8002FEA8(0x17);
            return;
        }
        break;
    case 9:
        func_8002FEC4(0U);
        func_8002FEE0(0U);
        func_8002FEA8(0xA);
        return;
    case 10:
        func_8002FF48(*func_8002A628(0));
        func_8002E930();
        var_a0_3 = 0xB;
        if (func_8002F428(0xA) != 0) {
block_50:
            func_8002FEA8(var_a0_3);
            if (M2C_FIELD(temp_v0, s32 *, 0x11C) == -1) {
                M2C_FIELD(temp_v0, s16 *, 0x194) = 0x28;
            }
            func_8001F1C4(M2C_FIELD(temp_v0, s32 *, 0xF8) + 0x138);
            return;
        }
        break;
    case 12:
        func_8002FEC4(0U);
        func_8002FEE0(0U);
        func_8002FEA8(0xD);
        return;
    case 13:
        func_8002FF48(*func_8002A628(0));
        func_8002E930();
        var_a0_3 = 0xE;
        if (func_8002F428(0x14) != 0) {
            goto block_50;
        }
        break;
    case 15:
        func_8002FF48(*func_8002A628(0));
        func_8002FEC4(0U);
        func_8002FEE0(0U);
        func_8002FEA8(0x10);
        D_800B9314 = 0;
        if (M2C_FIELD(temp_v0, s32 *, 0x11C) == -1) {
            M2C_FIELD(temp_v0, s16 *, 0x194) = 0x28;
            func_8002E5B8(temp_v0);
            return;
        }
        break;
    case 16:
        func_8002FF48(*func_8002A628(0));
        func_8002E930();
        if (M2C_FIELD(temp_v0, s32 *, 0x11C) == -1) {
            func_8002E654(temp_v0);
        }
        func_8002F784();
        if (func_800166B0(temp_v0) & 0x10) {
            temp_s2_3 = func_8002FED0();
            temp_s1_5 = func_8002FEEC();
            temp_s0_2 = func_800055A4(D_80057D40);
            temp_v1_3 = temp_s1_5 & 0xFF;
            if ((M2C_FIELD(temp_s0_2, s32 *, 0xF4) != M2C_FIELD(func_800055A4(temp_s2_3 & 0xFF), s32 *, 0xF4)) || (temp_a1 = M2C_FIELD(((temp_v1_3 * 2) + temp_s0_2), s16 *, 0x1F0), (temp_a1 == -1)) || (var_a0_2 = 0x11, (temp_v1_3 == 4))) {
                goto block_61;
            }
            D_800B9304 = (s32) temp_a1;
            goto block_70;
        }
        break;
    case 17:
        func_8002FF48(*func_8002A628(0));
        func_8002E930();
        if (func_8002F6D0() != 0) {
            func_8002FEA8(0x12);
            if (M2C_FIELD(temp_v0, s32 *, 0x11C) == -1) {
                M2C_FIELD(temp_v0, s16 *, 0x194) = 0x28;
                return;
            }
        }
        break;
    case 19:
        func_8002FF48(*func_8002A628(0));
        temp_v0_8 = func_800055A4(D_80057D40);
        func_8002FEC4(M2C_FIELD(temp_v0_8, u8 *, 0xF7));
        func_8002FEE0(M2C_FIELD(func_8002A628(1), u8 *, 3));
        D_800B9308 = (s32) M2C_FIELD((((func_8002FEEC() & 0xFF) * 2) + temp_v0_8), s16 *, 0x1E8);
        D_800B930C = (s32) M2C_FIELD(temp_v0_8, s16 *, 0x1F8);
        func_8002FEA8(0x14);
        D_800B9314 = 0;
        M2C_FIELD(temp_v0_8, s16 *, 0x194) = 0x28;
        return;
    case 20:
        func_8002FF48(*func_8002A628(0));
        func_8002E930();
        if (M2C_FIELD(temp_v0, s32 *, 0x11C) == -1) {
            func_8002E544(temp_v0, func_8002FEEC() & 0xFF);
        }
        func_8002FAF8();
        var_a0_2 = 0x16;
        if (func_800166B0(temp_v0) & 0x10) {
            goto block_70;
        }
        break;
    case 11:
    case 14:
    case 18:
    case 22:
        func_8002FF48(*func_8002A628(0));
        func_8002E930();
        if (M2C_FIELD(temp_v0, s32 *, 0x11C) == -1) {
            func_8002E17C(temp_v0);
        }
        goto block_73;
    case 23:
        func_8002FF48(*func_8002A628(0));
        func_8002E930();
        break;
    }
}
