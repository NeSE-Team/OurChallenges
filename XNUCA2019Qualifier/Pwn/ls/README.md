# xnuca2019_ls (lua server)
This challenge implements a file server with lua-http.
It is packed through luapak with lua 5.3.5.
I have patched luapak at several places to make this challenge.

## First Part: reverse
There is an oracle to leak some info about a 64-bit integer, the player is required to guess the secret within 255 interactions.
The intended solution is binary search.

## Second Part: pwn or sandbox?
Lua intepreter is prone to memory corruption (crashes are shown in fuzzing folder) if we could execute arbitrary bytecode[1][2].
The intended solution is the exploit technique used in [1] or [2].
However, we saw sandbox escape exploits during the competition: 
- using cqueues.socket to send back message [3]
- the permission setting for server_dir fails because the binary is executed as root user.
- the filename check in io.open is bypassed through other api.[3]

# Refereces:
[1] https://github.com/yough3rt/luasandbox
[2] https://saelo.github.io/posts/pwning-lua-through-load.html
[3] http://blog.leanote.com/post/xp0int/%5BPWN%5D-ls-cpt.shao%E3%80%81MF
[4] https://the-ravi-programming-language.readthedocs.io/en/latest/lua_bytecode_reference.html
