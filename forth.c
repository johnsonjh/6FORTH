/*T**************************************************************
 *T*                                                            *
 *T*  Copyright (c) Bull HN Information Systems Inc., 1989      *
 *T*                                                            *
 *T**************************************************************/
 
/* About the program:
 
   This program is a revision of Tiny FORTH written by David Malmberg,
   published in 'Powerplay', February/March 1985, written in LOGO.
 
   For ideas, information, comments, errors, ..., concerning this program,
   please contact:
 
      Kathy Larkin
                LCPD (SDTC 1-523)
       HVN 862-4204
 
   Revision 2.0 (5/16/85 KEL)
    >  Added HELP function.
    >  Added EMIT function.
    >  Changed some ugly stuff.
   Revision 2.1 (5/21/85 KEL)
    >  Added SAVE and LOAD functions.
   Revision A01 (6/7/85 KEL)
    >  Changed greeting and version number
    >  Submitted to LADC X account
 */
 
#include      <stdio.h>
/*
 *
 * Global Defines
 *
 */
#define       ARGSIZE             50
#define       BUFSIZE             200
#define       DEFSIZE             100
#define       EMPTY               0
#define       EVER                ( ; ; )
#define       FALSE               0.0
#define       FIRST_ASCII         '!'
#define       INITVOCABSIZE       48
#define       LAST_ASCII          'z'
#define       NAMESIZE            32
#define       NDXSTACKSIZE        20
#define       NOT_FOUND           -1
#define       STACKSIZE           100
#define       TRUE                1.0
#define       VARSIZE             100
#define       VOCABSIZE           148
/*
 *
 * Global Structures and Variables
 *
 */
struct        Deflist {
              char dname [NAMESIZE];
              char *list_ptr;
              };
struct        Deflist definition [DEFSIZE];
 
struct        Vartable {
              char name [NAMESIZE];
              double vvalue;
              };
struct        Varstruct {
              int count;
              struct Vartable vartbl [VARSIZE];
              };
struct        Varstruct variable;
 
struct        Vocablist {
              int count;
              char vname [VOCABSIZE] [NAMESIZE];
              };
static struct Vocablist vocabulary = {0, "+", "-", "*", "/", "=", ">", "<",
                        "MIN", "MAX", "0=", "0>", "0<", "ABS", "NOT", ".",
                        ".\"", "DROP", "DO", "LOOP", "I", "LEAVE", "BEGIN",
                        "UNTIL", "WHILE", "REPEAT", "DUP", "SWAP", "ABORT",
                        "COLD", "LIST", "CR", "@", "!", "VARIABLE", ":", ";",
                        "VLIST", "IF", "THEN", "ELSE", "FORGET", "OVER", "EMIT",
                        "SAVE", "LOAD","QUIT", "HELP", "?"};
 
struct        Stacktype {
              int top;
              double value [STACKSIZE];
              };
struct        Stacktype stack;
struct        Stacktype ndx_stack;
 
char          in_buf [BUFSIZE];
double        number;
/*
 *
 * Main Program
 *
 */
main ()
{
   tiny_forth ();
   }
/*
 *
 * Function                       tiny_forth ()
 *
 */
   tiny_forth ()
   {
      title ();
      setup ();
      input ();
      }
/*
 *
 * Function                       title ()
 *
 */
   title () /* prints FORTH titles */
   {
      printf ("FORTH A01 here.\n");
      }
/*
 *
 * Function                       setup()
 *
 */
   setup () /* performs initialization tasks */
   {
      int i;
      vocabulary.count = INITVOCABSIZE;
      stack.top = ndx_stack.top = 0;
      variable.count = 0;
      for (i = 0; i < DEFSIZE; definition [i++].list_ptr = NULL);
      }
/*
 *
 * Function                       push (value)
 *
 */
   double push (value) /* pushes value onto stack */
   double value;
   {
      if (stack.top == STACKSIZE)
         printf (" *** Stack overflow!\n");
      else stack.value [stack.top++] = value;
      return (value);
      }
/*
 *
 * Function                       pop ()
 *
 */
   double pop () /* pops top value off of the stack */
   {
      if (stack.top == EMPTY){
         printf (" *** Attempting to pop empty stack!\n");
         return (0);
         }
      else return (stack.value [--stack.top]);
      }
/*
 *
 * Function                       input ()
 *
 */
   input () /* gets and processes user input */
   {
      char *ptr, *gets ();
      for EVER {
         ptr = gets (in_buf);
         while (isspace (*ptr)) ptr++;
         if (*ptr != '\0') do_list (ptr);
         }
      }
/*
 *
 * Function                       do_list ()
 *
 */
   do_list (ptr) /* processes line of input */
   char *ptr;
   {
      char *do_word (), *next ();
      while (ptr != NULL)
         ptr = next (do_word (ptr));
      }
/*
 *
 * Function                       do_word (ptr)
 *
 */
   char *do_word (ptr) /* processes one word of input */
   char *ptr;
   {
      int i;
      char word [NAMESIZE],
           *var_process (), *print_list (), *if_proc (), *loop (), *jump (), *help_proc (), *xit (),
           *next (), *prev (), *def_code (), *begin_stmt (), *do_process (), *save_defs (), *load_defs ();
      double x, y, push (), pop (), push_ndx (), pop_ndx ();
 
      make_word (ptr, word);
      switch (i = match_vocab (word)) {
         case /*     +    */  (0): push (pop () + pop ()); break;
         case /*     -    */  (1): x = pop (); push (pop () - x); break;
         case /*     *    */  (2): push (pop () * pop ()); break;
         case /*     /    */  (3): y = pop (); push (pop () / y); break;
         case /*     =    */  (4):
         case /*     >    */  (5):
         case /*     <    */  (6):
         case /*    MIN   */  (7):
         case /*    MAX   */  (8):
         case /*    0=    */  (9):
         case /*    0>    */ (10):
         case /*    0<    */ (11):
         case /*    ABS   */ (12):
         case /*    NOT   */ (13): try_logic (i - 3); break;
         case /*     .    */ (14): printf (" %5.3f", pop ()); break;
         case /*    ."    */ (15): ptr = print_list (ptr); break;
         case /*   DROP   */ (16): pop (); break;
         case /*    DO    */ (17): ptr = do_process (ptr); break;
         case /*   LOOP   */ (18): ptr = loop (ptr); break;
         case /*     I    */ (19): push (push_ndx (pop_ndx ())); break;
         case /*   BEGIN  */ (21): ptr = begin_stmt (ptr); break;
         case /*   UNTIL  */ (22): if (!(pop ()==TRUE)) ptr = jump (ptr, "BEGIN", prev); break;
         case /*   WHILE  */ (23): if (pop ()==FALSE) ptr = jump (ptr, "REPEAT", next); break;
         case /*  REPEAT  */ (24): ptr = jump (ptr, "BEGIN", prev); break;
         case /*    DUP   */ (25): push (push (pop ())); break;
         case /*   SWAP   */ (26): x = pop (); y = pop (); push (x); push (y); break;
         case /*   ABORT  */ (27): stack.top = ndx_stack.top = EMPTY; break;
         case /*   COLD   */ (28): tiny_forth (); break;
         case /*   LIST   */ (29): list_defs (); break;
         case /*    CR    */ (30): printf ("\n"); break;
         case /*     @    */ (31):
         case /*     !    */ (32): printf (" ** Expected variable name before %s not found.\n", word); break;
         case /* VARIABLE */ (33): def_var (ptr = next (ptr)); break;
         case /*     :    */ (34): ptr = def_code (ptr); break;
         case /*     ;    */ (35): printf (" ** Syntax error.  Type HELP ; for more information.\n"); break;
         case /*   VLIST  */ (36): for(i = 0;i < vocabulary.count;printf("%s ",vocabulary.vname[i++]));
                                   printf ("\n"); break;
         case /*     IF   */ (37): ptr = if_proc (ptr); break;
         case /*    THEN  */ (38): break;
         case /*    ELSE  */ (39): ptr = jump (ptr, "THEN",  next); break;
         case /*   FORGET */ (40): ptr = xit (ptr); break;
         case /*    OVER  */ (41): x = pop (); push (y = pop ()); push (x); push (y);break;
         case /*    EMIT  */ (42): putc ((int) pop (), stdout); break;
         case /*    SAVE  */ (43): ptr = save_defs (ptr); break;
         case /*    LOAD  */ (44): ptr = load_defs (ptr); break;
         case /*    QUIT  */ (45): exit (0);
         case /*    HELP  */ (46): ptr = help_proc (ptr); break;
         case /*     ?    */ (47): printf (" Type HELP ? for syntax information.\n");break;
         default: if (numeric (word)) push (number);
                  else if ((i = def_member (word)) >= 0) do_list (definition [i].list_ptr);
                     else if ((i = var_member (word)) >= 0) ptr = var_process (ptr, word, i);
                        else printf (" %s has not been defined yet!\n", word);
         }
      return (ptr);
      }
/*
 *
 * Function                       numeric (word)
 *
 */
   numeric (word) /* sets global number and returns TRUE if word is numeric */
   char *word;
   {
      char *num_ptr;
      double strtod ();
      number = strtod (word, &num_ptr);
      return ((num_ptr == word) ?  FALSE : TRUE);
      }
/*
 *
 * Function                       str_cmp (str1, str2)
 *
 */
   str_cmp (str1, str2) /* compares strings */
   char *str1, *str2;
   {
      while (*str1 == *str2)
         if ((*str1++ == *str2) && (*str2++ == '\0'))
            return (TRUE);
      return (FALSE);
      }
/*
 *
 * Function                       make_word (ptr, word)
 *
 */
   make_word (ptr, word) /* moves word pointed at by ptr into word and ends with \0 */
   char *ptr, word [];
   {
      int i = 0;
      while ((word [i++] = ((*ptr >= FIRST_ASCII) && (*ptr <= LAST_ASCII)) ?
            *ptr++ : '\0' ) != '\0');
      }
/*
 *
 * Function                       match_vocab (word)
 *
 */
   match_vocab (word) /* returns index of vocabulary item which matches word */
   char word [];
   {
      int i;
      for (i = 0; i < INITVOCABSIZE && !str_cmp (word, vocabulary.vname [i]); i++);
      return (i);
      }
/*
 *
 * Function                       help_proc (ptr)
 *
 */
   char *help_proc (ptr) /* processes HELP request */
   char *ptr;
   {
      int i;
      char word [NAMESIZE], *next ();
      if ((ptr = next (ptr)) == NULL) {
         printf (" ** Expected vocabulary word after HELP not found.\n");
         printf ("    Type HELP HELP for more information.\n");
         }
      else {
         make_word (ptr, word);
         if (str_cmp (word, "?"))
            for (i = 0; i < INITVOCABSIZE; i++)
               switch (i) /* eliminate duplicates */ {
                  case (17):                       /* DO LOOP */
                  case (21): case (23): case (24): /* BEGIN UNTIL WHILE REPEAT */
                  case (34):                       /* : ; */
                  case (37): case (39):            /* IF THEN ELSE */
                  case (46): break;                /* HELP */
                  default: prnt_help (i);
                  }
         else prnt_help (match_vocab (word));
         }
      return (ptr);
      }
/*
 *
 * Function                       prnt_help (i)
 *
 */
   prnt_help (i) /* Prints help information for the ith vocabulary word */
   int i;
   {
      switch (i) {
         case ( 0): printf (" +                      (n2 n1 -- sum)       ");
                    printf ("Adds two top numbers.\n"); break;
         case ( 1): printf (" -                      (n2 n1 -- diff)      ");
                    printf ("Subtracts n1 from n2.\n"); break;
         case ( 2): printf (" *                      (n2 n1 -- prod)      ");
                    printf ("Multiplies two top numbers.\n"); break;
         case ( 3): printf (" /                      (n2 n1 -- quot)      ");
                    printf ("Divides n2 by n1.\n"); break;
         case ( 4): printf (" =                      (n2 n1 -- flag)      ");
                    printf ("True if top two numbers are equal.\n"); break;
         case ( 5): printf (" >                      (n2 n1 -- flag)      ");
                    printf ("True if n2 greater than n1.\n"); break;
         case ( 6): printf (" <                      (n2 n1 -- flag)      ");
                    printf ("True if n2 less than n1.\n"); break;
         case ( 7): printf (" MIN                    (n2 n1 -- min)       ");
                    printf ("Leaves lesser of two top numbers.\n"); break;
         case ( 8): printf (" MAX                    (n2 n1 -- max)       ");
                    printf ("Leaves greater of two top numbers.\n"); break;
         case ( 9): printf (" 0=                     (n -- flag)          ");
                    printf ("True if top number is zero.\n"); break;
         case (10): printf (" 0>                     (n -- flag)          ");
                    printf ("True if top number is positive.\n"); break;
         case (11): printf (" 0<                     (n -- flag)          ");
                    printf ("True if top number is negative.\n"); break;
         case (12): printf (" ABS                    (n -- absolute)      ");
                    printf ("Gives absolute value of top number.\n"); break;
         case (13): printf (" NOT                    (flag1 -- flag2)     ");
                    printf ("Reverses value of flag.\n"); break;
         case (14): printf (" .                      (n -- )              ");
                    printf ("Prints top of stack.\n"); break;
         case (15): printf (" .\"                     ( -- )               ");
                    printf ("Prints message until \" mark.\n");break;
         case (16): printf (" DROP                   (n -- )              ");
                    printf ("Discards top number.\n"); break;
         case (17):
         case (18): printf (" DO...LOOP              (end+1 start -- )    ");
                    printf ("Performs loop given index range.\n"); break;
         case (19): printf (" I                      ( -- index)          ");
                    printf ("Puts current loop index on stack.\n"); break;
         case (20): printf (" LEAVE                  ( -- )               ");
                    printf ("Terminates loop at next LOOP.\n"); break;
         case (21):
         case (22): printf (" BEGIN...UNTIL          (flag -- )           ");
                    printf ("Loops back to BEGIN until flag\n"); blank_fill ();
                    printf ("tested at UNTIL is true.\n");
         case (23):
         case (24): printf (" BEGIN...WHILE...REPEAT (flag -- )           ");
                    printf ("Tests flag at WHILE and jumps past\n"); blank_fill ();
                    printf ("REPEAT if false.  REPEAT causes\n"); blank_fill ();
                    printf ("unconditional jump to BEGIN.\n"); break;
         case (25): printf (" DUP                    (n -- nn)            ");
                    printf ("Duplicates the top number.\n"); break;
         case (26): printf (" SWAP                   (n2 n1 -- n1 n2)     ");
                    printf ("Exchanges two top numbers.\n"); break;
         case (27): printf (" ABORT                  ( -- )               ");
                    printf ("Clears stack and DO...LOOP indices.\n"); break;
         case (28): printf (" COLD                   ( -- )               ");
                    printf ("Restarts FORTH from scratch.\n"); break;
         case (29): printf (" LIST                   ( -- )               ");
                    printf ("Prints definitions of all new\n"); blank_fill ();
                    printf ("FORTH words.\n"); break;
         case (30): printf (" CR                     ( -- )               ");
                    printf ("Prints a carriage return.\n"); break;
         case (31): printf (" xxx @                  ( -- n)              ");
                    printf ("Puts the current value of variable\n"); blank_fill ();
                    printf ("xxx on the stack.\n"); break;
         case (32): printf (" xxx !                  (n -- )              ");
                    printf ("Sets the value of the variable xxx\n"); blank_fill ();
                    printf ("to the top of the stack.\n"); break;
         case (33): printf (" VARIABLE xxx           (n -- )              ");
                    printf ("Creates a variable named xxx with\n"); blank_fill ();
                    printf ("initial value equal to the top of\n"); blank_fill ();
                    printf ("the stack.\n"); break;
         case (34):
         case (35): printf (" : xxx ... ;            ( -- )               ");
                    printf ("Defines new FORTH word named xxx.\n"); break;
         case (36): printf (" VLIST                  ( -- )               ");
                    printf ("Prints vocabulary list of current\n"); blank_fill ();
                    printf ("FORTH words.\n"); break;
         case (37):
         case (38): printf (" IF...THEN              (flag -- )           ");
                    printf ("Executes words between IF and THEN\n"); blank_fill ();
                    printf ("if flag is true.\n");
         case (39): printf (" IF...ELSE...THEN       (flag -- )           ");
                    printf ("Executes words between IF and ELSE\n"); blank_fill ();
                    printf ("if flag is true and words between\n"); blank_fill ();
                    printf ("ELSE and THEN if flag is false.\n"); break;
         case (40): printf (" FORGET xxx             ( -- )               ");
                    printf ("Erases the current definition of\n"); blank_fill ();
                    printf ("FORTH word xxx.\n"); break;
         case (41): printf (" OVER                   (n2 n1 -- n2 n1 n2)  ");
                    printf ("Puts copy of 2nd number on top.\n"); break;
         case (42): printf (" EMIT                   (n -- )              ");
                    printf ("Outputs top of stack as character.\n"); break;
         case (43): printf (" SAVE xxx               ( -- )               ");
                    printf ("Saves current definitions in file\n"); blank_fill ();
                    printf ("xxx (see LOAD command).\n"); break;
         case (44): printf (" LOAD xxx               ( -- )               ");
                    printf ("Restores SAVEd definitions from\n"); blank_fill ();
                    printf ("file xxx.\n"); break;
         case (45): printf (" QUIT                   ( -- )               ");
                    printf ("Exits the FORTH interpreter.\n"); break;
         case (46):
         case (47): printf (" HELP xxx               ( -- )               ");
                    printf ("Prints info on topic xxx.\n");
                    printf (" HELP ?                 ( -- )               ");
                    printf ("Prints info on all topics.\n"); break;
         default  : printf (" ** No HELP available on that topic.\n");
                    printf ("    Type HELP HELP for syntax information.\n"); break;
         }
      }
/*
 *
 * Function                       blank_fill ()
 *
 */
   blank_fill () /* outputs 45 blanks */
   {
      int i;
      for (i = 0; i < 45; i++)
         putc (' ', stdout);
      }
/*
 *
 * Function                       list_defs ()
 *
 */
   list_defs () /* lists current definitions */
   {
      int ndex, no_defs_found;
      for (ndex = 0, no_defs_found = TRUE; ndex < DEFSIZE; ndex++)
         if (definition [ndex].list_ptr) {
            printf (" %s: %s\n", definition [ndex].dname, definition [ndex].list_ptr);
            no_defs_found = FALSE;
            }
      if (no_defs_found)
         printf (" No new FORTH words defined.\n");
      }
/*
 *
 * Function                       def_member (word)
 *
 */
   def_member (word) /* tries to match word with a current definition */
   char word [];
   {
      int def_ndx = 0;
      while (def_ndx < DEFSIZE)
         if (definition [def_ndx].list_ptr && (str_cmp (word, definition[def_ndx].dname)))
            return (def_ndx);
         else def_ndx++;
      return (NOT_FOUND);
      }
/*
 *
 * Function                       def_code (ptr)
 *
 */
   char *def_code (ptr) /* creates a word definition */
   char *ptr;
   {
      int i;
      char *end_ptr, *malloc (), *strchr (), *next ();
 
      if ((end_ptr = strchr (ptr, ';')) == NULL)
         printf (" ** No ending ; found in definition.\n");
      else {
         for (i = 0; (i < DEFSIZE) && (definition [i].list_ptr); i++);
         if (i >= DEFSIZE)
            printf (" ** Out of definition space!\n");
         else {
            make_word (ptr = next(ptr), definition [i].dname);
            printf (" %s is now defined as a word.\n", definition [i].dname);
            strcpy (vocabulary.vname [vocabulary.count++], definition [i].dname);
            *(end_ptr - 1) = '\0';
            strcpy (definition [i].list_ptr = malloc (sizeof (in_buf)), next (ptr));
         }
      }
      return (end_ptr);
   }
/*
 *
 * Function                       if_proc (ptr)
 *
 */
   char *if_proc (ptr) /* processes IF...THEN and IF...ELSE...THEN statements */
   char *ptr;
   {
      char *target_ptr = ptr, *strstr ();
      double pop ();
      if (pop () == FALSE)
         if ((target_ptr = strstr (ptr, "ELSE")) == NULL)
            if ((target_ptr = strstr (ptr, "THEN")) == NULL)
               printf (" ** Expected ELSE or THEN not found.\n");
      return (target_ptr);
      }
/*
 *
 * Function                       var_member (word)
 *
 */
   var_member (word) /* tries to match word with a variable definition */
   char word [];
   {
      int var_ndx = 0;
      while (var_ndx < variable.count)
         if (str_cmp (word, variable.vartbl [var_ndx].name)) return (var_ndx);
         else var_ndx++;
      return (NOT_FOUND);
      }
/*
 *
 * Function                       var_process (ptr, word, var_ndx)
 *
 */
   char *var_process (ptr, word, var_ndx) /* creates a variable definition */
   char *ptr, word [];
   int var_ndx;
   {
      double pop (), push ();
      make_word (ptr = next (ptr), word);
      if (str_cmp (word, "@")) push (variable.vartbl [var_ndx].vvalue);
      else if (str_cmp (word, "!")) variable.vartbl [var_ndx].vvalue = pop ();
         else printf (" ** Expected @ or ! after variable %s not found.\n",
                      variable.vartbl [var_ndx].name);
      return (ptr);
      }
/*
 *
 * Function                       do_process (loop_ptr)
 *
 */
   char *do_process (ptr) /* processes DO...LOOP statements */
   char *ptr;
   {
      char *strstr ();
      double ndx_start, pop (), push_ndx ();
      if (strstr (ptr, "LOOP") == NULL){
         printf (" ** DO without ending LOOP.\n");
         return (NULL);
         }
      else {
         ndx_start = pop ();
         push_ndx (pop ());
         push_ndx (ndx_start);
         return (ptr);
         }
      }
/*
 *
 * Function                       push_ndx (ndx_value)
 *
 */
   double push_ndx (ndx_value) /* pushes ndx_value onto ndx_stack */
   double ndx_value;
   {
      if (ndx_stack.top == NDXSTACKSIZE)
         printf (" *** Index stack overflow!\n");
      else ndx_stack.value [ndx_stack.top++] = ndx_value;
      return (ndx_value);
      }
/*
 *
 * Function                       pop_ndx ()
 *
 */
   double pop_ndx () /* pops ndx_stack */
   {
      if (ndx_stack.top == EMPTY){
         printf (" *** Index stack underflow!\n");
         return (0);
         }
      else return (ndx_stack.value [--ndx_stack.top]);
      }
/*
 *
 * Function                       loop (ptr)
 *
 */
   char *loop (ptr) /* processes DO...LOOP iteration */
   char *ptr;
   {
      double pop_ndx (), push_ndx (),
             loop_cur = pop_ndx () + 1, loop_end;
      char *prev (), *jump ();
 
      if ((loop_end = pop_ndx ()) != loop_cur){
         push_ndx (loop_end);
         push_ndx (loop_cur);
         ptr = jump (ptr, "DO", prev);
         }
      return (ptr);
      }
/*
 *
 * Function                       jump (ptr, target, sequence)
 *
 */
   char *jump (ptr, target, sequence) /* returns a pointer to target */
   char *ptr,
        target [],
        *(*sequence) ();
   {
      char word [NAMESIZE];
      do make_word (ptr = (*sequence) (ptr), word);
         while (!str_cmp (word, target));
      return (ptr);
      }
/*
 *
 * Function                       try_logic (function)
 *
 */
   try_logic (function) /* processes logic function statements */
   int function;
   {
      double pop (), push (),
             z2 = pop (), z1;
      if (function < 6) z1 = pop ();
      switch (function) {
         case (1): push ((z1 == z2) ? TRUE : FALSE); break;
         case (2): push ((z1 > z2) ? TRUE : FALSE); break;
         case (3): push ((z1 < z2) ? TRUE : FALSE); break;
         case (4): push ((z1 <= z2) ? z1 : z2); break;
         case (5): push ((z1 >= z2) ? z1 : z2); break;
         case (10): /* same as case (6) */
         case (6): push ((z2 == 0) ? TRUE : FALSE); break;
         case (7): push ((z2 > 0) ? TRUE : FALSE); break;
         case (8): push ((z2 < 0) ? TRUE : FALSE); break;
         case (9): push ((z2 < 0) ? -z2 : z2); break;
         default : printf ("*** Internal error: invalid function %d\n", function); break;
         }
   }
/*
 *
 * Function                       print_list (ptr)
 *
 */
   char *print_list (ptr) /* processes ." (print) statement */
   char *ptr;
   {
      char *strchr ();
      if (strchr (ptr += 3, '\"') == NULL){
         printf (" ** Ending \" not found.\n");
         return (NULL);
         }
      else {
         do putc (*ptr, stdout);
            while (*++ptr != '\"');
         return (ptr);
         }
      }
/*
 *
 * Function                       def_var (ptr)
 *
 */
   def_var (ptr) /* sets up new variable definition */
   char *ptr;
   {
      if (variable.count >= VARSIZE)
         printf (" ** Out of variable space!\n");
      else {
         make_word (ptr, variable.vartbl[variable.count].name);
         variable.vartbl [variable.count].vvalue = pop ();
         printf (" %s is now defined as a variable.\n", variable.vartbl [variable.count++].name);
         }
      }
/*
 *
 * Function                       begin_stmt (ptr)
 *
 */
   char *begin_stmt (ptr) /* checks syntax of BEGIN...UNTIL or BEGIN...WHILE...REPEAT statement */
   char *ptr;
   {
      char *strstr ();
      if ((strstr (ptr, "UNTIL") != NULL) ||
         ((strstr (ptr, "WHILE") != NULL) && (strstr (ptr, "REPEAT") != NULL)))
         return (ptr);
      else {
         printf (" ** Expected UNTIL or WHILE...REPEAT not found.\n");
         return (NULL);
         }
      }
/*
 *
 * Function                       xit (ptr)
 *
 */
   char *xit (ptr) /* deletes a word definition */
   char *ptr;
   {
      char word [NAMESIZE];
      int ndex;
      make_word (ptr = next (ptr), word);
      if ((ndex = def_member (word)) < 0)
         printf (" %s is not defined as a word.\n", word);
      else {
         free (definition [ndex].list_ptr);
         definition [ndex].list_ptr = NULL;
         printf (" %s forgotten.\n", definition [ndex].dname);
         ndex = vocabulary.count;
         while (!str_cmp (vocabulary.vname [--ndex], word));
         strcpy (vocabulary.vname [ndex], vocabulary.vname [--vocabulary.count]);
         }
      return (ptr);
      }
/*
 *
 * Function                       next (ptr)
 *
 */
   char *next (ptr) /* advances pointer to next word, NULL if none */
   char *ptr;
   {
      if (ptr == NULL) return (NULL);
      while (*ptr >= FIRST_ASCII && *ptr <= LAST_ASCII) ptr++;
      while ((*ptr != '\0') && isspace (*ptr)) ptr++;
      return ((*ptr == '\0') ? NULL : ptr);
      }
/*
 *
 * Function                       prev (ptr)
 *
 */
   char *prev (ptr) /* sets pointer to previous word */
   char *ptr;
   {
      do --ptr;
         while (isspace (*ptr));
      while (*--ptr >= FIRST_ASCII && *ptr <= LAST_ASCII);
      return (++ptr);
      }
/*
 *
 * Function                       open_file (file_name, mode)
 *
 */
   FILE *open_file (file_name, mode)
   char *file_name, *mode;
   {
      FILE *file_ptr, *fopen ();
      if ((file_ptr = fopen (file_name, mode)) == NULL)
         printf (" *** Error opening file %s.\n", file_name);
      return (file_ptr);
      }
/*
 *
 * Function                       save_defs (ptr)
 *
 */
   char *save_defs (ptr)
   char *ptr;
   {
      char file_name [NAMESIZE], *next ();
      FILE *save_file, *open_file ();
      if ((ptr = next (ptr)) == NULL)
         printf (" ** Expected file name after SAVE not found.\n");
      else {
         make_word (ptr, file_name);
         write_defs (save_file = open_file (file_name, "w"));
         if (save_file != NULL) fclose (save_file);
         }
      return (ptr);
      }
/*
 *
 * Function                       write_defs (file_ptr)
 *
 */
   write_defs (file_ptr)
   FILE *file_ptr;
   {
      int ndex, num_defs = 0;
      if (file_ptr != NULL) {
         for (ndex = 0; ndex < DEFSIZE; ndex++)
            if (definition [ndex].list_ptr != NULL) {
               fprintf (file_ptr, ": %s %s ;\n", definition [ndex].dname, definition [ndex].list_ptr);
               num_defs++;
               }
         }
      printf (" %d definitions saved.\n", num_defs);
      }
/*
 *
 * Function                       load_defs (ptr)
 *
 */
   char *load_defs (ptr)
   char *ptr;
   {
      FILE *load_file, *open_file ();
      char file_name [NAMESIZE], *next ();
      if ((ptr = next (ptr)) == NULL)
         printf (" ** Expected file name after LOAD not found.\n");
      else {
         make_word (ptr, file_name);
         read_defs (load_file = open_file (file_name, "r"));
         if (load_file != NULL) fclose (load_file);
         }
      return (ptr);
      }
/*
 *
 * Function                       read_defs (file_ptr)
 *
 */
   read_defs (file_ptr)
   FILE *file_ptr;
   {
      char buffer [BUFSIZE], *fgets ();
      if (file_ptr != NULL)
         while (fgets (buffer, BUFSIZE, file_ptr) != NULL)
            def_code (buffer);
      }
