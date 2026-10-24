typedef unsigned char u8;
typedef unsigned int u32;

void func_800166F0(u8 *src, u8 *dst, u32 *size) {
    register u32 i asm("$7");
    register u32 command asm("$8");
    register u32 control asm("$3");
    u8 *start;
    u8 *copy;

    start = dst;
top:
    command = *src;
    src++;
    control = command & 0xFF;
    if (control == 0) {
        goto done;
    }
    if (command & 0x80) {
        i = 0;
        control = (command & 0x7F) << 3;
        command = *src;
        src++;
        control |= command >> 5;
        command &= 0x1F;
        copy = dst - control;
        for (; i < command; i++) {
            *dst++ = *copy++;
        }
        goto top;
    }
    if (command & 0x40) {
        command &= 0x3F;
        i = 0;
        for (; i < command; i++) {
            *dst++ = *src;
        }
        src++;
        goto top;
    }
    command = control;
    i = 0;
    for (; i < command; i++) {
        *dst++ = *src++;
    }
    goto top;
done:
    *size = dst - start;
}
