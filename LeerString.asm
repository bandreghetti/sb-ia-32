SECTION TEXT    
    PUSH LABEL
    CALL LeerString
    ENTER 0,0
    MOV EBX, 0
    MOV EDX, 1
    MOV ECX, 100
    L1: MOV EAX, 3
        PUSH ECX
        MOV ECX, DWORD[ESB+12]
        INT 0X80
        CMP DWORD[ECX], 0XA
        INC ECX
        MOV DWORD[ESP+12], ECX
        POP ECX
        LOOPNE L1
    LEAVE 
    RET 0
SECTION DATA
SECTION BSS
    LABEL: SPACE  

 

