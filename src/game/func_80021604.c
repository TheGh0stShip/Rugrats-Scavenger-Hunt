/* Target: func_80021604  VRAM 0x80021604  ROM 0x22204
 * Size: 0x94  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high (round 2 audit; UNVERIFIED)
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

void func_80021604(u32 dir, s32 *outX, s32 *outY, s32 x, s32 y, s32 *outAngle)
{
    switch (dir) {
    case 0:
    case 4:
        *outX = x;
        *outY = y + 1;
        if (outAngle != 0) {
            *outAngle = 0;
        }
        break;
    case 2:
        *outX = x;
        *outY = y - 1;
        if (outAngle != 0) {
            *outAngle = 180;
        }
        break;
    case 1:
        *outX = x + 1;
        *outY = y;
        if (outAngle != 0) {
            *outAngle = 90;
        }
        break;
    case 3:
        *outX = x - 1;
        *outY = y;
        if (outAngle != 0) {
            *outAngle = 270;
        }
        break;
    }
}
