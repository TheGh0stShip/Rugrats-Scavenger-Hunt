extern void func_8003E47C(void *resource, int value, int mode);
extern void func_800425F0(void *resource);

void func_8003C6C4(unsigned char *command)
{
    switch (command[0]) {
        case 0:
            func_8003E47C(*(void **)(command + 4), -2, 1);
            break;
        case 1:
            func_8003E47C(*(void **)(command + 4), -2, 0);
            break;
        case 2:
            func_800425F0(*(void **)(command + 4));
            break;
    }
}
