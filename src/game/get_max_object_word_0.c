typedef signed int s32;

typedef struct Pair { s32 first; s32 second; } Pair;
extern Pair *D_80055F94[];

s32 func_80042568(void) {
    s32 maximum = 0;
    s32 i;
    for (i = 0; D_80055F94[i] != 0; i++) {
        if (maximum < D_80055F94[i]->first) maximum = D_80055F94[i]->first;
    }
    return maximum;
}
