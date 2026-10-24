.set noreorder
.set noat
.text

.globl func_80045A54
.ent func_80045A54
func_80045A54:
.set synthesize_samples, func_80045A54 - 0x1C4
    addiu   $sp, $sp, -0x44
    sw      $ra, 0x00($sp)
    sw      $a0, 0x04($sp)
    sw      $a1, 0x08($sp)
    sw      $a2, 0x0C($sp)
    sw      $a3, 0x10($sp)
    sw      $s0, 0x14($sp)
    sw      $s1, 0x18($sp)
    sw      $s2, 0x1C($sp)
    sw      $s3, 0x20($sp)
    sw      $s4, 0x24($sp)
    sw      $s5, 0x28($sp)
    sw      $s6, 0x2C($sp)
    sw      $s7, 0x30($sp)
    addiu   $at, $a0, 0x140
    lw      $s0, 0x00($at)
    lw      $s1, 0x04($at)
    lw      $s2, 0x08($at)
    lw      $s3, 0x0C($at)
    lw      $s4, 0x10($at)
    lw      $s5, 0x14($at)
    lw      $s6, 0x18($at)
    lw      $s7, 0x1C($at)
    lh      $a3, 0x164($a0)

    bltzal  $zero, stage_quarter
     nop
    addiu   $t3, $ra, 0x190
    bal     interpolate_lar
     nop
    addiu   $t1, $a1, 0
    addiu   $t0, $a2, 0
    bal     synthesize_samples
     ori    $t3, $zero, 12

    bltzal  $zero, stage_half
     nop
    addiu   $t3, $ra, 0x184
    bal     interpolate_lar
     nop
    addiu   $t1, $a1, 0x1A
    addiu   $t0, $a2, 0x1A
    bal     synthesize_samples
     ori    $t3, $zero, 13

    bltzal  $zero, stage_three_quarters
     nop
    addiu   $t3, $ra, 0x170
    bal     interpolate_lar
     nop
    addiu   $t1, $a1, 0x36
    addiu   $t0, $a2, 0x36
    bal     synthesize_samples
     ori    $t3, $zero, 12

    bltzal  $zero, stage_direct
     nop
    addiu   $t3, $ra, 0x164
    bal     interpolate_lar
     nop
    addiu   $t1, $a1, 0x50
    addiu   $t0, $a2, 0x50
    bal     synthesize_samples
     ori    $t3, $zero, 0x77

    addiu   $at, $a0, 0x140
    sw      $s0, 0x00($at)
    sw      $s1, 0x04($at)
    sw      $s2, 0x08($at)
    sw      $s3, 0x0C($at)
    sw      $s4, 0x10($at)
    sw      $s5, 0x14($at)
    sw      $s6, 0x18($at)
    sw      $s7, 0x1C($at)
    sh      $a3, 0x164($a0)
    lw      $ra, 0x00($sp)
    lw      $a0, 0x04($sp)
    lw      $a1, 0x08($sp)
    lw      $a2, 0x0C($sp)
    lw      $a3, 0x10($sp)
    lw      $s0, 0x14($sp)
    lw      $s1, 0x18($sp)
    lw      $s2, 0x1C($sp)
    lw      $s3, 0x20($sp)
    lw      $s4, 0x24($sp)
    lw      $s5, 0x28($sp)
    lw      $s6, 0x2C($sp)
    lw      $s7, 0x30($sp)
    jr      $ra
     addiu  $sp, $sp, 0x44

interpolate_lar:
    lh      $at, 0x188($a0)
    addiu   $t0, $a0, 0x168
    beqz    $at, interpolate_ready
     addiu  $t1, $t0, 0x10
    move    $t0, $t1
    addiu   $t1, $a0, 0x168
interpolate_ready:
    move    $t5, $ra
    lh      $t2, 0x00($t0)
    jalr    $t3
     lh     $t4, 0x00($t1)
    sll     $t6, $t2, 16
    lh      $t2, 0x02($t0)
    jalr    $t3
     lh     $t4, 0x02($t1)
    or      $t6, $t6, $t2
    lh      $t2, 0x04($t0)
    jalr    $t3
     lh     $t4, 0x04($t1)
    sll     $t7, $t2, 16
    lh      $t2, 0x06($t0)
    jalr    $t3
     lh     $t4, 0x06($t1)
    or      $t7, $t7, $t2
    lh      $t2, 0x08($t0)
    jalr    $t3
     lh     $t4, 0x08($t1)
    sll     $t8, $t2, 16
    lh      $t2, 0x0A($t0)
    jalr    $t3
     lh     $t4, 0x0A($t1)
    or      $t8, $t8, $t2
    lh      $t2, 0x0C($t0)
    jalr    $t3
     lh     $t4, 0x0C($t1)
    sll     $t9, $t2, 16
    lh      $t2, 0x0E($t0)
    jalr    $t3
     lh     $t4, 0x0E($t1)
    move    $ra, $t5
    jr      $ra
     or     $t9, $t9, $t2

stage_quarter:
    sra     $t2, $t2, 2
    sra     $t4, $t4, 1
    addu    $t2, $t2, $t4
    sra     $t4, $t4, 1
    b       stage_direct
     addu   $t2, $t2, $t4
stage_half:
    sra     $t2, $t2, 1
    sra     $t4, $t4, 1
    b       stage_direct
     addu   $t2, $t2, $t4
stage_three_quarters:
    sra     $t2, $t2, 1
    sra     $t4, $t4, 2
    addu    $t4, $t4, $t2
    sra     $t2, $t2, 1
    b       stage_direct
     addu   $t2, $t2, $t4
stage_direct:
    bgez    $t2, magnitude_ready
     move   $t4, $zero
    negu    $t2, $t2
    ori     $t4, $zero, 1
magnitude_ready:
    addiu   $at, $t2, -0x2B33
    bltz    $at, magnitude_ready_for_clamp
     sll    $at, $t2, 1
    addiu   $at, $t2, -0x4E66
    bltz    $at, magnitude_ready_for_clamp
     addiu  $at, $t2, 0x2B33
    sra     $at, $t2, 2
    addiu   $at, $at, 0x6600
magnitude_ready_for_clamp:
    addiu   $t2, $at, -0x7FFF
    blez    $t2, magnitude_in_range
     move   $t2, $at
    ori     $t2, $zero, 0x7FFF
magnitude_in_range:
    bnez    $t4, stage_return
     negu   $t2, $t2
    negu    $t2, $t2
stage_return:
    jr      $ra
     andi   $t2, $t2, 0xFFFF
.end func_80045A54
