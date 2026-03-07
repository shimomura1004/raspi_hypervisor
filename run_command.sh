#!/bin/bash

docker run --rm -it -v $(pwd):/work -w /work raspvisor $*
