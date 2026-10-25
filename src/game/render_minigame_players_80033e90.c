typedef unsigned char u8;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad00[0xF8]; s32 character; u8 padFC[0xB0]; s32 models[4]; } Player;
typedef struct { u8 pad00[0x2C]; u8 model[1]; } Resource;
typedef struct { u8 pad00[0x28]; Vec3f pos; } Camera;
typedef struct {
    Vec3f position;
    s32 unused;
    Player *player;
    s32 base_model_id;
    s32 clear_mask;
    Vec3f *primary_position;
} RenderStack;

extern s32 D_80052F18;
extern Camera *D_80052F04;
extern s32 D_80057D5C;
extern Vec3f D_80054840[];
extern Vec3f D_8005487C[];
extern Vec3f D_800548F4[];
extern Vec3f D_80054870;
extern u8 D_80059CF0[];

extern Player *func_800055A4(s32);
extern void func_8000B910(void *, Vec3f *, Vec3f *, void *, void *, s32);
extern Resource *func_800197C8(s32);
extern void *func_8001E630(void);
extern s32 func_80025CCC(s32);
extern void func_80047A38(void *, f32, f32, f32, f32, f32, f32, f32);

void func_80033E90(void)
{
    register s32 flags __asm__("$19") = 0x20000000;
    register s32 player_index __asm__("$20") = 0;
    register s32 slot __asm__("$21");
    register s32 position_offset __asm__("$22");
    register Vec3f *alternate_position __asm__("$23");
    s32 model_id;
    s32 player_times_three;
    register s32 angle_base __asm__("$30");
    register s32 angle_twice __asm__("$17");
    RenderStack local;
    register void *matrix __asm__("$18");
    register void *model __asm__("$16");
    register f32 zero;

    if (D_80057D5C > 0) {
        goto get_player;
    }
    if (D_80052F18 != 0) {
        goto finished;
    }

missing_player:
    __asm__ volatile("" : "+r"(player_index) : : "memory");
    slot = 0;
    if (player_index < D_80057D5C) {
get_player:
        local.player = func_800055A4(player_index);
        slot = 0;
    }

render_setup:
    local.base_model_id = player_index * 4 + 0x180;
    __asm__ volatile("" ::: "memory");
    local.clear_mask = 0xFEFFFFFF;
    player_times_three = player_index * 3;
    position_offset = player_times_three * 4;
    local.primary_position = &D_80054840[player_index];
    angle_base = player_times_three * 15;
    alternate_position = &D_8005487C[player_index];
    zero = 0.0f;

render_slot:
            if (D_80052F18 == 0) {
                model_id = slot + local.base_model_id;
                if (func_80025CCC(model_id) != 0) {
                    flags |= 0x01000000;
                }
            } else {
                model_id = local.player->models[slot];
                if (model_id < 0) {
                    flags |= 0x01000000;
                    model_id &= 0x7FFFFFFF;
                } else {
                    flags &= local.clear_mask;
                }
            }
            model = func_800197C8(model_id)->model;
            if (D_80052F18 == 0) {
                local.position = *local.primary_position;
            } else {
                local.position = *alternate_position;
            }

            __asm__ volatile("sll %0,$30,1" : "=r"(angle_twice));
            matrix = func_8001E630();
            func_80047A38(matrix, zero, (f32)angle_twice, zero,
                          1.0f, local.position.x, local.position.y, local.position.z);
            if (flags & 0x01000000) {
                func_8000B910(model, &local.position, &local.position,
                              D_80059CF0, matrix, flags);
            } else {
                func_8000B910(model, &local.position, &D_80052F04->pos,
                              D_80059CF0, matrix, flags);
            }
            flags &= local.clear_mask;

            if (D_80052F18 == 1) {
                local.position = *(Vec3f *)((u8 *)D_800548F4 + position_offset);
                func_8000B910(func_800197C8(0x194)->model, &local.position,
                              &D_80052F04->pos, D_80059CF0, matrix, flags);
                local.position = *alternate_position;
                func_8000B910(func_800197C8(local.player->character + 0x195)->model,
                              &local.position, &D_80052F04->pos,
                              D_80059CF0, matrix, flags);
            }

    slot++;
    if (slot < 4) {
        goto render_slot;
    }
    player_index++;
    if (player_index < D_80057D5C) {
        goto get_player;
    }
    if (player_index < 4 && D_80052F18 == 0) {
        goto missing_player;
    }

    if (D_80052F18 == 0) {
        register u8 *model_cursor __asm__("$17");
        register Vec3f *fixed_position;
        model_cursor = (u8 *)func_800055A4(D_80057D5C + 2);
        fixed_position = D_80054840;
        __asm__ volatile("" : "+r"(fixed_position));
        fixed_position += 4;
        __asm__ volatile("mtc1 $0,$f20");
        zero = 0.0f;
        slot = 0;
        do {
            model_id = *(s32 *)(model_cursor + 0x1AC);
            if (model_id < 0) {
                flags |= 0x01000000;
                model_id &= 0x7FFFFFFF;
            } else {
                flags &= ~0x01000000;
            }
            model = func_800197C8(model_id)->model;
            local.position = *fixed_position;
            matrix = func_8001E630();
            func_80047A38(matrix, zero, zero, zero, 1.0f,
                          local.position.x, local.position.y, local.position.z);
            if (flags & 0x01000000) {
                func_8000B910(model, &local.position, &local.position,
                              D_80059CF0, matrix, flags);
            } else {
                func_8000B910(model, &local.position, &D_80052F04->pos,
                              D_80059CF0, matrix, flags);
            }
            model_cursor += 4;
            slot++;
        } while (slot < 4);
    }
finished:
    return;
}
