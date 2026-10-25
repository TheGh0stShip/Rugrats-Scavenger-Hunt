typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct {
    u8 pad00[0x14];
    u16 *runs;
    u8 pad18[4];
    f32 *deltas;
} AnimModel;

typedef struct {
    u32 offset;
    u32 *frames;
    u16 flags;
    u16 ticks;
    u16 key_count;
    u16 control;
    u32 zero;
    u16 keys[1];
} AnimTrack;

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    u8 pad06[10];
} AnimVertex;

#define UNPACK_X(word) ((s32)((word) << 21) >> 21)
#define UNPACK_Y(word) ((s32)((word) << 10) >> 21)
#define UNPACK_Z(word) ((s32)(word) >> 22)

void func_8000B380(AnimModel *model, AnimTrack *track, u16 key,
                   AnimVertex *output, s32 delta)
{
    register u16 *runs __asm__("$11");
    register f32 *slopes __asm__("$8");
    register u32 *current __asm__("$17");
    register u32 *next __asm__("$16");
    register u16 key_count __asm__("$20");
    register s32 scale2 __asm__("$19");
    register s32 scale4 __asm__("$23");
    register AnimVertex *output_base __asm__("$18") = output;
    register AnimVertex *output_origin __asm__("$21") = output_base;
    register s32 stride __asm__("$7");
    s32 interpolate;
    register u32 span __asm__("$24");
    register s32 output_index __asm__("$25");
    register u16 *key_ticks __asm__("$2");

    output_index = 0;
    interpolate = 1;
    {
        register u16 *keys __asm__("$3") = track->keys;
        key_ticks = (u16 *)((u8 *)keys + ((u32)key << 1));
    }
    runs = model->runs;
    slopes = model->deltas;
    scale2 = (s16)(track->flags & 0x8000);
    scale4 = (s16)(track->flags & 0x2000);
    key_count = track->key_count;
    span = key_ticks[1] - key_ticks[0] - 1;
    if ((s32)span <= 0) {
        interpolate = 0;
        span = 1;
    }

    current = (u32 *)((u8 *)track->frames + ((u32)key << 2));
    stride = (u32)key_count << 2;
    next = current + 1;
    if (track->control & 0x80) {
        key_count--;
        stride = (u32)key_count << 2;
        if (key == key_count - 1) {
            next = track->frames;
        }
    }

    {
    register s32 do_interpolate __asm__("$22") = interpolate;
    while (*runs != 0) {
        s32 copied;

        if (*runs != 0) {
            s32 i;
            register AnimVertex *dst __asm__("$10");

            dst = (AnimVertex *)((u8 *)output_origin + ((u32)output_index << 4));
            for (i = 0; i < *runs; i++, output_index++, dst++) {
                u32 packed;
                register s32 x __asm__("$9");
                register s32 y __asm__("$12");
                register s32 z __asm__("$13");

                packed = *current;
                x = UNPACK_X(packed);
                y = UNPACK_Y(packed);
                z = UNPACK_Z(packed);
                if (scale2 != 0) {
                    x *= 2;
                    y *= 2;
                    z *= 2;
                } else if (scale4 != 0) {
                    x *= 4;
                    y *= 4;
                    z *= 4;
                }

                if (do_interpolate != 0) {
                    u32 packed_next;
                    register s32 nx __asm__("$5");
                    register s32 ny __asm__("$6");
                    register s32 nz __asm__("$14");

                    packed_next = *next;
                    nx = UNPACK_X(packed_next);
                    ny = UNPACK_Y(packed_next);
                    nz = UNPACK_Z(packed_next);
                    if (scale2 != 0) {
                        nx *= 2;
                        ny *= 2;
                        nz *= 2;
                    }
                    slopes[0] = (f32)((s16)nx - (s16)x) / (f32)span;
                    slopes[1] = (f32)((s16)ny - (s16)y) / (f32)span;
                    slopes[2] = (f32)((s16)nz - (s16)z) / (f32)span;
                    slopes += 3;
                }

                if (delta != 0) {
                    f32 fraction = (f32)delta;
                    dst->x = (s16)(s32)((f32)(s16)x + slopes[-3] * fraction);
                    dst->y = (s16)(s32)((f32)(s16)y + slopes[-2] * fraction);
                    dst->z = (s16)(s32)((f32)(s16)z + slopes[-1] * fraction);
                } else {
                    dst->x = (s16)x;
                    dst->y = (s16)y;
                    dst->z = (s16)z;
                }

                current = (u32 *)((u8 *)current + stride);
                next = (u32 *)((u8 *)next + stride);
            }
        }

        copied = runs[1];
        runs += 2;
        if (copied != 0) {
            __asm__ volatile("" : "+r"(copied));
            if (copied != 0) {
            s32 i;
            AnimVertex *dst;

            dst = (AnimVertex *)((u8 *)output_origin + ((u32)output_index << 4));
            i = 0;
            do {
                dst->x = output_base[*runs].x;
                dst->y = output_base[*runs].y;
                dst->z = output_base[*runs].z;
                i++;
                output_index++;
                dst++;
                runs++;
            } while (i < copied);
            }
        }
    }
    }
}
