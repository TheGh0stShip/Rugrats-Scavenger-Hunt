/* Target: func_80011098  VRAM 0x80011098  ROM 0x11C98
 * Size: 0xD4  Compiler: SN GCC 2.8 (-O2)
 * Confidence: medium
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct { u8 pad_00[0x18]; } OSMesgQueue_8005;

extern OSMesgQueue_8005 D_80058B30;
extern void *D_80058BB0[];
extern u16 D_80058970;
extern u8 D_80058940[];
extern u8 D_80052C00;
extern u8 D_80052C01;
extern u8 D_80058B84[4];
extern char D_800008E0[];

extern void func_80047EB0(OSMesgQueue_8005 *mq, void **msg, s32 count);
extern void func_80049030(s32 event, OSMesgQueue_8005 *mq, void *msg);
extern s32 func_8004DEC0(OSMesgQueue_8005 *mq, u8 *bits, void *status);
extern void func_800510F0(char *fmt, ...);
extern void func_8004DB30(OSMesgQueue_8005 *mq);

void func_80011098(void) {
    s32 i;
    s32 n;

    func_80047EB0(&D_80058B30, D_80058BB0, 1);
    D_80058970 = 5;
    func_80049030(5, &D_80058B30, &D_80058970);
    if (func_8004DEC0(&D_80058B30, &D_80052C00, D_80058940) != 0) {
        func_800510F0(D_800008E0);
    }
    i = 0;
    n = 0;
    for (; i < 4; i++) {
        if ((D_80052C00 >> i) & 1) {
            D_80058B84[n] = i;
            n++;
        }
    }
    D_80052C01 = n;
    func_8004DB30(&D_80058B30);
}
