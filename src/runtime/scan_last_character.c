typedef int s32;

extern s32 func_8003AF84(s32 value);

s32 func_8003A6C0(char *text)
{
    s32 last;

    func_8003AF84((s32)text);
    last = 0;
    while (*text != 0) {
        last = *text;
        text++;
    }
    return last;
}
