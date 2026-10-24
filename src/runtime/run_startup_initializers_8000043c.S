.set noreorder
.set noat
.text

.globl func_8000043C
.ent func_8000043C
func_8000043C:
    addiu   $sp, $sp, -0x10
    sw      $s0, 0x4($sp)
    sw      $s1, 0x8($sp)
    sw      $ra, 0xC($sp)
    lui     $s0, 0x8000
    addiu   $s0, $s0, 0x39F0
    lui     $s1, 0x8000
    addiu   $s1, $s1, 0x39F0
1:
    beq     $s0, $s1, 3f
     lw     $t0, 0x0($s0)
    beqz    $t0, 2f
     nop
    jalr    $t0
     nop
2:
    addiu   $s0, $s0, 8
    bgez    $zero, 1b
     nop
3:
    lw      $ra, 0xC($sp)
    lw      $s1, 0x8($sp)
    lw      $s0, 0x4($sp)
    addiu   $sp, $sp, 0x10
    jr      $ra
     nop
.end func_8000043C
