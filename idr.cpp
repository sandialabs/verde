//********************************************************************/
//- File:        idr.cpp
//-
//- Description: Handles parsing related operations, such as matching
//-              keywords and identifiers, storing parsed data,
//-              and calling appropriate command handlers.
//-
//- Owner:       Darryl Melander
//********************************************************************/


#include "idr.hpp"
#ifdef USING_DFPARSER
#include "DFParser.hpp"
#endif

// structure containing keyword, optional identifiers, and keyword handler
// function pointer
struct KeywordHandler
{
  char*		keyWord;
  char*		keywordIdentifier [IDR::MAX_IDENTIFIERS];
  void		(*keyword_handler_function)( const struct FunctionData* );
};

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "VerdeMessage.hpp"

//#include "ParsingDefines.hpp"
#include "keywordtable.h"

#include "CommandHandler.hpp"

static int compare_strings(const char* c1, const char* c2);
static int words_in_string(const char* string);
extern "C" 
{
  static int compare_keywords(const void* v1, const void* v2);
  static int compare_identifier(const void* v1, const void* v2);
}

extern void aprepro_init();

IDR* IDR::_instance = NULL;

IDR* IDR::instance()
{
  if (!_instance)
    _instance = new IDR;
  return _instance;
}

void IDR::delete_instance()
{
  delete _instance;
  _instance = NULL;
}

IDR::IDR()
    : quitOnError(VERDE_FALSE),
      pauseOnError(VERDE_FALSE),
      parseError(VERDE_FALSE),
      identifierFinished(VERDE_FALSE),
      lastMatchedKeyword(-1),
      numKeywords(0), maxIdentifiers(0),
      kMatchBegin(0), kMatchEnd(0), kMatchWord(0),
      iMatchBegin(0), iMatchEnd(0), iMatchWord(0)
{
    // This holds all keywords and their associated identifiers.
    // Needs to be const so that we can use string
    // literals to construct it.
  struct constKeywordHandler
  {
    const char* keyWord;
    const char* keywordIdentifier[MAX_IDENTIFIERS];
    void (*keyword_handler_function)( const struct FunctionData* );
  };
  
  constKeywordHandler temp_keyword_table[] = 
  {
#include "CommandTable.h"
    { "", {""}, NULL }
  };
  
  int i, j;
  
    // Get the number of keywords so we can allocate some space
    // for idrKeywordTable.
  i = 0;
  while(temp_keyword_table[i].keyWord[0])
    i++;
  idrKeywordTable = new KeywordHandler [i+1];
  
    // Set some variables for word searching
  numKeywords = i;
  kMatchBegin = 0;
  kMatchEnd = numKeywords;
  
    // Now get a pointer to each record in the keyword
    // table for sorting purposes.
  constKeywordHandler** sorting_keyword_table =
    new constKeywordHandler* [numKeywords];
  for (; i--; )
    sorting_keyword_table[i] = &temp_keyword_table[i];
  
    // Sort the keywords alphabetically
  qsort((void*)sorting_keyword_table,
        numKeywords,
        sizeof(KeywordHandler*),
        compare_keywords);
  
    // Allocate some variables to save some counts
  wordsInKeyword        = new int [numKeywords];
  wordsInIdentifier     = new int*[numKeywords];
  identifiersPerKeyword = new int [numKeywords];
  
    // Now copy each record into idrKeywordTable,
    // in the sorted order
  for (i = numKeywords; i--; )
  {
      // copy the keyword
    idrKeywordTable[i].keyWord =
      new char[strlen(sorting_keyword_table[i]->keyWord) + 1];
    strcpy(idrKeywordTable[i].keyWord, sorting_keyword_table[i]->keyWord);
      // Change case
    case_convert(idrKeywordTable[i].keyWord);
      // Find out how many words are in it
    wordsInKeyword[i] = words_in_string(idrKeywordTable[i].keyWord);
    
      // copy the function pointer
    idrKeywordTable[i].keyword_handler_function =
      sorting_keyword_table[i]->keyword_handler_function;
    
      // Now deal with the identifiers in this keyword.
    for (j = 0; sorting_keyword_table[i]->keywordIdentifier[j][0]; j++)
    {
        // Copy identifier
      idrKeywordTable[i].keywordIdentifier[j] =
        new char [strlen(sorting_keyword_table[i]->keywordIdentifier[j]) + 1];
      strcpy(idrKeywordTable[i].keywordIdentifier[j],
             sorting_keyword_table[i]->keywordIdentifier[j]);
        // change case
      case_convert(idrKeywordTable[i].keywordIdentifier[j]);
    }
    
      // Store the number of identifiers
    identifiersPerKeyword[i] = j;
    
      // Sort identifiers
    qsort(idrKeywordTable[i].keywordIdentifier,
          identifiersPerKeyword[i], sizeof(char*),
          compare_identifier);
    
      // Now go back through identifiers and find how many words in each
    wordsInIdentifier[i] = new int[j];
    for (; j--; )
      wordsInIdentifier[i][j] =
        words_in_string(idrKeywordTable[i].keywordIdentifier[j]);
    
      // store the max if needed
    if (identifiersPerKeyword[i] > maxIdentifiers)
      maxIdentifiers = identifiersPerKeyword[i];
  }
    // Set the last entry to have a bunch of NULL pointers.
  idrKeywordTable[numKeywords].keyWord = NULL;
  idrKeywordTable[numKeywords].keywordIdentifier[0] = NULL;
  idrKeywordTable[numKeywords].keyword_handler_function = NULL;
  
    // Free the temporary array
  delete [] sorting_keyword_table;
  
  {
    PRINT_DEBUG(36, 
      "\nKeywords encountered: %d\n"
      "Max identifiers encountered: %d\n\n\n",
      numKeywords, maxIdentifiers );
  }

  aprepro_init();
}

IDR::~IDR()
{
  int i;
  int j;
  
  for (i = numKeywords; i--; )
  {
    delete []idrKeywordTable[i].keyWord;
    
    for (j = identifiersPerKeyword[i]; j--; )
    {
      delete [] idrKeywordTable[i].keywordIdentifier[j];
    }
    
    delete [] wordsInIdentifier[i];
  }
  
  delete [] identifiersPerKeyword;
  delete [] wordsInIdentifier;
  delete [] wordsInKeyword;
  delete [] idrKeywordTable;
  
}

static int compare_strings(const char* c1, const char* c2)
{
    // Make sure they are both valid words
  if (!c2)
  {
    if (c1)
      return -1;
    else
      return 0;
  }
  else if (!c1)
    return 1;
  
  for (;;)  // Until we break...
  {
    if (!*c2)
    {
      if (*c1)
        return 1;
      else
        break;
    }
    if (!*c1)
      return -1;
    if (tolower(*c1) < tolower(*c2))
      return -1;
    if (tolower(*c1) > tolower(*c2))
      return 1;
    c1++;
    c2++;
  }
  return 0;
}

extern "C"
{
static int compare_keywords(const void* v1, const void* v2)
{
    // See which keyword is alphabetically first, case insensitive
  const char* c1 = (*(const KeywordHandler**)v1)->keyWord;
  const char* c2 = (*(const KeywordHandler**)v2)->keyWord;

  int rv = compare_strings(c1, c2);
  
    // If they are equal, warn
  if (rv == 0)
    PRINT_ERROR("Duplicate keyword found: %s.\n",
                ((const KeywordHandler*)v1)->keyWord);
  return rv;
}

static int compare_identifier(const void* v1, const void* v2)
{
  const char* c1 = *((const char**)v1);
  const char* c2 = *((const char**)v2);
  int rv = compare_strings(c1, c2);
  if (rv == 0)
    PRINT_ERROR("Duplicate identifiers found: %s.\n",
                c1);
  return rv;
}
}


static int words_in_string(const char* string)
{
  int num_words = 0;
    // Skip initial white space
  while (*string == ' ' || *string == '\t')
    string++;
  while(*string)
  {
      // We are now at the start of a word
    num_words++;
    
      // Skip over all non-white space
    while (*string != '\0' &&
           *string != ' '  &&
           *string != '\t')
      string++;
      // Skip all white space following a word
    while (*string == ' ' || *string == '\t')
      string++;
  }
  return num_words;
}

int IDR::begin_keyword_search(const char* key_string)
{
    // reset variables
  kMatchBegin = 0;
  kMatchEnd = numKeywords;
  kMatchWord = 0;
  
    // call search function
  return continue_keyword_search(key_string);
}

int IDR::begin_identifier_search(const char* ident_string)
{
#ifdef USING_DFPARSER
    // If the last keyword we matched was a df operator,
    // search for a keyword instead of an identifier
  if (lastMatchedKeyword <= ALTERNATIVE_ID_BASE)
  {
    int rv = begin_keyword_search(ident_string);
    if (rv >= 0)
      rv = ALTERNATIVE_ID_BASE - rv;
    else if (rv <= ALTERNATIVE_ID_BASE)
      rv -= numKeywords;
    else
      return rv;
  }
#endif
  
    // Do a normal identifier search if we didn't have to deal with
    // the DF stuff
  
    // reset variables
  iMatchBegin = 0;
  iMatchEnd = identifiersPerKeyword[lastMatchedKeyword];
  iMatchWord = 0;
  
    // call search function
  return continue_identifier_search(ident_string);
}

void FunctionData::append_to_buffer (Integer count,
                                     Real rdata,
                                     Integer idata,
                                     const char* cdata,
                                     const char* id_string )
{
  grow_data_buffers(count);
  
  integerData[count] = idata;
  realData[count] = rdata;
  
  if( cdata )
    strcpy(characterData[count], cdata);
  else
    characterData[count][0] = '\0';
  
  if( id_string )
    strcpy( keywordIdentifier[count], id_string );
  else
    keywordIdentifier[count][0] = '\0';
}

void FunctionData::grow_data_buffers(unsigned short req_size)
{
  const unsigned short BUFFER_INCREMENT = 5;
  
    // Added so that parsing code can always check for the next keyword
    // I've seen where it checks for the next 3 doubles too so ...
  req_size += 3;
  
  if (req_size >= bufferLength)
  {
    int old_len = bufferLength;
    
    while (req_size >= bufferLength)
      bufferLength += BUFFER_INCREMENT;
    
    PRINT_DEBUG (36, "\ngrew idr data buffers from %d to %d\n\n\n",
                    old_len, bufferLength );

    
    int   *tmp_i = (int*)malloc( bufferLength * sizeof(int) );
    Real  *tmp_r = (Real*)malloc( bufferLength * sizeof(Real) );
    char **tmp_c = (char**)malloc( bufferLength * sizeof(char*) );
    char **tmp_k = (char**)malloc( bufferLength * sizeof(char*) );
    
    int i;
    for( i=0; i < bufferLength; i++ )
      tmp_c[i] = (char*)malloc(IDR::MAX_STRING_LENGTH*sizeof(char));
    
    for( i=0; i < bufferLength; i++ )
      tmp_k[i] = (char*)malloc(IDR::MAX_KEYWORD_STR_LEN*sizeof(char));
    
    for( i=0; i < old_len; i++ )
    {
      tmp_i[i] = integerData[i];
      tmp_r[i] = realData[i];
      
      strncpy(tmp_c[i], characterData[i], IDR::MAX_STRING_LENGTH);
      tmp_c[i][IDR::MAX_STRING_LENGTH - 1] = '\0';
      
      strncpy(tmp_k[i], keywordIdentifier[i],
              IDR::MAX_KEYWORD_STR_LEN);
      tmp_k[i][IDR::MAX_KEYWORD_STR_LEN - 1] = '\0';
    }
    
    for (i = old_len; i < bufferLength; i++)
    {
      tmp_i[i] = 0;
      tmp_r[i] = 0.0;
      tmp_c[i][0] = 0;
      tmp_k[i][0] = 0;
    }
    
    if (integerData)
      free(integerData);
    
    if (realData)
      free(realData);
    
    if (characterData)
    {
      for( i=0; i<old_len; i++ )
        if( characterData[i] )
          free(characterData[i]);
      free(characterData);
    }
    
    if (keywordIdentifier)
    {
      for( i=0; i<old_len; i++ )
        if( keywordIdentifier[i] )
          free(keywordIdentifier[i]);
      free(keywordIdentifier);
    }
    
    integerData       = tmp_i;
    realData          = tmp_r;
    characterData     = tmp_c;
    keywordIdentifier = tmp_k;
  }
}

void FunctionData::clean_out()
{
    // set elements used back to null to avoid future confusion
    // in keyword handler
  for (int i = 0; i < dataListLength; i++)
  {
    integerData[i] = 0;
    realData[i] = 0.0;
    characterData[i][0] = 0;
    keywordIdentifier[i][0] = 0;
  }
}

FunctionData::~FunctionData()
{
  int i;
  
  if (integerData)
    free((char *)integerData);
  
  if (realData)
    free((char *)realData);
  
  if (characterData)
  {
    for( i=0; i<bufferLength; i++ )
      if( characterData[i] )
        free(characterData[i]);
    free((char *)characterData);
  }
  
  if (keywordIdentifier)
  {
    for( i=0; i<bufferLength; i++ )
      if( keywordIdentifier[i] )
        free(keywordIdentifier[i]);
    free((char *)keywordIdentifier);
  }
}

void IDR::case_convert(char* string)
{
  char* ptr = string;

  while( *ptr )
  {
    *ptr = tolower(*ptr);
    ptr++;
  }
}

int IDR::continue_keyword_search(const char* key_string )
{
  char tmp_str[IDR::MAX_KEYWORD_STR_LEN];
  char* tmp_word;
  
  int i, word;
  int new_match_begin = -1, new_match_end = -1;
  keywordFinished = VERDE_TRUE;
  
    // Make a lower-case copy of the input string
  char *keyword_string_2;
  NEWSTR(key_string, keyword_string_2);
  case_convert( keyword_string_2 );
  
#ifdef USING_DFPARSER
    // If we are looking up the first word,
    // lookup the word in the DF tables.
  int df_match = 0;
  int num_df_matches = 0;
  if (kMatchWord == 0)
  {
    num_df_matches = DFParser::instance()->lookup_token(keyword_string_2, &df_match);
  }
#endif
  
    // Loop over each keyword, finding possible matches.
  for( i=kMatchBegin; i<kMatchEnd; i++ )
  {
    strcpy( tmp_str, idrKeywordTable[i].keyWord );
    
    tmp_word = strtok( tmp_str, " \t" );
    
      // If we're matching something other than the first word,
      // move to that word in the string.
    if( kMatchWord )
    {
      word = 0;
      while( (tmp_word=strtok( NULL, " \t" )) )
        if( ++word == kMatchWord )
          break;
    }
    
      // If word #kMatchWord exists in this keyword
      // and it starts with the passed in string...
    if( tmp_word &&
        strstr( tmp_word, keyword_string_2 ) == tmp_word )
    {
        // ...set beginning and end
        // index of possible matches.
      if( new_match_begin == -1 )
        new_match_begin = i;
      new_match_end = i+1;
    }
  }
  
    // If there was at least one match in the IDR tables...
  if( new_match_begin >= 0 )
  {
      // If there was more than one potential match OR
      // there was only one potential match but it still
      // has additional words in it...
    if(new_match_end > new_match_begin+1 ||
       kMatchWord+1 < wordsInKeyword[new_match_begin] )
    {
        // We aren't finished if any of the matches
        // are multiple-word keywords
      for (i=new_match_begin; i<new_match_end; i++)
      {
        if (wordsInKeyword[i] != 1)
        {
          keywordFinished = VERDE_FALSE;
          break;
        }
      }
      
        // If we think we might be finished with a
        // single-word keyword...
      if (keywordFinished && kMatchWord == 0)
      {
          // All possible matches are single-word keywords.
        
          // If we have only one choice,
          // OR
          // we have an exact match in IDR...
#ifdef USING_DFPARSER
        if ((new_match_end == new_match_begin + 1 && num_df_matches == 0) ||
#else
        if (new_match_end == new_match_begin + 1 ||
#endif
            !strcmp(keyword_string_2,
                    idrKeywordTable[new_match_begin].keyWord))
        {
          lastMatchedKeyword = new_match_begin;
        }
#ifdef USING_DFPARSER
          // Or we have an exact match in DF...
        else if (num_df_matches > 0 &&
                 !strcmp(keyword_string_2,
                         DFParser::instance()->get_token_string(df_match)))
        {
          lastMatchedKeyword = ALTERNATIVE_ID_BASE - df_match;
        }
#endif
          // Or we have an ambiguous set of matches...
        else
        {
          keyword_insufficient_specification_error(new_match_begin,
                                                   new_match_end,
                                                   keyword_string_2);
          FREESTR(keyword_string_2);
          return ERROR_INSUFFICIENT_INFO;
        }
      }
        // If we've got at least one potential match
        // that hasn't had all of its words examined...
      else
      {
        
#ifdef USING_DFPARSER
          // see if a DF match would come before the IDR match
        if (num_df_matches > 0 &&
            strcmp(DFParser::instance()->get_token_string(df_match),
                   idrKeywordTable[new_match_begin].keyWord) < 0)
        {
          lastMatchedKeyword = ALTERNATIVE_ID_BASE - df_match;
        }
        else
          lastMatchedKeyword = new_match_begin;
#else
        lastMatchedKeyword = new_match_begin;
#endif
        
        keywordFinished = VERDE_FALSE;
        kMatchBegin = new_match_begin;
        kMatchEnd = new_match_end;
        kMatchWord++;
      }
    }
    else  // If there is one and only one match in the IDR table,
          // and we are examining its last word...
    {
        // Go ahead and save the match.  If DF stuff indicates a
        // different match or ambiguity, the code down below
        // will change lastMatchedKeyword or complain appropriately.
      lastMatchedKeyword = new_match_begin;
      
#ifdef USING_DFPARSER
        // If we're matching the first word and
        // there is a DF match as well...
      if (num_df_matches > 0)
      {
          // If the DF token is an exact match...
        if (!strcmp(keyword_string_2,
                    DFParser::instance()->get_token_string(df_match)))
        {
          lastMatchedKeyword = ALTERNATIVE_ID_BASE - df_match;
        }
          // If neither the DF nor IDR match was exact...
        else if (strcmp(keyword_string_2,
                        idrKeywordTable[new_match_begin].keyWord)
                 != 0)
        {
            // We didn't find an exact match among the possibilities.
          keyword_insufficient_specification_error(new_match_begin,
                                                   new_match_end,
                                                   keyword_string_2);
          keywordFinished = VERDE_FALSE;
          FREESTR(keyword_string_2);
          return ERROR_INSUFFICIENT_INFO;
        }

          // Otherwise, the IDR match was exact, and we already
          // set the variables correctly.
      }
#endif
    }
    
    if (lastMatchedKeyword >= 0)
    {
      PRINT_DEBUG(36, "keyword() found keyword '%s' in IDR tables\n\n",
                    idrKeywordTable[new_match_begin].keyWord);
      iMatchBegin = 0;
      iMatchEnd = identifiersPerKeyword[lastMatchedKeyword];
    }
#ifdef USING_DFPARSER
    else
      PRINT_DEBUG(36, "keyword() found keyword '%s' in DF tables\n\n",
                     DFParser::instance()->get_token_string(df_match));
#endif
    
    FREESTR(keyword_string_2);
    return lastMatchedKeyword;
  }
#ifdef USING_DFPARSER
  else if (num_df_matches > 0)
  {
      // If we didn't find a match, and we are on the first word,
      // we may find something in the DF tables.  We either need
      // an exact match or a single match.
    
      // If there was only one match or there was an exact match, we found it.
    if (num_df_matches == 1 ||
        !strcmp(keyword_string_2, DFParser::instance()->get_token_string(df_match))
       )
    {
      lastMatchedKeyword = ALTERNATIVE_ID_BASE - df_match;
      PRINT_DEBUG(36, "keyword() found keyword '%s' in DF tables\n\n",
                     DFParser::instance()->get_token_string(df_match));
      FREESTR(keyword_string_2);
      return lastMatchedKeyword;
    }
    else
      PRINT_INFO("Ambiguous search results for DF keyword.\n");
  }
#endif
  
    // If we got to this point, we didn't find a match
  FREESTR(keyword_string_2);
  return ERROR_NO_KEYWORD_MATCH;
}

int IDR::continue_identifier_search(const char* identifier_string)
{
  char tmp_str[MAX_KEYWORD_STR_LEN];
  char* tmp_word;
  
  int i, word;
  int new_match_begin = -1, new_match_end = -1;
  int rv = ERROR_NO_KEYWORD_MATCH;
  
    // if no keywords have been matched then exit
  if( lastMatchedKeyword == -1 )
  {
    identifierFinished = VERDE_TRUE;
    return ERROR_INSUFFICIENT_INFO;
  }
  
    // change identifier_string to lower case
  char *identifier_string_2;
  NEWSTR(identifier_string, identifier_string_2);
  case_convert( identifier_string_2 );
  
#ifdef USING_DFPARSER
    // If we are looking up the first word,
    // lookup the word in the DF tables.
  int df_match = 0;
  int num_df_matches = 0;
  if (iMatchWord == 0)
  {
    num_df_matches = DFParser::instance()->lookup_token(identifier_string_2, &df_match);
  }
#endif
  
    // loop over each identifier to find possible matches
    // with identifier_string
  for( i=iMatchBegin; i<iMatchEnd; i++ )
  {
      // initialize tmp_word with first word of the ith keyword identifier
    strcpy( tmp_str,
            idrKeywordTable[lastMatchedKeyword].keywordIdentifier[i]);
    
    if (strcmp("*", tmp_str)==0)
      rv = MATCH_ANYTHING;
    
    tmp_word = strtok(tmp_str, " \t");
    
      // If we're matching something other than the first word,
      // move to that word in the string.
    if( iMatchWord )
    {
      word = 0;
      while( (tmp_word=strtok( NULL, " \t" )) )
        if( ++word == iMatchWord )
          break;
    }
    
      // If word #iMatchWord exists in this identifier
      // and it starts with the passed in string...
    if( tmp_word &&
        strstr( tmp_word, identifier_string_2 ) == tmp_word )
    {
        // ...set beginning and end
        // index of possible matches.
      if( new_match_begin == -1 )
        new_match_begin = i;
      new_match_end = i+1;
    }
  }
  
  identifierFinished = VERDE_TRUE;
  
    // If there was at least one match...
  if( new_match_begin >= 0 )
  {
      // If there was more than one potential match OR
      // there was only one potential match but it still
      // has additional words in it...
    if(new_match_end > new_match_begin+1 ||
       iMatchWord+1 < wordsInIdentifier[lastMatchedKeyword][new_match_begin])
    {
      for (i=new_match_begin; i<new_match_end; i++)
      {
          // We aren't finished if any of the matches
          // are multiple-word keywords
        if (wordsInIdentifier[lastMatchedKeyword][i] != 1)
        {
          identifierFinished = VERDE_FALSE;
          break;
        }
      }
        // If we think we might be finished with a
        // single-word identifier...
      if (identifierFinished && iMatchWord == 0)
      {
          // All possible matches are single-word identifiers.

          // If we only have one choice...
#ifdef USING_DFPARSER
        if ((new_match_end == new_match_begin + 1 && num_df_matches == 0) ||
#else
        if (new_match_end == new_match_begin + 1 ||
#endif
            !strcmp(identifier_string_2,
                    idrKeywordTable[lastMatchedKeyword].keywordIdentifier[new_match_begin]))
        {
          rv = lastMatchedKeyword*maxIdentifiers + new_match_begin;
        }
#ifdef USING_DFPARSER
          // Or we have an exact match in DF...
        else if (num_df_matches > 0 &&
                 !strcmp(identifier_string_2,
                         DFParser::instance()->get_token_string(df_match)))
        {
          rv = ALTERNATIVE_ID_BASE - numKeywords - df_match;
        }
#endif
          // Or we have an ambiguous set of matches...
        else
        {
          identifier_insufficient_specification_error(new_match_begin,
                                                      new_match_end,
                                                      identifier_string_2);
          rv = ERROR_INSUFFICIENT_INFO;
        }
      }
        // If we've got at least one potential match
        // that hasn't had all of its words examined...
      else
      {
#ifdef USING_DFPARSER
          // If the operator matches better than the first identifier,
          // we should return that instead of the identifier ID
        if (num_df_matches > 0 &&
            strcmp(DFParser::instance()->get_token_string(df_match),
                   idrKeywordTable[lastMatchedKeyword].keywordIdentifier[new_match_begin]) < 0)
        {
          rv = ALTERNATIVE_ID_BASE - numKeywords - df_match;
        }
        else
          rv = lastMatchedKeyword*maxIdentifiers + new_match_begin;
#else
        rv = lastMatchedKeyword*maxIdentifiers + new_match_begin;
#endif
        identifierFinished = VERDE_FALSE;
        iMatchBegin = new_match_begin;
        iMatchEnd = new_match_end;
        iMatchWord++;
      }
    }
    else
    {
        // There was only one match, and we've
        // looked at all of its words.
      
        // Go ahead and save the match.  If DF stuff indicates a
        // different match or ambiguity, the code down below
        // will change rv or complain appropriately.
      rv = lastMatchedKeyword*maxIdentifiers + new_match_begin;
      
#ifdef USING_DFPARSER
        // If we're matching the first word and
        // there is a DF match as well...
      if (num_df_matches > 0)
      {
          // If the DF token is an exact match...
        if (!strcmp(identifier_string_2,
                    DFParser::instance()->get_token_string(df_match)))
        {
          rv = ALTERNATIVE_ID_BASE - numKeywords - df_match;
        }
          // If neither the DF nor IDR match was exact...
        else if (strcmp(identifier_string_2,
                        idrKeywordTable[new_match_begin].keyWord)
                 != 0)
        {
            // We didn't find an exact match among the possibilities.
          identifier_insufficient_specification_error(new_match_begin,
                                                      new_match_end,
                                                      identifier_string_2);
          rv = ERROR_INSUFFICIENT_INFO;
        }

          // Otherwise, the IDR match was exact, and we already
          // set the variables correctly.
      }
#endif
    }
    
  }
#ifdef USING_DFPARSER
  else if (num_df_matches > 0)
  {
      // There was no IDR match.  Check DF matches.  If there was only
      // one match, or if there is an exact match, that's what we'll use.
    if (num_df_matches == 1 ||
        !strcmp(identifier_string_2, DFParser::instance()->get_token_string(df_match)))
    {
      rv = ALTERNATIVE_ID_BASE - numKeywords - df_match;
    }
    else
    {
      PRINT_INFO("Ambiguous search results for DF keyword.\n");
      rv = ERROR_INSUFFICIENT_INFO;
    }
  }
#endif

  if (rv >= 0)
	  PRINT_DEBUG(36, "identifier() found identifier '%s' in IDR tables\n\n",
                   idrKeywordTable[new_match_begin].keyWord);
#ifdef USING_DFPARSER
else if (rv <= ALTERNATIVE_ID_BASE){}
    PRINT_DEBUG(36, "identifier() found identifier '%s' in DF tables\n\n",
                   DFParser::instance()->get_token_string(df_match));
#endif
  
  FREESTR(identifier_string_2);
  return rv;
}

void IDR::identifier_insufficient_specification_error(int new_match_begin,
                                                      int new_match_end,
                                                      const char* id_string)
{
  PRINT_INFO("Parser unable to match Keyword Identifier '%s'\n\n", id_string);
  PRINT_INFO("       Possible matches:\n");
  for (int j = new_match_begin; j < new_match_end; j++)
  {
    PRINT_INFO("         '%s'\n",
               idrKeywordTable[lastMatchedKeyword].keywordIdentifier[j]);
  }
  PRINT_INFO("\nAdded input specification is required\n\n");
}

void IDR::keyword_insufficient_specification_error(int new_match_begin,
                                                   int new_match_end,
                                                   const char* id_string)
{
  PRINT_INFO("Parser unable to match Keyword '%s'\n\n",
             id_string);
  PRINT_INFO("       Possible matches:\n");
  for (int j=new_match_begin; j<new_match_end; j++)
  {
    PRINT_INFO("         '%s'\n",
               idrKeywordTable[j].keyWord);
  }
  PRINT_INFO("\nAdded input specification is required\n\n");
}

const char* IDR::keyword_string(int keyword_id)
{
  if (keyword_id >= 0)
      // Normal keyword
    return idrKeywordTable[keyword_id].keyWord;
#ifdef USING_DFPARSER
  else if (keyword_id <= ALTERNATIVE_ID_BASE)
  {
      // DF keyword
    return DFParser::instance()->get_token_string(ALTERNATIVE_ID_BASE - keyword_id);
  }
#endif
  else
    return NULL;
}

const char* IDR::identifier_string(int identifier_id)
{
    // If this is a normal identifier ID...
  if (identifier_id >= 0)
  {
    int keyword;
    int identifier;
    
    keyword = identifier_id / maxIdentifiers;
    identifier = identifier_id % maxIdentifiers;
    
    return idrKeywordTable[keyword].keywordIdentifier[identifier];
  }
#ifdef USING_DFPARSER
  else if (identifier_id <= ALTERNATIVE_ID_BASE)
  {
    identifier_id -= ALTERNATIVE_ID_BASE;
    identifier_id *= -1;

      // If this is a DF identifier
    if (identifier_id >= numKeywords)
      return DFParser::instance()->get_token_string(identifier_id - numKeywords);
    else
        // This is a keyword disguised as an identifier
      return idrKeywordTable[identifier_id].keyWord;
  }
#endif
  else
    return NULL;
}

void IDR::call_keyword_handler(FunctionData* parsed_data )
{
  int keyword_id = parsed_data->keywordId;
  CommandHandler::initialize_echo_stream();
  
#ifdef USING_DFPARSER
  if (keyword_id >= 0)
  {
#endif
    if (DEBUG_FLAG(36))
    {
      PRINT_DEBUG(36,
        "call_keyword_handler() processing command '%s'\n\n",
        idrKeywordTable[ keyword_id ].keyWord );
    }
    
      // If the command isn't "help", call the keyword handler
    if (!CommandHandler::help (parsed_data)) 
      (*(idrKeywordTable[ keyword_id ].keyword_handler_function))( parsed_data );
#ifdef USING_DFPARSER
  }
  else if (keyword_id <= ALTERNATIVE_ID_BASE)
  {
      // Go through each identifier until we find one that is a
      // keyword in disguise.
    int i;
    for (i = 0; i < parsed_data->dataListLength; i++)
    {
      if (parsed_data->keywordIdentifier[i][0] && islower(parsed_data->keywordIdentifier[i][0]))
      {
      }
    }


    
      // Call the generic entity handler.
      // We may want to find a better way to do this, since
      // node <range> move <xyz> isn't handled here.
      // 
      // Maybe we should find the first "real" keyword in parsed_data,
      // and then call its keywords handler.
// Todo: uncomment below
//    AttributeCommands::geom_attribute_handler(parsed_data);
  }
#endif
  
    // Clear out the data
  parsed_data->clean_out();
}

VerdeBoolean IDR::is_keyword_valid(int keyword_id)
{
#ifdef USING_DFPARSER
  if (keyword_id >= 0 || keyword_id <= ALTERNATIVE_ID_BASE)
    return VERDE_TRUE;
  return VERDE_FALSE;
#else
  return (keyword_id >= 0 ? VERDE_TRUE : VERDE_FALSE);
#endif
}
