;
; (C) Copyright IBM Corporation 1986,1987,1988,1989,1990
;
;
;
; Assembler 'glue' allowing IBM 'C' compiled code to communicate with
; the Adapter Interface in PCDOS mode.
;
;
;
;
;
        page 80,132
;
; Call Adapter Interface, Get Adapter Interface address vector & Get Adapter
; interface linked list address
; (_CALLAFI , _GETAFI & _GETAFIL)
;
DGROUP  group   _DATA
;
_DATA   segment word public 'DATA'
        assume  ds:DGROUP,cs:_text
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Global pointer to Adapter Interface entry point address list
; declared as: extern char far *afi_ptr;
;
        public  _afi_ptr       ; Adapter Interface entry table block address
;
_afi_ptr dd    0               ;
         db    '(C) Copyright IBM Corporation 1986,1987,1988,1989'
_DATA   ends
;
_TEXT   segment byte public 'CODE'
        public  _callafi,getafi_,_getafil
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  Call Adapter Interface - CALLAFI
;
;  This subroutine is assembler glue, to allow IBM 'C' compiled code
;  to call the Adapter Interface.
;
;  declared as shown:
; extern void far callafi(int,char far *);
;
;
;
_callafi proc   far             ; far entry point
        push    bp
        mov     bp,sp
        push    di              ; save register variables
        push    si              ; save register variables
;
        mov     ax,[bp+6]       ; entry no. to ax
;
        les     bx,[bp+8]       ; parm block address
        push    es              ; onto stack
        push    bx              ;     offset too
;
        shl     ax,1            ; form offset from entry no. required
        shl     ax,1            ;
        mov     si,ax           ;
;
        les     bx,_afi_ptr    ; entry block address to es:bx
        call    dword ptr es:[bx][si]    ; call entry point
;
        pop     si              ; restore register variables
        pop     di              ; restore register variables
        pop     bp              ; restore bp
        ret                     ; return to caller
;
_callafi endp
;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  Get Adapter Interface address vector - _GETAFI
;
;  This subroutine is assembler glue, to allow IBM 'C' compiled code
;  to acquire the Adapter Interface entry point address list.
;
;  declared as shown:
; extern char far *(far getafi(void));
;
;
getafi_ proc   far              ; far entry point to fetch
        push    bp
        mov     bp,sp
        push    di              ; save register variables
        push    si              ; save register variables
;
        mov     ax,357fh        ; read interrupt vector 7f
        int     21h             ; by function call
        mov     ax,es           ; es to ax
        or      ax,bx           ; is 7f vector null
        jz      return          ;
;
        mov     ax,0105h        ; get Interface address
        int     7fh             ; by software interrupt 7f
;
        jc      return          ; Interface not OK if carry set
;
        mov     word ptr _afi_ptr,dx  ; save afi pointer offset
        mov     word ptr _afi_ptr+2,cx  ; save afi pointer segment
;
return:
;
        mov     ax,word ptr _afi_ptr  ; return ptr in dx:ax
        mov     dx,word ptr _afi_ptr+2  ; return ptr in dx:ax
;
        cld                     ; clear direction flag
        clc                     ; clear carry flag
        pop     si              ; restore register variables
        pop     di              ; restore register variables
        pop     bp              ; restore bp
        ret                     ; return to caller
;
getafi_ endp
;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  Get Adapter Interface Linked List address vector - _GETAFIL
;
;  This subroutine is assembler glue, to allow 'C' to
;  acquire the Adapter Interface entry point address list.
;
;  declared as shown:
; extern struct afi_list_entry far *(far getafil(void));
;
;
_getafil proc   far             ;far entry point to fetch
                                ;
        mov     ax,357Fh        ;read interrupt vector 7f
        int     21h             ;by function call
        mov     ax,es           ;es to ax
        or      ax,bx           ;is 7f vector null
        jz      llerror         ;IF  INT 7F vector <> NULL
        mov     ax,0113h        ;    Get Interface Linked List address into
        xor     bx,bx           ;    CX:DX by software interrupt 7f(ax=113,bx=0)
        int     7Fh             ;
                                ;AND Linked List address returned OK
        jc      llerror         ;
        mov     ax,dx           ;   Put Linked list address dx:ax
        mov     dx,cx           ;   which is this functions returned ptr value
        jmp     short llend     ;
llerror:                        ;ELSE
        xor     ax,ax           ;   Set return ptr in dx:ax
        xor     dx,dx           ;   to NULL
llend:                          ;END IF
        cld                     ; clear direction flag
        ret                     ; return to caller
;
_getafil endp
;
_TEXT   ends
        end
