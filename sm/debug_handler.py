import sys
import os

# このスクリプトは gdb から使う想定なので、それ以外の環境で起動されたらエラーとする
try:
    import gdb
except ImportError:
    print("Error: This script must be run within GDB. Use 'source debug_handler.py' inside GDB.")
    sys.exit(1)

# プロジェクトルートに移動し、共通モジュールを読み込めるようにする
if '__file__' in globals():
    # sm/debug_handler.py の親の親がプロジェクトルートと仮定
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    os.chdir(project_root)
    if project_root not in sys.path:
        sys.path.append(project_root)

import debug_handler_common

config = {
    'initial': './sm/build/kernel8.elf',
    'el3': './sm/build/kernel8.elf',
    'el2': './hv/build/kernel8.elf',
    'el1': {
        1: './os/vmm/build/kernel8.elf',
        2: './os/raspios/build/kernel8.elf'
    }
}
debug_handler_common.register(config)
