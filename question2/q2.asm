; Program: Temperature Data Processor
; Purpose: Reads 'temperature_data.txt', counts total lines and valid entries.
; Handles: LF (\n), CRLF (\r\n), and file errors.
; -----------------------------------------------------------------------------

section .data
    filename    db  "temperature_data.txt", 0
    total_msg   db  "Total readings: ", 0
    valid_msg   db  "Valid readings: ", 0
    err_msg     db  "Error: Could not open file.", 10, 0
    newline     db  10, 0

section .bss
    fd          resd 1          ; File descriptor
    buffer      resb 4096       ; 4KB buffer to load file into memory
    bytes_read  resd 1
    out_str     resb 16         ; Space for integer-to-string conversion

section .text
    global _start

_start:
    ; --- FILE HANDLING: OPEN ---
    ; sys_open (eax=5), filename (ebx), access mode (ecx=0 for O_RDONLY)
    mov eax, 5
    mov ebx, filename
    mov ecx, 0
    int 0x80

    ; Check for error (negative return value in eax)
    test eax, eax
    js open_error
    mov [fd], eax

    ; --- FILE HANDLING: READ ---
    ; sys_read (eax=3), fd (ebx), buffer (ecx), count (edx)
    mov eax, 3
    mov ebx, [fd]
    mov ecx, buffer
    mov edx, 4096
    int 0x80
    mov [bytes_read], eax

    ; --- INITIALIZE COUNTERS ---
    xor esi, esi    ; esi = buffer offset
    xor edi, edi    ; edi = Total lines (X)
    xor ebp, ebp    ; ebp = Valid readings (Y)
    xor ecx, ecx    ; ecx = Flag: Is the current line non-empty? (0=No, 1=Yes)

process_loop:
    cmp esi, [bytes_read]
    jge finalize_last_line ; End of buffer reached

    mov al, [buffer + esi]

    ; --- STRING TRAVERSAL LOGIC ---
    ; Check for Carriage Return (\r) - skip it to handle CRLF
    cmp al, 13
    je skip_char

    ; Check for Line Feed (\n) - marks end of a line
    cmp al, 10
    je end_of_line

    ; If it's not a control char and not a space, it's a "valid" character
    cmp al, ' '
    jbe skip_char   ; Ignore spaces or other controls for validity
    mov ecx, 1      ; Mark this line as having valid data

skip_char:
    inc esi
    jmp process_loop

end_of_line:
    inc edi         ; Increment total readings (X)
    add ebp, ecx    ; Add 1 to Y if flag was set, else 0
    xor ecx, ecx    ; Reset flag for the next line
    inc esi
    jmp process_loop

finalize_last_line:
    ; If file doesn't end with a newline but has data, count the last line
    test esi, esi
    jz print_results
    inc edi
    add ebp, ecx

print_results:
    ; Display "Total readings: X"
    push total_msg
    call print_string
    mov eax, edi
    call print_int

    ; Display "Valid readings: Y"
    push valid_msg
    call print_string
    mov eax, ebp
    call print_int

    jmp exit

open_error:
    push err_msg
    call print_string

exit:
    ; Close file descriptor
    mov eax, 6
    mov ebx, [fd]
    int 0x80

    ; sys_exit
    mov eax, 1
    xor ebx, ebx
    int 0x80

; --- HELPER FUNCTIONS ---

print_string:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8] ; String pointer
    
    ; Calculate length
    xor edx, edx
.len_loop:
    cmp byte [ebx + edx], 0
    je .do_print
    inc edx
    jmp .len_loop
.do_print:
    mov eax, 4      ; sys_write
    mov ecx, ebx
    mov ebx, 1      ; stdout
    int 0x80
    pop ebp
    ret 4

print_int:
    ; Simple Int to ASCII conversion
    mov ecx, out_str
    add ecx, 15
    mov byte [ecx], 0
    mov ebx, 10
.convert:
    xor edx, edx
    div ebx
    add dl, '0'
    dec ecx
    mov [ecx], dl
    test eax, eax
    jnz .convert
    
    push ecx
    call print_string
    push newline
    call print_string
    ret