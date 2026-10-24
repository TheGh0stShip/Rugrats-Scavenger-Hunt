typedef signed int s32;
typedef unsigned char u8;

struct Obj_800160F4 {
    u8 pad_00[0xFC];
    s32 unkFC;
};

extern s32 D_80057D94;
extern void func_800235F0(void *);
extern void func_80005654(void);

void func_800160F4(struct Obj_800160F4 *arg0) {
    if (D_80057D94 != 1) {
        if (arg0->unkFC != 0) {
            func_800235F0(arg0);
        }
        func_80005654();
    }
}

