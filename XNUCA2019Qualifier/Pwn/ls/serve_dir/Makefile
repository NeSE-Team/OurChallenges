TARGET=serve_dir
TARGET_BUG=serve_dir_bug
SRC=serve_dir.c
SRC_BUG=serve_dir_bug.c
all:
	gcc -std=c99 -Os -fPIC -I/usr/local/include -c ${SRC} -o ${TARGET}.o
	#gcc -std=c99 -Os -fPIC -I/usr/local/include -c ${SRC_BUG} -o ${TARGET_BUG}.o
	gcc -o ${TARGET} ${SRC} .luapak/lib/lua/5.3/compat53/utf8.a .luapak/lib/lua/5.3/lpeg.a .luapak/lib/lua/5.3/compat53/string.a .luapak/lib/lua/5.3/compat53/table.a /home/ww9210/develop/ls/lua-5.3.5/src/liblua.a ../lfs_v1_6_3/src/lfs.a ../cqueues/src/5.3/_cqueues.a .luapak/lib/lua/5.3/_openssl.a -L/usr/local/lib -L/usr/local/lib -lm -lpthread -lssl -lcrypto -ldl -lrt 
	#gcc -o ${TARGET_BUG} ${SRC_BUG} .luapak/lib/lua/5.3/compat53/utf8.a .luapak/lib/lua/5.3/lpeg.a .luapak/lib/lua/5.3/compat53/string.a .luapak/lib/lua/5.3/compat53/table.a /home/ww9210/develop/ls/lua-5.3.5/src/liblua.a ../lfs_v1_6_3/src/lfs.a ../cqueues/src/5.3/_cqueues.a .luapak/lib/lua/5.3/_openssl.a -L/usr/local/lib -L/usr/local/lib -lm -lpthread -lssl -lcrypto -ldl -lrt 
clean:
	rm ${TARGET} ${TARGET}.o ${SRC}

