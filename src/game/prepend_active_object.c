typedef unsigned char u8;
typedef signed int s32;

extern u8 *D_80055AF4;
extern s32 func_80047D60(s32);

void func_8003EEC0(u8 *object) {
    s32 state = func_80047D60(1);
    u8 *head = D_80055AF4;
    *(void **)(object + 0x10) = *(void **)(head + 0x20);
    *(void **)object = *(void **)head;
    *(void **)head = object;
    func_80047D60(state);
}
