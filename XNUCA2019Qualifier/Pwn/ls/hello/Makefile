all:
	gcc -std=c99 -Os -fPIC -I/usr/local/include -c server_hello.c -o server_hello.o
	#gcc -o server_hello server_hello.o .luapak/lib/lua/5.3/compat53/utf8.a .luapak/lib/lua/5.3/lpeg.a .luapak/lib/lua/5.3/compat53/string.a .luapak/lib/lua/5.3/compat53/table.a /usr/local/lib/liblua.a /home/ww9210/luatest/examples/lua-http/examples/test_luapak/cqueues/_cqueues.a /usr/local/lib/lua/5.3/_openssl.so -L/usr/local/lib -L/usr/local/lib -lm -lpthread -lssl -lcrypto -ldl -lrt 
	gcc -o server_hello server_hello.o .luapak/lib/lua/5.3/compat53/utf8.a .luapak/lib/lua/5.3/lpeg.a .luapak/lib/lua/5.3/compat53/string.a .luapak/lib/lua/5.3/compat53/table.a /home/ww9210/develop/ls/lua-5.3.5/src/liblua.a ../cqueues/src/5.3/_cqueues.a .luapak/lib/lua/5.3/_openssl.a -L/usr/local/lib -L/usr/local/lib -lm -lpthread -lssl -lcrypto -ldl -lrt 
clean:
	rm server_hello server_hello.o
	rm -rf .luapak

