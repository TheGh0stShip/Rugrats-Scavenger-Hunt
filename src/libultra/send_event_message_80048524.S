.set noreorder
.set noat
.text

.globl func_80048524
.ent func_80048524
func_80048524:
    move    $s2, $ra
    lui     $t2, 0x800C
    addiu   $t2, $t2, 0x44D0
    addu    $t2, $t2, $a0
    lw      $t1, 0x0($t2)
    beqz    $t1, 2f
     nop
    lw      $t3, 0x8($t1)
    lw      $t4, 0x10($t1)
    slt     $at, $t3, $t4
    beqz    $at, 2f
     nop
    lw      $t5, 0xC($t1)
    addu    $t5, $t5, $t3
    div     $zero, $t5, $t4
    bnez    $t4, 1f
     nop
    .word   0x0007000D
1:
    addiu   $at, $zero, -1
    bne     $t4, $at, 3f
     lui    $at, 0x8000
    bne     $t5, $at, 3f
     nop
    .word   0x0006000D
3:
    mfhi    $t5
    lw      $t4, 0x14($t1)
    addiu   $at, $zero, 4
    mult    $t5, $at
    mflo    $t5
    addu    $t4, $t4, $t5
    lw      $t5, 0x4($t2)
    sw      $t5, 0x0($t4)
    addiu   $t2, $t3, 1
    sw      $t2, 0x8($t1)
    lw      $t2, 0x0($t1)
    lw      $t3, 0x0($t2)
    beqz    $t3, 2f
     nop
    jal     func_80048764
     move   $a0, $t1
    move    $t2, $v0
    move    $a1, $t2
    lui     $a0, 0x8005
    jal     func_8004871C
     addiu  $a0, $a0, 0x6068
2:
    jr      $s2
     nop
.end func_80048524
