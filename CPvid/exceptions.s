
.align 2
.global _v_generalException
_v_generalException: !'general exception' at 0x100
	mov.l origGeneral, r0
	jmp @r0
	nop
.align 2
origGeneral:
	.long 0x8002173C+0x100 !address of VBR+0x100
!12 bytes


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.align 2
.global _v_missException
_v_missException: !'miss exception' at 0x400
	mov.l origMiss, r0
	jmp @r0
	nop
.align 2
origMiss:
	.long 0x8002173C+0x400 !address of VBR+0x400
!12 bytes


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.align 2
.global _v_interrupt
_v_interrupt: !'interrupt' at 0x600
	mov.l r1, @-r15
	mov.l r2, @-r15
	mov.l r3, @-r15
	mov.l r4, @-r15
	mov.l r5, @-r15
	mov.l r6, @-r15
	mov.l r7, @-r15
	sts.l pr, @-r15	

	mov.l AudioInterruptHandler, r0
	jsr @r0
	nop
	
	lds.l @r15+, pr
	mov.l @r15+, r7
	mov.l @r15+, r6
	mov.l @r15+, r5
	mov.l @r15+, r4
	mov.l @r15+, r3
	mov.l @r15+, r2
	mov.l @r15+, r1
	
	mov.l origInterrupt, r0 !//jump to original interrupt Handler
	jmp @r0
	nop
.align 2
origInterrupt:
	.long 0x8002173C+0x600 !address of VBR+0x600
AudioInterruptHandler:
	.long _handleInterrupt
!52 bytes

