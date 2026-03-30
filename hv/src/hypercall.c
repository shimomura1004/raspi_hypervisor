#include "mm.h"
#include "loader.h"
#include "pm.h"
#include "hypercall.h"
#include "hypercall_type.h"
#include "psci.h"
#include "utils.h"
#include "debug.h"

// todo: これは common に移す？
void hypercall(unsigned long hvc_nr, unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3) {
    struct pt_regs *regs = vcpu_pt_regs(current_pcpu()->current_vcpu);
    unsigned long id = a0;

	// todo: 修正する
	// PSCI 呼び出しのチェック
	if ((id & 0xFF000000) == 0x84000000 || (id & 0xFF000000) == 0xC4000000) {
		switch (id) {
			case PSCI_0_2_FN_SYSTEM_OFF:
				INFO("PSCI SYSTEM_OFF called");
				system_shutdown();
				return;
			case PSCI_0_2_FN_SYSTEM_RESET:
				INFO("PSCI SYSTEM_RESET called");
				system_reboot();
				return;
			case PSCI_0_2_FN_PSCI_VERSION:
				regs->regs[0] = 0x00020000; // PSCI v2.0
				return;
			default:
				WARN("Unsupported PSCI call: 0x%lx", id);
				regs->regs[0] = -1; // NOT_SUPPORTED
				return;
		}
	}

    switch (id) {
	case HYPERCALL_TYPE_WARN_LU: {
		WARN("HVC #%lu(%lu)", id, a1);
		break;
    }
	case HYPERCALL_TYPE_INFO_LX: {
		INFO("HVC #%d: 0x%lx(%ld)", id, a1, a1);
		break;
    }
	case HYPERCALL_TYPE_INFO_LX_LX: {
		INFO("HVC #%d: 0x%lx(%ld), 0x%lx(%ld)", id, a1, a1, a2, a2);
		break;
    }
	case HYPERCALL_TYPE_INFO_LX_LX_LX: {
		INFO("HVC #%d: 0x%lx(%ld), 0x%lx(%ld), 0x%lx(%ld)", id, a1, a1, a2, a2, a3, a3);
		break;
    }

	case HYPERCALL_TYPE_INFO_STR: {
		INFO("HVC #%d: %s", id, (const char *)get_pa_2nd(a1));
		break;
	}

	case HYPERCALL_TYPE_CREATE_VM_FROM_ELF: {
		// 最初にこの VM に CPU 時間が割当たったタイミングで arg が使用される
		// よってゲストのメモリに依存しないようハイパーバイザ側にコピーしておく
		struct loader_args args = *(struct loader_args *)get_pa_2nd(a1);

		INFO("Prepare VM(%s) by hypercall", args.filename);
		// todo: VM の作成に失敗した場合の処理を追加する
        int vmid = create_vm_with_loader(elf_binary_loader, &args);
		// todo: この戻り値(VMID)は使われていないし、そもそも x8 に戻したところで意味がない
        regs->regs[8] = vmid;
		break;
    }

	case HYPERCALL_TYPE_DESTOY_VM: {
		INFO("Destroy VM(%d) by hypercall", (int)a1);
		destroy_vm((int)a1);
		break;
	}

	case HYPERCALL_TYPE_SHUTDOWN_HV: {
		INFO("Shutting down hypervisor");
		system_shutdown();
		break;
	}

	case HYPERCALL_TYPE_REBOOT_HV: {
		INFO("Rebooting hypervisor");
		system_reboot();
		break;
	}

	case HYPERCALL_TYPE_CAUSE_PANIC: {
		PANIC("Panic by hypercall");
		break;
	}

    default:
		WARN("uncaught hvc64 exception: %ld", id);
		break;
	}
}
