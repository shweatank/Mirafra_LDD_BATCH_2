	.file	"main.c"
	.text
	.section	.rodata
.LC0:
	.string	"Enter operation: "
.LC1:
	.string	"%f %c %f"
.LC2:
	.string	"Invalid operation"
.LC3:
	.string	"Result: %.2f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	-24(%rbp), %rcx
	leaq	-29(%rbp), %rdx
	leaq	-28(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc99_scanf@PLT
	movzbl	-29(%rbp), %eax
	movsbl	%al, %eax
	cmpl	$47, %eax
	je	.L2
	cmpl	$47, %eax
	jg	.L3
	cmpl	$45, %eax
	je	.L4
	cmpl	$45, %eax
	jg	.L3
	cmpl	$42, %eax
	je	.L5
	cmpl	$43, %eax
	jne	.L3
	movq	add@GOTPCREL(%rip), %rax
	movq	%rax, -16(%rbp)
	jmp	.L6
.L4:
	movq	sub@GOTPCREL(%rip), %rax
	movq	%rax, -16(%rbp)
	jmp	.L6
.L5:
	movq	mul@GOTPCREL(%rip), %rax
	movq	%rax, -16(%rbp)
	jmp	.L6
.L2:
	movq	divide@GOTPCREL(%rip), %rax
	movq	%rax, -16(%rbp)
	jmp	.L6
.L3:
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$1, %eax
	jmp	.L8
.L6:
	movss	-24(%rbp), %xmm0
	movl	-28(%rbp), %eax
	movq	-16(%rbp), %rdx
	movaps	%xmm0, %xmm1
	movd	%eax, %xmm0
	call	*%rdx
	movd	%xmm0, %eax
	movl	%eax, -20(%rbp)
	pxor	%xmm2, %xmm2
	cvtss2sd	-20(%rbp), %xmm2
	movq	%xmm2, %rax
	movq	%rax, %xmm0
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf@PLT
	movl	$0, %eax
.L8:
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L9
	call	__stack_chk_fail@PLT
.L9:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
