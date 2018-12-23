



/*void printi(int x) {
#asm
    MOV DX, SS:[BP+4]
    xor  ah, ah
    mov  al, dl
    mov  dh, 10
    div  dh
    test al, 0ffh
    jz   single
    push ax
    xor  ah, ah
    div  dh
    test al, 0ffh
    jz   two
    push ax
    mov  dl, al
    add  dl, '0'
    mov  ah, 02h
    int  21h
    pop  ax
    two:
    mov  dl, ah
    add  dl, '0'
    mov  ah, 02h
    int  21h
    pop  ax
    single:
    mov  dl, ah
    add  dl, '0'
    mov  ah, 02h
    int  21h
    mov dl, 0ah
    int 21h
#endasm
return;
}*/

int fib(int x) {
    int a, b;
    if (x == 0 || x == 1)
        return 1;
    a = fib(x - 1);
    b = fib(x - 2);
    return a + b;
}

int main() {
    int x[10], i;
    int c,d;
    c=4-3;
    d=10;
    for (i = 0; i < 10; i++) {
        x[i]=fib(i);
    }
    for (i = 0; i < 10; i++) {
        printi(x[i]);
    }
    return 0;
}
