extern void func_8001FB84(int red, int green, int blue, int alpha);
void func_80013764(void) {
    int red = 0xFF;
    int green = 0;
    __asm__("" : "=r"(red) : "0"(red));
    __asm__("" : "=r"(green) : "0"(green));
    func_8001FB84(red, green, green, red);
}
