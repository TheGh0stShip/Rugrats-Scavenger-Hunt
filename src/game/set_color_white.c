extern void func_8001FB84(int red, int green, int blue, int alpha);
void func_800136AC(int alpha) {
    int color;
    alpha &= 0xFF;
    __asm__ volatile ("" : "=r"(alpha) : "0"(alpha));
    color = 0xFF;
    __asm__("" : "=r"(color) : "0"(color));
    func_8001FB84(color, color, color, alpha);
}
