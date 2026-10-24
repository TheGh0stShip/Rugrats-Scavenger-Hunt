typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;
typedef double f64;
char *func_8003ACF0(const char *s, s32 c) { u32 w, x, m; char t;
while (((u32)s & 3) != 0) { t = *s; if (t == (char)c) return (char *)s; s++; if (t == 0) goto fail; }
    m = c & 0xFF; m |= m << 8; m |= m << 16;
loop:
        w = *(u32 *)s;
        x = w ^ m;
        if ((x & 0xFF000000) == 0) return (char *)s;
        if ((w & 0xFF000000) == 0) goto fail;
        if ((x & 0x00FF0000) == 0) return (char *)s + 1;
        if ((w & 0x00FF0000) == 0) goto fail;
        if ((x & 0x0000FF00) == 0) return (char *)s + 2;
        if ((w & 0x0000FF00) == 0) goto fail;
        if ((x & 0x000000FF) == 0) return (char *)s + 3;
        s += 4;
        if ((w & 0x000000FF) != 0) goto loop;
fail: return 0; }
