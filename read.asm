BITS 32
;global	readdata,writedata,openfile
global	call4
section .text

; int open(
open

call4:
	mov	eax,[esp+4]
	mov	ebx,[esp+8]
	mov	ecx,[esp+12]
	mov	edx,[esp+16]
	int	128
	ret

;; void readdata(int fd, char* buf, int len)
;readdata:
;	mov	eax,3	; sys_read
;readwrite:
;	mov	ebx,[esp+4]
;	mov	ecx,[esp+8]
;	mov	edx,[esp+12]
;	int	128
;	ret
;
;; void writedata(int fd, const char* buf, int len)
;writedata:
;	mov	eax,4 ; sys_write
;	jmp	readwrite
;
;; void openfile(const char* file, int flags, int mode)
;openfile:
;	mov	eax,5 ; sys_open
;	jmp	readwrite
