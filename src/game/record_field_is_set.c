int func_8000CEEC(char **object, int index) {
    int offset = index * 12;
    char *records = *(char **)(*object + 0x18);

    return *(unsigned short *)(records + offset + 0x20) != 0;
}
