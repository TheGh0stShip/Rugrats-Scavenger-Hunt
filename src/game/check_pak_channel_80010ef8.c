typedef unsigned char u8;
typedef int s32;

extern u8 D_80058B30[];
extern u8 D_80058B9C;
extern u8 D_80058990[];

extern void func_8004E270(void *buffer, u8 *status);
extern s32 func_8004E550(void *channel);
extern s32 func_8001101C(s32 status);

s32 func_80010EF8(s32 channel)
{
    s32 status = 0;
    s32 attempt;
    s32 result;
    u8 *channel_status = &D_80058B9C;

    func_8004E270(D_80058B30, channel_status);
    if (((*channel_status >> channel) & 1) == 0) {
        return result;
    }

    attempt = status;
    if (channel < 0) {
        status = 1;
    }
    if (status == 0) {
        for (attempt = 0; attempt < 3; attempt++) {
            status = func_8004E550(D_80058990 + channel * 104);
        }
    }
    return func_8001101C(status);
}
