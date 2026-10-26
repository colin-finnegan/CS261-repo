    .global main
    .text

main:
    movq $10, %rbx
    movq $13, num1
    # 'symbol' is same as '( symbol )'
    movq ( num1 ) , %rax  
    addq %rbx, %rax
    jmp  _end
    subq $5, %rax

_end:
    ret

.data
num1:   .quad  0x1234567890abcdef

