#!/usr/bin/env bash

set -x


update-alternatives --set gcc /usr/bin/gcc-4.8

export PATH="/usr/local/cuda/bin:$PATH"
export LD_LIBRARY_PATH="/usr/local/gdev/lib64:$LD_LIBRARY_PATH"
