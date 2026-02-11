# Project overview
- このプロジェクトは Raspberry Pi 3 用の実験的なハイパーバイザ実装です

# Project structure
- `src/`: ハイパーバイザのソース
- `include/`: ハイパーバイザのヘッダ
- `os/`: ゲスト OS のソースやヘッダ
    - `vmm`: VM を起動するための管理用 OS
    - `raspios`: 動作確認を行うための最小構成の OS で、複数コアに対応している
- `build/`: ビルド成果物
