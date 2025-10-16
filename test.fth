CR ." Integer Forth Test Program V1"
CR ." =============================" CR CR

3 VARIABLE A
2 VARIABLE B

CR ." DO...LOOP and BEGIN...UNTIL:" CR
11 0 DO I . LOOP CR
10 BEGIN DUP . 1 - DUP 0= UNTIL . CR

CR ." Arithmetic Operations: " CR
A @ B @ + . CR
A @ B @ - . CR
A @ B @ * . CR
A @ B @ / . CR

CR ." Minimum and Maximum:" CR
A @ B @ MIN .  ."  is min" CR
A @ B @ MAX .  ."  is max" CR

CR ." Zero and Sign Tests:" CR
-5.0 DUP ABS . CR
5.0 0> IF ."  5.0 is positive" THEN CR
-5.0 0< IF ." -5.0 is negative" THEN CR
0.0 0= IF ."  0 is zero" THEN CR

CR ." Stack Manipulation: " CR
7 8 OVER . . . CR
3 4 SWAP . . CR

CR ." Defining SQUARE word: " CR
: SQUARE DUP * ;
."   5 SQUARE:"
5 SQUARE . CR

CR ." Defining CUBE word:" CR
: CUBE DUP SQUARE * ;
."   9 CUBE:"
9 CUBE . CR

CR ." Defining FACTORIAL word:" CR
: FACTORIAL 1 SWAP 1 + 1 DO I * LOOP ;
."   13 FACTORIAL:"
13 FACTORIAL . CR

CR ." List new words:" CR
LIST

CR ." Forgetting words:" CR
FORGET SQUARE
FORGET CUBE
FORGET FACTORIAL

CR ." EMIT (letter A): "
65 EMIT CR

CR ." Variable assignment:" CR
100 VARIABLE TEMP
TEMP @ . CR
200 TEMP !
TEMP @ . CR

CR ." Equality testing:" CR
."  5 3 - 8 is "
5 3 - 8 = IF ." equal" ELSE ." not equal" THEN CR
."  5 3 + 8 is "
5 3 + 8 = IF ." equal" ELSE ." not equal" THEN CR

CR ." Listing vocabulary:" CR
VLIST

CR ." Reset environment." CR
COLD

CR ." Setup multiplication table:" CR
: ROW CR 11 1 DO DUP I * . LOOP DROP ;
: ROWS 11 1 DO I ROW LOOP ;
CR ." Multiplication Table:" ROWS CR

CR ." Goodbye, cruel world." CR
QUIT
