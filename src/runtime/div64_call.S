.set noreorder
.set noat
.text

.globl func_8003B3C0
.ent func_8003B3C0
func_8003B3C0:
    bnez    $a2, 1f
     nop
    move    $t5, $ra
    jal     func_8003B164
     nop
    move    $ra, $t5
    move    $v1, $t1
    jr      $ra
     xor    $v0, $v0, $v0
1:
    move    $t5, $ra
    jal     func_8003B068
     nop
    move    $ra, $t5
    move    $v1, $a1
    jr      $ra
     move   $v0, $a0
.end func_8003B3C0
