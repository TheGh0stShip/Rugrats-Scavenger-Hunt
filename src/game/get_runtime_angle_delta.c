extern int func_80006DB0(int, int);

float func_80006D00(int x, int y, int value) {
    int count;
    int difference;

    count = func_80006DB0(x, y);
    difference = count - value;
    return (float)(((difference % count) * 360.0) / count);
}
