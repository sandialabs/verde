//-------------------------------------------------------------------------
// Filename      : VerdeUtil.cc 
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

#include "VerdeUtil.hpp"
#include <ctype.h>
#include <string.h>

void VerdeUtil::convert_string_to_lowercase(char *string)
{
   register char *p = string;
   while (*p)
   {
      if (isupper(*p))
         *p = tolower(*p);
      p++;
   }
}

int VerdeUtil::strcmp_case_insensitive (const char *s1, const char *s2)
{
   char c1, c2;
   
   do
   {
      c1 = *s1++;
      if(isupper(c1))
         c1 = tolower(c1);
      
      c2 = *s2++;
      if(isupper(c2))
         c2 = tolower(c2);
      
      if(c1 != c2)
         return c1 - c2;
      
   } while(c1 != '\0');
   
   return 0;
}

int VerdeUtil::strncmp_case_insensitive (const char *s1, const char *s2,
                                         int n)
{
   char c1, c2;
   
   do
   {
      c1 = *s1++;
      if(isupper(c1))
         c1 = tolower(c1);
      
      c2 = *s2++;
      if(isupper(c2))
         c2 = tolower(c2);
      
      if(c1 != c2)
         return c1 - c2;
      
      n--;
   } while(c1 && n > 0);
   
   return 0;
}
VerdeBoolean VerdeUtil::is_double(char *token, double &value)
{
  static const char *numbers = "-.0123456789";

  if (strcspn(token,numbers) == 0)
  {
    value = atof(token);
    return VERDE_TRUE;
  }
  else
  {
    value = 0.0;
    return VERDE_FALSE;
  }
  
}

VerdeBoolean VerdeUtil::is_integer(char *token, int &value)
{
  static const char *numbers = "-0123456789";

  if (strcspn(token,numbers) == 0)
  {
    value = atoi(token);
    return VERDE_TRUE;
  }
  else
  {
    value = 0;
    return VERDE_FALSE;
  }
  
}

VerdeBoolean VerdeUtil::is_on_off(char *token, VerdeBoolean &value)
{
  if (strcmp(token,"TRUE") == 0 ||
      strcmp(token,"true") == 0 ||
      strcmp(token,"ON") == 0   ||
      strcmp(token,"on") == 0 )
  {
    value = VERDE_TRUE;
    return VERDE_TRUE;
  }
  if (strcmp(token,"FALSE") == 0 ||
      strcmp(token,"false") == 0 ||
      strcmp(token,"OFF") == 0   ||
      strcmp(token,"off") == 0 )
  {
    value = VERDE_FALSE;
    return VERDE_TRUE;
  }
  return VERDE_FALSE;
}
