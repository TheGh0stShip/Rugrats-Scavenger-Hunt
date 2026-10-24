/* Target: func_80011594  VRAM 0x80011594  ROM 0x12194
 * Size: 0x194  Compiler: SN GCC 2.8 (-O2)
 * Confidence: medium
 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern char *D_80052C40[];
extern s32 D_80058BC0;
extern int func_8001F588(s32 a);
extern void func_800137E8(s32 a);
extern void func_800136AC(s32 alpha);
extern void func_800136D8(s32 alpha);
extern char *func_8003ACF0(char *s, s32 c);
extern void func_800138FC(char *s, s32 x, s32 y);

void func_80011594(s32 page, s32 alpha) {
    s32 a = alpha;
    s32 i;
    char *split;

    if (a == 0) {
        return;
    }
    func_8001F588(1);
    func_800137E8(8);
    for (i = page; i < page + 12; i++) {
        split = 0;
        switch ((u8)D_80052C40[i][0]) {
            case '1':
                func_800136D8(a);
                break;
            case '0':
            case '2':
                func_800136AC(a);
                break;
            case '4':
                func_800136AC(a);
                split = func_8003ACF0(D_80052C40[i], '-');
                if (split != 0) {
                    *split = 0;
                    split++;
                }
                break;
            case '9':
                D_80058BC0 = 1;
                break;
        }
        if (split != 0) {
            s32 y = (i - page) * 15 + 40;

            func_800138FC(D_80052C40[i] + 1, 0x96, y);
            func_800138FC(split, 0x16A, y);
            split[-1] = '-';
        } else {
            func_800138FC(D_80052C40[i] + 1, 0x100, (i - page) * 15 + 40);
        }
    }
}
