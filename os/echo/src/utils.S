.globl get_el
get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret

.globl put32
put32:
	str w1,[x0]
	ret

.globl get32
get32:
	ldr w0,[x0]
	ret

.globl delay
delay:
	subs x0, x0, #1
	bne delay
	ret

.globl issue_hvc
issue_hvc:
	mov x8, x0
	hvc #0
	ret

.globl issue_smc
issue_smc:
	smc #0
	ret
