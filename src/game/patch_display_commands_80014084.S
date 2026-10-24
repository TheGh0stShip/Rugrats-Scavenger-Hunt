.set noat
.set noreorder
.text
.globl func_80014084
.ent func_80014084
func_80014084:
    addiu   $sp, $sp, -8
    sw      $s0, 0($sp)
    move    $s0, $a0
    andi    $v0, $a1, 0xFFFF
    beqz    $v0, .Lreturn
     move   $t2, $zero
    lui     $v0, 0x8000
    addiu   $a0, $v0, 0x1150
    lui     $t9, 0xFCFF
    ori     $t9, $t9, 0xFFFF
    lui     $t8, 0xFFFE
    ori     $t8, $t8, 0x793C
    lui     $t3, 0xFC12
    ori     $t3, $t3, 0x7E24
    li      $t7, -3079
    lui     $v0, 0x8005
    lw      $t1, 0x2EE8($v0)
    lui     $t0, 0xB900
    ori     $t0, $t0, 0x31D
    lui     $t6, 0xB700
    li      $t4, 1
    lui     $t5, 0xB600
    andi    $v0, $t2, 0xFFFF
.Lloop:
    sll     $v0, $v0, 3
    addu    $v0, $v0, $s0
    lhu     $v1, 0($v0)
    lhu     $v0, 2($v0)
    sll     $v1, $v1, 3
    addiu   $a3, $v0, -2
    sltiu   $v0, $a3, 11
    beqz    $v0, .Lnext
     addu   $v1, $a2, $v1
    sll     $v0, $a3, 2
    addu    $v0, $v0, $a0
    lw      $v0, 0($v0)
    jr      $v0
     nop
.Lcase2:
    sw      $t9, 0($v1)
    j       .Lnext
     sw     $t8, 4($v1)
.Lcase3:
    sw      $t3, 0($v1)
    j       .Lnext
     sw     $t7, 4($v1)
.Lcase4:
    beqz    $t1, .Lcase4_off
     lui    $v0, 0xC811
    ori     $v0, $v0, 0x2008
    j       .Lstore_pair
     sw     $t0, 0($v1)
.Lcase4_off:
    lui     $v0, 0x0C19
    ori     $v0, $v0, 0x2008
    j       .Lstore_pair
     sw     $t0, 0($v1)
.Lcase5:
    beqz    $t1, .Lcase5_off
     lui    $v0, 0xC811
    ori     $v0, $v0, 0x2038
    j       .Lstore_pair
     sw     $t0, 0($v1)
.Lcase5_off:
    lui     $v0, 0x0C19
    ori     $v0, $v0, 0x2038
    j       .Lstore_pair
     sw     $t0, 0($v1)
.Lcase6:
    beqz    $t1, .Lcase6_off
     lui    $v0, 0xC810
    ori     $v0, $v0, 0x4240
    j       .Lstore_pair
     sw     $t0, 0($v1)
.Lcase6_off:
    lui     $v0, 0x0C18
    ori     $v0, $v0, 0x4240
    j       .Lstore_pair
     sw     $t0, 0($v1)
.Lcase7:
    beqz    $t1, .Lcase7_off
     lui    $v0, 0xC810
    ori     $v0, $v0, 0x4A50
    j       .Lstore_pair
     sw     $t0, 0($v1)
.Lcase7_off:
    lui     $v0, 0x0C18
    ori     $v0, $v0, 0x4A50
    j       .Lstore_pair
     sw     $t0, 0($v1)
.Lcase8:
    sw      $t6, 0($v1)
    j       .Lnext
     sw     $t4, 4($v1)
.Lcase9:
    sw      $t5, 0($v1)
    j       .Lnext
     sw     $t4, 4($v1)
.Lcase12:
    li      $v0, 0x2000
    sw      $t5, 0($v1)
.Lstore_pair:
    sw      $v0, 4($v1)
.Lnext:
    addiu   $t2, $t2, 1
    andi    $v0, $t2, 0xFFFF
    andi    $v1, $a1, 0xFFFF
    sltu    $v0, $v0, $v1
    bnez    $v0, .Lloop
     andi   $v0, $t2, 0xFFFF
.Lreturn:
    lw      $s0, 0($sp)
    addiu   $sp, $sp, 8
    jr      $ra
     nop
.end func_80014084
