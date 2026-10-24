.set noat
.set noreorder
.text
.globl func_80042B70
.ent func_80042B70
func_80042B70:
    addiu   $sp, $sp, -0xA0
    move    $a1, $a0
    lui     $a0, %hi(D_800C40C0)
    lw      $a0, %lo(D_800C40C0)($a0)
    lui     $v1, %hi(D_800C40C4)
    lw      $v1, %lo(D_800C40C4)($v1)
    addiu   $v0, $sp, 0x78
    li      $a2, 2
    sw      $ra, 0x98($sp)
    sw      $zero, 0x10($sp)
    sw      $v0, 0x64($sp)
    sw      $a2, 0x18($sp)
    addiu   $v1, $v1, 0x30
    sw      $v1, 0x60($sp)
    lw      $v0, 0($a1)
    lui     $v1, %hi(D_800367F0)
    addiu   $v1, $v1, %lo(D_800367F0)
    sw      $v0, 0x50($sp)
    lw      $a3, 4($a1)
    lui     $v0, %hi(D_800368C0)
    addiu   $v0, $v0, %lo(D_800368C0)
    subu    $v0, $v0, $v1
    sw      $a2, 0x20($sp)
    sw      $v1, 0x28($sp)
    sw      $v0, 0x2C($sp)
    sw      $zero, 0x24($sp)
    sw      $a3, 0x54($sp)
    lw      $v0, 8($a1)
    sw      $v0, 0x30($sp)
    lw      $v1, 0xC($a1)
    li      $v0, 0x1000
    sw      $v0, 0x34($sp)
    li      $v0, 0x800
    sw      $v0, 0x3C($sp)
    sw      $zero, 0x40($sp)
    sw      $zero, 0x44($sp)
    sw      $zero, 0x48($sp)
    sw      $zero, 0x4C($sp)
    sw      $zero, 0x58($sp)
    sw      $zero, 0x5C($sp)
    jal     func_80051350
     sw     $v1, 0x38($sp)
    move    $a0, $v0
    addiu   $a1, $sp, 0x10
    jal     func_80048F00
     li     $a2, 1
    lui     $a0, %hi(D_800C40C4)
    lw      $a0, %lo(D_800C40C4)($a0)
    move    $a1, $zero
    li      $a2, 1
    jal     func_80048DD0
     addiu  $a0, $a0, 0x30
    lw      $ra, 0x98($sp)
    jr      $ra
     addiu  $sp, $sp, 0xA0
    nop
.end func_80042B70
