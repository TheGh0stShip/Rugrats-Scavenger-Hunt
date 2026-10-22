typedef void *Ptr;

extern Ptr D_80055AF0;
extern Ptr D_80055AF4;
extern void func_800407D0(Ptr);

void func_8003EF10(Ptr object, Ptr argument) {
    if (D_80055AF0 == 0) {
        D_80055AF0 = object;
        if (D_80055AF4 == 0) {
            D_80055AF4 = object;
            func_800407D0(argument);
        }
    }
}
