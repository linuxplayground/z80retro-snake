  .export _conout
  .export _conio
  .code
_conout:
  push  bc
  ld    c,2
  ex    de,hl
  call  5
  pop   bc
  ret

_conio:
  push  bc
  ld    c,6
  ld    e,0xff
  call  5
  ld    l,a
  ld    h,0
  pop   bc
  ret
