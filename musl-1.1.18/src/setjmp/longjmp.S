.global _longjmp
.global longjmp
.type   _longjmp, @function
.type   longjmp,  @function
_longjmp:
longjmp:
	tst	r2, r2
	stz	#1, r2
	mov.l	r2, 4[r1]
	mov.l	[r1], r0
	mov.l	64[r1], r2
	mov.l	r2, [r0]
	mov.l	60[r1], r15
	mov.l	56[r1], r14
	mov.l	52[r1], r13
	mov.l	48[r1], r12
	mov.l	44[r1], r11
	mov.l	40[r1], r10
	mov.l	36[r1], r9
	mov.l	32[r1], r8
	mov.l	28[r1], r7
	mov.l	24[r1], r6
	mov.l	20[r1], r5
	mov.l	16[r1], r4
	mov.l	12[r1], r3
	mov.l	8[r1], r2
	mov.l	4[r1], r1

	rts
	 nop
