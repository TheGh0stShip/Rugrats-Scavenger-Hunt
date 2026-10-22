double func_8004B540(double value, int exponent) {
    if (exponent != 0) {
        value *= 1 << exponent;
    }
    return value;
}
