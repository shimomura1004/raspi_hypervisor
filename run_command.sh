#!/bin/bash

PROJECT_ROOT=$(git rev-parse --show-toplevel)
RELATIVE_PATH=$(pwd | sed "s|^${PROJECT_ROOT}||")

docker run --rm -it -v ${PROJECT_ROOT}:/work -w /work${RELATIVE_PATH} -e BOARD=$BOARD raspvisor $*
