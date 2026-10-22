extern void func_8001FB84(int red, int green, int blue, int alpha);
void func_800137BC(void) {
    int red = 0;
    int green;
    int blue;
    __asm__ volatile ("" : "=r"(red) : "0"(red));
    green = red;
    __asm__ volatile ("" : "=r"(green) : "0"(green));
    blue = 0xFF;
    __asm__ volatile ("" : "=r"(blue) : "0"(blue));
    func_8001FB84(red, green, blue, blue);
}
