.set noreorder
.set noat
.text

.globl func_8004871C
.ent func_8004871C
func_8004871C:
    move    $t9, $a0
    lw      $t8, 0x0($a0)
    lw      $t7, 0x4($a1)
    lw      $t6, 0x4($t8)
    slt     $at, $t6, $t7
    bnez    $at, 2f
     nop
1:
    move    $t9, $t8
    lw      $t8, 0x0($t8)
    lw      $t6, 0x4($t8)
    slt     $at, $t6, $t7
    beqz    $at, 1b
     nop
2:
    lw      $t8, 0x0($t9)
    sw      $t8, 0x0($a1)
    sw      $a1, 0x0($t9)
    jr      $ra
     sw     $a0, 0x8($a1)
.end func_8004871C
