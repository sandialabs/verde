//- Class:       VerdeDefines 
//- Description: VerdeDefines class - all global definitions for VERDE.
//- Owner:       Tim Tautges
//- Checked by:
//- Version: $Id: 

#ifndef VERDEOBJECT_HPP
#define VERDEOBJECT_HPP

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <sys/types.h>
#include <assert.h>

// sanity check defines (these ARE needed!)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

// typedef for integers and flags of just a few bits.
// Usage: "Bit isMarked : 1;" where 1 is bit width of isMarked.
// If there are just a few small flags in a class, cBit and IttyBit might
// save some space; the length of int and short are machine dependent,
// but char is as short as possible on all platforms.
typedef unsigned int Bit;
//typedef unsigned short IttyBit;
typedef unsigned char cBit;

//static int const INVALID_ENTITY_ID = -1 ;

// Boolean flags.
enum VerdeBoolean { VERDE_FALSE = 0, VERDE_TRUE = 1} ;

// Completion status
enum VerdeStatus { VERDE_FAILURE = 0, VERDE_SUCCESS = 1 } ;

// Flags that indicate the Forward and Reversed senses.
//enum VerdeSense {VERDE_UNKNOWN = -1, VERDE_FORWARD = 0, VERDE_REVERSED = 1};

// #define's for the entire VERDE system

#ifdef M_PI
const double VERDE_PI           =              M_PI;
#else
const double VERDE_PI           =              3.1415926535897932384626;
#endif
#define DEGREES_TO_RADIANS(angle) ( (angle) / 180.0 * VERDE_PI )


#ifdef INT_MAX
const int VERDE_INT_MAX = INT_MAX;
#else
const int VERDE_INT_MAX = 2147483647;
#endif

#ifdef INT_MIN
const int VERDE_INT_MIN = INT_MIN;
#else
const int VERDE_INT_MIN = -2147483647;
#endif

#ifdef DBL_MIN
const double VERDE_DBL_MIN      =              DBL_MIN;
#else
const double VERDE_DBL_MIN      =              1.0E-30;
#endif

#ifdef DBL_MAX
const double VERDE_DBL_MAX      =              DBL_MAX;
#else
const double VERDE_DBL_MAX      =              1.0E30;
#endif

#define VERDE_MIN(a,b)                   ( (a) < (b) ? (a) : (b) )
#define VERDE_MAX(a,b)                   ( (a) > (b) ? (a) : (b) )

#define VERDE_MIN_4(a,b,c,d)             (( (a) < (b) ? (a) : (b) ) < \
                                          ( (c) < (d) ? (c) : (d) ) ? \
                                          ( (a) < (b) ? (a) : (b) ) : \
                                          ( (c) < (d) ? (c) : (d) ))

#define VERDE_MAX_4(a,b,c,d)             (( (a) > (b) ? (a) : (b) ) > \
                                          ( (c) > (d) ? (c) : (d) ) ? \
                                          ( (a) > (b) ? (a) : (b) ) : \
                                          ( (c) > (d) ? (c) : (d) ))
struct min_max 
{
   double min;
   double max;
};


#endif
