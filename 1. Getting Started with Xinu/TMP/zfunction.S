.globl zfunction

zfunction:
	push %ebp
	movl %esp, %ebp
	movl 8(%ebp), %eax
	andl $0xfffc01ff, %eax
	sarl $8, %eax
	leave
	ret 

