# Project overview
- このプロジェクトは Raspberry Pi 3 用の実験的なハイパーバイザ実装です

# Project structure
- `sm/src/`: セキュアモニタのソース
- `sm/include/`: セキュアモニタのヘッダ
- `hv/build/`: セキュアモニタのビルド成果物
- `hv/src/`: ハイパーバイザのソース
- `hv/include/`: ハイパーバイザのヘッダ
- `hv/build/`: ハイパーバイザのビルド成果物
- `os/`: ゲスト OS のソースやヘッダ
    - `vmm`: VM を起動するための管理用 OS
    - `raspios`: 動作確認を行うための最小構成の OS で、複数コアに対応している

# その他の指示
- 最終的な出力はすべて日本語で行ってください。
