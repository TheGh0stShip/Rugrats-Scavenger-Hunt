extern unsigned char D_80052C04[];
extern int func_80010EF8(void);

int func_80010FB4(int index)
{
    unsigned char *state = D_80052C04 + index;
    int result;

    if (*state == 0) {
        return 0;
    }
    result = func_80010EF8();
    if (result != 0) {
        return result;
    }
    *state = 0;
    return 0;
}
