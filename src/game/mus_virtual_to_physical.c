typedef struct {
    int pad0[3];
    unsigned int base;
    unsigned int addr;
} region_t;

extern region_t *func_8004290C(unsigned int addr);
extern unsigned int func_800495D0(unsigned int addr);

unsigned int func_800428A0(unsigned int addr)
{
    region_t *r;

    r = func_8004290C(addr);
    if (r == 0)
        return func_800495D0(addr);
    if ((addr & 0xFF000000) == 0xFF000000)
        { addr &= 0xFFFFFF; addr += 0x140000; }
    return func_800495D0(r->addr + addr - r->base);
}
