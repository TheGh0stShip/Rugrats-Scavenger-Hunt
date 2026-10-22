typedef unsigned char u8;
extern u8 *D_80061D78;
void *func_8001E630(void) {
    u8 *result = D_80061D78;
    D_80061D78 += 0x40;
    return result;
}
