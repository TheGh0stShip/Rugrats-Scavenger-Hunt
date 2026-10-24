typedef unsigned int u32;

struct State_80052F18 {
    u32 words[13];
};

extern struct State_80052F18 D_80052F18;
extern struct State_80052F18 D_80052F4C;

void func_800196E8(void)
{
    D_80052F4C = D_80052F18;
}
