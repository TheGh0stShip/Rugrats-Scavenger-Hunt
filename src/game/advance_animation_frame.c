/* func_8000C674  VRAM 0x8000C674  ROM 0xD274  Size: 0x43C
 * Advance an animation controller one tick and fire frame events.
 * SN GCC 2.8 -O2 with reviewed scheduling normalization. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct AnimInfo {
    u8 pad_00[0x20];
    u16 unk20;
    u8 pad_22[0x2];
} AnimInfo; /* stride 0xC via base+0x20 */

typedef struct AnimHdr {
    u8 pad_00[0x2];
    u16 count;          /* 0x02 */
    u8 pad_04[0x4];
    s32 flags;          /* 0x08 */
    u8 pad_0C[0xC];
    struct { u8 pad_00[8]; u16 unk8; u8 pad_0A[2]; } entries[1];
    u8 pad_24[0x7C - 0x24];
    u16 unk7C;
} AnimHdr;

typedef struct AnimRes {
    u8 pad_00[0x18];
    AnimHdr *hdr;       /* 0x18 */
    u8 pad_1C[0x44 - 0x1C];
    s32 id;             /* 0x44 */
} AnimRes;

typedef struct AnimEvt {
    u16 type;
    u16 unk02;
    s32 arg;
} AnimEvt;

typedef struct AnimObj {
    AnimRes *res;       /* 0x00 */
    u8 pad_04;
    u8 tick;            /* 0x05 */
    s16 prevAnim;       /* 0x06 */
    s16 anim;           /* 0x08 */
    s16 nextAnim;       /* 0x0A */
    s16 prevFrame;      /* 0x0C */
    s16 frame;          /* 0x0E */
    s16 unk10;
    s16 unk12;
} AnimObj;

extern u32 D_80099B30;
extern s32 D_8007782C;
extern s32 D_8005B96C;
extern s32 func_8000CE08(AnimRes *res, s32 anim);
extern s32 func_8000CDC8(AnimRes *res, s32 anim);
extern s32 func_8000CEEC(AnimObj *obj, s32 anim);
extern AnimEvt *func_8000CE50(AnimRes *res, s32 anim, s32 frame);
extern void func_8000CCE4(AnimObj *obj, s32 arg);
extern s32 func_8000CD24(AnimRes *res, s32 anim, s32 arg);
extern int func_8001F138(s32 arg);
extern int func_8001F1C4(s32 arg);

#define ANIM_INFO(res, i) ((AnimInfo *)((u8 *)(res)->hdr + (i) * 12))

void func_8000C674(AnimObj *obj) {
    AnimRes *res = obj->res;
    s32 change;
    s32 done;
    s32 step;
    s32 f;
    s32 end;
    s32 start;
    s32 small;
    s32 pad;
    AnimEvt *ev;

    change = 1;
    done = 0;
    step = change;
    if (res->hdr == 0 || res->hdr->count == 0) {
        return;
    }
    if (obj->tick == (D_80099B30 & 0xFF)) {
        return;
    }
    obj->tick = D_80099B30;
    obj->prevAnim = obj->anim;
    obj->prevFrame = obj->frame;

    if (*(s32 *)&obj->anim == 0x10006) {
        if (obj->frame == func_8000CE08(res, obj->anim) >> 1 &&
            func_8000CE08(res, obj->anim) >= 16 &&
            res->hdr->count >= 7 && res->hdr->unk7C != 0) {
            obj->nextAnim = 7;
            obj->frame = func_8000CE08(res, obj->anim);
        }
    }

    if (res->hdr->flags & 8) {
        step += D_8007782C;
    }

    if (!(res->hdr->flags & 4)) {
        if (func_8000CEEC(obj, obj->anim) == 0) {
            goto other;
        }
        start = obj->frame;
        end = start + step - 1;
        small = step < 2;
        for (f = start; f <= end; f++) {
            if (f >= func_8000CE08(res, obj->anim) && !small) {
                goto overflow;
            }
            ev = func_8000CE50(res, obj->anim, f);
            switch (ev->type) {
                case 0:
                    break;
                case 1:
                    func_8001F138(ev->arg);
                    break;
                case 2:
                    func_8001F1C4(ev->arg);
                    if (obj->res->id == 0x8B) {
                        D_8005B96C = 1;
                    }
                    break;
            }
        }
        goto advance;
overflow:
        step = 1;
        {
            s32 n = func_8000CE08(res, obj->anim);
            done = step;
            obj->frame = n - 1;
        }
        func_8000CCE4(obj, 8);
        goto advance;
other:
        if (step >= 2) {
            pad = 0;
            start = obj->frame;
            end = start + step - 1;
            for (f = start; f <= end; f++) {
                if (f >= func_8000CE08(res, obj->anim) && pad == 0) {
                    goto overflow;
                }
            }
        }
advance:
        if (!(res->hdr->flags & 4)) {
            obj->frame += step;
        }
    }

    obj->unk12 = obj->unk10;
    done |= func_8000CD24(res, obj->anim, 1);

    if (res->hdr->entries[obj->anim].unk8 == 0) {
        if (obj->frame < func_8000CDC8(res, obj->anim)) {
            goto clear;
        }
        if (done == 0) {
            obj->frame = 0;
        } else {
            obj->frame = func_8000CDC8(res, obj->anim) - 1;
        }
    } else {
        if (obj->frame < func_8000CE08(res, obj->anim)) {
            goto clear;
        }
        if (done != 0) {
            obj->frame = func_8000CE08(res, obj->anim) - 1;
        } else {
            obj->frame = 0;
        }
    }
    res->hdr->flags |= 1;
    goto next;
clear:
    res->hdr->flags &= ~1;
next:

    if (obj->anim != obj->nextAnim) {
        change = 0;
        if (res->hdr->flags & 1) {
            obj->frame = 0;
            change = 1;
        }
    }
    if (change) {
        if (obj->anim != obj->nextAnim) {
            obj->unk10 = -1;
        }
        obj->anim = obj->nextAnim;
    }
    obj->unk12 = obj->unk10;
    if (res->hdr->entries[obj->anim].unk8 != 0) {
        obj->unk10 = func_8000CE50(res, obj->anim, obj->frame)->unk02;
    } else {
        obj->unk10 = obj->frame;
    }
}
