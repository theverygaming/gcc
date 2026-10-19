#ifdef L_mulsi3
	.align	4
	.global __mulsi3
__mulsi3:
	// https://en.wikipedia.org/wiki/Ancient_Egyptian_multiplication#Russian_peasant_multiplication
	mov r2, #0
.Lmulsi3_loop:
	cmp r0, #0
	ifeq rjmp .Lmulsi3_end
	and r3, r0, #0x1
	cmp r3, #0
	ifneq add r2, r2, r1
	shr r0, r0, #1
	shl r1, r1, #1
	rjmp .Lmulsi3_loop
.Lmulsi3_end:
	mov r0, r2
	mov rip, lr
#endif /* L_mulsi3 */
