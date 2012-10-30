BITS 32
global	readdata
section .text

; void readdata(int fd, char* out, int len)
readdata:
	mov	eax,3	; sys_read
	mov	ebx,[esp+4]
	mov	ecx,[esp+8]
	mov	edx,[esp+12]
	int	128
	ret
