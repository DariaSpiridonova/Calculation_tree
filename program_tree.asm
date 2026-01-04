 PUSH 8.000000
 PUSH 0 
POPR DI  
POPM [DI]
 PUSH 15.000000
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
 PUSH 7.000000
 PUSH 3  
POPR SI   
PUSHM [SI]
ADD
PUSH 0   
JNE  :0
POPR   BP
POPR   BP
JMP  :1
:0     
PUSH 2  
POPR SI   
PUSHM [SI]
 PUSH 5.000000
 ADD
 PUSH 6.000000
 MUL
PUSH 0 
POPR DI  
POPM [DI]
:1
; ****************************************
IN
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
OUT
; ****************************************
