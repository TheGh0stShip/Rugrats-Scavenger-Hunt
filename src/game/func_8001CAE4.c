typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

struct Player_800055A4 {
    u8 pad_00[0x150];
    s16 team;  /* 0x150 */
};

struct State_8005B320 {
    s32 count;     /* 0x00 */
    s32 list[4];   /* 0x04 */
    s32 sel;       /* 0x14 */
    s32 next;      /* 0x18 */
};

extern struct State_8005B320 D_8005B320;
extern s32 D_80057D5C;
extern struct Player_800055A4 *D_800529F0;
extern s32 func_8001BF3C(void);
extern struct Player_800055A4 *func_800055A4(s32);
extern void func_8001BED8(s32);
extern void func_800211C0(struct Player_800055A4 *, s32);
extern void func_80005500(s32, void *);

void func_8001CAE4(void) {
    s32 i;
    struct Player_800055A4 *p;

    switch (func_8001BF3C()) {
        case 0x31:
            D_8005B320.count = 0;
            D_8005B320.sel = -1;
            for (i = 0; i < D_80057D5C; i++) {
                if (func_800055A4(i) != D_800529F0 &&
                    func_800055A4(i)->team == D_800529F0->team) {
                    D_8005B320.list[D_8005B320.count] = i;
                    D_8005B320.count++;
                }
            }
            if (D_8005B320.count != 0) {
                func_8001BED8(0x22);
                D_8005B320.next = 0x21;
                break;
            }
            goto done;
        case 0x32:
            p = func_800055A4(D_8005B320.sel);
            func_800211C0(p, func_800055A4(D_8005B320.sel)->team);
        done:
            func_8001BED8(0);
            func_80005500(5, D_800529F0);
            break;
    }
}

