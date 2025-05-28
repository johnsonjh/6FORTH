6FORTH is a floating point FORTH language interpreter.

6FORTH supports a subset of the standard FORTH vocabulary.

The FORTH language is stack oriented.  That is, all numeric arguments are
put on a stack.  FORTH uses Reverse Polish Notation (RPN) where arithmetic
operators follow their arguments.  For example, 3 + 4 becomes 3 4 +, and
(9 - 8)/(7 * 6) becomes 9 8 - 7 6 * /.

FORTH is a revision of 'TINY.FORTH', written in LOGO by David Malmberg, and
published in the February/March 1985 issue of Powerplay magazine.  FORTH is
written in C for CP-6 and GCOS-8 operating systems by Kathy Larkin, LCPD.

 The FORTH vocabulary includes:

   - Arithmetic operators: +, -, *, /
   - Stack manipulation words: DUP, DROP, SWAP, OVER
   - Logical comparison words: >, =, <, 0>, 0=, 0<, NOT, MIN, MAX, ABS
   - Variable manipulation words: VARIABLE, @, !
   - Execution control words: DO...LOOP, I, LEAVE, IF...THEN, IF...THEN...ELSE,
     BEGIN...UNTIL, BEGIN...WHILE...REPEAT
   - Other words: VLIST, ., .", EMIT, :, LIST, FORGET, CR, ABORT, COLD, SAVE,
     LOAD, HELP, QUIT

 Arithmetic operators:

 Syntax            Stack Contents      Description

 +                 (n2 n1 -- sum)      Replaces top two numbers on the stack
                                       with their sum.
 -                 (n2 n1 -- diff)     Subtracts n1 from n2.
 *                 (n2 n1 -- prod)     Multiplies top two numbers.
 /                 (n2 n1 -- quot)     Divides n2 by n1.

 Examples:

 5 72.6 +                              Puts 77.6 on the top of the stack.
 999.9 99.9 - . CR                     Prints 900.0, leaves stack unchanged.
 5 4 3 * *                             Puts 60.0 on the stack ((3*4)*5).
 6 5 4 3 * * /                         Puts 0.10 on the stack (((3*4)*5)/6).
 34 8 + 3 2 * /                        Puts 7.0 on the stack ((34+8)/(3*2)).

 Stack manipulation words:

 Syntax            Stack Contents      Description

 DUP               (n -- n n)          Duplicates the top number.
 DROP              (n -- )             Discards top number.
 SWAP              (n2 n1 -- n1 n2)    Exchanges top two numbers.
 OVER              (n2 n1 -- n2 n1 n2) Puts copy of second number on top.

 Examples:

 6 7 OVER DUP                          Leaves stack with 6 7 6 6.
 6 7 DROP DUP                          Leaves stack with 6 6.
 6 7 OVER SWAP DROP                    Leaves stack with 6 6.
 6 DUP *                               Leaves 36 on the stack (squares 6).

 Logical comparison words:

 Syntax            Stack Contents      Description

 >                 (n2 n1 -- flag)     True if n2 is greater than n1.
 =                 (n2 n1 -- flag)     True if top two numbers are equal.
 <                 (n2 n1 -- flag)     True if n2 is less than n1.
 0>                (n -- flag)         True if top number is positive.
 0=                (n -- flag)         True if top number is zero.
 0<                (n -- flag)         True if top number is negative.
 NOT               (flag1 -- flag2)    Reverses value of flag.
 MIN               (n2 n1 -- min)      Leaves lesser of two top numbers.
 MAX               (n2 n1 -- max)      Leaves greater of two top numbers.
 ABS               (n -- abs)          Gives absolute value of top number.

 Examples:

 5 3 - 0>                              Puts true (1.0) on the stack.
 -5 3 >                                Puts false (0.0) on the stack.
 10 5 4 MAX DUP * *                    Puts 250.0 (= 5 * 5 * 10) on the stack.
 11 1 DO MIN LOOP                      Leaves minimum of top 10 numbers on
                                       the stack.
 5 3 + 8 = IF ." Equal! " THEN CR      Prints 'Equal!'.
 5 3 - 8 = IF ." Equal! " ELSE ." Not Equal! " CR    Prints 'Not Equal!'.

 Variable manipulation words:

 Syntax            Stack Contents      Description

 VARIABLE xxx      (n -- )             Creates a variable named xxx with an
                                       initial value equal to n.
 xxx @             ( -- n)             (Fetch) Puts the current value of the
                                       variable xxx on the stack.
 xxx !             (n -- )             (Store) Sets the value of the variable
                                       xxx to n.

 Examples:

 5 VARIABLE FIVE                       Creates variable FIVE with value = 5.
 4 FIVE @ +                            Puts 9 on the stack (given FIVE = 5).
 35 20 - VARIABLE DIFF                 Creates DIFF with value = 15.
 DIFF @ FIVE @ - DIFF !                Changes value of DIFF to 10.

 Execution control words:

 Syntax            Stack Contents      Description

 DO...LOOP         (end+1 start -- )   Performs words between DO and LOOP
                                       (end - start) number of times.
 I                 ( -- index)         Puts current loop index on stack.
 LEAVE             ( -- )              Exits DO...LOOP after this iteration.
 IF...THEN         (flag -- )          Executes words between IF and THEN if
                                       flag is true.
 IF...ELSE...THEN  (flag -- )          Executes words between IF and ELSE if
                                       flag is true, and words between ELSE
                                       and THEN if flag is false.
 BEGIN...UNTIL     (flag -- )          Loops back to BEGIN until flag tested
                                       at UNTIL is true.
 BEGIN..WHILE..REPEAT (flag -- )       Tests flag at WHILE and jumps past
                                       REPEAT if false.  Jumps back to BEGIN
                                       when REPEAT encountered.

 Examples:

 FLAG @ IF ." Flag true " THEN CR    Prints 'Flag true' if variable FLAG = 1.
 FLAG @ IF ." Flag true " ELSE ." Flag false " THEN CR
                                       Prints 'Flag true' if FLAG = 1,
                                       otherwise prints  'Flag false'
 11 0 DO I . LOOP CR                   Prints numbers from 0 to 10.
 10 BEGIN DUP . 1 - DUP 0= UNTIL . CR  Prints numbers from 10 to 0.

 Other words:

 Syntax            Stack Contents      Description

 VLIST             ( -- )              Prints a vocabulary list of all
                                       currently defined FORTH words.
 .                 (n -- )             (Dot) Prints top of stack.
 ." mmm "          ( -- )              (Dot quote) Prints message until ending
                                       quote mark.
 : xxx ... ;       ( -- )              Defines new FORTH word xxx with
                                       definition equal to FORTH words up to
                                       ending semi-colon.
 LIST              ( -- )              Prints definitions of all new FORTH
                                       words (all colon-definitions).
 FORGET xxx        ( -- )              Discards the current definition of
                                       FORTH word xxx.
 CR                ( -- )              Prints a carriage return.
 ABORT             ( -- )              Clears the stack and DO...LOOP indices.
 COLD              ( -- )              Restarts FORTH from scratch.
 SAVE xxx          ( -- )              Saves all new FORTH definitions over
                                       file xxx.  If file xxx does not exist,
                                       it is created.  If file xxx does exist,
                                       it is overwritten.
 LOAD xxx          ( -- )              Loads FORTH definitions from file xxx.
 HELP xxx          ( -- )              Prints information for FORTH word xxx.
                                       HELP ? prints information on all FORTH
                                       words.
 QUIT              ( -- )              Exits FORTH.

 Examples:

 11 1 DO I . LOOP CR                    Prints numbers from 1 to 10.
 -12 7 + ." Sum is " . CR               Prints 'Sum is -5.00'.
 : SQUARE DUP * ; : CUBE DUP SQUARE * ; Defines two new FORTH words, SQUARE
                                        and CUBE, and places them into the
                                        the vocabulary list. (To print the
                                        cube of n, type 'n CUBE . CR')
 : FACTORIAL 1 SWAP 1 + 1 DO I * LOOP ; Defines new FORTH word FACTORIAL.
 FORGET FACTORIAL                       Removes the definition of FORTH
                                        word FACTORIAL.
 SAVE :FORTH                            Saves all user defined FORTH
                                        definitions in file named :FORTH.
 LOAD :FORTH                            All definitions previously SAVEd
                                        into file :FORTH are reloaded.
 HELP ?                                 Prints syntax information for all
                                        FORTH words.
 HELP LOOP                              Prints syntax information for the
                                        DO...LOOP construct.
