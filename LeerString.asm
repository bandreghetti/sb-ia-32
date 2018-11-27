SECTION TEXT    
    PUSH LABEL
    CALL LeerString
    LeerString:
        ENTER 0,0
        MOV EBX, 0
        MOV EDX, 1
        MOV ECX, DWORD[EBP+12]
        L1: MOV EAX, 3
            PUSH ECX
            MOV ECX, DWORD[ESP+12]
            INT 0X80
        MOV EAX, ECX
        INC ECX
            CMP BYTE[ECX], 0XA
            MOV DWORD[ESP+12], ECX
            POP ECX
            LOOPNE L1
        LEAVE 
        RET 8
SECTION DATA
SECTION BSS
    LABEL: SPACE  

 

