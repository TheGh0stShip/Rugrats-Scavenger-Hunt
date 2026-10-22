int func_80025FDC(char *object) {
    int index;
    int count = 0;

    for (index = 0; index < 4; index++, object += 4) {
        if ((*(unsigned int *)(object + 0x1AC) & 0x80000000) == 0) {
            count++;
        }
    }
    return count;
}
