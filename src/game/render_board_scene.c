/* Target: func_80026824  VRAM 0x80026824  ROM 0x27424
 * Size: 0x105C  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3
 * Confidence: medium */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct {
    f32 x, y, z;
} Vec3f;

struct Placed_8000DD88 {
    s16 row;
    s16 col;
    s16 id;
    s16 angle;
    u8 pad_18[0x8];
};
struct Rect_8000DD88 {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    s16 type;
    s16 count;
    u8 pad_0C[0x4];
    struct Placed_8000DD88 items[1];
};
struct Room_800581F0 {
    u8 pad_00[0x1C];
    s16 unk1C;
    u8 pad_1E[0x210 - 0x1E];
};
struct Grid_800581F0 {
    u8 pad_00[0x12];
    s16 cells[0x40];
};
struct AnimInner {
    u8 pad_00[0x8];
    s32 unk08;
};
struct AnimHead {
    u16 flags;
    u8 pad_02[0x16];
    struct AnimInner *unk18;
};
struct Model_800197C8 {
    struct AnimHead *head;
    u8 pad_04[0x4];
    s16 animState;
    u8 pad_0A[0xA];
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};
struct Player_800529F0 {
    u8 pad_00[0x13C];
    u8 prevCol;
    u8 prevRow;
    u8 col;
    u8 row;
};
struct Cam_80052F04 {
    u8 pad_00[0x28];
    Vec3f pos;
};
struct Res_800197C8 {
    u8 pad_00[0x2C];
    struct Model_800197C8 model;
};
struct Inst_8006FBE0 {
    u8 data[0x18];
};

struct Globals_80052F18 {
    s32 mode;
    s32 frame;
    s32 unk8;
};
extern struct Globals_80052F18 D_80052F18;
extern s32 D_80052EE4;
extern struct Cam_80052F04 *D_80052F04;
extern struct Player_800529F0 *D_800529F0;
extern s32 D_80054208[];
extern f32 D_80054324;
extern s32 D_80054328;
extern f32 D_80054330;
extern s32 D_80054334;
extern s16 D_80057DA0;
extern s32 D_800581C0[];
extern s32 D_800581D0;
extern s8 D_800581E8;
extern s8 D_800581E9;
extern u8 *D_800581F0;
extern u8 D_80059CF0[];
extern f32 D_80059D48[3];
extern struct Inst_8006FBE0 D_8006FBE0[];
extern struct Inst_8006FBE0 D_8006FCE8;
extern Vec3f D_800B92A8;

extern s32 func_8000B8C4(void);
extern s32 func_8000B8D8(void);
extern s32 func_8000B8EC(void);
extern struct Rect_8000DD88 *func_8000DD88(u8);
extern struct Res_800197C8 *func_800197C8(s32);
extern void func_80019BD4(s32);
extern void *func_8001E630(void);
extern void func_8000CF10(void *, f32, f32, f32, f32, f32, f32, f32);
extern void *func_8000B910(void *, Vec3f *, Vec3f *, void *, void *, s32);
extern void func_8000C034(void *, f32, f32, f32, void *, void *, s32);
extern f32 func_80006D00(s32, s32, s32);
extern void func_80006AE0(Vec3f *, s32, s32, f32);
extern void func_8000CD50(struct AnimHead *, s32, s32);
extern void func_8000C3AC(void *, Vec3f *);
extern void func_8000C674(void *);
extern void func_8002A154(void);
extern void func_80027880(void);
extern s32 func_80025FC4(struct Player_800529F0 *, s32);
extern void func_80005544(void);
extern void func_80010B0C(void);
extern void func_8000C530(void *, s32, s32);
extern int func_8001F138(s32);
extern s32 func_8003A840(void);
extern s32 func_80010BD8(s32, s32, s32 *);
extern s32 func_80027978(s32);
extern void func_80011D58(s32);
extern s32 func_8000E078(s32, s32);
extern void func_8000DCC8(s32, s32, s32 *, s32 *);

#define ROOM(i) ((struct Room_800581F0 *)(D_800581F0 + 0x2000 + (i) * 0x210))
#define GRID_ROW(r) ((struct Grid_800581F0 *)(D_800581F0 + (r) * 0x80))

void func_80026824(void) {
    struct Model_800197C8 *main;
    struct Model_800197C8 *extra1 = 0;
    struct Model_800197C8 *extra2 = 0;
    struct Rect_8000DD88 *rect = 0;
    struct Placed_8000DD88 *it;
    void *mtx;
    s32 flags;
    s32 resId;
    s32 bgId;
    s32 rowStart, rowEnd, colStart, colEnd;
    s32 row, col;
    s32 cell, hi, id, k;
    s32 rowOff, z0, z1;
    s32 bonus = 0;
    s32 dist, alpha;
    s32 tileRow, tileCol;
    Vec3f v;
    s32 a, b;

    if (func_8000B8C4()) {
        return;
    }
    if (D_80054334 == 0) {
        D_80054334 = 0x56;
    }
    if (D_80052F18.mode != 2) {
        rect = func_8000DD88(D_800581E9);
    }
    if (func_8000B8D8()) {
        if (D_80052F18.mode == 0) {
            main = &func_800197C8(0x54)->model;
            flags = 0x20000000;
        } else if (D_80052F18.mode == 1) {
            main = &func_800197C8(0x55)->model;
            flags = 0x20000000;
        } else {
            main = 0;
            flags = 0x20000000;
        }
        colStart = 0;
        rowStart = 0;
        rowEnd = 0;
        colEnd = 0;
    } else {
        func_80019BD4(0x54);
        func_80019BD4(0x55);
        bgId = D_80052F18.frame + D_80052F18.mode * 3 + 0xE8;
        if (func_8000B8C4()) {
            goto background;
        }
        flags = 0;
        if (D_80052F18.mode == 2) {
            colStart = flags;
            rowStart = 0;
            rowEnd = 0;
            colEnd = 0;
            resId = D_800581E9 + 0x56;
            main = &func_800197C8(resId)->model;
            if (D_800581E8 >= 0) {
                resId = D_800581E8 + 0x5B;
                extra1 = &func_800197C8(resId)->model;
                if (D_800581C0[D_800581E8] < 2) {
                    resId = D_800581C0[D_800581E8] + 0x5F;
                    extra2 = &func_800197C8(resId)->model;
                }
            }
        } else {
            resId = ROOM(D_800581E9)->unk1C;
            main = &func_800197C8(resId)->model;
            rowStart = rect->x;
            colStart = rect->y;
            rowEnd = rowStart + rect->w;
            colEnd = colStart + rect->h;
        }
        func_80019BD4(bgId);
        if (resId != D_80054334) {
            func_80019BD4(D_80054334);
            D_80054334 = resId;
        }
        goto draw;
    background:
        main = &func_800197C8(bgId)->model;
        flags = 0x10000000;
        colStart = 0;
        rowStart = 0;
        rowEnd = 64;
        colEnd = 64;
    }
draw:
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    mtx = func_8001E630();
    func_8000CF10(mtx, 0.0f, 0.0f, 0.0f, 1.0f, v.x, v.y, v.z);
    if (main != 0) {
        if (!func_8000B8EC()) {
            flags |= 0x08000000;
            func_8000B910(main, &v, &D_80052F04->pos, D_80059CF0, mtx, flags);
        } else {
            flags |= 0x08000000;
            func_8000C034(main, D_80059D48[0], D_80059D48[1], D_80059D48[2], D_80059CF0, mtx, flags);
        }
        if (extra1 != 0) {
            func_8000B910(extra1, &v, &D_80052F04->pos, D_80059CF0, mtx, flags);
        }
        if (extra2 != 0) {
            v.x = 0.0f;
            v.z = 0.0f;
            v.y = 10.0f;
            mtx = func_8001E630();
            flags |= 0x08000000;
            func_8000CF10(mtx, 0.0f, 0.0f, 0.0f, 1.0f, v.x, v.y, v.z);
            func_8000B910(extra2, &v, &D_80052F04->pos, D_80059CF0, mtx, flags);
        }
    }

    if (D_80052F18.mode == 2) {
        flags = 0;
        if (D_800581E9 == 0) {
            struct Res_800197C8 *res;
            struct Model_800197C8 *m;

            D_80054330 += D_80054328;
            func_80006AE0(&v, 0, 0, func_80006D00(0, 0, D_800581D0 & 0x7FFF) + 5.0f);
            v.y = 150.0f;
            mtx = func_8001E630();
            a = (s32)(D_80054330 + 90.0f) % 360;
            b = (s32)(D_80054330 + 180.0f) % 360;
            func_8000CF10(mtx, (f32)a, D_80054330, (f32)b, 1.0f, v.x, v.y, v.z);
            res = func_800197C8(0x1F);
            func_8000B910(&res->model, &v, &D_80052F04->pos, D_80059CF0, mtx, 0);
            if (D_800581D0 & 0x8000) {
                func_8000CD50(res->model.head, 0, 1);
            }
            v.z = 0.0f;
            v.y = 0.0f;
            v.x = 0.0f;
            func_8000C3AC(&func_800197C8(0x1E4)->model, &v);
            mtx = func_8001E630();
            func_8000CF10(mtx, 0.0f, 0.0f, 0.0f, D_80054324, v.x, v.y, v.z);
            m = &func_800197C8(0x1E4)->model;
            func_8000B910(m, &v, &D_80052F04->pos, D_80059CF0, mtx, 0);
            func_8000C674(m);
            m = &func_800197C8(0x1E5)->model;
            mtx = func_8001E630();
            func_8000CF10(mtx, 0.0f, 0.0f, 0.0f, D_80054324, D_800B92A8.x, D_800B92A8.y, D_800B92A8.z);
            func_8000B910(m, &D_800B92A8, &D_80052F04->pos, D_80059CF0, mtx, 0);
            func_8002A154();
            func_80027880();
            return;
        }
    } else {
        flags = 0;
    }


    for (row = rowStart; row < rowEnd; row++) {
        for (col = colStart; col < colEnd; col++) {
            rowOff = row - rowStart;
            z0 = rowOff * 600;
            z1 = z0 + 300;
            cell = GRID_ROW(row)->cells[col];
            if (cell == 0) {
                continue;
            }
            hi = cell & 0x8000;
            id = cell & 0x7FFF;
            if ((u32)(id - 0x14) < 5 && D_80052F18.mode == 0 && D_80052F18.frame == 1) {
                id = D_80054208[id];
            }
            k = id - 0xF9;
            if ((u32)k < 4) {
                if (D_800529F0 != 0 && func_80025FC4(D_800529F0, 0xF)) {
                    bonus = 1;
                    id = k + 0xF;
                } else if (D_80052F18.mode == 1) {
                    id = (resId == 0x52) ? 0xFF : 0xFE;
                }
            }
            if ((u32)(id - 0xF5) < 4) {
                id = 0xE;
            }
            if (hi && id != 0x13 && D_80052F18.unk8 != 0) {
                id = 0xC;
            }
            tileCol = col - colStart;
            tileRow = rowOff;
            if (id >= 0x27) {
                v.x = tileCol * 600;
                v.z = z0;
            } else {
                v.x = tileCol * 600 + 300;
                v.z = z1;
            }
            v.y = 0.0f;
            mtx = func_8001E630();
            func_8000CF10(mtx, 0.0f, 0.0f, 0.0f, 1.0f, v.x, v.y, v.z);
            if (id >= 0x27) {
                flags |= 0x40000000;
                func_8000C034(&func_800197C8(id)->model, v.x, v.y, v.z, D_80059CF0, mtx, flags);
            } else {
                s32 t = ROOM(D_800581E9)->unk1C;
                if (t < 0x53 && t >= 0x51 && (u32)id < 0xF) {
                    id += 0x18;
                }
                flags |= 0x40000000;
                func_8000C034(&D_8006FBE0[id], v.x, v.y, v.z, D_80059CF0, mtx, flags);
            }
        }
    }

    if (ROOM(D_800581E9)->unk1C == 0x4B && D_800529F0 != 0 && func_80025FC4(D_800529F0, 0xF)) {
        v.x = 3900.0f;
        v.z = 2100.0f;
        mtx = func_8001E630();
        func_8000CF10(mtx, 0.0f, 0.0f, 0.0f, 1.0f, v.x, v.y, v.z);
        flags |= 0x40000000;
        func_8000C034(&D_8006FCE8, v.x, v.y, v.z, D_80059CF0, mtx, flags);
    }


    flags = 0;
    if (D_80052EE4 == 0 && func_8000B8EC()) {
        s32 i;

        func_80005544();
        func_80010B0C();
        for (i = 0, it = rect->items; i < rect->count; i++, it++) {
            struct Model_800197C8 *m;

            id = it->id;
            if ((u32)(id - 0x106) < 4) {
                if (!bonus) {
                    continue;
                }
            } else if ((u32)(id - 0x10A) < 0xC && bonus) {
                continue;
            }
            tileRow = it->row;
            tileCol = it->col;
            v.y = 0.0f;
            v.x = tileCol * 600 + 300;
            v.z = tileRow * 600 + 300;
            if ((u32)id < 0x100 && (u32)id >= 0xFE) {
                v.x -= 300.0f;
                v.z -= 300.0f;
            }
            mtx = func_8001E630();
            if (it->angle == 0x5A) {
                D_80054330 = 270.0f;
            } else if (it->angle == 0x10E) {
                D_80054330 = 90.0f;
            } else {
                D_80054330 = it->angle;
            }
            func_8000CF10(mtx, 0.0f, D_80054330, 0.0f, 1.0f, v.x, v.y, v.z);
            m = &func_800197C8(id)->model;
            flags = ((m->head->flags & 0x2000) == 0) << 27;
            if (id == 0x144) {
                if (m->animState == 1 && (m->head->unk18->unk08 & 1)) {
                    func_8000C530(m, 2, 0);
                    func_8001F138(0x36);
                } else if (m->animState == 3 && (m->head->unk18->unk08 & 1)) {
                    func_8000C530(m, 0, 0);
                    func_8001F138(0x36);
                } else if (func_8003A840() % 750 == 1) {
                    if (m->animState == 0) {
                        func_8000C530(m, 1, 0);
                    } else if (m->animState == 2) {
                        func_8000C530(m, 3, 0);
                    }
                }
            }
            if (func_80010BD8((s32)v.x, (s32)v.z, &dist) && !func_80027978(id)) {
                flags |= 0x04000000;
                if (!((f32)dist >= 0.0f)) {
                    dist = -dist;
                }
                if ((u32)(dist - 0x10) < 0x1D) {
                    goto after_draw;
                }
                if (dist < 0x10) {
                    k = dist - 0xF;
                    alpha = ((f32)k >= 0.0f) ? k * 17 : (0xF - dist) * 17;
                } else {
                    k = dist - 0x2D;
                    alpha = ((f32)k >= 0.0f) ? k * 17 : (0x2D - dist) * 17;
                }
                m->r = 0xFF;
                m->b = 0xFF;
                m->g = 0xFF;
                m->a = alpha;
                func_8000B910(m, &v, &D_80052F04->pos, D_80059CF0, mtx, flags);
            } else if (id == 0x137) {
                flags &= ~0x04000000;
                func_8000B910(m, &v, &v, D_80059CF0, mtx, flags);
            } else {
                flags &= ~0x04000000;
                func_8000B910(m, &v, &D_80052F04->pos, D_80059CF0, mtx, flags);
            }
            func_8000C674(m);
        after_draw:
            if (id == 0xEE) {
                func_80011D58(flags);
            }
        }
    }


    if (D_800529F0->col != 0 && D_80057DA0 == 0 && !func_80025FC4(D_800529F0, 0xE) &&
        func_8000E078(D_800529F0->row, D_800529F0->col) == D_800581E9) {
        s32 d;

        func_8000DCC8(D_800529F0->row, D_800529F0->col, &tileRow, &tileCol);
        d = D_800529F0->col - D_800529F0->prevCol;
        if (d != 0) {
            D_80054330 = (d < 0) ? 90.0f : 270.0f;
        } else {
            d = D_800529F0->row - D_800529F0->prevRow;
            if (d != 0) {
                D_80054330 = (d < 0) ? 0.0f : 180.0f;
            }
        }
        v.y = 0.0f;
        v.x = tileCol * 600 + 300;
        v.z = tileRow * 600 + 300;
        mtx = func_8001E630();
        func_8000CF10(mtx, 0.0f, D_80054330, 0.0f, 1.0f, v.x, v.y, v.z);
        func_8000B910(&func_800197C8(0x1E2)->model, &v, &D_80052F04->pos, D_80059CF0, mtx, 0);
    }
}
