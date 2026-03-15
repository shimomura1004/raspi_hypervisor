# Hypervisor/Secure monitor implementation for Raspberry Pi 3B

## 参考にした実装
- [https://github.com/matsud224/raspvisor](https://github.com/matsud224/raspvisor)

## ビルド
- ビルド時には環境変数 BOARD によりターゲットボードの選択が必要
  - `BOARD=raspi3 make`
  - `BOARD=virt make`

### セキュアモニタ
- `BOARD=<BOARD> make sm`

### ハイパーバイザ
- `BOARD=<BOARD> make hv`

## 起動
### セキュアモニタ
- `BOARD=<BOARD> ./run_sm.sh`

### ハイパーバイザ
- `BOARD=<BOARD> ./run_hv.sh`

## 終了
- VMM にて `shutdown` コマンドの実行 (QEMU 上での再起動)
- `Ctrl-a` -> `x` (QEMU の終了)

## ハイパーバイザの操作方法
|キー操作     |動作                        |
|------------|----------------------------|
|`?` -> `0-9`|コンソール切り替え            |
|`?` -> `t`  |システムタイマ情報の表示      |
|`?` -> `l`  |各 vCPU 上で動く VM 情報を表示|

## VMM のコマンド
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
