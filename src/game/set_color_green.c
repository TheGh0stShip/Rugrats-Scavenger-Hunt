extern void func_8001FB84(int red, int green, int blue, int alpha);
void func_80013790(void) {
    int red = 0;
    int green = 0xFF;
    __asm__("" : "=r"(red) : "0"(red));
    __asm__("" : "=r"(green) : "0"(green));
    func_8001FB84(red, green, red, green);
}
