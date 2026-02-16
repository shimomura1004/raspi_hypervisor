.section ".text.boot"

#include "../../../hv/include/hypercall_type.h"

.globl _start
_start:
  mov x8, #HYPERCALL_TYPE_WARN_LU
  mov x9, 777
  hvc #0

  ldr x9, hello_str
  mov x8, #HYPERCALL_TYPE_INFO_LX
  hvc #0
  mov x8, #HYPERCALL_TYPE_INFO_STR
  hvc #0

  mov x8, #HYPERCALL_TYPE_CREATE_VM_FROM_ELF
  ldr x9, echo_bin_args_p
  hvc #0
1:
  b 1b
