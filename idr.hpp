//********************************************************************
//- File:        idr.hpp
//-
//- Description: Handles parsing related operations, such as matching
//-              keywords and identifiers, storing parsed data,
//-              and calling appropriate command handlers.
//-              IDR stands for Input Deck Reader.
//-
//- Owner:       Darryl Melander
//********************************************************************

#ifndef IDR_HPP
#define IDR_HPP

#include "VerdeDefines.hpp"
#include "idr_types.h"

// #ifdef LINUX
// extern "C" int yylex();
// #endif

// #if defined (osf1) || defined (LINUX)
// static void finalize_putback();
// #endif

#if defined (NT) || defined (HP) || defined (__SunOS_5_7) || defined (__SunOS_5_8)
extern "C" {
#endif
  extern void yyerror(const char* s);
#if defined (NT) || defined (HP) || defined (__SunOS_5_7) || defined (__SunOS_5_8)
}
#endif


//! structure to hold data returned from parser to keyword handler functions
struct FunctionData
{
  int	 keywordId;
  int	 dataListLength;
  int*     integerData;
  Real*    realData;
  char**   characterData;
  char**   keywordIdentifier;
  
  FunctionData()
      : keywordId(-1),
        dataListLength(0),
        integerData(NULL),
        realData(NULL),
        characterData(NULL),
        keywordIdentifier(NULL),
        bufferLength(0)
    {}
  ~FunctionData();

  void append_to_buffer(Integer count,
                        Real rdata,
                        Integer idata,
                        const char* cdata,
                        const char* id_string);
  void grow_data_buffers(unsigned short req_size);
  void clean_out();
  
private:
  unsigned short bufferLength;
};

struct KeywordHandler;

//! Input Deck Reader. Handles parsing related operations.
class IDR
{
public:
  
    /*! IDR-specific constants.  Done with an
    // enum because some compilers can't handle
    // constants with a class scope. */
  enum IdrConstant
  {
      // Special return values for word searches
    ERROR_INSUFFICIENT_INFO = -1,
    ERROR_NO_KEYWORD_MATCH  = -2,
    MATCH_ANYTHING          = -3,
    ALTERNATIVE_ID_BASE     = -4,
      // Other IDR constants
    MAX_IDENTIFIERS         = 400,
    MAX_STRING_LENGTH       = 128,
    MAX_KEYWORD_STR_LEN     = 30
  };
  
  static IDR* instance();
  static void delete_instance();
  
  static void case_convert(char* string);
  
  //!Search for the best match for 'keyword_string'.
  int begin_keyword_search(const char* keyword_string);
  /*! Check to see if there are more words in the most recently
  // matched keyword. */
  VerdeBoolean keyword_search_complete();
  //! Check additional words in the most recently matched keyword
  int continue_keyword_search(const char* keyword_string);
  //! See if the given ID refers to a valid keyword
  VerdeBoolean is_keyword_valid(int keyword_id);
  
  //! Search for the best match for 'identifier_string'.
  int begin_identifier_search(const char* identifier_string);
  /*! Check to see if there are more words in the most recently
      matched identifier. */
  VerdeBoolean identifier_search_complete();
  //! Check additional words in the most recently matched identifier
  int continue_identifier_search(const char* identifier_string);
  //! See if the given ID refers to a valid identifier
  VerdeBoolean is_identifier_valid(int identifier_id)
    { return is_keyword_valid(identifier_id); }

  //! get the string for keyword 'keyword_id'
  const char* keyword_string(int keyword_id);
  /*! get the string for identifier 'identifier_id',
      where identifier_id is keyword*maxIdentifiers + identifierID */
  const char* identifier_string(int identifier_id);
  
 //! Get/set whether to quit and/or pause when you hit a parsing error.
  void quit_on_error(VerdeBoolean quit);
  VerdeBoolean quit_on_error();
  void pause_on_error(VerdeBoolean pause);
  VerdeBoolean pause_on_error();
  
  //! get/set whether an error has occurred.
  VerdeBoolean parse_error_occurred();
  void signal_parse_error();
  
  unsigned int number_keywords();
  
  //! Call the command handler using parsed_data.
  void call_keyword_handler(FunctionData* parsed_data );
  //! Make sure parsed_data is big enough to hold req_size entries
  void grow_data_buffers(FunctionData* parsed_data,
                         int req_size);
  ~IDR();
  
private:
  
  IDR();
  
  void identifier_insufficient_specification_error(
    int new_match_begin, int new_match_end, const char* id_string);
  void keyword_insufficient_specification_error(
    int new_match_begin, int new_match_end, const char* id_string);
  
  static IDR* _instance;
  
  unsigned char quitOnError : 1;
  unsigned char pauseOnError : 1;
  unsigned char parseError : 1;
  unsigned char keywordFinished : 1;
  unsigned char identifierFinished : 1;
  
  int lastMatchedKeyword;
  int numKeywords;
  int maxIdentifiers;
  
  int kMatchBegin, kMatchEnd;
  int kMatchWord;
  
  int iMatchBegin, iMatchEnd;
  int iMatchWord;

  int* wordsInKeyword;
  int* identifiersPerKeyword;
  int** wordsInIdentifier;
  
  KeywordHandler* idrKeywordTable;


  
};

inline unsigned int IDR::number_keywords()
{ return numKeywords; }

inline VerdeBoolean IDR::keyword_search_complete()
{ return (VerdeBoolean)keywordFinished; }

inline VerdeBoolean IDR::identifier_search_complete()
{ return (VerdeBoolean)identifierFinished; }

inline void IDR::signal_parse_error()
{ parseError = VERDE_TRUE; }
 
inline VerdeBoolean IDR::parse_error_occurred()
{ return (VerdeBoolean)parseError; }

inline void IDR::quit_on_error( VerdeBoolean quit )
{ quitOnError = quit ? VERDE_TRUE : VERDE_FALSE; }

inline VerdeBoolean IDR::quit_on_error()
{ return (VerdeBoolean)quitOnError; }

inline void IDR::pause_on_error( VerdeBoolean pause )
{ pauseOnError = pause ? VERDE_TRUE : VERDE_FALSE; }

inline VerdeBoolean IDR::pause_on_error()
{ return (VerdeBoolean)pauseOnError; }

#endif
