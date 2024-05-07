    .text
    .global print_tb
print_tb:
    pushl   %ebp
    movl    %esp, %ebp

    push    $print_tb_str
    call    puts 
    addl    $4, (%esp)
    jmp     return_addres

return_addres:
    movl    4(%ebp), %eax
    movl    (%ebp), %ebp
loop:
    cmpl    $bar, %eax
    jnz     not_bar

    push    $bar_str
    call    puts
    addl    $4, (%esp)
    jmp     return_addres
not_bar:
    cmpl    $foo, %eax
    jnz     not_foo

    push    $foo_str
    call    puts
    addl    $4, (%esp)
    jmp     return_addres
not_foo:
    cmpl    $main, %eax
    jnz     pre_loop
    push    $main_str
    call    puts
    addl    $4, (%esp)
    jmp     end
pre_loop:
    decl    %eax
    jmp     loop
end:
    movl    %ebp, %esp
    popl    %ebp
    xorl    %eax, %eax
    ret


.data
 main_str:
    .string "main"
print_tb_str:
    .string "print_tb"
bar_str:
    .string "bar"
foo_str:
    .string "foo"