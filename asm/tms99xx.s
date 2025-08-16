  .export _tms_mcinit
  .export _tms_w_addr
  .export _tms_r_addr
  .export _tms_wait
  .export _tms_flushmc

io_tmsdata     equ 0x80
io_tmslatch    equ 0x81
joy0           equ 0xa8
joy1           equ 0xa9

tms_pat        equ 0x0800
tms_patLen     equ 0x0600
tms_name       equ 0x1400
tms_nameLen    equ 0x0300
tms_spAttr     equ 0x1000
tms_spAttrLen  equ 0x0080
tms_spPatt     equ 0x0800
tms_spPattLen  equ 0x0000

  .code

;===============================================================================
; Write one byte of data to the VDP at the current address.
; includes a delay
; OUTPUT: void
; CLOBBERS: none
;===============================================================================
tms_put:
  out   (io_tmsdata),a
  push  hl
  pop   hl
  push  hl
  pop   hl
  ret

;===============================================================================
; Set the VDP Address to write to.
; INPUT: hl = write address
; OUTPUT: void
; CLOBBERS: none
;===============================================================================
_tms_w_addr:
  ld    a,l
  out   (io_tmslatch),a
  ld    a,h
  or    0x40
  out   (io_tmslatch),a
  ret

;===============================================================================
; Set the VDP Address to read from.
; INPUT: hl = read address
; OUTPUT: void
; CLOBBERS: none
;===============================================================================
_tms_r_addr:
  ld    a,l
  out   (io_tmslatch),a
  ld    a,h
  out   (io_tmslatch),a
  ret

;===============================================================================
; Initialize the VDP in Graphics Mode 1 hybrid mode.
; refer to .tms_init_g1_registers at the end of this file for details.
; Disable all sprites by setting byte 0 in each sprite attribute table to 0xD0
; INPUT: void
; OUTPUT: void
; CLOBBERS: none
;===============================================================================
_tms_mcinit:
  push  bc

  ld    hl,tms_initmcregs
  ld    b,tms_initmclen
  ld    c,io_tmslatch
  otir
  ;
  ; clear sprite attribute table and set Y position to D0
  ;
  ld    b,32
  ld    de,tms_spAttr
  call  _tms_w_addr
clear_sp_loop:
  ld    a,0xd0
  call  tms_put
  xor   a
  call  tms_put
  call  tms_put
  call  tms_put
  djnz  clear_sp_loop
  ;
  ; init the name table for mc plotting
  ;
  ld    hl,tms_name
  call  _tms_w_addr

  ld    c,0
  ld    e,6
l1:
  ld    d,4
l2:
  ld    a,c
  ld    b,32
l3:
  call  tms_put
  inc   a
  dec   b
  jr    nz,l3   ; value in row loop

  dec   d
  jr    nz,l2   ; row loop (row rows)

  ld    c,a
  dec   e
  jr    nz,l1 ; group loop (6 groups)

  pop   bc
  ret

;===============================================================================
; Copy system memory to VDP memory after vblank signal only.
; INPUT: DE = VDP target memory address, HL = host memory address, BC = count
; OUTPUT: void
; CLOBBERS: AF, BC, DE, HL
;===============================================================================
tms_wr_fast:
  ex    de,hl
  call  _tms_w_addr
  ex    de,hl
  ld    d,b
  ld    e,c
  ld    c,io_tmsdata
; goldilocks
  ld    b,e
  inc   e
  dec   e
  jr    z,tms_wr_fast_l
  inc   d
tms_wr_fast_l:
  outi
  jp    nz,tms_wr_fast_l
  dec   d
  jp    nz,tms_wr_fast_l
  ret

;===============================================================================
; Copy system memory to VDP memory at any time.
; INPUT: DE = VDP target memory address, HL = host memory address, BC = count
; OUTPUT: void
; CLOBBERS: AF, BC, DE, HL
;===============================================================================
tms_write_slo:
  call  _tms_w_addr
  push bc
  ld    d,b
  ld    e,c
  ld    c,io_tmsdata
tms_wr_sl_l:
  outi
  push  hl
  pop   hl
  push  hl
  pop   hl
  dec   de
  ld    a,d
  or    e
  jr    nz,tms_wr_sl_l
  pop bc
  ret

;===============================================================================
; Flush the tms_buffer in system memory to the VDP nameTable after the vsync
; status is set on the status register.
; INPUT: void
; OUTPUT: void
; CLOBBERS: AF, BC, DE, HL
;===============================================================================
_tms_flushmc:
  push  bc
  ; hl is the source data
  ld    de,tms_pat
  ld    bc,tms_patLen

  ;call tms_wait
  call  tms_wr_fast
  pop   bc
  ret

;===============================================================================
; Wait for the VDP VSYNC status to appear on the status register
; INPUT: void
; OUTPUT: void
; CLOBBERS: AF
;===============================================================================
_tms_wait:
  in    a,(joy0)        ; read the /INT status via bodge wire 
  and   0x02            ; check U6, pin 4 (D1)
  jp    nz,_tms_wait
  in    a,(io_tmslatch) ; read the VDP status register to reset the IRQ
  ret

;===============================================================================
; Multicolor Mode Registers
;===============================================================================
tms_initmcregs:
  .byte 0x00,0x80       ; Graphics mode 1, no external video
  .byte 0xe8,0x81       ; 16K,enable display, enable int, 16x16 sprites
  .byte 0x05,0x82       ; R2 = name table = 0x1400
  .byte 0x00,0x83       ; R3 = color table = NOT USED
  .byte 0x01,0x84       ; R4 = pattern table = 0x0800
  .byte 0x20,0x85       ; R5 = sprite attribute table = 0x1000
  .byte 0x00,0x86       ; R6 = sprite pattern table = 0x0000
  .byte 0x0a,0x87       ; R7 = dartk yellow border
tms_initmcend:
tms_initmclen equ tms_initmcend-tms_initmcregs


