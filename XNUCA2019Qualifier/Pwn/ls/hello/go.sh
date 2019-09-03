#!/bin/bash
PACKAGE_DIR=~/luatest/examples/lua-http:http-scm-0.rockspec
ENTRY_SCRIPT=./server_hello.lua
LUAPAK=luapak
#LUAPAK=
${LUAPAK} make ${PACKAGE_DIR} -s ${ENTRY_SCRIPT} -v
#luapak wrapper ${PACKAGE_DIR} -s ${ENTRY_SCRIPT} -v
#luapak analyse-deps ${PACKAGE_DIR} -s ${ENTRY_SCRIPT} -v
