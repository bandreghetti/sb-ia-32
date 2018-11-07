; Programa que calcula a área de um triângulo
	TRIANGULO: EQU 1 ; teste comentário

SECTION TEXT
		INPUT		B
		INPUT		H
		LOAD		B
		MULT		H
		IF TRIANGULO
		DIV		DOIS
		STORE		R
		OUTPUT	R
		STOP
SECTION BSS
	B:		SPACE
	H:		SPACE
	R:		SPACE
	SECTION DATA
	DOIS:	CONST		0x02
