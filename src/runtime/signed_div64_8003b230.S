.set noreorder
.set noat
.text
.globl func_8003B230
.ent func_8003B230
func_8003B230:
.set unsigned_divide, func_8003B230 + 0x170
    lui     $t0, 0x8000
    move    $v1, $a0
    xor     $v1, $v1, $a2
    and     $t9, $t0, $a0
    beqz    $t9, dividend_ready
     nop
    li      $t1, -1
    xor     $a1, $a1, $t1
    xor     $a0, $a0, $t1
    addiu   $a1, $a1, 1
    sltiu   $t9, $a1, 1
    addu    $a0, $a0, $t9
dividend_ready:
    and     $t9, $t0, $a2
    beqz    $t9, divisor_ready
     nop
    li      $t1, -1
    xor     $a3, $a3, $t1
    xor     $a2, $a2, $t1
    addiu   $a3, $a3, 1
    sltiu   $t9, $a3, 1
    addu    $a2, $a2, $t9
divisor_ready:
    and     $v1, $v1, $t0
    beqz    $v1, unsigned_divide
     nop
    bnez    $a2, wide_divisor
     nop
    move    $t5, $ra
    jal     func_8003B164
     nop
    j       negate_result
     move   $ra, $t5
wide_divisor:
    move    $t5, $ra
    jal     func_8003B068
     nop
    move    $ra, $t5
negate_result:
    li      $a3, -1
    xor     $v1, $v1, $a3
    xor     $v0, $v0, $a3
    addiu   $v1, $v1, 1
    sltiu   $t9, $v1, 1
    jr      $ra
     addu   $v0, $v0, $t9
    nop
    nop
.end func_8003B230
