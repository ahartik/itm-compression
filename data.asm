BITS 32
global ex1_encoded,ex2_encoded
section .text

ex1_encoded:
    incbin "ex1_data.bin"
ex2_encoded:
    incbin "ex2_data.bin"
