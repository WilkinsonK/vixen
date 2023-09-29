; This is an example ASM script.
; Meant only as a sanity check to ensure that
; things are working as expected.
%define SYS_EXIT 60

segment .text
global _start
_start:
    mov rax, SYS_EXIT
    mov rdi, 0
    syscall
