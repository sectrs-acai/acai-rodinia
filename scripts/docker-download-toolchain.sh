#!/usr/bin/env bash
set -euo pipefail
readonly SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

function download_none_elf {
    local TOOLCHAIN_URL="https://developer.arm.com/-/media/Files/downloads/gnu/11.3.rel1/binrel/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-elf.tar.xz?rev=73ff9780c12348b1b6772a1f54ab4bb3&hash=57C22236F869382FCF207B9A011E37D6438714EF"

    set -x

    mkdir -p $SCRIPT_DIR/build
    local BUILD_DIR=$SCRIPT_DIR/build
    cd $BUILD_DIR

    local TOOLCHAIN_DIR=$BUILD_DIR/aarch64-none-elf
    local TOOLCHAIN_GZ=$BUILD_DIR/aarch64-none-elf.tar.gz
    local TOOLCHAIN_BIN=$TOOLCHAIN_DIR/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-elf

    if [ ! -f "$TOOLCHAIN_GZ" ]; then
        echo $TOOLCHAIN_URL
        wget $TOOLCHAIN_URL -O $TOOLCHAIN_GZ
        mkdir -p $TOOLCHAIN_DIR
        tar xf $TOOLCHAIN_GZ -C $TOOLCHAIN_DIR
    fi

    ln --relative -s $TOOLCHAIN_BIN $SCRIPT_DIR/aarch64-none-elf ||  true # ignore existing link

    export PATH="$PATH:$SCRIPT_DIR/aarch64-none-elf/bin"
}

function download_linux-gnu {

    local TOOLCHAIN_URL="https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-x86_64-aarch64-none-linux-gnu.tar.xz?rev=6750d007ffbf4134b30ea58ea5bf5223&hash=0F1CE8273B8A30129CA04BD61FFB547D"

    set -x

    mkdir -p $SCRIPT_DIR/build
    local BUILD_DIR=$SCRIPT_DIR/build
    cd $BUILD_DIR

    local TOOLCHAIN_DIR=$BUILD_DIR/aarch64-none-linux-gnu
    local TOOLCHAIN_GZ=$BUILD_DIR/aarch64-none-linux-gnu.tar.gz
    local TOOLCHAIN_BIN=$TOOLCHAIN_DIR/arm-gnu-toolchain-12.2.rel1-x86_64-aarch64-none-linux-gnu

    if [ ! -f "$TOOLCHAIN_GZ" ]; then
        echo $TOOLCHAIN_URL
        wget $TOOLCHAIN_URL -O $TOOLCHAIN_GZ
        mkdir -p $TOOLCHAIN_DIR
        tar xf $TOOLCHAIN_GZ -C $TOOLCHAIN_DIR
    fi

    ln --relative -s $TOOLCHAIN_BIN $SCRIPT_DIR/aarch64-none-linux-gnu ||  true # ignore existing link

    export PATH="$PATH:$SCRIPT_DIR/aarch64-none-linux-gnu/bin"
}

download_linux-gnu
# download_none_elf

# export CROSS_COMPILE=$TOOLCHAIN_BIN/aarch64-none-elf-
