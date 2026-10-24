typedef struct RuntimeSequence {
    char pad0[4];
    float value;
    char pad8[4];
    int active;
    int index;
} RuntimeSequence;

extern RuntimeSequence D_800B92A8;
extern float D_80054390[];
extern float D_80002220;

void func_8002A154(void) {
    if (D_800B92A8.active != 0) {
        D_800B92A8.value = D_80054390[D_800B92A8.index];
        D_800B92A8.index++;
        if (D_80054390[D_800B92A8.index] == D_80002220) {
            D_800B92A8.active = 0;
        }
    }
}
