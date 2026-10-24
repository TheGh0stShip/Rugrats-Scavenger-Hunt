/* func_8000DB84: within a room rectangle, turn cells whose low bits are 1 into marker 0xD (keeping bit 15). */
typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;
typedef struct R { u8 pad[0x2014]; s16 x, y, w, h; } R;
extern u8 *D_800581F0;

void func_8000DB84(s32 idx) {
    u8 *base = D_800581F0;
    R *room0 = (R *)(base + idx * 0x210);
    R *room;
    s32 r, c;
    s32 row;
    s32 p;
    s32 one;

    __asm__("" : "=r"(room) : "0"(room0));
    __asm__("" : "=r"(one) : "0"(1));
    r = room0->x;
    row = r * 0x80 + (s32)base;
    if (r < r + room->w) {
        do {
            p = row + 0x12;
            c = room->y;
            p = c * 2 + p;
            if (c < c + room->h) {
                do {
                    s32 v = *(s16 *)p;
                    s32 hi = v & 0x8000;
                    if ((v & 0x7FFF) == one) {
                        *(s16 *)p = hi | 0xD;
                    }
                    p += 2;
                } while (room->y + room->h > ++c);
            }
            row += 0x80;
        } while (room->x + room->w > ++r);
    }
}
