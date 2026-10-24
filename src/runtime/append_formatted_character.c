typedef int s32;

extern s32 D_800C3E60;
extern s32 D_800C3E64;
extern char *D_800C3E68;
extern char *D_800C3E6C;

extern int func_8003A6C0(char *text);
extern void func_8003A7D0(s32 character);

void func_80038F60(char character)
{
    char *current;

    *D_800C3E68 = 0;
    if (D_800C3E64 == 2) {
        D_800C3E68 = D_800C3E6C;
        func_8003A6C0(D_800C3E6C);
    } else {
        current = D_800C3E68 = D_800C3E6C;
        while (*current != 0) {
            func_8003A7D0(*current++);
        }
    }
    D_800C3E60 = 0x7F;
    *D_800C3E68++ = character;
}
