//-------------------------------------------------------------------------
// Filename      : VerdeUtil.hpp
//
// Purpose       : This file contains utility functions that can be used
//                 throughout Verde.
//
// Special Notes : This is a pure virtual class, to prevent instantiation.
//                 All functions are static, called like this:
//                 VerdeUtil::function_name();
//
// Creator       : Darryl Melander
//
// Date          : 06/08/98
//
// Owner         : Darryl Melander
//-------------------------------------------------------------------------


#ifndef VERDE_UTIL_HPP
#define VERDE_UTIL_HPP

#include "VerdeDefines.hpp"
#include <string.h>

//! utility function to be use throughout VERDE
class VerdeUtil
{
public:
   
   //! converts string to lowercase
   static void convert_string_to_lowercase (char *string);

   //! insensitive case comparison
   static int  strcmp_case_insensitive     (const char *s1, const char *s2);

   //! insensitive # character comparison of a string
   static int  strncmp_case_insensitive    (const char *s1, const char *s2,                                            int n);

   static VerdeBoolean is_double (char *token, double &value);
   static VerdeBoolean is_integer (char *token, int &value);
   static VerdeBoolean is_on_off (char *token, VerdeBoolean &value);

   static int compare( const char* a, const char* b)
	{ return ( a != NULL && b !=NULL && 0 == strcmp(a,b)); }

private:
   VerdeUtil(){};
};

#endif
