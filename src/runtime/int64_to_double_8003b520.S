.set noreorder
.set noat
.text
.globl func_8003B520
.ent func_8003B520
func_8003B520:
    move    $v1, $a1
    move    $v0, $a0
    or      $a0, $a0, $a1
    bnez    $a0, nonzero
     nop
    addiu   $sp, $sp, -8
    sw      $v0, 0($sp)
    sw      $v1, 4($sp)
    ldc1    $f0, 0($sp)
    jr      $ra
     addiu  $sp, $sp, 8
nonzero:
    xor     $a2, $a2, $a2
    lui     $a0, 0x8000
    and     $t9, $a0, $v0
    beqz    $t9, magnitude_ready
     nop
    li      $a3, -1
    xor     $v1, $v1, $a3
    xor     $v0, $v0, $a3
    addiu   $v1, $v1, 1
    sltiu   $t9, $v1, 1
    addu    $v0, $v0, $t9
    move    $a2, $a0
magnitude_ready:
    xor     $a0, $a0, $a0
    lui     $t9, 0xFFE0
    and     $t9, $t9, $v0
    beqz    $t9, left_normalize_test
     nop
right_normalize:
    sll     $t9, $v0, 31
    srl     $v1, $v1, 1
    or      $v1, $v1, $t9
    srl     $v0, $v0, 1
    addiu   $a0, $a0, 1
    lui     $t9, 0xFFE0
    and     $t9, $t9, $v0
    bnez    $t9, right_normalize
     nop
    j       pack_double
     nop
left_normalize_test:
    lui     $t9, 0x0010
left_normalize:
    and     $t9, $t9, $v0
    bnez    $t9, pack_double
     nop
    srl     $t9, $v1, 31
    sll     $v0, $v0, 1
    or      $v0, $v0, $t9
    sll     $v1, $v1, 1
    j       left_normalize_test
     addiu  $a0, $a0, -1
pack_double:
    lui     $t9, 0x000F
    ori     $t9, $t9, 0xFFFF
    and     $v0, $v0, $t9
    or      $v0, $v0, $a2
    addiu   $a0, $a0, 0x433
    sll     $a0, $a0, 20
    or      $v0, $v0, $a0
    addiu   $sp, $sp, -8
    sw      $v0, 0($sp)
    sw      $v1, 4($sp)
    ldc1    $f0, 0($sp)
    jr      $ra
     addiu  $sp, $sp, 8
    nop
    nop
.end func_8003B520
