.set noreorder
.set noat
.text
.globl func_8003B068
.ent func_8003B068
func_8003B068:
    xor     $v1, $v1, $v1
    sltu    $t9, $a2, $a0
    bnez    $t9, normalize_divisor
     nop
    bne     $a0, $a2, divisor_larger
     nop
    sltu    $t9, $a3, $a1
    bnez    $t9, normalize_divisor
     nop
    bne     $a1, $a3, divisor_larger
     nop
    xor     $a1, $a1, $a1
    xor     $a0, $a0, $a0
    li      $v1, 1
    jr      $ra
     xor    $v0, $v0, $v0
divisor_larger:
    jr      $ra
     xor    $v0, $v0, $v0

normalize_divisor:
    li      $v0, 1
    lui     $t0, 0x8000
normalize_loop:
    and     $t9, $a2, $t0
    bnez    $t9, divide_loop
     nop
    srl     $t9, $a3, 31
    sll     $a2, $a2, 1
    or      $a2, $a2, $t9
    sll     $a3, $a3, 1
    sltu    $t9, $a2, $a0
    bnez    $t9, keep_normalizing
     nop
    bne     $a0, $a2, normalize_done
     nop
    sltu    $t9, $a3, $a1
    beqz    $t9, normalize_done
     nop
keep_normalizing:
    j       normalize_loop
     sll    $v0, $v0, 1

normalize_done:
    sll     $t9, $a2, 31
    srl     $a3, $a3, 1
    or      $a3, $a3, $t9
    srl     $a2, $a2, 1
divide_loop:
    sltu    $t9, $a2, $a0
    bnez    $t9, subtract_divisor
     nop
    bne     $a0, $a2, shift_divisor
     nop
    sltu    $t9, $a1, $a3
    bnez    $t9, shift_divisor
     nop
subtract_divisor:
    sltu    $t9, $a1, $a3
    subu    $a1, $a1, $a3
    subu    $a0, $a0, $a2
    subu    $a0, $a0, $t9
    or      $v1, $v1, $v0
shift_divisor:
    sll     $t9, $a2, 31
    srl     $a3, $a3, 1
    or      $a3, $a3, $t9
    srl     $a2, $a2, 1
    srl     $v0, $v0, 1
    bnez    $v0, divide_loop
     nop
    jr      $ra
     xor    $v0, $v0, $v0
.end func_8003B068
