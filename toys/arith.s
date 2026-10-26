#Colin Finnegan and Masar Salim
.section .note.GNU-stack

    .global main
    .text
main:
    pushq %rbx

    movq $1, %rbx #sets the value of register rbx to 1
    incb arr( , %rbx, 8) #increments the array index by 8 bytes
    movq arr( , %rbx, 8) , %r10 #moves value of arr[1] into %r10 = 0x03
    incq %rbx #increments the array index by 8 bytes
    addq arr( , %rbx, 8) , %r10 #adds value of arr[2] to %r10 = 0x08

dummy1:
    decq %rbx #decrement the array index by 8 bytes
    movq %r10, arr( , %rbx, 8) #move value in %r10 to arr[1]
    salq $4  , arr( , %rbx, 8) #shifts arr[1] to the left by 4 bits
    sarq $1  , arr( , %rbx, 8) #shifts arr[1] to the right by 1 bit

    movq arr( , %rbx, 8) , %rax #rax = 0x40

    popq %rbx
    ret

    .data
arr: 
    .quad 0x30 , 0x02 
    .quad 0x05

dummy2:
    .quad 0x06 , 0x0e 
    .quad 0x0f
