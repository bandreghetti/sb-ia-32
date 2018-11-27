SECTION TEXT    
    PUSH LABEL
    CALL LeerInteiro
    ENTER 4,0
    MOV DWORD[EBP-4],0
    MOV EBX, 0
    MOV EDX, 1
    MOV ECX, DWORD[EBP+8]
    MOV EAX, 3
    INT 0X80
    CMP BYTE[EAX], 0X20
    JE NEG
        PUSH 0
        MOV EAX,DWORD[ECX]
        SUB EAX, 0X30
    POS:
        CALL NUMBER
        PUSH EAXMOV ECX, 11
    LOOP1:
        MOV EAX, 3
        PUSH ECX
        MOV ECX, [ESP+24]
        INT 0X80
        CMP DWORD[ECX],0XA
        JE END
        MOV EAX, DWORD[ECX]
        SUB EAX, 0X30
        CALL DIGIT
        MOV DWORD[ECX], EAX
        MOV EAX, WORD[ESP+4]
        IMUL EAX, 10
        ADD EAX, BYTE[ECX]
        POP ECX
        MOV [ESP], EAX 
        LOOP LOOP1
    END:
        POP 
        POP EAX
        POP ECX
        CMP ECX, 0
        JE NOSIGNAL
        IMUL EAX, -1
    NOSIGNAL:
        LEAVE
        RET 4   
    DIGIT:
        CMP EAX, 9
        JG _EXIT
        CMP EAX, 0
        JL _EXIT
        RET 0
    NEG:
        PUSH 1
        MOV EAX, 0
        JMP POS    

SECTION DATA
SECTION BSS
    LABEL: SPACE  
