// VerdeString.cpp

#include <stddef.h>
#include <stdio.h>

#ifdef CANT_USE_STD_IO
#include <iostream.h>
#else
#include <iostream>
#endif

#ifdef CANT_USE_STD_IO
#include <iomanip.h>
#else
#include <iomanip>
#endif

#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "VerdeString.hpp"

class VerdeStringRep {
private:
   friend class VerdeString;
   int    refCount;
   char*  chars;
   
   VerdeStringRep(const char*);
   VerdeStringRep(char*, int);
  //- NOTE: The above function is specialized for the implementation
  //-       It USES the pointer passed to it instead of allocating and
  //-       copying.

   VerdeStringRep(const VerdeStringRep &s);
   ~VerdeStringRep();
   
   void increment();
   void decrement();
   
   const VerdeStringRep& operator=(const VerdeStringRep &s);
};

VerdeStringRep::VerdeStringRep(const char *cp)
: refCount(0), chars(new char[strlen(cp) +1]) {
  strcpy(chars, cp);
}

VerdeStringRep::VerdeStringRep(char *cp, int)
: refCount(0), chars(cp) {}

VerdeStringRep::~VerdeStringRep()
{
    delete [] chars;
}

void VerdeStringRep::increment() 
{
   ++refCount;
}

void VerdeStringRep::decrement()
{
   if (--refCount == 0)
      delete this;
}

VerdeStringRep::VerdeStringRep(const VerdeStringRep &orig)
: refCount(0), chars(new char[strlen(orig.chars)+1]) {
  strcpy(chars, orig.chars);
}

const VerdeStringRep& VerdeStringRep::operator=(const VerdeStringRep &orig)
{
    if (this != &orig) {
       delete [] chars;
       chars = new char[strlen(orig.chars)+1];
       strcpy(chars, orig.chars);
    }
    return *this;
}


VerdeString::VerdeString()
: rep(new VerdeStringRep("")) {
  rep->increment();
}

VerdeString::VerdeString(const VerdeString& s)
: rep(s.rep) {
  rep->increment();
}


VerdeString::~VerdeString()
{
    rep->decrement();
}

VerdeString::VerdeString(const char *s)
: rep(new VerdeStringRep(s)) {
  rep->increment();
}

VerdeString::VerdeString(const int i)
{
  char si[32];
  sprintf(si, "%d", i);
  rep = new VerdeStringRep(si);
  rep->increment();
}

/*
VerdeString::VerdeString(const double f, const unsigned int s_length)
{
  assert( s_length < 1000 );
  char *si = new char[ 2*s_length + 32 ];
  sprintf(si, "%e", f);
    // change precision to be short enough
  if (s_length) {
    if ( strlen( si ) > s_length ) {
      char format_string[8];
      char digit_string[4];
      //char *format_string = "%.xxxlg";
      //char *digit_string = "xxx";
      unsigned int num_digits = s_length;
      do {        
          // create format string - specify one fewer digit
        num_digits--;
        strcpy( format_string, "%." );
        sprintf( digit_string, "%d", num_digits );
        strcat( format_string, digit_string );
        strcat( format_string, "lg" );
        
          // create output string
        sprintf( si, format_string, f );
      } while( num_digits > 0 && strlen( si ) > s_length );
    }
  }
  rep = new VerdeStringRep(si);
  rep->increment();
  delete [] si;
}
*/



VerdeString& VerdeString::operator=(const VerdeString& s)
{
    if (rep != s.rep)
    {
        rep->decrement();
        rep = s.rep;
        rep->increment();
    }
    return *this;
}

char VerdeString::get_at(size_t pos) const
{
    //assert(pos < strlen(rep->chars));
    return rep->chars[pos];
}

size_t VerdeString::length() const
{
    return strlen(rep->chars);
}

const char * VerdeString::c_str() const
{
    //assert(this != NULL);  // <--- Add this line
    return rep->chars;
}

VerdeString operator+(const VerdeString& s1, const VerdeString& s2)
{
    return VerdeString(s1) += s2;
}

VerdeString operator+(const VerdeString& s1, const char *c2)
{
    return VerdeString(s1) += c2;
}

VerdeString operator+(const char *c1, const VerdeString& s2)
{
    return VerdeString(c1) += s2;
}

VerdeString& VerdeString::operator+=(const VerdeString& s)
{
  if (s.length() > 0)
    {
      int new_count = length() + s.length();
      char *buf = new char[new_count + 1];

      memcpy(buf,rep->chars,length());
      memcpy(buf+length(),s.rep->chars,s.length());
      buf[new_count] = '\0';
      rep->decrement();
      rep = new VerdeStringRep(buf, 1);
      rep->increment();
    }
  return *this;
}

VerdeString& VerdeString::operator+=(const char *c)
{
  size_t c_length;
  if ((c_length = strlen(c)) > 0)
    {
      size_t new_count = length() + c_length;
      char *buf = new char[new_count + 1];

      memcpy(buf,rep->chars,length());
      memcpy(buf+length(),c,c_length);
      buf[new_count] = '\0';
      rep->decrement();
      rep = new VerdeStringRep(buf, 1);
      rep->increment();
    }
  return *this;
}

size_t VerdeString::find(const VerdeString& s, size_t pos) const
{
    assert(pos < length());
    char *p = strstr(rep->chars,s.c_str());
    if (p)
        return pos + (p - rep->chars);
    return MAX_POS;
}

size_t VerdeString::find_first_of(const VerdeString& s, size_t pos) const
{
    assert(pos < length());
    char *p = strpbrk(rep->chars+pos,s.rep->chars);
    if (p)
        return p - rep->chars;
    return MAX_POS;
}

size_t VerdeString::find_first_not_of(const VerdeString& s, size_t pos)
const
{
    assert(pos < length());
    for (size_t i = pos; i < length(); ++i)
        if (strchr(s.rep->chars,rep->chars[i]) == NULL)
            return i;
    return MAX_POS;
}

VerdeString VerdeString::substr(size_t pos, size_t n) const
{
    assert(pos < length());
    if (n > length() - pos)
        n = length() - pos;

    if (n > 0) {
      char *substring = new char[n+1];
      strncpy(substring, rep->chars+pos, n);
      substring[n] = '\0';
      VerdeString tmp(substring);
      delete [] substring;
      return tmp;
    } else {
        return VerdeString();    // Empty VerdeString
    }
}

std::ostream& operator<<(std::ostream &os, const VerdeString &s)
{
    unsigned int width = os.width();
    width = (width == 0 ? s.length() : width);
    if (s.length() <= width) {
      os.write(s.c_str(),s.length());
      for (unsigned int i=s.length(); i<width; i++)
	os.put(' ');
    }
    else {
      os.write(s.c_str(),width);
    }
    return os;
}

void VerdeString::to_upper_case(unsigned int n)
{
  if(n > length() || n == 0)
     n = length();
  
  for(size_t pos=0; pos < n; pos++)
  {
    char c = toupper(rep->chars[pos]);
    rep->chars[pos] = c;
  }
}

  
void VerdeString::put_at(size_t pos, char c)
{
  assert(pos < length());
  if (rep->chars[pos] != c)  {
		
    if (pos < length())  {
      if (rep->refCount != 1) {
	VerdeStringRep *new_rep = new VerdeStringRep(rep->chars);
	rep->decrement();
	rep = new_rep;
	rep->increment();
      }
      rep->chars[pos] = c;
    } else {
      // Append character the lazy way
      char buf[2];
      buf[0] = c;
      buf[1] = '\0';
      operator+=(buf);
    }
  }
}

int VerdeString::operator==(const VerdeString& s2) const
{
  return strcmp(rep->chars, s2.rep->chars) == 0;
}

int VerdeString::operator!=(const VerdeString& s2) const
{
  return strcmp(rep->chars, s2.rep->chars) != 0;
}

int VerdeString::operator==(const char *s2) const
{
  return strcmp(rep->chars, s2) == 0;
}

int VerdeString::operator!=(const char *s2) const
{
  return strcmp(rep->chars, s2) != 0;
}

int operator<=(const VerdeString& s1, const VerdeString& s2)
{
  return strcmp(s1.c_str(), s2.c_str()) <= 0;
}

int operator>=(const VerdeString& s1, const VerdeString& s2)
{
  return strcmp(s1.c_str(), s2.c_str()) >= 0;
}


#ifdef TEST_STRING

void main() {
VerdeString a = "Test ";
VerdeString b = "String Class";
VerdeString blank(' ');

VerdeString c = a + b;
c+= b;
VerdeString e = c;
VerdeString f = c;
VerdeString g = c;
g.put_at(1, 'Z');
cout << "G = " << g << endl;
cout << "C = " << c << endl;

c.put_at(1, 'X');
VerdeString d = b;
d.put_at(1, 'Y');
}
#endif
