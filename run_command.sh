#!/bin/bash

PROJECT_ROOT=$(git rev-parse --show-toplevel)
RELATIVE_PATH=$(pwd | sed "s|^${PROJECT_ROOT}||")

docker run --rm -it -v ${PROJECT_ROOT}:/work -w /work${RELATIVE_PATH} -p 1234:1234 -e BOARD=$BOARD raspvisor $*
