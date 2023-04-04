#!/usr/bin/env bash
set -euo pipefail

readonly SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
readonly DOCKER=docker
CONTAINER_NAME=arm-cca-rodina:latest
CMD="$@"
SOURCE=$SCRIPT_DIR/..

if [ -z "$CMD" ]; then
    printf "%s\n" "No command given. Aborting. E.g. call $0 /bin/bash"
    exit 1
fi

if [[ "$(docker images -q $CONTAINER_NAME 2> /dev/null)" == "" ]]; then
  printf "%s\n" "$CONTAINER_NAME does not exist. Running ./docker_build.sh first"
  $SCRIPT_DIR/docker_build.sh
fi

if [ -z ${DISPLAY+x} ]; then
    DISPLAY_CMD=""
else
   # Shares X11 DISPLAY env with docker:
   # this way we can launch xterm output of fvp from docker
   # troubleshoot: if command fails, grant docker access to X:
   # xhost +local:docker
   DISPLAY_CMD="-e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix"
fi

docker run --rm --env UID=$(id -u) \
        --env GID=$(id -g)  -i -t \
	   $DISPLAY_CMD \
	   --network host --security-opt label=disable \
	   --mount "type=bind,source=$SOURCE,target=/armcca" \
	   $CONTAINER_NAME /bin/bash -c "(cd /armcca && $CMD)"
