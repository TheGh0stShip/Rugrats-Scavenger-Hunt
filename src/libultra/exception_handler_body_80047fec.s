#define THREAD_NEXT 0x00
#define THREAD_PRI 0x04
#define THREAD_STATE 0x10
#define THREAD_FLAGS 0x12
#define THREAD_FP 0x18
#define THREAD_GP1 0x20
#define THREAD_GP2 0x28
#define THREAD_GP3 0x30
#define THREAD_GP4 0x38
#define THREAD_GP5 0x40
#define THREAD_GP6 0x48
#define THREAD_GP7 0x50
#define THREAD_GP8 0x58
#define THREAD_GP9 0x60
#define THREAD_GP10 0x68
#define THREAD_GP11 0x70
#define THREAD_GP12 0x78
#define THREAD_GP13 0x80
#define THREAD_GP14 0x88
#define THREAD_GP15 0x90
#define THREAD_GP16 0x98
#define THREAD_GP17 0xA0
#define THREAD_GP18 0xA8
#define THREAD_GP19 0xB0
#define THREAD_GP20 0xB8
#define THREAD_GP21 0xC0
#define THREAD_GP22 0xC8
#define THREAD_GP23 0xD0
#define THREAD_GP24 0xD8
#define THREAD_GP25 0xE0
#define THREAD_GP28 0xE8
#define THREAD_GP29 0xF0
#define THREAD_GP30 0xF8
#define THREAD_GP31 0x100
#define THREAD_LO 0x108
#define THREAD_HI 0x110
#define THREAD_SR 0x118
#define THREAD_PC 0x11C
#define THREAD_CAUSE 0x120
#define THREAD_BADVADDR 0x124
#define THREAD_RCP 0x128
#define THREAD_FPCSR 0x12C
#define THREAD_FP0 0x130
#define THREAD_FP2 0x138
#define THREAD_FP4 0x140
#define THREAD_FP6 0x148
#define THREAD_FP8 0x150
#define THREAD_FP10 0x158
#define THREAD_FP12 0x160
#define THREAD_FP14 0x168
#define THREAD_FP16 0x170
#define THREAD_FP18 0x178
#define THREAD_FP20 0x180
#define THREAD_FP22 0x188
#define THREAD_FP24 0x190
#define THREAD_FP26 0x198
#define THREAD_FP28 0x1A0
#define THREAD_FP30 0x1A8

.set noreorder
.set noat
.text

.globl func_80047FEC
.ent func_80047FEC
func_80047FEC:
.set handle_cop_unusable, func_80047FEC + 0x5F4
    sw      $zero, THREAD_FP($k0)
    mfc0    $t0, $13
savecontext:
    move    $t0, $k0
    lui     $k0, 0x8005
    lw      $k0, 0x6070($k0)
    ld      $t1, THREAD_GP1($t0)
    sd      $t1, THREAD_GP1($k0)
    ld      $t1, THREAD_SR($t0)
    sd      $t1, THREAD_SR($k0)
    ld      $t1, THREAD_GP8($t0)
    sd      $t1, THREAD_GP8($k0)
    ld      $t1, THREAD_GP9($t0)
    sd      $t1, THREAD_GP9($k0)
    ld      $t1, THREAD_GP10($t0)
    sd      $t1, THREAD_GP10($k0)
    sd      $v0, THREAD_GP2($k0)
    sd      $v1, THREAD_GP3($k0)
    sd      $a0, THREAD_GP4($k0)
    sd      $a1, THREAD_GP5($k0)
    sd      $a2, THREAD_GP6($k0)
    sd      $a3, THREAD_GP7($k0)
    sd      $t3, THREAD_GP11($k0)
    sd      $t4, THREAD_GP12($k0)
    sd      $t5, THREAD_GP13($k0)
    sd      $t6, THREAD_GP14($k0)
    sd      $t7, THREAD_GP15($k0)
    sd      $s0, THREAD_GP16($k0)
    sd      $s1, THREAD_GP17($k0)
    sd      $s2, THREAD_GP18($k0)
    sd      $s3, THREAD_GP19($k0)
    sd      $s4, THREAD_GP20($k0)
    sd      $s5, THREAD_GP21($k0)
    sd      $s6, THREAD_GP22($k0)
    sd      $s7, THREAD_GP23($k0)
    sd      $t8, THREAD_GP24($k0)
    sd      $t9, THREAD_GP25($k0)
    sd      $gp, THREAD_GP28($k0)
    sd      $sp, THREAD_GP29($k0)
    sd      $fp, THREAD_GP30($k0)
    sd      $ra, THREAD_GP31($k0)
    mflo    $t0
    sd      $t0, THREAD_LO($k0)
    mfhi    $t0
    sd      $t0, THREAD_HI($k0)
    lw      $k1, THREAD_SR($k0)
    andi    $t1, $k1, 0xFF00
    beqz    $t1, savercp
     nop
    lui     $t0, 0x8005
    addiu   $t0, $t0, 0x6050
    lw      $t0, 0($t0)
    li      $at, -1
    xor     $t2, $t0, $at
    andi    $t2, $t2, 0xFF00
    or      $t4, $t1, $t2
    lui     $at, 0xFFFF
    ori     $at, $at, 0x00FF
    and     $t3, $k1, $at
    or      $t3, $t3, $t4
    sw      $t3, THREAD_SR($k0)
    andi    $t0, $t0, 0xFF00
    and     $t1, $t1, $t0
    lui     $at, 0xFFFF
    ori     $at, $at, 0x00FF
    and     $k1, $k1, $at
    or      $k1, $k1, $t1
savercp:
    lui     $t1, 0xA430
    lw      $t1, 0x000C($t1)
    beqz    $t1, endrcp
     nop
    lui     $t0, 0x8005
    addiu   $t0, $t0, 0x6050
    lw      $t0, 0($t0)
    srl     $t0, $t0, 16
    li      $at, -1
    xor     $t0, $t0, $at
    andi    $t0, $t0, 0x3F
    lw      $t4, THREAD_RCP($k0)
    and     $t0, $t0, $t4
    or      $t1, $t1, $t0
endrcp:
    sw      $t1, THREAD_RCP($k0)
    mfc0    $t0, $14
    sw      $t0, THREAD_PC($k0)
    lw      $t0, THREAD_FP($k0)
    beqz    $t0, fp_done
     nop
    cfc1    $t0, $31
    nop
    sw      $t0, THREAD_FPCSR($k0)
    sdc1    $f0, THREAD_FP0($k0)
    sdc1    $f2, THREAD_FP2($k0)
    sdc1    $f4, THREAD_FP4($k0)
    sdc1    $f6, THREAD_FP6($k0)
    sdc1    $f8, THREAD_FP8($k0)
    sdc1    $f10, THREAD_FP10($k0)
    sdc1    $f12, THREAD_FP12($k0)
    sdc1    $f14, THREAD_FP14($k0)
    sdc1    $f16, THREAD_FP16($k0)
    sdc1    $f18, THREAD_FP18($k0)
    sdc1    $f20, THREAD_FP20($k0)
    sdc1    $f22, THREAD_FP22($k0)
    sdc1    $f24, THREAD_FP24($k0)
    sdc1    $f26, THREAD_FP26($k0)
    sdc1    $f28, THREAD_FP28($k0)
    sdc1    $f30, THREAD_FP30($k0)
fp_done:
    mfc0    $t0, $13
    sw      $t0, THREAD_CAUSE($k0)
    li      $t1, 2
    sh      $t1, THREAD_STATE($k0)
    andi    $t1, $t0, 0x7C
    li      $t2, 0x24
    beq     $t1, $t2, handle_break
     nop
    li      $t2, 0x2C
    beq     $t1, $t2, handle_cop_unusable
     nop
    li      $t2, 0
    bne     $t1, $t2, panic
     nop
    and     $s0, $k1, $t0
next_interrupt:
    andi    $t1, $s0, 0xFF00
    srl     $t2, $t1, 12
    bnez    $t2, interrupt_index_ready
     nop
    srl     $t2, $t1, 8
    addi    $t2, $t2, 16
interrupt_index_ready:
    lui     $at, 0x8000
    addu    $at, $at, $t2
    lbu     $t2, 0x3690($at)
    lui     $at, 0x8000
    addu    $at, $at, $t2
    lw      $t2, 0x36B0($at)
    jr      $t2
     nop

IP6_Hdlr:
    li      $at, -0x2001
    b       next_interrupt
     and    $s0, $s0, $at
IP7_Hdlr:
    li      $at, -0x4001
    b       next_interrupt
     and    $s0, $s0, $at
counter:
    mfc0    $t1, $11
    mtc0    $t1, $11
    li      $a0, 0x18
    jal     func_80048524
     nop
    lui     $at, 0xFFFF
    ori     $at, $at, 0x7FFF
    b       next_interrupt
     and    $s0, $s0, $at
cart:
    li      $a0, 0x10
    li      $at, -0x801
    and     $s0, $s0, $at
    lui     $sp, 0x800C
    addiu   $sp, $sp, 0x5BA0
    addiu   $sp, $sp, 0x0FF0
    li      $t2, 4
    lui     $at, 0x8005
    addu    $at, $at, $t2
    lw      $t2, 0x6020($at)
    beqz    $t2, cart_message
     nop
    jalr    $t2
     nop
    beqz    $v0, cart_message
     li     $a0, 0x10
    b       redispatch
     nop
cart_message:
    jal     func_80048524
     nop
    b       next_interrupt
     nop

rcp:
    lui     $s1, 0xA430
    lw      $s1, 8($s1)
    lui     $t0, 0x8005
    addiu   $t0, $t0, 0x6050
    lw      $t0, 0($t0)
    srl     $t0, $t0, 16
    and     $s1, $s1, $t0
    andi    $t1, $s1, 1
    beqz    $t1, video_interrupt
     nop
    andi    $s1, $s1, 0x3E
    lui     $t4, 0xA404
    lw      $t4, 0x10($t4)
    ori     $t1, $zero, 0x8008
    lui     $at, 0xA404
    sw      $t1, 0x10($at)
    andi    $t4, $t4, 0x300
    beqz    $t4, sp_break
     nop
    jal     func_80048524
     li     $a0, 0x20
    beqz    $s1, no_more_rcp
     nop
    b       video_interrupt
     nop
sp_break:
    jal     func_80048524
     li     $a0, 0x58
    beqz    $s1, no_more_rcp
     nop
video_interrupt:
    andi    $t1, $s1, 8
    beqz    $t1, audio_interrupt
     nop
    andi    $s1, $s1, 0x37
    lui     $at, 0xA440
    sw      $zero, 0x10($at)
    jal     func_80048524
     li     $a0, 0x38
    beqz    $s1, no_more_rcp
     nop
audio_interrupt:
    andi    $t1, $s1, 4
    beqz    $t1, serial_interrupt
     nop
    andi    $s1, $s1, 0x3B
    li      $t1, 1
    lui     $at, 0xA450
    sw      $t1, 0x0C($at)
    jal     func_80048524
     li     $a0, 0x30
    beqz    $s1, no_more_rcp
     nop
serial_interrupt:
    andi    $t1, $s1, 2
    beqz    $t1, parallel_interrupt
     nop
    andi    $s1, $s1, 0x3D
    lui     $at, 0xA480
    sw      $zero, 0x18($at)
    jal     func_80048524
     li     $a0, 0x28
    beqz    $s1, no_more_rcp
     nop
parallel_interrupt:
    andi    $t1, $s1, 0x10
    beqz    $t1, display_interrupt
     nop
    andi    $s1, $s1, 0x2F
    li      $t1, 2
    lui     $at, 0xA460
    sw      $t1, 0x10($at)
    jal     func_80048524
     li     $a0, 0x40
    beqz    $s1, no_more_rcp
     nop
display_interrupt:
    andi    $t1, $s1, 0x20
    beqz    $t1, no_more_rcp
     nop
    andi    $s1, $s1, 0x1F
    li      $t1, 0x800
    lui     $at, 0xA430
    sw      $t1, 0($at)
    jal     func_80048524
     li     $a0, 0x48
no_more_rcp:
    li      $at, -0x401
    b       next_interrupt
     and    $s0, $s0, $at

prenmi:
    lw      $k1, THREAD_SR($k0)
    li      $at, -0x1001
    and     $k1, $k1, $at
    sw      $k1, THREAD_SR($k0)
    lui     $t1, 0x8005
    addiu   $t1, $t1, 0x604C
    lw      $t2, 0($t1)
    beqz    $t2, first_nmi
     nop
    li      $at, -0x1001
    b       redispatch
     and    $s0, $s0, $at
first_nmi:
    li      $t2, 1
    sw      $t2, 0($t1)
    jal     func_80048524
     li     $a0, 0x70
    li      $at, -0x1001
    and     $s0, $s0, $at
    lui     $t2, 0x8005
    lw      $t2, 0x6068($t2)
    lw      $k1, THREAD_SR($t2)
    li      $at, -0x1001
    and     $k1, $k1, $at
    b       redispatch
     sw     $k1, THREAD_SR($t2)

sw2:
    li      $at, -0x201
    and     $t0, $t0, $at
    mtc0    $t0, $13
    li      $a0, 8
    jal     func_80048524
     nop
    li      $at, -0x201
    b       next_interrupt
     and    $s0, $s0, $at
sw1:
    li      $at, -0x101
    and     $t0, $t0, $at
    mtc0    $t0, $13
    li      $a0, 0
    jal     func_80048524
     nop
    li      $at, -0x101
    b       next_interrupt
     and    $s0, $s0, $at

handle_break:
    li      $t1, 1
    sh      $t1, THREAD_FLAGS($k0)
    jal     func_80048524
     li     $a0, 0x50
    b       redispatch
     nop
redispatch:
    lw      $t1, THREAD_PRI($k0)
    lui     $t2, 0x8005
    lw      $t2, 0x6068($t2)
    lw      $t3, THREAD_PRI($t2)
    slt     $at, $t1, $t3
    beqz    $at, enqueue_running
     nop
    move    $a1, $k0
    lui     $a0, 0x8005
    jal     func_8004871C
     addiu  $a0, $a0, 0x6068
    j       func_80048774
     nop
enqueue_running:
    lui     $t1, 0x8005
    addiu   $t1, $t1, 0x6068
    lw      $t2, 0($t1)
    sw      $t2, THREAD_NEXT($k0)
    j       func_80048774
     sw     $k0, 0($t1)

panic:
    lui     $at, 0x8005
    sw      $k0, 0x6074($at)
    li      $t1, 1
    sh      $t1, THREAD_STATE($k0)
    li      $t1, 2
    sh      $t1, THREAD_FLAGS($k0)
    mfc0    $t2, $8
    sw      $t2, THREAD_BADVADDR($k0)
    jal     func_80048524
     li     $a0, 0x60
    j       func_80048774
     nop
.end func_80047FEC
