#ifndef	_P_BASE_H
#define	_P_BASE_H

#include "board_config.h"

// これは Arm コア側からみたアドレス(物理アドレス)
//   videocore からみたアドレスもある(バスアドレス)
// https://www.reddit.com/r/osdev/comments/uc98tz/raspberry_pi_3_base_peripheral_address/#:~:text=Physical%20addresses%20range%20from%200x3F000000,address%20range%20starting%20at%200x7E000000.
// BOARD 固有の定義を使用
#define DEVICE_BASE 		PERIPHERAL_BASE
#define PBASE 			(VA_START + DEVICE_BASE)

#endif  /*_P_BASE_H */
