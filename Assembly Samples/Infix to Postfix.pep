;
; Program #5: Infix to Postfix
;
; Pedro De Los Reyes
;

         br      main
         charArr:.block 10   ; global array for chars with 10 slots, 1 byte each
         arrPnt: .block 2    ; global variable to act as the stack pointer
         c:      .block 1    ; global variable holds current char
         sp:     .block 2    ; global variable holds precedence of top of stack
         cp:     .block 2    ; global variable holds precedenc of current char c
main:    ldx     0, i
         stx     arrPnt, d   ; top of stack at 0
         addx    1, i        
         lda     '$', i      ; store char '$' as initial value on stack
         sta     charArr, x  ; for precedence and to empty stack at end of program
         stx     arrPnt, d
         lda     -1, i
         sta     sp, d       ; sets precedence of top of stack to -1
charLoop:chari   c, d 
         lda     0, i
         ldbytea c, d
         cpa     ' ', i      ; ignore spaces
         breq    charLoop
         cpa     '!', i      ; end of the postfix
         breq    outLoop
         cpa     '(', i
         breq    oParen
         cpa     '*', i
         breq    multDiv
         cpa     '/', i      
         breq    multDiv
         cpa     '+', i
         breq    addSub
         cpa     '-', i
         breq    addSub         
         cpa     ')', i
         breq    cParen
         charo   c, d
         br      charLoop
oParen:  lda     0, i
         sta     cp, d
         cpa     sp, d
         brgt    addStk      ; if current char precedece > sp precedence, push
         ldx     arrPnt, d
         charo   charArr, x  ; else output operator
         subx    1, i
         stx     arrPnt, d  
         br      charLoop
multDiv: lda     3, i
         sta     cp, d
         cpa     sp, d      
         brgt    addStk
         ldx     arrPnt, d
         charo   charArr, x
         subx    1, i
         stx     arrPnt, d
         br      charLoop
addSub:  lda     2, i
         sta     cp, d
         cpa     sp, d      
         brgt    addStk
         ldx     arrPnt, d
         charo   charArr, x
         subx    1, i
         stx     arrPnt, d
         br      charLoop
cParen:  ldx     arrPnt, d
         subx    1, i
         stx     arrPnt, d 
         lda     0, i
         ldbytea charArr, x
         cpa     '(', i
         breq    stkMin1
         charo   charArr, x
         ldx     arrPnt, d      
         br      cParen
stkMin1: ldx     arrPnt, d   ; after popping till '(' is reached
         subx    1, i        ;   decrement stack pointer and update 
         stx     arrPnt, d   ;   precedence of top of stack
         subsp   2, i
         subsp   1, i
         call    getPrec
         addsp   1, i
         addsp   2, i
         lda     -2, s
         sta     cp, d
         br      charLoop                             
addStk:  ldx     arrPnt, d   ; make room for value to add to stack array
         addx    1, i        ; increment stack pointer by 1
         lda     c, d        
         sta     charArr, x  ; store char c to top of stack array
         stx     arrPnt, d
         lda     cp, d
         sta     sp, d       ; update the precedence of the top of the stack
         br      charLoop
outLoop: ldx     arrPnt, d   ; output whats left on the stack and end program
         charo   charArr, x
         subx    1, i
         stx     arrPnt, d 
         cpx     0, i
         brgt    outLoop
         stop
;_________getPrecedence(char param)___________________
;
; takes a char as a parameter and returns its precedence

retVal:  .equate 3 
param:   .equate 2   
 
getPrec: lda     0 ,i  
         ldbytea param, s    ; param
         charo   param, s    ; for debugging
         cpa     '*', i      ; If char is '*' or '/' 
         breq    prec3       
         cpa     '/', i      
         breq    prec3
         cpa     '+', i      ; If char is '+' or '-'    
         breq    prec2
         cpa     '-', i      
         breq    prec2
         cpa     '(', i      ; If char is '(' 
         breq    prec0       ; If char is anything else 
         lda     -1, i       
         sta     retVal,s    ; return -1
         br      end 
prec3:   lda     3, i
         sta     retVal, s   ; return 3
         br      end
prec2:   lda     2, i
         sta     retVal, s   ; return 2
         br      end
prec0:   lda     0, i
         sta     retVal, s   ; return 0
end:     ret0
;________________________________________

.end
         