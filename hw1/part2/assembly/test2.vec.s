	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 15	sdk_version 10, 15, 4
	.globl	__Z5test2PfS_S_i        ## -- Begin function _Z5test2PfS_S_i
	.p2align	4, 0x90
__Z5test2PfS_S_i:                       ## @_Z5test2PfS_S_i
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	4096(%rdx), %rax
	leaq	4096(%rsi), %rcx
	cmpq	%rdx, %rcx
	seta	%r9b
	leaq	4096(%rdi), %r8
	cmpq	%rsi, %rax
	seta	%cl
	andb	%r9b, %cl
	cmpq	%rdx, %r8
	seta	%r9b
	cmpq	%rdi, %rax
	seta	%r8b
	andb	%r9b, %r8b
	orb	%cl, %r8b
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	testb	%r8b, %r8b
	jne	LBB0_5
	jmp	LBB0_2
	.p2align	4, 0x90
LBB0_3:
	incl	%ecx
	cmpl	$20000000, %ecx         ## imm = 0x1312D00
	je	LBB0_4
## %bb.1:
	xorl	%eax, %eax
	testb	%r8b, %r8b
	je	LBB0_2
	.p2align	4, 0x90
LBB0_5:                                 ## =>This Inner Loop Header: Depth=1
	movss	(%rsi,%rax,4), %xmm0    ## xmm0 = mem[0],zero,zero,zero
	maxss	(%rdi,%rax,4), %xmm0
	movss	%xmm0, (%rdx,%rax,4)
	movss	4(%rsi,%rax,4), %xmm0   ## xmm0 = mem[0],zero,zero,zero
	maxss	4(%rdi,%rax,4), %xmm0
	movss	%xmm0, 4(%rdx,%rax,4)
	addq	$2, %rax
	cmpq	$1024, %rax             ## imm = 0x400
	jne	LBB0_5
	jmp	LBB0_3
	.p2align	4, 0x90
LBB0_2:                                 ## =>This Inner Loop Header: Depth=1
	movaps	(%rsi,%rax,4), %xmm0
	movaps	16(%rsi,%rax,4), %xmm1
	maxps	(%rdi,%rax,4), %xmm0
	maxps	16(%rdi,%rax,4), %xmm1
	movaps	%xmm0, (%rdx,%rax,4)
	movaps	%xmm1, 16(%rdx,%rax,4)
	movaps	32(%rsi,%rax,4), %xmm0
	movaps	48(%rsi,%rax,4), %xmm1
	maxps	32(%rdi,%rax,4), %xmm0
	maxps	48(%rdi,%rax,4), %xmm1
	movaps	%xmm0, 32(%rdx,%rax,4)
	movaps	%xmm1, 48(%rdx,%rax,4)
	addq	$16, %rax
	cmpq	$1024, %rax             ## imm = 0x400
	jne	LBB0_2
	jmp	LBB0_3
LBB0_4:
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function

.subsections_via_symbols
