.set noreorder
.set noat
.text

.globl func_80051050
.ent func_80051050
func_80051050:
    negu    $v1, $a0
    slti    $at, $a1, 12
    bnez    $at, 3f
     nop
    andi    $v1, $v1, 3
    beqz    $v1, 1f
     subu   $a1, $a1, $v1
    swl     $zero, 0x0($a0)
    addu    $a0, $a0, $v1
1:
    addiu   $at, $zero, -32
    and     $a3, $a1, $at
    beqz    $a3, 2f
     subu   $a1, $a1, $a3
    addu    $a3, $a3, $a0
4:
    sw      $zero, 0x0($a0)
    sw      $zero, 0x4($a0)
    sw      $zero, 0x8($a0)
    sw      $zero, 0xC($a0)
    addiu   $a0, $a0, 32
    sw      $zero, -16($a0)
    sw      $zero, -12($a0)
    sw      $zero, -8($a0)
    bne     $a0, $a3, 4b
     sw     $zero, -4($a0)
2:
    addiu   $at, $zero, -4
    and     $a3, $a1, $at
    beqz    $a3, 3f
     subu   $a1, $a1, $a3
    addu    $a3, $a3, $a0
5:
    addiu   $a0, $a0, 4
    bne     $a0, $a3, 5b
     sw     $zero, -4($a0)
3:
    blez    $a1, 7f
     nop
    addu    $a1, $a1, $a0
6:
    addiu   $a0, $a0, 1
    bne     $a0, $a1, 6b
     sb     $zero, -1($a0)
7:
    jr      $ra
     nop
.end func_80051050
