extern int D_80054AB0; extern int D_800C3E50[];
void func_80035890(int i) {
    for (; i < D_80054AB0 - 1; i++) D_800C3E50[i] = (D_800C3E50 + 1)[i];
    D_80054AB0--;
}
