#include "printf.h"
#include "utils.h"
#include "mini_uart.h"
#include "../../../include/loader.h"

#define BUFFER_LENGTH 128

void new_vm();
void destroy_vm(int vmid);

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
void print_help_help() {
	printf("  help\n");
}
void print_help() {
	print_help_new();
	print_help_kill();
	print_help_list();
	print_help_shutdown();
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
		printf("'shutdown' is not supported.\n");
	}
	else if (EQUAL(args[0], "help")) {
		print_help();
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

	int count = 0;
	char buf[BUFFER_LENGTH];

	while (1) {
		count = 0;
		buf[count] = 0;
		printf("> ");

		while (1) {
			// todo: busy loop ではなく sleep したい
			buf[count] = uart_recv();

			if (buf[count] == '\n' || buf[count] == '\r') {
				printf("\n");

				if (count == 0) {
					break;
				}

				buf[count] = 0;
				execute_command(buf);
				break;;
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
