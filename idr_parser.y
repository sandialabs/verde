%{
#include "VerdeDefines.hpp"
#include "VerdeMessage.hpp"
#include "UserInterface.hpp"
#ifdef UNIX_PORT
  #include "nt_gui_source/util/GUIParsingViewer.hpp"
#endif //unix_port
#include <string.h>
#include <math.h>
#include "idr.hpp"


void debug_idr_num(const char*, const char*, const char*, Real );
static inline void journal_number(Real val);

#ifdef UNIX_PORT
 #define __BLACKBOX_H__          // To exclude some Stingray header
 #define __CRYPTOFIL_H__         // To exclude some Stingray header
 #define __SECCOMP_H__           // To exclude some Stingray header
 #define __SECCOMPRESS_FILE_H__  // To exclude some Stingray header
 #define  _TOKENIZER_H_          // To exclude some Stingray header
#endif //unix_port

Integer cntr = 0;

extern int exit_yyparse;

char idrStringBuffer[IDR::MAX_STRING_LENGTH];
FunctionData& idrParsedData()
{
  static FunctionData f;
  return f;
}

inline void reset_lex_state();

void include_message   PROTO((const char* var));
//Added by CAT for NT port
void unput_string PROTO((const char*, Integer ));

void undefined_warning PROTO((const char* var));
void redefined_warning PROTO((const char* var));
void warning PROTO((const char *string));
void append_to_buffer PROTO((FunctionData* parsed_data,
                             Integer count,Real rdata, Integer idata,
                             const char* cdata, const char* id_string ));
void process_idr_error();
void save_string PROTO((char *to, const char *from));

void debug_idr(const char* s1, const char* s2, const char* s3);
 

int   echo = TRUE;
#ifdef NT
	extern "C" int yyparse();
	extern int yylex();
#else
extern "C" {
  int yyparse();
  int yylex();
}
#endif


#ifndef NT
static void finalize_putback();
#else
extern void finalize_putback();
#endif
 
#ifdef DEBUG
#define PRINT_DEBUG_NL PRINT_ERROR("\n");
#else
#define PRINT_DEBUG_NL
#endif

symrec *format;

char if_result;
%}

%union
{
  Integer	keyword;
  Integer	identifier;
  Real    realval;
  Integer	integer;
  char*   string;
  char*   qstring;
  Integer token_id;
  symrec  *tptr;		/* For returning symbol-table pointers	*/
}

%token  <keyword>    KEY_WORD
%token  <identifier> IDENTIFIER
%token  <realval>    REAL
%token  <integer>    INTEGER
%token  <string>     IDR_ERROR
%token  <qstring>    QUOTED_STRING
%token  <token_id>   SEPARATOR
%token  <token_id>   EQUALS
%token  <token_id>   ESCAPE_ERROR
%token  <token_id>   END
%token  <realval>    NUM	/* Simple double precision number	*/
%token  <string>     QSTRING	/* Quoted string			*/
%token  <tptr>       UNDVAR VAR FNCT	/* Variable and function	*/
%token  <tptr>       SVAR SFNCT SDFNCT /* String Variable and function */
%token  <tptr>       FNCTDI /* Returns double, 1 int parameter */
%token  <tptr>       FNCTDC /* Returns double, 1 char * parameter */
%token  <tptr>       APP_IF_BEGIN /* Need to evaluate an 'if' */
%type   <realval>    exp bool
%type   <string>     sexp

/* Precedence (Lowest to Highest) and associativity */
%right	'=' 
%right  EQ_PLUS EQ_MINUS
%right  EQ_TIME EQ_DIV
%right  EQ_POW
%right '?' ':'
%left   LOR                /* Logical OR     */
%left   LAND               /* Logical AND    */
%left '<' '>' LE GE EQ NE  /* <=, >=, ==, != */
%left	'-' '+'
%left	'/' '*' '%'
%left	UNARY NOT 	/* Negation--unary minus/plus 		*/
%right	POW	  	/* Exponentiation	      		*/
%left	INC DEC   	/* increment (++), decrement (--) 	*/
%left	CONCAT	  	/* Concatenate Strings			*/


%start statements

%%

statements	:
		| statements statement
		;

statement : END
          {}
	|  command END
   {
     debug_idr_num("statement", "END", "number of entries", (Real)cntr);
     append_to_buffer(&idrParsedData(), cntr,
                      0, 0,
                      NULL, NULL);
     IDR::instance()->call_keyword_handler(&idrParsedData());
     cntr = 0;
     idrStringBuffer[0] = '\0';
     if(exit_yyparse == VERDE_TRUE)
	YYACCEPT;
   }

          |  comment END
   {
     cntr = 0;
     idrStringBuffer[0] = '\0';
   } 

          | command ESCAPE_ERROR error END
   {
     PRINT_ERROR("Previous improperly escaped newline.\n");
     process_idr_error();
     cntr = 0;
     idrStringBuffer[0] = '\0';
   }
          | error END
   {
     process_idr_error();
     cntr = 0;
     idrStringBuffer[0] = '\0';
   }
          | command error END
   {
     process_idr_error();
     cntr = 0;
     idrStringBuffer[0] = '\0';
   }
          | IDR_ERROR END
   {
     process_idr_error();
     cntr = 0;
     idrStringBuffer[0] = '\0';
   }
          | command IDR_ERROR END
   {
     process_idr_error();
     cntr = 0;
     idrStringBuffer[0] = '\0';
   }

command   : KEY_WORD
   {
     debug_idr("command", "KEY_WORD", IDR::instance()->keyword_string($1));
     idrParsedData().dataListLength = 0;
     idrParsedData().keywordId = $1;
       //UserInterface::instance()->
       //prepend_to_journal_string(IDR::instance()->keyword_string($1));
   }
          | KEY_WORD data
   {
     debug_idr("command", "KEY_WORD data", IDR::instance()->keyword_string($1));
     idrParsedData().dataListLength = cntr;
     idrParsedData().keywordId = $1;
       //UserInterface::instance()->
       //prepend_to_journal_string(IDR::instance()->keyword_string($1));
   }
          | KEY_WORD EQUALS data
   {
     debug_idr("command", "KEY_WORD EQUALS data", IDR::instance()->keyword_string($1));
     idrParsedData().dataListLength = cntr;
     idrParsedData().keywordId = $1;
       //UserInterface::instance()->
       //prepend_to_journal_string(IDR::instance()->keyword_string($1));
   }
          | strdatum KEY_WORD
   {
     debug_idr("command", "strdatum KEY_WORD", IDR::instance()->keyword_string($2));
     idrParsedData().dataListLength = 0;
     idrParsedData().keywordId = $2;
       //UserInterface::instance()->
       //prepend_to_journal_string(IDR::instance()->keyword_string($2));
   }
          | strdatum KEY_WORD data
   {
     debug_idr("command", "strdatum KEY_WORD data", IDR::instance()->keyword_string($2));
     idrParsedData().dataListLength = cntr;
     idrParsedData().keywordId = $2;
       //UserInterface::instance()->
       //prepend_to_journal_string(IDR::instance()->keyword_string($2));
   }
          | strdatum KEY_WORD EQUALS data
   {
     debug_idr("command", "strdatum KEY_WORD EQUALS data", IDR::instance()->keyword_string($2));
     idrParsedData().dataListLength = cntr;
     idrParsedData().keywordId = $2;
       //UserInterface::instance()->
       //prepend_to_journal_string(IDR::instance()->keyword_string($2));
   }
;

comment   : '#'
          | comment data
          | comment if_eval
;

if_eval   : APP_IF_BEGIN exp ')'
   {
     debug_idr_num("if_eval", ")", "expression value", (Real)$2);
     if_result = $2 ? 1 : 0;
   }
;

data      : datum
          | strdatum
          | data datum
          | data strdatum
;

qstring   : QUOTED_STRING
   {
     if( !(strlen($1) < IDR::MAX_STRING_LENGTH) )
     {
       yyerror("excessively long string");
       exit(EXIT_FAILURE);
     }
     strcpy( idrStringBuffer, $1 );
   };

strdatum  : '{' sexp '}'
   {
     debug_idr("strdatum", "'{' sexp '}'", $2);
     unput_string($2, strlen($2));
     finalize_putback();
     FREESTR($2);
   };

datum   :   SEPARATOR
   {
     debug_idr("datum", "SEPARATOR", NULL);
     append_to_buffer( &idrParsedData(), cntr++,
                       0.0, 0, 
                       NULL, NULL );
   }
          | INTEGER
   {
     debug_idr_num("datum", "INTEGER", NULL, (Real)$1);
     if ($1 > VERDE_INT_MIN && $1 < VERDE_INT_MAX)
       append_to_buffer( &idrParsedData(), cntr++,
                         (Real)$1, (Integer)$1,
                         NULL, NULL );
     else
       append_to_buffer( &idrParsedData(), cntr++,
                         (Real)$1, VERDE_INT_MIN,
                         NULL, NULL );
   }
          | IDENTIFIER INTEGER
   {
     debug_idr_num("datum", "IDENTIFIER INTEGER",
                   IDR::instance()->identifier_string($1), (Real)$2);
     if ($2 > VERDE_INT_MIN && $2 < VERDE_INT_MAX)
       append_to_buffer( &idrParsedData(), cntr++, 
                         (Real)$2, (Integer)$2, 
                         NULL, IDR::instance()->identifier_string($1) );
     else
       append_to_buffer( &idrParsedData(), cntr++, 
                         (Real)$2, VERDE_INT_MIN, 
                         NULL, IDR::instance()->identifier_string($1) );
       
   }
          | IDENTIFIER EQUALS INTEGER
   {
     debug_idr_num("datum", "IDENTIFIER EQUALS INTEGER",
               IDR::instance()->identifier_string($1), (Real)$3);
       /* Even though this is an integer value, let's check it */
       /* against min and max bounds */
     if ((Real)$3 > VERDE_INT_MIN && (Real)$3 < VERDE_INT_MAX)
       append_to_buffer( &idrParsedData(), cntr++, 
                         (Real)$3, (Integer)$3, 
                         NULL, IDR::instance()->identifier_string($1) );
     else
       append_to_buffer( &idrParsedData(), cntr++, 
                         (Real)$3, VERDE_INT_MIN, 
                         NULL, IDR::instance()->identifier_string($1) );
   }
          | REAL
   {
     debug_idr_num("datum", "REAL", NULL, (Real)$1);
     if ((Real)$1 > VERDE_INT_MIN && (Real)$1 < VERDE_INT_MAX)
       append_to_buffer( &idrParsedData(), cntr++, 
                         (Real)$1, (Integer)$1, 
                         NULL, NULL );
     else
       append_to_buffer( &idrParsedData(), cntr++, 
                         (Real)$1, VERDE_INT_MIN, 
                         NULL, NULL );
   }
          | IDENTIFIER REAL
   {
     debug_idr_num("datum", "IDENTIFIER REAL",
               IDR::instance()->identifier_string($1), (Real)$2);
     if ((Real)$2 > VERDE_INT_MIN && (Real)$2 < VERDE_INT_MAX)
       append_to_buffer( &idrParsedData(), cntr++, 
                         (Real)$2, (Integer)$2, 
                         NULL, IDR::instance()->identifier_string($1) );
     else
       append_to_buffer( &idrParsedData(), cntr++, 
                         (Real)$2, VERDE_INT_MIN, 
                         NULL, IDR::instance()->identifier_string($1) );
   }
          | IDENTIFIER EQUALS REAL
   {
     debug_idr_num("datum", "IDENTIFIER EQUALS REAL",
               IDR::instance()->identifier_string($1), (Real)$3);
     if ((Real)$3 > VERDE_INT_MIN && (Real)$3 < VERDE_INT_MAX)
       append_to_buffer( &idrParsedData(), cntr++, 
                         (Real)$3, (Integer)$3, 
                         NULL, IDR::instance()->identifier_string($1) );
     else
       append_to_buffer( &idrParsedData(), cntr++, 
                         (Real)$3, VERDE_INT_MIN, 
                         NULL, IDR::instance()->identifier_string($1) );
   }
          | IDENTIFIER '{' exp '}'
   {
     debug_idr_num("datum", "IDENTIFIER '{' exp '}'",
               IDR::instance()->identifier_string($1), (Real)$3);
     if ($3 > VERDE_INT_MIN && $3 < VERDE_INT_MAX)
       append_to_buffer(&idrParsedData(), cntr++, 
                        (Real)$3, (Integer)$3, 
                        NULL, IDR::instance()->identifier_string($1));
     else
       append_to_buffer(&idrParsedData(), cntr++, 
                        (Real)$3, VERDE_INT_MIN, 
                        NULL, IDR::instance()->identifier_string($1));
     
     if (!UserInterface::instance()->aprepro_is_journaled())
     {
       journal_number((Real)$3);
     }
   }
          | IDENTIFIER EQUALS '{' exp '}'
   {
     debug_idr_num("datum", "IDENTIFIER EQUALS '{' exp '}'",
               IDR::instance()->identifier_string($1), (Real)$4);
     if ((Real)$4 > VERDE_INT_MIN && (Real)$4 < VERDE_INT_MAX)
       append_to_buffer( &idrParsedData(), cntr++, 
                         (Real)$4, (Integer)$4, 
                         NULL, IDR::instance()->identifier_string($1) );
     else
       append_to_buffer( &idrParsedData(), cntr++, 
                         (Real)$4, VERDE_INT_MIN, 
                         NULL, IDR::instance()->identifier_string($1) );
     
     if (!UserInterface::instance()->aprepro_is_journaled())
     {
       journal_number((Real)$4);
     }
   }
          | '{' exp '}'
   {
     debug_idr_num("datum", "'{' exp '}'", NULL, (Real)$2);
     if ($2 > VERDE_INT_MIN && $2 < VERDE_INT_MAX)
       append_to_buffer( &idrParsedData(), cntr++,
                         (Real)$2, (Integer)$2,
                         NULL, NULL );
     else
       append_to_buffer( &idrParsedData(), cntr++,
                         (Real)$2, VERDE_INT_MIN,
                         NULL, NULL );
     
     if (!UserInterface::instance()->aprepro_is_journaled())
     {
       journal_number((Real)$2);
     }
   }
          | IDENTIFIER '{' sexp '}'
   {
     debug_idr("datum", "IDENTIFIER '{' sexp '}'", IDR::instance()->identifier_string($1));
     append_to_buffer(&idrParsedData(), cntr++, 
                      VERDE_DBL_MAX, VERDE_INT_MAX,
                      $3,
                      IDR::instance()->identifier_string($1));
     FREESTR($3);
   }
          | qstring
   {
     debug_idr("datum", "qstring", idrStringBuffer);
     append_to_buffer( &idrParsedData(), cntr++, 
                       VERDE_DBL_MAX, VERDE_INT_MAX, 
                       idrStringBuffer, NULL );
     idrStringBuffer[0] = '\0';
   }
          | IDENTIFIER qstring
   {
     debug_idr("datum", "IDENTIFIER qstring", IDR::instance()->identifier_string($1));
     append_to_buffer( &idrParsedData(), cntr++, 
                       VERDE_DBL_MAX, VERDE_INT_MAX, 
                       idrStringBuffer, 
                       IDR::instance()->identifier_string($1) );
     idrStringBuffer[0] = '\0';
   }
          | IDENTIFIER EQUALS qstring
   {
     debug_idr("datum", "IDENTIFIER EQUALS qstring",
               IDR::instance()->identifier_string($1));
     append_to_buffer( &idrParsedData(), cntr++, 
                       VERDE_DBL_MAX, VERDE_INT_MAX, 
                       idrStringBuffer, 
                       IDR::instance()->identifier_string($1) );
     idrStringBuffer[0] = '\0';
   }
          | IDENTIFIER
   {
     debug_idr("datum", "IDENTIFIER", IDR::instance()->identifier_string($1));
     append_to_buffer( &idrParsedData(), cntr++, 
                       VERDE_DBL_MAX, VERDE_INT_MAX, 
                       NULL, IDR::instance()->identifier_string($1) );
   }
;

bool:     exp '<' exp           { $$ = $1 < $3;                         }
        | exp '>' exp           { $$ = $1 > $3;                         }
        | NOT exp               { $$ = !($2);                           }
        | exp LE  exp           { $$ = $1 <= $3;                        }
        | exp GE  exp           { $$ = $1 >= $3;                        }
        | exp EQ  exp           { $$ = $1 == $3;                        }
        | exp NE  exp           { $$ = $1 != $3;                        }
        | bool LOR bool         { $$ = $1 || $3;                        }
        | bool LAND bool        { $$ = $1 && $3;                        }
        | '(' bool ')'          { $$ = $2;                              }
;

bool:     sexp '<' sexp  {
                           $$ = (strcmp($1,$3) <  0 ? 1 : 0);
                           FREESTR($1);
                           FREESTR($3);
                         }
        | sexp '>' sexp  {
                           $$ = (strcmp($1,$3) >  0 ? 1 : 0);
                           FREESTR($1);
                           FREESTR($3);
                         }
        | sexp LE  sexp  {
                           $$ = (strcmp($1,$3) <=  0 ? 1 : 0);
                           FREESTR($1);
                           FREESTR($3);
                         }
        | sexp GE  sexp  {
                           $$ = (strcmp($1,$3) >=  0 ? 1 : 0);
                           FREESTR($1);
                           FREESTR($3);
                         }
        | sexp EQ  sexp  {
                           $$ = (strcmp($1,$3) ==  0 ? 1 : 0);
                           FREESTR($1);
                           FREESTR($3);
                         }
        | sexp NE  sexp  {
                           $$ = (strcmp($1,$3) !=  0 ? 1 : 0);
                           FREESTR($1);
                           FREESTR($3);
                         }
;

sexp:     QSTRING             { $$ = $1; }
        | SVAR                {
                                NEWSTR($1->value.svar, $$);
                              }
        | UNDVAR '=' sexp     {
                                $$ = $3;
                                NEWSTR($3, $1->value.svar);
                                $1->type = SVAR;
                              }
        | SVAR '=' sexp       {
                                $$ = $3;
                                FREESTR($1->value.svar);
                                NEWSTR($3, $1->value.svar);
                                redefined_warning($1->name);
                              }
        | VAR '=' sexp        {
                                $$ = $3;
                                NEWSTR($3, $1->value.svar);
                                redefined_warning($1->name);
                                $1->type = SVAR;
                              }
        | SFNCT '(' ')'       { $$ = (*($1->value.strfnct))(""); }
        | SFNCT '(' sexp ')'  {
                                $$ = (*($1->value.strfnct))($3);
                                FREESTR($3);
                              }
        | SDFNCT '(' exp ')'  {
                                $$ =
                                  (*($1->value.stdfnct))((Real)$3, NULL, NULL);
                              }
        | SDFNCT '(' exp ',' sexp ',' sexp ')'
                              {
                                $$ = (*($1->value.stdfnct))((Real)$3, $5, $7);
                                FREESTR($5);
                                FREESTR($7);
                              }
        | sexp CONCAT sexp    {
                                int len1 = strlen($1);
                                int len3 = strlen($3);
                                ALLOC($$, len1+len3+1, char *);
                                BCOPY($1, $$, len1+1);
                                strcat($$, $3);
                                FREESTR($1);
                                FREESTR($3);
                              }
;

exp:	  NUM               { $$ = $1; }
	| INC NUM           { $$ = $2 + 1; }
	| DEC NUM		{ $$ = $2 - 1; }
	| VAR               { $$ = $1->value.var; }
	| INC VAR		{ $$ = ++($2->value.var); }
	| DEC VAR		{ $$ = --($2->value.var); }
	| VAR INC		{ $$ = ($1->value.var)++; }
	| VAR DEC		{ $$ = ($1->value.var)--; }
	| VAR '=' exp       {
                                $$ = $3; $1->value.var = $3;
                                redefined_warning($1->name);
                              }
	| SVAR '=' exp      {
                                $$ = $3;
                                FREESTR($1->value.svar);
                                $1->value.var = $3;
                                redefined_warning($1->name);          
                                $1->type = VAR;
                              }
	| VAR EQ_PLUS exp	{ $1->value.var += $3; $$ = $1->value.var; }
	| VAR EQ_MINUS exp	{ $1->value.var -= $3; $$ = $1->value.var; }
	| VAR EQ_TIME exp	{ $1->value.var *= $3; $$ = $1->value.var; }
	| VAR EQ_DIV exp	{ $1->value.var /= $3; $$ = $1->value.var; }
	| VAR EQ_POW exp	{ errno = 0;
				  $1->value.var = pow($1->value.var,$3); 
				  $$ = $1->value.var; 
				  MATH_ERROR("Power");
				}
	| UNDVAR		{ $$ = $1->value.var;
				  undefined_warning($1->name);          }
	| INC UNDVAR		{ $$ = ++($2->value.var);		
				  $2->type = VAR;                       
				  undefined_warning($2->name);          }
	| DEC UNDVAR		{ $$ = --($2->value.var);		
				  $2->type = VAR;                       
				  undefined_warning($2->name);          }
	| UNDVAR INC		{ $$ = ($1->value.var)++;		
				  $1->type = VAR;                       
				  undefined_warning($1->name);          }
	| UNDVAR DEC		{ $$ = ($1->value.var)--;		
				  $1->type = VAR;                       
				  undefined_warning($1->name);          }
	| UNDVAR '=' exp	{ $$ = $3; $1->value.var = $3;
				  $1->type = VAR;                       }
	| UNDVAR EQ_PLUS exp	{ $1->value.var += $3; $$ = $1->value.var; 
				  $1->type = VAR;                       
				  undefined_warning($1->name);          }
	| UNDVAR EQ_MINUS exp	{ $1->value.var -= $3; $$ = $1->value.var; 
				  $1->type = VAR;                       
				  undefined_warning($1->name);          }
	| UNDVAR EQ_TIME exp	{ $1->value.var *= $3; $$ = $1->value.var; 
				  $1->type = VAR;                       
				  undefined_warning($1->name);          }
	| UNDVAR EQ_DIV exp	{ $1->value.var /= $3; $$ = $1->value.var; 
				  $1->type = VAR;                       
				  undefined_warning($1->name);          }
	| UNDVAR EQ_POW exp	{ errno = 0;
				  $1->value.var = pow($1->value.var,$3); 
				  $$ = $1->value.var; 
				  $1->type = VAR;                       
				  MATH_ERROR("Power");
				  undefined_warning($1->name);          }
	| FNCT '(' ')'		{ $$ = (*($1->value.fnctptr))(0.,0.,0.,0.);}
	| FNCT '(' exp ')'	{ $$ = (*($1->value.fnctptr))($3,0.,0.,0.);}

	| FNCT '(' exp ',' exp ')'
				{ $$ = (*($1->value.fnctptr))($3, $5,0.,0.);}
	| FNCT '(' exp ',' exp ',' exp')'
				{ $$ = (*($1->value.fnctptr))($3, $5, $7,0.); }
	| FNCT '(' exp ',' exp ';' exp ',' exp ')'
				{ $$ = (*($1->value.fnctptr))($3, $5, $7, $9); }
	| FNCTDI '(' ')'	{ $$ = (*($1->value.fnct_di))(0);}
	| FNCTDI '(' exp ')'	{ $$ = (*($1->value.fnct_di))((int)$3);}
	| FNCTDC '(' sexp ')'
                   {
                     $$ = (*($1->value.fnct_dc))($3, NULL);
                     FREESTR($3);
                   }
	| FNCTDC '(' sexp ',' sexp ')'
                   {
                     $$ = (*($1->value.fnct_dc))($3, $5);
                     FREESTR($3);
                     FREESTR($5);
                   }
	| exp '+' exp		{ $$ = $1 + $3;                         }
	| exp '-' exp		{ $$ = $1 - $3; 			}
	| exp '*' exp		{ $$ = $1 * $3; 			}
	| exp '/' exp		{ if ($3 == 0.)
				    {
				      yyerror("Zero divisor"); 
				      yyerrok;
				    }
				  else
				    $$ = $1 / $3; 			}
	| exp '%' exp		{ if ($3 == 0.)
				    {
				      yyerror("Zero divisor");
				      yyerrok;
				    }
				  else
				    $$ = (int)$1 % (int)$3;		}  
	| '-' exp %prec UNARY	{ $$ = -$2;				}
	| '+' exp %prec UNARY	{ $$ =  $2;				}
	| exp POW exp 		{ errno = 0;
				  $$ = pow($1, $3); 
				  MATH_ERROR("Power");			}
	| '(' exp ')'		{ $$ = $2;				}
        | '[' exp ']'           { errno = 0;
				  $$ = (double)($2 < 0 ? 
					-floor(-($2)): floor($2) );
				  MATH_ERROR("floor (int)");		}

        | bool { $$ = ($1) ? 1 : 0; }
        | bool '?' exp ':' exp  { $$ = ($1) ? ($3) : ($5);              }
;
%%

#include "idr_lexer.cpp"

void append_to_buffer (struct FunctionData* parsed_data,
                       Integer count,
                       Real rdata,
                       Integer idata,
                       const char* cdata,
                       const char* id_string )
{
    // Fixup cdata, if present
  if (cdata)
  {
    char save_buffer[IDR::MAX_STRING_LENGTH];
    save_string(save_buffer, cdata);
    cdata = save_buffer;
  }
  
    // Add it to the buffer
  parsed_data->append_to_buffer(count, rdata, idata, cdata, id_string);

    // Do journalling stuff.
    // If there was a number, this has the potential of being
    // replaced by a name string.  With no number, there is
    // no chance.
//  UserInterface::instance()->append_to_journal_string(parsed_data, count,
//                                                      idata != VERDE_INT_MAX ?
//                                                      VERDE_TRUE : VERDE_FALSE);
}

void debug_idr(const char* s1, const char* s2, const char* s3)
{
  if (DEBUG_FLAG(54))
  {
	  if (s3)
      PRINT_DEBUG(54,
                  "  idr write token \"%s\", \"%s\": %s\n",s1, s2, s3);
    else
      PRINT_DEBUG(54,
                  "  idr write token \"%s\", \"%s\"\n",s1, s2);

  }
}

void debug_idr_num(const char* s1, const char* s2, const char* s3, Real n)
{
  if (DEBUG_FLAG(54))
  {
	  if (s3)
      PRINT_DEBUG(54,
                  "  idr write token \"%s\", \"%s\": %s %g\n",s1, s2, s3, n);
    else
      PRINT_DEBUG(54,
                  "  idr write token \"%s\", \"%s\": %g\n",s1, s2, n);

  }
}

void process_idr_error()
{
  PRINT_DEBUG_NL;
  if( IDR::instance()->quit_on_error() )
    exit(EXIT_FAILURE);
  if( IDR::instance()->pause_on_error() )
  {
    PRINT_ERROR("Press return to continue");
    /*char key = */ getchar();
    PRINT_ERROR("\n");
  }
  IDR::instance()->signal_parse_error();
  reset_lex_state();
}

void save_string(char *to, const char *from)
{
    // Copies 'from' to 'to'. Strips off trailing whitespace (if any)
  int len = strlen(from);
  for (int i=len-1; i >= 0; i--)
  {
    if (from[i] != ' ' && from[i] != '\t' && from[i] != '\n')
      break;
    else
      len--;
  }
  int offset = 0;
  
    // Due to problems with the aprepro string handling, we need to
    // kludge the string here. If it comes in with quotes
    // ("string"), we strip them off.
  if (len >= 2)
  {
    if (from[0] == '\"' && from[len-1] == '\"' ||
        from[0] == '\'' && from[len-1] == '\'') {
      offset = 1;
      len-=2;
    }
  }
  strncpy(to, from+offset, len);
  to[len] = '\0';
}

void journal_number(Real val)
{
  UserInterface::instance()->append_to_journal_string(val);
}


void yyerror(const char* s)
{
  	PRINT_ERROR("%s (%s, line %d)\n",
              s, UserInterface::instance()->currentFile->filename.c_str(),
              UserInterface::instance()->currentFile->lineNumber );
		  
}

int yywrap()
{
  return 1;
}
