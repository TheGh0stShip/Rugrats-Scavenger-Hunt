/* Target: func_8000E32C  VRAM 0x8000E32C  ROM 0xEF2C  Size: 0x300. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;
typedef float f32;

typedef struct {
    f32 x, y, z;
} Vec3f;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
    s32 velocity;
    s32 phase;
    u8 pad_14[4];
    f32 scale;
} Particle;

typedef struct {
    u8 pad_00[0x2C];
    f32 x;
    f32 y;
    f32 z;
    u8 pad_38[0xF4 - 0x38];
    s32 slot;
    u8 pad_F8[0x13C - 0xF8];
    u8 row;
    u8 column;
    u8 pad_13E[0x12];
    s16 room;
} ParticleSource;

typedef struct {
    f32 x;
    f32 z;
} PositionOffset;

typedef struct {
    u8 pad_00[4];
    s16 count;
} RoomState;

extern Particle D_80058208[][5];
extern PositionOffset D_80052F90[];
extern RoomState *D_800581F0;
extern s8 D_800581E9;
extern s32 D_80057D5C;
extern u8 *D_80052F04;
extern u8 D_80059CF0[];
extern s32 func_8000E078(s32 column, s32 row);
extern f32 func_800475F0(f32 angle);
extern s32 func_8003A840(void);
extern void *func_80005544(void);
extern void *func_8001E630(void);
extern void func_8000CF64(void *matrix, f32 rx, f32 ry, f32 rz,
                          f32 scale, f32 x, f32 y, f32 z);
extern u8 *func_800197C8(s32 id);
extern void *func_8000B910(void *model, void *position, void *camera,
                           void *display, void *matrix, int flags);

void func_8000E32C(ParticleSource *source) {
    s32 slot = source->slot;
    register s32 particle_offset __asm__("$18");
    register s32 i __asm__("$19");
    register Particle *particle_base __asm__("$20");
    register PositionOffset *offset_base __asm__("$21");
    Particle *particle;
    Particle *draw_particle;
    Vec3f position;
    void *matrix;
    f32 x;

    if (func_8000E078(source->column, source->row) == D_800581F0->count - 1) {
        return;
    }
    if (source == 0 || D_800581E9 != source->room) {
        return;
    }
    particle_base = &D_80058208[0][0];
    offset_base = D_80052F90;
    for (i = 0, particle_offset = slot * 0x8C;
         i < 5;
         i++) {
        particle = (Particle *)((u8 *)particle_base + particle_offset);
        x = particle->x;
        particle->y += particle->velocity;
        particle->phase++;
        x += func_800475F0(particle->phase) * 5.0f;
        particle->x = x;
        if (source->y + 1000.0f < particle->y) {
            particle->x = source->x;
            particle->y = source->y + 300.0f;
            particle->phase = 0;
            particle->z = source->z;
            particle->velocity = (func_8003A840() % 6 + 1) * 4;
            if (particle->velocity < 2) {
                particle->velocity = 2;
            }
            particle->scale = func_8003A840() / 65535.0f;
            if (particle->scale < 0.75) {
                particle->scale = 0.75f;
            }
        }
        position.x = ((Particle *)((u8 *)particle_base + particle_offset))->x;
        position.y = ((Particle *)((u8 *)particle_base + particle_offset))->y;
        position.z = ((Particle *)((u8 *)particle_base + particle_offset))->z;
        if (func_80005544() != source && source->slot <= D_80057D5C - 1) {
            position.x += offset_base[source->slot].x;
            position.z += offset_base[source->slot].z;
        }
        matrix = func_8001E630();
        draw_particle = (Particle *)((u8 *)particle_base + particle_offset);
        particle_offset += sizeof(Particle);
        func_8000CF64(matrix, 0.0f, 0.0f, 0.0f,
                      draw_particle->scale,
                      position.x, position.y, position.z);
        func_8000B910(func_800197C8(0xE1) + 0x2C, &position,
                      D_80052F04 + 0x28, D_80059CF0, matrix, 0);
    }
}
