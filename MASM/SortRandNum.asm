TITLE SortingRandomNumbers     (sortRandNum.asm)

;----------------------------------------
; Author: Johannes Pikel
; Email: pikelj@oregonstate.edu
; ONID: pikelj
; Course / Project ID   CS271 - 400              Date: 2016.07.19
; Assignment: Program #4
; Due Date: 2016.08.01
; Description: This program generates random numbers in the range [100...999],
; displays the orginal list, sorts the list, calculates the value and then displays
; the list again in descending order.
; This function passes parameters to functions on the stack instead of using
; global variables.
;
; Extra Credits: will be displaying the numbers ordered by column
; and using a heap sort algorithm also, you may choose to use a bubble sort if you so desire
; see the .main below and uncomment the appropriate lines, described below
; Numbers are written to file first and then read into the array.
;
; ***Please note:   median is the middle element when the total elements is odd,
;					when the total elements is even, then the median is the average of the two elements
;					that are either side of the middle of the list
;----------------------------------------

INCLUDE Irvine32.inc

;a few constants that will be used.  LOWLIMIT and UPPERLIMIT is the range of numbers the user
; is allowed to enter.  LOWRANGE and HIRANGE is the range of the randomly generated numbers
	LOWLIMIT	EQU		<10>
	UPPERLIMIT	EQU		<200>
	LOWRANGE	EQU		<100>
	HIRANGE		EQU		<900>


.data
; a few strings that give general information about the program, title, author, etc
	authName	BYTE	"Johannes Pikel", 0
	

	authMess	BYTE	"Programmed by: ", 0

	progName	BYTE	"Program: Sorting Random Integers", 0ah, 0dh, 0

;string gives some introduction to what the program will do
	intro_1		BYTE	"This program generates random numbers in the range [100...999].", 0ah, 0dh
				BYTE	"It displays the list, sorts the list, calculates the median and display the", 0ah, 0dh
				BYTE	"sorted list in descending order.", 0ah, 0dh, 0

;prompt for user to enter numbers
	prompt_1	BYTE	"How many numbers should be generated? [10...200]: ",0

;warning message for invalid entry
	warning_1	BYTE	"Invalid entry, range is [10...200]",0

; a warning used when the number is too high
	tooHigh		BYTE	"Out of range, number greater than 200. Try again.", 0

; warning used when the number is too low
	tooLow		BYTE	"Out of range, number less than 10. Try again.", 0


; storage location for the number entered
	userNum		DWORD	?

; storage location to confirm the number is valid
	validNum	DWORD	0


; the Array! This will store up to 200 elements of DWORD
	myArray		DWORD	UPPERLIMIT DUP(0)

; titles! for the different displays
	title_1		BYTE	"The unsorted random numbers:",0
	title_2		BYTE	"The sorted random numbers:",0
	title_3		BYTE	"The median is:", 0

; used to store current x and y coordinates
	xCoord		BYTE	0
	yCoord		BYTE	0

;extraCredit messages
	extCred		BYTE	"**EC: Using Heap Sort algorithim **",0
	extCred_1	BYTE	"**EC: Display the numbers ordered by column instead of row.**",0
	extCred_2	BYTE	"**EC: Numbers are written to a file first and then read in from that file to the array.**",0

; say goodbye string
	byeMess		BYTE	"Good-bye from ",0

;fileName and fileHandle for use in writing to a file and reading from a file
	fileName	BYTE	"numbers.txt",0
	fileHandle	DWORD	?


.code
main PROC

	push	OFFSET	extCred_2	;+24
	push	OFFSET	extCred_1	;+20
	push	OFFSET	extCred		;+16
	push	OFFSET	authName	;+12
	push	OFFSET	authMess	;+8
	push	OFFSET progName		;+4
	call	IntroduceProg		;+0			;prepare and introduce the program


	push	OFFSET	intro_1		;+4
	call	displayInstructions	;+0

	push	OFFSET	tooLow		;+24
	push	OFFSET	tooHigh		;+20
	push	OFFSET	prompt_1	;+16
	push	OFFSET	warning_1	;+12
	push	OFFSET	validNum	;+8
	push	OFFSET	userNum		;+4
	call	getUserInput		;+0

	call	Randomize			;seed the random number generator

;uncomment this section to fill the array using the standard fillArray PROC
;but the sections below that call writeNumToFile and readNumIn PROC need to be commented 
;	push	HIRANGE				;+16
;	push	LOWRANGE			;+12
;	push	OFFSET	myArray		;+8
;	push	userNum				;+4
;	call	fillArray			;fill the array with random numbers


;if the above fillArray is used comment from here
	push	HIRANGE				;+20
	push	LOWRANGE			;+16
	push	userNum				;+12
	push	OFFSET fileName		;+8
	push	OFFSET fileHandle	;+4
	call	writeNumToFile

	push	OFFSET myArray		;+16
	push	userNum				;+12
	push	OFFSET fileName		;+8
	push	OFFSET fileHandle	;+4
	call	readNumIn
;to here... commented section, if fillArray is used

	call	WaitMsg
	call	Clrscr

	push	OFFSET	xCoord
	push	OFFSET	yCoord
	push	OFFSET	myArray
	push	OFFSET	title_1
	push	userNum
	call	displayList			;ret@; +4 userNum;+ 8 @title_1; +12 @myArray; +16 @yCoord; +20 @xCoord

	push	userNum
	push	OFFSET	myArray
;	call	bubbleSort		;you may also use bubble sort if so desired, make sure to comment out heap sort
	call	heapSort		;sort the list using Heap Sort!!!

	push	OFFSET	xCoord
	push	OFFSET	yCoord
	push	OFFSET	myArray
	push	OFFSET	title_3
	push	userNum
	call	showMedian			;show the median

	push	OFFSET	xCoord
	push	OFFSET	yCoord
	push	OFFSET	myArray
	push	OFFSET	title_2
	push	userNum
	call	displayList			;display the now ordered list

	push	OFFSET	authName
	push	OFFSET	byeMess	
	call	sayGoodBye


	exit	; exit to operating system
main ENDP

;----------------------------------------
IntroduceProg	PROC
; introduce the programmer and program name
; Displays on screen information about the program
; receives:	on stack, @progName + 4, @authMess + 8, @authName + 12
; returns: nothing
; preconditions: OFFSETS pushed to stack as above
; registers changed: edx, edi
;----------------------------------------
	push	ebp						;ebp, ret@ +4, @progName+8, @authMess+12, @authName+16, @extCred +20
	mov		ebp, esp																	  ; @extCred_1 +24
																						  ;@extCred_2 + 28
	mov		edi, [ebp + 8]			;print to screen the program title
	mov		edx, edi
	call	WriteString

	mov		edi, [ebp + 12]			;print to screen the "Programmed by:"
	mov		edx, edi
	call	WriteString

	mov		edi, [ebp + 16]			;print to screen the author name
	mov		edx, edi
	call	WriteString
	call	CrLF
	call	CrLF

; print the extra credit statements
	mov		edi, [ebp + 20]
	mov		edx, edi
	call	WriteString
	call	CrLF

	mov		edi, [ebp + 24]
	mov		edx, edi
	call	WriteString
	call	CrLF

	mov		edi, [ebp + 28]
	mov		edx, edi
	call	WriteString
	call	CrLF

	call	CrLF
	pop		ebp
	ret		24						;clean up @progName, @authMess, @authname, and extra credits

IntroduceProg ENDP


;----------------------------------------
displayInstructions PROC
; display program instructions to the user
; receives: on stack @intro_1	+4
; returns: nothing
; preconditions: stack arranged properly
; registers changed: edx, edi
;----------------------------------------
	push	ebp			;ebp, ret@ +4, @intro_1 +8
	mov		ebp, esp

	mov		edi, [ebp + 8]

	mov		edx, edi
	call	WriteString
	call	CrLF
	
	pop		ebp
	ret		4
displayInstructions ENDP

;----------------------------------------
getUserInput PROC
; get a number from the user
; validate that the number is in fact a number and in 
; the range from 1 to 400
; receives: @validNum + 12, @userNum on stack ebp+8
; returns: number stored in memory location firstNum
; preconditions: global variable wanring_1 and prompt_1 contain strings
; registers changed: edx, eax, esi
;----------------------------------------

	push	ebp			;ebp, ret@ +4, @userNum address now at +8 on stack, @validNum +12, @warning_1 +16, @prompt_1 +20, @tooHigh+24, @tooLow+28
	mov		ebp,esp

getInput:
	mov		esi, [ebp + 20]
	mov		edx, esi
	call	WriteString
	call	ReadInt
	jno		goodNum
	mov		esi, [ebp +16]
	mov		edx, esi
	call	WriteString
	call	CrLF
	jmp		getInput

goodNum:
	mov		esi, [ebp + 8]		;go ahead and store the number in @userNum
	mov		[esi], eax
	call	validate			;validate will check the number from the stack and store the 0 or 1 in @validNum
	mov		esi, [ebp + 12]
	mov		ebx, [esi]			;moving @validNum into ebx to check if number is in range
	cmp		ebx, 0
	je		storeNum
	jmp		getInput

storeNum:

	pop		ebp

	ret		24					; clean up the @ usernum, @validNum on the stack, @prompt_1, and @warning_1, @tooHigh, @tooLow

getUserInput ENDP


;----------------------------------------
validate PROC
; make sure the number is not above the limit
; make sure the number is not below the low limit
; receives: on stack:
;			ebp + 4 ->from getUserInput ebp
;			ret@ + 12-> return address to next command in main from getUserInput
;			@userNum +16, @validNum +20, @warning_1 +24, @prompt_1 +28, @tooHigh+32, @tooLow+36
; returns: @validNum changed to 0 if valid and 1 if not valid
; preconditions: stack arranged as above
; registers changed: edx, ebx, esi, edi
;----------------------------------------
						
	push	ebp					;ebp, ret@ + 4, ebp+8, ret@ +12, @userNum +16, @validNum +20, @warning_1 +24, @prompt_1 +28, @tooHigh+32, @tooLow+36
	mov		ebp, esp

	mov		esi, [ebp + 16]
	mov		eax, [esi]	

	cmp		eax, UPPERLIMIT		;compare the number to the UPPERLIMIT global constant
	jle		notTooHigh
	mov		edi, [ebp + 32]		;@tooHigh message at +32 on stack
	mov		edx, edi
	call	WriteString
	call	CrLF
	jmp		invalidNum

notTooHigh:

	cmp		eax, LOWLIMIT		;compare the number to the LOWLIMIT global constant
	jge		validEntry
	mov		edi, [ebp + 36]		;@tooLow message at +36 on stack
	mov		edx, edi
	call	WriteString
	call	CrLF

invalidNum:
	mov		eax, 1				;1 is number is not in range !
	jmp		endValidate

validEntry:
	mov		eax, 0				;0 is number is in range

endValidate:
	add		esi, 4
	mov		esi, [ebp+20]	
	mov		[esi], eax			;we'll store our 0 or 1 in @validNum, so our input PROC may use it, when we return

	pop		ebp
	ret
validate ENDP

;----------------------------------------
fillArray PROC
;Description: fills the array for the desired number elements with random numbers
; in the range 100-999
;Receives: on stack usernum +4, @myArray +8
;Returns:	filled @myArray with random elements
;Preconditions:	@myArray can contained at least 200 elements
;Registers changed:	edi, ecx,  eax
;Reference: array fill taken from Lecture 19 slide, ArrayFill uses register indirect addressing
; from OSU CS 271-400 Summer 2016 term
;----------------------------------------
	push	ebp				;on stack: ebp, ret@+4, userNum +8, @myArray + 12, LOWRANGE +16, HIRANGE +20
	mov		ebp, esp
	mov		edi, [ebp + 12]
	mov		ecx, [ebp + 8]

addMore:
	mov		eax, [ebp + 20]			;HIRANGE is 900
	call	RandomRange				;RandomRange returns a random int from 0 to (900-1)
	add		eax, [ebp + 16]			; then we add 100 to get our range to 100 - 999

	mov		[edi], eax				;move the new number into the array
	add		edi, SIZEOF DWORD		;increment the location in the array
	loop	addMore

	pop		ebp
	ret		16
fillArray ENDP

;----------------------------------------
writeNumToFile PROC
LOCAL string1[4]:BYTE

;Description: creates and opens a file called "numbers.txt" fills that file with randomly generated numbers
; for the amount of numbers entered by the user.
; Uses the local buffer array to temporarily store the numbers generated until they are written to the file
;Receives: on stack @fileHandle+4, @fileName+8, userNum+12, LOWRANGE+16, HIRANGE+20
;using the LOCAL directive we no longer need to push ebp, because it handles that for us
;Returns: nothing.  Random numbers written into a file 
;Preconditions:na
;Registers changed:eax, ebx, ecx, edx, edi, esi
;----------------------------------------
								;@buffer - 4 ;ebp, ret@+4, @fileHandle+8, @fileName+12, userNum+16, LOWRANGE+20, HIRANGE+24
	
	mov		edx, [ebp + 12]		;move fileName into edx
	call	CreateOutputFile	;create and open the output file
	mov		esi, [ebp + 8]		;esi is @fileHandle
	mov		[esi], eax			;store the output file in the fileHandle
	
	mov		ecx, [ebp + 16]		;loop counter for number of randoms to generate

nextNum:						;fill our temporary array!
	push	ecx
	mov		eax, [ebp +24]		;move our high range in to eax
	call	RandomRange
	add		eax, [ebp + 20]

	lea		edi, string1
	add		edi, 2				;go to the end of the string

convertNextDigit:				;convert our int into a string for storage in our file
	mov		ebx, 10
	XOR		edx, edx
	div		ebx
	add		edx, 48
	push	eax
	XOR		eax, eax
	mov		eax, edx
	std
	stosb
	pop		eax
	cmp		eax, 0
	jg		convertNextDigit

	call	CrLF

	mov		eax, [esi]			;prepare our fileHandle
	lea		edx, string1		;edx contains our @string1
	mov		ecx, 3
	call	WriteToFile
	pop		ecx
	loop	nextNum

	mov		eax, [esi]
	call	CloseFile			;close the file done writting

	ret		20					;clean up the STACK!
writeNumToFile ENDP

;----------------------------------------
readNumIn	PROC
LOCAL	string2[4]:BYTE
;Description: opens a file called "numbers.txt" and fills the array passed as
; a parameter with the amount of numbers the user entered.
;Receives: on stack @fileHandle+4, @fileName+8, userNum+12, @myArray+16
;Returns: array filled with numbers from teh file 
;Preconditions:na
;Registers changed:eax, ebx, ecx, edx, edi, esi
;----------------------------------------
				;ebp, ret@+4, @fileHandle+8, @fileName+12, userNum+16 @myArray +20

	mov		esi, [ebp + 8]		;esi is fileHandle
	mov		edi, [ebp + 20]		;list[0]
	mov		edx, [ebp + 12]		;move filename to edx
	call	OpenInputFile
	mov		[esi], eax			;store filehandle

	mov		ecx, [ebp + 16]		;loop counter for number of digits entered
	mov		eax, [esi]			;eax contains filehandle

readNext:
	push	ecx
	push	eax
	lea		edx, string2		;edx contains our array offset
	mov		ecx, 3				;read only 3 bytes/digits from file since we have a range of 100 - 999
	call	ReadFromFile
;	lea		edx, string2
;	call	WriteString			;just used for testing
;	call	CrLf

	lea		esi, string2
	add		esi, 2
	mov		ecx, 3
	mov		edx, 1

convertNextByte:
	push	edx
	std
	lodsb
	movzx	eax, al
	mov		ebx, 48
	sub		eax, ebx
	mul		edx
	mov		ebx, eax
	mov		eax, [edi]
	add		eax, ebx
	mov		[edi], eax

	XOR		eax, eax
	XOR		ebx, ebx
	pop		eax
	mov		ebx, 10
	mul		ebx
	mov		edx, eax

	loop	convertNextByte

	add		edi, SIZEOF DWORD	;move to next element in number array
	pop		eax
	pop		ecx

	loop	readNext

;	mov		eax, [esi]			;eax contains fileHandle
;	call	CloseFile			;not sure if since we made it to the end of the file it close automatically?

	ret		16

readNumIn ENDP

;----------------------------------------
displayList PROC
;Description: Prints to screen all the elements contained in the array in aligned columns,
; 20 elements per column and 10 elements per line
;Receives: on stacl: userNum +4, @title_1 +8, @myArray +12, @yCoord + 20, @xCoord +24
;Returns:	prints to screen the contents of the array
;Preconditions: array is fill to the number of elements passed b
;Registers changed: eax, ebx, ecx
;----------------------------------------
	push	ebp				;ebp; +4 ret@; +8 userNum;+ 12 @title_1; +16 @myArray; +20 @yCoord; +24 @xCoord
	mov		ebp, esp
	mov		edi, [ebp + 16]
	mov		ecx, [ebp + 8]


	mov		esi, [ebp + 12]		;move the title into esi to print to screen
	mov		edx, esi
	call	WriteString

	mov		esi, [ebp + 24]		;move xCoord for the columns
	mov		dl, [esi]
	mov		esi, [ebp +20]		;move the yCoord for the rows
	mov		dh, [esi]
	inc		dh					; move to next row after the title line

	mov		ebx, 20				;number per column counter

showNext:
	call	Gotoxy
	mov		eax, [edi]
	call	WriteDec
	add		edi, 4
	add		dh, 1
	dec		ebx
	cmp		ebx, 0
	je		newLine
	jmp		noNewLine

newLine:
	
	add		dl, 10
	sub		dh, 20
	mov		ebx, 20

noNewLine:
	
	loop	showNext


	mov		dh, 0
	mov		[esi], dh
	mov		esi, [ebp + 24]
	mov		dl, 0
	mov		[esi], dl

	pop		ebp
	ret		20
displayList ENDP

;----------------------------------------
bubbleSort PROC
;Description: The bubble sort works by taking the first element, checking it against the 
; element to the right and if the first element is less than, then swaps them.  It continues
; Assuming the first element is the smallest of the set it continues checking until it is in the
; last place in the array.  Then the sort resets and begins again from the beginning, until all 
; elements are sorted in descending order.
;Receives: @list +4 and userNum +8 on the stack
;Returns: a sorted array in @list
;Preconditions: array is filled and userNum > 0
;Registers changed: ecx, edi, eax
;Reference: code taken from Assembly Language for x86 Processors, page 375, Section 9.5
;----------------------------------------

	push	ebp
	mov		ebp, esp
	mov		edi, [ebp + 8] ; @list[0]
	mov		ecx, [ebp + 12]  ; userNum value
	dec		ecx

L1:
	push	ecx
	mov		edi, [ebp + 8]
	
L2:
	mov		eax, [edi]
	cmp		[edi + 4], eax
	jl		L3
	xchg	eax, [edi + 4]
	mov		[edi], eax

L3:
	add		edi, 4
	loop	L2

	pop		ecx
	loop	L1

	pop		ebp
	ret		8
bubbleSort ENDP


;----------------------------------------
showMedian PROC
;Description: Calculates the median by dividing the total number of elements in half
; Then multiplies by 4 because each is a DWORD
;Receives: userNum by value +4 on stack
;			@title_3	+12 
;			@myArray	+16
;			@yCoord		+20
;			@xCoord		+24
; When the number of elements in the array is odd, the median is the middle element
; as in the case of 1, 3, 3, 5, 9, 11, 13 the median is 5
; When the number of elements in the array is even, there is not middle element so the two
; elements either side of the middle are summed and divided by 2.
;Returns:	nothing
;Preconditions: myArray is filled and sorted to show the median
;Registers changed: eax, ebx, edi, edx
;----------------------------------------

	push	ebp				;ebp; +4 ret@; +8 userNum;+ 12 @title_3; +16 @myArray; +20 @yCoord; +24 @xCoord
	mov		ebp, esp
	mov		edi, [ebp + 16]
	mov		eax, [ebp + 8]

							;calculate the median, which is the middle element of the array so total elements/2
							;first check if the number of elements is even or odd.
	XOR		edx, edx
	mov		ebx, 2
	div		ebx
	push	edx
	mov		ebx, SIZEOF DWORD
	mul		ebx
	add		edi, eax
	mov		eax, [edi]
	pop		edx
	cmp		edx, 0				;if edx is 0 number is even, otherwise our median is the middle element
	je		evenNum
	jmp		finished

evenNum:						;so we need to add the two numbers either side of the middle and divide by 2
	sub		edi, SIZEOF DWORD	
	mov		ebx, [edi]		
	add		eax, ebx
	mov		ebx, 2
	div		ebx

finished:
	mov		dl, 0
	mov		dh, 22
	call	Gotoxy

	mov		esi, [ebp + 12]
	mov		edx, esi
	call	WriteString

								;update our x and y coordinates. to be below a possible 20 numbers per column
	mov		esi, [ebp + 24]
	mov		dl, 0
	mov		esi, [ebp +20]
	mov		dh, 22

	add		dl, 25
	call	Gotoxy

	call	WriteDec
	call	CrLF
	call	Waitmsg

	add		dh, 3				;store the updated x and y coordinates
	mov		[esi], dh
	mov		esi, [ebp + 24]
	mov		dl, 0
	mov		[esi], dl
	call	Gotoxy

	pop		ebp
	ret		20
showMedian ENDP


;----------------------------------------
adjustHeap PROC
;Description:	Compares both children of the parent if avaiable
;swaps the parent with the smallest child and then calls adjustHeap on that child and it's subtree
;Receives: @list +4, last +8 (last element in array), position +12 (current root)
;Returns:@list from the root passed and subtree is adjust in descending order
;Preconditions:	@list is filled, position is a valid index in the array
;Registers changed: eax, ebx, edi
;----------------------------------------
	push	ebp			;ebp, ret@+4, @list+8, last+12, position+16
	mov		ebp, esp

	mov		edi, [ebp + 8]	;edi is list[0]

	mov		eax, [ebp + 16]			;in order to use our index locations we need to calculate their OFFSETs
	mov		ebx, 2
	mul		ebx
	inc		eax
	mov		ebx, SIZEOF DWORD
	mul		ebx
	push	eax						;left child OFFSET is ebp - 4

	mov		eax, [ebp + 16]
	mov		ebx, 2
	mul		ebx
	add		eax, 2
	mov		ebx, SIZEOF DWORD
	mul		ebx
	push	eax						;right child OFFSET is ebp - 8



	mov		eax, [ebp + 16]	;position index
	mov		ebx, SIZEOF DWORD
	mul		ebx
	push	eax						;position OFFSET is ebp - 12

	mov		eax, [ebp + 12]
	mul		ebx
	push	eax						;last OFFSET is ebp - 16
									;DONE calculating OFFSETS

	mov		eax, [ebp - 8]			;rightChild
	mov		ebx, [ebp - 16]			;last
	cmp		eax, ebx				;checking if there is a rightChild to the parent, totalelements > rightChild
	jge		noRightChild
	
	add		edi, [ebp - 4]
	mov		eax, [edi]				;eax leftChild value
	mov		edi, [ebp + 8]
	add		edi, [ebp - 8]
	mov		ebx, [edi]				;ebx rightChild value
	cmp		eax, ebx				;Find the child whose value is the lowest
	jg		minRight
	mov		edi, [ebp + 8]			;leftChild is the minimum
	add		edi, [ebp - 4]
	push	[ebp - 4]				;push minIndex to stack ebp - 20, aka leftChild's OFFSET
	mov		eax, [edi]				;eax contains minIndex value aka leftChild
	jmp		compareMin

minRight:							;rightChild is the minimum
	mov		edi, [ebp + 8]
	add		edi, [ebp - 8]
	push	[ebp - 8]				;push minIndex to stack ebp - 20, aka rightChild's OFFSET
	mov		eax, [edi]				;eax contains minIndex value aka rightChild

compareMin:
	mov		edi, [ebp + 8]
	add		edi, [ebp - 12]
	mov		ebx, [edi]
	cmp		eax, ebx				;comparing minIndex value to position's value
	jg		finPop					;need to pop the minIndex off the stack, if minIndex value > position value
	mov		eax, [ebp - 20]			;get the minIndex from stack ebp - 20		
	XOR		edx, edx
	mov		ebx, SIZEOF DWORD
	div		ebx
	push	eax						;eax(minIndex) is position2 for call to swapElements
	push	[ebp + 16]				;position is position1 for call to swapElements
	push	[ebp + 8]				;push list to stack for swapElements
	call	swapElements			; swapElements
	pop		eax						;finished with minIndex ebp - 20 so pop
	mov		ebx, SIZEOF DWORD
	div		ebx
	push	eax						;new position to stack
	push	[ebp + 12]				;last to stack
	push	[ebp + 8]				;list to stack
	call	adjustHeap				;now we need to call adjustHeap on the child as the new root
	jmp		finished

noRightChild:						;There's only a leftChild
	mov		eax, [ebp - 4]			;leftChild OFFSET
	mov		ebx, [ebp - 16]			;last OFFSET
	cmp		eax, ebx
	jge		finished
	mov		edi, [ebp + 8]
	add		edi, eax
	mov		eax, [edi]

	mov		edi, [ebp + 8]
	mov		ebx, [ebp - 12]			;position OFFSET
	add		edi, ebx
	mov		ebx, [edi]
	cmp		eax, ebx				;comparing leftChild value to position value
	jg		finished
	XOR		edx, edx
	mov		eax, [ebp - 4]
	mov		ebx, SIZEOF DWORD
	div		ebx
	push	eax						;push an extra leftChild's index to stack for after swapElements
	push	eax						;leftChild's index pushed to stack, position2 for swapElements
	push	[ebp + 16]				;position is position 1 for call to swapElements
	push	[ebp +8]				;push list to stack for swapElements
	call	swapElements
	pop		eax
	push	eax						;push eax as new position to stack for call to adjustHeap
	push	[ebp + 12]				;last pushed to stack
	push	[ebp + 8]
	call	adjustHeap				;again adjustHeap on the child as the new root	
	jmp		finished
finPop:
	pop		eax						;pop minIndex

finished:
	pop		eax						;last OFFSET
	pop		eax						;position OFFSET
	pop		eax						;rightChild OFFSET
	pop		eax						;leftChild OFFSET

	pop		ebp
	ret		12
adjustHeap ENDP

;----------------------------------------
buildHeap PROC
;Description:	Starting at the last non-leaf node, which can be defined as the floor of
; total elements divided by 2
; adjusts each element and it's subtree
; then move on to the next element by decrementing position
;Receives:	@list + 4, userNum + 8 (total elements)
;Returns:	@list ordered into a proper heap, but not necessarily in ascending or descending values
;Preconditions:	@list is filled, userNum > 0
;Registers changed:	eax, ebx, ecx, edx
;----------------------------------------
	push	ebp			;ebp, ret@ +4, @list+8, userNum +12
	mov		ebp, esp

	XOR		edx, edx			;find the last non-leaf node
	mov		eax, [ebp + 12]
	mov		ebx, 2
	div		ebx
	mov		ecx, eax

loopStart:
	push	ecx				;store counter for later
	dec		ecx				;position we want is counter -1, which comes from floor is meant to be total elements/2 -1
	push	ecx				;position, 
	push	[ebp + 12]		;last
	push	[ebp + 8]		;list OFFSET
	call	adjustHeap
	pop		ecx
	loop	loopStart
	
	pop		ebp
	ret		8
buildHeap ENDP

;----------------------------------------
heapSort PROC
;Description: First uses buildHeap to build a proper heap, where each parent's value
; is less than it's children's values
; Then starts at the end of the array, swaps the last and first
; Then percolates down the first element to it's correct position
; Resulting in a max tree
;Receives:	@list +4, userNum +8
;Returns:	array in descending order
;Preconditions:	@list is filled with elements, userNum is > 0
;Registers changed: eax, ecx
;----------------------------------------
	push	ebp		;ebp, ret@ +4, @list +8, userNum +12
	mov		ebp, esp

	push	[ebp + 12]		;userNum
	push	[ebp + 8]		;list OFFSET
	call	buildHeap

	mov		eax, [ebp + 12]
;	dec		eax
	mov		ecx, eax			;ecx = userNum

loopSort:
	push	ecx					;store for later
	dec		ecx					;last is size - 1
	push	ecx					;swap last	
	push	0					;with first
	push	[ebp + 8]
	call	swapElements
	
	pop		ecx					;get the current loop location again which is last
	push	ecx					;store for loop
	dec		ecx					;last is size - 1
	push	0					;push position 0 to percolate down 
	push	ecx					;to last
	push	[ebp + 8]
	call	adjustHeap

	pop		ecx					;get the loop counter which is size, so we can reach element 0
	loop	loopSort
	pop		ebp
	ret		8
heapSort ENDP


;----------------------------------------
swapElements PROC
;Description: swaps the elements contained in the array position 1 and position 2
;Receives: @list +4, position1 +8, position2 +12
;Returns: list positions swapped
;Precondtions: positions 1 and 2 are valid indices
;Registers changed: edi, eax, ebx, ecx
;----------------------------------------
	push	ebp			;ebp, ret@ +4, @list + 8, position1 + 12, position2 + 16
	mov		ebp, esp
	
	mov		edi, [ebp + 8]		;list[0]
	
	mov		eax, [ebp + 12]		;position1 index
	mov		ebx, SIZEOF DWORD	
	mul		ebx					;position1 OFFSET

	add		edi, eax
	mov		ecx, [edi]			;position1 value in ecx

	mov		eax, [ebp + 16]
	mul		ebx
	mov		edi, [ebp + 8]
	add		edi, eax			;edi is @position2
	
	xchg	[edi], ecx			;position2 swap with ecx, ecx contains position2

	mov		eax, [ebp + 12]		;position1 index
	mov		ebx, SIZEOF DWORD
	mul		ebx
	mov		edi, [ebp + 8]
	add		edi, eax
	mov		[edi], ecx			;now position1 contains ecx

	pop		ebp
	ret		12
swapElements ENDP


;----------------------------------------
sayGoodBye PROC
;Description: say good bye to the user
;Receives: good bye message and authName on stack
;Returns: displays message on screen
;Precondtions: OFFSETs pushed to stack
;Registers changed: edx
;----------------------------------------
	push	ebp			;ebp, ret@ +4, @byeMess+8, @authName +12
	mov		ebp, esp

	mov		dh, 47
	mov		dl, 0
	call	Gotoxy

	mov		edx, [ebp + 8]
	call	WriteString
	mov		edx, [ebp + 12]
	call	WriteString

	call	CrLF
	pop		ebp
	ret		8
sayGoodBye ENDP

END main
