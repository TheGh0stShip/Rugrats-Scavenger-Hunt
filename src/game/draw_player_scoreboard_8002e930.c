typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef signed int s32;

struct ScoreboardObject {
    u8 pad_000[0xF4];
    s32 player_id;
    s32 portrait_id;
    u8 pad_0FC[0x1E8 - 0xFC];
    s16 current[4];
    s16 maximum[4];
    s16 score;
};

extern int func_8001F588(s32);
extern void func_80033144(s32, s32, s32, s32);
extern void *func_80032870(s32);
extern void func_80032A20(void *, s32, s32);
extern struct ScoreboardObject *func_800055A4(s32);
extern s32 func_800301C8(struct ScoreboardObject *, s32);
extern void func_80013790(void);
extern void func_80013724(void);
extern void func_80013764(void);
extern void func_800137E8(s32);
extern void func_800138FC(char *, s32, s32);
extern s32 func_8003A57C(char *, const char *, ...);
extern void func_8000D0AC(s32);

extern s32 D_800545E0;
extern s32 D_80057D5C;
extern struct ScoreboardObject *D_8005B214;
extern char D_8005B218[];
extern const char D_80002960[];
extern const char D_80002968[];
extern const char D_8000296C[];
extern s32 D_800B9318;

void func_8002E930(void) {
    struct LoopState {
        char pad[16];
        s32 player;
        s32 y;
    } loop;
    s32 i;
    s32 j;
    s32 x;
    s32 current;
    s32 maximum;
    s32 draw_y;
    s32 text_x;
    s32 row_y;
    struct ScoreboardObject *object;
    struct ScoreboardObject *image;

    if (D_800545E0 == 0) {
        return;
    }
    func_8001F588(1);
    func_80033144(255, 255, 255, 255);
    for (i = 0; i < 5; i++) {
        func_80032A20(func_80032870(i + 0x24), 0x98 + i * 0x38, 0x37);
    }
    for (loop.player = 0, loop.y = 0; loop.player < D_80057D5C;
         loop.y += 0x18, loop.player++) {
        object = func_800055A4(loop.player);
        func_80033144(255, 255, 255, 255);
        draw_y = loop.y + 0x53;
        image = func_80032870(object->portrait_id + 0x20);
        func_80032A20(image, 0x58, draw_y);
        if (object->player_id == D_8005B214->player_id) {
            func_80033144(255, 255, 255, 255);
            draw_y = loop.y + 0x51;
            image = func_80032870(0x29);
            func_80032A20(image, 0x3A, draw_y);
        }
        row_y = loop.y;
        draw_y = row_y + 0x57;
        for (j = 0, x = 0; j < 4;
             j++, x += 0x38, draw_y = row_y + 0x57) {
            text_x = x + 0xA8;
            maximum = object->maximum[j];
            current = object->current[j];
            if (maximum != -1) {
                if (func_800301C8(object, j) != 0) {
                    func_80013790();
                } else {
                    func_80013724();
                }
                func_8003A57C(D_8005B218, D_80002960, current, maximum);
            } else {
                func_80013764();
                func_8003A57C(D_8005B218, D_80002968);
            }
            func_800137E8(8);
            func_800138FC(D_8005B218, text_x, draw_y);
            func_80013724();
            func_8003A57C(D_8005B218, D_8000296C, object->score);
            func_800138FC(D_8005B218, 0x190, draw_y);
        }
    }
    func_8000D0AC(D_800B9318);
}
