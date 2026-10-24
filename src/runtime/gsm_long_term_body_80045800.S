.set noreorder
.set noat
.text
.globl func_80045800
.ent func_80045800
func_80045800:
.set gain_table, func_80045800 + 0x88
    nop
    addiu   $t0, $ra, 0x84
    move    $ra, $at
    addu    $t0, $t0, $a2
    addu    $t0, $t0, $a2
    lhu     $t0, 0($t0)
    ori     $t2, $zero, 0x77
    addiu   $t5, $a0, 0
history_shift:
    lhu     $t4, 0x50($t5)
    addiu   $t2, $t2, -1
    sh      $t4, 0($t5)
    bgez    $t2, history_shift
     addiu  $t5, $t5, 2
    ori     $t2, $zero, 0x27
    addiu   $t6, $a0, 0
    addiu   $t5, $a0, 0
    subu    $t5, $t5, $t3
    subu    $t5, $t5, $t3
    move    $t7, $a3
reconstruct_loop:
    lh      $t1, 0xF0($t5)
    addiu   $t5, $t5, 2
    mult    $t1, $t0
    mflo    $t1
    addiu   $t1, $t1, 0x4000
    sra     $t1, $t1, 15
    lh      $at, 0($t7)
    addiu   $t7, $t7, 2
    addu    $t1, $t1, $at
    sh      $t1, 0xF0($t6)
    addiu   $t6, $t6, 2
    bgtz    $t2, reconstruct_loop
     addiu  $t2, $t2, -1
    jr      $ra
     nop
.end func_80045800
