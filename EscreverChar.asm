SECTION TEXT    
    PUSH LABEL
    CALL EscreverChar
    EscreverChar:
        ENTER 0,0
        MOV ECX, [EBP+8]
        MOV EAX, 4
        MOV EBX, 1
        MOV EDX, 1
        INT 0X80
        LEAVE 
        RET 0
SECTION DATA
SECTION BSS
    LABEL: SPACE    
 

