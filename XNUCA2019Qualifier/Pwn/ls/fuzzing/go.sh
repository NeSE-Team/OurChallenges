#!/bin/sh
AFL=/home/ww9210/develop/afl-2.52b/afl-fuzz
QEMU_MODE=-Q
INPUT_DIR=input/
OUTPUT_DIR=output/
DICT_FILE=dict.txt
FOLDER_NAME=fuzzor02
BINARY=/home/ww9210/develop/ls/lua-5.3.5/src/lua
NO_FORK_SERVER=AFL_NO_FORKSRV

mkdir ${OUTPUT_DIR}
${AFL} ${QEMU_MODE} -i ${INPUT_DIR} -m none -o ${OUTPUT_DIR} -t 200 -x ${DICT_FILE} -M ${FOLDER_NAME} ${BINARY}
