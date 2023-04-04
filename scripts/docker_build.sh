#!/usr/bin/env bash
#
# Builds docker image
#
readonly SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
readonly CONTAINER_NAME=arm-cca-rodina:latest

docker build --tag $CONTAINER_NAME $SCRIPT_DIR
