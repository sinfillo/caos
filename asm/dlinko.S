    .data
end_a:
    .int 0
ind_a:
    .int 0
end_b:
    .int 0
ind_b:
    .int 0
sz:
    .int 0
    
    .text
    .global supersum
supersum:
    mov     %esi, end_a
    mov     %edi, end_b
    push    %esi
    push    %edi
    jmp     clear_a
clear_a:
    cmpl    $0, end_a
    jz      clear_b
    decl    end_a
    mov     end_a, %esi
    incl    end_a
    cmpb    $0, (%eax, %esi, 1)
    jnz     prepare
    decl    end_a
    jmp     clear_a
clear_b:
    cmpl    $0, end_b
    jz      prepare
    decl    end_b
    mov     end_b, %esi
    incl    end_b
    cmpb    $0, (%ebx, %esi, 1)
    jnz     prepare
    decl    end_b
    jmp     clear_b
prepare:
    clc
    pushf
    jmp     get_c
get_c:
    mov     end_b, %esi
    cmpl    ind_b, %esi
    jz      add_a_ost
    mov     end_a, %esi
    cmpl    ind_a, %esi
    jz      add_b_ost
    mov     ind_a, %esi
    movb    (%eax, %esi, 1), %dl
    mov     ind_b, %esi
    popf
    adcb    (%ebx, %esi, 1), %dl
    pushf
    mov     sz, %esi
    incl    sz
    incl    ind_a
    incl    ind_b
    mov     %dl, (%ecx, %esi, 1)
    jmp     get_c
add_a_ost:
    mov     end_a, %esi
    cmpl    ind_a, %esi
    jz      end
    mov     ind_a, %esi
    movb    (%eax, %esi, 1), %dl
    popf
    adcb    $0, %dl
    pushf
    mov     sz, %esi
    incl    sz
    incl    ind_a
    mov     %dl, (%ecx, %esi, 1)
    jmp     get_c
add_b_ost:
    mov     end_b, %esi
    cmpl    ind_b, %esi
    jz      end
    mov     ind_b, %esi
    movb    (%ebx, %esi, 1), %dl
    popf
    adcb    $0, %dl
    pushf
    mov     sz, %esi
    incl    sz
    incl    ind_b
    mov     %dl, (%ecx, %esi, 1)
    jmp     get_c
end:
    popf
    jc      add_new
    jmp     end_prog
add_new:
    mov     sz, %esi
    movb    $1, (%ecx, %esi, 1)
    incl    sz
    jmp     end_prog
end_prog:
    mov     sz, %eax
    pop     %esi
    pop     %edi
    ret
    