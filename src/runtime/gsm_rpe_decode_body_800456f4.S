.set noreorder
.set noat
.text
.globl func_800456F4
.ent func_800456F4
func_800456F4:
.set rpe_table, func_800456F4 + 0xD8
    nop
    addiu   $t7, $ra, 0xD4
    move    $ra, $at
    addu    $t7, $t7, $a0
    lb      $at, 0($t7)
    nop
    sra     $t0, $at, 4
    andi    $t1, $at, 7
    b       scale_ready
     nop
large_xmax:
    addiu   $t0, $a0, -8
    sra     $t0, $t0, 3
    andi    $t1, $a0, 7
scale_ready:
    sll     $t4, $t1, 11
    addiu   $t4, $t4, 0x47FF
    ori     $t2, $zero, 6
    subu    $t2, $t2, $t0
    ori     $t3, $zero, 1
    addiu   $at, $t2, -1
    sllv    $t3, $t3, $at
    ori     $t6, $zero, 13
    move    $t8, $a2
decode_loop:
    lh      $t5, 0($t8)
    nop
    sll     $t5, $t5, 13
    addiu   $t5, $t5, -0x7000
    mult    $t5, $t4
    mflo    $t5
    addiu   $t5, $t5, 0x4000
    sra     $t5, $t5, 15
    addu    $t5, $t5, $t3
    srav    $t5, $t5, $t2
    sh      $t5, 0($t8)
    addiu   $t6, $t6, -1
    bgtz    $t6, decode_loop
     addiu  $t8, $t8, 2
    move    $t9, $a3
    ori     $t6, $zero, 0x28
clear_loop:
    sh      $zero, 0($t9)
    addiu   $t6, $t6, -1
    bgtz    $t6, clear_loop
     addiu  $t9, $t9, 2
    addu    $t9, $a3, $a1
    addu    $t9, $t9, $a1
    move    $t8, $a2
    ori     $t6, $zero, 13
place_loop:
    lh      $at, 0($t8)
    addiu   $t8, $t8, 2
    sh      $at, 0($t9)
    addiu   $t6, $t6, -1
    bgtz    $t6, place_loop
     addiu  $t9, $t9, 6
    jr      $ra
     nop
.end func_800456F4
