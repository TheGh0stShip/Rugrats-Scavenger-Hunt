typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

struct Rec_800197C8 {
    u8 pad_00[0x2C];
    void *list;  /* 0x2C */
    u8 pad_30[0xA];
    s16 index;   /* 0x3A */
};

struct Ctx_8005B214 {
    u8 pad_00[0xE0];
    s32 id;      /* 0xE0 */
};

extern struct Ctx_8005B214 *D_8005B214;
extern struct Rec_800197C8 *func_800197C8(s32);
extern s32 func_8000CE08(void *, s32);
extern void func_8001A5C0(void);

void func_8001C398(void) {
    s32 idx = func_800197C8(D_8005B214->id)->index;

    if (idx >= func_8000CE08(func_800197C8(D_8005B214->id)->list, 2) - 1) {
        func_8001A5C0();
    }
}

