#!/usr/bin/env bash

sudo update-alternatives --set gcc /usr/bin/gcc-4.8
export PATH="/usr/local/cuda/bin:$PATH"
