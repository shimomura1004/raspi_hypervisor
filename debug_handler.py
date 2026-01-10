import sys
import os
try:
    import gdb
except ImportError:
    print("Error: This script must be run within GDB. Use 'source debug_handler.py' inside GDB.")
    sys.exit(1)

# スクリプトのあるディレクトリにカレントディレクトリを変更(launch.json の cwd 指定が効かないため)
if '__file__' in globals():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

# GDB の初期設定
gdb.execute("set arch aarch64")
gdb.execute("set pagination off")

# 起動時にシンボルファイルを読み込んでアーキテクチャを確定させる
try:
    gdb.execute("file ./build/kernel8.elf")
except Exception as e:
    print(f"[GDB Script] Warning: Failed to load file './build/kernel8.elf': {e}")

def get_reg(reg_name):
    try:
        val = gdb.parse_and_eval(reg_name)
        return int(val)
    except Exception:
        return None

last_vmid = -1
last_el = -1

def stop_handler(event):
    global last_vmid, last_el
    try:
        # 現在の Exception Level を取得
        # PSTATE (CPSR) の [3:2] ビットが EL を表す
        cpsr = get_reg("$cpsr")
        if cpsr is None:
            cpsr = get_reg("$CPSR")
        if cpsr is None:
            return

        el = (cpsr >> 2) & 0x3
        
        # VTTBR_EL2 から VMID を取得 (上位16ビット [63:48])
        vttbr = get_reg("$VTTBR_EL2")
        if vttbr is None:
            vttbr = get_reg("$vttbr_el2")
        
        vmid = 0 if vttbr is None else (vttbr >> 48) & 0xFFFF

        # 状態が変わっていなければ何もしない
        if el == last_el and (el != 1 or vmid == last_vmid):
            return

        last_el = el
        last_vmid = vmid

        # 変わっていればシンボル情報を切り替え
        
        if el == 2:
            print(f"\n[GDB Script] Switched to EL2 (Hypervisor)")
            gdb.execute("symbol-file ./build/kernel8.elf")
            
        elif el == 1:
            print(f"\n[GDB Script] Switched to EL1 (Guest) VMID:{vmid}")
            
            if vmid == 1:
                # VMID 1 用のシンボル (VMM 決め打ち)
                gdb.execute("symbol-file ./example/vmm/build/kernel8.elf")
            elif vmid == 2:
                # VMID 1 用のシンボル (RASPIOS 決め打ち)
                gdb.execute("symbol-file ./example/raspios/build/kernel8.elf")
            else:
                # その他の VM (IDLE VM など)
                print(f"[GDB Script] Unknown VMID {vmid}, unloading symbols.")
                gdb.execute("symbol-file")

    except Exception as e:
        # エラーが発生したら表示する
        print(f"[GDB Script] Error: {e}")

gdb.events.stop.connect(stop_handler)
print("[GDB Script] VMID auto-switch handler loaded.")
