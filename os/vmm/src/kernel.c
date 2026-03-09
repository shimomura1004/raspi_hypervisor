#include "printf.h"
#include "utils.h"
#include "mini_uart.h"
#include "irq.h"
#include "../../../hv/include/loader.h"
#include "hypercall_type.h"

#define BUFFER_LENGTH 128
#define ARG_MAX 8
#define HVC_ARG_MAX 4
#define SMC_ARG_MAX 5
#define EQUAL(A, B) (strncmp(A, B, sizeof(B)) == 0)

void new_vm();
void destroy_vm(int vmid);
void shutdown_hv();
void reboot_hv();
void issue_hvc(int hvc_nr, int arg1, int arg2, int arg3);
void issue_smc(int smc_nr, int arg1, int arg2, int arg3, int arg4);

struct loader_args vm_args = {
	.loader_addr = 0x0,
	.entry_point = 0x0,
	.sp = 0x100000,
	.filename = "",
};
struct loader_args *vm_args_p = &vm_args;

// "new RASPIOS.ELF 2" -> "new" "RASPIOS.ELF" "2"
int command_parser(char *buf, char *args[]) {
	int arg_count = 0;
	int start_index = 0;

	while (arg_count < ARG_MAX) {
		for (int idx = start_index; idx < BUFFER_LENGTH; idx++) {
			if ((buf[idx] == ' ') || (buf[idx] == 0)) {
				args[arg_count++] = &buf[start_index];
				start_index = idx + 1;

				if (buf[idx] == 0) {
					return arg_count;
				}

				buf[idx] = 0;
				break;
			}
		}
	}

	return -1;
}

void print_help_new() {
	printf("  new <filename> <vcpu_num>\n");
}
void print_help_kill() {
	printf("  kill <vmid>\n");
}
void print_help_list() {
	printf("  list\n");
}
void print_help_shutdown() {
	printf("  shutdown\n");
}
void print_help_reboot() {
	printf("  reboot\n");
}
void print_help_debug() {
	printf("  debug hvc <hvc_nr>\n");
	printf("  debug smc <smc_nr>\n");
}
void print_help_help() {
	printf("  help\n");
}
void print_help() {
	print_help_new();
	print_help_kill();
	print_help_list();
	print_help_shutdown();
	print_help_reboot();
	print_help_debug();
	print_help_help();
}

void execute_debug_commands(char *args[], int arg_count) {
	if (EQUAL(args[1], "panic")) {
		printf("Triggering exception...\r\n");
		issue_hvc(HYPERCALL_TYPE_CAUSE_PANIC, 0, 0, 0);
	}
	else if (EQUAL(args[1], "hvc")) {
		printf("Triggering HVC call...\r\n");
		int int_args[HVC_ARG_MAX] = {0};
		for (int i=2; i < arg_count; i++) {
			int_args[i - 2] = atoi(args[i]);
		}
		issue_hvc(int_args[0], int_args[1], int_args[2], int_args[3]);
	}
	else if (EQUAL(args[1], "smc")) {
		printf("Triggering SMC call...\r\n");
		int int_args[SMC_ARG_MAX] = {0};
		for (int i=2; i < arg_count; i++) {
			int_args[i - 2] = atoi(args[i]);
		}
		issue_smc(int_args[0], int_args[1], int_args[2], int_args[3], int_args[4]);
	}
	else {
		printf("debug command error: %s\n", args[1]);
	}
}

void execute_command(char *buf) {
	char *args[ARG_MAX];
	// char *args[ARG_MAX] = {}; とすると -ffreestanding としても memset が参照されるため 
	for (int i=0; i < ARG_MAX; i++) {
		args[i] = 0;
	}

	int arg_count = command_parser(buf, args);
	if (arg_count < 0) {
		printf("command error\n");
		return;
	}

	if (EQUAL(args[0], "new")) {
		if (arg_count != 3) {
			print_help_new();
			return;
		}
		int vcpu_num = atoi(args[2]);
		
		printf("create a new vm '%s' with vcpu_num %d\n", args[1], vcpu_num);
		for (int i=0; (vm_args.filename[i] = args[1][i]); i++);
		vm_args.vcpu_num = vcpu_num;
		new_vm();
	}
	else if (EQUAL(args[0], "kill")) {
		if (arg_count != 2) {
			print_help_kill();
			return;
		}
		int vmid = atoi(args[1]);
		printf("killing vm %d...\n", vmid);
		destroy_vm(vmid);
	}
	else if (EQUAL(args[0], "list")) {
		printf("'list' is not supported.\n");
	}
	else if (EQUAL(args[0], "shutdown")) {
		shutdown_hv();
	}
	else if (EQUAL(args[0], "reboot")) {
		reboot_hv();
	}
	else if (EQUAL(args[0], "help")) {
		print_help();
	}
	else if (EQUAL(args[0], "debug")) {
		execute_debug_commands(args, arg_count);
	}
	else {
		printf("command error: %s\n", args[0]);
		print_help();
	}
}

void kernel_main(void)
{
	uart_init();
	init_printf(0, putc);
	
	irq_vector_init();
	enable_interrupt_controller();
	enable_irq();

	int count = 0;
	char buf[BUFFER_LENGTH];

	while (1) {
		count = 0;
		buf[count] = 0;
		printf("> ");

		while (1) {
			buf[count] = uart_recv();

			if (buf[count] == '\n' || buf[count] == '\r') {
				printf("\n");

				if (count == 0) {
					break;
				}

				buf[count] = 0;
				execute_command(buf);
				break;
			}
			else if (buf[count] == 127) {
				if (count > 0) {
					buf[count--] = 0;
					printf("\b\033[K");
				}
				continue;
			}

			uart_send(buf[count]);
			count++;

			if (count >= BUFFER_LENGTH) {
				printf("\nToo long!\n");
				break;
			}
		}
	}
}
