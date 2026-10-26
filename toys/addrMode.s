  .global main
  .text

main:
  pushq %rbx

  mov   $message, %rdi
  xor   %rax, %rax
  call  printf
  
#  movq $35, %rax 

  movq $5, %rbx
  movq arr( , %rbx, 8) , %rax

  popq %rbx
  ret

  .data

message: .asciz "\nHello World!\n\n"
arr:     .quad 10, 20, 30, 0x40 , 50 , 60


