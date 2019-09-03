#!/bin/bash
PACKAGE_DIR=~/luatest/examples/lua-http:http-scm-0.rockspec
ENTRY_SCRIPT=./serve_dir.lua
LUAPAK=luapak
#LUAPAK=
make clean
${LUAPAK} make ${PACKAGE_DIR} -s ${ENTRY_SCRIPT} -v
#luapak wrapper ${PACKAGE_DIR} -s ${ENTRY_SCRIPT} -v
#luapak analyse-deps ${PACKAGE_DIR} -s ${ENTRY_SCRIPT} -v
make
./serve_dir 8000 ./work_dir
