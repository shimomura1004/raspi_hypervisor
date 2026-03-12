#include "debug.h"
#include "exception.h"

static const char *entry_error_messages[] = {
	"SYNC_INVALID_EL2",
	"IRQ_INVALID_EL2",		
	"FIQ_INVALID_EL2",		
	"ERROR_INVALID_EL2",		

	"SYNC_INVALID_EL01_64",		
	"IRQ_INVALID_EL01_64",		
	"FIQ_INVALID_EL01_64",		
	"ERROR_INVALID_EL01_64",		

	"SYNC_INVALID_EL01_32",		
	"IRQ_INVALID_EL01_32",		
	"FIQ_INVALID_EL01_32",		
	"ERROR_INVALID_EL01_32",	

	"SYNC_ERROR",
	"HVC_ERROR",
	"DATA_ABORT_ERROR",
};

void show_invalid_entry_message(int type, unsigned long esr, unsigned long elr, unsigned long far, unsigned long mpidr) {
	PANIC("uncaught exception(%s) esr: 0x%lx, elr: 0x%lx, far: 0x%lx, mpidr: 0x%lx",
		  entry_error_messages[type], esr, elr, far, mpidr);
}
