#!/bin/bash

PROJECT_ROOT=$(git rev-parse --show-toplevel)
RELATIVE_PATH=$(pwd | sed "s|^${PROJECT_ROOT}||")

# 1234 ポートが空いている場合のみマッピングを行う
PORT_MAPPING=""
if ! lsof -Pi :1234 -sTCP:LISTEN -t >/dev/null 2>&1; then
    PORT_MAPPING="-p 1234:1234"
else
    echo "Port 1234 is in use, skipping GDB mapping."
fi

# 標準入出力がオープンされていたらコンテナを -it で起動する
TTY_OPTS=""
if [ -t 0 ] && [ -t 1 ]; then
    TTY_OPTS="-it"
fi

docker run --rm ${TTY_OPTS} -v ${PROJECT_ROOT}:/work -w /work${RELATIVE_PATH} ${PORT_MAPPING} -e BOARD=$BOARD raspvisor $*

