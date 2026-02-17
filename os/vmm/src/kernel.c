#include "printf.h"
#include "utils.h"
#include "mini_uart.h"
#include "irq.h"
#include "../../../hv/include/loader.h"
#include "../../../hv/include/hypercall_type.h"

#define BUFFER_LENGTH 128

void new_vm();
void destroy_vm(int vmid);
void shutdown_hv();
void reboot_hv();
void issue_hvc(int hvc_nr);
void issue_smc(int smc_nr);

struct loader_args vm_args = {
	.loader_addr = 0x0,
	.entry_point = 0x0,
	.sp = 0x100000,
	.filename = "",
};
struct loader_args *vm_args_p = &vm_args;

int strncmp(char *a, char *b, int n) {
	for (int i=0; i < n; i++) {
		if (a[i] != b[i]) {
			return 1;
		}
	}
	return 0;
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

#define ARG_MAX 8
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

#define EQUAL(A, B) (strncmp(A, B, sizeof(B)) == 0)
void execute_debug_commands(char *args[], int arg_count) {
	if (EQUAL(args[1], "panic")) {
		printf("Triggering exception...\r\n");
		issue_hvc(HYPERCALL_TYPE_CAUSE_PANIC);
	}
	else if (EQUAL(args[1], "hvc")) {
		// todo: 任意の hvc を呼ぶのではなく、決められたハイパーコールを呼ぶ関数群とするべき
		printf("Triggering HVC call...\r\n");
		// todo: 第二引数以降も渡す
		issue_hvc(args[2][0] - '0');
	}
	else if (EQUAL(args[1], "smc")) {
		printf("Triggering SMC call...\r\n");
		// todo: 第二引数以降も渡す
		issue_smc(args[2][0] - '0');
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
		printf("create a new vm '%s' with vcpu_num %s\n", args[1], args[2]);
		for (int i=0; (vm_args.filename[i] = args[1][i]); i++);
		vm_args.vcpu_num = *args[2] - '0';
		new_vm();
	}
	else if (EQUAL(args[0], "kill")) {
		if (arg_count != 2) {
			print_help_kill();
			return;
		}
		printf("killing vm %s...\n", args[1]);
		destroy_vm(*args[1] - '0');
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
