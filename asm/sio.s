  .export _chin
  .export _chout
  .export _cstat

  .code

SIOA_D equ 0x30
SIOA_C equ 0x32

_cstat:
  in a,(SIOA_C)
  and 1
  ld l,a
  ld h,0
  ret

_chin:
  in a,(SIOA_C)
  and 1
  jr z,_chin
  in a,(SIOA_D)
  ld l,a
  ld h,0
  ret

_chout:
chout:
  in a,(SIOA_C)
  and 4
  jr z,chout
  ld a,l
  out (SIOA_D),a
  ret

