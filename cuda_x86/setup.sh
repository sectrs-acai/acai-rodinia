#!/bin/zsh
#
set -euo pipefail

b_fvp=( \
    "backprop" \
    "bfs" \
    "gaussian" \
    "heartwall" \
    "hotspot" \
    "needle" \
    "nn" \
    "pathfinder" \
    "srad_v1" \
    "srad_v2"\
    )

BENCH_OUT_DIR=/mnt/host/mnt/host/benchmark-single

function do_run {
    SCRIPT_DIR=${0:a:h}
    TS=$(date +"%Y-%m-%d_%H-%M-%S")
    DIR=$BENCH_OUT_DIR/$TS
    mkdir -p $DIR
    set +x
    num=10

    for b in ${b_fvp_ok[@]}; do
        for i in {1..$num}; do
            LOG=$DIR/$b
            echo "executing $b"

            cd $SCRIPT_DIR/$b
            cat ./run | tee -a $LOG
            exec ./run 2>&1 | tee -a $LOG
        done
    done
}

function do_nvcc {
    SCRIPT_DIR=${0:a:h}
    set +x
    for b in ${b_fvp[@]}; do
        cd $SCRIPT_DIR/$b && make nvcc && cd $SCRIPT_DIR
    done

    for b in ${b_x86[@]}; do
        cd $SCRIPT_DIR/$b && make nvcc && cd $SCRIPT_DIR
    done
}

function do_gcc {
    # must be executed with bash
    HERE_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
    source $HERE_DIR/../../../../scripts/env-aarch64.sh
    set +x
    for b in ${b_fvp[@]}; do
        cd $HERE_DIR/$b && make gcc && cd $HERE_DIR
    done
}

function do_clean {
    # must be executed with bash
    HERE_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
    set +x
    for b in ${b_fvp[@]}; do
        cd $HERE_DIR/$b && make clean && cd $HERE_DIR
    done
}


case "$1" in
    nvcc)
        do_nvcc
    ;;
    gcc)
        do_gcc
    ;;
    run)
        do_run
        ;;
    clean)
        do_clean
        ;;
    *)
        echo "unknown"
        exit 1
esac
