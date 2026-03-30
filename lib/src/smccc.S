// SMCCC (SMC Calling Convention) implementation

.globl hvc_call_4
hvc_call_4:
	hvc #0
	ret

.globl smc_call_4
smc_call_4:
	smc #0
	ret
