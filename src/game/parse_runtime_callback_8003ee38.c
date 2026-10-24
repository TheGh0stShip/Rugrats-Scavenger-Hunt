typedef void (*RuntimeCallback)(void *resource, int value);

extern RuntimeCallback D_800C4010;

unsigned char *func_8003EE38(unsigned char *object, unsigned char *source)
{
    int value = *source++;

    if ((*source++ & 0x80) != 0) {
        source++;
    }
    if ((*(unsigned int *)object & 3) == 2 && D_800C4010 != 0) {
        D_800C4010(*(void **)(object + 0x44), value);
    }
    return source;
}
