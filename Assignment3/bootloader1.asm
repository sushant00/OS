; A boot sector that enters 32 - bit protected mode.
[org 0x7c00]
	mov bp , 0x9000 ; Set the stack.
	mov sp , bp
	call switch_to_pm ; Note that we never return from here.
	jmp $

gdt_start :

gdt_null : ; the mandatory null descriptor
	dd 0x0 ; ’dd ’ means define double word ( i.e. 4 bytes )
	dd 0x0
gdt_code : ; the code segment descriptor
	dw 0xffff ; Limit ( bits 0 -15)
	dw 0x0 ; Base ( bits 0 -15)
	db 0x0 ; Base ( bits 16 -23)
	db 10011010b ; 1st flags , type flags
	db 11001111b ; 2nd flags , Limit ( bits 16 -19)
	db 0x0 ; Base ( bits 24 -31)
gdt_data : ; the data segment descriptor
	dw 0xffff ; Limit ( bits 0 -15)
	dw 0x0 ; Base ( bits 0 -15)
	db 0x0 ; Base ( bits 16 -23)
	db 10010010b ; 1st flags , type flags
	db 11001111b ; 2nd flags , Limit ( bits 16 -19)
	db 0x0 ; Base ( bits 24 -31)

gdt_end :
gdt_descriptor :

	dw gdt_end - gdt_start - 1
	; of the true size
	dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

[ bits 16]
switch_to_pm :
	cli 
	lgdt [gdt_descriptor]

	mov eax , cr0
	or eax , 0x1 
	mov cr0 , eax
	jmp CODE_SEG : init_pm

[ bits 32]
init_pm :
	mov ax , DATA_SEG ; Now in PM , our old segments are meaningless ,
	mov ds , ax ; so we point our segment registers to the
	mov ss , ax ; data selector we defined in our GDT
	mov es , ax
	mov fs , ax
	mov gs , ax
	mov ebp , 0x90000 ; Update our stack position so it is right
	mov esp , ebp ; at the top of the free space.
	call BEGIN_PM
	jmp $

[bits 32]
VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f
; prints a null - terminated string pointed to by EDX
print_string_pm :
	pusha
	mov edx , VIDEO_MEMORY ; Set edx to the start of vid mem.
print_string_pm_loop :
	mov al , [ebx] ; Store the char at EBX in AL
	mov ah , WHITE_ON_BLACK ; Store the attributes in AH
	cmp al , 0 ; if (al == 0) , at end of string , so
	je print_string_pm_done ; jump to done
	mov [edx] , ax ; Store char and attributes at current
	; character cell.
	add ebx , 1 ; Increment EBX to the next char in string.
	add edx , 2 ; Move to next character cell in vid mem.
	jmp print_string_pm_loop ; loop around to print the next char.
print_string_pm_done :
	popa
	ret

[bits 32]
shift_l:
	mov edx, esi
shift_l_loop:
	cmp edx, 0
	je shift_l_done
	shl ebx, 1
	sub edx, 1
	jmp shift_l_loop
shift_l_done:
	jmp continue1


[bits 32]

shift_r:
	mov edx, esi
shift_r_loop:
	cmp edx, 0
	je shift_r_done
	shr ecx, 1
	sub edx, 1
	jmp shift_r_loop
shift_r_done:
	jmp continue2

[bits 32]
print_cr3:
	pusha
	mov esi, 32
	mov edi, 0xb8020
print_cr3_loop:
	add edi, 2
	sub esi, 1
	mov ecx, cr3
	mov ebx, 1
	jmp shift_l
continue1:
	and ecx, ebx
	jmp shift_r
continue2:
	mov al, cl
	add al, 0x30
	mov ah, WHITE_ON_BLACK
	mov [edi], ax
	cmp esi, 0
	je print_cr3_done
	jmp print_cr3_loop
print_cr3_done:
	popa
	ret


[ bits 32]
; This is where we arrive after switching to and initialising protected mode.

BEGIN_PM :
	mov ebx , MSG_PROT_MODE
	call print_string_pm ; Use our 32 - bit print routine.
	jmp start_lg ; start long mode.

; Global variables
MSG_PROT_MODE db "Hello World! CR3=",0	

start_lg:
	mov edi, 0x1000    ; Set the destination index to 0x1000.
    mov cr3, edi       ; Set control register 3 to the destination index.
    xor eax, eax       ; Nullify the A-register.
    mov ecx, 4096      ; Set the C-register to 4096.
    rep stosd          ; Clear the memory.
    mov edi, cr3       ; Set the destination index to control register 3.
    mov DWORD [edi], 0x2003      ; Set the uint32_t at the destination index to 0x2003.
    add edi, 0x1000              ; Add 0x1000 to the destination index.
    mov DWORD [edi], 0x3003      ; Set the uint32_t at the destination index to 0x3003.
    add edi, 0x1000              ; Add 0x1000 to the destination index.
    mov DWORD [edi], 0x4003      ; Set the uint32_t at the destination index to 0x4003.
    add edi, 0x1000              ; Add 0x1000 to the destination index.

    mov ebx, 0x00000003          ; Set the B-register to 0x00000003.
    mov ecx, 512                 ; Set the C-register to 512.
 
	.SetEntry:
	    mov DWORD [edi], ebx         ; Set the uint32_t at the destination index to the B-register.
	    add ebx, 0x1000              ; Add 0x1000 to the B-register.
	    add edi, 8                   ; Add eight to the destination index.
	    loop .SetEntry               ; Set the next entry.
	mov eax, cr4                 ; Set the A-register to control register 4.
    or eax, 1 << 5               ; Set the PAE-bit, which is the 6th bit (bit 5).
    mov cr4, eax                 ; Set control register 4 to the A-register.

    mov ecx, 0xC0000080          ; Set the C-register to 0xC0000080, which is the EFER MSR.
    rdmsr                        ; Read from the model-specific register.
    or eax, 1 << 8               ; Set the LM-bit which is the 9th bit (bit 8).
    wrmsr                        ; Write to the model-specific register.

    mov eax, cr0                 ; Set the A-register to control register 0.
    or eax, 0x1              ; Set the PG-bit, which is the 32nd bit (bit 31).
    mov cr0, eax                 ; Set control register 0 to the A-register

    lgdt [GDT64.Pointer]         ; Load the 64-bit global descriptor table.
	
	call print_cr3

    jmp GDT64.Code:Realm64       ; Set the code segment and enter 64-bit long mode.

    jmp $

GDT64:                           ; Global Descriptor Table (64-bit).
    .Null: equ $ - GDT64         ; The null descriptor.
    dw 0xFFFF                    ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 1                         ; Granularity.
    db 0                         ; Base (high).
    .Code: equ $ - GDT64         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011010b                 ; Access (exec/read).
    db 10101111b                 ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
    .Data: equ $ - GDT64         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
    .Pointer:                    ; The GDT-pointer.
    dw $ - GDT64 - 1             ; Limit.
    dq GDT64                     ; Base.

[BITS 64]
 
Realm64:
    cli                           ; Clear the interrupt flag.
    mov ax, GDT64.Data            ; Set the A-register to the data descriptor.
    mov ds, ax                    ; Set the data segment to the A-register.
    mov es, ax                    ; Set the extra segment to the A-register.
    mov fs, ax                    ; Set the F-segment to the A-register.
    mov gs, ax                    ; Set the G-segment to the A-register.
    mov ss, ax                    ; Set the stack segment to the A-register.
    mov edi, 0xB8000              ; Set the destination index to 0xB8000.
    mov rax, 0x1F201F201F201F20   ; Set the A-register to 0x1F201F201F201F20.
    mov ecx, 500                  ; Set the C-register to 500.
    hlt                           ; Halt the processor.



times 510 -( $ - $$ ) db 0
dw 0xaa55