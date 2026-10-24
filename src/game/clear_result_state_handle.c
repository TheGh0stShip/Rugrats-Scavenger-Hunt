typedef int s32;

struct ResultState {
    char pad[0x208];
    s32 handle;
    s32 value;
};

extern struct ResultState D_80057F70;
extern void func_80025E04(s32 a, s32 b);

void func_80009434(s32 a) {
    struct ResultState *s = &D_80057F70;

    s->handle = -1;
    func_80025E04(a, s->value);
}
