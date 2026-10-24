.set noreorder
.set noat
.text
.globl func_8003B400
.ent func_8003B400
func_8003B400:
    addiu   $sp, $sp, -8
    sdc1    $f12, 0($sp)
    lw      $a0, 0($sp)
    lw      $a1, 4($sp)
    addiu   $sp, $sp, 8
    move    $v1, $a0
    srl     $a0, $a0, 20
    andi    $a0, $a0, 0x7FF
    sltiu   $t9, $a0, 0x3FF
    bnez    $t9, overflow
     addiu  $a0, $a0, -0x3FF
    sltiu   $t9, $a0, 0x40
    beqz    $t9, below_one
     nop
    move    $a2, $v1
    move    $a3, $a1
    move    $v0, $v1
    lui     $t9, 0x000F
    ori     $t9, $t9, 0xFFFF
    and     $v0, $v0, $t9
    lui     $t9, 0x0010
    or      $v0, $v0, $t9
    sltiu   $t9, $a0, 0x34
    bnez    $t9, shift_right
     addiu  $a0, $a0, -0x34
    beqz    $a0, apply_sign
     nop
    move    $v1, $a0
    addiu   $a0, $a0, -0x20
    negu    $a0, $a0
    move    $a1, $a3
    srlv    $a1, $a1, $a0
    move    $a0, $v1
    sllv    $v0, $v0, $a0
    or      $v0, $v0, $a1
    j       apply_sign
     sllv   $a3, $a3, $a0
shift_right:
    negu    $a0, $a0
    move    $v1, $a0
    sltiu   $t9, $a0, 0x20
    bnez    $t9, short_shift
     addiu  $a0, $a0, -0x20
    move    $a3, $v0
    xor     $v0, $v0, $v0
    j       apply_sign
     srlv   $a3, $a3, $a0
short_shift:
    negu    $a0, $a0
    move    $a1, $v0
    sllv    $a1, $a1, $a0
    move    $a0, $v1
    srlv    $a3, $a3, $a0
    or      $a3, $a3, $a1
    srlv    $v0, $v0, $a0
apply_sign:
    lui     $t9, 0x8000
    and     $t9, $t9, $a2
    beqz    $t9, return_value
     nop
    li      $v1, -1
    xor     $a3, $a3, $v1
    xor     $v0, $v0, $v1
    addiu   $a3, $a3, 1
    sltiu   $t9, $a3, 1
    addu    $v0, $v0, $t9
return_value:
    jr      $ra
     move   $v1, $a3
below_one:
    li      $v1, 0
    jr      $ra
     lui    $v0, 0x8000
overflow:
    xor     $v0, $v0, $v0
    jr      $ra
     xor    $v1, $v1, $v1
    nop
.end func_8003B400
