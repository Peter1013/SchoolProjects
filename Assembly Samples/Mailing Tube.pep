; _______________Program #6: Mailing Tube____________________________________
;                
;                By: Pedro De Los Reyes
;
; Program takes a length M, a number of objects N, and lengths and values 
; corresponding to those N objects. Outputs the combination of objects with 
; the maximum value that fits in the tube.


         br      main
;________________Variables___________________________________________________

M:       .block  2           ; Maximum length of our tube, determines maximum 
; total length of objects in our tube

N:       .block  2           ; Number of objects to chose from, to fit in tube

squareN: .block  2           ; Value of 2^n calculated for testing each possible
; combination of objects(0 to 2^n represents combinations on a bitwise level)

maxVal:  .block  2           ; Stores the maximum combination value determined so 
; far. Will contain the maximum value after all combinations have been tested

maxCom:.block  2           ; Stores the bitwise representation of the 
; combination so far( 3 would be 110 where object 0, and 1, but not 2 are counted
; n determines the number of digit places utilized)
tempLen: .block  2           ; holds the length of the current combination being tested
tempCom: .block  2           ; hold the bitwise representation of the current 
; combination being tested
tempVal: .block  2           ; holds the combined value of current combination being
; calculated to test against maxVal
j:       .block  2           ; j, k, used for loop condition
k:       .block  2
tempJ:   .block  2           ; temporary j value will be ASR, n number of times 
Row:     .block  2      
offset:  .equate 2           ; byte offset of lengths
Arr:     .block  60          ; Global array to hold object value followed by weight
; Every 4 bytes is a value/length pair, accessing an object's value requires a byte 
; offset of 4 and accesing a value's length is the value offset + 2 bytes

Masci:   .ascii  "M: \x00"   ; Ascii constants
Nasci:   .ascii  "N: \x00"
Colon:   .ascii  ": \x00"  
subs:    .ascii  "Subset: \x00"
wght:    .ascii  "Total Value: \x00"
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 
main:    deci    M,d
         deci    N,d
         ldx     0,i    
         stx     maxVal,d    ; initialize maxValue to 0 
         stx     maxCom,d    ; initialize maxCombo to 0   
         stx     Row,d 
LoadArr: cpx     N,d         ;loop to load all values and weights to an array
         brge    Loaded
         aslx                ; weight/length offset
         aslx                ; byte offset of index and weight
         deci    Arr,x       ; take the Nth items's weight and store it
         addx    offset,d       ; length offset
         deci    Arr,x       ; take the Nth item's length and store it
         subx    offset,d       ; undo length offset
         asrx                ; undo weight/length offset
         asrx
         addx    1,i         ; j++ 
         br      LoadArr
Loaded:  ldx     0,i
         stx     j,d
Output:  stro    Masci,d     ; Output initial data
         deco    M,d
         charo   '\n',i
         stro    Nasci,d
         deco    N,d
         charo   '\n',i
OutLoop: cpx     N,d         ; loop to output initial data
         brge    sqrN        ;~~~~~~~~~~INCOMPLETE loops to end program so far~~~~~~~~~~~
         aslx
         aslx
         deco    j,d
         stro    Colon,d
         deco    Arr,x
         charo   ' ',i
         addx    offset,d
         deco    Arr,x
         charo   '\n',i
         subx    offset,d
         asrx
         asrx
         addx    1,i         ; j++
         stx     j,d
         br      OutLoop
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; get 2 ^ n and save it in squareN

sqrN:    lda     1,i         ; 2^0 = 1
         sta     squareN,d
         lda     0,i
         sta     j,d         ; j = 0
sqrLoop: lda     j,d
         cpa     N,d         ; if j > N
         brge    Soltn 
         lda     squareN,d   ; this will repeat N number of times
         asla    
         sta     squareN,d 
         lda     j,d
         adda    1,i
         sta     j,d
         br      sqrLoop
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Tests each possible combination (2^n total) of 
                             ; objects and tests their combined length and 
                             ; weight
Soltn:   lda     0,i
         sta     j,d         ; set j to 0
         sta     k,d         ; set k to 0
         br      jLoop
jPlus:   lda     j,d
         adda    1,i         ; j++
         sta     j,d
         lda     0,i
         sta     tempVal,d   ; reset the temp value
jLoop:   lda     j,d
         sta     tempJ,d
         cpa     squareN,d   ; while j < squareN
         brge    done        
         lda     0,i
         sta     tempLen,d   ; reset the temp length
         sta     k,d         ; reset k to 0
kLoop:   lda     k,d
         cpa     N,d         ; while k < N
         brge    testLen
         lda     tempJ,d     ; tempJ will be arithemtic shifted to count N amount of bits
         asra
         brc     addVals      ; if there is a 1 in the kth digit place, branch to add its length
         sta     tempJ,d
         lda     k,d
         adda    1,i         ; k++
         sta     k,d
         br      kLoop     
addVals: sta     tempJ,d
         ldx     k,d         ; accessing kth item's values
         aslx
         aslx
         lda     tempVal,d 
         adda    Arr,x       ; adding item value to combination weight, so far  
         sta     tempVal,d   
         addx    offset,d       ; byte offset of length value
         lda     tempLen,d
         adda    Arr,x       ; take Nth items length and add it to temp length
         sta     tempLen,d
         lda     k,d
         adda    1,i         ; k++
         sta     k,d
         br      kLoop
testLen: lda     tempLen,d   ; Testing length against M and weight agains the 
; current running maxVal
         cpa     M,d
         brgt    jPlus
         lda     tempVal,d
         cpa     maxVal,d
         brlt    jPlus
         sta     maxVal,d
         lda     j,d
         sta     maxCom,d
         br      jPlus
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Outputs Max weight and max length
done:    stro    wght,d
         deco    maxVal,d
         charo   ' ',i
         stro    subs,d
         lda     0,i
         sta     j,d
         lda     maxCom,d
         sta     tempCom,d
sLoop:   lda     j,d         ; loop to output subset. Utilizes bitwise values of maxCom
         cpa     N,d
         brge    end         
         lda     tempCom,d
         asra    
         brc     outIndx
         sta     tempCom,d
         lda     j,d
         adda    1,i
         sta     j,d
         br      sLoop
outIndx: sta     tempCom,d 
         deco    j,d
         charo   ' ',i
         lda     j,d
         adda    1,i
         sta     j,d
         br      sLoop   
end:     stop
.end