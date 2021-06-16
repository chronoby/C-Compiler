	.text
	.file	"main"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$5, (%rsp)
	movl	$0, 4(%rsp)
	cmpl	$0, (%rsp)
	jg	.LBB0_2
	jmp	.LBB0_6
	.p2align	4, 0x90
.LBB0_5:                                # %ifcont3
                                        #   in Loop: Header=BB0_2 Depth=1
	movl	(%rsp), %eax
	addl	%eax, 4(%rsp)
	decl	%eax
	movl	%eax, (%rsp)
	cmpl	$0, (%rsp)
	jle	.LBB0_6
.LBB0_2:                                # %loopin
                                        # =>This Inner Loop Header: Depth=1
	cmpl	$3, (%rsp)
	je	.LBB0_6
# %bb.3:                                # %else
                                        #   in Loop: Header=BB0_2 Depth=1
	cmpl	$4, (%rsp)
	jne	.LBB0_5
# %bb.4:                                # %then2
                                        #   in Loop: Header=BB0_2 Depth=1
	decl	(%rsp)
	cmpl	$0, (%rsp)
	jg	.LBB0_2
.LBB0_6:                                # %loopcont
	movl	(%rsp), %esi
	movl	4(%rsp), %edx
	movl	$.L__unnamed_1, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	xorl	%eax, %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L__unnamed_1,@object           # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%d %d\n"
	.size	.L__unnamed_1, 7

	.section	".note.GNU-stack","",@progbits
