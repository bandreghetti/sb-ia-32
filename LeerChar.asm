SECTION TEXT
    PUSH LABEL
    CALL LeerChar
    LeerChar:
        ENTER 0,0
        MOV ECX, [EBP+8]
        MOV EAX, 3
        MOV EBX, 0
        MOV EDX, 1
        INT 0X80
        LEAVE
        RET 0

SECTION DATA    
SECTION BSS
    LABEL: SPACE    

