.set noat
.set noreorder
.text
.globl func_8001F1C4
.ent func_8001F1C4
func_8001F1C4:
    addiu   $sp, $sp, -0x28
    sw      $ra, 0x20($sp)
    sw      $s3, 0x1C($sp)
    sw      $s2, 0x18($sp)
    sw      $s1, 0x14($sp)
    jal     func_8001361C
     sw     $s0, 0x10($sp)
    lui     $s0, 0x8006
    lui     $a0, 0x8005
    lw      $v1, 0x1D80($s0)
    addiu   $a0, $a0, 0x3978
    sll     $v1, $v1, 1
    addu    $v1, $v1, $a0
    lhu     $a0, 0($v1)
    jal     func_80043554
     move   $s2, $v0
    beqz    $v0, .Lstart
     move   $a0, $s2
    lw      $v0, 0x1D80($s0)
    addiu   $v0, $v0, 1
    sw      $v0, 0x1D80($s0)
    sltiu   $v0, $v0, 4
    beqzl   $v0, .Lstart
     sw     $zero, 0x1D80($s0)
    move    $a0, $s2
.Lstart:
    lui     $v0, 0x8006
    addiu   $s3, $v0, 0x1E54
    lhu     $a2, 2($s3)
    jal     func_800433F4
     li     $a1, 0x40
    lui     $s1, 0x8006
    lui     $a0, 0x8005
    lw      $v1, 0x1D80($s1)
    addiu   $s0, $a0, 0x3978
    sll     $v1, $v1, 1
    addu    $v1, $v1, $s0
    sh      $v0, 0($v1)
    lhu     $a0, 0($v1)
    jal     func_80043554
     nop
    bnez    $v0, .Lincrement_counter
     lui    $v1, 0x8006
.Lretry_check:
    lw      $v0, 0x1D80($s1)
    sll     $v0, $v0, 1
    addu    $v0, $v0, $s0
    lhu     $a0, 0($v0)
    jal     func_80043554
     nop
    bnez    $v0, .Lsuccess
     move   $a0, $s2
    lhu     $a2, 2($s3)
    jal     func_800433F4
     li     $a1, 0x40
    lw      $v1, 0x1D80($s1)
    sll     $v1, $v1, 1
    addu    $v1, $v1, $s0
    j       .Lretry_check
     sh     $v0, 0($v1)
.Lsuccess:
    lui     $v1, 0x8005
    lw      $v0, 0x3930($v1)
    addiu   $v0, $v0, 1
    sw      $v0, 0x3930($v1)
    lui     $v1, 0x8006
.Lincrement_counter:
    lw      $v0, 0x1E00($v1)
    addiu   $v0, $v0, 1
    jal     func_80047E80
     sw     $v0, 0x1E00($v1)
    lui     $v0, 0x8006
    lui     $v1, 0x8005
    lw      $v0, 0x1D80($v0)
    addiu   $v1, $v1, 0x3978
    sll     $v0, $v0, 1
    addu    $v0, $v0, $v1
    lhu     $v0, 0($v0)
    lw      $ra, 0x20($sp)
    lw      $s3, 0x1C($sp)
    lw      $s2, 0x18($sp)
    lw      $s1, 0x14($sp)
    lw      $s0, 0x10($sp)
    addiu   $sp, $sp, 0x28
    jr      $ra
     nop
.end func_8001F1C4
