; Modified by, Phillip Porter
;              Pedro De los reyes
; DATE: 3/2/17
; COMP 462: Lab 4

; SwitchFlash.s
; Runs on MSP432
;  
;
;
; Original file made by...
; Daniel Valvano
; July 1, 2015

;  This example accompanies the book
;  "Embedded Systems: Introduction to the MSP432 Microcontroller",
;  ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2015
;  Section 4.2    Program 4.1
;
;Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
;   You may use, edit, run or distribute this file
;   as long as the above copyright notice remains
;THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
;OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
;MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
;VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
;OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
;For more information about my classes, my research, and my books, see
;http://users.ece.utexas.edu/~valvano/

;Base address:0x40004C00

; Port 1
P1IN      EQU 0x40004C00  ; Port 1 Input
P1OUT     EQU 0x40004C02  ; Port 1 Output
P1DIR     EQU 0x40004C04  ; Port 1 Direction
P1REN     EQU 0x40004C06  ; Port 1 Resistor Enable
P1DS      EQU 0x40004C08  ; Port 1 Drive Strength
P1SEL0    EQU 0x40004C0A  ; Port 1 Select 0
P1SEL1    EQU 0x40004C0C  ; Port 1 Select 1
	
	
; Port 2
P2IN      EQU 0x40004C01  ; Port 2 Input
P2OUT     EQU 0x40004C03  ; Port 2 Output
P2DIR     EQU 0x40004C05  ; Port 2 Direction
P2REN     EQU 0x40004C07  ; Port 2 Resistor Enable
P2DS      EQU 0x40004C09  ; Port 2 Drive Strength
P2SEL0    EQU 0x40004C0B  ; Port 2 Select 0
P2SEL1    EQU 0x40004C0D  ; Port 2 Select 1

; Port 4
P4IN      EQU 0x40004C21  ; Port 4 Input
P4OUT     EQU 0x40004C23  ; Port 4 Output 
P4DIR	  EQU 0x40004C25  ; Port 4 Direction
P4SEL0    EQU 0x40004C2B  ; Port 4 Select 0
P4SEL1	  EQU 0x40004C2D  ; Port 4 Select 1

; Port 5
P5IN      EQU 0x40004C40  ; Port 5 Input
P5OUT     EQU 0x40004C42  ; Port 5 Output
P5DIR     EQU 0x40004C44  ; Port 5 Direction
P5SEL0    EQU 0x40004C4A  ; Port 5 Select 0
P5SEL1    EQU 0x40004C4C  ; Port 5 Select 1

; Systick memory locations, also defined in SysTick.s
SYSTICK_STCSR         EQU 0xE000E010  ; SysTick Control and Status Register
SYSTICK_STRVR         EQU 0xE000E014  ; SysTick Reload Value Register
SYSTICK_STCVR         EQU 0xE000E018  ; SysTick Current Value Register
	
DELAY_T	  EQU 62				    	; Delay in ms
		
		AREA    DATA, Align=4
			
Buff_Ind SPACE   4						;  4 bytes for a 32 bit int index for the buffer
Buff_Max SPACE	 4						;  Maximum value of index for both arrays
Time_St  SPACE   4						;  Recorded beginning of measure
Time_Ar  SPACE   200					; (4 bytes SYSTICK_STCVR) * 50 entries = 200 bytes
Port_Ar  SPACE   200					; (2 bytes port 4 + 2 bytes port 5) * 50 entries = 200 bytes
							
        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB
		PRESERVE8
		IMPORT  Clock_Init48MHz
		IMPORT  SysTick_Init
		IMPORT  SysTick_Wait
        EXPORT  Start
	
Start	
	BL 	Clock_Init48MHz				; Initialize 48MHz speed
	BL  SysTick_Init				; Initialize the SysTick timer
	BL  Dump_Init					; Initialize space for 3 sec dump and set indexes
	BL  Port1_Init					; Initialize P1.0 and make it GPIO output (internal led)
    BL  Port4_Init                  ; Initialize P4.0 and make it GPIO input  (external switch)
	BL  Port5_Init					; Initialise P5.0 and make it GPIO output (external led)    

__loop
	BL Dump_Capture

__heartbeat
	LDR  R1, =P1OUT
	LDRB R0, [R1]					; load P1 byte into R0
	EOR  R0, #0x01					; flip led 1
	STRB R0, [R1]					; store P1 byte back into P1 with P1.0 flipped
	
	; Main task
	MOV R0, #(DELAY_T)				; load into R0 number of ms to delay				
	BL  Delay                       ; delay for R0 of ms
	BL  Port4_Input					; read value of P4.0 into R0
	CMP R0, #0x01					; is P4.0 on by pos logic? IE: is switch depressed?
	BEQ __togglelight               ; Branch if P4.0 is 0
__solidlight						; else, solid led on
	MOV R0, #0x01					
	BL Port5_Output					; Set P5.0 to 1
	B __loop
	
__togglelight
	LDR  R1, =P5OUT
	LDRB R0, [R1]					; fetch current port 5 output
	EOR  R0, #0x01					; toggle bit in R5.0
	BL Port5_Output
	B __loop
	
       
;------------Dump_Init--------------
; Initialize time values, and port values in dump arrays
Dump_Init
	MOV R0, #0						; Initial index
	LDR R1, =Buff_Ind				
	STR R0, [R1]					; Store index of 0
	
	MOV R0, #50						; Size of the arrays in 4 byte words
	LDR R1, =Buff_Max		
	STR R0, [R1]					; Store buffer limit in global var
	
	SUB R1, R0, #1					; last index of arrays
	LDR R0, =__Default_Entry		; Default entry value
	LDR R2, =Time_Ar
	LDR R3, =Port_Ar
	
__Dump_Init_loop
	STR	R0, [R2, R1, LSL #2]		; Store 0xFFFF,FFFF at 4 byte word located at Time_Arr[R1]
	STR	R0, [R3, R1, LSL #2]		; Store 0xFFFF,FFFF at 4 byte word located at Port_Arr[R1]
	SUBS R1, #1						; Decrement R1
	BGE __Dump_Init_loop
	BX  LR
__Default_Entry
	DCD 0xFFFFFFFF
	
;------------Dump_Capture----------
; Stores values from SYSTICK_STCVR, P4.0 and P5.0 into dump arrays
; SYSTICK_STCVR stored in Time_Ar and 
; Preserves: R0, R1, R2, R3, R4, R5
Dump_Capture
	; Save registers R2 to R4 
	PUSH {R2, R3, R4}
	
	; Begin
	LDR R4, =Buff_Ind
	LDR R3, [R4]					; Get value of index for buffer
	
	LDR R2, =Buff_Max
	LDR R2, [R2]					; Get max value for buffer
	
	CMP R3, R2						; Compare Current index to max entry count
	BGE __Dump_Capture_exit			; actual index must be 1 less than max count
	; Else, take capture and save more registers (R0, R1, R5)
	PUSH {R0, R1, R5}
	
	; Port 4 value
	LDR  R0, =P4IN
    LDRB R0, [R0]                   ; read all 8 bits of Port 4 input
    AND  R0, #0x01              	; select input pin P4.0
    
	; Port 5 value
	LDR  R1, =P5OUT
	LDRB R1, [R1]					; read all 8 bits of Port 5 output
	AND  R1, #0x01					; select output pin P5.0
	
	CMP R3, #0						; Is this the first entry?
	BNE __Not_First
	
__First
	LDR R5, =SYSTICK_STCVR
	LDR R5, [R5]					; get systick time
	LDR R2, =Time_St
	STR R5, [R2]					; store it in Time_St :: time start
	MOV R2, #0						; starting time
	B __Store_Values
	
__Not_First
	; first time stamp
	LDR R2, =Time_St
	LDR R2, [R2]					; get first value of time stamp
	
	; current time value
	LDR  R5, =SYSTICK_STCVR
	LDR  R5, [R5]					; get current value of time
	
	SUB  R2, R2, R5					; adjust for time from start
	
__Store_Values
	; Store Port values
	LDR R5, =Port_Ar				; Get pointer to Port Array
	STRH R1, [R5, R3, LSL #2]		; Store Half word with Port 5.0 in array of words in 
									; lower order Half-word at Port_Ar[Buff_Ind]	
	
	ADD R5, #2						; Offset Pointer to Port_Ar by half_word
	STRH R0, [R5, R3, LSL #2]		; Store Half word with Port 4.0 in array of words in 
									; higher order Half-word at (Port_Ar + 2)[Buff_Ind]	
									
	LDR R5, =Time_Ar				; Get pointer to Time Array
	STR R2, [R5, R3, LSL #2]		; Store time value in time array at Time_Ar[Buff_Ind]
	
	ADD R3, #0x01					; Increment buffer index
	STR R3, [R4]					; Store buffer index back
	
	; Restore additional registers (R5, R1, R0)
	POP {R5, R1, R0}
	
__Dump_Capture_exit
	; Restore original registers (R4, R3, R2)
	POP {R4, R3, R2}
	BX LR	
;------------Port1_Init------------
; Initialize GPIO Port 1 for internal led control
; P1.0 will be used in positive logic built in led
; Input: none
; Output: none
; Modifies: R0, R1
Port1_Init
	LDR  R1, =P1SEL0
	LDRB R0, [R1]
    BIC  R0, #0x01                  ; configure P1.0 as GPIO
    STRB R0, [R1]
    LDR  R1, =P1SEL1
	LDRB R0, [R1]
    BIC  R0, #0x01                  ; configure P1.0 as GPIO
    STRB R0, [R1]
	LDR  R1, =P1DIR
	LDRB R0, [R1]					
	ORR  R0, #0x01					; configure P1.0 as output
	STRB R0, [R1]
	LDR  R1, =P1IN
	LDRB R0, [R1]
	BIC  R0, #0x01					; initialize P1.0 to 0, led off
	STRB R0, [R1]
	BX   LR

;------------Port4_Init------------
; Initialize GPIO Port 4 for positive logic external switches on 
; P4.0 will be used in positive logic with external switch
; Input: none
; Output: none
; Modifies: R0, R1
Port4_Init
	LDR  R1, =P4SEL0
	LDRB R0, [R1]
    BIC  R0, #0x01                  ; configure P4.0 as GPIO
    STRB R0, [R1]
    LDR  R1, =P4SEL1
	LDRB R0, [R1]
    BIC  R0, #0x01                  ; configure P4.0 as GPIO
    STRB R0, [R1]
	LDR  R1, =P4DIR
	LDRB R0, [R1]					
	BIC  R0, #0x01					; configure P4.0 as input
	STRB R0, [R1]
	LDR  R1, =P1IN
	LDRB R0, [R1]
	BIC  R0, #0x01					; initialize P4.0 to 0, pos logic
	STRB R0, [R1]
	BX   LR

;------------Port5_Init------------
; Initialize GPIO Port 5 for positive logic external LED out 
; P5.0 will be used in positive logic external LED out
; Input: none
; Output: none
; Modifies: R0, R1
Port5_Init
	LDR  R1, =P5SEL0
	LDRB R0, [R1]
    BIC  R0, #0x01                  ; configure P5.0 as GPIO
    STRB R0, [R1]
    LDR  R1, =P5SEL1
	LDRB R0, [R1]
    BIC  R0, #0x01                  ; configure P5.0 as GPIO
    STRB R0, [R1]
	LDR  R1, =P5DIR
	LDRB R0, [R1]					
	ORR  R0, #0x01					; configure P5.0 as output
	STRB R0, [R1]
	LDR  R1, =P5OUT
	LDRB R0, [R1]
	ORR  R0, #0x01					; initialize P5.0 to 1, pos logic
	STRB R0, [R1]
	BX   LR
	
;------------Port4_Input------------
; Read and return the status of the switch.
; Input: none
; Output: R0  0x01 if external switch is pressed 
;         R0  0x00 if external switch is not pressed 
;         
; Modifies: R1
Port4_Input
    LDR  R1, =P4IN
    LDRB R0, [R1]                   ; read all 8 bits of Port 4
    AND  R0, #0x01              	; select input pin P4.0
    BX   LR
	
;------------Port5_Output------------
; Set the output state of P5.0
; Input: R0  new state of P5.0 (only 1 significant bit)
; Output: none
; Modifies: R1, R2
Port5_Output
    LDR  R1, =P5OUT
	LDRB R2, [R1]					; load current port 5 output
	BIC  R2, #0x01					; clear value in current port 5.0 output
	AND  R0, #0x01              	; mask away all but P5.0 for next output
	ORR  R2, R0                 	; update port 1 output values
    STRB R2, [R1]                   ; write to P1.0 output
    BX   LR
	
;------------Delay------------------
;Delay the system for time given by R0 in ms, assuming speed 48Mhz
;Input: R0
;Output: None
;Modifies R0, R1
Delay
	MOV  R1, #4000					; Clock cycles per ms / 3(approx delay cycles)
	MUL  R0, R1						; Delay cycles per ms * R0 (62)
__Delay__loop
	SUBS R0, R0, #0x01
	BGE __Delay__loop
	BX  LR

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
