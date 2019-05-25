.section .init
	mov.l   #0, r14
	cmp     #0, r14
	beq.b   break1
	mov.l   #0, r1
	mov.l   #4, r2
break1:
	jsr     r14
	mov.l   #0, r1
	mov.l   [r1], r14
	cmp     #0, r14
	beq.b   end
	mov.l   #0, r14
	cmp     #0, r14
	beq.s   end
	jsr     r14
end:
	rts

.section .fini
	rts
