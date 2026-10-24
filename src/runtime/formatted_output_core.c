/* Target: func_800398E0  VRAM 0x800398E0  ROM 0x3A4E0
 * Size: 0xC9C  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;
typedef unsigned long long u64;
typedef double f64;
typedef char *va_list;

#define VA_WORD(list, type)                                                  \
    (*(type *)((list = (char *)(((u32)list + 3) & ~3), list += 4) - 4))
#define VA_WIDE(list, type) (*(type *)((list = (char *)(((u32)list + 7) & ~7), list += 8) - 8))

typedef struct Copy40_800398E0 {
    u8 b[40];
} Copy40_800398E0;

extern s32 D_800C3E60;
extern char *D_800C3E68;
extern char *D_800C3E70;
extern s8 D_800C3E74;
extern char D_800C3EA0[];
extern char D_800C3FA0[];
extern u8 D_80055930[];
extern char D_80055870[];
extern char D_80055884[];
extern char D_80055896;
extern char D_80003130[];

extern void func_80038F60(s32 c);
extern void *func_80038D90(char *dst, char *src, s32 n);
extern char *func_80039024(u32 v, s32 base);
extern char *func_80039068(u64 v, s32 base);
extern char *func_8003931C(f64 x, s32 ndigits, s32 *exp, s32 *neg);
extern void func_800391BC(char *digits, s32 prec, s32 exp, char *out);
extern char *func_80039634(f64 x, s32 prec, s32 *exp, s32 *neg);
extern char *func_800396E0(f64 x, s32 prec, char *out);
extern char *func_8003ADD0(char *dst, const char *src);
extern s32 func_8003AEA0(const char *s);

#define PUT(ch)                         \
    do {                                \
        if (--D_800C3E60 == 0) {        \
            func_80038F60(ch);          \
        } else {                        \
            *D_800C3E68++ = (ch);       \
        }                               \
    } while (0)

s32 func_800398E0(char *buf, const char *fmt, va_list ap) {
    register s8 c asm("$16");
    register s32 prec asm("$17");
    register va_list args asm("$19") = ap;
    register s32 flags asm("$20");
    register s32 fprec asm("$21");
    register const char *format asm("$22") = fmt;
    register s32 width asm("$23");
    s32 len;
    s32 prefix;
    s32 isSigned;
    s32 base;
    s32 direct;
    s32 count;
    s32 i;
    s32 exp;
    s32 neg;
    s32 eexp;
    s32 eneg;
    char *s;
    char *p;
    char *digits;
    f64 x;
    u32 v;
    s64 lv;
    char unused_padding[24];

    count = 0;
    for (;;) {
        c = *format++;
        if (c != '%') {
            if (c == 0) {
                D_800C3E74 = 0;
                return count;
            }
            if (D_80055930[c] & 0x80) {
                PUT((s8)c);
                count++;
                c = *format++;
            }
            PUT((s8)c);
            count++;
            continue;
        }

        flags = 0;
        D_800C3E74 = 0;
    next_flag:
        c = *format++;
        switch (c - ' ') {
            case '-' - ' ': flags |= 1; goto next_flag;
            case '+' - ' ': flags |= 2; goto next_flag;
            case '0' - ' ': flags |= 4; goto next_flag;
            case ' ' - ' ': flags |= 8; goto next_flag;
            case '#' - ' ': D_800C3E74 = 1; goto next_flag;
        }

        width = 0;
        if ((s8)c == '*') {
            width = VA_WORD(args, s32);
            if (width < 0) {
                flags |= 1;
                width = -width;
            }
            c = *format++;
        } else {
            while (D_80055930[c & 0xFF] & 8) {
                width = width * 10 + (s8)c - '0';
                c = *format++;
            }
        }

        prec = 0;
        fprec = 6;
        if ((s8)c == '.') {
            c = *format++;
            if ((s8)c == '*') {
                prec = VA_WORD(args, s32);
            } else {
                while (D_80055930[c & 0xFF] & 8) {
                    prec = prec * 10 + (s8)c - '0';
                    c = *format++;
                }
            }
            fprec = prec;
        }

        direct = 0;
        isSigned = 0;
        D_800C3E70 = D_80055870;
        base = 10;

    conv:
        switch (c) {
            case 'd':
            case 'i':
                isSigned = 1;
                goto integer;
            case 'o':
                base = 8;
                flags &= ~0xA;
                goto integer;
            case 'X':
                D_800C3E70 = D_80055884;
            case 'x':
                base = 16;
                flags &= ~0xA;
                goto integer;
            case 'u':
                goto integer;
            case 'p':
                flags = 0;
                prec = 8;
                D_800C3E70 = D_80055884;
                base = 16;
                goto integer;
            case 'c':
                s = D_800C3EA0;
                *s = VA_WORD(args, s32);
                len = 1;
                goto emit;
            case 's':
                s = VA_WORD(args, char *);
                if (s == 0) {
                    s = D_80003130;
                }
                len = func_8003AEA0(s);
                if ((prec != 0) & (prec < len)) {
                    len = prec;
                }
                goto emit;
            case 'n':
                *VA_WORD(args, s32 *) = count;
                continue;
            case 'h':
                flags |= 0x40;
                c = *format++;
                goto conv;
            case 'l':
                c = *format++;
                if ((s8)c != 'l') {
                    goto conv;
                }
                flags |= 0x20;
                c = *format++;
                goto conv;
            case 'L':
                flags |= 0x80;
                c = *format++;
                goto conv;
            case 'f':
                x = VA_WIDE(args, f64);
                digits = func_80039634(x, fprec, &exp, &neg);
                if (neg != 0) {
                    flags |= 0x100;
                }
                s = &D_800C3EA0[1];
                p = s;
                if (exp == 0x7FFFFFFF) {
                    func_8003ADD0(s, digits);
                } else if (exp <= 0) {
                    *p++ = '0';
                    *p++ = '.';
                    i = 0;
                    while (exp != 0) {
                        *p++ = '0';
                        exp++;
                        i++;
                        if (i == fprec) {
                            goto f_term;
                        }
                    }
                    while (i != fprec) {
                        *p++ = *digits++;
                        i++;
                    }
                f_term:
                    *p = 0;
                } else if (fprec + exp < 40) {
                    func_80038D90(p, digits, exp);
                    digits += exp;
                    p += exp;
                    if (fprec != 0) {
                        *p++ = '.';
                        func_80038D90(p, digits, fprec);
                    } else if (D_800C3E74 != 0) {
                        *p++ = '.';
                    }
                    p[fprec] = 0;
                } else if (fprec + exp < 255) {
                    *(Copy40_800398E0 *)p = *(Copy40_800398E0 *)digits;
                    if (exp < 41) {
                        p += exp;
                    } else {
                        p += 40;
                        i = exp - 40;
                        do {
                            *p++ = '0';
                        } while (--i != 0);
                    }
                    if (fprec != 0) {
                        *p++ = '.';
                        i = fprec;
                        do {
                            *p++ = '0';
                        } while (--i != 0);
                    } else if (D_800C3E74 != 0) {
                        *p++ = '.';
                    }
                    *p = 0;
                } else {
                    fprec = 20;
                    c = 'e';
                    goto e_format;
                }
                goto float_done;
            case 'e':
            case 'E':
                x = VA_WIDE(args, f64);
            e_format:
                digits = func_8003931C(x, fprec, &eexp, &eneg);
                s = &D_800C3EA0[1];
                if (eneg != 0) {
                    flags |= 0x100;
                }
                if (eexp == 0x7FFFFFFF) {
                    func_8003ADD0(s, digits);
                    goto float_done;
                }
                D_80055896 = c;
                func_800391BC(digits, fprec, eexp - 1, s);
                D_80055896 = 'e';
                goto float_done;
            case 'g':
            case 'G':
                x = VA_WIDE(args, f64);
                if (x < 0.0) {
                    x = -x;
                    flags |= 0x100;
                }
                D_80055896 = c - 2;
                s = func_800396E0(x, fprec, &D_800C3EA0[1]);
                D_80055896 = 'e';
            float_done:
                len = func_8003AEA0(s);
                prec = 0;
                direct = 1;
                goto pad;
            default:
                PUT((s8)c);
                count++;
                continue;
        }

    integer:
        if (prec != 0) {
            flags &= ~4;
        }
        if (flags & 0x20) {
            lv = VA_WIDE(args, s64);
            if (isSigned && lv < 0) {
                lv = -lv;
                flags |= 0x100;
            }
            s = func_80039068(lv, base);
        } else {
            v = VA_WORD(args, u32);
            if (flags & 0x40) {
                if (isSigned) {
                    v = (s16)v;
                } else {
                    v = (u16)v;
                }
            }
            if (isSigned && (s32)v < 0) {
                v = -v;
                flags |= 0x100;
            }
            s = func_80039024(v, base);
        }
        len = D_800C3FA0 - s;

    pad:
        prefix = 0;
        if ((D_800C3E74 != 0) & (base != 10)) {
            if (!isSigned && *s != '0') {
                prefix = (base == 16) ? 2 : 1;
            }
        }
        if ((flags & 5) == 4) {
            prec = width;
        }
        if (prec != 0) {
            if (flags & 0x10A) {
                prec--;
                if (direct) {
                    width--;
                    count++;
                    if (flags & 0x100) {
                        PUT('-');
                    } else {
                        PUT((flags & 2) ? '+' : ' ');
                    }
                    flags &= ~0x10A;
                }
            }
            prec -= prefix;
            while (len < prec) {
                if (direct) {
                    PUT('0');
                    width--;
                    count++;
                    prec--;
                } else {
                    *--s = '0';
                    len++;
                }
            }
        }
        if (prefix != 0) {
            s--;
            if (prefix == 2) {
                *s = D_800C3E70[0x10];
                s--;
            }
            *s = '0';
            len += prefix;
        }
        if (flags & 0x10A) {
            len++;
            s--;
            if (flags & 0x100) {
                *s = '-';
            } else if (flags & 2) {
                *s = '+';
            } else {
                *s = ' ';
            }
        }

    emit:
        if (!(flags & 1)) {
            while (len < width) {
                PUT(' ');
                width--;
                count++;
            }
        }
        width -= len;
        count += len;
        for (len--; len != -1; len--) {
            PUT(*s);
            s++;
        }
        while (width > 0) {
            PUT(' ');
            width--;
            count++;
        }
    }
}
