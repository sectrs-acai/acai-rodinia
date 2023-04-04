#!/bin/zsh
#
set -euo pipefail
SCRIPT_DIR=${0:a:h}

b_fvp=( \
    "backprop" \
    "bfs" \
    "gaussian" \
    "heartwall" \
    "hotspot" \
    "lud" \
    "needle" \
    "nn" \
    "pathfinder" \
    "srad_v1" \
    "srad_v2"\
    )

b_x86=( \
"backprop-x86" \
    "bfs-x86" \
    "gaussian-x86" \
    "heartwall-x86" \
    "hotspot-x86" \
    "lud-x86" \
    "needle-x86" \
    "nn-x86" \
    "pathfinder-x86" \
    "srad_v1-x86" \
    "srad_v2-x86" )


b_fvp_ok=("backprop" \
        "bfs" \
        "gaussian" \
        "heartwall" \
        "needle" \
        "nn" \
        "pathfinder" \
        "srad_v1" \
        "srad_v2" )



BENCH_OUT_DIR=/tmp/benchmark-single

function do_run_fvp_single {
    TS=$(date +"%Y-%m-%d_%H-%M-%S")
    DIR=$BENCH_OUT_DIR/$TS
    mkdir -p $DIR
    set +x

    for b in ${b_fvp_ok[@]}; do
        LOG=$DIR/$b
        echo "executing $b"

        cd $SCRIPT_DIR/$b
        cat ./run | tee -a $LOG
        LD_LIBRARY_PATH=/mnt/mnt/host/assets/snapshots/aarch64-lib/ exec ./run 2>&1 | tee -a $LOG
    done
}

function do_nvcc {
    set +x
    for b in ${b_fvp[@]}; do
        cd $SCRIPT_DIR/$b && make nvcc && cd $SCRIPT_DIR
    done

    for b in ${b_x86[@]}; do
        cd $SCRIPT_DIR/$b && make nvcc && cd $SCRIPT_DIR
    done
}

case "$1" in
    nvcc)
        do_nvcc
    ;;
    fvp-single)
        do_run_fvp_single
        ;;
    *)
        echo "unknown"
        exit 1
esac