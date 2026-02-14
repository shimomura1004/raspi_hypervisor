import gdb
import os

# グローバル変数としてハンドラインスタンスを保持（再読み込み時の重複登録防止）
_current_handler = None

def register(config):
    global _current_handler
    # 既に登録されているハンドラがあれば解除を試みる（GDBの仕様上完全ではないが、二重実行防止）
    if _current_handler:
        try:
            gdb.events.stop.disconnect(_current_handler.stop_handler)
        except:
            pass
    
    _current_handler = DebugHandler(config)

class DebugHandler:
    def __init__(self, config):
        self.config = config
        self.last_vmid = -1
        self.last_el = -1
        self.setup_gdb()

    def setup_gdb(self):
        # GDB の初期設定
        gdb.execute("set arch aarch64")
        gdb.execute("set pagination off")

        # ターゲットに応じた初期シンボルファイルの読み込み
        elf_path = self.config.get('initial', '')
            
        try:
            if elf_path:
                gdb.execute(f"file {elf_path}")
        except Exception as e:
            print(f"[GDB Script] Warning: Failed to load file '{elf_path}': {e}")

        gdb.events.stop.connect(self.stop_handler)
        print(f"[GDB Script] VMID auto-switch handler loaded.")

    def get_reg(self, reg_name):
        try:
            val = gdb.parse_and_eval(reg_name)
            return int(val)
        except Exception:
            return None

    def stop_handler(self, event):
        try:
            # 現在の Exception Level を取得
            cpsr = self.get_reg("$cpsr")
            if cpsr is None:
                cpsr = self.get_reg("$CPSR")
            if cpsr is None:
                return

            el = (cpsr >> 2) & 0x3
            
            # VTTBR_EL2 から VMID を取得
            vttbr = self.get_reg("$VTTBR_EL2")
            if vttbr is None:
                vttbr = self.get_reg("$vttbr_el2")
            
            vmid = 0 if vttbr is None else (vttbr >> 48) & 0xFFFF

            if el == self.last_el and (el != 1 or vmid == self.last_vmid):
                return

            self.last_el = el
            self.last_vmid = vmid

            if el == 3:
                print(f"\n[GDB Script] Switched to EL3 (Secure Monitor)")
                path = self.config.get('el3')
                if path:
                    gdb.execute(f"symbol-file {path}")
            elif el == 2:
                print(f"\n[GDB Script] Switched to EL2 (Hypervisor)")
                path = self.config.get('el2')
                if path:
                    gdb.execute(f"symbol-file {path}")
            elif el == 1:
                print(f"\n[GDB Script] Switched to EL1 (Guest) VMID:{vmid}")
                el1_map = self.config.get('el1', {})
                path = el1_map.get(vmid)
                if path:
                    gdb.execute(f"symbol-file {path}")
                else:
                    print(f"[GDB Script] Unknown VMID {vmid}, unloading symbols.")
                    gdb.execute("symbol-file")

        except Exception as e:
            print(f"[GDB Script] Error: {e}")