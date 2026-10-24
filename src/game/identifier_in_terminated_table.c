/* Returns 1 when arg0 appears in the -1 terminated identifier table. */
typedef int s32;

extern s32 D_80054268[];

s32 func_80027978(s32 arg0) {
    s32 i;

    i = 0;
    do {
        if (D_80054268[i] == arg0) {
            return 1;
        }
    } while (D_80054268[i++] != -1);
    return 0;
}
