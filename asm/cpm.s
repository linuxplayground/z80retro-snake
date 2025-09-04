  .export _conio
  .code
_conio:
  push  bc
  ld    c,6
  ld    e,0xff
  call  5
  ld    l,a
  ld    h,0
  pop   bc
  ret
