.286
.no87

; IMF player assembly routines by Lameguy64
;
; Feel free to use this in your programs. It should be fairly easy to integrate
; with little to no modifications.
;
; Please note that this was written for wasm.

IMFPROGS segment byte public 'CODE' use16
    assume cs:IMFPROGS

	; Timer interrupt stuff
	
    OldInt		dd  ?
    TimerCnt	dw  ?
    TimerSteps	dw  ?

    ; IMF stuff
	
    imf_seg		dw ?	; Base segment
    imf_off		dw ?	; Base offset
	imf_len		dw ?	; Length
    
	imf_pos		dw ?	; IMF position
	imf_left	dw ?	; IMF data remaining
    imf_cnt		dd ?	; IMF timing counters
	adl_cnt		dd ?	; Adlib timing counters
	
	; IMF playback control
	
	imf_playing	db ?	; Playback status
	imf_looping	db ?	; Looping status
	imf_loop dw ?		; Loop count
	
	; InstallPlayer_
	;
	;	Install player service to timer interrupt.
	;
	; Input:
	;	ax	- Interrupt rate in Hz.
	;
	; C declaration:
	;	extern void InstallPlayer(int rate);
	;
	public InstallPlayer_
	InstallPlayer_ proc
		push	es
		push	ax
		push	bx
		
		mov		bx,ax
		
        mov		ax,0
		mov		imf_seg,ax
		mov		imf_off,ax
		mov		imf_len,ax
		mov		imf_pos,ax
		mov		imf_left,ax
		mov		0[word ptr imf_cnt],ax
		mov		2[word ptr imf_cnt],ax
		mov		0[word ptr adl_cnt],ax
		mov		2[word ptr adl_cnt],ax
		mov		imf_playing,al
		mov		imf_looping,al
		mov		imf_loop,ax
		
        cli								; Install hook
        xor		ax,ax
        mov		es,ax
        mov		ax,es:[8*4]
        mov		word ptr OldInt,ax
        mov		ax,es:2[8*4]
        mov		word ptr OldInt+2,ax
        mov		word ptr es:[8*4],offset IntHandler
        mov		es:2[8*4],cs			; Set timer
        mov		ax,bx
        call	SetTimer0
        sti
        
		pop		bx
		pop		ax
        pop		es
        ret
    InstallPlayer_ endp

	; RemovePlayer_
	;
	;	Removes player service from timer interrupt.
	;
	; C declaration:
	;	extern void RemovePlayer();
	;
	public RemovePlayer_
	RemovePlayer_ proc
        push	ax
        push	es
        cli
        xor		ax,ax
        mov		es,ax
        mov		ax,word ptr OldInt
        mov		es:[8*4],ax
        mov		ax,word ptr OldInt+2
        mov		es:2[8*4],ax
        call	ResetTimer0
        sti
        pop		es
        pop		ax
        ret
    RemovePlayer_ endp

	; PlayIMF_
	;
	;	Play an IMF file loaded in memory.
	;
	;	Automatically detects for IMF Type 2 files and adjusts
	;	data size accordingly.
	;
	; Input:
	;	dx	- Segment to IMF data.
	;	ax	- Offset to IMF data.
	;	bx	- Length of IMF data.
	;
	; C declaration:
	;	extern void PlayIMF(void* imfdata, int imflen);
	;
	public PlayIMF_
	PlayIMF_ proc
		
		push	ax
		push	bx
		push	cx
		push	dx
		
		mov		imf_seg,dx				; IMF segment
        
		push	ds
		push	di
		mov		ds,dx
		mov		di,ax
		mov		cx,[ds:di]
		pop		di
		pop		ds
		
		cmp		cx,0					; If 1st two bytes are zero,
		je		imf1					; its a IMF1 file!
		
		add		ax,2					; Skip header in IMF2 files
		
		mov		bx,cx					; Use first two bytes as data length
		
	imf1:
		
		add		ax,4
		sub		bx,4
	
		mov		imf_off,ax				; IMF base offset
		mov		imf_pos,ax				; IMF current position
        
		mov		imf_len,bx				; IMF data length
		mov		imf_left,bx
		mov		imf_loop,0
		
        mov		0[word ptr imf_cnt],0	; Clear the counters
		mov		2[word ptr imf_cnt],0
		mov		0[word ptr adl_cnt],0
		mov		2[word ptr imf_cnt],0
		
		mov		ax,02001h				; Enable waveform control
		call	adlOut					; (or IMF won't sound right)
		mov		ax,00008h				; Set to speech synth mode (?)
		call	adlOut
		
		mov		imf_playing,1			; Set playing condition
		
		pop		dx
		pop		cx
		pop		bx
		pop		ax
		ret
		
	PlayIMF_ endp
	
	; StopIMF_
	;
	;	Stop the playing IMF file. Also clears all OPL2 registers.
	;
	; C declaration;
	;	extern void StopIMF()
	;
	public StopIMF_
	StopIMF_ proc
		push	ax
		mov		imf_playing,0			; Stop playing
		mov		dx,388h
		mov		ax,0b0h
	offloop:							; Clear OPL2 registers
		out		dx,al
		inc		ax
		push	ax
		rept	6						; Small delay before writing data
		in		al,dx
		endm
		mov		ax,0
		inc		dx
		out		dx,al
		dec		dx
		pop		ax
		cmp		ax,0b8h
		jle		offloop
		pop		ax
	StopIMF_ endp
	
	; GetPosIMF_
	;
	;	Get current IMF position
	;
	; C declaration:
	;	extern int GetPosIMF();
	;
	public GetPosIMF_
	GetPosIMF_ proc
		mov		ax,imf_pos
		ret
	GetPosIMF_ endp
	
	
	public GetLoopIMF_
	GetLoopIMF_ proc
		mov		ax,imf_loop
		ret
	GetLoopIMF_ endp
	
	
	; adlOut
	;
	;	Write to the OPL2 registers.
	;
	; Input:
	;	al - Register to set.
	;	ah - Value to set.
	;
	adlOut proc
		push	ax
		push	cx
		push	dx
		
		mov		dx,0388h
		out		dx,al
		rept	6
		in		al,dx
		endm
		xchg	al,ah
		inc		dx
		out		dx,al
		dec		dx
		
		mov		cx,35
	adl_delay:
		in		al,dx
		dec		cx
		jnz		adl_delay
		
		pop		dx
		pop		cx
		pop		ax
		ret
	adlOut endp
	
	; SetTimer0
	;
	;	Configures timer 0 rate in Hz (routine based from RAD).
	;
	; Input:
	;	ax	- Timer rate in Hz.
	;
	SetTimer0 proc
		push	ax
		push	bx
		push	dx
		mov		bx,ax
		mov		ax,13532		; 1193180 mod 65536 (TASM craps out)
		mov		dx,18			; 1193180/65536 (TASM can't calculate this)
		div		bx
		mov		bx,ax
		mov		al,36h
		out		43h,al
		mov		al,bl
		out		40h,al
		mov		al,bh
		out		40h,al
		mov		TimerSteps,bx	; for keeping 18.2 timer correct
		mov		TimerCnt,0		; counter
		pop		dx
		pop		bx
		pop		ax
		ret
	SetTimer0 endp

	; ResetTimer0
	;
	;	Resets timer 0 rate.
	;
	ResetTimer0 proc
		push	ax
		mov		al,36h
		out		43h,al
		xor		al,al
		out		40h,al
		out		40h,al
		pop		ax
		ret
	ResetTimer0 endp

	; IntHandler
	;
	;	Interrupt service handler (based from RAD).
	;
	IntHandler proc
		push	ax
		call	DoPlayer		; Do player routine
	lx:							; see if we have passed 18.2/s mark
		mov		ax,TimerSteps	; this no. of steps per int.
		add		TimerCnt,ax
		jnc		ly				; don't call timer interrupt
		pop		ax
		jmp		cs:OldInt		; call old interrupt handlers
	ly:							; standard exit
		mov		al,20h
		out		20h,al
		pop		ax
		iret
	IntHandler endp

	; DoPlayer
	;
	;	The actual IMF player routine.
	;
	DoPlayer proc
		
		mov		al,imf_playing
		cmp		al,0
		jne		proceed
		ret
		
	proceed:
		
		push	ds
		push	di
		push	cx
		push	dx
		
		mov		ds,imf_seg				; Get IMF segment
		
	sqloop:
		
		mov		ax,[WORD PTR imf_cnt+2]	; Based on Wolf3D's IMF routine
		cmp		ax,[WORD PTR adl_cnt+2]
		ja		sqdone
		mov		ax,[WORD PTR imf_cnt]
		cmp		ax,[WORD PTR adl_cnt]
		ja		sqdone
		
		mov		di,imf_pos				; Get IMF offset
		
		mov		ax,2[ds:di]				; Get time to next event
		add		ax,[word ptr adl_cnt]	; Based on Wolf3D's IMF routine
		mov		[word ptr imf_cnt],ax
		mov		ax,[WORD PTR adl_cnt+2]
		adc		ax,0
		mov		[WORD PTR imf_cnt+2],ax
		
		mov		ax,0[ds:di]
		call	adlOut
		
		add		imf_pos,4				; Move to next instruction
		
		sub		imf_left,4				; Continue if there's still IMF data
		jnz		sqloop
	
		; Restart playback
		
		mov		ax,imf_len
		mov		imf_left,ax
		mov		ax,imf_off
		mov		imf_pos,ax
		
		add		imf_loop,1				; Increment loop count
		
		mov		0[word ptr imf_cnt],0
		mov		2[word ptr imf_cnt],0
		mov		0[word ptr adl_cnt],0
		mov		2[word ptr imf_cnt],0
		
	sqdone:
	
		add		[WORD PTR adl_cnt],1	; Based on Wolf3D's IMF routine
		adc		[WORD PTR adl_cnt+2],0
		
		pop		dx
		pop		cx
		pop		di
		pop		ds
		ret
		
	DoPlayer endp
	
IMFPROGS ends
end
