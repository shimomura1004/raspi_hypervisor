#ifndef _BOARDS_RASPI3_H
#define _BOARDS_RASPI3_H

// ハイパーバイザが動作する仮想アドレスと raspi3 の物理アドレスのマッピング
//   - VA: 0x0000_0000-0x3fff_ffff (Device region)
//       - VA: 0x1000_0000-0x1fff_ffff -> PA: 0x4000_0000-0x4fff_ffff (qa7)
//       - VA: 0x3f00_0000-0x3fff_ffff -> PA: 0x3f00_0000-0x3fff_ffff (armctrl)
//   - VA: 0x4000_0000-            (RAM)
//       - VA: 0x4000_0000-0x7fff_ffff -> PA: 0x0000_0000-0x3fff_ffff

// raspi3 ボード用の物理アドレス・仮想アドレスの変換マクロ
#define P2V_ARMCTRL(pa)     (pa)
#define V2P_ARMCTRL(pa)     (pa)

#define P2V_QA7(pa)         ((pa) - 0x30000000)
#define V2P_QA7(pa)         ((pa) + 0x30000000)

#define P2V_RAM(pa)         ((pa) + 0x40000000)
#define V2P_RAM(pa)         ((pa) - 0x40000000)

// 以下のマクロが使えるのは C ソースのみ
// todo: どうせ C からしか使えないなら関数にしたほうがいいか？
// qa7 は 0x40000000 以降
#define IN_DRAM_PA(pa)      (0x00000000 <= (pa) && (pa) < 0x3f000000)
#define IN_ARMCTRL_PA(pa)   (0x3f000000 <= (pa) && (pa) < 0x40000000)
#define IN_QA7_PA(pa)       (0x40000000 <= (pa) && (pa) < 0x50000000)
#define P2V(pa)             (IN_DRAM_PA(pa) \
                               ? P2V_RAM(pa) \
                               : (IN_ARMCTRL_PA(pa) \
                                    ? P2V_ARMCTRL(pa) \
                                    : (IN_QA7_PA(pa) \
                                         ? P2V_QA7(pa) \
                                         : 0)))

// Raspberry Pi 3B Specific Constants (物理アドレス)

// これは Arm コア側からみたアドレス(物理アドレス)
//   videocore からみたアドレスもある(バスアドレス)
// https://www.reddit.com/r/osdev/comments/uc98tz/raspberry_pi_3_base_peripheral_address/#:~:text=Physical%20addresses%20range%20from%200x3F000000,address%20range%20starting%20at%200x7E000000.

// BCM2837 ではペリフェラルが2つのブロックに分かれて配置されている
//   0x3F000000 - 0x3FFFFFFF : General Purpose I/O
//   0x40000000 - 0x401FFFFF : Local Peripherals

#define PA_RAM_BASE                 0x00000000
#define RAM_SIZE                    0x3F000000

// 利用可能な物理メモリの終端
// raspi3 では DRAM 末尾にデバイス領域(PA_DEVICE_BASE~)が重なっているため、その手前までを RAM として扱う
#define HIGH_MEMORY                 0x3F000000

// BCM2837 Peripheral Base
#define PA_PERIPHERAL_BASE          0x3F000000
#define PA_DEVICE_BASE              PA_PERIPHERAL_BASE
#define DEVICE_SIZE                 0x01000000

// ARM Local Peripheral Base (QA7)
#define PA_LOCAL_PERIPHERAL_BASE    0x40000000
#define LOCAL_PERIPHERAL_SIZE       0x00200000

// Use Mini UART for Raspberry Pi 3
#define UART_TYPE_MINI              1

#endif /* _BOARDS_RASPI3_H */
