#include <math.h>
#include "idr.hpp"
#include "idr_parser.h"
#include "builtin.h"
#include <string.h>
#include <stdlib.h>
#include "UserInterface.hpp"
#include "VerdeString.hpp"

struct init
{
  const char *fname;
  double (*fnct) (double, double, double, double);
};

struct init_di
{
  const char *fname;
  double (*fnct) (int);
};

struct init_dc
{
  const char *fname;
  double (*fnct) (const char *, const char *);
};

struct str_init
{
  const char *fname;
  char *(*fnct) (const char *);
};

struct strd_init
{
  const char *fname;
  char *(*fnct) (double, const char *, const char *);
};

struct var_init
{
  const char *vname;
  double value;
};

struct svar_init
{
  const char *vname;
  const char *value;
};


static struct init arith_fncts[] =
{
  {"Vangle", do_angle},
  {"Vangled", do_angled},
  {"abs", do_fabs},
  {"acos", do_acos},
  {"acosd", do_acosd},
  {"acosh", do_acosh},
  {"asin", do_asin},
  {"asind", do_asind},
  {"asinh", do_asinh},
  {"atan", do_atan},
  {"atan2", do_atan2},
  {"atan2d", do_atan2d},
  {"atand", do_atand},
  {"atanh", do_atanh},
  {"ceil", do_ceil},
  {"cos", do_cos},
  {"cosd", do_cosd},
  {"cosh", do_cosh},
  {"d2r", do_d2r},
  {"dim", do_dim},
  {"dist", do_dist},
  {"exp", do_exp},
  {"floor", do_floor},
  {"fmod", do_fmod},
  {"hypot", do_hypot},
  {"int", do_int},
  {"julday", do_julday},
  {"lgamma", do_lgamma},
  {"ln", do_log},
  {"log", do_log},
  {"log10", do_log10},
  {"log1p", do_log1p},
  {"max", do_max},
  {"min", do_min},
  {"polarX", do_polarX},
  {"polarY", do_polarY},
  {"r2d", do_r2d},
  {"rand", do_rand},
  {"sign", do_sign},
  {"sin", do_sin},
  {"sind", do_sind},
  {"sinh", do_sinh},
  {"sqrt", do_sqrt},
  {"tan", do_tan},
  {"tand", do_tand},
  {"tanh", do_tanh},
  {0, 0}				/* Last line must be 0, 0 */
};

static struct init_dc fncts_dc[] =
{
  //{"Id", do_Id},
  {"word_count", do_word_count},
  {"strtod", do_strtod},
  {0, 0}				/* Last line must be 0, 0 */
};
  /*
static struct init_di fncts_di[] =
{
  {"Vx", do_Vx},
  {"Vy", do_Vy},
  {"Vz", do_Vz},
  {"Nx", do_Nx},
  {"Ny", do_Ny},
  {"Nz", do_Nz},
  {"Length", do_Length},
  {"IntNum", do_IntNum},
  {"IntSize", do_IntSize},
  {"Radius", do_Radius},
  {"get_error_count", do_get_error_count},
  {"set_error_count", do_set_error_count},
  {"get_warning_count", do_get_warning_count},
  {"set_warning_count", do_set_warning_count},
  {0, 0}*/				/* Last line must be 0, 0 */
/*
};
*/
static struct str_init string_fncts[] =
{
  {"tolower", do_tolower},
  {"toupper", do_toupper},
  {"execute", do_execute},
  {"rescan", do_rescan},
  {"Units", do_Units}, 
  {"getenv", do_getenv},
  {"error", do_error},
  //{"Type", do_Type},
  {"DUMP", do_dumpsym},
  {0, 0}				/* Last line must be 0, 0 */
};

static struct strd_init string_fnctd[] =
{
  {"tostring", do_tostring},
  {"get_word", do_get_word},
  {0, 0}				/* Last line must be 0, 0 */
};

static struct var_init variables[] =
{
  {"VERDE", 1},                         /* So user can do ifdef(VERDE)*/
  {"DEG",   57.2957795130823208768},	/* 180/pi, degrees per radian */
  {"RAD",   0.01745329251994329576},    /* pi/180, radians per degree */
  {"E",     2.71828182845904523536},	/* e, base of natural log     */
  {"GAMMA", 0.57721566490153286060},	/* euler-mascheroni constant  */
  {"PHI",   1.61803398874989484820},    /* golden ratio               */
  {"PI",    3.14159265358979323846},	/* pi                         */
  {"PI_2",  1.57079632679489661923},	/* pi / 2                     */
  {"SQRT2", 1.41421356237309504880},	/* square root of 2           */
  {0, 0}			        /* Last line must be 0, 0     */
};

static struct svar_init svariables[] =
{
  {"_FORMAT", "%.10g"},		/* Default output format */
  {"_C_", "#"},                   /* Comment character */
  {0, 0}				/* Last line must be 0, 0 */
};

void define_idr_variable(const char *string)
{
  char* s_cpy = new char[strlen(string) + 1];
  strcpy(s_cpy, string);
  char *variable = strtok(s_cpy, "=");
  char *value    = strtok(NULL, "=");
  if (strchr(value, '"'))
  {
    char *pt = strrchr(value, '"');
    *pt = '\0';
    value++;
    symrec *s = putsym(variable, SVAR);
    NEWSTR(value, s->value.svar);
  }
  else
  {
    double val;
    sscanf (value, "%lf", &val);
    symrec *s = putsym (variable, VAR);
    s->value.var = val;
  }
  delete [] s_cpy;
}

// Define an idr variable for use in correlating FASTQ and VERDE entities.
// For example, define_idr_variable("point", 1, 2)
// defines the variable point1 with the value 2 which means that FASTQ 
// point 1 is the same as VERDE vertex 2.
void define_idr_variable(const char *entity, int fastq_id, int verde_id)
{
  static char variable[64];
  sprintf(variable, "%s%d", entity, fastq_id);
  symrec *s = putsym (variable, VAR);
  s->value.var = (double)verde_id;
}

void aprepro_init ()
{
  int i;
  symrec *ptr;
  for (i = 0; arith_fncts[i].fname != 0; i++)
  {
    ptr = putsym (arith_fncts[i].fname, FNCT);
    ptr->value.fnctptr = arith_fncts[i].fnct;
  }
  for (i = 0; fncts_dc[i].fname != 0; i++)
  {
    ptr = putsym (fncts_dc[i].fname, FNCTDC);
    ptr->value.fnct_dc = fncts_dc[i].fnct;
  }
/*  for (i = 0; fncts_di[i].fname != 0; i++)
  {
    ptr = putsym (fncts_di[i].fname, FNCTDI);
    ptr->value.fnct_di = fncts_di[i].fnct;
  }
  */
  for (i = 0; string_fncts[i].fname != 0; i++)
  {
    ptr = putsym (string_fncts[i].fname, SFNCT);
    ptr->value.strfnct = string_fncts[i].fnct;
  }
  for (i = 0; string_fnctd[i].fname != 0; i++)
  {
    ptr = putsym (string_fnctd[i].fname, SDFNCT);
    ptr->value.stdfnct = string_fnctd[i].fnct;
  }
  for (i = 0; variables[i].vname != 0; i++)
  {
    ptr = putsym (variables[i].vname, VAR);
    ptr->value.var = variables[i].value;
  }
  for (i = 0; svariables[i].vname != 0; i++)
  {
    ptr = putsym (svariables[i].vname, SVAR);
    NEWSTR(svariables[i].value, ptr->value.svar);
  }
  /*VerdeString version_string = UserInterface::instance()->version();
  char *version = new char[version_string.length()+1];
  strcpy(version, version_string.c_str());
  ptr = putsym("VERSION", SVAR);
  ptr->value.svar = version;*/
}

const unsigned HASHSIZE = 113;

unsigned hash (const char *symbol)
{
  unsigned hashval;
  for (hashval = 0; *symbol != '\0'; symbol++)
    hashval = *symbol + 65599 * hashval;
  return (hashval % HASHSIZE);
}

static symrec *sym_table[HASHSIZE];

symrec* putsym (const char *sym_name, int sym_type)
{
  symrec *ptr;
  unsigned hashval;
  
  ptr = (symrec *) malloc (sizeof (symrec));
  if (ptr == NULL)
    return NULL;
  NEWSTR (sym_name, ptr->name);
  ptr->type = sym_type;
  ptr->value.var = 0;
  
  hashval = hash (ptr->name);
  ptr->next = sym_table[hashval];
  sym_table[hashval] = ptr;
  return ptr;
}

symrec* getsym (const char *sym_name)
{
  symrec *ptr;
  for (ptr = sym_table[hash (sym_name)]; ptr != NULL; ptr = ptr->next)
    if (strcmp (ptr->name, sym_name) == 0)
      return ptr;
  return NULL;
}

char* do_dumpsym (const char *)
{
  symrec *ptr;
  unsigned hashval;
  
  PRINT_INFO("\n#   Variable = Value\n");
  for (hashval = 0; hashval < HASHSIZE; hashval++)
  {
    for (ptr = sym_table[hashval]; ptr != (symrec *) 0; ptr = ptr->next)
    {
      if (ptr->type == VAR)
        PRINT_INFO("#  {%s\t= %.10g}\n", ptr->name, ptr->value.var);
      else if (ptr->type == SVAR)
        PRINT_INFO("#  {%s\t= \"%s\"}\n", ptr->name, ptr->value.svar);
    }
  }
  char* tmp;
  NEWSTR("", tmp);
  return tmp;
}

//  Push the contents of 'string' onto the stack to be reread.
//  'string' will be surrounded by {} so it must be a valid expression.
char* do_execute (const char *string)
{
  int i;
  
//  NOTE: The closing } has not yet been scanned in the call to execute();
//        therefore, we put that back on the stack last (to be read first),
//        then push our beginning {, then push the string.  The unread }
//        becomes our closing }.
  
  for (i = strlen (string) - 1; i >= 0; --i)
  UserInterface::instance()->unget_input(string[i]); 
  UserInterface::instance()->unget_input ('{');
  UserInterface::instance()->unget_input ('}');
  char* tmp;
  NEWSTR("", tmp);
  return tmp;
}

// Push the contents of 'string' onto the stack to be reread.
// 'string' will not be surrounded by {}.
char* do_rescan (const char* string)
{
  int i;
  
// NOTE: The closing } has not yet been scanned in the call to execute();
//        therefore, we read it ourselves using input(), then we push our
//        string and then put the closing } back on the stack last
//        (to be read first),
  
    // eat up everything until the closing brace.
  while ((i = UserInterface::instance()->get_input ()) != '}' && i != 0);
  
  if (i == 0)
  {
    yyerror ("Aprepro: ERR: End-of-file in rescan() command");
  }
  for (i = strlen (string) - 1; i >= 0; --i)
    UserInterface::instance()->unget_input (string[i]);
  UserInterface::instance()->unget_input ('}');
  char* tmp;
  NEWSTR("", tmp);
  return tmp;
}


