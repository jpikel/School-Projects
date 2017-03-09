TITLE Low Level I/O Procedures     (lowlevelio.asm)

;----------------------------------------
; Author: Johannes Pikel
; Email: pikelj@oregonstate.edu
; ONID: pikelj
; Course / Project ID   CS271 - 400              Date: 2016.07.26
; Assignment: Program #5
; Due Date: 2016.08.07
; Description: This program will ask the user to enter 10 valid integers.
; It will accept a string that a user enters
; it will check if it is a valid 32 bit integer using macros, if it is valid
; it records it and moves on.  Once the user has entered 10 valid integers it will
; display all of them, their sum and their average.
; I wanted to make sure that I kept track of the registers I used in each PROC
; so I explicitly pushed and pop each register, instead of simply pushing all the registers
; I felt this made me pay more attention to what I was using and what I doing.
;----------------------------------------

INCLUDE Irvine32.inc

;----------------------------------------
;MACRO displayString
;macro that will write a string
;takes the buffer passed loads it into edx and then calls WriteString
; Buffer should already be an OFFSET of a string
;----------------------------------------

displayString		MACRO		buffer
	push	edx						;store edx
	mov		edx, buffer
	call	WriteString
	pop		edx						;restore edx
ENDM

;----------------------------------------
;MACRO getString
; shows a prompt to the user
; reads in a string for the max number of characters passed - 1
; stores the read in string in the buffer passed, should be an OFFSET 
;----------------------------------------

getString			MACRO		buffer, max, prompt
	push	edx
	push	ecx
	mov		edx, prompt
	call	WriteString
	mov		edx, buffer
	mov		ecx, max
	call	ReadString
	pop		ecx
	pop		edx
ENDM



;Constants, NUMLIMIT is the total numbers the user will enter
;NUMCHAR is the total number of characters read from input
;MAX AND MIN ASCII are the digits 0 - 9 range
	NUMLIMIT		EQU			<10>
	NUMCHAR			EQU			<15>
	MAXASCII		EQU			<57>
	MINASCII		EQU			<48>



.data
; string for the program's title/introduce, programmers name, hello message and good-bye message
	progTitle		BYTE		"Programming Assignment 6: Designing low-level I/O procedures",0ah,0dh,0
	progAuth		BYTE		"Johannes Pikel",0ah,0ah,0dh,0
	progWrit		BYTE		"Written by: ",0
	
	progIntro		BYTE		"Please provide 10 signed decimal integers.", 0ah, 0dh
					BYTE		"Each number needs to be small enough to fit inside a 32 bit registers.", 0ah,0adh
					BYTE		"After you have finished inputting the raw numbers I will display a list", 0ah,0dh
					BYTE		"of the integers, their sum, and their average value.",0ah,0ah,0dh,0
	
	goodBye			BYTE		0ah, 0dh, "Thanks for playing from ",0

; string for user input message, error message, some title strings for the display PROC
	errMsg			BYTE		"ERROR: You did not enter a signed number or your number was too big or too small." ,0ah,0dh,0

	inptMsg			BYTE		"Please enter a signed number: ",0

	numTitle		BYTE		"You entered the following numbers: ",0
	sumTitle		BYTE		"The sum of these numbers is: ",0
	subTotal		BYTE		"Sub-total: ",0
	aveTitle		BYTE		"The average is: ",0

;string for the use when numbering input, and then a comma for printing the array
	lineNum			BYTE		". ",0
	comma			BYTE		", ",0

; array to store valid numbers
	numArr			SDWORD		NUMLIMIT	DUP(0)

; stores the sum of our integers entered
	sumInts			SDWORD		0

;extra credit strings
	extra_1			BYTE		"**EC: Number each line of input and display a running subtotal**",0ah,0dh,0
	extra_2			BYTE		"**EC: Handle signed integers (ints and sum range: 2,147,483,647 to -2,147,483,648)",0ah,0dh
					BYTE		"**Going outside this range may cause unexpected results.",0ah,0dh
					BYTE		"**You may enter raw numbers '42', negative numbers '-42' or plus signed numbers '+42'.**",0

.code
main PROC
	push	OFFSET	extra_2			;+24
	push	OFFSET	extra_1			;+20
	push	OFFSET	progTitle		;+16
	push	OFFSET	progWrit		;+12
	push	OFFSET	progAuth		;+8
	push	OFFSET	progIntro		;+4
	call	IntroduceProg


	push	ecx						;store ecx and edi to pop after loop is complete
	push	edi
	mov		ecx, 1	
	mov		edi, OFFSET numArr
					
nextNum:							;the start of our loop to get the 10 numbers
	push	ecx						;+44
	push	OFFSET	subTotal		;+40
	push	OFFSET	sumInts			;+36
	push	OFFSET	lineNum			;+32
	push	NUMLIMIT				;+28
	push	edi						;+24
	push	OFFSET	errMsg			;+20
	push	MAXASCII				;+16
	push	MINASCII				;+12
	push	NUMCHAR					;+8
	push	OFFSET	inptMsg			;+4		;getUserInput is also where the loop for 10 entries is implemented
	call	getUserInput			;+0		;readVal and WriteVal is called from within this PROC

	add		edi, SIZEOF DWORD		;move to the next location in the array
	inc		ecx 
	cmp		ecx, NUMLIMIT
	jle		nextNum

	pop		edi
	pop		ecx

	push	OFFSET	sumInts			;+32
	push	OFFSET	comma			;+28
	push	MINASCII				;+24
	push	NUMLIMIT				;+20
	push	OFFSET	numArr			;+16
	push	OFFSET	aveTitle		;+12
	push	OFFSET	sumTitle		;+8
	push	OFFSET	numTitle		;+4
	call	displayOutput			;		writeVal is called from within this PROC
		
	push	OFFSET	progAuth
	push	OFFSET	goodBye
	call	sayGoodBye



	exit	; exit to operating system

main ENDP

;----------------------------------------
IntroduceProg PROC
;description: displays on screen the program's title, author and introduction
;receives: on stack ret@, @progIntro+4, @progAuth+8, @progWrit+12, @progTitle+16, @ extra_1+20, @exrta_2+24
;returns: nothing
;preconditions: appropriate strings pushed onto stack
;registers changed: edp, edi (saved and restored)
;----------------------------------------
	push			ebp					;ebp, ret@+4, @progIntro+8, @progAuth+12, @progWrit+16, @progTitle+20
	mov				ebp, esp			;@extra_1+24, @extra_2+8
	push			edi					;edi is -4

	mov				edi, [ebp + 20]
	displayString	edi					;display program title
	
	mov				edi, [ebp + 16]
	displayString	edi					;display written by

	mov				edi, [ebp + 12]
	displayString	edi					;display program author

	mov				edi, [ebp + 8]
	displayString	edi					;display program introduction text

	mov				edi, [ebp + 24]
	displayString	edi					;display extra credit message

	mov				edi, [ebp + 28]
	displayString	edi					;display extra credit message

	call			CrLF
	call			CrLF
	pop				edi
	pop				ebp
	ret				24
IntroduceProg ENDP


;----------------------------------------
getUserInput PROC
;description: a short loop that continues for the NUMLIMIT times, with each pass moves to a new array location
; so that readVal may store an integer in that location
;receives: on stack, @inptMsg+4, NUMCHAR +8, MINASCII+12, MAXASCII+16, @errMsg+20, @numArr+24, NUMLIMIT+28, @lineNum+32
;returns:	an array filled with integers, from string input
;preconditions:na
;registers changed: eax, ecx, edi
;----------------------------------------
	push			ebp				;ebp, ret@+4, @inptMsg+8, value NUMCHAR +12, MINASCII +16, MAXASCII +20
	mov				ebp, esp		;@errMsg +24, @numArr+28, NUMLIMIT+32, @lineNum+36, @sumInts+40, @sumTitle+44
	push			eax				;num to print + 48
	push			ebx
	push			ecx
	push			edi
	push			esi

	mov				edi, [ebp + 28]		;edi is our @numArray
										
	mov				eax, [ebp + 48]		;our number to print 
	call			WriteDec			;print the line number
	mov				edx, [ebp + 36]		;print the ". "
	displayString	edx

	push			edi					;+24		@numArr
	push			[ebp + 24]			;+20		@errMsg
	push			[ebp + 20]			;+16		MAXASCII
	push			[ebp + 16]			;+12		MINASCII
	push			[ebp + 12]			;+8			NUMCHAR
	push			[ebp + 8]			;+4			@inptMsg
	call			readVal				;read 1 string into our array[] at current location

;This short section uses the WriteVal procedure to display our running subtotal										
	displayString	[ebp + 44]			;display "sub-total"
	mov				eax, [edi]			;the integer we just converted
	mov				esi, [ebp + 40]		;our running subtotal
	mov				ebx, [esi]
	add				eax, ebx			;add the two together, display them and then store back in @sumInts
	mov				[esi], eax
	push			[ebp + 16]			;ASCII 48
	push			esi					;@sumInts
	call			WriteVal			;using WriteVal we will display the our sum
	call			CrLF
;End running subtotal

	pop				esi
	pop				edi
	pop				ecx
	pop				ebx
	pop				eax
	pop				ebp
	ret				44
getUserInput ENDP


;----------------------------------------
readVal PROC
LOCAL	tempString[NUMCHAR]:BYTE
;description: pusing the getString macro asks the user to enter a string of digits
; compares the string to see if it is less than 11 digits otherwise it can't possibly be a 32 bit integer
; compares each BYTE in the string to see if it an integer, converts it to the correct integer, multiplies it
; by it's 10's place and then adds it to the current number in the array.  This starts at the end of the string 
; so we start with 1's, then 10's then 100's etc
; uses a local tempString to store the entered string, no need to store it permanently in memory after this PROC ends
;receives:  on stack ret@, @prompt+4, value NUMCHAR +8, MINASCII +16, MAXASCII +20, @errMsg+24, @numArr+28
;returns: an array filled with valid integers
;preconditions: na
;registers changed: eax, ebx, ecx, edx, esi, edi, ebp (saved and restored)
;----------------------------------------
;									;@tempString-4, ebp, ret@+4, @inptMsg+8, value NUMCHAR +12, MINASCII +16, MAXASCII +20
;									;@errMsg +24, @numArr+28

;store registers, I prefer to do this explicitly so I keep track of what I have done
	push			edi
	push			esi
	push			eax
	push			ebx
	push			ecx
	push			edx

	mov				edi, [ebp + 28]				;edi is our number array location for storage

tryAgain:	
	push			edi
												;empty the tempString
	mov				eax, 0h
	lea				edi, tempString				;@tempString
	mov				ecx, 15			
	cld			
	rep	stosb									;clean up the @tempString1 to all 0s
		
	pop				edi

	mov				ebx, [ebp + 8]				;@prompt string
	mov				eax, [ebp + 12]				;value of number of chars to read in
	lea				esi, tempString				;@tempString local string variable
	getString		esi, eax, ebx

	mov				edx, esi
	call			StrLength					;get the length of the entered string, not including the null byte
	mov				ecx, eax					;prepare our loop counter
	cmp				ecx, 11						;string has more than 11 digits so must be less than -2,147,483,648
	jg				notCorrect					;we also want to check then that our first char is '-' otherwise we are

	add				esi, ecx					;add the number of bytes counted to the esi
	dec				esi							;go to the end of the string, end of string is bytes counted - 1

	XOR				edx, edx					;prepare edx
	mov				edx, 1						;edx is places multiplier

nextByte:
	XOR				eax, eax					; prepare our registers
	XOR				ebx, ebx
	push			edx

	std											;set the direction flag
	lodsb										;load the byte of esi into al
	movzx			eax, al						;000 extend the al into eax

; check if the ASCII is in the range >= 48 <= 57
	mov				ebx, [ebp + 16]				;compare the ASCII code to < 48
	cmp				eax, ebx
	jl				notInt
	mov				ebx, [ebp + 20]				;compare the ASCII code to > 57
	cmp				eax, ebx
	jg				notInt

;This is the math function to convert the ASCII code from 48 to 57 into an int
	mov				ebx, [ebp + 16]				;ebp+20 = 48
	sub				eax, ebx					;subtract 48 from our ASCII

;mulitply by the places multiplier and then add to the current digits stored in that location in the array
	mul				edx
	mov				ebx, eax					;store eax in ebx
	mov				eax, [edi]					;get the number currently in edi
	add				eax, ebx					;add them together
	test			eax, eax
	js				notInt						;integer has gotten too big or small to fit!

	mov				[edi], eax					;store in location passed

	XOR				eax, eax					;clear out eax and ebx
	XOR				ebx, ebx
	pop				eax							;get the edx we pushed into eax
	mov				ebx, 10						;increase our places multiplier by 10
	mul				ebx		
	mov				edx, eax
	
	loop			nextByte
	jmp				finished

notInt:
	pop				edx
	dec				ecx
	cmp				ecx, 0						;the minus sign should be the very last char in our string
	jne				notCorrect					;if it is not it is an incorrect string
	mov				ebx, 45						;45 is the ASCII code for '-'
	cmp				eax, ebx					;eax contains our last char loaded
	jne				plusSign
	mov				eax, [edi]					;get our latest number
	imul			eax, -1						;multiply it by -1 to make it negative
	mov				[edi], eax					;and store it
	cmp				eax, 0						
	je				notCorrect
	jmp				finished

plusSign:
	mov				ebx, 43						;43 is ASCII for '+'
	cmp				eax, ebx
	jne				notCorrect					;if the last char is not a + sign it's not a correct number
	jmp				finished					;otherwise we'll discard the + and just be done

notCorrect:
	mov				edx, [ebp + 24]				;error message to display
	displayString	edx
	mov				edx, 0						;if we encountered an error we need to zero out our storage
	mov				[edi], edx
	jmp				tryAgain
	
finished:
	pop				edx
	pop				ecx
	pop				ebx
	pop				eax
	pop				esi
	pop				edi
	ret				24
readVal	ENDP

;----------------------------------------
displayOutput PROC
;description: displays the array with the number of integers entered
; calls the writeVal PROC to convert each int stored into a string that will then be displayed on screen
;receives: on stack @numTitle+4, @sumTitle+8, @aveTitle+12, @numArr+16, NUMLIMIT+ 20, MINASCII +24, @comma+28, @sumInts+32
;returns:	nothing
;preconditions: array filled with integers
;registers changed: eax, ecx, esi
;----------------------------------------
	push			ebp				;ebp, ret@+4, @numTitle+8, @sumTitle+12, @aveTitle+16, 
	mov				ebp,esp			;@numArr+20, NUMLIMIT +24, MINASCII +28, @comma+32, @sumInts+36

	push			eax
	push			ecx
	push			esi

	mov				eax, [ebp + 8]		;print "You entered the following..."
	displayString	eax
	call			CrLF
	
	mov				esi, [ebp + 20]		;esi contains our address to the array
	mov				ecx, [ebp + 24]		;our outer loop counter is the number of digits we entered, i.e. 10

nextNum:
	push			[ebp + 28]			;push 48					+8
	push			esi					;push our array location	+4
	call			writeVal			;							+0

	cmp				ecx, 1
	jle				noMoreComma
	displayString	[ebp + 32]
	add				esi, SIZEOF DWORD
noMoreComma:
	loop			nextNum				;print the next number in the array

;Moving on to display the sum of the numbers
	call			CrLF
	displayString	[ebp + 12]			;display "the sum is..."
	mov				esi, [ebp + 36]		;contains our @sumInts
	push			[ebp + 28]			;MINASCII = 48
	push			esi
	call			WriteVal			;pass the int to WriteVal to display it on screen

;now we calculate and display the average
	call			CrLF
	displayString	[ebp + 16]
	XOR				edx, edx
	mov				eax, [esi]			;our sum
	mov				ebx, [ebp + 24]		;the total integers we entered
	div				ebx
	mov				[esi], eax			;since we don't need out @sumInts for sum we'll use it as 
	push			[ebp + 28]			;a temporary location to store our average
	push			esi
	call			WriteVal
	call			CrLF

	pop				esi
	pop				ecx
	pop				eax

	pop				ebp
	ret				28
displayOutput	ENDP


;----------------------------------------
writeVal	PROC
LOCAL	tempString1[NUMCHAR]:BYTE, tempString2[NUMCHAR]:BYTE
;description: converts an int to a string and displays that string
;receives: on stack @digitPassed + 4, MINASCII + 8
;returns: na
;preconditions: passed valid int that can be converted into string
;registers changed: eax, ebx, ecx, edx, ebp, edi (saved and restored)
;----------------------------------------
										;tempString2 - 8, tempString1-4, ebp, ret@+4, @digitPassed+8, ASCII 48 + 12
	push			eax
	push			ebx
	push			ecx
	push			edx
	push			edi
	push			esi

	mov				eax, 0h
	lea				edi, tempString1	;@tempString1
	mov				ecx, 15			
	cld			
	rep	stosb							;initialize the @tempString1 to all 0s

	mov				eax, 0h
	lea				edi, tempString2	;@tempString
	mov				ecx, 15			
	cld			
	rep	stosb							;initialize the @tempString2 to all 0s

	lea				edi, tempString1	;@tempString						
										
	mov				esi, [ebp + 8]		;@digitPassed
	mov				eax, [esi]			;esi contains the OFFSET of our number to convert to string
	test			eax, eax			;see if have a negative number
	jns				notNeg				;this chunk of code example taken from http://stackoverflow.com/questions/19886399/assembly-masm-dealing-with-negative-integers
	neg				eax					;take the negation of the number
	push			1					;1 is negative
	jmp				nextDigit

notNeg:
	push			0					;0 is not negative

nextDigit:								;divide the number by 10
	mov				ebx, 10				;our divisor
	XOR				edx, edx			;prepare for division
	div				ebx

	add				edx, [ebp + 12]		;add 48 to our remainder should be 0 - 9 so convert the int to ASCII

	push			eax					;store our eax
	XOR				eax, eax
	mov				eax, edx			;move our edx into eax

	cld
	stosb								;store the converted ASCII symbol in edi(tempString1) and increment

	pop				eax					;if we still have numbers left it will be greater than 0
	cmp				eax, 0
	jg				nextDigit

	pop				eax
	cmp				eax, 1
	jne				finished
	mov				eax, '-'
	stosb

finished:								;however we wrote our string in reverse
										;so now we will reverse our string to get the correct ouput!
	lea				esi, tempString1	;@tempString1 in esi, this string is in reverse!
	lea				edi, tempString2	;@tempString2 will contain the digits in correct order

	mov				edx, esi
	call			StrLength			;find out how many digits we have before we get to null byte
	mov				ecx, eax

	add				esi, eax			;go to the end of our tempString1 - 1 byte
	dec				esi
										;use a second temporary string to reverse our first string
reverse:								;in order to print the digits in the correct order
	std									;could have used a counter for the number of digits per string
	lodsb								;but I felt that was another variable to keep up with
	cld									;here then we load a byte from the end of tempString1 and store it at the front
	stosb								;of tempString2
	loop		reverse
	
	lea				edi, tempString2	;@tempString
	displayString	edi					;display the now correct reversed numbers
	
	pop				esi
	pop				edi
	pop				edx
	pop				ecx
	pop				ebx
	pop				eax
	ret				8
writeVal	ENDP


;----------------------------------------
sayGoodBye	PROC
;description: says goodbye to the user
;receives: on stack ret@, @goodBye+4, @progAuth+8
;returns: nothing
;preconditions: strings pushed to stack, goodbye message and program's author's name
;registers changed: ebp, edi (saved and restored)
;----------------------------------------
	push			ebp				;ebp, ret@+4, @goodBye+8, @progAuth+12
	mov				ebp, esp
	push			edi				;save edi

	mov				edi, [ebp + 8]
	displayString	edi

	mov				edi, [ebp + 12]
	displayString	edi

	pop				edi
	pop				ebp
	ret				8
sayGoodBye	ENDP

END main
