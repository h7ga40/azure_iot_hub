.global ___setjmp
.hidden ___setjmp
.global __setjmp
.global _setjmp
.global setjmp
.type   __setjmp, @function
.type   _setjmp,  @function
.type   setjmp,   @function
___setjmp:
__setjmp:
_setjmp:
setjmp:
	mov.l	r0, [r1]
	mov.l	r1, 4[r1]
	mov.l	r2, 8[r1]
	mov.l	r3, 12[r1]
	mov.l	r4, 16[r1]
	mov.l	r5, 20[r1]
	mov.l	r6, 24[r1]
	mov.l	r7, 28[r1]
	mov.l	r8, 32[r1]
	mov.l	r9, 36[r1]
	mov.l	r10, 40[r1]
	mov.l	r11, 44[r1]
	mov.l	r12, 48[r1]
	mov.l	r13, 52[r1]
	mov.l	r14, 56[r1]
	mov.l	r15, 60[r1]
	mov.l	[r0], r2
	mov.l	r2, 64[r1]
	mov.l	#0, r1
	rts
	 mov  #0, r0
