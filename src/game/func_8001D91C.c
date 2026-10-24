/* Target: func_8001D91C  VRAM 0x8001D91C  ROM 0x1E51C  Size: 1444
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct {
    u8 pad_000[0xF0];
    s32 unkF0;
} Obj_80052F04;

typedef struct {
    u8 pad_000[0x100];
    s32 unk100;
} Player;

typedef struct {
    u8 pad_00[0x28];
    s32 state;
} Game_80053634;

typedef struct {
    u8 pad_0[6];
    u8 unk6;
    u8 unk7;
} Pad_8005B7F8;

typedef struct {
    s32 mode;
} Settings_80052F18;

extern u32 D_80052EE0;
extern Obj_80052F04 *D_80052F04;
extern s32 D_80057D40;
extern Player *D_8005B214;
extern Game_80053634 *D_80053634;
extern s32 D_80053638;
extern s32 D_80059D40;
extern Settings_80052F18 D_80052F18;
extern Pad_8005B7F8 D_8005B7F8;

extern s32 func_8001BF00(void);
extern Player *func_800055A4(s32);
extern void func_800121D0(void);
extern s32 func_8000B8D8(void);
extern void func_8001D1CC(void);
extern void func_8001D130(void);
extern s32 func_8001F588(s32);
extern void func_80008E20(s32);
extern void func_80008FB4(void);
extern void func_80035CFC(void);
extern void func_80034300(void);
extern void func_80035864(void);
extern void func_8001A8BC(s32);
extern void func_80016F50(void);
extern void func_80012D7C(s32);
extern void func_8001BF24(s32);
extern void func_80033E70(void);
extern void func_80034808(void);
extern void func_8001B5FC(void);
extern void func_800118F0(void);
extern void func_80031CB0(void);
extern void func_8001DEF8(void);
extern void func_8001C404(void);
extern void func_80035D0C(void);
extern void func_80035E74(void);
extern void func_8001C2AC(void);
extern void func_80022A50(Player *);
extern void func_8001A65C(void);
extern int func_800191EC(s32);
extern s32 func_80012CE8(void);
extern s32 func_8001C2C8(void);
extern void func_8001A5C0(void);
extern void func_8001BED8(s32);
extern void func_8001B800(void);
extern void func_8001BF78(void);
extern void func_8001ABC8(void);
extern void func_80034978(void);
extern void func_80036138(void);
extern void func_8001ADC8(void);
extern void func_8001B08C(void);
extern void func_800137E8(s32);
extern void func_800138FC(const char *, s32, s32);
extern void func_8001C398(void);
extern void func_8001C59C(void);
extern void func_8001C600(void);
extern void func_80011DAC(void);
extern void func_80031EDC(void);
extern void func_8001C71C(void);
extern void func_8001C77C(void);
extern void func_8001CDFC(void);
extern void func_8001C938(void);
extern void func_8001C958(void);
extern void func_8001CAE4(void);
extern void func_8001CC40(void);
extern void func_8000B2C8(void);

void func_8001D91C(void) {
    s32 state;

    D_80052EE0 |= 1;
    switch (func_8001BF00()) {
        case 4:
        case 5:
        case 10:
        case 26:
        case 34:
        case 40:
        case 41:
        case 48:
            if (D_80052F04->unkF0 == 2) {
                break;
            }
        default:
            D_80052EE0 ^= 1;
            break;
    }
    D_8005B214 = func_800055A4(D_80057D40);
    func_800121D0();
    if (func_8000B8D8()) {
        if (func_8001BF00() != 0x1F) {
            func_8001D1CC();
        }
    } else if (func_8001BF00() == 0x22) {
        func_8001D1CC();
    } else {
        func_8001D130();
    }
    func_8001F588(0);

    state = D_80053634->state;
    if (D_80059D40 == 8) {
        func_80008E20(D_80052F18.mode != 0);
        func_80008FB4();
    }
    if (state != D_80053638) {
        switch (D_80053638) {
            case 10:
            case 26:
                func_80035CFC();
                break;
            case 9:
                func_80034300();
                break;
            case 31:
                func_80035864();
                break;
        }
        D_80053638 = state;
        switch (state) {
            case 41:
                func_8001A8BC(9);
                break;
            case 40:
                func_80016F50();
                break;
            case 12:
            case 13:
                func_80012D7C(10);
                func_8001BF24(0x1B);
                break;
            case 9:
                func_80012D7C(10);
                func_8001BF24(0x1F);
                func_80033E70();
                break;
            case 31:
                func_80012D7C(10);
                func_8001BF24(0x23);
                func_80034808();
                break;
            case 5:
                D_8005B7F8.unk6 = 0;
                D_8005B7F8.unk7 = 0;
                break;
            case 4:
                func_8001B5FC();
                break;
            case 21:
                func_800118F0();
                break;
            case 24:
                func_8001BF24(0x13);
                func_80031CB0();
                func_8001DEF8();
                break;
            case 11:
            case 17:
            case 18:
            case 19:
            case 20:
                func_8001C404();
                break;
            case 10:
                func_8001BF24(0);
                func_80035D0C();
                break;
            case 26:
                func_8001BF24(0);
                func_80035E74();
                break;
            case 14:
            case 35:
                func_8001BF24(0x27);
                break;
        }
        if ((u32)(state - 0x10) < 12) {
            func_8001C2AC();
        }
    }
    if (func_8001BF00()) {
        func_80022A50(D_8005B214);
        if (D_8005B214->unk100 == 1) {
            D_8005B214->unk100 = 2;
        }
    }
    switch (D_80053634->state) {
        case 48:
            func_8001A65C();
            break;
        case 40:
            func_800191EC(0);
            break;
        case 3:
            if (func_80012CE8() && func_8001C2C8()) {
                func_8001A5C0();
                func_8001BED8(0);
            }
            break;
        case 4:
        case 41:
            func_8001B800();
            break;
        case 5:
            func_8001BF78();
            break;
        case 9:
            func_8001ABC8();
            break;
        case 31:
            func_80034978();
            break;
        case 10:
        case 26:
            func_80036138();
            break;
        case 12:
        case 13:
            func_8001ADC8();
            break;
        case 14:
        case 35:
            func_8001B08C();
            break;
        case 15:
            func_800137E8(8);
            func_800138FC("Camera Mode", 0x100, 0x3C);
            break;
        case 16:
            func_800137E8(8);
            func_800138FC("Catnap Square", 0x100, 0x3C);
            func_8001C398();
            break;
        case 11:
        case 17:
        case 18:
        case 19:
        case 20:
            func_800137E8(8);
            func_8001C59C();
            break;
        case 28:
            func_8001C600();
            break;
        case 27:
            func_800137E8(8);
        case 21:
            func_80011DAC();
            break;
        case 24:
            func_800137E8(8);
            func_80031EDC();
            break;
        case 25:
            func_8001BED8(0x18);
            break;
        case 23:
            func_800137E8(8);
            func_800138FC("Safety Square", 0x100, 0x3C);
            func_8001C71C();
            break;
        case 22:
            func_800137E8(8);
            func_800138FC("Mystery Square", 0x100, 0x3C);
            func_8001C77C();
            break;
        case 29:
            func_8001CDFC();
            break;
        case 30:
            func_8001C938();
            break;
        case 32:
            func_8001C958();
            break;
        case 33:
            func_8001CAE4();
            break;
        case 34:
            func_8001CC40();
            break;
        case 49:
        case 50:
            func_8000B2C8();
            break;
    }
}
