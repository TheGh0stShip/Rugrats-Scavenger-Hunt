extern void func_80033144(int red, int green, int blue, int alpha);

void func_80033118(void) {
    register int color __asm__("$4") = 0xFF;

    __asm__("" : "=r"(color) : "0"(color));
    func_80033144(color, color, color, color);
}
