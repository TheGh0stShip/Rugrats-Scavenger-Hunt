typedef unsigned char u8;
typedef short s16;
typedef int s32;

struct RouteRecord {
    u8 pad_00[4]; u8 x; u8 y; s16 kind; s16 reserved; s16 depth; u8 path[9][2];
};

extern s16 D_8006D6A0[][64];
extern u8 D_8006F6A0[][2];
extern struct RouteRecord D_8006F6B8[];
extern struct RouteRecord gRouteTable[] __asm__("D_8006F6B8");
extern s32 D_800539A0;
extern s32 func_8000DF14(s16, s16);
extern s16 func_8000DF40(s16);
extern s16 func_8000DF94(s16);
extern s16 func_8000DFA4(s16);
extern s32 func_80005798(s16);
extern void func_8000695C(s16, u8 *, u8 *);
extern void func_80021604(s32, s32 *, s32 *, s32, s32, s32 *);

#define ROUTE_COUNT (*(s32 *)D_8006F6B8)

void func_8002012C(s16 x, s16 y, s16 depth, s16 maxDepth, s16 mode,
                   u8 *target, s16 prevX, s16 prevY) {
    s32 one;
    s32 go;
    s16 i;
    s16 j;
    struct { u8 b[2]; s32 x; s32 y; } loc;

    __asm__("" : "=r"(one) : "0"(1));
    {
        s16 *row = D_8006D6A0[y];
        row[x] = one;
    }
    {
        u8 (*hist)[2];
        __asm__("" : "=r"(hist) : "0"(D_8006F6A0));
        hist[depth][0] = x;
        hist[depth][1] = y;
    }
    if (depth != 0 && (depth == maxDepth || mode != 0)) {
        if (ROUTE_COUNT >= 40) return;
        D_8006F6B8[ROUTE_COUNT].x = x;
        D_8006F6B8[ROUTE_COUNT].y = y;
        D_8006F6B8[ROUTE_COUNT].kind = func_8000DF40(func_8000DF14(y, x));
        D_8006F6B8[ROUTE_COUNT].reserved = 0;
        {
            s16 d;
            __asm__("" : "=r"(d) : "0"(depth));
            for (j = 0; j < d + 1; j++) {
                gRouteTable[*(s32 *)gRouteTable].path[j][0] = D_8006F6A0[j][0];
                gRouteTable[*(s32 *)gRouteTable].path[j][1] = D_8006F6A0[j][1];
            }
        }
        D_8006F6B8[ROUTE_COUNT].depth = depth;
        ROUTE_COUNT++;
    }
    __asm__("" : "=r"(go) : "0"(one));
    __asm__("" : : "r"(one));
    if (go == 1 && depth < maxDepth) {
        if (func_8000DFA4(func_8000DF14(y, x)) != 0 && D_800539A0 == 0) {
            func_8000695C(func_8000DF14(y, x), &loc.b[0], &loc.b[1]);
            D_800539A0 = go;
            func_8002012C(loc.b[1], loc.b[0], depth + 1, maxDepth + 1, mode, 0, x, y);
            D_8006F6A0[depth][0] = x;
            D_8006F6A0[depth][1] = y;
            return;
        }
        for (i = 0; i < 4; i++) {
            u8 nx;
            u8 ny;
            func_80021604(i, &loc.x, &loc.y, x, y, 0);
            nx = loc.x;
            ny = loc.y;
            if (nx == 0xFF || ny == 0xFF) continue;
            if (target != 0 && nx == target[0] && ny == target[1]) continue;
            if (func_80005798(func_8000DF14(ny, nx)) == 0) continue;
            if (ny == prevY && nx == prevX) continue;
            if (func_8000DF94(func_8000DF14(ny, nx)) != 0) {
                func_8002012C(nx, ny, depth, maxDepth, mode, 0, x, y);
            } else if (func_8000DFA4(func_8000DF14(y, x)) != 0) {
                if (D_800539A0 == 1)
                    func_8002012C(nx, ny, depth + 1, maxDepth + 1, mode, 0, x, y);
            } else {
                func_8002012C(nx, ny, depth + 1, maxDepth, mode, 0, x, y);
            }
            D_8006F6A0[depth][0] = x;
            D_8006F6A0[depth][1] = y;
        }
    }
}
