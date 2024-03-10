.data
job: .half 1,2,3,4
place: .asciz "shimla"
people: .dword 100
place .byte 4,3,2,1
name: .asciz "harjas
.text
add x1,x2,x3
label1: add x1,x2,x3
label2: beq x0,x0,label1
addi x1,x2,3
add x1,x2,x3
jal x3,label2
sw x4,4(x3)
lui x3, 5
exit:




