extern checkcrc32
extern checkfibnacci
extern checkbase64
extern checkxorstring

# gogogo
SECTION .start
    vmmcall 
    push 0x1100
    push 0x1000
    call checkcrc32
    add esp, 0x8
    jmp hlt
    push 0x1100
    push 0x1000
    call checkfibnacci
    add esp, 0x8
    jmp hlt
    push 0x1100
    push 0x1000
    call checkbase64
    add esp, 0x8
    jmp hlt
    push 0x1100
    push 0x1000
    call checkxorstring
    add esp, 0x8
    jmp hlt
hlt:
    hlt


