//- Class: VerdeString
//- 
//- Description: This file defines the VerdeString class which is a 
//- simple implementation of a character string. Basic functionality 
//- is provided as well as equality/inequality tests, subscripting into 
//- a string, searching a string for another string, and stream I/O.
//- This class was written by someone else, but I forget who and cannot
//- find where I got it from. I am still looking.
//-
//- Owner: Greg Sjaardema
//- Author: <unknown at this time, I am looking>
//- Checked by: Jim Hipp, August 31, 1994
//- Version: $Id: 

#if !defined(STRING_HPP)
#define STRING_HPP

#include <assert.h>
#include <stdlib.h>
#ifdef CANT_USE_STD_IO
#include <iostream.h>
#else
#include <iostream>
#endif

class VerdeStringRep;

const size_t MAX_POS = (size_t) -1;    // "SIZE_T_MAX"

//! simple implemention of a character string
class VerdeString
{
public:

   //! default constructor
   VerdeString();

   //! copy Constructor
   VerdeString(const VerdeString& s);

   //! create a string from a char*
   VerdeString(const char *s);

   //! create a string from a integer
   VerdeString(const int i);

   //! create a string from a double.  
   //! use either fixed point or scientific notation, whichever is shorter.
   //! s_length is the maximum string length: If s_length > 0, then
   //! string will contain no spaces and be close to s_length long
   //! without going over. Hence precision is variable.
//   VerdeString(const double f, const unsigned int s_length = 0);

   //! default destructor
   ~VerdeString();

   //! assignment
   VerdeString& operator=(const VerdeString& s);
  
   //! concatenation function
   VerdeString& operator+=(const VerdeString& s);

   //! concatenation function
   VerdeString& operator+=(const char *c);

   //! concatenation function
   friend VerdeString operator+(const VerdeString& s1, const VerdeString& s2);

   //! concatenation function
   friend VerdeString operator+(const VerdeString& s1, const char *c2);

   //! concatenation function
   friend VerdeString operator+(const char *c1, const VerdeString& s2);

   //! comparison equality/inequality with other VerdeStrings 
   friend int operator<=(const VerdeString&, const VerdeString&);

   //! comparison equality/inequality with other VerdeStrings 
   friend int operator>=(const VerdeString&, const VerdeString&);
  
   //! comparison equality/inequality with other VerdeStrings 
   int operator==(const VerdeString &s) const;

   //! comparison equality/inequality with other VerdeStrings 
   int operator!=(const VerdeString &s) const;

   //! comparison equality/inequality with other VerdeStrings 
   int operator==(const char *s) const;

   //! comparison equality/inequality with other VerdeStrings 
   int operator!=(const char *s) const;
  
   //! Subscripting function
   char get_at(size_t pos) const;

   //! Subscripting function
   void put_at(size_t pos, char c);

   //! Subscripting function
   VerdeString substr(size_t pos, size_t n = MAX_POS) const;
  
   //! Searching function
   size_t find(const VerdeString& s, size_t pos = 0) const;

   //! Searching function
   size_t find_first_of(const VerdeString& s, size_t pos = 0) const;

   //! Searching function
   size_t find_first_not_of(const VerdeString& s, size_t pos = 0) const;
  
   //! I/O function
   friend std::ostream& operator<<(std::ostream&, const VerdeString&);
   //! I/O function
   friend std::istream& operator>>(std::istream& is, VerdeString&);

   //! returns string length
   size_t length() const;
   
   //! converts to a const char *
   const char *c_str() const;
   
   //! converts string to upper
   void to_upper_case(unsigned int n = 0);
  
private:
  
  //! specialized class for the implementation
  VerdeStringRep *rep;

};

#endif
