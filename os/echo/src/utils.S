.globl issue_hvc
issue_hvc:
	mov x8, x0
	hvc #0
	ret

.globl issue_smc
issue_smc:
	smc #0
	ret
