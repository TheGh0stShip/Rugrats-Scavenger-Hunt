typedef unsigned char u8;
typedef unsigned int u32;
extern u8 D_800B92A8[];
typedef struct {
    u8 pad[0xC];
    u32 enabled;
    u32 value;
} State;
void func_8002A13C(void) {
    State *state = (State *)D_800B92A8;
    state->enabled = 1;
    state->value = 0;
}
