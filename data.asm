BITS 32
global ex1_encoded,ex2_encoded,ex4_encoded
section .text

ex1_encoded:
    incbin "ex1_data.bin"
ex2_encoded:
    incbin "ex2_data.bin"
ex4_encoded:
    incbin "ex4_data.bin"
