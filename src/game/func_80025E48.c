typedef unsigned char u8;
typedef unsigned int u32;
typedef signed int s32;

struct Player_80025E48 {
    u8 pad_00[0xF8];
    s32 unkF8;
    u8 pad_FC[0x1AC - 0xFC];
    u32 unk1AC[4];
};

extern s32 D_80057D5C;
extern s32 D_80052F18;
extern s32 D_800535E0;
extern struct Player_80025E48 *func_800055A4(s32);
extern struct Player_80025E48 *func_80009140(void);

s32 func_80025E48(void) {
    struct Player_80025E48 *p;
    s32 total;
    s32 i;
    s32 j;
    s32 local;
    s32 result;

    result = i = total = 0;
    for (; i < D_80057D5C; i++) {
        p = func_800055A4(i);
        j = local = 0;
        for (; j < 4; j++) {
            if (!(p->unk1AC[j] & 0x80000000)) {
                total++;
                local++;
            }
            if (D_80052F18 == 1 && local == 4) {
                D_800535E0 = p->unkF8 + 2;
                result = D_800535E0;
                goto end;
            }
        }
    }
    if (total == D_80057D5C * 4) {
        result = D_800535E0 = 1;
    } else if (D_80052F18 != 1) {
        p = func_80009140();
        total = 0;
        for (j = 0; j < 4; j++) {
            if (!(p->unk1AC[j] & 0x80000000)) {
                total++;
            }
        }
        if (total == 4) {
            result = D_800535E0 = 6;
        }
    }
end:
    return result;
}
