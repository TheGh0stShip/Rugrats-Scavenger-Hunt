typedef unsigned char u8;
u8 *func_8003EA7C(void *object, u8 *input) {
    u8 *bytes = object;
    bytes[0xD8] = *input++;
    bytes[0xCE] = *input++;
    bytes[0xCF] = *input++;
    return input;
}
