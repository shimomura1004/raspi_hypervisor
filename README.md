# Hypervisor/Secure monitor implementation for Raspberry Pi 3B

## 参考にした実装
- [https://github.com/matsud224/raspvisor](https://github.com/matsud224/raspvisor)

## セキュアモニタ
### ビルド
- `make sm`

### 起動
- `./run_sm.sh`

## ハイパーバイザ
### ビルド
- `make hv`

### 起動
- `./run_hv.sh`
- VMM(virtual machine manager) OSS がコンソール1で動くので切り替える

### 終了
- VMM にて `shutdown` コマンドの実行 (QEMU 上での再起動)
- `Ctrl-a` -> `x` (QEMU の終了)

### ハイパーバイザの操作方法
|キー操作     |動作                        |
|------------|----------------------------|
|`?` -> `0-9`|コンソール切り替え            |
|`?` -> `t`  |システムタイマ情報の表示      |
|`?` -> `l`  |各 vCPU 上で動く VM 情報を表示|

### VMM のコマンド
|コマンド                                        |動作                        |
|-----------------------------------------------|----------------------------|
|`new` + `OS イメージのファイルパス` + `vcpu の数`   |指定した OS をゲストとして起動 |
|`kill` + `vmid`                                |指定した VMID の VM を終了する|
|`list`                                         |未サポート                   |
|`shutdown`                                     |ハイパーバイザの停止           |
|`reboot`                                       |ハイパーバイザの再起動         |

- ハイパーバイザが動作する qemu 環境ではファイルシステムは fs.img の中身になっている
- Makefile 内に fs.img の内容が書かれている
- たとえば RASPIOS を2コアで起動したければ、`new RASPIOS.ELF 2` と入力する
