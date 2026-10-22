extern int D_80053030[];
extern int D_80053090;

void func_800167D0(void) {
    int index;

    for (index = 0; index < 12; index++) {
        if (D_80053030[index * 2] != index) {
            D_80053090 = 1;
        }
    }
}
