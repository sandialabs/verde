
#ifndef MYTYPES
#define MYTYPES
#include <errno.h>

/* typedefs to allow easy modification of word size for a particular application
 * or computer */

typedef unsigned int	UInteger;
typedef int 		Integer;
/* NOTE: If the following typedef is changed, also change the VERDE_DBL_MAX
   values in idr_parser.y */
typedef double		Real;
typedef unsigned char	UByte;
typedef void		Void;

#ifndef TRUE
#define TRUE 1
#endif
 
#ifndef FALSE
#define FALSE 0
#endif
 
#ifndef NULL
#define NULL 0
#endif

#if defined(__STDC__) || defined(__cplusplus)
#if defined PROTO
#undef PROTO
#endif
#define PROTO(proto) proto
#else
#define PROTO(proto) ()
#endif

struct symrec
{
	char *name;
	int   type;
	union {
		double var;
		double (*fnctptr)(double, double, double, double);
		char *svar;
		// Todo:  replace with original??
		//char *(*strfnct)(char *);
		char *(*strfnct)(const char *);
			// Todo:  replace with original???
			//char *(*stdfnct)(double, char *, char *);
	        char *(*stdfnct)(double, const char *, const char *);
	        double (*fnct_di)(int);
			// Todo:  replace with original??
			//double (*fnct_dc)(char *, char *);
	        double (*fnct_dc)(const char *, const char *);
	} value;
	struct symrec *next;
};

typedef struct symrec symrec;
/* these should be defined in ANSI C, and probably C++, but just in case ... */

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

/* Code for saving quoted strings into allocated memory 
   From: "Lex and Yacc," O'Reilly and Associates, p. 95
*/
extern symrec	*putsym PROTO((const char*, int));
extern symrec	*getsym PROTO((const char*));

#define ALLOC(x,s,t)	do { x = (t)calloc(1, (s)); \
			       if (x == (t)0)  { \
				 yyerror("memory allocation failed");\
					   exit(EXIT_FAILURE); }} while(0)
#if defined(SYSV) || defined(__STDC__) || defined(__cplusplus)
#define BCOPY(from,to,size)	memcpy(to,from,size)
#else
#define BCOPY(from,to,size)	bcopy(from,to,size)
#endif

#define FREESTR(x) \
          do { \
            free(x); \
          } while(0)


/* NOTE: the while(0) loop causes lint to return a warning: constant in 
         conditional context.  However, it is valid C and is used to 
	 create a block so we don't get into bad nesting problems.  It
	 also lets the user terminate the macro with a ;
*/
#define NEWSTR(from,to)	do { int len=strlen(from);\
			       ALLOC(to, len+1, char *);\
			       BCOPY(from,to,len+1);\
			       } while(0)

#define OPEN_FILE(pointer, file, mode) do { \
			 errno = 0;\
			 if ((pointer = fopen(file, mode)) == NULL) \
			   {  char tmpstr[128];\
			      sprintf(tmpstr, "Aprepro: ERR:  Can't open '%s'",file); \
			      perror(tmpstr);\
			      exit(EXIT_FAILURE);} \
			      } while(0)

#define SET_FILE_LIST(file_num, line, temp_file, loop_cnt) do { \
				file_list[file_num].lineno = line; \
				file_list[file_num].tmp_file = temp_file; \
				if (loop_cnt >= 0) \
				  file_list[file_num].loop_count = loop_cnt; \
			     } while(0)

#define MATH_ERROR(function) do { \
				  if (errno != 0) \
				    yyerror(function); \
				  if (errno == EDOM) \
				    perror("	DOMAIN error"); \
				  else if (errno == ERANGE) \
				    perror("	RANGE error"); \
				  else if (errno != 0) \
				    perror("	Unknown error"); \
				    } while (0)

#endif

