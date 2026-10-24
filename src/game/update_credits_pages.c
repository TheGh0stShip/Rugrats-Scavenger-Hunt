/* Target: func_80011728  VRAM 0x80011728  ROM 0x12328
 * Size: 0x1BC  Compiler: SN GCC 2.8 (-O2)
 * Confidence: medium
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_80058BC0; /* finish requested */
extern s32 D_80058BC4; /* current page index */
extern s32 D_80058BC8; /* fade-in alpha */
extern s32 D_80058BCC; /* hold timer */
extern s32 D_80058BD0; /* state */
extern s32 D_80058BD4; /* previous page index */
extern s32 D_80058BD8; /* fade-out alpha */
extern u16 D_80058B5A; /* pad 0 raw buttons */
extern void func_80011594(s32 page, s32 alpha);

s32 func_80011728(void) {
    switch ((u32)D_80058BD0) {
        case 3:
            D_80058BD8 -= 16;
            if (D_80058BD8 < 0) {
                D_80058BD8 = 0;
            }
            if (D_80058BC8 == 0 && D_80058BD8 == 0) {
                D_80058BD0 = 2;
            }
            break;
        case 0:
            D_80058BC8 += 16;
            if (D_80058BC8 >= 0x100) {
                D_80058BC8 = 0xFF;
            }
            D_80058BD8 -= 16;
            if (D_80058BD8 < 0) {
                D_80058BD8 = 0;
            }
            if (D_80058BC8 == 0xFF && D_80058BD8 == 0) {
                D_80058BD0 = 1;
                D_80058BCC = 120;
            }
            break;
        case 1:
            if (--D_80058BCC == 0) {
                D_80058BD8 = 0xFF;
                D_80058BC8 = 0;
                D_80058BD4 = D_80058BC4;
                D_80058BC4 += 12;
                if (D_80058BC0 != 0) {
                    D_80058BD0 = 3;
                } else {
                    D_80058BD0 = 0;
                }
            }
            break;
    }
    if (D_80058BD0 == 1) {
        if (D_80058B5A & 0x8000) {
            D_80058BCC = 1;
        } else if (D_80058B5A & 0x4000) {
            D_80058BCC = 1;
            D_80058BC0 = 1;
        }
    }
    func_80011594(D_80058BD4, D_80058BD8);
    func_80011594(D_80058BC4, D_80058BC8);
    return D_80058BD0;
}
