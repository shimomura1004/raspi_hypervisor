# Hypervisor implementation for Raspberry Pi 3B

## 参考にした実装
- [https://github.com/matsud224/raspvisor](https://github.com/matsud224/raspvisor)

## ハイパーバイザの起動
- `./run.sh`
- VMM(virtual machine manager) OSS がコンソール1で動くので切り替える

## ハイパーバイザの終了
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
|`new` + `OS イメージのファイルパス` + `vcpu の数`|指定した OS をゲストとして起動|
|`kill`                                         |未サポート                   |
|`list`                                         |未サポート                   |
|`shutdown`                                     |未サポート                   |

- ハイパーバイザが動作する qemu 環境ではファイルシステムは fs.img の中身になっている
- Makefile 内に fs.img の内容が書かれている
- たとえば RASPIOS を2コアで起動したければ、`new RASPIOS.ELF 2` と入力する
