.invalid "hi"
VALID: .string "hello"
mov VALID.15,r2
LOOP: cmp r2,0
bne LOOP
.struct 3
stop