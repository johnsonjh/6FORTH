/**************************************************************************************************/

/*
 * 6FORTH
 * Copyright (c) 1989 Bull HN Information Systems Inc.
 *
 * This program is a revision of Tiny FORTH written by David Malmberg,
 * published in 'Powerplay', February/March 1985, written in LOGO.
 *
 * For ideas, information, comments, errors, ..., concerning this program,
 * please contact: Kathy Larkin LCPD (SDTC 1-523) HVN 862-4204
 */

/**************************************************************************************************/

/*
 * Revision 2.0 (5/16/85 KEL)
 *  >  Added HELP function.
 *  >  Added EMIT function.
 *  >  Changed some ugly stuff.
 *
 * Revision 2.1 (5/21/85 KEL)
 *  >  Added SAVE and LOAD functions.
 *
 * Revision A01 (6/7/85 KEL)
 *  >  Changed greeting and version number
 *  >  Submitted to LADC X account
 *
 * Revision A02 (5/28/2025 JHJ)
 *  >  Rename to 6FORTH
 *  >  ANSIfication, safety, and portability
 *
 * Revision A03 (10/??/2025 JHJ)
 *  >  Support for ELKS
 *  >  Improve portability
 */

/**************************************************************************************************/

#define INCLUDE_HELP

/**************************************************************************************************/

#include "6forth.h"

/**************************************************************************************************/

#define BUFSIZE      (384)
#define DEFSIZE      (100)
#define EMPTY          (0)
#define FALSE_       (0)
#define FIRST_ASCII  ('!')
#define INITVOCABSIZE (48)
#define LAST_ASCII   ('z')
#define NAMESIZE      (32)
#define NDXSTACKSIZE  (20)
#define NOT_FOUND     (-1)
#define STACKSIZE    (100)
#define TRUE_        (1)
#define VARSIZE      (100)
#define VOCABSIZE    (148)

/**************************************************************************************************/

struct Deflist
{
  char dname[NAMESIZE];
  char *list_ptr;
};

static struct Deflist definition[DEFSIZE];

/**************************************************************************************************/

struct Vartable
{
  char name[NAMESIZE];
  cell_t vvalue;
};

struct Varstruct
{
  int count;
  struct Vartable vartbl[VARSIZE];
};

static struct Varstruct variable;

/**************************************************************************************************/

struct Vocablist
{
  int count;
  char vname[VOCABSIZE][NAMESIZE];
};

static struct Vocablist vocabulary =
{ 0,
  { "+",      "-",    "*",     "/",        "=",        ">",
    "<",      "MIN",  "MAX",   "0=",       "0>",       "0<",
    "ABS",    "NOT",  ".",     ".\"",      "DROP",     "DO",
    "LOOP",   "I",    "LEAVE", "BEGIN",    "UNTIL",    "WHILE",
    "REPEAT", "DUP",  "SWAP",  "ABORT",    "COLD",     "LIST",
    "CR",     "@",    "!",     "VARIABLE", ":",         ";",
    "VLIST",  "IF",   "THEN",  "ELSE",     "FORGET",   "OVER",
    "EMIT",   "SAVE", "LOAD",  "QUIT",
#ifdef INCLUDE_HELP
    "HELP", "?"
#endif
  }
};

/**************************************************************************************************/

struct Stacktype
{
  int top;
  cell_t value[STACKSIZE];
};

static struct Stacktype stack;
static struct Stacktype ndx_stack;

/**************************************************************************************************/

static char in_buf[BUFSIZE];
static cell_t number;
static int forever = 1;
static jmp_buf cold;

/**************************************************************************************************/

#ifdef ANSI_FUNC
int
main (void)
#else
int
main ()
#endif
{
#ifdef __TURBOC__
  static double __ignore__;
  __ignore__ = sqrt (23);
#endif
  (void)setjmp (cold);
  title ();
  tiny_forth ();

  return 0;
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static void
tiny_forth (void)
#else
static void
tiny_forth ()
#endif
{
  setup ();
  input ();
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static void
title (void)
#else
static void
title ()
#endif
{ /* prints FORTH titles */
  (void)printf ("6FORTH A03 here.\n");
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static void
setup (void)
#else
static void
setup ()
#endif
{ /* performs initialization tasks */
  int i;

  vocabulary.count = INITVOCABSIZE;
  stack.top = ndx_stack.top = 0;
  variable.count = 0;

  for (i = 0; i < DEFSIZE; definition[i++].list_ptr = NULL);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static cell_t
push (cell_t value)
#else
static cell_t
push (value)
cell_t value;
#endif
{ /* pushes value onto stack */
  /* TODO:  If the stack overflows, we should handle it somehow */
  if (stack.top == STACKSIZE)
    (void)printf (" *** Stack overflow!\n");
  else
    stack.value[stack.top++] = value;

  return (value);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static cell_t
pop (void)
#else
static cell_t
pop ()
#endif
{ /* pops top value off of the stack */
  /* TODO: If the stack underflows, optionally handle it better than 0.0 */
  if (stack.top == EMPTY) {
    (void)printf (" *** Attempting to pop empty stack!\n");
    return (0);
  } else
    return (stack.value[--stack.top]);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
safe_gets(char *buf, unsigned long buf_size)
#else
static char *
safe_gets(buf, buf_size)
char *buf;
unsigned long buf_size;
#endif
{
  unsigned long len;
  char *src, *dst;
  unsigned char ch;

  while (forever) {
    if (fgets(buf, (int)buf_size, stdin) == NULL)
      return NULL;
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
      buf[len - 1] = '\0';
    src = buf;
    dst = buf;
    while (*src) {
      ch = (unsigned char)*src;
      if (ch >= 32 && ch <= 126)
        *dst++ = *src;
      src++;
    }
    *dst = '\0';
    if (buf[0] == '\0')
      continue;
    return buf;
  }

  return NULL;
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static void
input (void)
#else
static void
input ()
#endif
{ /* gets and processes user input */
  char *ptr;

  while (forever) {
    ptr = safe_gets (in_buf, (unsigned long)sizeof (in_buf));
    if (ptr == NULL || *ptr == EOF)
      exit (0);
    while (isspace (*ptr))
      ptr++;
    if (*ptr != '\0')
      do_list (ptr);
  }
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
next (char *ptr)
#else
static char *
next (ptr)
char *ptr;
#endif
{ /* advances pointer to next word, NULL if none */
  if (ptr == NULL)
    return (NULL);

  while (*ptr >= FIRST_ASCII && *ptr <= LAST_ASCII)
    ptr++;

  while ((*ptr != '\0') && isspace (*ptr))
    ptr++;

  return ((*ptr == '\0') ? NULL : ptr);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static void
do_list (char *ptr)
#else
static void
do_list (ptr)
char *ptr;
#endif
{ /* processes line of input */
  while (ptr != NULL)
    ptr = next (do_word (ptr));
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static void
write_defs (FILE *file_ptr)
#else
static void
write_defs (file_ptr)
FILE *file_ptr;
#endif
{
  int ndex, num_defs = 0;

  if (file_ptr != NULL) {
    for (ndex = 0; ndex < DEFSIZE; ndex++)
      if (definition[ndex].list_ptr != NULL) {
        (void)fprintf (file_ptr, ": %s %s ;\n", definition[ndex].dname, definition[ndex].list_ptr);
        num_defs++;
      }
  }

  (void)printf (" %d definitions saved.\n", num_defs);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static void
read_defs (FILE *file_ptr)
#else
static void
read_defs (file_ptr)
FILE *file_ptr;
#endif
{
  char buffer[BUFSIZE];

  if (file_ptr != NULL)
    while (fgets (buffer, BUFSIZE, file_ptr) != NULL)
      (void)def_code (buffer);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
load_defs (char *ptr)
#else
static char *
load_defs (ptr)
char *ptr;
#endif
{
  FILE *load_file;
  char file_name[NAMESIZE];

  if ((ptr = next (ptr)) == NULL)
    (void)printf (" ** Expected file name after LOAD not found.\n");
  else {
    make_word (ptr, file_name);
    read_defs (load_file = open_file (file_name, "r"));
    if (load_file != NULL)
      (void)fclose (load_file);
  }

  return (ptr);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
save_defs (char *ptr)
#else
static char *
save_defs (ptr)
char *ptr;
#endif
{
  char file_name[NAMESIZE];
  FILE *save_file;

  if ((ptr = next (ptr)) == NULL)
    (void)printf (" ** Expected file name after SAVE not found.\n");
  else {
    make_word (ptr, file_name);
    write_defs (save_file = open_file (file_name, "w"));
    if (save_file != NULL)
      (void)fclose (save_file);
  }

  return (ptr);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
do_word (char *ptr)
#else
static char *
do_word (ptr)
char *ptr;
#endif
{ /* processes one word of input */
  int i;
  char word[NAMESIZE];
  cell_t x, y;
  make_word (ptr, word);
  switch (i = match_vocab (word))
  {
  case /*     +    */ (0):
    (void)push (pop () + pop ());
    break;
  case /*     -    */ (1):
    x = pop ();
    (void)push (pop () - x);
    break;
  case /*     *    */ (2):
    (void)push (pop () * pop ());
    break;
  case /*     /    */ (3):
    y = pop ();
    if (y == 0)
      (void)printf(" *** Division by zero!\n");
    else
      (void)push (pop () / y);
    break;
  case /*     =    */ (4):
  case /*     >    */ (5):
  case /*     <    */ (6):
  case /*    MIN   */ (7):
  case /*    MAX   */ (8):
  case /*    0=    */ (9):
  case /*    0>    */ (10):
  case /*    0<    */ (11):
  case /*    ABS   */ (12):
  case /*    NOT   */ (13):
    try_logic (i - 3);
    break;
  case /*     .    */ (14):
    (void)printf (" %ld", pop ());
    break;
  case /*    ."    */ (15):
    ptr = print_list (ptr);
    break;
  case /*   DROP   */ (16):
    (void)pop ();
    break;
  case /*    DO    */ (17):
    ptr = do_process (ptr);
    break;
  case /*   LOOP   */ (18):
    ptr = loop (ptr);
    break;
  case /*     I    */ (19):
    (void)push (push_ndx (pop_ndx ()));
    break;
  case /*   BEGIN  */ (21):
    ptr = begin_stmt (ptr);
    break;
  case /*   UNTIL  */ (22):
    if (!(pop () == TRUE_))
      ptr = jump (ptr, "BEGIN", prev);
    break;
  case /*   WHILE  */ (23):
    if (pop () == FALSE_)
      ptr = jump (ptr, "REPEAT", next);
    break;
  case /*  REPEAT  */ (24):
    ptr = jump (ptr, "BEGIN", prev);
    break;
  case /*    DUP   */ (25):
    (void)push (push (pop ()));
    break;
  case /*   SWAP   */ (26):
    x = pop ();
    y = pop ();
    (void)push (x);
    (void)push (y);
    break;
  case /*   ABORT  */ (27):
    stack.top = ndx_stack.top = EMPTY;
    break;
  case /*   COLD   */ (28):
    longjmp(cold, 0);
    /*NOTREACHED*/ /* unreachable */ /* not reached */
    break;
  case /*   LIST   */ (29):
    list_defs ();
    break;
  case /*    CR    */ (30):
    (void)printf ("\n");
    break;
  case /*     @    */ (31):
  case /*     !    */ (32):
    (void)printf (" ** Expected variable name before %s not found.\n", word);
    break;
  case /* VARIABLE */ (33):
    def_var (ptr = next (ptr));
    break;
  case /*     :    */ (34):
    ptr = def_code (ptr);
    break;
  case /*     ;    */ (35):
    (void)printf (" ** Syntax error.");
#ifdef INCLUDE_HELP
    (void)printf ("  Type HELP ; for more information.\n");
#endif
    break;
  case /*   VLIST  */ (36):
    for (i = 0; i < vocabulary.count;
         (void)printf ("%s ", vocabulary.vname[i++]));
    (void)printf ("\n");
    break;
  case /*     IF   */ (37):
    ptr = if_proc (ptr);
    break;
  case /*    THEN  */ (38):
    break;
  case /*    ELSE  */ (39):
    ptr = jump (ptr, "THEN", next);
    break;
  case /*   FORGET */ (40):
    ptr = xit (ptr);
    break;
  case /*    OVER  */ (41):
    x = pop ();
    (void)push (y = pop ());
    (void)push (x);
    (void)push (y);
    break;
  case /*    EMIT  */ (42):
    (void)putc ((int)pop (), stdout);
    break;
  case /*    SAVE  */ (43):
    ptr = save_defs (ptr);
    break;
  case /*    LOAD  */ (44):
    ptr = load_defs (ptr);
    break;
  case /*    QUIT  */ (45):
    exit (0);
#ifdef INCLUDE_HELP
  case /*    HELP  */ (46):
    ptr = help_proc (ptr);
    break;
  case /*     ?    */ (47):
    (void)printf (" Type HELP ? for syntax information.\n");
    break;
#endif
  default:
    if (numeric (word))
      (void)push (number);
    else if ((i = def_member (word)) >= 0)
      do_list (definition[i].list_ptr);
    else if ((i = var_member (word)) >= 0)
      ptr = var_process (ptr, word, i);
    else
      (void)printf (" %s has not been defined yet!\n", word);
  }

  return (ptr);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static int
numeric (char *word)
#else
static int
numeric (word)
char *word;
#endif
{ /* sets global number and returns TRUE_ if word is numeric */
  char *num_ptr;

  number = strtol (word, &num_ptr, 10);

  return ((num_ptr == word) ? (int)(FALSE_) : (int)(TRUE_));
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static int
str_cmp (char *str1, char *str2)
#else
static int
str_cmp (str1, str2)
char *str1, *str2;
#endif
{
  char *p1, *p2;

  p1 = str1;
  p2 = str2;

  if (p1 == NULL || p2 == NULL) {
    return (p1 == NULL && p2 == NULL) ? (int)(TRUE_) : (int)(FALSE_);
  }

  while (*p1 != '\0' && *p2 != '\0') {
    if (*p1 != *p2)
      return (int)(FALSE_);
    p1++;
    p2++;
  }

  return (int)(*p1 == *p2);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static void
make_word (char *ptr, char word[])
#else
static void
make_word (ptr, word)
char *ptr, word[];
#endif
{ /* moves word pointed at by ptr into word and ends with \0 */
  int i = 0;

  word[0] = '\0';

  if (ptr == NULL)
    return;

  while ((word[i++] = ((*ptr >= FIRST_ASCII) && (*ptr <= LAST_ASCII)) ? *ptr++ : '\0') != '\0');
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static int
match_vocab (char word[])
#else
static int
match_vocab (word)
char word[];
#endif
{ /* returns index of vocabulary item which matches word */
  int i;

  for (i = 0; i < INITVOCABSIZE && !str_cmp (word, vocabulary.vname[i]); i++);

  return (i);
}

/**************************************************************************************************/

#ifdef INCLUDE_HELP
# ifdef ANSI_FUNC
static char *
help_proc (char *ptr)
# else
static char *
help_proc (ptr)
char *ptr;
# endif
{ /* processes HELP request */
  int i;
  char word[NAMESIZE];

  if ((ptr = next (ptr)) == NULL) {
    (void)printf (" ** Expected vocabulary word after HELP not found.\n");
    (void)printf ("    Type HELP HELP for more information.\n");
  } else {
    make_word (ptr, word);
    if (str_cmp (word, "?"))
      for (i = 0; i < INITVOCABSIZE; i++)
        switch (i) /* eliminate duplicates */
        {
        case (17): /* DO LOOP */
        case (21):
        case (23):
        case (24): /* BEGIN UNTIL WHILE REPEAT */
        case (34): /* : ; */
        case (37):
        case (39): /* IF THEN ELSE */
        case (46):
          break;
        default:   /* HELP */
          prnt_help (i);
        }
    else
      prnt_help (match_vocab (word));
  }

  return (ptr);
}
#endif

/**************************************************************************************************/

#ifdef INCLUDE_HELP
# ifdef ANSI_FUNC
static void
prnt_help (int i)
# else
static void prnt_help (i) int i;
# endif
{ /* Prints help information for the ith vocabulary word */
  switch (i)
  {
  case (0):
    (void)printf (" +                      (n2 n1 -- sum)       ");
    (void)printf ("Adds two top numbers.\n");
    break;
  case (1):
    (void)printf (" -                      (n2 n1 -- diff)      ");
    (void)printf ("Subtracts n1 from n2.\n");
    break;
  case (2):
    (void)printf (" *                      (n2 n1 -- prod)      ");
    (void)printf ("Multiplies two top numbers.\n");
    break;
  case (3):
    (void)printf (" /                      (n2 n1 -- quot)      ");
    (void)printf ("Divides n2 by n1.\n");
    break;
  case (4):
    (void)printf (" =                      (n2 n1 -- flag)      ");
    (void)printf ("True if top two numbers are equal.\n");
    break;
  case (5):
    (void)printf (" >                      (n2 n1 -- flag)      ");
    (void)printf ("True if n2 greater than n1.\n");
    break;
  case (6):
    (void)printf (" <                      (n2 n1 -- flag)      ");
    (void)printf ("True if n2 less than n1.\n");
    break;
  case (7):
    (void)printf (" MIN                    (n2 n1 -- min)       ");
    (void)printf ("Leaves lesser of two top numbers.\n");
    break;
  case (8):
    (void)printf (" MAX                    (n2 n1 -- max)       ");
    (void)printf ("Leaves greater of two top numbers.\n");
    break;
  case (9):
    (void)printf (" 0=                     (n -- flag)          ");
    (void)printf ("True if top number is zero.\n");
    break;
  case (10):
    (void)printf (" 0>                     (n -- flag)          ");
    (void)printf ("True if top number is positive.\n");
    break;
  case (11):
    (void)printf (" 0<                     (n -- flag)          ");
    (void)printf ("True if top number is negative.\n");
    break;
  case (12):
    (void)printf (" ABS                    (n -- absolute)      ");
    (void)printf ("Gives absolute value of top number.\n");
    break;
  case (13):
    (void)printf (" NOT                    (flag1 -- flag2)     ");
    (void)printf ("Reverses value of flag.\n");
    break;
  case (14):
    (void)printf (" .                      (n -- )              ");
    (void)printf ("Prints top of stack.\n");
    break;
  case (15):
    (void)printf (" .\"                     ( -- )               ");
    (void)printf ("Prints message until \" mark.\n");
    break;
  case (16):
    (void)printf (" DROP                   (n -- )              ");
    (void)printf ("Discards top number.\n");
    break;
  case (17):
  case (18):
    (void)printf (" DO...LOOP              (end+1 start -- )    ");
    (void)printf ("Performs loop given index range.\n");
    break;
  case (19):
    (void)printf (" I                      ( -- index)          ");
    (void)printf ("Puts current loop index on stack.\n");
    break;
  case (20):
    (void)printf (" LEAVE                  ( -- )               ");
    (void)printf ("Terminates loop at next LOOP.\n");
    break;
  case (21):
  case (22):
    (void)printf (" BEGIN...UNTIL          (flag -- )           ");
    (void)printf ("Loops back to BEGIN until flag\n");
    blank_fill ();
    (void)printf ("tested at UNTIL is true.\n");
    /*FALLTHRU*/ /* fall through */ /* fallthrough */
  case (23):
  case (24):
    (void)printf (" BEGIN...WHILE...REPEAT (flag -- )           ");
    (void)printf ("Tests flag at WHILE and jumps past\n");
    blank_fill ();
    (void)printf ("REPEAT if false.  REPEAT causes\n");
    blank_fill ();
    (void)printf ("unconditional jump to BEGIN.\n");
    break;
  case (25):
    (void)printf (" DUP                    (n -- nn)            ");
    (void)printf ("Duplicates the top number.\n");
    break;
  case (26):
    (void)printf (" SWAP                   (n2 n1 -- n1 n2)     ");
    (void)printf ("Exchanges two top numbers.\n");
    break;
  case (27):
    (void)printf (" ABORT                  ( -- )               ");
    (void)printf ("Clears stack and DO...LOOP indices.\n");
    break;
  case (28):
    (void)printf (" COLD                   ( -- )               ");
    (void)printf ("Restarts FORTH from scratch.\n");
    break;
  case (29):
    (void)printf (" LIST                   ( -- )               ");
    (void)printf ("Prints definitions of all new\n");
    blank_fill ();
    (void)printf ("FORTH words.\n");
    break;
  case (30):
    (void)printf (" CR                     ( -- )               ");
    (void)printf ("Prints a carriage return.\n");
    break;
  case (31):
    (void)printf (" xxx @                  ( -- n)              ");
    (void)printf ("Puts the current value of variable\n");
    blank_fill ();
    (void)printf ("xxx on the stack.\n");
    break;
  case (32):
    (void)printf (" xxx !                  (n -- )              ");
    (void)printf ("Sets the value of the variable xxx\n");
    blank_fill ();
    (void)printf ("to the top of the stack.\n");
    break;
  case (33):
    (void)printf (" VARIABLE xxx           (n -- )              ");
    (void)printf ("Creates a variable named xxx with\n");
    blank_fill ();
    (void)printf ("initial value equal to the top of\n");
    blank_fill ();
    (void)printf ("the stack.\n");
    break;
  case (34):
  case (35):
    (void)printf (" : xxx ... ;            ( -- )               ");
    (void)printf ("Defines new FORTH word named xxx.\n");
    break;
  case (36):
    (void)printf (" VLIST                  ( -- )               ");
    (void)printf ("Prints vocabulary list of current\n");
    blank_fill ();
    (void)printf ("FORTH words.\n");
    break;
  case (37):
  case (38):
    (void)printf (" IF...THEN              (flag -- )           ");
    (void)printf ("Executes words between IF and THEN\n");
    blank_fill ();
    (void)printf ("if flag is true.\n");
    /*FALLTHRU*/ /* fall through */ /* fallthrough */
  case (39):
    (void)printf (" IF...ELSE...THEN       (flag -- )           ");
    (void)printf ("Executes words between IF and ELSE\n");
    blank_fill ();
    (void)printf ("if flag is true and words between\n");
    blank_fill ();
    (void)printf ("ELSE and THEN if flag is false.\n");
    break;
  case (40):
    (void)printf (" FORGET xxx             ( -- )               ");
    (void)printf ("Erases the current definition of\n");
    blank_fill ();
    (void)printf ("FORTH word xxx.\n");
    break;
  case (41):
    (void)printf (" OVER                   (n2 n1 -- n2 n1 n2)  ");
    (void)printf ("Puts copy of 2nd number on top.\n");
    break;
  case (42):
    (void)printf (" EMIT                   (n -- )              ");
    (void)printf ("Outputs top of stack as character.\n");
    break;
  case (43):
    (void)printf (" SAVE xxx               ( -- )               ");
    (void)printf ("Saves current definitions in file\n");
    blank_fill ();
    (void)printf ("xxx (see LOAD command).\n");
    break;
  case (44):
    (void)printf (" LOAD xxx               ( -- )               ");
    (void)printf ("Restores SAVEd definitions from\n");
    blank_fill ();
    (void)printf ("file xxx.\n");
    break;
  case (45):
    (void)printf (" QUIT                   ( -- )               ");
    (void)printf ("Exits the FORTH interpreter.\n");
    break;
  case (46):
  case (47):
    (void)printf (" HELP xxx               ( -- )               ");
    (void)printf ("Prints info on topic xxx.\n");
    (void)printf (" HELP ?                 ( -- )               ");
    (void)printf ("Prints info on all topics.\n");
    break;
  default:
    (void)printf (" ** No HELP available on that topic.\n");
    (void)printf ("    Type HELP HELP for syntax information.\n");
    break;
  }
}
#endif

/**************************************************************************************************/

#ifdef INCLUDE_HELP
# ifdef ANSI_FUNC
static void
blank_fill (void)
# else
static void
blank_fill ()
# endif
{ /* outputs 45 blanks */
  int i;

  for (i = 0; i < 45; i++)
    (void)putc (' ', stdout);
}
#endif

/**************************************************************************************************/

#ifdef ANSI_FUNC
static void
list_defs (void)
#else
static void
list_defs ()
#endif
{ /* lists current definitions */
  int ndex, no_defs_found;

  for (ndex = 0, no_defs_found = (int)(TRUE_); ndex < DEFSIZE; ndex++)
    if (definition[ndex].list_ptr) {
      (void)printf (" %s: %s\n", definition[ndex].dname, definition[ndex].list_ptr);
      no_defs_found = (int)(FALSE_);
    }
  if (no_defs_found)
    (void)printf (" No new FORTH words defined.\n");
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static int
def_member (char word[])
#else
static int
def_member (word)
char word[];
#endif
{ /* tries to match word with a current definition */
  int def_ndx = 0;

  while (def_ndx < DEFSIZE)
    if (definition[def_ndx].list_ptr
        && (str_cmp (word, definition[def_ndx].dname)))
      return (def_ndx);
    else
      def_ndx++;

  return (NOT_FOUND);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
def_code (char *ptr)
#else
static char *
def_code (ptr)
char *ptr;
#endif
{ /* creates a word definition */
  int i;
  char *end_ptr;

  /* TODO: Sane behavior if word already exists as a variable or builtin word;
           For redefintions the old defintion is not forgotten and freed first */
  if ((end_ptr = strchr (ptr, ';')) == NULL)
    (void)printf (" ** No ending ; found in definition.\n");
  else {
    for (i = 0; (i < DEFSIZE) && (definition[i].list_ptr); i++);
    if (i >= DEFSIZE)
      (void)printf (" ** Out of definition space!\n");
    else {
      make_word (ptr = next (ptr), definition[i].dname);
      (void)printf (" %s is now defined as a word.\n", definition[i].dname);
      (void)strcpy (vocabulary.vname[vocabulary.count++], definition[i].dname);
      *(end_ptr - 1) = '\0';
      definition[i].list_ptr = malloc (sizeof (in_buf));
      if (definition[i].list_ptr == NULL) {
        (void)printf (" ** Memory exhaused!\n");
      } else {
        if (next (ptr) != NULL)
          (void)strcpy (definition[i].list_ptr, next (ptr));
      }
    }
  }

  return (end_ptr);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
char *
xstrstr (char *haystack, char *needle)
#else
char *
xstrstr (haystack, needle)
char *haystack;
char *needle;
#endif
{
  char *h, *n;

  if (!*needle)
    return haystack;

  while (*haystack)
    {
      if (*haystack == *needle)
        {
          h = haystack;
          n = needle;

          while (*h && *n && *h == *n)
	    {
              h++;
              n++;
            }

          if (!*n)
            return haystack;
        }

      haystack++;
    }

  return (char *)0;
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
if_proc (char *ptr)
#else
static char *
if_proc (ptr)
char *ptr;
#endif
{ /* processes IF...THEN and IF...ELSE...THEN statements */
  char *target_ptr = ptr;

  if (pop () == FALSE_)
    if ((target_ptr = xstrstr (ptr, "ELSE")) == NULL)
      if ((target_ptr = xstrstr (ptr, "THEN")) == NULL)
        (void)printf (" ** Expected ELSE or THEN not found.\n");

  return (target_ptr);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static int
var_member (char word[])
#else
static int
var_member (word)
char word[];
#endif
{ /* tries to match word with a variable definition */
  int var_ndx = 0;

  while (var_ndx < variable.count)
    if (str_cmp (word, variable.vartbl[var_ndx].name))
      return (var_ndx);
    else
      var_ndx++;

  return (NOT_FOUND);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
var_process (char *ptr, char word[], int var_ndx)
#else
static char *
var_process (ptr, word, var_ndx)
char *ptr, word[];
int var_ndx;
#endif
{ /* creates a variable definition */
  make_word (ptr = next (ptr), word);

  if (str_cmp (word, "@"))
    (void)push (variable.vartbl[var_ndx].vvalue);
  else if (str_cmp (word, "!"))
    variable.vartbl[var_ndx].vvalue = pop ();
  else
    (void)printf (" ** Expected @ or ! after variable %s not found.\n",
                  variable.vartbl[var_ndx].name);

  return (ptr);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
do_process (char *ptr)
#else
static char *
do_process (ptr)
char *ptr;
#endif
{ /* processes DO...LOOP statements */
  cell_t ndx_start;

  if (xstrstr (ptr, "LOOP") == NULL) {
    (void)printf (" ** DO without ending LOOP.\n");
    return (NULL);
  } else {
    ndx_start = pop ();
    (void)push_ndx (pop ());
    (void)push_ndx (ndx_start);
    return (ptr);
  }
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static cell_t
push_ndx (cell_t ndx_value)
#else
static cell_t
push_ndx (ndx_value)
cell_t ndx_value;
#endif
{ /* pushes ndx_value onto ndx_stack */
  if (ndx_stack.top == NDXSTACKSIZE)
    (void)printf (" *** Index stack overflow!\n");
  else
    ndx_stack.value[ndx_stack.top++] = ndx_value;

  return (ndx_value);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static cell_t
pop_ndx (void)
#else
static cell_t
pop_ndx ()
#endif
{ /* pops ndx_stack */
  if (ndx_stack.top == EMPTY) {
    (void)printf (" *** Index stack underflow!\n");
    return (0);
  } else
    return (ndx_stack.value[--ndx_stack.top]);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
loop (char *ptr)
#else
static char *
loop (ptr)
char *ptr;
#endif
{ /* processes DO...LOOP iteration */
  cell_t loop_cur = pop_ndx () + 1, loop_end;

  if ((loop_end = pop_ndx ()) != loop_cur) {
    (void)push_ndx (loop_end);
    (void)push_ndx (loop_cur);
    ptr = jump (ptr, "DO", prev);
  }

  return (ptr);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
jump (char *ptr, char target[], char *(*sequence) (char *))
#else
static char *
jump (ptr, target, sequence)
char *ptr, target[], *(*sequence)();
#endif
{ /* returns a pointer to target */
  char word[NAMESIZE];

  do make_word (ptr = (*sequence) (ptr), word);
  while (!str_cmp (word, target));

  return (ptr);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static void
try_logic (int function)
#else
static void try_logic (function) int function;
#endif
{ /* processes logic function statements */
  cell_t z2 = pop (), z1 = 0;

  if (function < 6)
    z1 = pop ();
  switch (function)
  {
  case (1):
    (void)push ((z1 == z2) ? TRUE_ : FALSE_);
    break;
  case (2):
    (void)push ((z1 > z2) ? TRUE_ : FALSE_);
    break;
  case (3):
    (void)push ((z1 < z2) ? TRUE_ : FALSE_);
    break;
  case (4):
    (void)push ((z1 <= z2) ? z1 : z2);
    break;
  case (5):
    (void)push ((z1 >= z2) ? z1 : z2);
    break;
  case (10):
    /* same as case (6) */
  case (6):
    (void)push ((z2 == 0) ? TRUE_ : FALSE_);
    break;
  case (7):
    (void)push ((z2 > 0) ? TRUE_ : FALSE_);
    break;
  case (8):
    (void)push ((z2 < 0) ? TRUE_ : FALSE_);
    break;
  case (9):
    (void)push ((z2 < 0) ? -z2 : z2);
    break;
  default:
    (void)printf ("*** Internal error: invalid function %d\n", function);
    break;
  }
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
print_list (char *ptr)
#else
static char *
print_list (ptr)
char *ptr;
#endif
{ /* processes ." (print) statement */
  char *end_quote;

  if (ptr == NULL || strlen(ptr) < 3) {
    (void)printf (" ** Invalid input string.\n");
    return (NULL);
  }

  ptr += 3;

  end_quote = strchr (ptr, '\"');
  if (end_quote == NULL) {
    (void)printf (" ** Ending \" not found.\n");
    return (NULL);
  }

  while (ptr < end_quote) {
    (void)putc(*ptr++, stdout);
  }

  return end_quote + 1;
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static void
def_var (char *ptr)
#else
static void
def_var (ptr)
char *ptr;
#endif
{ /* sets up new variable definition */
  /* TODO: Sane behavior if variable already exists as a variable or word */
  if (variable.count >= VARSIZE)
    (void)printf (" ** Out of variable space!\n");
  else {
    make_word (ptr, variable.vartbl[variable.count].name);
    variable.vartbl[variable.count].vvalue = pop ();
    (void)printf (" %s is now defined as a variable.\n", variable.vartbl[variable.count++].name);
  }
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
begin_stmt (char *ptr)
#else
static char *
begin_stmt (ptr)
char *ptr;
#endif
{ /* checks syntax of BEGIN...UNTIL or BEGIN...WHILE...REPEAT statement */
  if ((xstrstr (ptr, "UNTIL") != NULL) ||
      ((xstrstr (ptr, "WHILE") != NULL) && (xstrstr (ptr, "REPEAT") != NULL)))
    return (ptr);
  else {
    (void)printf (" ** Expected UNTIL or WHILE...REPEAT not found.\n");
    return (NULL);
  }
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
xit (char *ptr)
#else
static char *
xit (ptr)
char *ptr;
#endif
{ /* deletes a word definition */
  char word[NAMESIZE];
  int ndex;
  unsigned long len;
  char *src;

  make_word (ptr = next (ptr), word);

  if ((ndex = def_member (word)) < 0)
    (void)printf (" %s is not defined as a word.\n", word);
  else {
    free (definition[ndex].list_ptr);
    definition[ndex].list_ptr = NULL;
    (void)printf (" %s forgotten.\n", definition[ndex].dname);
    ndex = vocabulary.count;
    while (!str_cmp (vocabulary.vname[--ndex], word));
    src = vocabulary.vname[--vocabulary.count];
    len = strlen(src) + 1;
    (void)memmove(vocabulary.vname[ndex], src, len);
  }

  return (ptr);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static char *
prev (char *ptr)
#else
static char *
prev (ptr)
char *ptr;
#endif
{ /* sets pointer to previous word */
  do --ptr;
  while (isspace (*ptr));
  while (*--ptr >= FIRST_ASCII && *ptr <= LAST_ASCII);

  return (++ptr);
}

/**************************************************************************************************/

#ifdef ANSI_FUNC
static FILE *
open_file (char *file_name, char *mode)
#else
static FILE *
open_file (file_name, mode)
char *file_name, *mode;
#endif
{
  FILE *file_ptr;

  if ((file_ptr = fopen (file_name, mode)) == NULL)
    (void)printf (" *** Error opening file %s.\n", file_name);

  return (file_ptr);
}

/**************************************************************************************************/
