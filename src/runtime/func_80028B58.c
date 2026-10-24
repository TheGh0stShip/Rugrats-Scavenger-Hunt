typedef unsigned char u8;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    u8 pad_00[0x18];
} IoMesg_80028B58;

extern u8 D_80077810[];
extern void func_80047E80(void);
extern void func_80047C50(void *, s32);
extern int func_80050FA0(
    IoMesg_80028B58 *, s32, s32, u32, void *, s32, void *
);
extern int func_80048DD0(void *, void *, s32);

void func_80028B58(u32 src, u8 *dst, s32 len) {
    IoMesg_80028B58 io;
    void *msg;
    s32 n;

    func_80047E80();
    while (len != 0) {
        n = len;
        if (len > 0x4000) {
            n = 0x4000;
        }
        func_80047C50(dst, n);
        func_80050FA0(&io, 0, 0, src, dst, n, D_80077810);
        func_80048DD0(D_80077810, &msg, 1);
        src += n;
        len -= n;
        dst += n;
    }
    func_80047E80();
}
