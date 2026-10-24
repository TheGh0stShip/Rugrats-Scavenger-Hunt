.set noreorder
.set noat
.text

.globl func_80047FC0
.ent func_80047FC0
func_80047FC0:
    lui     $k0, 0x800C
    addiu   $k0, $k0, 0x4320
    sd      $at, 0x20($k0)
    mfc0    $k1, $12
    sw      $k1, 0x118($k0)
    addiu   $at, $zero, -4
    and     $k1, $k1, $at
    mtc0    $k1, $12
    sd      $t0, 0x58($k0)
    sd      $t1, 0x60($k0)
    sd      $t2, 0x68($k0)
.end func_80047FC0
