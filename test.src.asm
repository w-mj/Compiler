SSEG	SEGMENT	STACK
STK	DB	100 DUP(0)
SSEG	ENDS
DSEG	SEGMENT
DSEG	ENDS
CSEG	SEGMENT
	ASSUME	CS:CSEG,DS:DSEG
	ASSUME	SS:SSEG
	MOV	AX,CSEG
	ADD	AX,7D0H
	MOV	DS,AX
	ADD	AX,7D0H
	MOV	ES,AX
	ADD	AX,7D0H
	MOV	SS,AX
	MOV	DI,0H
	MOV	BX,0EA60H
	MOV	[DI+0H],BX
	MOV	AH,4CH
	INT	21H
CSEG	ENDS
END main