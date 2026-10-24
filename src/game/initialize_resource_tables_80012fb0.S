.set noat
.set noreorder
.text
.globl func_80012FB0
.ent func_80012FB0
func_80012FB0:
    addiu   $sp, $sp, -0x48
    sw      $s0, 0x28($sp)
    lui     $s0, 0x8006
    sw      $s4, 0x38($sp)
    addiu   $s4, $s0, -0x73C8
    move    $a1, $s4
    sw      $s2, 0x30($sp)
    lui     $s2, 0x8005
    lw      $a0, 0x2EB0($s2)
    li      $a2, 8
    sw      $ra, 0x44($sp)
    sw      $s6, 0x40($sp)
    sw      $s5, 0x3C($sp)
    sw      $s3, 0x34($sp)
    jal     func_80028B58
     sw     $s1, 0x2C($sp)
    lw      $v1, -0x73C8($s0)
    move    $s1, $zero
    sll     $v0, $v1, 1
    addu    $v0, $v0, $v1
    sll     $s3, $v0, 2
    jal     func_8001A2B0
     move   $a0, $s3
    move    $a1, $v0
    lw      $a0, 0x2EB0($s2)
    move    $a2, $s3
    sw      $a1, 8($s4)
    jal     func_80028B58
     addiu  $a0, $a0, 8
    lw      $v0, -0x73C8($s0)
    blez    $v0, .Lallocate_second
     move   $s3, $s1
    move    $s6, $s0
    move    $s5, $s4
    move    $s4, $s2
    lui     $s2, 0x8000
    move    $s0, $s1
.Lscan:
    lw      $v0, 8($s5)
    addu    $a0, $s0, $v0
    lw      $v1, 8($a0)
    slt     $v0, $s3, $v1
    bnezl   $v0, .Ladvance
     move   $s3, $v1
    addiu   $a1, $sp, 0x10
    lw      $v0, 0x2EB0($s4)
    lw      $a0, 0($a0)
    li      $a2, 0x18
    jal     func_80028B58
     addu   $a0, $v0, $a0
    lw      $v1, 0x10($sp)
    lw      $v0, 0x1140($s2)
    bne     $v1, $v0, .Ladvance
     nop
    lw      $v1, 0x24($sp)
    sltu    $v0, $s3, $v1
    bnezl   $v0, .Ladvance
     move   $s3, $v1
.Ladvance:
    lw      $v0, -0x73C8($s6)
    addiu   $s1, $s1, 1
    slt     $v0, $s1, $v0
    bnez    $v0, .Lscan
     addiu  $s0, $s0, 0xC
.Lallocate_second:
    jal     func_8001A2B0
     move   $a0, $s3
    lui     $s1, 0x8006
    addiu   $s2, $s1, -0x73B8
    move    $a1, $s2
    li      $a2, 8
    lui     $s0, 0x8005
    addiu   $s0, $s0, 0x2EB0
    lw      $a0, 0x10($s0)
    lui     $v1, 0x8005
    sw      $v0, 0x2EC8($v1)
    lui     $v0, 0x8006
    jal     func_80028B58
     sw     $s3, -0x73D0($v0)
    lw      $v1, -0x73B8($s1)
    sll     $v0, $v1, 1
    addu    $v0, $v0, $v1
    sll     $s3, $v0, 2
    jal     func_8001A2B0
     move   $a0, $s3
    move    $a1, $v0
    lw      $a0, 0x10($s0)
    move    $a2, $s3
    sw      $a1, 8($s2)
    jal     func_80028B58
     addiu  $a0, $a0, 8
    lw      $ra, 0x44($sp)
    lw      $s6, 0x40($sp)
    lw      $s5, 0x3C($sp)
    lw      $s4, 0x38($sp)
    lw      $s3, 0x34($sp)
    lw      $s2, 0x30($sp)
    lw      $s1, 0x2C($sp)
    lw      $s0, 0x28($sp)
    addiu   $sp, $sp, 0x48
    jr      $ra
     nop
.end func_80012FB0
