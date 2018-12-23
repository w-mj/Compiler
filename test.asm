SSEG	SEGMENT	STACK
STK	DB	100 DUP(0)
SSEG	ENDS
DSEG	SEGMENT
DSEG	ENDS
CSEG	SEGMENT
	ASSUME	CS:CSEG,DS:DSEG
	ASSUME	SS:SSEG
printi	PROC	NEAR
	MOV	BP,SP
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
	MOV	DI,SS:[BP+2]
	RET
printi	ENDP
fib	PROC	NEAR
	MOV	BP,SP
	MOV	AX,SS:[BP+4+0H]
	MOV	BX,0H
	CMP	AX,BX
	JNZ	mark7
	MOV	ES:[0H],01H
	JMP	mark8
mark7:	MOV	ES:[0H],00H
mark8:	MOV	AX,SS:[BP+4+0H]
	MOV	BX,1H
	CMP	AX,BX
	JNZ	mark9
	MOV	ES:[2H],01H
	JMP	mark10
mark9:	MOV	ES:[2H],00H
mark10:	MOV	AX,ES:[0H]
	OR	AX,ES:[2H]
	JZ	mark11
	MOV	ES:[4H],01H
	JMP	mark12
mark11:	MOV	ES:[4H],00H
mark12:	MOV	AX,ES:[4H]
	AND	AX,AX
	JZ	mark1
	MOV	AL,1H
	MOV	DI,SS:[BP+2]
	RET
mark1:	MOV	AX,SS:[BP+4+0H]
	MOV	BX,1H
	SUB	AX,BX
	MOV	ES:[6H],AX
	MOV	AX,ES:[6H]
	PUSH	AX
	PUSH	DI
	ADD	DI,4H
	CALL	fib
	POP	 DX
	POP	 DX
	MOV	BP,SP
	MOV	ES:[8H],AX
	MOV	BX,ES:[8H]
	MOV	[DI+2H],BX
	MOV	AX,SS:[BP+4+0H]
	MOV	BX,2H
	SUB	AX,BX
	MOV	ES:[0AH],AX
	MOV	AX,ES:[0AH]
	PUSH	AX
	PUSH	DI
	ADD	DI,4H
	CALL	fib
	POP	 DX
	POP	 DX
	MOV	BP,SP
	MOV	ES:[0CH],AX
	MOV	BX,ES:[0CH]
	MOV	[DI+0H],BX
	MOV	AX,[DI+2H]
	MOV	BX,[DI+0H]
	ADD	AX,BX
	MOV	ES:[0EH],AX
	MOV	AX,ES:[0EH]
	MOV	DI,SS:[BP+2]
	RET
fib	ENDP
main:
	MOV	AX,CSEG
	ADD	AX,7D0H
	MOV	DS,AX
	ADD	AX,7D0H
	MOV	ES,AX
	ADD	AX,7D0H
	MOV	SS,AX
	MOV	DI,0H
	MOV	BX,5H
	MOV	[DI+0H],BX
mark5:	MOV	AX,[DI+0H]
	MOV	BX,0AH
	CMP	AX,BX
	JAE	mark13
	MOV	ES:[10H],01H
	JMP	mark14
mark13:	MOV	ES:[10H],00H
mark14:	MOV	AX,ES:[10H]
	AND	AX,AX
	JZ	mark3
	JMP	mark4
mark6:	MOV	AX,[DI+0H]
	INC	AX
	MOV	[DI+0H],AX
	JMP	mark5
mark4:	MOV	AX,[DI+0H]
	PUSH	AX
	PUSH	DI
	ADD	DI,4H
	CALL	fib
	POP	 DX
	POP	 DX
	MOV	BP,SP
	MOV	ES:[12H],AX
	MOV	BX,ES:[12H]
	MOV	[DI+2H],BX
	MOV	AX,[DI+2H]
	PUSH	AX
	PUSH	DI
	ADD	DI,4H
	CALL	printi
	POP	 DX
	POP	 DX
	MOV	BP,SP
	JMP	mark6
mark3:	MOV	AH,4CH
	INT	21H
CSEG	ENDS
END main
