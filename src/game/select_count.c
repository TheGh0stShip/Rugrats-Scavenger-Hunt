int func_80006DB0(int first, int second) {
    int count = 12;

    if (!first) {
        count = 8;
        if (!second) {
            count = 24;
        }
    }
    return count;
}
