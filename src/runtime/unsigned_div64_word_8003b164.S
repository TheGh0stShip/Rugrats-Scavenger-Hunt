.set noreorder
.set noat
.text
.globl func_8003B164
.ent func_8003B164
func_8003B164:
    divu    $zero, $a0, $a3
    bnez    $a3, high_divided
     nop
    break   7
high_divided:
    mflo    $t2
    mfhi    $a0
    xor     $t0, $t0, $t0
divide_high_remainder:
    beqz    $a0, divide_low
     nop
    move    $v1, $a1
    move    $v0, $a0
    move    $t1, $a3
normalize_loop:
    sll     $t9, $v0, 31
    srl     $v1, $v1, 1
    or      $v1, $v1, $t9
    srl     $v0, $v0, 1
    bnez    $v0, normalize_loop
     srl    $t1, $t1, 1
    addiu   $t1, $t1, 1
    divu    $zero, $v1, $t1
    bnez    $t1, estimate_ready
     nop
    break   7
estimate_ready:
    mflo    $v1
    xor     $v0, $v0, $v0
    addu    $t0, $t0, $v1
    sltu    $t9, $t0, $v1
    addu    $t2, $t2, $v0
    addu    $t2, $t2, $t9
    nop
    multu   $v1, $a3
    mflo    $v1
    mfhi    $v0
    sltu    $t9, $a1, $v1
    subu    $a1, $a1, $v1
    subu    $a0, $a0, $v0
    j       divide_high_remainder
     subu   $a0, $a0, $t9
divide_low:
    divu    $zero, $a1, $a3
    bnez    $a3, low_divided
     nop
    break   7
low_divided:
    mflo    $v1
    mfhi    $t1
    xor     $v0, $v0, $v0
    addu    $v1, $v1, $t0
    sltu    $t9, $v1, $t0
    addu    $v0, $v0, $t2
    jr      $ra
     addu   $v0, $v0, $t9
    nop
.end func_8003B164
