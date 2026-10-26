.section .note.GNU-stack

.section .text
.global main

main:

    pushq   %rbp

    movabsq     $0x0f0f0f0f0f0f0f0f , %rbx       # Init the %rbx register

    # printf ("Initially, %%rbx = %08lx\n" , rbx );
    mov     $format1, %rdi    # 1st arg
    mov     %rbx, %rsi        # 2nd arg
    xor     %rax, %rax        # Must zero %rax before calling printf
    call    printf

    # pushq 0x8877665544332211            # does not work
    
    movabs  $0x8877665544332211 , %r10     # But, this works
    push    %r10
    popq    %rbx 

    # printf ("After push/pop:  %%rbx = %08lx\n",  rbx);
    mov     $format2, %rdi    # 1st arg
    mov     %rbx, %rsi        # 2nd arg
    xor     %rax, %rax        # Must zero %rax before calling printf
    call    printf

    # printf ("i=%d ,  j=%d ,  k=%d\n", i , j , k);
    mov     $format3, %rdi
    mov     i, %rsi           # %rsi = Mem[i] 
    mov     j, %rdx           # %rdx = Mem[j]
    mov     k, %rcx           # %rcx = Mem[k]
    xor     %rax, %rax        # Must zero %rax before calling printf
    call    printf

    mov     i, %rax
    pushq   %rax 
    pushq   j
    popq    ( k )             # Remember:  'k' is the same as '(k)'
    popq    j
    
    # printf ("i=%d ,  j=%d ,  k=%d\n", i , j , k);
    LEAQ    format3, %rdi     # mov  $format3 , %rdi
    mov     i, %rsi           # %rsi = Mem[i] 
    mov     j, %rdx           # %rdx = Mem[j]
    mov     k, %rcx           # %rcx = Mem[k]
    xor     %rax, %rax        # Must zero %rax before calling printf
    call    printf

    popq    %rbp

    ret
#---------------------------------------------
.section .data
# Remember: the value of each symbol is its memory address
i:  .quad 0x1122334455667788
j:  .quad 0xDDEEFF
k:  .quad 0xAABBCC11

#---------------------------------------------
.section    .rodata
format1:    .string "\nInitially:        %%rbx = 0x%016lx\n"
format2:    .string "\nAfter push/pop:   %%rbx = 0x%016lx\n"
format3:    .asciz  "\ni = 0x%016lX , j = 0x%016lX , k = 0x%016lX\n\n"





