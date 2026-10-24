typedef char *va_list;
#define va_start(ap, last) (ap = (va_list)__builtin_next_arg(last))
extern int D_800C3E60;
extern int D_800C3E64;
extern char *D_800C3E68;
extern char *D_800C3E6C;
extern int func_800398E0(char *buf, const char *fmt, va_list ap);
extern void func_80038F60(int a);

int func_8003A648(const char *fmt, ...) {
    va_list ap;
    char buf[0x80];
    int n;

    D_800C3E60 = 0x80;
    D_800C3E64 = 2;
    D_800C3E6C = buf;
    D_800C3E68 = buf;
    va_start(ap, fmt);
    n = func_800398E0(buf, fmt, ap);
    func_80038F60(0);
    return n;
}
