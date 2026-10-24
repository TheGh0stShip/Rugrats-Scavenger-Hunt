typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

typedef struct Actor {
    u8 pad_000[0x118];
    u32 flags;
} Actor;

extern void func_80012D04(s32 first, s32 second);
extern void func_80005500(s32 mode, Actor *actor);
extern void func_800069C8(s32 animation, Actor *actor, s32 loop);

void func_80006E30(Actor *actor) {
    func_80012D04(10, 1);
    func_80005500(4, actor);
    func_800069C8(0, actor, 0);
    actor->flags |= 0x400;
}
