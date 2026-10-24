typedef char *va_list;
#define va_start(ap, last) (ap = (va_list)__builtin_next_arg(last))
extern int D_800C3E60;
extern char *D_800C3E68;
extern char *D_800C3E6C;
extern int func_800398E0(char *buf, const char *fmt, va_list ap);

int func_8003A57C(char *buf, const char *fmt, ...) {
    va_list ap;
    int n;

    D_800C3E60 = -1;
    D_800C3E6C = buf;
    D_800C3E68 = buf;
    va_start(ap, fmt);
    n = func_800398E0(buf, fmt, ap);
    buf[n] = 0;
    return n;
}
