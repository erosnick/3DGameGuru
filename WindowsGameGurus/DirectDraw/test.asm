.CODE

myAdd PROC
    add rcx, rdx
    mov rax, rcx
    ret
myAdd ENDP

mySub PROC
    sub rcx, rdx
    mov rax, rcx
    ret
mySub ENDP

; rcx - First argument
; rdx - Second argument
; r8 - Third argument
; r9 - Fourth argument
Fill PROC
    cld             ; Clear direction of copy to forward
    mov rax, rdx    ; Color goes here
    mov rdi, r8     ; Number of DWORDS goes here
    rep stosd       ; Send the Pentium X on its way...
    ret
Fill ENDP

END
