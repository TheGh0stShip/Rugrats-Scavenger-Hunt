extern volatile int D_80057280;

void func_80003ACC(int limit) {
    D_80057280 = 0;
    while (D_80057280 < limit) {
        D_80057280++;
    }
}
