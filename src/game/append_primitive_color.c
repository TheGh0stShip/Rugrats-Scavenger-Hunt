typedef unsigned int u32;

typedef struct Command {
    u32 word0;
    u32 word1;
} Command;

extern Command *D_80077830;

void func_80033144(u32 red, u32 green, u32 blue, u32 alpha) {
    u32 color;
    Command *command;

    color = (red << 24) | ((green & 0xFF) << 16) |
            ((blue & 0xFF) << 8) | (alpha & 0xFF);
    command = D_80077830++;
    command->word0 = 0xFA000000;
    command->word1 = color;
}
