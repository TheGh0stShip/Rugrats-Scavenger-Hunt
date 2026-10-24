extern void func_8003FF88(unsigned char *object, int mode);

int func_8003F6E8(unsigned char *object, int mode, void *value)
{
    switch (mode) {
    case 3: {
        void **link = *(void ***)(object + 0x80);
        if (link != 0) {
            *link = value;
        } else {
            *(void **)(object + 0x7C) = value;
        }
        *(void **)(object + 0x80) = value;
        break;
    }
    case 4:
        *(int *)(object + 0x78) = 1;
        *(int *)(object + 0x84) = 0;
        *(short *)(object + 0x5A) = 1;
        *(int *)(object + 0x74) = 0;
        *(int *)(object + 0x4C) = 0;
        *(int *)(object + 0x50) = 1;
        *(int *)(object + 0x48) = 0;
        func_8003FF88(object, 4);
        break;
    case 9:
        *(int *)(object + 0x84) = 1;
        break;
    default:
        func_8003FF88(object, mode);
        break;
    }
    return 0;
}
