#!/bin/bash

docker run --rm -it -v $(pwd):/work -w /work -e BOARD=$BOARD raspvisor $*
