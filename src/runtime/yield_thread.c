typedef unsigned int u32;
typedef unsigned short u16;

typedef struct {
    unsigned char pad[0x10];
    u16 state;
} Thread;

extern Thread *D_80056070;
extern Thread *D_80056068;
extern u32 func_80048900(void);
extern void func_80048920(u32 status);
extern void func_80048614(Thread **queue);

void func_80050B80(void) {
    u32 status = func_80048900();

    D_80056070->state = 2;
    func_80048614(&D_80056068);
    func_80048920(status);
}
