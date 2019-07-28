.entry MYSTRUCT
.entry LOL
MAIN: mov r0         ,r3
add r2,STR
LOOP: jmp      END
prn #-5
MYSTRUCT: .struct 3, "hey!"
LOL:   prn r2
sub r1, BOL
.extern BOL
K: .data 22
inc K
 mov MYSTRUCT.1 ,r3
bne LOOP
END: clr r1
STR: .string "abcdef"
THEDATA: .data 6,-9,15
