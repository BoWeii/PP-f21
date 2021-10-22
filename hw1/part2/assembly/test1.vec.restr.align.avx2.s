	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 15	sdk_version 10, 15, 4
	.globl	__Z4testPfS_S_i         ## -- Begin function _Z4testPfS_S_i
	.p2align	4, 0x90
__Z4testPfS_S_i:                        ## @_Z4testPfS_S_i
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	4096(%rdx), %rax
	leaq	4096(%rdi), %rcx
	cmpq	%rdx, %rcx
	seta	%r9b
	leaq	4096(%rsi), %r8
	cmpq	%rdi, %rax
	seta	%cl
	andb	%r9b, %cl
	cmpq	%rdx, %r8
	seta	%r9b
	cmpq	%rsi, %rax
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
	vmovss	(%rdi,%rax,4), %xmm0    ## xmm0 = mem[0],zero,zero,zero
	vaddss	(%rsi,%rax,4), %xmm0, %xmm0
	vmovss	%xmm0, (%rdx,%rax,4)
	vmovss	4(%rdi,%rax,4), %xmm0   ## xmm0 = mem[0],zero,zero,zero
	vaddss	4(%rsi,%rax,4), %xmm0, %xmm0
	vmovss	%xmm0, 4(%rdx,%rax,4)
	vmovss	8(%rdi,%rax,4), %xmm0   ## xmm0 = mem[0],zero,zero,zero
	vaddss	8(%rsi,%rax,4), %xmm0, %xmm0
	vmovss	%xmm0, 8(%rdx,%rax,4)
	vmovss	12(%rdi,%rax,4), %xmm0  ## xmm0 = mem[0],zero,zero,zero
	vaddss	12(%rsi,%rax,4), %xmm0, %xmm0
	vmovss	%xmm0, 12(%rdx,%rax,4)
	addq	$4, %rax
	cmpq	$1024, %rax             ## imm = 0x400
	jne	LBB0_5
	jmp	LBB0_3
	.p2align	4, 0x90
LBB0_2:                                 ## =>This Inner Loop Header: Depth=1
	vmovaps	(%rdi,%rax,4), %ymm0
	vmovaps	32(%rdi,%rax,4), %ymm1
	vmovaps	64(%rdi,%rax,4), %ymm2
	vmovaps	96(%rdi,%rax,4), %ymm3
	vaddps	(%rsi,%rax,4), %ymm0, %ymm0
	vaddps	32(%rsi,%rax,4), %ymm1, %ymm1
	vaddps	64(%rsi,%rax,4), %ymm2, %ymm2
	vaddps	96(%rsi,%rax,4), %ymm3, %ymm3
	vmovaps	%ymm0, (%rdx,%rax,4)
	vmovaps	%ymm1, 32(%rdx,%rax,4)
	vmovaps	%ymm2, 64(%rdx,%rax,4)
	vmovaps	%ymm3, 96(%rdx,%rax,4)
	addq	$32, %rax
	cmpq	$1024, %rax             ## imm = 0x400
	jne	LBB0_2
	jmp	LBB0_3
LBB0_4:
	popq	%rbp
	vzeroupper
	retq
	.cfi_endproc
                                        ## -- End function

.subsections_via_symbols
