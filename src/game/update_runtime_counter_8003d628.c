float func_8003D628(unsigned char *object)
{
    object[0xD0]--;
    if (object[0xD0] == 0) {
        if (*(signed char *)(object + 0xD1) == 0) {
            object[0xD1] = object[0xD8];
            object[0xD0] = object[0xCE];
        } else {
            object[0xD1] = 0;
            object[0xD0] = object[0xCF];
        }
    }
    return (float)*(signed char *)(object + 0xD1);
}
