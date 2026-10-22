typedef unsigned char u8;
extern u8 *D_80061D78;
void *func_8001E630(void) {
    register u8 *current __asm__("$4") = D_80061D78;
    register u8 *result __asm__("$2");

    __asm__("" : "=r"(current) : "0"(current));
    result = current;
    __asm__("" : "=r"(result) : "0"(result));
    D_80061D78 = current + 0x40;
    return result;
}
