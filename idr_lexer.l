%{

#undef input
#undef lex_input
#undef unput
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "UserInterface.hpp"
//#include "RefEntityName.hpp"
//#include "PickMouseTool.hpp"
#include "CommandHandler.hpp"
#include "idr.hpp"

extern int echo;
extern void include_message   PROTO((const char* var));
extern char if_result;
	
symrec *putsym(const char *, int);
symrec *getsym(const char *);

void print_token PROTO(( const char*, const char*, const char* ));
void unput_string PROTO((const char*, Integer));
inline void reset_lex_state();
void conv_string PROTO((char *string));
const char *pathopen PROTO((const char *file));

static void finalize_putback();
static void do_manual_journal();

#ifdef FLEX_SCANNER
  #define YY_BREAK
    // Override where input comes from
  static int redefined_yy_input(char* buf, int max_size);
  #ifdef YY_INPUT
    #undef YY_INPUT
  #endif
  #define YY_INPUT(buffer, result, max_size)\
   result = redefined_yy_input(buffer, max_size)
  
    // Override how to unput
  void putback(char c);
  #define unput(c)  putback(c)
  
    // Setup the extra variables needed to putback
  static const unsigned short MAX_PUTBACK_STACK = 4;
  static YY_BUFFER_STATE putback_stack[MAX_PUTBACK_STACK];
  static unsigned short putback_stack_index = 0;
  static const unsigned short PUTBACK_BUFFER_SIZE = 100;
  static char putback_buffer[PUTBACK_BUFFER_SIZE+1];
  static unsigned short putback_index = PUTBACK_BUFFER_SIZE;
#else
  #define input()    (UserInterface::instance()->get_input())
  #define lex_input()    (UserInterface::instance()->get_input())
  #define unput(c)   (UserInterface::instance()->unget_input(c))
#endif

static Integer oldState = 0; // State from which PARSING state was entered.
/*Integer yyLineNumber = 1;*/
static Integer keyword_id;
static Integer identifier_id;
static Integer last_keyword_id;
static Integer last_identifier_id;

#ifdef USING_DFPARSER
#include "DFParser.hpp"
static int operatorMatchIndex = -1;
#endif

int info_msg = TRUE;
int warning_msg = TRUE;

#define LINENUM_INC	UserInterface::instance()->currentFile->lineNumber++
#define MAX_IF_NESTING 64
/* Treated similar to a start state.*/
#define IF_WHILE_SKIP -1

/* Global variables used by the looping mechanism */
int loop_lvl = 0;
int current_loop_count = 0;
FILE *tmp_file;

#ifndef LINUX
char  *temp_f;
#endif

/* Global variables used by ifdef mechanism */
const unsigned char APP_IFDEF = 0;
const unsigned char APP_IFNOTDEF = 1;
const unsigned char APP_IF = 2;
unsigned char app_if_type;
int if_state[MAX_IF_NESTING];
int if_lvl = 0;
static unsigned char journal_skip_lvl = 0;
static unsigned char journal_line_skip = 0;
%}

%p 5000
%a 4000
%n 1000
%e 2000
%o 4000

s         ([+-]?)
d         ([0-9])
e         ([eE]{s}{d}+)
comma     \,
integer   {s}{d}+
uinteger  {d}+
realval   {s}(({d}+\.{d}*{e}?)|({d}*\.{d}+{e}?)|({d}+{e}))
urealval  (({d}+\.{d}*{e}?)|({d}*\.{d}+{e}?)|({d}+{e}))
word      [a-zA-Z_][0-9a-zA-Z_@\.]*
cmnt      [#\$][^\\\n]*
cmnt_e    [#\$][^\\\n]*\\[^\"]
nl        [ \t]*\\\n
fastq_cont [ \t]*\*\n
esc_err   ([ \t]*\\[ \t]*\n)|({cmnt_e}{ws})
sep       [ \t]*\,[ \t]*\,[ \t]*
ws        [ \t]*{comma}?[ \t]*
WS        [ \t\f]*
qstring   \"[^\"\n]*[\"\n]
mlstring  \'[^\']*[\']
end       \n
comment   #+|\{_C_\}|\$+

%s DATA
%s ERROR_STATE
%s M_KEYWORD
%s M_IDENTIFIER
%s Q_STRING
%s PARSING 
%s GET_FILENAME 
%s IF_SKIP 
%s GET_VAR 
%s GET_LOOP_VAR
%s LOOP
%s LOOP_SKIP
%s COMMENT
%x OPERATOR
%%

<COMMENT>{comment} {
  UserInterface::instance()->append_to_journal_string(yytext);
  break;
}

<INITIAL>{comment} {
    // Reset the journal string
  UserInterface::instance()->clear_journal_string();
  print_token("comment",(char*)yytext,"<INITIAL>");
  BEGIN(COMMENT);
  UserInterface::instance()->append_to_journal_string(yytext);
  return('#');
}

<INITIAL>{ws}           break; /* Empty Rule */

<INITIAL>{word} {
    // Reset the journal string
  UserInterface::instance()->clear_journal_string();
    // Check if it is an entity name
  // Todo:  uncomment below
  //const char *class_name = RefEntityName::instance()->
    //get_refentity_type((char*)yytext);
  if (false /*class_name != NULL*/)
  {
      // Send the name to the journal string
    /*if (UserInterface::instance()->names_are_journaled() &&
        !journal_line_skip)
    {
      UserInterface::instance()->append_to_journal_string(yytext);
      UserInterface::instance()->append_to_journal_string(' ');
      journal_skip_lvl += 2;
    }*/
    
    //keyword_id = IDR::instance()->begin_keyword_search(class_name);
    //Todo: uncomment below
    //int id = RefEntityName::instance()->
      //get_refentity_id((char*)yytext);
    char sid[32];
    //sprintf(sid, "%d", id);
    unput_string(sid, strlen(sid));
    finalize_putback();
    
// Todo:  uncomment this???
/*
      // If it was 'picked' (the picked group),
      // see if its contents need to be journalled
#ifndef UNIX_PORT
    if (strcmp((char*)yytext, "picked") == 0 &&
        PickMouseTool::instance()->
        picked_contents_require_journal() &&
        UserInterface::instance()->names_are_journaled())
    {
      CommandHandler::journal_picked_group_contents();
      PickMouseTool::instance()->
        picked_contents_require_journal(VERDE_FALSE);
    }

#endif //UNIX_PORT
*/

  }
    // check if it is referring to the current selection
  // Todo:  allow selection 
  else if (false /*strcmp((char*)yytext, "selection") == 0*/)
  {
	 // Replace the word selection with each selected entity
    PRINT_DEBUG(54, "This is where 'selection' is replaced"
                    " with the currently selected items.\n");
    keyword_id = -1;

/*  Todo:  uncomment this???
#ifndef UNIX_PORT
    int num_selections = PickMouseTool::instance()->
      current_selection_count();
#else
    int num_selections = 0;
#endif //UNIX_PORT
    const char *last_name = NULL;
    const char *type_name = NULL;
    for ( ; num_selections--; )
    {
#ifndef UNIX_PORT
        // Get the type name
      type_name = PickMouseTool::instance()->
        get_selection_type_name(num_selections);
        // Compare it to the previous type name.
        // If they are not the same, write
        // the previous type name and a space.
      if (last_name && strcmp(last_name, type_name))
      {
        unput_string(last_name, strlen(last_name));
        unput(' ');
      }
        // Put the ID back on the input line,
        // with a space before it
      int id = PickMouseTool::instance()->
        get_selection_id(num_selections);
      char sid[32];
      sprintf(sid, "%d", id);
      unput_string(sid, strlen(sid));
      unput(' ');
      
        // If this is the last (first) entity,
        // process it.  Otherwise, wait until you see
        // if the next entity is of the same type.
#endif //UNIX_PORT
      if (num_selections == 0)
      {
        finalize_putback();
        keyword_id = IDR::instance()->begin_keyword_search(type_name);
      }
        // Set this type name to be last_name
      last_name = type_name;
    }
*/

  }
  else
  {
    keyword_id = IDR::instance()->begin_keyword_search((const char*)yytext);
  }
  if( IDR::instance()->is_keyword_valid(keyword_id) )
  {
    last_keyword_id = keyword_id;
    print_token("keyword",(char*)yytext,"<INITIAL>");
    if ((strcmp("quit",IDR::instance()->keyword_string(keyword_id))==0 ||
         strcmp("exit",IDR::instance()->keyword_string(keyword_id))==0) &&
        !UserInterface::instance()->reading_fastq())
    {
      return 0;
    }
    if( IDR::instance()->keyword_search_complete())
    {
      yylval.keyword = keyword_id;
      BEGIN(DATA);
      if (journal_skip_lvl > 0)
        journal_skip_lvl--;
      else
        if (!journal_line_skip)
        {
          UserInterface::instance()->append_to_journal_string(
            IDR::instance()->keyword_string(keyword_id));
          UserInterface::instance()->append_to_journal_string(' ');
        }
      return KEY_WORD;
    }
    else
      BEGIN(M_KEYWORD);
  }
  else
  {
    IDR::case_convert((char*)yytext);	
    PRINT_ERROR("Unrecognized Keyword: '%s'\n",
                (char *)yytext);
    print_token("error",(char*)yytext,"<INITIAL>");
    BEGIN(ERROR_STATE);
    yyerror("syntax error");
    return IDR_ERROR;
  }
  break;
}

<INITIAL>{end} {
  print_token("end",NULL,"<INITIAL>");
  LINENUM_INC;
  yylval.token_id = END;
  if (journal_line_skip)
    journal_line_skip--;
  return END;
}

<M_KEYWORD>{word} {
  keyword_id = IDR::instance()->continue_keyword_search((const char*)yytext);
  if( IDR::instance()->is_keyword_valid(keyword_id) )
  {
    last_keyword_id = keyword_id;
    print_token("keyword",(char*)yytext,"<M_KEYWORD>");
    if( IDR::instance()->keyword_search_complete() )
    {
      yylval.keyword = keyword_id;
      BEGIN(DATA);
      if (journal_skip_lvl > 0)
        journal_skip_lvl--;
      else
      {
        UserInterface::instance()->append_to_journal_string(
          IDR::instance()->keyword_string(keyword_id));
        UserInterface::instance()->append_to_journal_string(' ');
      }
      return KEY_WORD;
    }
  }
  else
  {
      // Rescan the word we just got
      // as begining DATA, not M_KEYWORD.
    yyless(0);
    BEGIN(DATA);
    yylval.keyword = last_keyword_id;
    if (journal_skip_lvl > 0)
      journal_skip_lvl--;
    else
      if (!journal_line_skip)
      {
        UserInterface::instance()->append_to_journal_string(
          IDR::instance()->keyword_string(last_keyword_id));
        UserInterface::instance()->append_to_journal_string(' ');
      }
    return KEY_WORD;
  }
  break;
}

<M_KEYWORD>{ws} {
  print_token("whitespace",(char*)yytext,"<M_KEYWORD>");
  break;
}

<M_KEYWORD>(.|{end}) {
    // Put the character back
    // so that it is scanned again.
  yyless(0);
    // Finish up the multiple-word keyword
  BEGIN(DATA);
  yylval.keyword = last_keyword_id;
  if (journal_skip_lvl > 0)
    journal_skip_lvl--;
  else
    if (!journal_line_skip)
    {
      UserInterface::instance()->append_to_journal_string(
        IDR::instance()->keyword_string(last_keyword_id));
      UserInterface::instance()->append_to_journal_string(' ');
    }
  return KEY_WORD;
}

<M_KEYWORD>{comment} {
  print_token("comment",NULL,"<M_KEYWORD>");
  BEGIN(INITIAL);
  yyless(0);
  return END;
}

<DATA>{integer} {
  print_token("integer",(char*)yytext,"<DATA>");
  yylval.integer = (Integer)atoi((char*)yytext);
  if (journal_skip_lvl > 0)
    journal_skip_lvl--;
  else
    if (!journal_line_skip)
    {
      UserInterface::instance()->append_to_journal_string(yytext);
      UserInterface::instance()->append_to_journal_string(' ');
    }
  return INTEGER;
}

<DATA>{realval} {
  print_token("real",(char*)yytext,"<DATA>");
  yylval.realval = (Real)atof((char*)yytext);
  if (journal_skip_lvl > 0)
    journal_skip_lvl--;
  else
    if (!journal_line_skip)
    {
      UserInterface::instance()->append_to_journal_string(yytext);
      UserInterface::instance()->append_to_journal_string(' ');
    }
  return REAL;
}

<DATA>{word} {
  
    /* Check if it is an entity name */
  // Todo:  uncomment this
  /*const char *class_name = RefEntityName::instance()->
    get_refentity_type((char*)yytext);*/
  if (false /*class_name != NULL*/)
  {
      // Send the name to the journal string
    /*if (UserInterface::instance()->names_are_journaled())
    {
      UserInterface::instance()->append_to_journal_string(yytext);
      UserInterface::instance()->append_to_journal_string(' ');
      journal_skip_lvl += 2;
    }*/
    
    // Todo: uncomment this
	/*identifier_id = IDR::instance()->begin_identifier_search(class_name);
    int id = RefEntityName::instance()->
      get_refentity_id((char*)yytext);*/
    char sid[32];
    //sprintf(sid, "%d", id);
    unput_string(sid, strlen(sid));
    finalize_putback();

/*  Todo:  uncomment this???

#ifndef UNIX_PORT
      // If it was 'picked' (the picked group),
      // see if its contents need to be journalled
    if (strcmp((char*)yytext, "picked") == 0 &&
        PickMouseTool::instance()->
        picked_contents_require_journal() &&
        UserInterface::instance()->names_are_journaled())
    {
      CommandHandler::journal_picked_group_contents();
      PickMouseTool::instance()->
        picked_contents_require_journal(VERDE_FALSE);
    }
#endif //UNIX_PORT
*/
  }
  else if (false /*strcmp((char*)yytext, "selection") == 0*/)
  {
	  // Replace the word selection with each selected entity
    PRINT_DEBUG(54, "This is where 'selection' is replaced "
                    "with the currently selected items.\n");
    identifier_id = -1;

#ifndef UNIX_PORT

// Todo: uncomment this???
//    int num_selections = PickMouseTool::instance()->
//    current_selection_count();
#else
    int num_selections = 0;
#endif //UNIX_PORT

/*
Todo: uncomment this???

    const char *last_name = NULL;
    const char *type_name = NULL;

    for ( ; num_selections--; )
    {
#ifndef UNIX_PORT
        // Get the type name
      type_name = PickMouseTool::instance()->
        get_selection_type_name(num_selections);
        // Compare it to the previous type name.
        // If they are not the same, write
        // the previous type name and a space.
      if (last_name && strcmp(last_name, type_name))
      {
        unput_string(last_name, strlen(last_name));
        unput(' ');
      }
        // Put the ID back on the input line,
        // with a space before it
      int id = PickMouseTool::instance()->
        get_selection_id(num_selections);
      char sid[32];
      sprintf(sid, "%d", id);
      unput_string(sid, strlen(sid));
      unput(' ');
      
        // If this is the last (first) entity,
        // process it.  Otherwise, wait until you see
        // if the next entity is of the same type.
#endif //not UNIX_PORT


	  if (num_selections == 0)
      {
        finalize_putback();
        identifier_id = IDR::instance()->begin_identifier_search(type_name);
      }
        // Set this type name to be last_name
      last_name = type_name;
    }

*/
  }
  else
  {
    identifier_id = IDR::instance()->begin_identifier_search((char*)yytext);
  }
  if( IDR::instance()->is_identifier_valid(identifier_id))
  {
    last_identifier_id = identifier_id;
    print_token("identifier",(char*)yytext,"<DATA>");
    if (IDR::instance()->identifier_search_complete())
    {
      yylval.identifier = identifier_id;
      if (journal_skip_lvl > 0)
        journal_skip_lvl--;
      else
        if (!journal_line_skip)
        {
          UserInterface::instance()->append_to_journal_string(
            IDR::instance()->identifier_string(identifier_id));
          UserInterface::instance()->append_to_journal_string(' ');
        }
      return IDENTIFIER;
    }
    else
      BEGIN(M_IDENTIFIER);
  }
  else if (identifier_id == IDR::MATCH_ANYTHING)
  {
    IDR::case_convert((char*)yytext);	
    yylval.qstring = (char*)yytext;
    if (journal_skip_lvl > 0)
      journal_skip_lvl++;
    else
      if (!journal_line_skip)
      {
        UserInterface::instance()->append_to_journal_string(yytext);
        UserInterface::instance()->append_to_journal_string(' ');
      }
    BEGIN(DATA);
    return QUOTED_STRING;
  }
  else
  {
    print_token("word",(char*)yytext,"<DATA>");
    IDR::case_convert((char*)yytext);	
    yylval.string = (char*)yytext;
    PRINT_ERROR("Unrecognized Identifier: '%s'\n",
                yylval.string);
    BEGIN(ERROR_STATE);
    yyerror("syntax error");
    return IDR_ERROR;
  }
  break;
}

<DATA>{sep} {
  print_token("separator",NULL,"<DATA>");
  yylval.token_id = SEPARATOR;
  return SEPARATOR;
}

<DATA>{ws} {
  print_token("whitespace",(char*)yytext,"<DATA>");
  break;
}

<DATA>("'"|\") {
  print_token("opening quote",NULL,"<DATA>");
  BEGIN(Q_STRING);
  break;
}

<DATA>{end} {
  print_token("end",NULL,"<DATA>");
  LINENUM_INC;
  yylval.token_id = END;
  BEGIN(INITIAL);
  if (journal_line_skip)
    journal_line_skip--;
  return END;
}

<DATA>{comment} {
  print_token("end",NULL,"<DATA>");
  yyless(0);
  BEGIN(INITIAL);
  return END;
}

<COMMENT>{end} {
  print_token("end", NULL, "<COMMENT>");
  LINENUM_INC;
  BEGIN(INITIAL);
  if (journal_line_skip)
    journal_line_skip--;
  else
    do_manual_journal();
  return END;
}

<COMMENT>[^\{|\n] {
  UserInterface::instance()->append_to_journal_string(yytext);
  break;
}

<DATA>"{" {
  oldState = DATA;
  print_token("parse", (char*)yytext, "<DATA>");
  BEGIN(PARSING);
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("{");
  return('{');
}

<DATA>. {
  print_token("operator",(char*)yytext,"<DATA>");
  yyless(0);
  oldState = DATA;
  BEGIN(OPERATOR);
  break;
}

<Q_STRING>(''|\"\") {
  print_token("q:repeated quote",NULL,"<Q_STRING>");
  char* ptr = (char*)yytext;
  while( *ptr )
    ptr++;
  *--ptr = '\0';
  yyleng -= 1;
  yymore();
  break;
}

<Q_STRING>[^'\"\n]*	{
  yymore();
  print_token("q:text",(char*)yytext,"<Q_STRING>");
  break;
}

<Q_STRING>(('|\")|\n) {
    // Print that we got the closing quote
  print_token("closing quote",NULL,"<Q_STRING>");

    // Get rid of the last character
    // (it is the closing quote or a \n).
  char save_char = ((char*)yytext)[yyleng-1];
  ((char*)yytext)[yyleng-1] = '\0';
  yylval.qstring = (char*)yytext;
  
    // Print what the string was
  print_token("quoted string",
              (char*)yytext,"<Q_STRING>");
  
    // Journal the string
  if (!journal_line_skip)
  {
    UserInterface::instance()->append_to_journal_string('"');
    UserInterface::instance()->append_to_journal_string(yytext);
    UserInterface::instance()->append_to_journal_string("\" ");
  }
    // Put back the \n
  if (save_char == '\n')
  {
    unput('\n');
    finalize_putback();
  }

  BEGIN(DATA);
  return QUOTED_STRING;
}

<M_IDENTIFIER>{word} {
  identifier_id = IDR::instance()->continue_identifier_search((char*)yytext);
  if( IDR::instance()->is_identifier_valid(identifier_id) )
  {
    last_identifier_id = identifier_id;
    print_token("identifier",
    (char*)yytext,"<M_IDENTIFIER>");
    if(IDR::instance()->identifier_search_complete())
    {
      yylval.identifier = identifier_id;
      if (journal_skip_lvl > 0)
        journal_skip_lvl--;
      else
        if (!journal_line_skip)
        {
          UserInterface::instance()->append_to_journal_string(
            IDR::instance()->identifier_string(identifier_id));
          UserInterface::instance()->append_to_journal_string(' ');
        }
      BEGIN(DATA);
      return IDENTIFIER;
    }
  }
  else if (identifier_id == IDR::ERROR_INSUFFICIENT_INFO ||
           identifier_id == IDR::ERROR_NO_KEYWORD_MATCH)
  {
    yyless(0);
    BEGIN(DATA);
    yylval.token_id = last_identifier_id;
    if (journal_skip_lvl > 0)
      journal_skip_lvl--;
    else
    if (!journal_line_skip)
    {
      UserInterface::instance()->append_to_journal_string(
        IDR::instance()->identifier_string(last_identifier_id));
      UserInterface::instance()->append_to_journal_string(' ');
    }
    return IDENTIFIER;
  }
  else
  {
    print_token("error",(char*)yytext,"<INITIAL>");
    BEGIN(ERROR_STATE);
    yyerror("syntax error");
    return IDR_ERROR;
  }
  break;
}

<M_IDENTIFIER>{ws} {
  print_token("whitespace",
  (char*)yytext,"<M_IDENTIFIER>");
  break;
}

<M_IDENTIFIER>(.|{end}) {
    // Put the character back
    // so that it is scanned again.
  yyless(0);
    // Finish up the multiple-word identifier
  BEGIN(DATA);
  yylval.identifier = last_identifier_id;
  if (journal_skip_lvl > 0)
    journal_skip_lvl--;
  else
  if (!journal_line_skip)
  {
    UserInterface::instance()->append_to_journal_string(
      IDR::instance()->identifier_string(last_identifier_id));
    UserInterface::instance()->append_to_journal_string(' ');
  }
  return IDENTIFIER;
}

<M_IDENTIFIER>{comment} {
  print_token("end",NULL,"<M_IDENTIFIER>");
  yyless(0);
  BEGIN(INITIAL);
  return END;
}

{nl} {
  print_token("newline",NULL,NULL);
  LINENUM_INC;
  break;
}

{fastq_cont} {
  print_token("fastq_cont",NULL,NULL);
  if (UserInterface::instance()->reading_fastq())
  {
    LINENUM_INC;
  }
  else
  {
      // This is a single '*' just before the \n.
      // Ignore the '*' and put the \n back.
    yyless(strlen(yytext)-1);
  }
  break;
}

{esc_err} {
  print_token("escaped newline error",
              (char*)yytext,NULL);
  LINENUM_INC;
  yylval.token_id = ESCAPE_ERROR;
  BEGIN(INITIAL);
  return ESCAPE_ERROR;
}

"{ECHO}" |
"{ECHO(ON)}" {
  echo = TRUE;
  break;
}

"{NOECHO}" |
"{ECHO(OFF)}" {
  echo = FALSE;
  break;
}

<INITIAL>({comment}{ws}*)*"{"[Ll]"oop(" {
  UserInterface::instance()->clear_journal_string();
  BEGIN(GET_LOOP_VAR);
   if (!journal_line_skip &&
       UserInterface::instance()->aprepro_is_journaled())
     UserInterface::instance()->append_to_journal_string("# {Loop(");
  break;
}

<GET_LOOP_VAR>{realval}")".*"\n" |
<GET_LOOP_VAR>{integer}")}".*"\n" {
  if (!journal_line_skip &&
      UserInterface::instance()->aprepro_is_journaled())
  {
      // Take off the new line
    yytext[strlen(yytext-2)] = '\0';
    UserInterface::instance()->append_to_journal_string(yytext);
    do_manual_journal();
  }

  
    /* Loop control defined by integer */
  char *pt = strchr((char*)yytext, ')');
  *pt = '\0';
  double tmp_real;
  sscanf ((char*)yytext, "%lf", &tmp_real);
  current_loop_count = (int)tmp_real;
  if (current_loop_count <= 0)
  {
    BEGIN(LOOP_SKIP);
  }
  else /* Value defined and != 0. */
  {
#ifndef LINUX
    temp_f = (char *)tmpnam(NULL);
    OPEN_FILE(tmp_file, temp_f, "w");
#else
    tmp_file = tmpfile();
    if(tmp_file == NULL)
    {
      char tmpstr[128];
      sprintf(tmpstr, "Aprepro: ERR: Can't open tmpfile");
      perror(tmpstr);
      exit(EXIT_FAILURE);
    } 
#endif
    loop_lvl++;
    BEGIN(LOOP);
  }
  LINENUM_INC;
  return END;
}

<GET_LOOP_VAR>.+")}".*"\n" {
    /* Loop control defined by variable */
  symrec *s;
  char *pt = strchr((char*)yytext, ')');
  *pt = '\0';
  s = getsym((char*)yytext);
  
  if (s == 0 || s->value.var == 0.)
  {
    BEGIN(LOOP_SKIP);
  }
  else /* Value defined and != 0. */
  {
    current_loop_count = (int)s->value.var;
#ifndef LINUX
    temp_f = (char *)tmpnam(NULL);
    OPEN_FILE(tmp_file, temp_f, "w");
#else
    tmp_file = tmpfile();
    if(tmp_file == NULL)
    {
      char tmpstr[128];
      sprintf(tmpstr, "Aprepro: ERR: Can't open tmpfile");
      perror(tmpstr);
      exit(EXIT_FAILURE);
    }
#endif
 
    loop_lvl++;
    BEGIN(LOOP);
  }
  LINENUM_INC;
    // We return END so that the parser can throw
    // away the optional comment.
  return END;
}

<LOOP>#*" "*"{"[Ee]"nd"[Ll]"oop".*"\n" {
  LINENUM_INC;
  if (--loop_lvl == 0)
  {
    BEGIN(INITIAL);
#ifndef LINUX
    fclose(tmp_file);
    UserInterface::instance()->
      add_input_file(temp_f,
                     FILE_TEMPORARY,
                     current_loop_count);
#else
    UserInterface::instance()->
      add_input_file(tmp_file,
      FILE_TEMPORARY,
      current_loop_count+1);
#endif
  }
  else
    fprintf(tmp_file, "%s", yytext);
  break;
}

<LOOP>"{"[Ll]"oop(".*"\n" {
  loop_lvl++; /* Nested Loop */
  fprintf(tmp_file, "%s", yytext);
  LINENUM_INC;
  break;
}

<LOOP>.*"\n" {
  fprintf(tmp_file, "%s", yytext);
  LINENUM_INC;
  break;
}

<LOOP_SKIP>#*" "*"{"[Ee]"nd"[Ll]"oop".*"\n" {
  LINENUM_INC;
  if (--loop_lvl == 0)
    BEGIN(INITIAL);
  break;
}

<LOOP_SKIP>"{"[Ll]"oop(".*"\n" {
  loop_lvl++; /* Nested Loop */
  LINENUM_INC;
  break;
}

<LOOP_SKIP>.*"\n" {
  LINENUM_INC;
  break;
}

<IF_SKIP>"{"" "*[Ii]f(n?def)?" "*"(" {
    /* This skips if(, ifdef(, and ifndef( */
  if_lvl++; 
  if (if_lvl >= MAX_IF_NESTING)
    yyerror("Too many nested if statements");
    /* Warning!
       We aren't doing anything about
       over-running the array! */
  if_state[if_lvl] = IF_WHILE_SKIP;
  break;
}

<INITIAL>"{"" "*[Ii]f" "*"(" {
    /* Enter parsing mode.  When we leave parsing
       mode, check to see how the parser evaluated
       the expression  */
  UserInterface::instance()->clear_journal_string();
  if_lvl++;
  oldState = INITIAL;
  if (if_lvl >= MAX_IF_NESTING)
    yyerror("Too many nested if statements");
  app_if_type = APP_IF;
  BEGIN(PARSING);
  return APP_IF_BEGIN;
}

<INITIAL>"{"" "*[Ii]fdef" "*"(" {
  UserInterface::instance()->clear_journal_string();
  oldState = INITIAL;
  if_lvl++; 
  if (if_lvl >= MAX_IF_NESTING)
    yyerror("Too many nested if statements");
  app_if_type = APP_IFDEF;
  BEGIN(GET_VAR);
  break;
}

<INITIAL>"{"" "*[Ii]fndef" "*"(" {
  UserInterface::instance()->clear_journal_string();
  oldState = INITIAL;
  if_lvl++; 
  if (if_lvl >= MAX_IF_NESTING)
    yyerror("Too many nested if statements");
  app_if_type = APP_IFNOTDEF;
  BEGIN(GET_VAR);
  break;
}

<COMMENT>"{"" "*[Ii]f" "*"(" {
    /* Enter parsing mode.  When we leave parsing
       mode, check to see how the parser evaluated
       the expression  */
  if_lvl++;
  oldState = COMMENT;
  if (if_lvl >= MAX_IF_NESTING)
    yyerror("Too many nested if statements");
  app_if_type = APP_IF;
  BEGIN(PARSING);
  return APP_IF_BEGIN;
}

<COMMENT>"{"" "*[Ii]fdef" "*"(" {
  oldState = COMMENT;
  if_lvl++; 
  if (if_lvl >= MAX_IF_NESTING)
    yyerror("Too many nested if statements");
  app_if_type = APP_IFDEF;
  BEGIN(GET_VAR);
  break;
}

<COMMENT>"{"" "*[Ii]fndef" "*"(" {
  oldState = COMMENT;
  if_lvl++;
  if (if_lvl >= MAX_IF_NESTING)
    yyerror("Too many nested if statements");
  app_if_type = APP_IFNOTDEF;
  BEGIN(GET_VAR);
  break;
}

<GET_VAR>.+")"" "*"}" {
  symrec *s;
  char *pt = strchr((char*)yytext, ')');
  *pt = '\0';
  s = getsym((char*)yytext);
  if (s == 0 || s->value.var == 0.)
  {
    if (app_if_type == APP_IFDEF)
    {
      BEGIN(IF_SKIP);
      if_state[if_lvl] = IF_SKIP;
    }
    else
    {
      BEGIN(oldState);
      if_state[if_lvl] = INITIAL;
    }
  }
  else // Value defined and != 0.
  {
    if (app_if_type == APP_IFDEF)
    {
      BEGIN(oldState);
      if_state[if_lvl] = INITIAL;
    }
    else
    {
      BEGIN(IF_SKIP);
      if_state[if_lvl] = IF_SKIP;
    }
  }
  LINENUM_INC;
  break;
}

"{"[Ee]"lse}" {
  LINENUM_INC;
  if (if_lvl > 0)
  {
    if (if_state[if_lvl] == IF_SKIP)
      BEGIN(oldState);
    if (if_state[if_lvl] == INITIAL)
      BEGIN(IF_SKIP);
      /* If neither is true, this is a nested 
         'if' that should be skipped */
  }
  else
    yyerror("Improperly placed Else statement");
  break;
}

"{"[Ee]"ndif}" {
    /* Ignore endif if not skipping */
    // If the if_state is neither IF_SKIP nor INITIAL,
    // this is a nested 'if' that should be skipped
  if (if_lvl > 0)
  {
    if (if_state[if_lvl] == IF_SKIP ||
        if_state[if_lvl] == INITIAL)
      BEGIN(oldState);
    if_lvl--;
  }
  else
  {
    if_lvl = 0;
    yyerror("Improperly Nested ifdef/ifndef statements");
  }
  break;
}

<IF_SKIP>"guiexitnow\n" {
  break;
}

<IF_SKIP>.                 { break; }
<IF_SKIP>\n                { LINENUM_INC; break; }

<INITIAL>"{"[Ii]"nclude(" {
  UserInterface::instance()->clear_journal_string();
  oldState = INITIAL;
  BEGIN(GET_FILENAME);
  break;
}
<INITIAL>"{"[Cc][Ii]"nclude(" {
  UserInterface::instance()->clear_journal_string();
  oldState = INITIAL;
  BEGIN(GET_FILENAME);
  break;
}
<COMMENT>"{"[Ii]"nclude(" {
  oldState = COMMENT;
  BEGIN(GET_FILENAME);
  break;
}
<COMMENT>"{"[Cc][Ii]"nclude(" { oldState = COMMENT; BEGIN(GET_FILENAME); break; }

<GET_FILENAME>.+")"{WS}"}" {
  BEGIN(oldState); 
  char *pt = strchr((char *)yytext, ')');
  *pt = '\0';
    /* Check to see if surrounded by double quote */ 
  if ((pt = strrchr((char *)yytext, '"')) != NULL)
    *pt = '\0';
  if ((pt = strchr((char *)yytext, '"')) != NULL) 
    pt++;
  else
    pt = (char *)yytext;
  
  include_message(pt);
  LINENUM_INC;
  UserInterface::instance()->add_input_file((char *)pt);
  break;
}

<COMMENT>"{" {
  oldState = COMMENT;
  print_token("parse", (char*)yytext, "<COMMENT>");
  BEGIN(PARSING);
  UserInterface::instance()->append_to_journal_string("{");
  return('{');
}

<PARSING>{uinteger} |        
<PARSING>{urealval} {
  yylval.realval = (Real)atof((char*)yytext);
  print_token("number", (char*)yytext, "<PARSING>");
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string(yytext);
  return(NUM);
}

<PARSING>{word} {
  print_token("word", (char*)yytext, "<PARSING>");
  symrec *s;
  s = getsym((char*)yytext);
  if (s == 0)
    s = putsym ((char*)yytext, UNDVAR);
  yylval.tptr = s;
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string(yytext);
  return(s->type);
}

<PARSING>"=" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("=");
  return('=');
}

<PARSING>"+=" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("+=");
  return(EQ_PLUS);
}

<PARSING>"-=" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("-=");
  return(EQ_MINUS);
}

<PARSING>"*=" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("*=");
  return(EQ_TIME);
}

<PARSING>"/=" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("/=");
  return(EQ_DIV);
}

<PARSING>"^=" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("^=");
  return(EQ_POW);
}

<PARSING>"**=" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("**=");
  return(EQ_POW);
}

<PARSING>"++" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("++");
  return(INC);
}

<PARSING>"--" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("--");
  return(DEC);
}

<PARSING>"+" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("+");
  return('+');
}

<PARSING>"-" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("-");
  return('-');
}

<PARSING>"*" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("*");
  return('*');
}

<PARSING>"~" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("~");
  return('*');     /* ~ is same as multiply */
}

<PARSING>"//" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("//");
  return(CONCAT);  /* String concatenation */
}

<PARSING>"/" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("/");
  return('/');
}

<PARSING>"%" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("%");
  return('%');
}

<PARSING>"^" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("^");
  return(POW);
}

<PARSING>"**" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("**");
  return(POW);
}

<PARSING>\n                { LINENUM_INC; break; }
<PARSING>"(" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("(");
  return('(');
}

<PARSING>")" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string(")");
  return(')');
}

<PARSING>"," {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string(",");
  return(',');
}

<PARSING>";" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string(";");
  return(';');
}

<PARSING>":" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string(":");
  return(':');
}

<PARSING>"?" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("?");
  return('?');
}

<PARSING>"<" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("<");
  return('<');
}

<PARSING>">" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string(">");
  return('>');
}

<PARSING>"<=" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("<=");
  return(LE);
}

<PARSING>">=" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string(">=");
  return(GE);
}

<PARSING>"==" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("==");
  return(EQ);
}

<PARSING>"!=" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("!=");
  return(NE);
}

<PARSING>"&&" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("&&");
  return(LAND);
}

<PARSING>"||" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("||");
  return(LOR);
}

<PARSING>"|" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("|");
  return('|');
}

<PARSING>"!" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("!");
  return(NOT);
}

<PARSING>"[" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("[");
  return('[');
}

<PARSING>"]" {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string("]");
  return(']');
}

<PARSING>{WS} {
  if (UserInterface::instance()->aprepro_is_journaled())
    UserInterface::instance()->append_to_journal_string(' ');
  break; /* Empty Rule */
}

<PARSING>{qstring} {
  char *pt = strrchr((char*)yytext, '"');
  *pt = '\0';
  NEWSTR((char*)yytext+1, yylval.string);
  print_token("qstring", yylval.string, "<PARSING>");
  if (UserInterface::instance()->aprepro_is_journaled())
  {
    UserInterface::instance()->append_to_journal_string('"');
    UserInterface::instance()->append_to_journal_string(yylval.string);
    UserInterface::instance()->append_to_journal_string('"');
  }
  return QSTRING;
}

<PARSING>{mlstring} {
  char *pt = strrchr((char*)yytext, '\'');
  *pt = '\0';
  NEWSTR((char*)yytext+1, yylval.string);
  print_token("mlstring", yylval.string, "<PARSING>");
  return QSTRING;
}

<PARSING>"}" {
    /* See if we are finishing an 'if' statement */
  if (app_if_type == APP_IF)
  {
    print_token("end if_eval",
                (char*)yytext, "<PARSING>");
                              // see what the result was
    if (if_result)
    {
      BEGIN(oldState);
      if_state[if_lvl] = INITIAL;
    }
    else
    {
      BEGIN(IF_SKIP);
      if_state[if_lvl] = IF_SKIP;
    }
                              /* indicate we've processed the IF */
    app_if_type = APP_IFDEF;
  }
  else
  {
    print_token("endparse",(char*)yytext,"<PARSING>");
    BEGIN(oldState);
    if (UserInterface::instance()->aprepro_is_journaled())
    {
      UserInterface::instance()->append_to_journal_string("}");
      UserInterface::instance()->append_to_journal_string(" ");
    }
    return('}');
  }
  break;
}

<PARSING>. {
  PRINT_ERROR("Aprepro: Unexpected symbol found: %c\n", *yytext);
  BEGIN(ERROR_STATE);
  return IDR_ERROR;
}

<INITIAL>"{" {
  print_token("beginparse",(char*)yytext,"<INITIAL>");
  oldState = INITIAL;
  BEGIN(PARSING);
  return('{');
}

<INITIAL>{realval} |
<INITIAL>{integer} {
  print_token("number", yytext, "<INITIAL>");
  PRINT_ERROR("Missing keyword in command.\n"
              "       Command can not begin with a number\n");
  BEGIN(ERROR_STATE);
  yyerror("syntax error");
  return IDR_ERROR;
}

<INITIAL>{qstring} |
<INITIAL>\'[^\'\n]*[\'\n] {
  print_token("quoted string", yytext, "<INITIAL>");
  PRINT_ERROR("Missing keyword in command.\n"
              "       Command can not begin with a quoted string\n");
  BEGIN(ERROR_STATE);
  yyerror("syntax error");
  return IDR_ERROR;
}


<INITIAL>. {
  print_token("operator", yytext, "<INITIAL>");
  yyless(0);
  oldState = INITIAL;
  BEGIN(OPERATOR);
  break;
}

<OPERATOR>[^a-zA-Z \t_\"\'0-9.,$#}{\n] {
  print_token("operator", (char*)yytext, "<OPERATOR>");
#ifdef USING_DFPARSER
  int new_match;
    // If this character is part of a token match...
  if (DFParser::instance()->lookup_token(yytext, &new_match))
    operatorMatchIndex = new_match;
    // If it's not part of a token match, but we
    // had a prior match
  else if (operatorMatchIndex != -1)
  {
      // Indicate that we need to rescan the last character
    yyless(yyleng-1);
      // Return the appropriate token type
    if (oldState == INITIAL)
    {
      yylval.keyword = IDR::ALTERNATIVE_ID_BASE - operatorMatchIndex;
      operatorMatchIndex = -1;
      oldState = DATA;
      return KEY_WORD;
    }
    else
    {
      yylval.identifier =
        IDR::ALTERNATIVE_ID_BASE -
        IDR::instance()->number_keywords() -
        operatorMatchIndex;
      operatorMatchIndex = -1;
      return IDENTIFIER;
    }
  }
    // If this isn't part of a match, and we didn't have a prior
    // match, just keep collecting chars until we hit the other
    // <OPERATOR> rule.  We'll throw out an error at that point.
#endif
  yymore();
  break;
}

<OPERATOR>[a-zA-Z \t_\"\'0-9.,$#}{\n] {
  print_token("non-operator", (char*)yytext, "<OPERATOR>");
  
    // If yyleng is 1, we don't want to put the character back to
    // be rescanned.  That would put us in an infinite loop.
    // Just send an error.  Ideally, this case should never arise,
    // but lets handle it just to be safe.
  if (yyleng == 1)
  {
    BEGIN(ERROR_STATE);
    yyerror("syntax error");
    return IDR_ERROR;
  }
  
    // Go to DATA state
  if (oldState == INITIAL)
    BEGIN(DATA);
  else
    BEGIN(oldState);
  
    // We use leng instead of yyleng because some (all?) versions of flex
    // modify yyleng in the yyless macro.
  int leng = yyleng;
  yyless(yyleng-1);
#ifdef USING_DFPARSER
    // Return the most recently completed operator,
    // stored in operatorMatchIndex.  Then reset it
    // to -1.
  if (operatorMatchIndex != -1)
  {
    if (oldState == INITIAL)
    {
      yylval.keyword = IDR::ALTERNATIVE_ID_BASE - operatorMatchIndex;
      operatorMatchIndex = -1;
      return KEY_WORD;
    }
    else
    {
      yylval.identifier =
        IDR::ALTERNATIVE_ID_BASE -
        IDR::instance()->number_keywords() -
        operatorMatchIndex;
      operatorMatchIndex = -1;
      return IDENTIFIER;
    }
  }
#endif
  char* tmp_str = new char[leng];
  strncpy(tmp_str, yytext, leng-1);
  tmp_str[leng-1] = '\0';
  PRINT_ERROR("Unrecognized symbol: '%s'\n",
              tmp_str);
  delete [] tmp_str;
  BEGIN(ERROR_STATE);
  yyerror("syntax error");
  return IDR_ERROR;
}

<ERROR_STATE>{end} {
  print_token("end", NULL, "<ERROR_STATE>");
  LINENUM_INC;
  yylval.token_id = END;
  BEGIN(INITIAL);
  return END;
}

<ERROR_STATE>.* {
  print_token("ignored characters", NULL, "<ERROR_STATE>");
  break;
}

<<EOF>> {

  PRINT_DEBUG(54, "  Closing out string buffer.\n");
  if (putback_stack_index > 0)
  {
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(putback_stack[--putback_stack_index]);
  }
  else
    yyterminate();
  break;
}

%%

/* "  --This quote is here for the benefit of source code highlighting
   editors that get messed up by solo quotes above... */

void print_token( const char* s, const char* t, const char* st )
{
  if (DEBUG_FLAG(54))
  {
    if( t )
    {
		if( st )
        PRINT_DEBUG(54,
          "lexer read token '%s' = '%s' in state %s\n",s,t,st);
		else
        
		PRINT_DEBUG(54,
          "lexer read token '%s' = '%s'\n",s,t);
    }
    else
    {
		if( st )
        PRINT_DEBUG(54, 
          "lexer read token '%s' in state %s\n",s,st);
		else
        PRINT_DEBUG(54,
          "lexer read token '%s'\n",s);
    }
    if (strcmp("end", s) == 0){}
      PRINT_DEBUG(54, "\n");
  }
}

void unput_string(const char* s, Integer length)
{
  Integer i;
  for( i=length; i--; )
    unput(s[i]);
}

inline void reset_lex_state()
{
  BEGIN(INITIAL);
}

/* Print error message to standard error and return.  Note: internally
 *   `lineno' starts at zero.  To avoid confusion, we add 1 to value
 *   when it is output.
 */

void include_message (const char *var)
{
  PRINT_INFO("INFO: Included File: `%s' (%s, line %d)\n",
		 var,
		 UserInterface::instance()->currentFile->filename.c_str(),
		 UserInterface::instance()->currentFile->lineNumber);
}

void undefined_warning (const char *var)
{
  PRINT_WARNING(" Undefined variable `%s' (%s, line %d)\n",
                var, 
                UserInterface::instance()->currentFile->filename.c_str(),
                UserInterface::instance()->currentFile->lineNumber);
}

void redefined_warning (const char *var)
{
  if (*var != '_')
    PRINT_WARNING(" Variable `%s' redefined (%s, line %d)\n",
                  var,
                  UserInterface::instance()->currentFile->filename.c_str(),
                  UserInterface::instance()->currentFile->lineNumber);
}

void warning (const char *string)
{
  PRINT_WARNING(" `%s' (%s, line %d)\n",
                string,
                UserInterface::instance()->currentFile->filename.c_str(),
                UserInterface::instance()->currentFile->lineNumber);
}

/* Units Files */
char *do_Units (const char *type)
{
  const char *const unit_dir = "units";     /* "Top" directory for units */

  char temp_char[BUFSIZ];
  char unit_file[BUFSIZ];
  char conversion_file[BUFSIZ];
  char *ret_string;

  /* Build path for unit file */
  sprintf (temp_char, "%s/%s", unit_dir, type);
  conv_string (temp_char);

  /* NOTE: pathopen returns pointer to static char variable.  This variable
     is overwritten each time pathopen is called.  Must copy string into
     allocated memory if need to save value pointed to.
     */

  strcpy (unit_file, pathopen (temp_char));

  if (DEBUG_FLAG(54))
  {
    if (unit_file == '\0')
      PRINT_ERROR("DEBUG: Could not open: $MATSPATH/%s\n\n", temp_char);
    else
      PRINT_ERROR("DEBUG: Found unit file: %s\n\n", unit_file);
  }

  /* Now find the conversion file
   * Build path for conversion file
   */
  sprintf (temp_char, "%s/conversion", unit_dir);
  conv_string (temp_char);

  /* NOTE: pathopen returns pointer to static char variable.  This variable
     is overwritten each time pathopen is called.  Must copy string into
     allocated memory if need to save value pointed to.
     */
  
  strcpy (conversion_file, pathopen (temp_char));
  if (DEBUG_FLAG(54))
  {
	  if (conversion_file == '\0')
      PRINT_DEBUG(54,
		  "DEBUG: Could not open: $MATSPATH/%s\n\n", temp_char);
	  else
      PRINT_DEBUG(54, 
		  "DEBUG: Found conversion file: %s\n\n", conversion_file);
  }


  if (unit_file != '\0' && conversion_file == '\0')
  {
    PRINT_ERROR("Aprepro unit file found, no conversion file\n\n");
    NEWSTR ("Aprepro: ERR: Units Conversion file not found", ret_string);
  }
  else if (unit_file != '\0' && conversion_file != '\0')
  {
      /* We need to do some weird stuff with braces since we are currently
         inside a function call that is inside a brace.  Therefore, we first
         output the closing brace and then do our includes.  At the end we
         simply do the expression {" "} which prints a blank character.
         The closing brace is supplied by the closing brace of the
         Material() function call which is in the current file.
      */
    UserInterface::instance()->add_input_file(conversion_file);
    UserInterface::instance()->add_input_file(unit_file);
    NEWSTR(" ", ret_string);
  }
  else
  {
    PRINT_ERROR("Aprepro unit file '%s' not found\n\n", type);
    NEWSTR ("Aprepro: ERR: Units File not found", ret_string);
  }
  return ret_string;
}

/* Convert string to all lower-case and replace all spaces with '_' */
void conv_string (char *string)
{
  char *p = string;
  while (*p != '\0')
    {
      if (*p == ' ')
        *p = '_';
      else if (isupper (*p))
        *p = tolower (*p);
      p++;
    }
}

const char *pathopen (const char *file)
{
  static char trypath[BUFSIZ];
  static char *savepath;
  static int first = 1;
  char *matspath, *cp;
  FILE *fd;

  if (first)
  {
    const char *mats = "/mats";
    const char *accesspath = (const char*)getenv("ACCESS");
    first = 0;
    
    if (accesspath == NULL)
    {
      PRINT_WARNING("Your ACCESS environment variable is not set\n");
      PRINT_WARNING("Please set your ACCESS environment variable and try again\n");
      return(NULL);
    }
    matspath = (char *)malloc (sizeof(char)*(strlen(accesspath)+strlen(mats)+1));
    strcpy(matspath, accesspath);
    strcat(matspath, mats);
    savepath = matspath;      /* used for restarting */
  }
  matspath = savepath;
  
  do
  {
    trypath[0] = '\0';
      /* this should take into account limits on size of trypath */
    for (cp = trypath; *matspath && *matspath != ':';)
      *cp++ = *matspath++;
    
    if (cp != trypath)
    {   /* nun-null element in path */
        /* add directory punctuation only if needed */
      if (*(cp - 1) != '/')
        *cp++ = '/';
        /* append filename */
      strcpy (cp, file);
    }
    else
      strcpy (trypath, file);
    PRINT_DEBUG(54, "Path Tried: %s\n", trypath);
    if ((fd = fopen (trypath, "r")))
    {
      fclose (fd);
      return (trypath);
    }
    
      /* no luck, keep going */
    if (*matspath == ':' && matspath[1] != '\0')
      matspath++;             /* skip colon */
  }
  while (*matspath);
    /*
     * You might have one of the mats
     * paths defined, WITHOUT the current working directory in it.
     * Therefore try to open the file in the current directory.
     */

  PRINT_DEBUG(54, "Path Tried: %s\n", file);
  
  if ((fd = fopen (file, "r")))
  {
    fclose (fd);
    return (file);
  }
  else
    return (NULL);
}

#ifdef FLEX_SCANNER

int redefined_yy_input(char* buf, int)
{
  buf[0] = UserInterface::instance()->get_input();
  return 1;
}

void putback(char c)
{
  if (putback_index != 0)
  {
    putback_buffer[--putback_index] = c;
  }
  else
  {
    PRINT_ERROR("Internal text replacement buffer overflow.\n");
  }
}
#endif

void finalize_putback()
{
#ifdef FLEX_SCANNER
  if (putback_index < PUTBACK_BUFFER_SIZE)
  {
      // Make sure the string is NULL terminated.
      // We only need to do this once, but this is
      // a convenient place to do it.
    putback_buffer[PUTBACK_BUFFER_SIZE] = '\0';

      // Make sure we have enough stack space to
      // get us back where we were.
    if (putback_stack_index < MAX_PUTBACK_STACK)
    {
        // Save the current buffer
      putback_stack[putback_stack_index++] = YY_CURRENT_BUFFER;
        // Send the string to be scanned
      yy_scan_string(&putback_buffer[putback_index]);
      PRINT_DEBUG(54, "   Rescanning string %s\n", &putback_buffer[putback_index]);
        // clean up and get ready for next putback
      putback_index = PUTBACK_BUFFER_SIZE;
    }
    else
      PRINT_ERROR("Internal text replacement stack overflow.\n");
  }
#endif
}

void do_manual_journal()
{
  UserInterface* ui = UserInterface::instance();
  VerdeString echo_string = ui->journal_string();
  if(ui->command_is_recorded())
    ui->recording_stream() << echo_string.c_str() << std::endl;
  if(ui->command_is_journaled())
  {
    // Don't journal comments if they start with 2 # (##)
    if (!(echo_string.get_at(0) == '#' && echo_string.get_at(1) == '#'))
    {
      ui->journal_stream() << echo_string.c_str() << std::endl;
      if (!ui->journal_stream().good())
        PRINT_WARNING("Unknown error writing to journal file.\n");
    }
  }
  ui->clear_journal_string();
}
