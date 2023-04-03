#!/usr/bin/env bash
set -euo pipefail

readonly SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
readonly URL=http://www.cs.virginia.edu/~skadron/lava/Rodinia/Packages/rodinia_3.1.tar.bz2

set -x

mkdir -p $SCRIPT_DIR/build
cd $SCRIPT_DIR/build


readonly DOWNLOAD_FILE=rodinia_3.1.tar.bz2
readonly DOWNLOAD_EXTRACT=$SCRIPT_DIR/build/rodina

if [ ! -f "$DOWNLOAD_FILE" ]; then
    wget ${URL} -O $DOWNLOAD_FILE
    mkdir -p $DOWNLOAD_EXTRACT
    tar xf $DOWNLOAD_FILE -C $DOWNLOAD_EXTRACT
fi
mkdir -p $SCRIPT_DIR/bin

ln --relative -s $DOWNLOAD_EXTRACT/rodinia_3.1/data $SCRIPT_DIR/data && true
