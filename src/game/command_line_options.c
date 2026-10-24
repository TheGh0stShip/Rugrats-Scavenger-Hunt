/* func_8001DF10: split the command line on spaces and set the debug
 * flags for the -d, -v, -s and -l switches. */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;


extern s32 D_80053900;
extern s32 D_80053904;
extern s32 D_80053908;
extern s32 D_8005390C;

void func_8001DF10(u8 *s) {
    u8 *argv[32];
    s32 argc = 1;
    u8 **p = argv;

    if (s == 0 || *(volatile u8 *)s == 0) {
        return;
    }
    do {
        while (*s == 0x20) {
            *s++ = 0;
        }
        if (*s == 0) {
            break;
        }
        argv[argc++] = s;
        while (*s != 0 && *s != 0x20) {
            s++;
        }
    } while (*s != 0);
    for (; argc >= 2; argc--, p++) {
        if (p[1][0] != 0x2D) {
            break;
        }
        switch (p[1][1]) {
            case 0x64:
                D_80053904 = 1;
                break;
            case 0x76:
                D_80053900 = 1;
                break;
            case 0x73:
                D_80053908 = 1;
                break;
            case 0x6C:
                D_8005390C = 1;
                break;
        }
    }
}
