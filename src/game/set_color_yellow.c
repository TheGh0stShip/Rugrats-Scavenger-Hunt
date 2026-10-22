extern void func_8001FB84(int red, int green, int blue, int alpha);
void func_800136D8(int alpha) {
    int color;
    alpha &= 0xFF;
    __asm__ volatile ("" : "=r"(alpha) : "0"(alpha));
    color = 0xFF;
    __asm__("" : "=r"(color) : "0"(color));
    func_8001FB84(color, color, 0, alpha);
}
