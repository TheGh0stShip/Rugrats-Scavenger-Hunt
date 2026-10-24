.set noat
.set noreorder
.text
.globl func_8001B5FC
.ent func_8001B5FC
func_8001B5FC:
    addiu   $sp, $sp, -0x18
    sw      $ra, 0x14($sp)
    jal     func_8001BF3C
     sw     $s0, 0x10($sp)
    bnez    $v0, .Lselect_result
     lui    $v0, 0x8005
    lui     $s0, 0x8005
    lw      $a1, 0x29F0($s0)
    lw      $v1, 0x118($a1)
    andi    $v0, $v1, 0x4000
    bnez    $v0, .Lcheck_random
     li     $a0, 0x31
    move    $a2, $zero
    ori     $v0, $v1, 0x4000
    j       .Lplay_first
     sw     $v0, 0x118($a1)
.Lcheck_random:
    lb      $v0, 0x1A6($a1)
    slti    $v0, $v0, 7
    bnez    $v0, .Lsound_ready
     li     $a0, 0x3D
    jal     func_8003A840
     nop
    lui     $v1, 0x6666
    ori     $v1, $v1, 0x6667
    mult    $v0, $v1
    sra     $v1, $v0, 31
    addiu   $v0, $v0, -1
    mfhi    $t5
    sra     $a0, $t5, 1
    subu    $a0, $a0, $v1
    sll     $v1, $a0, 2
    addu    $v1, $v1, $a0
    bne     $v1, $v0, .Lsetup_rows
     move   $t0, $zero
    li      $a0, 0x31
    lw      $a1, 0x29F0($s0)
.Lsound_ready:
    move    $a2, $zero
.Lplay_first:
    jal     func_800069C8
     nop
    move    $a0, $zero
    lw      $a1, 0x29F0($s0)
    jal     func_800069C8
     li     $a2, 2
    move    $t0, $zero
.Lsetup_rows:
    li      $t4, 1
    lui     $v0, 0x8006
    move    $t1, $v0
    lui     $t2, 0x6666
    ori     $t2, $t2, 0x6667
    lui     $v0, 0x8005
    addiu   $t3, $v0, 0x363C
    lui     $v0, 0x8005
    addiu   $a3, $v0, 0x368C
    lui     $v0, 0x8005
    sw      $zero, 0x37D8($v0)
    move    $v0, $t4
    sh      $v0, -0x480E($t1)
.Lrow_loop:
    lh      $v1, -0x480E($t1)
    addiu   $v1, $v1, -5
    subu    $v1, $t0, $v1
    mult    $v1, $t2
    sh      $t4, 0x32($a3)
    addiu   $t0, $t0, 1
    sra     $v0, $v1, 31
    mfhi    $t5
    sra     $a0, $t5, 1
    subu    $a0, $a0, $v0
    sll     $v0, $a0, 2
    addu    $v0, $v0, $a0
    subu    $a0, $v1, $v0
    sll     $v0, $a0, 1
    addu    $v0, $v0, $a0
    sll     $v0, $v0, 2
    addu    $v0, $v0, $t3
    lw      $v1, 0($v0)
    lw      $a0, 4($v0)
    lw      $a1, 8($v0)
    sw      $v1, 0($a3)
    sw      $a0, 4($a3)
    sw      $a1, 8($a3)
    slti    $v0, $t0, 5
    bnez    $v0, .Lrow_loop
     addiu  $a3, $a3, 0x3C
    lui     $v0, 0x8005
.Lselect_result:
    lw      $a0, 0x29F0($v0)
    beqz    $a0, .Lresult_b4
     lui    $v1, 0x8005
    jal     func_80026010
     nop
    sll     $v0, $v0, 16
    beqz    $v0, .Lresult_b4
     lui    $v1, 0x8005
    j       .Lstore_first
     li     $v0, 0x1B0
.Lresult_b4:
    li      $v0, 0x1B4
.Lstore_first:
    sw      $v0, 0x3738($v1)
    lui     $v0, 0x8005
    lw      $v0, 0x2F18($v0)
    bnez    $v0, .Lresult_b5
     lui    $v1, 0x8005
    j       .Lstore_second
     li     $v0, 0x1AF
.Lresult_b5:
    li      $v0, 0x1B5
.Lstore_second:
    sw      $v0, 0x36C0($v1)
    lw      $ra, 0x14($sp)
    lw      $s0, 0x10($sp)
    addiu   $sp, $sp, 0x18
    jr      $ra
     nop
.end func_8001B5FC
