typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

extern u16 D_80053978[4];
extern u32 D_80061D80;
extern u16 D_80061E54[2];
extern s32 D_80053930;
extern s32 D_80061E00;

extern s32 func_8001361C(void);
extern s32 func_80043554(u16);
extern u16 func_800433F4(s32, s32, u16);
extern void func_80047E80(void);

u16 func_8001F1C4(void) {
    s32 resource;
    u16 *streamArgs;
    volatile u16 *slots;
    u32 *streamIndex;
    u16 handle;

    resource = func_8001361C();
    if (func_80043554(D_80053978[D_80061D80])) {
        D_80061D80++;
        if (D_80061D80 >= 4) {
            D_80061D80 = 0;
        }
    }

    streamArgs = D_80061E54;
    handle = func_800433F4(resource, 0x40, streamArgs[1]);
    streamIndex = &D_80061D80;
    slots = D_80053978;
    slots[*streamIndex] = handle;
    if (!func_80043554(slots[*streamIndex])) {
    retry:
        if (!func_80043554(slots[*streamIndex])) {
            slots[*streamIndex] =
                func_800433F4(resource, 0x40, streamArgs[1]);
            goto retry;
        }
        D_80053930++;
    }
    D_80061E00++;
    func_80047E80();
    return D_80053978[D_80061D80];
}
