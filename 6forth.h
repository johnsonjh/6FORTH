#ifndef FORTH_H_
# define FORTH_H_
# include <ctype.h>
# include <setjmp.h>
# include <stdio.h>
# include <string.h>
# ifdef __ELKS__
#  include <math.h>
# endif
# ifdef multics
#  include <types.h>
#  include <values.h>
#  include <memory.h>
# else
#  include <stdlib.h>
# endif
# ifdef ANSI_FUNC
#  ifdef INCLUDE_HELP
static char *help_proc (char *ptr);
static void blank_fill (void);
static void prnt_help (int i);
#  endif
static char *begin_stmt (char *ptr);
static char *def_code (char *ptr);
static char *do_process (char *ptr);
static char *do_word (char *ptr);
static char *if_proc (char *ptr);
static char *jump (char *ptr, char target[], char *(*sequence)(char *));
static char *load_defs (char *ptr);
static char *loop (char *ptr);
static char *next (char *ptr);
static char *prev (char *ptr);
static char *print_list (char *ptr);
static char *safe_gets (char *buf, unsigned long buf_size);
static char *save_defs (char *ptr);
static char *var_process (char *ptr, char word[], int var_ndx);
static char *xit (char *ptr);
static double pop_ndx (void);
static double pop (void);
static double push (double value);
static double push_ndx (double ndx_value);
static FILE *open_file (char *file_name, char *mode);
static int def_member (char word[]);
static int match_vocab (char word[]);
static int numeric (char *word);
static int str_cmp (char *str1, char *str2);
static int var_member (char word[]);
static void def_var (char *ptr);
static void do_list (char *ptr);
static void input (void);
static void list_defs (void);
static void make_word (char *ptr, char word[]);
static void read_defs (FILE *file_ptr);
static void setup (void);
static void tiny_forth (void);
static void title (void);
static void try_logic (int function);
static void write_defs (FILE *file_ptr);
# else
#  ifdef INCLUDE_HELP
static char *help_proc ();
static void blank_fill ();
static void prnt_help ();
#  endif
static char *begin_stmt ();
static char *def_code ();
static char *do_process ();
static char *do_word ();
static char *if_proc ();
static char *jump ();
static char *load_defs ();
static char *loop ();
static char *next ();
static char *prev ();
static char *print_list ();
static char *safe_gets ();
static char *save_defs ();
static char *var_process ();
static char *xit ();
static double pop_ndx ();
static double pop ();
static double push ();
static double push_ndx ();
static FILE *open_file ();
static int def_member ();
static int match_vocab ();
static int numeric ();
static int str_cmp ();
static int var_member ();
static void def_var ();
static void do_list ();
static void input ();
static void list_defs ();
static void make_word ();
static void read_defs ();
static void setup ();
static void tiny_forth ();
static void title ();
static void try_logic ();
static void write_defs ();
# endif
#endif
