PUSH 8
PUSH 0 
POPR DI  
POPM [DI]
PUSH 15
PUSH 1 
POPR DI  
POPM [DI]
; ****************************************
PUSH 0  
POPR SI   
PUSHM [SI]
PUSH 1  
POPR SI   
PUSHM [SI]
ADD
PUSH 2 
POPR DI  
POPM [DI]
; ****************************************
CALL :100 
JMP :3  

:100      
:101      
PUSH 2  
POPR SI   
PUSHM [SI]
PUSH 0
JA :0
POPR   BP
POPR   BP
PUSH 0
JMP :1
:0
PUSH 1
:1
PUSH 0   
JNE  :2
POPR   BP
POPR   BP
RET
:2     
PUSH 2  
POPR SI   
PUSHM [SI]
PUSH 5
ADD
PUSH 6
MUL
PUSH 0 
POPR DI  
POPM [DI]
PUSH 2  
POPR SI   
PUSHM [SI]
PUSH 15
SUB
PUSH 2 
POPR DI  
POPM [DI]
; ****************************************
CALL :101
RET

:3
; ****************************************
IN
PUSH 1 
POPR DI  
POPM [DI]
; ****************************************
PUSH 2  
POPR SI   
PUSHM [SI]
PUSH 1  
POPR SI   
PUSHM [SI]
ADD
OUT
; ****************************************
HLT
