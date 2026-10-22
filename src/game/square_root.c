float func_80045CE0(float value) {
    float result;

    __asm__("sqrt.s %0,%1" : "=f"(result) : "f"(value));
    return result;
}
