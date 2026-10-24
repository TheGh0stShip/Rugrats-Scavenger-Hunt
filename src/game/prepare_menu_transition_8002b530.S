.set noat
.set noreorder
.text
.globl func_8002B530
.ent func_8002B530
func_8002B530:
    addiu   $sp, $sp, -0x40
    sw      $s7, 0x2C($sp)
    sw      $ra, 0x30($sp)
    sw      $s6, 0x28($sp)
    sw      $s5, 0x24($sp)
    sw      $s4, 0x20($sp)
    sw      $s3, 0x1C($sp)
    sw      $s2, 0x18($sp)
    sw      $s1, 0x14($sp)
    sw      $s0, 0x10($sp)
    sdc1    $f20, 0x38($sp)
    jal     func_8001BF24
     move   $s7, $a0
    move    $s3, $zero
    lui     $v0, 0x8005
    addiu   $s6, $v0, 0x4490
    lui     $v0, 0x8005
    addiu   $s5, $v0, 0x44E0
    move    $s1, $s5
    move    $s4, $s3
    lui     $at, 0x8000
    lwc1    $f20, 0x2358($at)
.Lloop:
    lui     $v0, 0x800C
    lh      $a0, -0x6D14($v0)
    li      $v0, 6
    sh      $v0, 0x30($s1)
    addiu   $v1, $a0, -2
    subu    $v1, $s3, $v1
    srl     $v0, $v1, 31
    addu    $v0, $v1, $v0
    sra     $s0, $v0, 1
    sll     $v0, $s0, 1
    subu    $s0, $v1, $v0
    xor     $v0, $s3, $a0
    bnez    $a0, .Lmode
     sltu   $s2, $zero, $v0
    jal     func_8001DEF8
     nop
.Lmode:
    li      $v0, 8
    bne     $s7, $v0, .Lreverse
     sll    $v1, $s2, 1
    addu    $v1, $v1, $s2
    sll     $v1, $v1, 2
    addu    $v1, $v1, $s4
    addu    $v1, $v1, $s5
    sll     $v0, $s0, 1
    addu    $v0, $v0, $s0
    sll     $v0, $v0, 2
    addu    $v0, $v0, $s6
    lwc1    $f0, 0xC($v1)
    lwc1    $f2, 0($v0)
    sub.s   $f0, $f0, $f2
    div.s   $f0, $f0, $f20
    swc1    $f0, 0x24($s1)
    lwc1    $f0, 0x10($v1)
    lwc1    $f2, 4($v0)
    sub.s   $f0, $f0, $f2
    div.s   $f0, $f0, $f20
    swc1    $f0, 0x28($s1)
    lwc1    $f0, 0x14($v1)
    lwc1    $f2, 8($v0)
    j       .Llast_axis
     sub.s  $f0, $f0, $f2
.Lreverse:
    addu    $v1, $v1, $s2
    sll     $v1, $v1, 2
    addu    $v1, $v1, $s4
    addu    $v1, $v1, $s5
    sll     $v0, $s0, 1
    addu    $v0, $v0, $s0
    sll     $v0, $v0, 2
    addu    $v0, $v0, $s6
    lwc1    $f0, 0xC($v1)
    lwc1    $f2, 0($v0)
    sub.s   $f0, $f0, $f2
    neg.s   $f0, $f0
    div.s   $f0, $f0, $f20
    swc1    $f0, 0x24($s1)
    lwc1    $f0, 0x10($v1)
    lwc1    $f2, 4($v0)
    sub.s   $f0, $f0, $f2
    neg.s   $f0, $f0
    div.s   $f0, $f0, $f20
    swc1    $f0, 0x28($s1)
    lwc1    $f0, 0x14($v1)
    lwc1    $f2, 8($v0)
    sub.s   $f0, $f0, $f2
    neg.s   $f0, $f0
.Llast_axis:
    div.s   $f0, $f0, $f20
    swc1    $f0, 0x2C($s1)
    sh      $s2, 0x32($s1)
    addiu   $s1, $s1, 0x3C
    addiu   $s3, $s3, 1
    slti    $v0, $s3, 2
    bnez    $v0, .Lloop
     addiu  $s4, $s4, 0x3C
    lw      $ra, 0x30($sp)
    lw      $s7, 0x2C($sp)
    lw      $s6, 0x28($sp)
    lw      $s5, 0x24($sp)
    lw      $s4, 0x20($sp)
    lw      $s3, 0x1C($sp)
    lw      $s2, 0x18($sp)
    lw      $s1, 0x14($sp)
    lw      $s0, 0x10($sp)
    ldc1    $f20, 0x38($sp)
    addiu   $sp, $sp, 0x40
    jr      $ra
     nop
.end func_8002B530
