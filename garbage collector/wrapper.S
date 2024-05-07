    .global gc_collect
gc_collect:
    push %ebp
    mov %esp, %ebp
    push %ebx
    push %esi
    push %edi

    push %esp
    call gc_collect_impl
    add $4, %esp

    pop %edi
    pop %esi
    pop %ebx
    pop %ebp
    ret