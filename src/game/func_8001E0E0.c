typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct {
    s32 unk0;
    s32 unk4;
} Pair_8005B830;

typedef struct {
    s32 unk0;
    void *unk4;
    s32 count;
    Pair_8005B830 entries[1];
} List_8005B830;

extern List_8005B830 D_8005B830;
extern void func_8001A3EC(void *);
extern void func_80019BD4(s32);
extern void func_8001E0B0(void);

void func_8001E0E0(void) {
    s32 i;

    if (D_8005B830.unk4 != 0) {
        func_8001A3EC(D_8005B830.unk4);
        D_8005B830.unk4 = 0;
    }
    for (i = 0; i < D_8005B830.count; i++) {
        func_80019BD4(D_8005B830.entries[i].unk0);
    }
    func_8001E0B0();
}

