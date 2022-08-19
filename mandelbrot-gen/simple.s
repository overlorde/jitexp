	.file	"simple.c"
	.text
	.section	.rodata
	.align 8
.LC0:
	.string	"undefined instructions %s (ASCII %x)\n"
	.text
	.globl	interpret
	.type	interpret, @function
interpret:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	jmp	.L2
.L8:
	movq	-48(%rbp), %rax
	addq	$2, %rax
	movzbl	(%rax), %eax
	movsbq	%al, %rax
	salq	$4, %rax
	leaq	-1552(%rax), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -32(%rbp)
	movq	-48(%rbp), %rax
	addq	$1, %rax
	movzbl	(%rax), %eax
	movsbq	%al, %rax
	salq	$4, %rax
	leaq	-1552(%rax), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -24(%rbp)
	movq	-48(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	cmpl	$61, %eax
	je	.L3
	cmpl	$61, %eax
	jg	.L4
	cmpl	$42, %eax
	je	.L5
	cmpl	$43, %eax
	je	.L6
	jmp	.L4
.L3:
	movq	-24(%rbp), %rax
	movsd	(%rax), %xmm0
	movq	-32(%rbp), %rax
	movsd	%xmm0, (%rax)
	movq	-24(%rbp), %rax
	movsd	8(%rax), %xmm0
	movq	-32(%rbp), %rax
	movsd	%xmm0, 8(%rax)
	jmp	.L7
.L6:
	movq	-32(%rbp), %rax
	movsd	(%rax), %xmm1
	movq	-24(%rbp), %rax
	movsd	(%rax), %xmm0
	addsd	%xmm1, %xmm0
	movq	-32(%rbp), %rax
	movsd	%xmm0, (%rax)
	movq	-32(%rbp), %rax
	movsd	8(%rax), %xmm1
	movq	-24(%rbp), %rax
	movsd	8(%rax), %xmm0
	addsd	%xmm1, %xmm0
	movq	-32(%rbp), %rax
	movsd	%xmm0, 8(%rax)
	jmp	.L7
.L5:
	movq	-32(%rbp), %rax
	movsd	(%rax), %xmm1
	movq	-24(%rbp), %rax
	movsd	(%rax), %xmm0
	mulsd	%xmm1, %xmm0
	movq	-32(%rbp), %rax
	movsd	8(%rax), %xmm2
	movq	-24(%rbp), %rax
	movsd	8(%rax), %xmm1
	mulsd	%xmm2, %xmm1
	subsd	%xmm1, %xmm0
	movsd	%xmm0, -16(%rbp)
	movq	-32(%rbp), %rax
	movsd	(%rax), %xmm1
	movq	-24(%rbp), %rax
	movsd	8(%rax), %xmm0
	mulsd	%xmm0, %xmm1
	movq	-32(%rbp), %rax
	movsd	8(%rax), %xmm2
	movq	-24(%rbp), %rax
	movsd	(%rax), %xmm0
	mulsd	%xmm2, %xmm0
	addsd	%xmm1, %xmm0
	movsd	%xmm0, -8(%rbp)
	movq	-32(%rbp), %rax
	movsd	-16(%rbp), %xmm0
	movsd	%xmm0, (%rax)
	movq	-32(%rbp), %rax
	movsd	-8(%rbp), %xmm0
	movsd	%xmm0, 8(%rax)
	jmp	.L7
.L4:
	movq	-48(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %ecx
	movq	stderr(%rip), %rax
	movq	-48(%rbp), %rdx
	leaq	.LC0(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	$1, %edi
	call	exit@PLT
.L7:
	addq	$3, -48(%rbp)
.L2:
	movq	-48(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L8
	nop
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	interpret, .-interpret
	.section	.rodata
.LC1:
	.string	"P5\n%d %d\n%d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$1712, %rsp
	movl	%edi, -1700(%rbp)
	movq	%rsi, -1712(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$255, %ecx
	movl	$900, %edx
	movl	$1600, %esi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, -1684(%rbp)
	jmp	.L10
.L18:
	movl	$0, -1688(%rbp)
	jmp	.L11
.L17:
	pxor	%xmm0, %xmm0
	cvtsi2sdl	-1688(%rbp), %xmm0
	movsd	.LC2(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movsd	.LC3(%rip), %xmm2
	movapd	%xmm0, %xmm1
	subsd	%xmm2, %xmm1
	movsd	.LC4(%rip), %xmm0
	mulsd	%xmm1, %xmm0
	movsd	%xmm0, -1680(%rbp)
	pxor	%xmm0, %xmm0
	cvtsi2sdl	-1688(%rbp), %xmm0
	movsd	.LC5(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movsd	.LC3(%rip), %xmm2
	movapd	%xmm0, %xmm1
	subsd	%xmm2, %xmm1
	movsd	.LC6(%rip), %xmm0
	mulsd	%xmm1, %xmm0
	movsd	%xmm0, -1672(%rbp)
	movl	$1, -1692(%rbp)
	jmp	.L12
.L13:
	movl	-1692(%rbp), %eax
	cltq
	salq	$4, %rax
	addq	%rbp, %rax
	subq	$1672, %rax
	pxor	%xmm0, %xmm0
	movsd	%xmm0, (%rax)
	movl	-1692(%rbp), %eax
	cltq
	salq	$4, %rax
	addq	%rbp, %rax
	subq	$1672, %rax
	movsd	(%rax), %xmm0
	movl	-1692(%rbp), %eax
	cltq
	salq	$4, %rax
	addq	%rbp, %rax
	subq	$1680, %rax
	movsd	%xmm0, (%rax)
	addl	$1, -1692(%rbp)
.L12:
	cmpl	$3, -1692(%rbp)
	jle	.L13
	movl	$0, -1692(%rbp)
	jmp	.L14
.L16:
	movq	-1712(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rdx
	leaq	-1680(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	interpret
	addl	$1, -1692(%rbp)
.L14:
	cmpl	$255, -1692(%rbp)
	jg	.L15
	movsd	-1664(%rbp), %xmm1
	movsd	-1664(%rbp), %xmm0
	mulsd	%xmm0, %xmm1
	movsd	-1656(%rbp), %xmm2
	movsd	-1656(%rbp), %xmm0
	mulsd	%xmm2, %xmm0
	addsd	%xmm0, %xmm1
	movsd	.LC8(%rip), %xmm0
	comisd	%xmm1, %xmm0
	ja	.L16
.L15:
	movl	-1692(%rbp), %eax
	movl	%eax, %edx
	movl	-1688(%rbp), %eax
	cltq
	movb	%dl, -1616(%rbp,%rax)
	addl	$1, -1688(%rbp)
.L11:
	cmpl	$1599, -1688(%rbp)
	jle	.L17
	movq	stdout(%rip), %rdx
	leaq	-1616(%rbp), %rax
	movq	%rdx, %rcx
	movl	$1600, %edx
	movl	$1, %esi
	movq	%rax, %rdi
	call	fwrite@PLT
	addl	$1, -1684(%rbp)
.L10:
	cmpl	$899, -1684(%rbp)
	jle	.L18
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L20
	call	__stack_chk_fail@PLT
.L20:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC2:
	.long	0
	.long	1083768832
	.align 8
.LC3:
	.long	0
	.long	1071644672
	.align 8
.LC4:
	.long	-1717986918
	.long	1074370969
	.align 8
.LC5:
	.long	0
	.long	1082925056
	.align 8
.LC6:
	.long	-858993459
	.long	1073532108
	.align 8
.LC8:
	.long	0
	.long	1074790400
	.ident	"GCC: (GNU) 12.1.0"
	.section	.note.GNU-stack,"",@progbits
