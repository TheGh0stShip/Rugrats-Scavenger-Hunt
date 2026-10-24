typedef unsigned char u8;

extern u8 *D_800C42EC;
extern void *D_800C42E0;
extern void *D_800C42E4;
extern void *D_800C42E8;
extern int D_80055FD0;

void *func_80042FD4(void *address)
{
    if (D_800C42EC + 0xB8 < (u8 *)address) {
        if (D_80055FD0 != 0) {
            D_80055FD0 = 0;
            return D_800C42E4;
        }
    } else if ((u8 *)address < D_800C42EC) {
        if (D_80055FD0 != 0) {
            D_80055FD0 = 0;
            return D_800C42E8;
        }
    } else {
        D_80055FD0 = 1;
    }
    return D_800C42E0;
}
