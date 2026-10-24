/* Target: func_800398E0  VRAM 0x800398E0  ROM 0x3A4E0
 * Size: 0xC9C  Pipeline: KMC GCC 2.7.2 -O1 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */

typedef char *va_list;
#define __va_rounded_size(T) (((sizeof(T) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))
#define va_arg(AP, T) (*(T *)(void *)((AP = (char *)((__alignof__(T) > 4 ? ((int)(AP) + 7) & -8 : ((int)(AP) + 3) & -4) + __va_rounded_size(T))) - (sizeof(T) < 4 ? sizeof(T) : __va_rounded_size(T))))

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;
typedef unsigned long long u64;
typedef double f64;

typedef struct Copy40_800398E0 {
    u8 b[40];
} Copy40_800398E0;

extern s32 D_800C3E60;
extern char *D_800C3E68;
extern char *D_800C3E70;
extern s8 D_800C3E74;
extern char D_800C3EA0[];
extern char D_800C3FA0[];
extern s8 D_80055930[];
extern char D_80055870[];
extern char D_80055884[];
extern char D_80055896;

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
    u8 c;
    s32 flags;
    s32 width;
    s32 prec;
    s32 fprec;
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

    count = 0;
    for (;;) {
        c = (u8)*fmt++;
        if (c != '%') {
            if (c == 0) {
                D_800C3E74 = 0;
                return count;
            }
            if (D_80055930[c] & 0x80) {
                PUT((s8)c);
                count++;
                c = (u8)*fmt++;
            }
            PUT((s8)c);
            count++;
            continue;
        }

        flags = 0;
        D_800C3E74 = 0;
    next_flag:
        c = (u8)*fmt++;
        switch ((s8)(c - ' ')) {
            case '-' - ' ': flags |= 1; goto next_flag;
            case '+' - ' ': flags |= 2; goto next_flag;
            case '0' - ' ': flags |= 4; goto next_flag;
            case ' ' - ' ': flags |= 8; goto next_flag;
            case '#' - ' ': D_800C3E74 = 1; goto next_flag;
        }

        width = 0;
        if ((s8)c == '*') {
            width = va_arg(ap, s32);
            if (width < 0) {
                flags |= 1;
                width = -width;
            }
            c = (u8)*fmt++;
        } else {
            while (D_80055930[c & 0xFF] & 8) {
                width = width * 10 + (s8)c - '0';
                c = (u8)*fmt++;
            }
        }

        prec = 0;
        fprec = 6;
        if ((s8)c == '.') {
            c = (u8)*fmt++;
            if ((s8)c == '*') {
                prec = va_arg(ap, s32);
            } else {
                while (D_80055930[c & 0xFF] & 8) {
                    prec = prec * 10 + (s8)c - '0';
                    c = (u8)*fmt++;
                }
            }
            fprec = prec;
        }

        direct = 0;
        isSigned = 0;
        D_800C3E70 = D_80055870;
        base = 10;

    conv:
        switch ((s8)c) {
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
                *s = va_arg(ap, char);
                len = 1;
                goto emit;
            case 's':
                s = va_arg(ap, char *);
                if (s == 0) {
                    s = "(null)";
                }
                len = func_8003AEA0(s);
                if ((prec != 0) & (prec < len)) {
                    len = prec;
                }
                goto emit;
            case 'n':
                *va_arg(ap, s32 *) = count;
                continue;
            case 'h':
                flags |= 0x40;
                c = (u8)*fmt++;
                goto conv;
            case 'l':
                c = (u8)*fmt++;
                if ((s8)c != 'l') {
                    goto conv;
                }
                flags |= 0x20;
                c = (u8)*fmt++;
                goto conv;
            case 'L':
                flags |= 0x80;
                c = (u8)*fmt++;
                goto conv;
            case 'f':
                x = va_arg(ap, f64);
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
                x = va_arg(ap, f64);
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
                x = va_arg(ap, f64);
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
            lv = va_arg(ap, s64);
            if (isSigned && lv < 0) {
                lv = -lv;
                flags |= 0x100;
            }
            s = func_80039068(lv, base);
        } else {
            v = va_arg(ap, u32);
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
