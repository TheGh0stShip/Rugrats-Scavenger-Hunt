typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct Actor_800055A4 {
    u8 pad_000[0x118];
    u32 flags;
};

extern u32 D_800B9280;
extern s32 D_800B9284;
extern struct Actor_800055A4 *func_800055A4(s32);
extern void func_8002903C(void *);
extern void func_8002919C(void *);
extern void func_800292D0(s32, s16);
extern void func_80029544(void);

void func_80028F80(void *arg0) {
    switch (D_800B9280) {
        case 0:
            func_8002903C(arg0);
            break;
        case 1:
            func_8002919C(arg0);
            break;
        case 2:
            func_800292D0((s32)arg0, 4);
            func_800055A4(D_800B9284)->flags |= 0x80000;
            break;
        case 3:
            func_800292D0((s32)arg0, 2);
            func_800055A4(D_800B9284)->flags |= 0x40000;
            break;
        case 4:
            func_80029544();
            break;
    }
}
