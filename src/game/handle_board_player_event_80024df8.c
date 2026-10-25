typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

#define FIELD(p, type, off) (*(type *)((u8 *)(p) + (off)))

extern s32 D_80052F18;
extern s32 D_800529F8;
extern u8 D_800529FB;
extern s32 D_800535E4;
extern s32 D_80057D5C;
extern u8 D_80057D68[];
extern s16 D_80057DA0;
extern u8 D_80059DB8[];

extern void func_80005500(s32, void *);
extern void *func_800055A4(s16);
extern void func_800069C8(s32, void *, s32);
extern void func_80006E30(void *);
extern void func_8000DE4C(u8, u8);
extern s16 func_8000DF14(u8, u8);
extern s16 func_8000DF40(s16);
extern s32 func_8000E008(s16);
extern s32 func_8000E108(void);
extern void func_8001BED8(s32);
extern void func_8001BF24(s32);
extern s32 func_8002411C(void *);
extern void func_800245FC(void *);
extern void func_80024BDC(void *);
extern void func_80024D14(void *, s32, s32);
extern void func_80024DAC(void *);
extern void func_80025730(void);

void func_80024DF8(void *arg)
{
    register void *player __asm__("$18") = arg;
    register s16 cell __asm__("$22");
    register s32 player_state __asm__("$16");
    s32 expected_state;
    register s32 handled __asm__("$19") = 0;
    register s32 run_default __asm__("$21") = 1;
    register s32 run_action __asm__("$20") = 1;
    register s32 update_position __asm__("$23") = 0;
    register s32 special __asm__("$17") = 0;
    register s32 finish __asm__("$fp") = special;

    if (D_80052F18 == 2) {
        func_80025730();
        return;
    }

    cell = func_8000DF40(func_8000DF14(FIELD(player, u8, 0x13D),
                                      FIELD(player, u8, 0x13C)));
    if (FIELD(player, s32, 0x118) & 0x10) {
        special = 1;
        FIELD(player, s32, 0x118) &= ~0x10;
    }
    if ((u32)(u16)(cell - 0xF9) < 4) {
        func_80006E30(player);
        goto cell_detection_done;
    } else {
        player_state = FIELD(player, s16, 0x150);
        expected_state = func_8000E108();
        if (player_state == expected_state) {
            if ((u32)(u16)(cell - 0x14) < 5) {
                func_8000DE4C(FIELD(player, u8, 0x13D),
                              FIELD(player, u8, 0x13C));
                func_80006E30(player);
            }
        } else {
            if (D_800535E4 != 0) {
                goto check_cell_range;
            }
            if (FIELD(player, u8, 0x1A8) == 0 && D_800529F8 < 5) {
                goto check_cell_range;
            }
            if (D_800529F8 != 0 && special == 0) {
                goto cell_range_done;
            }
check_cell_range:
            if ((u32)(u16)(cell - 0xF5) >= 4) {
                handled = 1;
            }
cell_range_done:
            ;
        }
    }

cell_detection_done:
    if (handled != 0) {
        if (FIELD(player, s32, 0x118) & 2) {
            func_800245FC(player);
        }
        if (FIELD(player, s32, 0x118) & 0x20) {
            finish = 1;
            FIELD(player, s32, 0x118) =
                (FIELD(player, s32, 0x118) & ~0x20) | 0x40;
        }
        if (func_8002411C(player) != 0) {
            update_position = 1;
            handled = 0;
        }

        if (handled != 0) {
            if (finish == 0) {
                s16 occupants;
                s32 i;

                occupants = 0;
                FIELD(D_80059DB8, u16, 0) = FIELD(player, u16, 0xF6);
                for (i = 0; i < D_80057D5C; i++) {
                    void *other;
                    if (i == FIELD(player, s32, 0xF4)) {
                        continue;
                    }
                    other = func_800055A4(i);
                    if (FIELD(other, u8, 0x13C) == FIELD(player, u8, 0x13C)) {
                        other = func_800055A4(i);
                        if (FIELD(other, u8, 0x13D) ==
                            FIELD(player, u8, 0x13D)) {
                            occupants++;
                            FIELD(D_80059DB8, s16, 2) = i;
                        }
                    }
                }
                if (D_80052F18 == 1 && occupants != 1) {
                    occupants = 0;
                }
                if (occupants != 0) {
                    if (D_80052F18 == 0) {
                        if (func_8000E008(FIELD(player, s16, 0x150)) != 0) {
                            handled = 0;
                            func_80024D14(player, 0x38, -1);
                            update_position = 1;
                        }
                    } else {
                        handled = 0;
                        func_80024D14(player, 0x36, 0x1F);
                        update_position = 1;
                    }
                }
            }

            if (handled != 0) {
                if (cell == 7) {
                    goto case_7;
                }
                if (cell < 8) {
                    if (cell < 5) {
                        if (cell < 3) {
                            if (cell == 1) {
                                goto case_1;
                            }
                            if (cell == 2) {
                                goto case_2;
                            }
                            goto generic_cell;
                        }
                        goto case_3_or_4;
                    }
                    if (cell == 5) {
                        goto case_5;
                    }
                    if (cell == 6) {
                        goto case_6;
                    }
                    goto generic_cell;
                }
                if (cell == 10) {
                    goto case_10;
                }
                if (cell < 11) {
                    if (cell == 8) {
                        goto case_8;
                    }
                    if (cell == 9) {
                        goto case_9;
                    }
                    goto generic_cell;
                }
                if (cell == 13) {
                    goto case_13;
                }
                __asm__("" ::: "$2");
                if (cell < 13) {
                    goto generic_cell;
                }
                if (cell >= 0xF9) {
                    goto generic_cell;
                }
                if (cell < 0xF5) {
                    goto generic_cell;
                }
                goto case_f5_f8;

case_f5_f8:
                func_8001BED8(0x1B);
                goto after_cell;
case_3_or_4:
                func_80024BDC(player);
                FIELD(player, s8, 0x1A9) = 1;
                run_default = 0;
                goto after_cell;
case_7:
                func_80024D14(player, 0x35, 0x11);
                run_default = 0;
                run_action = 0;
                goto after_cell;
case_8:
                func_80024D14(player, 0x35, 0x12);
                run_default = 0;
                run_action = 0;
                goto after_cell;
case_5:
                func_80024D14(player, 0x34, 0x13);
                run_default = 0;
                run_action = 0;
                goto after_cell;
case_6:
                func_80024D14(player, 0x34, 0x14);
                run_default = 0;
                run_action = 0;
                goto after_cell;
case_2:
                func_8001BED8(0x15);
                goto after_cell;
case_10:
                func_8001BED8(0x16);
                func_8001BF24(0x33);
                goto after_cell;
case_1:
                func_80024DAC(player);
                goto after_cell;
case_9:
                func_8001BED8(0x1A);
                goto after_cell;
case_13:
                func_80024D14(player, 0x3B, 0x17);
                run_default = 0;
                run_action = 0;
                goto after_cell;
generic_cell:
                func_80005500(0xC, player);
                run_action = 0;
after_cell:

                if (run_action != 0) {
                    func_80005500(0, player);
                }
                if (run_default != 0) {
                    func_800069C8(0, player, 0);
                }
                {
                    register s32 commit_position __asm__("$7") = 1;
                    __asm__("" : "+r"(commit_position));
                    if (commit_position != 0) {
                        FIELD(player, u8, 0x1A8) =
                            FIELD(player, u8, 0x133) - D_800529FB;
                        func_8000DE4C(FIELD(player, u8, 0x13D),
                                      FIELD(player, u8, 0x13C));
                    }
                }
                update_position = 1;
            }
        }
    }

    if (update_position != 0) {
        u8 *position;
        FIELD(player, u8, 0x13E) = D_80057D68[(D_80057DA0 - 1) * 2];
        __asm__("" ::: "memory");
        position = D_80057D68 + (D_80057DA0 - 1) * 2;
        FIELD(player, u8, 0x13F) = position[1];
    }
}
