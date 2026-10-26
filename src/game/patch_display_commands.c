typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u16 index; u16 type; u8 pad[4]; } DisplayCommandRef;

extern s32 D_80052EE8;

void func_80014084(DisplayCommandRef *refs, u32 count, Gfx *display_list)
{
    u16 index = 0;
    Gfx *command;
    s32 mode;
    u32 word;

    if ((u16)count != 0) {
        mode = D_80052EE8;
        do {
            command = &display_list[refs[index].index];
            switch (refs[index].type) {
            case 2:
                command->w0 = 0xFCFFFFFF;
                command->w1 = 0xFFFE793C;
                break;
            case 3:
            case 4:
                command->w0 = 0xFC127E24;
                command->w1 = 0xFFFFF3F9;
                break;
            case 5:
                if (mode) {
                    word = 0xC8112008;
                    command->w0 = 0xB900031D;
                    command->w1 = word;
                } else {
                    word = 0x0C192008;
                    command->w0 = 0xB900031D;
                    command->w1 = word;
                }
                break;
            case 6:
                if (mode) {
                    word = 0xC8112038;
                    command->w0 = 0xB900031D;
                    command->w1 = word;
                } else {
                    word = 0x0C192038;
                    command->w0 = 0xB900031D;
                    command->w1 = word;
                }
                break;
            case 7:
                if (mode) {
                    word = 0xC8104240;
                    command->w0 = 0xB900031D;
                    command->w1 = word;
                } else {
                    word = 0x0C184240;
                    command->w0 = 0xB900031D;
                    command->w1 = word;
                }
                break;
            case 8:
                if (mode) {
                    word = 0xC8104A50;
                    command->w0 = 0xB900031D;
                    command->w1 = word;
                } else {
                    word = 0x0C184A50;
                    command->w0 = 0xB900031D;
                    command->w1 = word;
                }
                break;
            case 9:
            case 10:
                command->w0 = 0xB7000000;
                command->w1 = 1;
                break;
            case 11:
                command->w0 = 0xB6000000;
                command->w1 = 1;
                break;
            case 12:
                word = 0x2000;
                command->w0 = 0xB6000000;
                command->w1 = word;
                break;
            }
            index++;
            __asm__("" : "=r"(count) : "0"(count));
        } while (index < (count & 0xFFFF));
    }
}
