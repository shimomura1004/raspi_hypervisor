#ifndef _SMCCC_H
#define _SMCCC_H

// SMCCC function identifier definition
// https://developer.arm.com/documentation/den0028/latest/
// [31] Call type
//   0b0: Fast call: 割込み不可
//   0b1: Yielding call: 割込みを許可する可能性がある
// [30] Size
//   0b0: SMC32
//   0b1: SMC64
// [29:24] Service call range
//   0x00: Arm Architecture Calls
//   0x01: CPU Service Calls
//   0x02: SiP Service Calls
//   0x03: OEM Service Calls
//   0x04: Standard Secure Service Calls
//   0x05: Standard Hypervisor Service Calls
//   0x06: Vendor Specific Hypervisor Service Calls
//   0x07: Vendor Specific EL3 Monitor Calls
//   0x08-0x2F: Reserved for future use
//   0x30-0x31: Trusted Application Calls
//   0x32-0x3F: Trusted OS Calls
// [23:17] Reserved
// [16] Hint bit denoting the absence of SVE specific live state
// [15:0] Function number

// Service Call Range
#define ARM_SMCCC_ARCH_CALL     0x00
#define ARM_SMCCC_STD_CALL      0x01    // PSCI はここに含まれる
#define ARM_SMCCC_VENDOR_CALL   0x30    // 自作の関数はここに含める

// PSCI Function IDs (SMC64)
#define PSCI_0_2_FN64_BASE      0xC4000000
#define PSCI_0_2_FN64_CPU_ON    0xC4000003

#endif /* _SMCCC_H */
