typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct {
    u32 hdr[2];
    void *dramAddr;
    u32 devAddr;
    u32 size;
    void *piHandle;
} IoMesg_8001EDEC;

typedef void *OSMesg;

extern u8 D_800664B0[];
extern void func_80047E80(void);
extern void func_80047C50(void *, s32);
extern s32 func_80050FA0(IoMesg_8001EDEC *, s32, s32, u32, void *, u32, void *);
extern s32 func_80048DD0(void *, OSMesg *, s32);

void func_8001EDEC(u32 rom, u8 *ram, s32 size) {
    IoMesg_8001EDEC mb;
    OSMesg msg;
    s32 len;

    func_80047E80();
    while (size != 0) {
        len = size;
        if (len > 0x4000) {
            len = 0x4000;
        }
        func_80047C50(ram, len);
        func_80050FA0(&mb, 0, 0, rom, ram, len, D_800664B0);
        func_80048DD0(D_800664B0, &msg, 1);
        rom += len;
        size -= len;
        ram += len;
    }
    func_80047E80();
}

